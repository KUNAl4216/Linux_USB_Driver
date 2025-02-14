/*

linux usb Device driver

*/

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/usb.h>

#define USB_VENDOR_ID 0x0781
#define USB_PRODUCT_ID 0x557d

struct usb_class_driver usb_cd;

static struct usb_drv_probe(struct usb_interface *interface, const struct usb_device_id *id)
{
        struct usb_host_interface *inface_desc;
        struct usb_endpoint_descriptor * endpoint;
        int ret;

        intface_desc = interface->cur_altsetting;

        printk(KERN_INFO "USB info %d now probed: (%04X:%04)\n",intface_desc->desc.bInterfacenumber, id->idvendor , id->idproduct);
        printk(KERN_INFO "ID->bNumEndpoints : %02X\n",intface_desc->desc.bNumEndpoints);
        printk(KERN_INFO "ID->bInterfaceclass:%02X\n",intface_desc.desc.bInterfaceclass);

        ret = usb_register_dev(interface,&usb_cd);
        if(ret)
        {
             printk(KERN_INFO "not able to get the minor number../n")

        }
        else
        {
             printk(KERN_INFO"Minor number = %d\n",interface->minor);
        }


}

static void usb_drv_disconnect(struct usb_interface *interface)
{
        printk(KERN_INFO "Disconnected and Release the MINOR number %d/n",interface->minor );
        usb_deregister_dev(interface, &usb_cd);
}

static struct usb_device_id usb_drv_table[] ={
        {
                USB_DEVICE(USB_VENDOR_ID,USB_PRODUCT_ID)
         };
        {}
};
MODULE_DEVICE_TABLE(usb, usb_drv_table);

static struct usb_driver usb_drv_struct = {
        .name          ="Kunal simple USB driver"
        .probe         = usb_drv_probe,
        .disconnect    = usb_drv_disconnect,
        .id_table      = usb_drv_table,
};

static int __init usb_test_init(void)
{
        printk(KERN_INFO "Register the usb driver with the usb subsystem\n");
        usb_register(&usb_drv_struct);
}

static void _exit usb_test_exit(void)
{     printk(KERN_INFO "DeRegister the usb driver with the usb subsystem\n");
      usb_deregister(&usb_drv_struct);
}

module_init(usb_test_init);
module_exit(usb_test_exit);

MODULE_LICENSE("GPL");
MODILE_AUTHOR("KUNAL simple USB driver");
MODULE_DESCRIPITON("USB test Driver");