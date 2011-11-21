/*
 * IRModule.h
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
#define DEVICE_TYPE IR_MODULE

#define PWM_FREQUENCY 104
#define TIMER_UNIT_5 0x120
#define RECORD_TIMEOUT 0x1388;

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

void irCommandReceived (APS_DataInd_t* indData);
void statusMessageReceived (APS_DataInd_t* indData);

void initializeDevice();
void initializeConfigurationServer();
void initializePWM();
void initializeTimer();
void initializeIR();
void registerEndpoints();

void sendIRPacket(ShortAddr_t addr);
void sendStatusPacket(ShortAddr_t addr);
void sendNetworkPacket(ShortAddr_t addr);

void retryIRPacket();
void retryStatusPacket();
void retryNetwork();

void readGreyCode();
void readButton();
void resetPWM();
void onStartRecording(uint8_t button);
void handleIRChange();
void playIR();
void onButtonDown(uint8_t button);



#endif /* IRMODULE_H_ */


// eof blink.h