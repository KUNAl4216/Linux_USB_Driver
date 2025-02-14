#include <linux/module.h>
#include <linux/init.h>
#include <linux/usb.h>


MODULE_LICENSE("GPL");
MODULE_AUTHOR("Kunal");
MODULE_DESCRIPTION("Simplified USB Driver Example");


#define VENDOR_ID  0x0781
#define PRODUCT_ID 0x557d


static struct usb_device_id usb_table[] = {
    { USB_DEVICE(VENDOR_ID, PRODUCT_ID) }, 
    {}                                     
};
MODULE_DEVICE_TABLE(usb, usb_table);


static int usb_probe(struct usb_interface *intf, const struct usb_device_id *id) {
    printk(KERN_INFO "USB device connected (Vendor: 0x%x, Product: 0x%x)\n", 
           id->idVendor, id->idProduct);
    return 0; 
}


static void usb_disconnect(struct usb_interface *intf) {
    printk(KERN_INFO "USB device disconnected\n");
}


static struct usb_driver usb_driver = {
    .name = "simple_usb_driver",   
    .id_table = usb_table,         
    .probe = usb_probe,            
    .disconnect = usb_disconnect,  
};


static int __init usb_driver_init(void) {
    printk(KERN_INFO "USB Driver Loaded\n");
    return usb_register(&usb_driver);  
}


static void __exit usb_driver_exit(void) {
    printk(KERN_INFO "USB Driver Unloaded\n");
    usb_deregister(&usb_driver);  
}

module_init(usb_driver_init);
module_exit(usb_driver_exit);
