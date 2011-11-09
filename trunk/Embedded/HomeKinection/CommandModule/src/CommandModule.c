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
#include <usart.h>
#include <CommandModule.h>
#include <gpio.h>
#include <pwm.h>
#include "helpers.h"


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
HAL_PwmDescriptor_t pwmChannelDimmer;

//Gloabl Usart Channel
HAL_UsartDescriptor_t usartChannel0;

static HAL_AppTimer_t retryTimer;
static HAL_AppTimer_t testTimer;
static HAL_AppTimer_t fakeTimer;

static DimmerCommandPacket dimmerMessage; // Dimmer Message buffer
static ShadeCommandPacket shadeMessage; // shade Message buffer
static IRCommandPacket irMessage; // shade Message buffer
static HIDCommandPacket hidMessage; // shade Message buffer

static bool usartTransmitEnable = true;
static bool ableToSend = true;
static uint8_t txBuffer[500];
static uint8_t rxBuffer[500];
static ShortAddr_t retryAddr;
static ShortAddr_t children[CS_MAX_CHILDREN_AMOUNT];
static ShortAddr_t childToSendTo = 0;
static int c_children = 0;

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
		case APP_NETWORK_SEND_DIMMER:
			sendDimmerPacket(childToSendTo);
			appState = APP_NETWORK_IDLE;			
		break;
		
		          ///APP_NETWORK_SEND_STATUS
		case APP_NETWORK_SEND_IR:
			sendIRPacket(childToSendTo);
			appState = APP_NETWORK_IDLE;			
		break;
		
		case APP_NETWORK_SEND_HID:
			sendHIDPacket(childToSendTo);
			appState = APP_NETWORK_IDLE;			
		break;
		
			          ///APP_NETWORK_SEND_STATUS
		case APP_NETWORK_SEND_SHADE:
			sendShadePacket(childToSendTo);
			appState = APP_NETWORK_IDLE;			
		break;
          
          ///APP_NETWORK_IDLE
		case APP_NETWORK_IDLE:		
		break;
          
		default:
		break;
	}		
  
}

void sendShadePacket(ShortAddr_t addr)
{
	packet.asdu = (uint8_t *)(&shadeMessage.data);
	packet.asduLength = sizeof(shadeMessage.data);
	packet.dstEndpoint = shadeEndpoint.endpoint;
	sendMessageToModule(addr);
}


void sendHIDPacket(ShortAddr_t addr)
{
	packet.asdu = (uint8_t *)(&hidMessage.data);
	packet.asduLength = sizeof(hidMessage.data);
	packet.dstEndpoint = hidEndpoint.endpoint;
	sendMessageToModule(addr);	
}

void sendIRPacket(ShortAddr_t addr)
{
	packet.asdu = (uint8_t *)(&irMessage.data);
	packet.asduLength = sizeof(irMessage.data);
	packet.dstEndpoint = irEndpoint.endpoint;
	sendMessageToModule(addr);	
}

void sendDimmerPacket(ShortAddr_t addr)
{	
	packet.asdu = (uint8_t *)(&dimmerMessage.data);
	packet.asduLength = sizeof(dimmerMessage.data);
     packet.dstEndpoint = dimmerEndpoint.endpoint;
	sendMessageToModule(addr);
	
}

void sendMessageToModule(ShortAddr_t addr)
{
     if(ableToSend)
	{
		packet.profileId = 1;
	     packet.dstAddrMode = APS_SHORT_ADDRESS;
	     packet.dstAddress.shortAddress = addr;
  
	     packet.clusterId = CPU_TO_LE16(0);
	     packet.srcEndpoint = 1;
	     packet.txOptions.acknowledgedTransmission = 0;
	     packet.radius = 0x0;
	     packet.APS_DataConf = networkTransmissionConfirm;
		ableToSend = false;
	     APS_DataReq(&packet); 	
	 }	
	 else
	 {
		HAL_StartAppTimer(&retryTimer);		 
		retryAddr = addr;
	 }
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
void statusMessageReceived(APS_DataInd_t* indData)
{	
	//setLED(LED_COLOR_ORANGE);	
     StatusMessageData *data = (StatusMessageData*)(indData->asdu);
     
     //if(data->deviceType == DIMMER_MODULE){
		//uint16_t * p_intensity = (uint16_t * )(data->message);
		//uint16_t intensity = *p_intensity;		
          //HAL_SetPwmCompareValue(&pwmChannelDimmer, 5  * intensity);     
     //}
	
	uint8_t message[] = "STATUS MESSAGE!\r\n";
	 
	HAL_WriteUsart(&usartChannel0, message, sizeof(message));
}

void fakeShadeMessage()
{
	
	if(shadeMessage.data.ButtonMask == SHADE_DIRECTION_DOWN)
	{
		setLED(LED_COLOR_LIME);
		shadeMessage.data.ButtonMask = SHADE_DIRECTION_UP;
	}
	else
	{
		setLED(LED_COLOR_CRIMSON);
		shadeMessage.data.ButtonMask = SHADE_DIRECTION_DOWN;
	}
	shadeMessage.data.Duration = 6000;
	if(c_children > 0){		
	     childToSendTo = children[0];
	     appState = APP_NETWORK_SEND_SHADE;
	     SYS_PostTask(APL_TASK_ID);			
	}
	
}

void sendTestMessage(){
	static uint8_t message[] = "STATUS MESSAGE!";
	static uint8_t nl[] = "\r\n";
	static uint8_t i = 1;
	static bool flip = true;
	if(usartTransmitEnable)
	{
		if(flip)
		{
			setLED(LED_COLOR_BLUE);
	          HAL_WriteUsart(&usartChannel0, message, sizeof(message) - i);	
		     usartTransmitEnable = false;
		     if(i >= (sizeof(message) -1))
		     {
     			i = 1;
		     }
     		else
	     	{
     			i++;
		     }			
			setLED(LED_COLOR_OFF);
		}		
		else
		{	
			setLED(LED_COLOR_BLUE);					
		     HAL_WriteUsart(&usartChannel0, nl, sizeof(nl));	
			setLED(LED_COLOR_OFF);
		}
		flip = !flip;
		
	}
	
	
}

void usartTransmitComplete(){
	usartTransmitEnable = true;
}

/*******************************************************************************
  Description: Callback For Handling Data Frame Reception

  Parameters: are not used.
  
  Returns: nothing.
*******************************************************************************/
void shadeCommandReceived(APS_DataInd_t* indData)
{	
     indData = indData;	
}

/*******************************************************************************
  Description: Callback For Handling Data Frame Reception

  Parameters: are not used.
  
  Returns: nothing.
*******************************************************************************/
void dimmerCommandReceived(APS_DataInd_t* indData)
{		
     indData = indData;
}


/*******************************************************************************
  Description: Callback For Handling Data Frame Reception

  Parameters: are not used.
  
  Returns: nothing.
*******************************************************************************/
void irCommandReceived(APS_DataInd_t* indData)
{	
	setLED(LED_COLOR_PINK);
     indData = indData;
}


/*******************************************************************************
  Description: Callback For Handling Data Frame Reception

  Parameters: are not used.
  
  Returns: nothing.
*******************************************************************************/
void hidCommandReceived(APS_DataInd_t* indData)
{	
	setLED(LED_COLOR_PINK);
     indData = indData;
}





/*******************************************************************************
  Description: Callback For Sending a transmission

  Parameters: are not used.
  
  Returns: nothing.
*******************************************************************************/
static void networkTransmissionConfirm(APS_DataConf_t *result)
{			
	//Empty Function just to make sure stuff doesn't explode... theoretically we could retry here.
	ableToSend = true;
	setLED(LED_COLOR_OFF);     
}

/*******************************************************************************
  Description: Callback For Network Joining

  Parameters: are not used.
  
  Returns: nothing.
*******************************************************************************/
static void networkStartConfirm(ZDO_StartNetworkConf_t *confirmInfo)
{
	
	if (ZDO_SUCCESS_STATUS == confirmInfo->status) {
		appState = APP_NETWORK_JOINED;
		SYS_PostTask(APL_TASK_ID);
		startLEDBlink(LED_COLOR_GREEN, LED_BLINK_SLOW_HEARTBEAT);
		// Configure blink timer
	}else{
		setLED(LED_COLOR_WHITE);
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
	 setLED(LED_COLOR_YELLOW);
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
	initializeLED();	
	setLED(LED_COLOR_PURPLE);
							
     initializeConfigurationServer();
     			     
     registerEndpoints();                             
			
	//join a network
	startNetworkReq.ZDO_StartNetworkConf = networkStartConfirm;
	ZDO_StartNetworkReq(&startNetworkReq);	     		
	     			
	initializeSerial();
	//initializePWM();
	
	testTimer.callback = sendTestMessage;
	testTimer.interval = 100;	
	testTimer.mode = TIMER_REPEAT_MODE;
	//HAL_StartAppTimer(&testTimer);
	
	retryTimer.callback = retryCallback;
	retryTimer.interval = 50;	
	retryTimer.mode = TIMER_ONE_SHOT_MODE;
	
	fakeTimer.callback = fakeShadeMessage;
	fakeTimer.interval = 7000;	
	fakeTimer.mode = TIMER_REPEAT_MODE;
	//HAL_StartAppTimer(&fakeTimer);
}

void retryCallback()
{
	sendMessageToModule(retryAddr);
}

void initializePWM()
{
	//setup pwm
	HAL_OpenPwm(PWM_UNIT_3);			
	pwmChannelDimmer.unit = PWM_UNIT_3	;
	pwmChannelDimmer.channel  = PWM_CHANNEL_0;
	pwmChannelDimmer.polarity = PWM_POLARITY_INVERTED;			
	HAL_SetPwmFrequency(PWM_UNIT_3, 500 , PWM_PRESCALER_64 );			
	HAL_StartPwm(&pwmChannelDimmer);
	HAL_SetPwmCompareValue(&pwmChannelDimmer, 100);          	
}

void initializeSerial()
{
	usartChannel0.baudrate = USART_BAUDRATE_9600;
	usartChannel0.tty = USART_CHANNEL_1;
	usartChannel0.mode = USART_MODE_ASYNC;
	usartChannel0.parity = USART_PARITY_NONE;
	usartChannel0.stopbits = USART_STOPBIT_1;
	usartChannel0.flowControl = USART_FLOW_CONTROL_NONE;	
	usartChannel0.dataLength = USART_DATA8;	
	usartChannel0.rxBuffer = rxBuffer;
	usartChannel0.rxBufferLength = sizeof(rxBuffer);
	usartChannel0.txBuffer = NULL;
	usartChannel0.txBufferLength = NULL;
	
	usartChannel0.txCallback = usartTransmitComplete;
	usartChannel0.rxCallback = usartReceiveComplete;
	
	HAL_OpenUsart(&usartChannel0);
}

void usartReceiveComplete(uint16_t length)
{
   if(length != sizeof(UsartMessagePacket))
   {
	startLEDBlink(LED_COLOR_ORANGE, LED_BLINK_FAST);
	return;   
   }
   UsartMessagePacket usartPacket;
   HAL_ReadUsart(&usartChannel0, (uint8_t*)(&usartPacket),sizeof(usartPacket));
      
   ShadeCommandData shadeData;
   HIDCommandData hidData;
   switch(usartPacket.type)
   {
	   case SHADE_CONTROL:	    
	    shadeData = (usartPacket.shadePacket);
	    shadeMessage.data.ButtonMask = shadeData.ButtonMask;
	    (shadeData.ButtonMask == SHADE_DIRECTION_DOWN)? setLED(LED_COLOR_RED) : setLED(LED_COLOR_BLUE);
         shadeMessage.data.Duration = shadeData.Duration;
	    childToSendTo = children[0];
	    appState = APP_NETWORK_SEND_SHADE;
	    SYS_PostTask(APL_TASK_ID);	
	    break;
		
	    case HID_CONTROL:	    
	    hidData = (usartPacket.hidPacket);
	    hidMessage.data.mouseData = hidData.mouseData;
	    hidMessage.data.keySequence = hidData.keySequence;
	    
	    setLED(LED_COLOR_GREEN);         
	    childToSendTo = children[0];
	    appState = APP_NETWORK_SEND_HID;
		
	    SYS_PostTask(APL_TASK_ID);	
	    break;	
	   default:
	    startLEDBlink(LED_COLOR_PURPLE, LED_BLINK_FAST);
	    break;		
   }
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
     
     dimmerEndpointParams.simpleDescriptor = &dimmerEndpoint;
	dimmerEndpointParams.APS_DataInd = dimmerCommandReceived;			
	APS_RegisterEndpointReq(&dimmerEndpointParams);
						                
     shadeEndpointParams.simpleDescriptor = &shadeEndpoint;
	shadeEndpointParams.APS_DataInd = shadeCommandReceived;			
	APS_RegisterEndpointReq(&shadeEndpointParams);       						                                                   
	
	irEndpointParams.simpleDescriptor = &irEndpoint;
	irEndpointParams.APS_DataInd = irCommandReceived;			
	APS_RegisterEndpointReq(&irEndpointParams);       						                                                   
	
	hidEndpointParams.simpleDescriptor = &hidEndpoint;
	hidEndpointParams.APS_DataInd = hidCommandReceived;			
	APS_RegisterEndpointReq(&hidEndpointParams);
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

//eof blink.c
