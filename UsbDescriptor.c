#include "UsbDef.h"
#include <stdint.h>
#include "UsbDescriptor.h"

/* device descriptor */
static uint8_t _DevDesc[] = 
{
	0x12,                 /*bLength */
	USB_DESCR_TYP_DEVICE, /*bDescriptorType*/
#if USB_DEVICE_TYPE == LOW_SPEED_DEVICE
	0x10, 0x01,           /*bcdUSB */
#elif USB_DEVICE_TYPE == FULL_SPEED_DEVICE
	0x00, 0x02,           /*bcdUSB */
#else
	#error "usb device type error"
#endif
	0x00,                 /*bDeviceClass*/
	0x00,                 /*bDeviceSubClass*/
	0x00,                 /*bDeviceProtocol*/
	DEFAULT_ENDP0_SIZE,   /*bMaxPacketSize*/
	0x3b, 0x41,           /*idVendor (0x413b)*/
	0x07, 0x21,           /*idProduct = 0x2107*/
	0x00, 0x01,           /*bcdDevice 2.00*/
	0x00,                 /*Index of string descriptor describing
                                                  manufacturer */
	0x00,                 /*Index of string descriptor describing
                                                 product*/
    0x00,                 /*Index of string descriptor describing the
                                                 device serial number */
    0x01                  /*bNumConfigurations*/
};

/* device descriptor */
const DescriptorData DevDesc = 
{
    _DevDesc,

    sizeof(_DevDesc)
};