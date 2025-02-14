#include <linux/init.h>       // For init and exit macros
#include <linux/module.h>     // For all kernel modules
#include <linux/kernel.h>     // For kernel log macros


MODULE_LICENSE("GPL");    
MODULE_AUTHOR("Kunal");
MODULE_DESCRIPTION("A simple Linux driver");
MODULE_VERSION("1.0");


static int __init hello_init(void) {
    printk(KERN_INFO "Hello, World! This is a simple Linux driver.\n");
    return 0; 
}


static void __exit hello_exit(void) {
    printk(KERN_INFO "Goodbye, World! Unloading the driver.\n");
}


module_init(hello_init);
module_exit(hello_exit);
