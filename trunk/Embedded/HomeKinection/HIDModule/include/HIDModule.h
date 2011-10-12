/*
 * DimmerModule.h
 *
 * Created: 9/26/2011 4:10:11 PM
 *  Author: Brian Bowman
 */ 


#ifndef HIDMODULE_H_
#define HIDMODULE_H_
/*****************************************************************************
******************************************************************************
*                                                                            *
*                                 DEFINES                                    *
*                                                                            *
*                                                                            *
******************************************************************************
*****************************************************************************/
#define DEVICE_MESSAGE_SUPPORT (DEVICE_MESSAGE_HID | DEVICE_MESSAGE_STATUS)

/******************************************************************************
                    Includes section
******************************************************************************/
#include "sliders.h"
#include "buttons.h"
#include "leds.h"
#include "defines.h"


/*****************************************************************************
******************************************************************************
*                                                                            *
*                           STRUCTS AND ENUMS                                *
*                                                                            *
*                                                                            *
******************************************************************************
*****************************************************************************/

typedef enum{
	USB_STATE_IDLE,
	USB_STATE_RELEASE,
	USB_STATE_WAIT
}USB_STATE;	


/*****************************************************************************
******************************************************************************
*                                                                            *
*                           FUNCTION DECLARATIONS                            *
*                                                                            *
*                                                                            *
******************************************************************************
*****************************************************************************/                  

static void networkStartConfirm(ZDO_StartNetworkConf_t *confirmInfo);
static void networkTransmissionConfirm(APS_DataConf_t *result);

void hidCommandReceived (APS_DataInd_t* indData);
void statusMessageReceived (APS_DataInd_t* indData);

void initializeDevice();
void initializeConfigurationServer();
void initializeSPI();
void initializeUSB();



void resetMAX();
void writeMAXReg(uint8_t addr, uint8_t data);
void writeMAXRegAck(uint8_t addr, uint8_t data);
void writeMAXBytes(uint8_t addr, uint8_t length, uint8_t *message);

uint8_t readMAXReg(uint8_t addr);
uint8_t readMAXRegAck(uint8_t addr);
void readMAXBytes(uint8_t addr, uint8_t length, uint8_t* buffer);

void enableUSBInit();
void USBSendDescriptor();
void USBClassRequest();
void USBVendorRequest();
void USBFeature(bool sc);
void USBGetStatus();
void USBSetInterface();
void USBGetInterface();
void USBSetConfiguration();
void USBGetConfiguration();
void USBStdRequest();
void USBHandleINT3();
void USBHandleSetup();
void USBHandleINT();
void USBHandleTimeOut();

void spiCompleteCallback();
void spiStartTransmission(uint8_t *message, uint16_t length);

void sendStatusPacket(ShortAddr_t addr);

void registerEndpoints();

void testMAXChip();


#endif /* COMMANDMODULE_H_ */


// eof blink.h