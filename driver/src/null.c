#include <linux/version.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/types.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <asm/uaccess.h>

#define DRIVER_NAME "driver"
#define START_MINOR 0
#define MINOR_COUNT 2

#define DRIVER_FILE_NAME "myDriver"
#define CLASS_NAME "myDriver_class"

// Prototypes
static int __init mod_init(void);
static void __exit mod_exit(void);
static ssize_t read(struct file *filp, char *buff, size_t count, loff_t *offp);
static ssize_t write(struct file *filp, const char *buff, size_t count, loff_t *offp);
static int open(struct inode *inode, struct file *filp);
static int close(struct inode *inode, struct file *filp);
static int register_driver(void);

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
	return register_driver();
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
	return 0;
}

static ssize_t write(struct file *filp, const char *buff, size_t count, loff_t *offp)
{
	printk("Write syscall for null device.\n");
	printk("Got %d Bytes.\n", count);

	return count;
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

module_init(mod_init);
module_exit(mod_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("JeFa");
MODULE_DESCRIPTION("Modul Template");
MODULE_SUPPORTED_DEVICE("None");
