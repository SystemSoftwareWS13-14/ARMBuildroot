#include <linux/version.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/types.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/workqueue.h>

#define DRIVER_NAME "driver"
#define START_MINOR 0
#define MINOR_COUNT 1

#define DRIVER_FILE_NAME "myDriver"
#define CLASS_NAME "myDriver_class"

#define TIMER_MS 2000

typedef struct{
        unsigned long last;
        unsigned long mom;
        unsigned long min;
        unsigned long max;
        unsigned long diff;
} tdiff;

// Prototypes
static int __init mod_init(void);
static void __exit mod_exit(void);
static int register_driver(void);

static int open(struct inode *inode, struct file *filp);
static int close(struct inode *inode, struct file *filp);
static ssize_t read(struct file *filp, char *buff, size_t count, loff_t *offp);
static ssize_t write(struct file *filp, const char *buff, size_t count, loff_t *offp);

static void task(struct work_struct *dummy);
static unsigned long timer_jiffies(void);
static void init_tdiff(tdiff *p_tdiff);
static void catch_time(void);
static void catch_tdiff_normal(tdiff *p_tdiff);
static void catch_tdiff_prec(tdiff *p_tdiff);
static void print_time(void);

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

static DECLARE_DELAYED_WORK(my_work, task);
static struct workqueue_struct *wq = 0;

static tdiff normal_time;
static tdiff prec_time;

// Fops functions
static int __init mod_init(void)
{
	printk("mod_init called\n");
	if (register_driver())
		return -EAGAIN;

	init_tdiff(&normal_time);
        init_tdiff(&prec_time);

	wq = create_workqueue("theWorkQueue");
        if (!wq)
                return -EAGAIN;
        
	if (!queue_delayed_work(wq, &my_work, timer_jiffies())) {
                flush_workqueue(wq);
                destroy_workqueue(wq);
                return -EAGAIN;
        }	
	return 0;
}

static void __exit mod_exit(void)
{
	printk("mod_exit called\n");

	flush_workqueue(wq);
        destroy_workqueue(wq);

	device_destroy(driver_class, dev);
	class_destroy(driver_class);
	cdev_del(char_device);
	unregister_chrdev_region(dev, MINOR_COUNT);

	pr_info("Unregistered driver\n");
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

static void task(struct work_struct *dummy)
{
	catch_time();
        print_time();

        if (!queue_delayed_work(wq, &my_work, timer_jiffies()))
                printk(KERN_ALERT "Failed to queue work!\n");
}

static unsigned long timer_jiffies(void)
{
        return msecs_to_jiffies(TIMER_MS);
}

static void init_tdiff(tdiff *p_tdiff)
{
        p_tdiff->min = ULONG_MAX;
        p_tdiff->max = 0;
        p_tdiff->diff = 0;
        p_tdiff->mom = 0;
        p_tdiff->last = 0;
}

static void catch_time(void)
{
        catch_tdiff_normal(&normal_time);
        catch_tdiff_prec(&prec_time);
}

static void catch_tdiff_normal(tdiff *p_tdiff)
{
        p_tdiff->last = p_tdiff->mom;
        p_tdiff->mom = jiffies;
        p_tdiff->diff = p_tdiff->mom - p_tdiff->last;

        //first timestamp
        if(p_tdiff->diff == p_tdiff->mom)
                return;

        if(p_tdiff->diff > p_tdiff->max)
                p_tdiff->max = p_tdiff->diff;
        if(p_tdiff->diff < p_tdiff->min)
                p_tdiff->min = p_tdiff->diff;
}

static void catch_tdiff_prec(tdiff *p_tdiff)
{
        p_tdiff->last = p_tdiff->mom;
        p_tdiff->mom = jiffies;
        p_tdiff->diff = p_tdiff->mom - p_tdiff->last;

        //first timestamp
        if(p_tdiff->diff == p_tdiff->mom)
                return;

        if(p_tdiff->diff > p_tdiff->max)
                p_tdiff->max = p_tdiff->diff;
        if(p_tdiff->diff < p_tdiff->min)
                p_tdiff->min = p_tdiff->diff;
}

static void print_time(void)
{
        printk("(NORM) Timer fired after %lu jiffies!\n", normal_time.diff);
        printk("(NORM) Current min: %lu jiffies\n", normal_time.min);
        printk("(NORM) Current max: %lu jiffies\n", normal_time.max);
        printk("======================================\n");
        printk("(PREC) Timer fired after %lu cycles!\n", prec_time.diff);
        printk("(PREC) Current min: %lu cycles\n", prec_time.min);
        printk("(PREC) Current max: %lu cycles\n", prec_time.max);
}

module_init(mod_init);
module_exit(mod_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("JeFa");
MODULE_DESCRIPTION("Modul Template");
MODULE_SUPPORTED_DEVICE("None");
