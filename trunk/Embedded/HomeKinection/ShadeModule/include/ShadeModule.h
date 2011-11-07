/*
 * DimmerModule.h
 *
 * Created: 9/26/2011 4:10:11 PM
 *  Author: Brian Bowman
 */ 


#ifndef SHADEMODULE_H_
#define SHADEMODULE_H_
/*****************************************************************************
******************************************************************************
*                                                                            *
*                                 DEFINES                                    *
*                                                                            *
*                                                                            *
******************************************************************************
*****************************************************************************/

#define DEVICE_MESSAGE_SUPPORT (DEVICE_MESSAGE_SHADE | DEVICE_MESSAGE_STATUS)
#define MODULE_TYPE_SHADE

/******************************************************************************
                    Includes section
*****************************************************************************/
//#include "sliders.h"
#include "buttons.h"
#include "leds.h"
#include "defines.h"
#include <halIrq.h>
#include "helpers.h"


/*****************************************************************************
******************************************************************************
*                                                                            *
*                           STRUCTS AND ENUMS                                *
*                                                                            *
*                                                                            *
******************************************************************************
*****************************************************************************/

typedef enum
{
	DOWN,
	UP
	
} BlindDirection;


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

void dimmerCommandReceived (APS_DataInd_t* indData);
void statusMessageReceived (APS_DataInd_t* indData);
void retryShadePacket();

void initializeDevice();
void initializeConfigurationServer();
void registerEndpoints();

void sendDimmerPacket(ShortAddr_t addr);
void sendStatusPacket(ShortAddr_t addr);

void retryDimmerPacket();
void retryStatusPacket();

void handleButtonPress(uint8_t button);
void handleButtonRelease(uint8_t buttons);

void stopBlindMotion();

void fakeMessage();


#endif /* DIMMERMODULE_H_ */


// eof blink.h