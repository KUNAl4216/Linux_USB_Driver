#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/usb.h>
#include <linux/fs.h>

#define DRIVER_NAME "usb_bulk_driver"
#define BUFFER_SIZE 512

static struct usb_device *device;
static struct usb_endpoint_descriptor *bulk_in = NULL;
static struct usb_endpoint_descriptor *bulk_out = NULL;
static unsigned char buffer[BUFFER_SIZE];


static int find_bulk_endpoints(struct usb_interface *interface) {
    struct usb_host_interface *iface_desc;
    struct usb_endpoint_descriptor *endpoint;
    int i;

    iface_desc = interface->cur_altsetting;

    for (i = 0; i < iface_desc->desc.bNumEndpoints; i++) {
        endpoint = &iface_desc->endpoint[i].desc;

        if (!bulk_in && usb_endpoint_is_bulk_in(endpoint)) {
            bulk_in = endpoint;
        }

        if (!bulk_out && usb_endpoint_is_bulk_out(endpoint)) {
            bulk_out = endpoint;
        }
    }

    if (!bulk_in || !bulk_out) {
        printk(KERN_ERR DRIVER_NAME ": Could not find both bulk IN and OUT endpoints.\n");
        return -ENODEV;
    }

    printk(KERN_INFO DRIVER_NAME ": Bulk IN endpoint found at 0x%02x.\n", bulk_in->bEndpointAddress);
    printk(KERN_INFO DRIVER_NAME ": Bulk OUT endpoint found at 0x%02x.\n", bulk_out->bEndpointAddress);

    return 0;
}


static int bulk_write(struct usb_device *udev, unsigned char *data, int size) {
    int retval, actual_length;

    retval = usb_bulk_msg(udev,usb_sndbulkpipe(udev, bulk_out->bEndpointAddress),data,size,&actual_length,5000); // Timeout in milliseconds

    if (retval) {
        printk(KERN_ERR DRIVER_NAME ": Bulk write failed: %d\n", retval);
        return retval;
    }

    printk(KERN_INFO DRIVER_NAME ": Bulk write successful: %d bytes sent.\n", actual_length);
    return 0;
}

static int bulk_read(struct usb_device *udev, unsigned char *buffer, int size) {
    int retval, actual_length;

    retval = usb_bulk_msg(udev,usb_rcvbulkpipe(udev, bulk_in->bEndpointAddress),buffer,size,&actual_length,5000); // Timeout in milliseconds

    if (retval) {
        printk(KERN_ERR DRIVER_NAME ": Bulk read failed: %d\n", retval);
        return retval;
    }

    printk(KERN_INFO DRIVER_NAME ": Bulk read successful: %d bytes received.\n", actual_length);
    return actual_length;
}


static void transfer_data(struct usb_device *udev) {
    int retval;

    
    retval = bulk_read(udev, buffer, BUFFER_SIZE);
    if (retval > 0) {
        printk(KERN_INFO DRIVER_NAME ": Data read: %s\n", buffer);

        
    }

    
    retval = bulk_write(udev, buffer, retval);
    if (retval < 0) {
        printk(KERN_ERR DRIVER_NAME ": Failed to write data back to USB device.\n");
    }
}


static int usb_probe(struct usb_interface *interface, const struct usb_device_id *id) {
    int retval;

    printk(KERN_INFO DRIVER_NAME ": USB device connected.\n");

    device = interface_to_usbdev(interface);

    
    retval = find_bulk_endpoints(interface);
    if (retval) {
        return retval;
    }

    
    transfer_data(device);

    return 0;
}


static void usb_disconnect(struct usb_interface *interface) {
    printk(KERN_INFO DRIVER_NAME ": USB device disconnected.\n");
}


static struct usb_device_id usb_table[] = {
    { USB_INTERFACE_INFO(USB_CLASS_MASS_STORAGE, 0x06, 0x50) },
    {}
};
MODULE_DEVICE_TABLE(usb, usb_table);


static struct usb_driver usb_driver = {
    .name = DRIVER_NAME,
    .id_table = usb_table,
    .probe = usb_probe,
    .disconnect = usb_disconnect,
};


static int __init usb_init(void) {
    int result = usb_register(&usb_driver);
    if (result) {
        printk(KERN_ERR DRIVER_NAME ": USB driver registration failed. Error: %d\n", result);
    } else {
        printk(KERN_INFO DRIVER_NAME ": USB driver registered.\n");
    }
    return result;
}

/
static void __exit usb_exit(void) {
    usb_deregister(&usb_driver);
    printk(KERN_INFO DRIVER_NAME ": USB driver deregistered.\n");
}

module_init(usb_init);
module_exit(usb_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Kunal");
MODULE_DESCRIPTION("USB Bulk Transfer Driver for Pen Drive");
