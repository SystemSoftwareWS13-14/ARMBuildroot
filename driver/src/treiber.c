#include <linux/version.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/types.h>
#include <linux/cdev.h>
#include <linux/device.h>

#define DRIVER_NAME "treiber"
#define DRIVER_FILE_NAME "treiber0"
#define CLASS_NAME "treiber_class"
#define START_MINOR 0
#define MINOR_COUNT 1

static struct file_operations fops;
static dev_t treiber_dev;
static struct cdev *treiber_object;
static struct class *treiber_class;

static int register_treiber(void);

static int __init mod_init(void)
{
	printk("MODERN mod_init called\n");
	return register_treiber();
	
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
	printk("MODERN mod_exit called\n");

	device_destroy(treiber_class, treiber_dev);
	class_destroy(treiber_class);
	cdev_del(treiber_object);
	unregister_chrdev_region(treiber_dev, MINOR_COUNT);

	pr_info("unregistered driver\n");
}

module_init(mod_init);
module_exit(mod_exit);

MODULE_LICENSE("GPL");

MODULE_AUTHOR("JeFa");
MODULE_DESCRIPTION("Modul Template");
MODULE_SUPPORTED_DEVICE("None");
