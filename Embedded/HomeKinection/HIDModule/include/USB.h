/*
 * HIDEnums.h
 *
 * Created: 10/6/2011 3:31:42 PM
 *  Author: Brian Bowman
 */ 


#ifndef USB_H_
#define USB_H_

// Panic_Button_Enum_Data.h
// Enumeration tables for a HID keyboard device
uint8_t DD[]= // DEVICE Descriptor
{0x12, // bLength = 18d
0x01, // bDescriptorType = Device (1)
0x00,0x01, // bcdUSB(L/H) USB spec rev (BCD)
0xFF,0xFF,0xFF, // bDeviceClass, bDeviceSubClass, bDeviceProtocol
0x40, // bMaxPacketSize0 EP0 is 64 bytes
0x6A,0x0B, // idVendor(L/H)--Maxim is 0B6A
0x46,0x53, // idProduct(L/H)--5346
0x34,0x12, // bcdDevice--1234
1,2,3, // iManufacturer, iProduct, iSerialNumber
1}; // bNumConfigurations
uint8_t CD[]= // CONFIGURATION Descriptor
{0x09, // bLength
0x02, // bDescriptorType = Config
0x22,0x00, // wTotalLength(L/H) = 34 bytes
0x01, // bNumInterfaces
0x01, // bConfigValue
0x00, // iConfiguration
0xE0, // bmAttributes. b7=1 b6=self-powered b5=RWU supported
0x01, // MaxPower is 2 ma
// INTERFACE Descriptor
0x09, // length = 9
0x04, // type = IF
0x00, // IF #0
0x00, // bAlternate Setting
0x01, // bNum Endpoints
0x03, // bInterfaceClass = HID
0x00,0x00, // bInterfaceSubClass, bInterfaceProtocol
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
0x83, // bEndpointAddress (EP3-IN)
0x03, // bmAttributes (interrupt)
0x40,0x00, // wMaxPacketSize (64)
0xFF}; // bInterval (poll every 255 msec)
uint8_t RepD[]= // Report descriptor
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
'M',0,
'A',0,
'X',0,
'3',0,
'4',0,
'2',0,
'0',0,
'E',0,
' ',0,
'U',0,
'S',0,
'B',0,
' ',0,
'P',0,
'a',0,
'n',0,
'i',0,
'c',0,
' ',0,
'B',0,
'u',0,
't',0,
't',0,
'o',0,
'n',0};
uint8_t STR3[]= // Serial Number
{24, // bLength
0x03, // bDescriptorType = string
'S',0,
'/',0,
'N',0,
' ',0,
'1',0,
'2',0,
'3',0,
'4',0,
'L',0,
'T',0,
'H',0};

#endif /* HIDENUMS_H_ */