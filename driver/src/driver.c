#include <linux/init.h>
#include <linux/module.h>
 
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Simple Hello world module.");
MODULE_AUTHOR("JeFa");
 
static int __init ModInit(void)
{
        printk(KERN_ALERT "Hello, world\n");
        return 0;
}
 
static void __exit ModExit(void)
{
        printk(KERN_ALERT "Goodbye, cruel world\n");
}
 
module_init(ModInit);
module_exit(ModExit);
