#include <linux/version.h>
#include <linux/module.h>
#include <linux/fs.h>

#define DRIVER_NAME "treiber"

static int major;
static struct file_operations fops;


static int __init mod_init(void)
{
	printk("mod_init called\n");

	major = register_chrdev(0,DRIVER_NAME,&fops);
	if(major <= 0)
		return -EIO;

	pr_info("Registered driver\n");
	return 0;
}

static void __exit mod_exit(void)
{
	printk("mod_exit called\n");
	unregister_chrdev(major, DRIVER_NAME);
	pr_info("unregistered driver\n");
}

module_init( mod_init );
module_exit( mod_exit );

MODULE_LICENSE("GPL");

MODULE_AUTHOR("JeFa");
MODULE_DESCRIPTION("Modul Template");
MODULE_SUPPORTED_DEVICE("None");
