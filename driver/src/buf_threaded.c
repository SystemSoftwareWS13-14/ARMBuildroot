#include <linux/version.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/types.h>
#include <linux/cdev.h>
#include <linux/wait.h>
#include <linux/device.h>
#include <linux/slab.h>
#include <linux/sched.h>
#include <asm/uaccess.h>
#include <linux/mutex.h>
#include <linux/kthread.h>
#include <linux/random.h>
#include <linux/delay.h>
#include <linux/sched.h>
#include "buffer/fifo_tsafe.h"

#define DRIVER_NAME "myBuffer"
#define START_MINOR 0
#define MINOR_COUNT 1

#define DRIVER_FILE_NAME "myBufferDriver"
#define CLASS_NAME "myDriver_class"

#define DEF_SIZE 32

// Structures
typedef struct {
	char *r_buff; // Read
	const char *w_buff; // Write
	size_t size;
} thread_data_t;
	
// Prototypes
static int __init mod_init(void);
static void __exit mod_exit(void);
static ssize_t read(struct file *filp, char *buff, size_t count, loff_t *offp);
static ssize_t write(struct file *filp, const char *buff, size_t count, loff_t *offp);
static int open(struct inode *inode, struct file *filp);
static int close(struct inode *inode, struct file *filp);

static int register_driver(void);
static int read_thread(void *data);
static int write_thread(void *data);

// File operations
static struct file_operations fops = {
	.read = read,
	.write = write,
	.open = open,
	.release = close
};

// Variables
static dev_t dev;
static struct cdev *char_device;
static struct class *driver_class;

static buffer dev_buf;
static wait_queue_head_t read_wait_queue;
static wait_queue_head_t write_wait_queue;
DEFINE_MUTEX(mutex); // Used for synchronization at wake up.

// Fops functions
static int __init mod_init(void)
{
	printk("mod_init called\n");

	if (register_driver())
		return -EAGAIN;

	dev_buf = buf_get();
	if (!buf_init(&dev_buf, DEF_SIZE))
		return -EAGAIN;

	init_waitqueue_head(&read_wait_queue);
	init_waitqueue_head(&write_wait_queue);

	return 0;
}

static void __exit mod_exit(void)
{
	printk("mod_exit called\n");

	buf_destroy(&dev_buf);

	device_destroy(driver_class, dev);
	class_destroy(driver_class);
	cdev_del(char_device);
	unregister_chrdev_region(dev, MINOR_COUNT);

	pr_info("Unregistered driver\n");
}

static ssize_t read(struct file *filp, char *buff, size_t count, loff_t *offp)
{
	int retval, read;
	thread_data_t data_for_thread;
	struct task_struct *task;

	mutex_lock(&mutex);

	if((filp->f_flags & O_NONBLOCK)) { // Non-blocking read
		pr_info("Buffer->read non-blocking mode\n");
		if (buf_isempty(&dev_buf)) {
			mutex_unlock(&mutex);
			return -EAGAIN;
		}
	} else { // Blocking read
		pr_info("Buffer->read blocking mode\n");
		while (buf_isempty(&dev_buf)) {
			pr_info("Buffer->read is empty, sleeping ...\n");
			mutex_unlock(&mutex);

			//!! Here is a race condition ? No think not.

			retval = wait_event_interruptible(read_wait_queue, !buf_isempty(&dev_buf));
			pr_info("Buffer->read woke up\n");

			if(retval == -ERESTARTSYS)
				return -ERESTARTSYS;
			
			mutex_lock(&mutex);
		}
	}
	
	pr_info("Buffer->read start thread\n");

	// Create thread
	data_for_thread.r_buff = buff;
	data_for_thread.size = count;
	task = kthread_run(read_thread, &data_for_thread, "readthread");
	ssleep(1); //!! Why do I need this sleep?
	read = kthread_stop(task);

	pr_info("Buffer->read stop thread, has read %dBytes\n", read);
	
	mutex_unlock(&mutex);	

	// Wakeup waiting writers , cause we read from buffer.
	wake_up_interruptible(&write_wait_queue);
	
	// Wake up additional waiting readers, maybe we read not all data.
	wake_up_interruptible(&read_wait_queue);

	return read;
}

static ssize_t write(struct file *filp, const char *buff, size_t count, loff_t *offp)
{
	int retval, written;
	thread_data_t data_for_thread;
	struct task_struct *task;

	mutex_lock(&mutex);

	if((filp->f_flags & O_NONBLOCK)) { // Non-blocking write
		pr_info("Buffer->write non-blocking mode\n");
		if (buf_isfull(&dev_buf)) {
			mutex_unlock(&mutex);
			return -EAGAIN;
		}
	} else { // Blocking write
		pr_info("Buffer->write blocking mode\n");
		while (buf_isfull(&dev_buf)) {
			pr_info("Buffer->write is full, sleeping ...\n");
			mutex_unlock(&mutex);
			retval = wait_event_interruptible(write_wait_queue, !buf_isfull(&dev_buf));
			pr_info("Buffer->write woke up\n");

			if(retval == -ERESTARTSYS)
				return -ERESTARTSYS;
			mutex_lock(&mutex);
		}
	}
	
	pr_info("Buffer->write start thread\n");

	data_for_thread.w_buff = buff;
        data_for_thread.size = count;	
	task = kthread_run(write_thread, &data_for_thread, "writethread");
	//!! sched_setscheduler
	ssleep(1); //!! Why do I need this sleep?
	written = kthread_stop(task);
	
	mutex_unlock(&mutex);
	
	pr_info("Buffer->write stop thread, has written %dBytes\n", written);
	
	// Wakeup waiting writers , maybe there is space lef to write into.
        wake_up_interruptible(&write_wait_queue);
	
	// Wake up waiting readers, cause we wrote into buffer
	wake_up_interruptible(&read_wait_queue);

	return written;
}

static int open(struct inode *inode, struct file *filp)
{
	return 0;
}

static int close(struct inode *inode, struct file *filp)
{
	return 0;
}

// Helper functions
static int register_driver(void)
{
	// Get device numbers
	if (alloc_chrdev_region(&dev, START_MINOR, MINOR_COUNT, DRIVER_NAME) < 0)
		return -EIO;

	// Create char device
	char_device = cdev_alloc();
	if (char_device == NULL)
	goto free_dev;

	char_device->owner = THIS_MODULE;
	char_device->ops = &fops;

	// Add device to system
	if (cdev_add(char_device, dev, MINOR_COUNT))
		goto free_object;

	// Add driver to sysfs in class CLASS_NAME
	driver_class = class_create(THIS_MODULE, CLASS_NAME);
	device_create(driver_class, NULL, dev, NULL, "%s", DRIVER_FILE_NAME);

	printk("Major: %d, Minor start: %d, Minor count: %d\n",
		MAJOR(dev), MINOR(dev), MINOR_COUNT);
	pr_info("Registered driver\n");
	return 0;

free_object:
	kobject_put(&char_device->kobj);
free_dev:
	unregister_chrdev_region(dev, MINOR_COUNT);
	return -EIO;
}

static int read_thread(void *data)
{
	thread_data_t *data_for_thread = data;
	char tmp[data_for_thread->size];
	int read, notCopied;
	int sleep_time;

        // Random sleep to simulate hardware
        sleep_time = get_random_int() % 5;
        pr_info("%d: Buffer->readThread sleeping for %ds", current->pid, sleep_time);
        ssleep(sleep_time + 2);
	
	// Read
	pr_info("%d: Buffer->readThread reading from buffer ...\n", current->pid);
	read = buf_read(&dev_buf, tmp, data_for_thread->size);
	pr_info("%d: Buffer->readThread read %dBytes\n", current->pid, read);
	notCopied = copy_to_user(data_for_thread->r_buff, tmp, read);

	set_current_state(TASK_INTERRUPTIBLE);
        while (!kthread_should_stop()) {
            schedule();
            set_current_state(TASK_INTERRUPTIBLE);
        }	

	pr_info("%d: Buffer->readThread stopping\n", current->pid);
	return (read - notCopied);
}

static int write_thread(void *data)
{
	thread_data_t *data_for_thread = data;
	char tmp[data_for_thread->size];
	int notCopied, copied, written = 0;
	int sleep_time;

	// Random sleep to simulate hardware
	sleep_time = get_random_int() % 5;
	pr_info("%d: Buffer->writeThread sleeping for %ds\n", current->pid, sleep_time);
	ssleep(sleep_time + 2);
	
	// Write
	pr_info("%d: Buffer->writeThread writing into buffer ...\n", current->pid);
	notCopied = copy_from_user(tmp, data_for_thread->w_buff, data_for_thread->size);
	copied = data_for_thread->size - notCopied;
	written = buf_write(&dev_buf, tmp, copied);

	set_current_state(TASK_INTERRUPTIBLE);
	while (!kthread_should_stop()) {
	    schedule();
	    set_current_state(TASK_INTERRUPTIBLE);
	}

	pr_info("%d: Buffer->writeThread stopping\n", current->pid);
        return written;
}

module_init(mod_init);
module_exit(mod_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("JeFa");
MODULE_DESCRIPTION("Modul Template");
MODULE_SUPPORTED_DEVICE("None");
