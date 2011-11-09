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
#include <ShadeModule.h>
#include <gpio.h>
#include <pwm.h>
#include <halPwm.h>


/*****************************************************************************
******************************************************************************
*                                                                            *
*                           GLOBAL VARIALBES                                 *
*                                                                            *
*                                                                            *
******************************************************************************
*****************************************************************************/

//Global State
static AppStateMachine appState = APP_INTIALIZATION;

//Global NetworkRequest
static ZDO_StartNetworkReq_t startNetworkReq;

static APS_DataReq_t packet; // Data transmission request

//Global PWM Ch1
HAL_PwmDescriptor_t pwmChannel1;

static ShadeCommandPacket shadeMessage; // Dimmer Message buffer
static StatusMessagePacket statusMessage; // Dimmer Message buffer

static ShortAddr_t myAddr;

static HAL_AppTimer_t retryTimer;
static HAL_AppTimer_t blindTimer;
static HAL_AppTimer_t fakeMessageTimer;

static bool ableToSend = true;
static ShadeButtonStatus ButtonStatus;

/*****************************************************************************
******************************************************************************
*                                                                            *
*                           FUNCTION IMPL                                    *
*                                                                            *
*                                                                            *
******************************************************************************
*****************************************************************************/



/*******************************************************************************
  Description: application task handler.

  Parameters: none.
  
  Returns: nothing.
*******************************************************************************/
void APL_TaskHandler(void)
{
	switch(appState)
	{
          ///APP_INTIALIZATION
		case APP_INTIALIZATION:		
			initializeDevice();
			appState = APP_NETWORK_WAITING_TO_JOIN;			
		break;
		
          ///APP_INTIALIZATION
		case APP_NETWORK_WAITING_TO_JOIN:
		break;
		
          ///APP_NETWORK_JOINED
		case APP_NETWORK_JOINED:	
		break;
		
                    ///APP_NETWORK_SEND_STATUS
		case APP_NETWORK_SEND_STATUS:
		     
			 if(ableToSend)
			 {		        
			   sendStatusPacket(CPU_TO_LE16(0));	 				 
			 }else{
			   HAL_StopAppTimer(&retryTimer);
			   retryTimer.callback = retryStatusPacket;
			   retryTimer.interval = 10;
			   retryTimer.mode = TIMER_ONE_SHOT_MODE;
			   HAL_StartAppTimer(&retryTimer);
			 }
		
			appState = APP_NETWORK_IDLE;			
		break;
          
          ///APP_NETWORK_SEND_STATUS
		case APP_NETWORK_SEND_DIMMER:
                if(ableToSend)
			 {
		       
			   sendStatusPacket(CPU_TO_LE16(0));	 				 
			 }else{
			   HAL_StopAppTimer(&retryTimer);
			   retryTimer.callback = retryShadePacket;
			   retryTimer.interval = 10;
			   retryTimer.mode = TIMER_ONE_SHOT_MODE;
			   HAL_StartAppTimer(&retryTimer);
			 }
			appState = APP_NETWORK_IDLE;			
		break;
          
          ///APP_NETWORK_IDLE
		case APP_NETWORK_IDLE:		
		break;
          
		default:
		break;
	}		
  
}

void sendStatusPacket(ShortAddr_t addr)
{	
     
			
     statusMessage.data.deviceType = SHADE_MODULE;
     statusMessage.data.statusMessageType = 0x0000;
     statusMessage.data.shortAddress = myAddr ;  
	 
	    
	 
	stuffStatusPacket((uint8_t*) &ButtonStatus, sizeof(ButtonStatus), &statusMessage)	 ;
     
	packet.asdu = (uint8_t *)(&statusMessage.data);
	packet.asduLength = sizeof(statusMessage.data);
	packet.profileId = 1;
	packet.dstAddrMode = APS_SHORT_ADDRESS;
	packet.dstAddress.shortAddress = addr;
	
	packet.srcEndpoint = shadeEndpoint.endpoint;
	packet.dstEndpoint = statusEndpoint.endpoint;
  
	packet.clusterId = CPU_TO_LE16(0);	
	packet.txOptions.acknowledgedTransmission = 0;
	packet.radius = 0x0;
	packet.APS_DataConf = networkTransmissionConfirm;
	APS_DataReq(&packet);
	ableToSend = false; 	
	
}

void retryStatusPacket()
{
	appState = APP_NETWORK_SEND_STATUS;
	SYS_PostTask(APL_TASK_ID);	
}

void retryShadePacket()
{
	appState = APP_NETWORK_SEND_DIMMER;
	SYS_PostTask(APL_TASK_ID);	
}

/*******************************************************************************
  Description: Callback For Handling Data Frame Reception

  Parameters: are not used.
  
  Returns: nothing.
*******************************************************************************/
void statusMessageReceived(APS_DataInd_t* indData)
{	
     indData = indData;       	
}


/*******************************************************************************
  Description: Callback For Handling Data Frame Reception

  Parameters: are not used.
  
  Returns: nothing.
*******************************************************************************/
void shadeCommandReceived(APS_DataInd_t* indData)
{		
	ShadeCommandData *data = (ShadeCommandData *)(indData->asdu);
     blindTimer.interval=data->Duration;
     HAL_StartAppTimer(&blindTimer);
   
   
   
   if( data->ButtonMask == SHADE_DIRECTION_DOWN)// & BlindDirection.DOWN)
   {
	setLED(LED_COLOR_CRIMSON);
	GPIO_4_set();
   }
   else if(data->ButtonMask == SHADE_DIRECTION_UP)
   {
	setLED(LED_COLOR_LIME);
	GPIO_3_set();
   }
   else
   {
	setLED(LED_COLOR_WHITE);
	
	GPIO_3_clr();
	GPIO_4_clr();
	   
   }
 
   
}


/*******************************************************************************
  Description: Callback For Sending a transmission

  Parameters: are not used.
  
  Returns: nothing.
*******************************************************************************/
static void networkTransmissionConfirm(APS_DataConf_t *result)
{			
	//Empty Function just to make sure stuff doesn't explode... theoretically we could retry here.			
     result = result;
	GPIO_0_clr();
	ableToSend = true;
}

/*******************************************************************************
  Description: Callback For Network Joining

  Parameters: are not used.
  
  Returns: nothing.
*******************************************************************************/
static void networkStartConfirm(ZDO_StartNetworkConf_t *confirmInfo)
{
	
	if (ZDO_SUCCESS_STATUS == confirmInfo->status) {
          myAddr = confirmInfo->shortAddr;
		appState = APP_NETWORK_JOINED;
		SYS_PostTask(APL_TASK_ID);
		setLED(LED_COLOR_GREEN);		
		// Configure blink timer
	}else{
		startLEDBlink(LED_COLOR_CRIMSON,LED_BLINK_MEDIUM);
	}
}

/*******************************************************************************
  Description: just a stub.

  Parameters: are not used.
  
  Returns: nothing.
*******************************************************************************/
void ZDO_MgmtNwkUpdateNotf(ZDO_MgmtNwkUpdateNotf_t *nwkParams) 
{  
     nwkParams = nwkParams;
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
  \brief intializeDecvice - setup all the peripherals we want

  \param none
  \return none
**************************************************************************/
void initializeDevice()
{
	
	GPIO_6_make_in();
	GPIO_7_make_in();	
	
	GPIO_3_make_out();	
	GPIO_4_make_out();	
	
	BSP_OpenButtons(handleButtonPress,handleButtonRelease);
	 
	initializeLED();     		
	setLED(LED_COLOR_RED);
     initializeConfigurationServer();
     			     
     registerEndpoints();                          
			
	//join a network
	startNetworkReq.ZDO_StartNetworkConf = networkStartConfirm;
	ZDO_StartNetworkReq(&startNetworkReq);	     		
	
	
				
			   blindTimer.callback = stopBlindMotion;
			   blindTimer.interval = 10;
			   blindTimer.mode = TIMER_ONE_SHOT_MODE;
			
	fakeMessageTimer.callback = fakeMessage;
	fakeMessageTimer.interval = 6500;
	fakeMessageTimer.mode = TIMER_REPEAT_MODE;
	//HAL_StartAppTimer(&fakeMessageTimer);
}

void fakeMessage()
{
   static bool direction = true;
   blindTimer.interval=6000;
   HAL_StartAppTimer(&blindTimer);
   
   
   
   if(direction)// & BlindDirection.DOWN)
   {
	setLED(LED_COLOR_LILAC);
	GPIO_3_set();
   }
   else   
   {
	setLED(LED_COLOR_LIME);
	GPIO_4_set();
   }
   
   direction = !direction;
	
}

void handleButtonPress(uint8_t button)
{
	
	if(button == BSP_KEY0 )
	{
			GPIO_3_set(); // down
			ButtonStatus.UpButton=true;
	}
	if(button == BSP_KEY1)
	{
		GPIO_4_set(); // up
		ButtonStatus.DownButton=true;
	}
	appState = APP_NETWORK_SEND_STATUS;
	SYS_PostTask(APL_TASK_ID);	
	
}

void handleButtonRelease(uint8_t button)
{
	if(button == BSP_KEY0 )
	{
		GPIO_3_clr();
		ButtonStatus.UpButton=false;
	}
	if(button == BSP_KEY1)
	{
		GPIO_4_clr();	
		ButtonStatus.DownButton=false;	
	}
	appState = APP_NETWORK_SEND_STATUS;
	SYS_PostTask(APL_TASK_ID);	
	
}

void stopBlindMotion()
{
	   setLED(LED_COLOR_OFF);
	   HAL_StopAppTimer(&blindTimer);
	   GPIO_3_clr();
	   GPIO_4_clr();
	
}

void initializeConfigurationServer()
{
     static DeviceType_t deviceType = CS_DEVICE_TYPE;
     static ExtPanId_t panId = CS_EXT_PANID;
     static ExtAddr_t uid = CS_UID;
     
     CS_WriteParameter(CS_DEVICE_TYPE_ID,&deviceType);	          		
	CS_WriteParameter(CS_EXT_PANID_ID, &panId);							
	CS_WriteParameter(CS_UID_ID, &uid);     
}

void registerEndpoints()
{     

	     statusEndpointParams.simpleDescriptor = &statusEndpoint;
	     statusEndpointParams.APS_DataInd = statusMessageReceived;			
	     APS_RegisterEndpointReq(&statusEndpointParams);

          shadeEndpointParams.simpleDescriptor = &shadeEndpoint;
	     shadeEndpointParams.APS_DataInd = shadeCommandReceived;			
	     APS_RegisterEndpointReq(&shadeEndpointParams);                         	         						                                                   
}




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


