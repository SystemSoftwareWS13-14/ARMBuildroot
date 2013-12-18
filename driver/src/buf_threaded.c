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
#include <linux/spinlock.h>
#include <linux/kthread.h>
#include "buffer/fifo.h"

#define DRIVER_NAME "myBuffer"
#define START_MINOR 0
#define MINOR_COUNT 1

#define DRIVER_FILE_NAME "myBufferDriver"
#define CLASS_NAME "myDriver_class"

#define DEF_SIZE 32

typedef struct {
	buffer buf;
	struct spinlock_t buf_lock;

	wait_queue_head_t read_wait_queue;
	wait_queue_head_t write_wait_queue;

	struct task_struct *read_thread_id;
	struct task_struct *write_thread_id;

} synch_buffer;

typedef struct {
	synch_buffer *sbuf;
	char *buff;
	int toCopy;
	int copied;
	
} thread_data;

// Prototypes
static int __init mod_init(void);
static void __exit mod_exit(void);
static ssize_t read(struct file *filp, char *buff, size_t count, loff_t *offp);
static ssize_t write(struct file *filp, const char *buff, size_t count, loff_t *offp);
static int open(struct inode *inode, struct file *filp);
static int close(struct inode *inode, struct file *filp);
static int register_driver(void);

static int synch_buffer_init(synch_buffer *sbuf);
static void synch_buffer_destroy(synch_buffer *sbuf);
static int synch_buffer_isempty(synch_buffer *sbuf);
static int synch_buffer_isfull(synch_buffer *sbuf);
static int thread_read(void *data);
static int thread_write(void *data);

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


// Fops functions
static int __init mod_init(void)
{
	printk("mod_init called\n");

	if (register_driver())
		return -EAGAIN;

	return 0;
}

static void __exit mod_exit(void)
{
	printk("mod_exit called\n");

	device_destroy(driver_class, dev);
	class_destroy(driver_class);
	cdev_del(char_device);
	unregister_chrdev_region(dev, MINOR_COUNT);

	pr_info("Unregistered driver\n");
}

static ssize_t read(struct file *filp, char *buff, size_t count, loff_t *offp)
{
	char tmp[count];
	int read, notCopied, retval;
	thread_data th_dat;
	struct task_struct *thread_id;
	synch_buffer* sbuf;

	sbuf = (synch_buffer*) filp->private_data;
	
	if(!(filp->f_flags & O_NONBLOCK))
	{
		printk(KERN_DEBUG "Buffer->read blocking mode\n");
		retval = wait_event_interruptible(sbuf->read_wait_queue, !synch_buffer_isempty(sbuf));
		printk(KERN_DEBUG "Buffer->read woke up\n");

		if(retval == -ERESTARTSYS)
			return -ERESTARTSYS;
	}	
	
	//prepare data for thread
	th_dat.sbuf = sbuf;
	th_dat.buff = buff;
	th_dat.toCopy = count;

	//irq save spinlock
	spin_lock_irq(&(sbuf->buf_lock));
	
	//start read-thread
	sbuf->read_thread_id = kthread_create(thread_read, &th_dat, "buff_read_thread");
	if(sbuf->read_thread_id == NULL)
		return -EIO;
	wake_up_process(thread_id);
	
	//wait for thread
	
	
	//wakeup waiting writers , cause we read from buffer
	wake_up_interruptible(&write_wait_queue);

	spin_unlock_irq(&(sbuf->buf_lock));

	return th_dat.copied;
}

static ssize_t write(struct file *filp, const char *buff, size_t count, loff_t *offp)
{
	char tmp[count];
	int notCopied, copied, retval;

	if(!(filp->f_flags & O_NONBLOCK))
	{
		printk(KERN_DEBUG "Buffer->write blocking mode\n");
		retval = wait_event_interruptible(write_wait_queue, !buf_isfull(&dev_buf));
		printk(KERN_DEBUG "Buffer->write woke up\n");

		if(retval == -ERESTARTSYS)
			return -ERESTARTSYS;
	}
	
	pr_info("Buffer->writing into buffer...\n");
	notCopied = copy_from_user(tmp, buff, count);
	copied = count - notCopied;
	
	//wake up waiting readers, cause we wrote into buffer
	wake_up_interruptible(&read_wait_queue);
	
	return buf_write(&dev_buf, tmp, copied);
}

static int open(struct inode *inode, struct file *filp)
{
	file->private_data = kmalloc(sizeof(synch_buffer), GFP_KERNEL);
	if(file->private_data == NULL)
		return -EAGAIN;

	if(synch_buf_init( (synch_buffer*) (file->private_data) != 0)
	{
		free(file->private_data);
		return -EAGAIN;
	}

	return 0;
}

static int close(struct inode *inode, struct file *filp)
{
	synch_buffer_destroy((synch_buffer*) (file->private_data));
	free(file->private_data);
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

static int synch_buffer_init(synch_buffer *sbuf)
{
	if (!buf_init(&(sbuf->buf), DEF_SIZE))
		return -EAGAIN;

	spin_lock_init(&(sbuf->buf_lock));
	init_waitqueue_head(&(sbuf->read_wait_queue));
	init_waitqueue_head(&(sbuf->write_wait_queue));
	
	return 0;
}
 
static void synch_buffer_destroy(synch_buffer *sbuf)
{
	buf_destroy(&(sbuf->buf));
}

static int synch_buffer_isempty(synch_buffer *sbuf)
{	
	//laeuft nur im Interrupt kontext (wait queue)
	int retval;
	spin_lock(sbuf->buf_lock);
	retval = buf_isfull(&(sbuf->buf));
	spin_unlock(sbuf->buf_lock);
	return retval;
}

static int synch_buffer_isfull(synch_buffer *sbuf)
{
	//laeuft nur im Interrupt kontext (wait queue)
	int retval;
	spin_lock(sbuf->buf_lock);
	retval = buf_isfull(&(sbuf->buf));
	spin_unlock(sbuf->buf_lock);
	return retval;
}

static int thread_read(void *data)
{
	int read, notCopied;
	thread_data *th_data = (thread_data*) data;
	char tmp[th_data->toCopy];

	pr_info("Buffer->reading from buffer...\n");
	read = buf_read(th_data->fifo_buf, tmp,th_data->toCopy);
	notCopied = copy_to_user(th_data->buff, tmp, read);
	
	th_data->copied = read - notCopied;

	return 0;
}

static int thread_write(void *data)
{

}

module_init(mod_init);
module_exit(mod_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("JeFa");
MODULE_DESCRIPTION("Modul Template");
MODULE_SUPPORTED_DEVICE("None");
