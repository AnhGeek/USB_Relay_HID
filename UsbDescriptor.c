#include "UsbDef.h"
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

/* report descriptor */
static uint8_t _CustRepDesc[] =
{
    0x06, 0x00, 0xFF,       // Usage Page (Vendor Defined 0xFF00)
    0x09, 0x01,             // Usage (Vendor Usage 1)
    0xA1, 0x01,             // Collection (Application)
    
    // Input report (optional if device sends data to the host)
    0x09, 0x01,             //   Usage (Vendor Usage 1)
    0x15, 0x00,             //   Logical Minimum (0)
    0x26, 0xFF, 0x00,       //   Logical Maximum (255)
    0x75, 0x08,             //   Report Size (8 bits)
    0x95, 0x01,             //   Report Count (1)
    0x81, 0x02,             //   Input (Data, Var, Abs)
    
    // Output report (to receive an 8-bit command from the host)
    0x09, 0x01,             //   Usage (Vendor Usage 1)
    0x15, 0x00,             //   Logical Minimum (0)
    0x26, 0xFF, 0x00,       //   Logical Maximum (255)
    0x75, 0x08,             //   Report Size (8 bits)
    0x95, 0x01,             //   Report Count (1)
    0x91, 0x02,             //   Output (Data, Var, Abs)

    0xC0                    // End Collection
};

/* report descriptor */
const DescriptorData CustRepDesc = 
{
    _CustRepDesc,

    sizeof(_CustRepDesc)
};

#define CUSTOM_REPORT_DESCR_SIZE  sizeof(_CustRepDesc)
#define TOTAL_CONFIG_DESCR_SIZE     9+9+9+7

/* configuration descriptor */
static uint8_t _CfgDesc[] =
{
    0x09,                 /* bLength: Configuation Descriptor size */
    USB_DESCR_TYP_CONFIG, /* bDescriptorType: Configuration */
    TOTAL_CONFIG_DESCR_SIZE & 0xff, (TOTAL_CONFIG_DESCR_SIZE >> 8) & 0xff,/* wTotalLength: Bytes returned */
    0x01,                 /*bNumInterfaces: 1 interface*/
    0x01,                 /*bConfigurationValue: Configuration value*/
    0x00,                 /*iConfiguration: Index of string descriptor describing
                                     the configuration*/
    0x80,                   /*bmAttributes: host powered*/
    100 / 2,                /*MaxPower 100 mA: this current is used for detecting Vbus*/

    /************** Descriptor of keyboard interface ****************/
    0x09,                 /*bLength: Interface Descriptor size*/
    USB_DESCR_TYP_INTERF, /*bDescriptorType: Interface descriptor type*/
    0x00,                 /*bInterfaceNumber: Number of Interface*/
    0x00,                 /*bAlternateSetting: Alternate setting*/
    0x01,                 /*bNumEndpoints*/
    0x03,                 /*bInterfaceClass: HID*/
    0x00,                 /*bInterfaceSubClass : 1=BOOT, 0=no boot*/
    0x00,                 /*nInterfaceProtocol : 0=none, 1=keyboard, 2=mouse*/
    0x00,                 /*iInterface: Index of string descriptor*/

    /******************** Descriptor of custom HID ********************/
    0x09,                 /*bLength: HID Descriptor size*/
    USB_DESCR_TYP_HID,    /*bDescriptorType: HID*/
    0x11, 0x01,           /*bcdHID: HID Class Spec release number*/
    0x00,                 /*bCountryCode: Hardware target country*/
    0x01,                 /*bNumDescriptors: Number of HID class descriptors to follow*/
    USB_DESCR_TYP_REPORT, /*bDescriptorType*/
    CUSTOM_REPORT_DESCR_SIZE & 0xff, (CUSTOM_REPORT_DESCR_SIZE >> 8) & 0xff, /*wItemLength: Total length of Report descriptor*/

    /******************** Descriptor of endpoint ********************/
    0x07,                 /*bLength: Endpoint Descriptor size*/
    USB_DESCR_TYP_ENDP,   /*bDescriptorType:*/
    0x01,                 /*bEndpointAddress: OUT endpoint 1 (0x01)*/
    0x03,                 /*bmAttributes: Interrupt endpoint*/
    MAX_PACKET_SIZE & 0xff, (MAX_PACKET_SIZE >> 8) & 0xff, /*wMaxPacketSize: 32 Byte max */
    0x0a                 /*bInterval: Polling Interval (10 ms)*/
};

/* configure descriptor */
const DescriptorData CfgDesc = 
{
    _CfgDesc,

    sizeof(_CfgDesc)
};

/* USB Device Qualifier */
static uint8_t _USB_DeviceQualifier[] = 
{
  	10,             	/* bLength */
  	USB_DESCR_TYP_QUALIF,	/* bDescriptorType */

	0x00, 0x02, 		  /*bcdUSB */

  	0x00,                              /* bDeviceClass */
  	0x00,                              /* bDeviceSubClass */
  	0x00,                              /* bDeviceProtocol */
  	
  	DEFAULT_ENDP0_SIZE,                   /* bMaxPacketSize0 */
  	0x00,                              /* bNumOtherSpeedConfigurations */
  	0x00                               /* bReserved */
};

/* device qualifier descriptor */
const DescriptorData DeviceQualifierCfg = 
{
    _USB_DeviceQualifier,

    sizeof(_USB_DeviceQualifier)
};





