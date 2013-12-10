#include <linux/version.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/types.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/slab.h>
#include <asm/uaccess.h>

#define DRIVER_NAME "myBuffer"
#define START_MINOR 0
#define MINOR_COUNT 1

#define DRIVER_FILE_NAME "myBufferDriver"
#define CLASS_NAME "myDriver_class"

#define DEF_SIZE 32

typedef struct {
	char * data;
	int index;
	int size;
	int byteCount;
} buffer;

// Prototypes
static int __init mod_init(void);
static void __exit mod_exit(void);
static ssize_t read(struct file *filp, char *buff, size_t count, loff_t *offp);
static ssize_t write(struct file *filp, const char *buff, size_t count, loff_t *offp);
static int open(struct inode *inode, struct file *filp);
static int close(struct inode *inode, struct file *filp);
static int register_driver(void);
static int buf_init(buffer *buf, const int size);
static int buf_read(buffer *buf, int byte, char *out);
static int buf_write(buffer *buf, int byte, char *in);
static int buf_destroy(buffer *buf);

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
static buffer buf;

// Fops functions
static int __init mod_init(void)
{
	printk("mod_init called\n");

	if (register_driver())
		return -EAGAIN;

	if (!buf_init(&buf, DEF_SIZE))
		return -EAGAIN;

	return 0;
}

static void __exit mod_exit(void)
{
	printk("mod_exit called\n");

	buf_destroy(&buf);

	device_destroy(driver_class, dev);
	class_destroy(driver_class);
	cdev_del(char_device);
	unregister_chrdev_region(dev, MINOR_COUNT);

	pr_info("Unregistered driver\n");
}

static ssize_t read(struct file *filp, char *buff, size_t count, loff_t *offp)
{
	char tmp[count];
	int read, notCopied;

	read = buf_read(&buf, count, tmp);
	notCopied = copy_to_user(buff, tmp, read);

	return (read - notCopied);
}

static ssize_t write(struct file *filp, const char *buff, size_t count, loff_t *offp)
{
	char tmp[count];
	int notCopied, copied;

	notCopied = copy_from_user(tmp, buff, count);
	copied = count - notCopied;
	
	return buf_write(&buf, copied, tmp);
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

static int buf_init(buffer *buf, const int size)
{
	buf->data = kmalloc(size, GFP_KERNEL);
	if (buf->data == NULL)
		return 0;

	buf->size = size;
	buf->index = 0;
	buf->byteCount = 0;

	return 1;
}

static int buf_read(buffer *buf, int byte, char *out)
{
	int i;
	int toRead = min(byte, buf->byteCount);

	for (i = 0; i < toRead; ++i) {
		out[i] = buf->data[buf->index];
		buf->index = (buf->index + 1) % buf->size;
	}

	buf->byteCount -= toRead;

	return toRead;
}

static int buf_write(buffer *buf, int byte, char *in)
{
	int i, index_w;
	int toWrite = min(byte, buf->size - buf->byteCount);

	for (i = 0; i < toWrite; ++i) {
		index_w = (buf->index + buf->byteCount) % buf->size;
		buf->data[index_w] = in[i];
		++buf->byteCount;
	}

	return toWrite;
}

static int buf_destroy(buffer *buf)
{
	if (buf->data == NULL)
		return 0;

	kfree(buf->data);
	buf->size = -1;
	buf->index = -1;
	buf->byteCount = -1;

	return 1;
}

module_init(mod_init);
module_exit(mod_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("JeFa");
MODULE_DESCRIPTION("Modul Template");
MODULE_SUPPORTED_DEVICE("None");
