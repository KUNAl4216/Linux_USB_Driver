#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/usb.h>
#include <linux/fs.h>
#include <linux/uaccess.h> 

#define VENDOR_ID  0x0781  
#define PRODUCT_ID 0x557d  


static const struct usb_device_id usb_device_table[] = {
    { USB_DEVICE(VENDOR_ID, PRODUCT_ID) }, 
    {} 
};
MODULE_DEVICE_TABLE(usb, usb_device_table);

#define BUF_SIZE 512

static struct usb_device *usb_dev;
static struct usb_class_driver class;
static unsigned char bulk_buf[BUF_SIZE]; 
static __u8 bulk_in_endpointAddr;
static __u8 bulk_out_endpointAddr;


static int usb_open(struct inode *inode, struct file *file) {
    return 0;
}


static ssize_t usb_read(struct file *file, char __user *buffer, size_t count, loff_t *ppos) {
    int ret;
    int actual_length;

    
    ret = usb_bulk_msg(usb_dev, usb_rcvbulkpipe(usb_dev, bulk_in_endpointAddr),bulk_buf, min(BUF_SIZE, count), &actual_length, 5000);
    if (ret) {
        printk(KERN_ERR "USB bulk read failed (%d)\n", ret);
        return ret;
    }

    
    if (copy_to_user(buffer, bulk_buf, actual_length)) {
        return -EFAULT;
    }

    return actual_length;
}

static ssize_t usb_read(struct file *file, char __user *buffer, size_t count, loff_t *ppos) {
    int ret;
    int actual_length;

    
    ret = usb_bulk_msg(usb_dev, usb_rcvbulkpipe(usb_dev, bulk_in_endpointAddr),
                       bulk_buf, min((size_t)BUF_SIZE, count), &actual_length, 5000);
    if (ret) {
        printk(KERN_ERR "USB bulk read failed (%d)\n", ret);
        return ret;
    }

    
    if (copy_to_user(buffer, bulk_buf, actual_length)) {
        return -EFAULT;
    }

    return actual_length;
}


static int usb_release(struct inode *inode, struct file *file) {
    return 0;
}


static const struct file_operations usb_fops = {
    .owner = THIS_MODULE,
    .open = usb_open,
    .read = usb_read,
    .write = usb_write,
    .release = usb_release,
};

static int usb_probe(struct usb_interface *interface, const struct usb_device_id *id) {
    struct usb_host_interface *iface_desc;
    struct usb_endpoint_descriptor *endpoint;
    int i;

    usb_dev = interface_to_usbdev(interface);
    iface_desc = interface->cur_altsetting;

    printk(KERN_INFO "USB Device (%04X:%04X) plugged\n", id->idVendor, id->idProduct);

    
    for (i = 0; i < iface_desc->desc.bNumEndpoints; i++) {
        endpoint = &iface_desc->endpoint[i].desc;

        if ((endpoint->bEndpointAddress & USB_ENDPOINT_DIR_MASK) == USB_DIR_IN) {
            bulk_in_endpointAddr = endpoint->bEndpointAddress;
        } else {
            bulk_out_endpointAddr = endpoint->bEndpointAddress;
        }
    }

    
    class.name = "usb/usb%d";
    class.fops = &usb_fops;

    if (usb_register_dev(interface, &class) < 0) {
        printk(KERN_ERR "Failed to register USB device\n");
        return -1;
    }

    printk(KERN_INFO "USB device registered successfully\n");
    return 0;
}


static void usb_disconnect(struct usb_interface *interface) {
    usb_deregister_dev(interface, &class);
    printk(KERN_INFO "USB Device removed\n");
}

static struct usb_driver usb_driver = {
    .name = "usb_file_driver",
    .id_table = usb_device_table,
    .probe = usb_probe,
    .disconnect = usb_disconnect,
};


static int __init usb_driver_init(void) {
    return usb_register(&usb_driver);
}


static void __exit usb_driver_exit(void) {
    usb_deregister(&usb_driver);
}

module_init(usb_driver_init);
module_exit(usb_driver_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Kunal");
MODULE_DESCRIPTION("USB Driver for File Copy and Paste");
