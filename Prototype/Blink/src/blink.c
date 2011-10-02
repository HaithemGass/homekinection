/**************************************************************************//**
  \file blink.c
  
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
#include <blink.h>

typedef enum
{
	APP_INTIALIZATION,
	APP_NETWORK_WAITING_TO_JOIN,
	APP_NETWORK_JOINED	
} AppStateMachine;

//Global State
static AppStateMachine appState = APP_INTIALIZATION;

//Global NetworkRequest
static ZDO_StartNetworkReq_t startNetworkReq;
static APS_DataReq_t dataReq; // Data transmission request


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

static void blinkTimerFired(void);                          // blinkTimer handler.
void APS_DataIndication (APS_DataInd_t* indData);
static void ZDO_StartNetworkConf(ZDO_StartNetworkConf_t *confirmInfo);



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
			BSP_OnLed(LED_RED);
			BSP_OnLed(LED_YELLOW);
			BSP_OnLed(LED_GREEN);
			
			//DeviceType_t deviceType = DEVICE_TYPE_COORDINATOR;
			DeviceType_t deviceType = DEVICE_TYPE_ROUTER;
			ExtPanId_t panId = 0x0123456789ABCDEF;	
			
			CS_WriteParameter(CS_DEVICE_TYPE_ID,&deviceType);
			
			CS_WriteParameter(CS_EXT_PANID_ID, &panId);
			
			//ExtAddr_t ownExtAddr = 0x123456788754321;
			ExtAddr_t ownExtAddr = 0x0123456789ABCDEF;
			CS_WriteParameter(CS_UID_ID, &ownExtAddr);
			
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
			
			appState = APP_NETWORK_WAITING_TO_JOIN;
			SYS_PostTask(APL_TASK_ID);
		break;
		
		case APP_NETWORK_WAITING_TO_JOIN:
		break;
		
		case APP_NETWORK_JOINED:
                // Start blink timer	
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
		// Configure blink timer		
		greenTimer.interval = result->status;       // Timer interval
		greenTimer.mode     = TIMER_REPEAT_MODE;        // Repeating mode (TIMER_REPEAT_MODE or TIMER_ONE_SHOT_MODE)
		greenTimer.callback = blinkGreen;          // Callback function for timer fire event
		HAL_StartAppTimer(&greenTimer); 						
}
/*******************************************************************************
  Description: blinking timer fire event handler.

  Parameters: none.
  
  Returns: nothing.
*******************************************************************************/
static void blinkTimerFired()
{
  BSP_ToggleLed(LED_RED);
  BSP_ToggleLed(LED_YELLOW);       

  //Assigning payload memory for the APS data request
  appMessageBuffer.data[0]=0;
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
  dataReq.APS_DataConf = NULL;
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
	BSP_ToggleLed(LED_GREEN);
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
			// Configure blink timer
			blinkTimer.interval = 180;       // Timer interval
			blinkTimer.mode     = TIMER_REPEAT_MODE;        // Repeating mode (TIMER_REPEAT_MODE or TIMER_ONE_SHOT_MODE)
			blinkTimer.callback = blinkTimerFired;          // Callback function for timer fire event
			HAL_StartAppTimer(&blinkTimer); 
	}else{
			// Configure blink timer
			blinkTimer.interval = 1000;       // Timer interval
			blinkTimer.mode     = TIMER_REPEAT_MODE;        // Repeating mode (TIMER_REPEAT_MODE or TIMER_ONE_SHOT_MODE)
			blinkTimer.callback = blinkTimerFired;          // Callback function for timer fire event
			HAL_StartAppTimer(&blinkTimer); 
	}
}

/*******************************************************************************
  Description: just a stub.

  Parameters: are not used.
  
  Returns: nothing.
*******************************************************************************/
void ZDO_MgmtNwkUpdateNotf(ZDO_MgmtNwkUpdateNotf_t *nwkParams) 
{
  nwkParams = nwkParams;  // Unused parameter warning prevention
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
