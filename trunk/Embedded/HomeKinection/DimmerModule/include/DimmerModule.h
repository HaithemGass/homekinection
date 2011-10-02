/*
 * DimmerModule.h
 *
 * Created: 9/26/2011 4:10:11 PM
 *  Author: Brian Bowman
 */ 


#ifndef DIMMERMODULE_H_
#define DIMMERMODULE_H_
/*****************************************************************************
******************************************************************************
*                                                                            *
*                                 DEFINES                                    *
*                                                                            *
*                                                                            *
******************************************************************************
*****************************************************************************/

#define DEVICE_MESSAGE_SUPPORT (DEVICE_MESSAGE_DIMMER | DEVICE_MESSAGE_STATUS)
#define MAX_DIMMER_BRIGHTNESS 520
//#define MAX_DIMMER_BRIGHTNESS 600
#define MODULE_TYPE_DIMMER

/******************************************************************************
                    Includes section
*****************************************************************************/
//#include "sliders.h"
//#include "buttons.h"
#include "leds.h"
#include "defines.h"
#include <halIrq.h>


/*****************************************************************************
******************************************************************************
*                                                                            *
*                           STRUCTS AND ENUMS                                *
*                                                                            *
*                                                                            *
******************************************************************************
*****************************************************************************/

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

void initializeDevice();
void initializeConfigurationServer();
void initializePWM();
void initializeRotaryEncoder();
void initializeZeroDetect();
void registerEndpoints();

void sendDimmerPacket(ShortAddr_t addr);
void sendStatusPacket(ShortAddr_t addr);

void retryDimmerPacket();
void retryStatusPacket();

void readGreyCode();
void readButton();
void resetPWM();


#endif /* DIMMERMODULE_H_ */


// eof blink.h