#include <linux/version.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/types.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/timer.h>

#define DRIVER_NAME "timer"
#define DRIVER_FILE_NAME "myTimer"
#define CLASS_NAME "myDriver_class"
#define START_MINOR 0
#define MINOR_COUNT 1
#define TIMER_MS 2000

typedef struct{
	unsigned long last;
	unsigned long mom;
	unsigned long min;
	unsigned long max;
	unsigned long diff;	
} tdiff;

static struct file_operations fops;
static dev_t treiber_dev;
static struct cdev *treiber_object;
static struct class *treiber_class;
static struct timer_list my_timer;
static tdiff normal_time;
static tdiff prec_time;

//prototypes
static int register_treiber(void);
static void unregister(void);
static void timer_fkt(unsigned long data);
static unsigned long timer_jiffies(void);
static void init_tdiff(tdiff *p_tdiff);
static void catch_time(void);
static void catch_tdiff_normal(tdiff *p_tdiff);
static void catch_tdiff_prec(tdiff *p_tdiff);
static void print_time(void);

static int open(struct inode *inode, struct file *filp);
static int close(struct inode *inode, struct file *filp);
static ssize_t read(struct file *filp, char *buff, size_t count, loff_t *offp);
static ssize_t write(struct file *filp, const char *buff, size_t count, loff_t *offp);

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
	if(register_treiber())
		return -EAGAIN;
	
	init_tdiff(&normal_time);
	init_tdiff(&prec_time);
	setup_timer(&my_timer, timer_fkt, 0L); 	
	if(mod_timer(&my_timer, timer_jiffies()))
	{
		unregister();
		return -EAGAIN;
	}	

	return 0;
}

static int register_treiber(void)
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
	
	del_timer(&my_timer);
	unregister();

	pr_info("unregistered driver\n");
}

static void unregister(void)
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

static void timer_fkt(unsigned long data)
{
	catch_time();
	print_time();

	if(mod_timer(&my_timer, timer_jiffies()))
		printk(KERN_ALERT "Failed to mod timer!\n");
}

static unsigned long timer_jiffies(void)
{
	return jiffies + msecs_to_jiffies(TIMER_MS);
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
	printk("(PREC) Timer fired after %lu jiffies!\n", prec_time.diff);
	printk("(PREC) Current min: %lu jiffies\n", prec_time.min); 
	printk("(PREC) Current max: %lu jiffies\n", prec_time.max);
}

module_init(mod_init);
module_exit(mod_exit);

MODULE_LICENSE("GPL");

MODULE_AUTHOR("JeFa");
MODULE_DESCRIPTION("Modul Template");
MODULE_SUPPORTED_DEVICE("None");
