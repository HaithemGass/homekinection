/*
 * DimmerModule.h
 *
 * Created: 9/26/2011 4:10:11 PM
 *  Author: Brian Bowman
 */ 


#ifndef IRMODULE_H_
#define IRMODULE_H_
/*****************************************************************************
******************************************************************************
*                                                                            *
*                                 DEFINES                                    *
*                                                                            *
*                                                                            *
******************************************************************************
*****************************************************************************/

#define DEVICE_MESSAGE_SUPPORT (DEVICE_MESSAGE_IR| DEVICE_MESSAGE_STATUS)
//#define MAX_DIMMER_BRIGHTNESS 520
#define PWM_FREQUENCY 104
#define MODULE_TYPE_DIMMER
#define TIMER_UNIT_5 0x120

/******************************************************************************
                    Includes section
*****************************************************************************/
//#include "sliders.h"
#include "buttons.h"
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
void initializeTimer();
void initializeIR();
void registerEndpoints();

void sendIRPacket(ShortAddr_t addr);
void sendStatusPacket(ShortAddr_t addr);

void retryDimmerPacket();
void retryStatusPacket();

void readGreyCode();
void readButton();
void resetPWM();
void onStartRecording(uint8_t button);
void handleIRChange();
void playIR();
void onButtonDown(uint8_t button);



#endif /* DIMMERMODULE_H_ */


// eof blink.h