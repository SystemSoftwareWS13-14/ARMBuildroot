#include <linux/version.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/types.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/sched.h>
#include <linux/semaphore.h>
#include <linux/delay.h>

#define DRIVER_NAME "lockDriver"
#define START_MINOR 0
#define MINOR_COUNT 1

#define DRIVER_FILE_NAME "myLockDriver"
#define CLASS_NAME "myDriver_class"

#define DELAY 3*HZ
#define DELAY_MS 200

// Prototypes
static int __init mod_init(void);
static void __exit mod_exit(void);
static int register_driver(void);

static int open(struct inode *inode, struct file *filp);
static int close(struct inode *inode, struct file *filp);
static ssize_t read(struct file *filp, char *buff, size_t count, loff_t *offp);
static ssize_t write(struct file *filp, const char *buff, size_t count, loff_t *offp);

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

static DEFINE_SEMAPHORE(sem);

// Fops functions
static int __init mod_init(void)
{
	int ret;

	printk("mod_init called\n");

	ret = register_driver();
	if (ret != 0)
		return ret;

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

static int open(struct inode *inode, struct file *filp)
{
	int ret;

	while (1) {
		ret = down_trylock(&sem);

		if (ret != 0) {
			printk("Lock: Not available, waiting ...\n");
			msleep(DELAY_MS);
		} else
			break;
	}

	printk("Lock: Acquired\n");	

	// Critical section
	set_current_state(TASK_INTERRUPTIBLE);
	schedule_timeout(DELAY); // Alternative is ssleep(seconds)
	printk("Opened driver after delay of %d jiffies.\n", DELAY);

	printk("Lock: Releasing ...\n");
	up(&sem);

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

module_init(mod_init);
module_exit(mod_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("JeFa");
MODULE_DESCRIPTION("Modul Template");
MODULE_SUPPORTED_DEVICE("None");
