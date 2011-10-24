/*
 * HIDEnums.h
 *
 * Created: 10/6/2011 3:31:42 PM
 *  Author: Brian Bowman
 */ 


#ifndef USB_H_
#define USB_H_

// USB.h
// Enumeration tables for a HID keyboard/mouse device
uint8_t DD[]= // DEVICE Descriptor
{0x12, // bLength = 18d
0x01, // bDescriptorType = Device (1)
0x00,0x02, // bcdUSB(L/H) USB spec rev (BCD)
0x00,0x00,0x00, // bDeviceClass, bDeviceSubClass, bDeviceProtocol
0x40, // bMaxPacketSize0 EP0 is 64 bytes
0x6A,0x0B, // idVendor(L/H)--Maxim is 0B6A
0x13,0x37, // idProduct(L/H)--5346
0x02,0x00, // bcdDevice--1234
1,2,3, // iManufacturer, iProduct, iSerialNumber
1}; // bNumConfigurations

uint8_t CD[]= // CONFIGURATION Descriptor
{0x09, // bLength
0x02, // bDescriptorType = Config
59,0x00, // wTotalLength(L/H) = 59 bytes
0x02, // bNumInterfaces
0x01, // bConfigValue
0x00, // iConfiguration
0xC0, // bmAttributes. b7=1 b6=self-powered b5=RWU supported
50, // MaxPower is 2 ma

//KEYBOARD


// INTERFACE Descriptor
0x09, // length = 9
0x04, // type = IF
0x00, // IF #0
0x00, // bAlternate Setting
0x01, // bNum Endpoints
0x03, // bInterfaceClass = HID
0x01,0x01, // bInterfaceSubClass, bInterfaceProtocol
0x00, // iInterface

// HID Descriptor--It's at CD[18]
0x09, // bLength
0x21, // bDescriptorType = HID
0x10,0x01, // bcdHID(L/H) Rev 1.1
0x00, // bCountryCode (none)
0x01, // bNumDescriptors (one report descriptor)
0x22, // bDescriptorType (report)
43,0x00, // CD[25]: wDescriptorLength(L/H) (report descriptor size is 43 bytes)

// Endpoint Descriptor
0x07, // bLength
0x05, // bDescriptorType (Endpoint)
0x82, // bEndpointAddress (EP3-IN)
0x03, // bmAttributes (interrupt)
0x40,0x00, // wMaxPacketSize (64)
0xFF,

//MOUSE

// INTERFACE Descriptor
0x09, // length = 9
0x04, // type = IF
0x01, // Interface #1
0x00, // bAlternate Setting
0x01, // bNum Endpoints
0x03, // bInterfaceClass = HID
0x01,0x02, // bInterfaceSubClass, bInterfaceProtocol ??
0x00, // iInterface

// HID Descriptor--It's at CD[44]
0x09, // bLength
0x21, // bDescriptorType = HID
0x11,0x01, // bcdHID(L/H) Rev 1.1
0x00, // bCountryCode (none)
0x01, // bNumDescriptors (one report descriptor)
0x22, // bDescriptorType (report)
46,0x00, // CD[51]: wDescriptorLength(L/H) (report descriptor size is 50 bytes)

// Endpoint Descriptor
0x07, // bLength
0x05, // bDescriptorType (Endpoint)
0x83, // bEndpointAddress (EP3-IN)
0x03, // bmAttributes (interrupt)
0x08,0x00, // wMaxPacketSize (64)
0x0A

}; // bInterval (poll every 100 msec)


uint8_t RepD_Keyboard[]= // Report descriptor
{
0x05, // bDescriptorType (report)
0x01, // Usage Page (generic desktop)
0x09,0x06, // Usage
0xA1,0x01, // Collection
0x05,0x07, // Usage Page 7 (Keyboard/Keypad)
0x19,0xE0, // Usage Minimum = 224
0x29,0xE7, // Usage Maximum = 231
0x15,0x00, // Logical Minimum = 0
0x25,0x01, // Logical Maximum = 1
0x75,0x01, // Report Size = 1
0x95,0x08, // Report Count = 8
0x81,0x02, // Input(Data,Variable,Absolute) FIRST byte is key modifier
0x95,0x01, // Report Count = 1
0x75,0x08, // Report Size = 8
0x81,0x01, // Input(Constant) SECOND byte is 00
0x19,0x00, // Usage Minimum = 0
0x29,0x65, // Usage Maximum = 101
0x15,0x00, // Logical Minimum = 0,
0x25,0x65, // Logical Maximum = 101
0x75,0x08, // Report Size = 8
0x95,0x01, // Report Count = 1
0x81,0x00, // Input(Data,Variable,Array) THIRD byte is keystroke
0xC0}; // End Collection

uint8_t RepD_Mouse[46]= // Report descriptor
{
    0x05, 0x01,                    // USAGE_PAGE (Generic Desktop)
    0x09, 0x02,                    // USAGE (Mouse)
    0xa1, 0x01,                    // COLLECTION (Application)
    0x09, 0x01,                    //   USAGE (Pointer)
    0xa1, 0x00,                    //   COLLECTION (Physical)
    0x05, 0x09,                    //     USAGE_PAGE (Button)
    0x19, 0x01,                    //     USAGE_MINIMUM (Button 1)
    0x29, 0x08,                    //     USAGE_MAXIMUM (Button 8)
    0x15, 0x00,                    //     LOGICAL_MINIMUM (0)
    0x25, 0x01,                    //     LOGICAL_MAXIMUM (1)
    0x75, 0x01,                    //     REPORT_SIZE (1)
    0x95, 0x08,                    //     REPORT_COUNT (8)
    0x81, 0x02,                    //     INPUT (Data,Var,Abs)
    0x05, 0x01,                    //     USAGE_PAGE (Generic Desktop)
    0x09, 0x30,                    //     USAGE (X)
    0x09, 0x31,                    //     USAGE (Y)
    0x09, 0x38,                    //     USAGE (Wheel)
    0x15, 0x81,                    //     LOGICAL_MINIMUM (-127)
    0x25, 0x7f,                    //     LOGICAL_MAXIMUM (127)
    0x75, 0x08,                    //     REPORT_SIZE (8)
    0x95, 0x03,                    //     REPORT_COUNT (3)
    0x81, 0x06,                    //     INPUT (Data,Var,Rel)
    0xc0,                          //         END_COLLECTION
    0xc0                           //     END_COLLECTION
};	

uint8_t STR0[]= // Language string
{0x04, // bLength
0x03, // bDescriptorType = string
0x09,0x04}; // wLANGID(L/H)
uint8_t STR1[]= // Manufacturer ID
{12, // bLength
0x03, // bDescriptorType = string
'M',0, // love that Unicode!
'a',0,
'x',0,
'i',0,
'm',0};
uint8_t STR2[]= // Product ID
{52, // bLength
0x03, // bDescriptorType = string
'H',0,
'O',0,
'M',0,
'E',0,
' ',0,
'K',0,
'I',0,
'N',0,
'E',0,
'C',0,
'T',0,
'I',0,
'O',0,
'N',0,
' ',0,
'H',0,
'I',0,
'D',0,
' ',0,
'M',0,
'O',0,
'D',0,
'U',0,
'L',0,
'E',0};
uint8_t STR3[]= // Serial Number
{24, // bLength
0x03, // bDescriptorType = string
'S',0,
'/',0,
'N',0,
' ',0,
'B',0,
'E',0,
'E',0,
'F',0,
'L',0,
'T',0,
'H',0};

#endif /* HIDENUMS_H_ */