/**************************************************************************//**
  \file DimmerModule.c
  
  \brief Blink application.

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2011, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
    History:     
******************************************************************************/

#include <appTimer.h>
#include <zdo.h>
#include <IRModule.h>
#include <gpio.h>
#include <pwm.h>

typedef enum
{
	APP_INTIALIZATION,
	APP_NETWORK_WAITING_TO_JOIN,
	APP_NETWORK_JOINED,
	APP_NETWORK_SEND_STATUS,
	APP_NETWORK_IDLE	
} AppStateMachine;

//Global State
static AppStateMachine appState = APP_INTIALIZATION;

//Global NetworkRequest
static ZDO_StartNetworkReq_t startNetworkReq;

static APS_DataReq_t dataReq; // Data transmission request

//Global PWM Ch1
HAL_PwmDescriptor_t pwmChannel1;

//Definitions in a global scope
//Application message buffer descriptor
BEGIN_PACK
typedef struct
{
	uint8_t header[APS_ASDU_OFFSET]; // Header
	uint8_t data[1]; // Application data
	uint8_t footer[APS_AFFIX_LENGTH - APS_ASDU_OFFSET]; //Footer
} PACK AppMessageBuffer_t;
END_PACK

static AppMessageBuffer_t appMessageBuffer; // Message buffer

static HAL_AppTimer_t blinkTimer; 
static HAL_AppTimer_t greenTimer;                          // Blink timer.

static int ramp = 1; 
static ShortAddr_t children[CS_MAX_CHILDREN_AMOUNT];
static int c_children = 0;
static bool encoderChannel1 = false;
static bool encoderChannel2 = false;
static void blinkTimerFired(void);                          // blinkTimer handler.
void APS_DataIndication (APS_DataInd_t* indData);
static void ZDO_StartNetworkConf(ZDO_StartNetworkConf_t *confirmInfo);
void ReadGreyCode(uint8_t bn);
void sendStatusPacket();


/*******************************************************************************
  Description: application task handler.

  Parameters: none.
  
  Returns: nothing.
*******************************************************************************/
void APL_TaskHandler(void)
{
	switch(appState)
	{
		case APP_INTIALIZATION:
		
			BSP_OpenLeds(); // Enable LEDs 
			GPIO_2_set();			
			GPIO_1_clr();						

			BSP_OpenButtons(ReadGreyCode, ReadGreyCode);
						
			DeviceType_t deviceType = DEVICE_TYPE_END_DEVICE;
			ExtPanId_t panId = 0x0123456789ABCDEF;	
			
			CS_WriteParameter(CS_DEVICE_TYPE_ID,&deviceType);
			
			CS_WriteParameter(CS_EXT_PANID_ID, &panId);
			
			ExtAddr_t ownExtAddr = 0x123456788754321;
			
			CS_WriteParameter(CS_UID_ID, &ownExtAddr);
			
			appMessageBuffer.data[0]=0;
			
			//setup endpoint
			//Specify endpoint descriptor
			static SimpleDescriptor_t simpleDescriptor = {1, 1, 1, 1, 0, 0, NULL, 0, NULL};
			//variable for registering endpoint
			static APS_RegisterEndpointReq_t endpointParams;
			//Set application endpoint properties
			
			endpointParams.simpleDescriptor = &simpleDescriptor;
			endpointParams.APS_DataInd = APS_DataIndication;
			//Register endpoint
			APS_RegisterEndpointReq(&endpointParams);
			
			//join a network
			startNetworkReq.ZDO_StartNetworkConf = ZDO_StartNetworkConf;
			ZDO_StartNetworkReq(&startNetworkReq);
			
			
			//setup pwm
			HAL_OpenPwm(PWM_UNIT_1);
			
			pwmChannel1.unit = PWM_UNIT_1;
			pwmChannel1.channel  = PWM_CHANNEL_0;
			pwmChannel1.polarity = PWM_POLARITY_INVERTED;
			
			HAL_SetPwmFrequency(PWM_UNIT_1, 32 , PWM_PRESCALER_1024 );
			
			HAL_StartPwm(&pwmChannel1);
			HAL_SetPwmCompareValue(&pwmChannel1, 0);
			appState = APP_NETWORK_WAITING_TO_JOIN;
			SYS_PostTask(APL_TASK_ID);
		break;
		
		case APP_NETWORK_WAITING_TO_JOIN:
		break;
		
		case APP_NETWORK_JOINED:
                // Start blink timer	
		break;
		
		case APP_NETWORK_SEND_STATUS:
			sendStatusPacket();
			appState = APP_NETWORK_IDLE;
			SYS_PostTask(APL_TASK_ID);
		break;
		case APP_NETWORK_IDLE:
		
		break;
		default:
		break;
	}		
  
}
static void blinkGreen()
{
	BSP_ToggleLed(LED_GREEN);   	
}

static void APS_DataConfirm(APS_DataConf_t *result)
{			
		if(result->status == APS_SUCCESS_STATUS){
			// Configure blink timer
			//GPIO_0_toggle();
		}
		else
		{
			// Configure blink timer
			//GPIO_2_set();
			//GPIO_1_clr();
			
		}					
}


void sendStatusPacket()
{
	HAL_SetPwmCompareValue(&pwmChannel1, appMessageBuffer.data[0]);
	dataReq.asdu = appMessageBuffer.data;
	dataReq.asduLength = sizeof(appMessageBuffer.data);
	dataReq.profileId = 1;
	dataReq.dstAddrMode = APS_SHORT_ADDRESS;
	dataReq.dstAddress.shortAddress = CPU_TO_LE16(0);
	dataReq.dstEndpoint = 1;
  
	dataReq.clusterId = CPU_TO_LE16(0);
	dataReq.srcEndpoint = 1;
	dataReq.txOptions.acknowledgedTransmission = 0;
	dataReq.radius = 0x0;
	dataReq.APS_DataConf = APS_DataConfirm;
	APS_DataReq(&dataReq); 
	
}

/*******************************************************************************
  Description: button release event handler.

  Parameters: buttonNumber - released button number.
  
  Returns: nothing.
*******************************************************************************/




/*******************************************************************************
  Description: Callback For Handling Data Frame Reception

  Parameters: are not used.
  
  Returns: nothing.
*******************************************************************************/
void APS_DataIndication (APS_DataInd_t* indData)
{	
	//GPIO_2_clr();
	GPIO_1_clr();
	//GPIO_2_toggle();	
	HAL_SetPwmCompareValue(&pwmChannel1, indData->asdu[0]);
}


/*******************************************************************************
  Description: Callback For Network Joining

  Parameters: are not used.
  
  Returns: nothing.
*******************************************************************************/
static void ZDO_StartNetworkConf(ZDO_StartNetworkConf_t *confirmInfo)
{
	
	if (ZDO_SUCCESS_STATUS == confirmInfo->status) {
		appState = APP_NETWORK_JOINED;
		SYS_PostTask(APL_TASK_ID);
		GPIO_2_clr();
		GPIO_1_set();
		// Configure blink timer
	}else{
		GPIO_0_set();
		GPIO_1_set();
		GPIO_2_set();
	}
}

/*******************************************************************************
  Description: just a stub.

  Parameters: are not used.
  
  Returns: nothing.
*******************************************************************************/
void ZDO_MgmtNwkUpdateNotf(ZDO_MgmtNwkUpdateNotf_t *nwkParams) 
{  
  if(nwkParams->status == ZDO_CHILD_JOINED_STATUS)
  {
	 children[c_children] = nwkParams->childAddr.shortAddr;
	 c_children ++;
	  
  }
}

/*******************************************************************************
  Description: just a stub.

  Parameters: none.
  
  Returns: nothing.
*******************************************************************************/
void ZDO_WakeUpInd(void) 
{
}

#ifdef _BINDING_
/***********************************************************************************
  Stub for ZDO Binding Indication

  Parameters:
    bindInd - indication

  Return:
    none

 ***********************************************************************************/
void ZDO_BindIndication(ZDO_BindInd_t *bindInd) 
{
  (void)bindInd;
}


void ReadGreyCode(uint8_t bn)
{
	bool newEn1, newEn2, sendMessage = false;
	uint8_t bstate = BSP_ReadButtonsState();
	newEn1 = ((bstate & 0b00000001) != 0);
	newEn2 = ((bstate & 0b00000010) != 0);
	
	(newEn2) ? GPIO_1_set() : GPIO_1_clr();
	(newEn1) ? GPIO_2_set() : GPIO_2_clr();
	
	if(newEn1 == encoderChannel1)
	{
		if(newEn2 == encoderChannel2)
		{
			return;
		}
		else
		{
			encoderChannel2 = newEn2;
			if(newEn1 == newEn2)
			{
				if(appMessageBuffer.data[0] < 32)
				{
				  appMessageBuffer.data[0]++;
				  sendMessage = true;
				}						
			}
			else
			{
				if(appMessageBuffer.data[0] > 0)
				{
				  appMessageBuffer.data[0]--;
				  sendMessage = true;
				}
			}
		}
		
	}
	else
	{
		encoderChannel1 = newEn1;
		if(newEn1 == newEn2)
		{
			if(appMessageBuffer.data[0] > 0)
			{
				appMessageBuffer.data[0]--;
				sendMessage = true;				
			}						
		}
		else
		{
			if(appMessageBuffer.data[0] < 32)
			{
				appMessageBuffer.data[0]++;
				sendMessage = true;				
			}
		}
		
	}
	encoderChannel1 = newEn1;
	encoderChannel2 = newEn2;
	if(sendMessage)
	{
		appState = APP_NETWORK_SEND_STATUS;	
		SYS_PostTask(APL_TASK_ID);
	}
}

/***********************************************************************************
  Stub for ZDO Unbinding Indication

  Parameters:
    unbindInd - indication

  Return:
    none

 ***********************************************************************************/
void ZDO_UnbindIndication(ZDO_UnbindInd_t *unbindInd)
{
  (void)unbindInd;
}
#endif //_BINDING_

/**********************************************************************//**
  \brief Main - C program main start function

  \param none
  \return none
**************************************************************************/
int main(void)
{
  SYS_SysInit();

  for(;;)
  {
    SYS_RunTask();
  }
}

//eof blink.c
