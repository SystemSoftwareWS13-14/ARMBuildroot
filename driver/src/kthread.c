#include <linux/version.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/types.h>
#include <linux/cdev.h>
#include <linux/device.h>

#include <linux/sched.h>
#include <linux/kthread.h>
#include <linux/delay.h>

#define DRIVER_NAME "kthread"
#define DRIVER_FILE_NAME "mykthread"
#define CLASS_NAME "myDriver_class"
#define START_MINOR 0
#define MINOR_COUNT 1

static struct file_operations fops;
static dev_t treiber_dev;
static struct cdev *treiber_object;
static struct class *treiber_class;
static struct task_struct *thread_id;
static int run_thread;
static DECLARE_COMPLETION( on_exit );

//prototypes
static int register_driver(void);
static void unregister_driver(void);

static int open(struct inode *inode, struct file *filp);
static int close(struct inode *inode, struct file *filp);
static ssize_t read(struct file *filp, char *buff, size_t count, loff_t *offp);
static ssize_t write(struct file *filp, const char *buff, size_t count, loff_t *offp);
static int thread_fkt(void *data);

// File operations
static struct file_operations fops = {
	.write = write,
	.read = read,
	.open = open,
	.release = close
};

static int __init mod_init(void)
{
	printk("mod_init called\n");
	if(register_driver())
		return -EIO;

	thread_id = kthread_create(thread_fkt, NULL, "mykthread");
	if(thread_id == NULL)
		return -EIO;

	run_thread = 1;
	wake_up_process(thread_id);

	return 0;
	
}

static int register_driver(void)
{
	// Reserviert Geraetenummer fuer Treiber und Anzahl minornr
	// dev_t haelt Geraetenummer
	if(alloc_chrdev_region(&treiber_dev, START_MINOR, MINOR_COUNT, DRIVER_NAME) < 0)
		return -EIO;
	
	// Haelt ein Kernelobjekt und verweist auf Modul/Treiber
	// verbindet Kernelobjekt und Treiber
	treiber_object = cdev_alloc();
	if( treiber_object == NULL )
		goto free_treiber_dev;

	treiber_object->owner = THIS_MODULE;
	treiber_object->ops = &fops;

	// Fuegt einen Treiber hinzu / registriert ihn in fops
 	if(cdev_add(treiber_object, treiber_dev, MINOR_COUNT))
		goto free_object;

	// An Geraeteklasse anmelden -> meldet sich im Geraetemodell in sysfs an
	// so wird dem hotplugmanager mitgeteilt, dass Geraetedatei angelegt
	// werden soll
	treiber_class = class_create( THIS_MODULE, CLASS_NAME);
	device_create( treiber_class, NULL, treiber_dev, NULL,
		       "%s", DRIVER_FILE_NAME);

	printk("Major: %d\n", MAJOR(treiber_dev));
	pr_info("Registered driver\n");
	return 0;

free_object:
	kobject_put( &treiber_object->kobj );
free_treiber_dev:
	unregister_chrdev_region( treiber_dev, MINOR_COUNT );
	return -EIO;
}

static void __exit mod_exit(void)
{
	printk("mod_exit called\n");

	unregister_driver();

	run_thread = 0;
	kill_pid( task_pid(thread_id), SIGTERM, 1);
	wait_for_completion(&on_exit);

	pr_info("unregistered driver\n");
}

static void unregister_driver(void)
{
	device_destroy(treiber_class, treiber_dev);
	class_destroy(treiber_class);
	cdev_del(treiber_object);
	unregister_chrdev_region(treiber_dev, MINOR_COUNT);
}

static int open(struct inode *inode, struct file *filp)
{
	return 0;
}

static int close(struct inode *inode, struct file *filp)
{
	return 0;
}

static ssize_t read(struct file *filp, char *buff, size_t count, loff_t *offp)
{
	return 0;
}

static ssize_t write(struct file *filp, const char *buff, size_t count, loff_t *offp)
{
	return 0;
}

static int thread_fkt(void *data)
{
	while(run_thread)
	{
		pr_info("Thread calls!\n");
		ssleep(2);
	}
	
	complete_and_exit(&on_exit, 0);
}

module_init(mod_init);
module_exit(mod_exit);

MODULE_LICENSE("GPL");

MODULE_AUTHOR("JeFa");
MODULE_DESCRIPTION("Modul Template");
MODULE_SUPPORTED_DEVICE("None");
