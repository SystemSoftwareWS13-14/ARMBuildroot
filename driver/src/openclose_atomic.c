#include <linux/version.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/types.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/atomic.h>

#define DRIVER_NAME "openclose_atomic"
#define DRIVER_FILE_NAME "openclose_atomic"
#define CLASS_NAME "openclose__atomic_class"
#define START_MINOR 0
#define MINOR_COUNT 1

static dev_t treiber_dev;
static struct cdev *treiber_object;
static struct class *treiber_class;
static atomic_t open_count;

static int register_treiber(void);
static int driver_open( struct inode *devfile,
			struct file *instance );
static int driver_close( struct inode *devfile,
			 struct file *instance );

static struct file_operations fops = {
	.read = NULL,
	.write = NULL,
	.open = driver_open,
	.release = driver_close
	};

static int __init mod_init(void)
{
	printk("MODERN mod_init called\n");
	atomic_set(&open_count, -1);
	return register_treiber();
	
}

static int register_treiber(void)
{
	//reserviert gerätenummer für treiber und  anzahl minornr
	//dev_t hält gerätenummer
	if(alloc_chrdev_region(&treiber_dev, START_MINOR, MINOR_COUNT, DRIVER_NAME) < 0)
		return -EIO;
	
	//hält ein kernelobjekt und verweist auf Modul/treiber
	//verbindet kernelobjekt und treiber
	treiber_object = cdev_alloc();
	if( treiber_object == NULL )
		goto free_treiber_dev;

	treiber_object->owner = THIS_MODULE;
	treiber_object->ops = &fops;

	//fügt einen treiber hinzu / registriert ihn in fops
 	if(cdev_add(treiber_object, treiber_dev, MINOR_COUNT))
		goto free_object;

	//
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
	printk("MODERN mod_exit called\n");

	device_destroy(treiber_class, treiber_dev);
	class_destroy(treiber_class);
	cdev_del(treiber_object);
	unregister_chrdev_region( treiber_dev, MINOR_COUNT);

	pr_info("unregistered driver\n");
}

static int driver_open( struct inode *devfile,
			struct file *instance )
{
	if(!atomic_inc_and_test(&open_count))
		return -EAGAIN;

	pr_info("Opened openclose!\n");

	if(atomic_read(&open_count) > 0)
		printk(KERN_ALERT "Race Condition detected!\n");
	return 0;
}

static int driver_close( struct inode *devfile,
			 struct file *instance)
{
	atomic_set(&open_count, -1);
	pr_info("Closed openclose!\n");
	return 0;
}

module_init( mod_init );
module_exit( mod_exit );

MODULE_LICENSE("GPL");

MODULE_AUTHOR("JeFa");
MODULE_DESCRIPTION("Modul Template");
MODULE_SUPPORTED_DEVICE("None");
