#include <linux/version.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/types.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/slab.h>
#include <asm/uaccess.h>

#define DRIVER_NAME "hello2"
#define START_MINOR 0
#define MINOR_COUNT 1
#define DRIVER_FILE_NAME "hello2"
#define CLASS_NAME "myDriver_class"
#define OUTPUT "Hello world!\n"

typedef struct {
	int count;
}rcount;

// Prototypes
static int __init mod_init(void);
static void __exit mod_exit(void);
static ssize_t read(struct file *filp, char *buff, size_t count, loff_t *offp);
static int open(struct inode *inode, struct file *filp);
static int close(struct inode *inode, struct file *filp);
static int register_driver(void);
static int get_count(struct file *filp);
static void add_count(struct file *filp, int add);
static void set_count(struct file *filp, int val);

// File operations
static struct file_operations fops = {
	.read = read,
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
	char output[] = {OUTPUT};
	int toCopy, notCopied, copied;
	size_t remaining;

	remaining = (strlen(output) + 1) - get_count(filp);
	toCopy = min(remaining, count);
	notCopied = copy_to_user(buff, output, toCopy);
	
	copied = toCopy - notCopied;	

	add_count(filp, copied);

	return copied;
}

static int open(struct inode *inode, struct file *filp)
{
	filp->private_data = kmalloc(sizeof(rcount), GFP_KERNEL);
	if(filp->private_data == NULL)
		return -EAGAIN;

	set_count(filp, 0);
	return 0;
}

static int close(struct inode *inode, struct file *filp)
{
	kfree(filp->private_data);
	return 0;
}

static int get_count(struct file *filp)
{
	return ((rcount*)filp->private_data)->count;
}

static void add_count(struct file *filp, int add)
{
	((rcount*)filp->private_data)->count += add;
}

static void set_count(struct file *filp, int val)
{
	((rcount*)filp->private_data)->count = val;
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
