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

//Gloabl Usart Channel
HAL_UsartDescriptor_t usartChannel0;

static DimmerCommandPacket dimmerMessage; // Dimmer Message buffer
static ShadeCommandPacket shadeMessage; // Dimmer Message buffer
 
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
          
          ///APP_NETWORK_IDLE
		case APP_NETWORK_IDLE:		
		break;
          
		default:
		break;
	}		
  
}

void sendDimmerPacket(ShortAddr_t addr)
{
	HAL_SetPwmCompareValue(&pwmChannel1, dimmerMessage.data.intensity);
	packet.asdu = (uint8_t *)(&dimmerMessage.data);
	packet.asduLength = sizeof(dimmerMessage.data);
	packet.profileId = 1;
	packet.dstAddrMode = APS_SHORT_ADDRESS;
	packet.dstAddress.shortAddress = addr;
	packet.dstEndpoint = 1;
  
	packet.clusterId = CPU_TO_LE16(0);
	packet.srcEndpoint = 1;
	packet.txOptions.acknowledgedTransmission = 0;
	packet.radius = 0x0;
	packet.APS_DataConf = networkTransmissionConfirm;
	APS_DataReq(&packet); 	
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
	GPIO_1_clr();
	GPIO_2_clr();	
     StatusMessageData *data = (StatusMessageData*)(indData->asdu);
     
     if(data->deviceType == DIMMER_MODULE){
		uint16_t * p_intensity = (uint16_t * )(data->message);
		uint16_t intensity = *p_intensity;		
          HAL_SetPwmCompareValue(&pwmChannel1, 5  * intensity);     
     }
	
	uint8_t message[] = "STATUS MESSAGE!\r\n";
	 
	HAL_WriteUsart(&usartChannel0, message, sizeof(message));
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
	GPIO_1_clr();
	GPIO_2_set();
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
     result = result;
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
	BSP_OpenLeds(); // Enable LEDs 
	GPIO_2_set();			
	GPIO_1_clr();	
							
     initializeConfigurationServer();
     			     
     registerEndpoints();                             
			
	//join a network
	startNetworkReq.ZDO_StartNetworkConf = networkStartConfirm;
	ZDO_StartNetworkReq(&startNetworkReq);	     		
	
     initializePWM();			
	initializeSerial();
}

void initializePWM()
{
	//setup pwm
	HAL_OpenPwm(PWM_UNIT_3);			
	pwmChannel1.unit = PWM_UNIT_3	;
	pwmChannel1.channel  = PWM_CHANNEL_0;
	pwmChannel1.polarity = PWM_POLARITY_INVERTED;			
	HAL_SetPwmFrequency(PWM_UNIT_3, 500 , PWM_PRESCALER_64 );			
	HAL_StartPwm(&pwmChannel1);
	HAL_SetPwmCompareValue(&pwmChannel1, 0);     
}

void initializeSerial()
{
	usartChannel0.baudrate = USART_BAUDRATE_9600;
	usartChannel0.tty = USART_CHANNEL_0;
	usartChannel0.mode = USART_MODE_ASYNC;
	usartChannel0.parity = USART_PARITY_NONE;
	usartChannel0.stopbits = USART_STOPBIT_1;
	usartChannel0.flowControl = USART_FLOW_CONTROL_NONE;	
	usartChannel0.dataLength = USART_DATA8;
	
	HAL_OpenUsart(&usartChannel0);
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
