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
#include <DimmerModule.h>
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

static DimmerCommandPacket dimmerMessage; // Dimmer Message buffer
static StatusMessagePacket statusMessage; // Dimmer Message buffer

static ShortAddr_t myAddr;

static HAL_AppTimer_t buttonTimer;

static bool encoderChannel1, encoderChannel2;
static uint16_t intensity = 0;
static bool ableToSend = true;
static bool lampOn = true;

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
		     GPIO_0_set();
			sendStatusPacket(CPU_TO_LE16(0));			
			appState = APP_NETWORK_IDLE;			
		break;
          
          ///APP_NETWORK_SEND_STATUS
		case APP_NETWORK_SEND_DIMMER:
		     GPIO_0_set();
			sendDimmerPacket(CPU_TO_LE16(0));
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
     
	uint16_t temp = (!lampOn) ? 0 : (MAX_DIMMER_BRIGHTNESS * intensity)/100 ;
	HAL_SetPwmCompareValue(&pwmChannel1,temp ); 
		
     statusMessage.data.deviceType = DIMMER_MODULE;
     statusMessage.data.statusMessageType = 0x0000;
     statusMessage.data.shortAddress = myAddr ;     
	 
	stuffStatusPacket((uint8_t*) &temp, sizeof(intensity), &statusMessage)	 ;
     
	packet.asdu = (uint8_t *)(&statusMessage.data);
	packet.asduLength = sizeof(statusMessage.data);
	packet.profileId = 1;
	packet.dstAddrMode = APS_SHORT_ADDRESS;
	packet.dstAddress.shortAddress = addr;
	
	packet.srcEndpoint = dimmerEndpoint.endpoint;
	packet.dstEndpoint = statusEndpoint.endpoint;
  
	packet.clusterId = CPU_TO_LE16(0);	
	packet.txOptions.acknowledgedTransmission = 0;
	packet.radius = 0x0;
	packet.APS_DataConf = networkTransmissionConfirm;
	APS_DataReq(&packet);
	ableToSend = false; 	
}


void sendDimmerPacket(ShortAddr_t addr)
{	     
     dimmerMessage.data.intensity = intensity;	 
     
	packet.asdu = (uint8_t *)(&dimmerMessage.data);
	packet.asduLength = sizeof(dimmerMessage.data);
	packet.profileId = 1;
	packet.dstAddrMode = APS_SHORT_ADDRESS;
	packet.dstAddress.shortAddress = addr;

     packet.srcEndpoint = dimmerEndpoint.endpoint;
	packet.dstEndpoint = dimmerEndpoint.endpoint;
  
	packet.clusterId = CPU_TO_LE16(0);	
	packet.txOptions.acknowledgedTransmission = 0;
	packet.radius = 0x0;
	packet.APS_DataConf = networkTransmissionConfirm;
	
	
	APS_DataReq(&packet); 	
}

/*******************************************************************************
  Description: Callback For Handling Data Frame Reception

  Parameters: are not used.
  
  Returns: nothing.
*******************************************************************************/
void statusMessageReceived(APS_DataInd_t* indData)
{	
     StatusMessageData *data = (StatusMessageData*)(indData->asdu);
     
     if(data->deviceType == DIMMER_MODULE){
          HAL_SetPwmCompareValue(&pwmChannel1, data->message[0]);     
     }          	
}


/*******************************************************************************
  Description: Callback For Handling Data Frame Reception

  Parameters: are not used.
  
  Returns: nothing.
*******************************************************************************/
void dimmerCommandReceived(APS_DataInd_t* indData)
{	
     indData = indData;
	 GPIO_2_clr();
	 GPIO_1_clr();
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
	BSP_OpenLeds(); // Enable LEDs 
	GPIO_2_set();			
	GPIO_1_clr();	
	
	/*buttonTimer.interval = 10;
	buttonTimer.mode = TIMER_REPEAT_MODE;
	buttonTimer.callback = readGreyCode;

     GPIO_6_make_in();
	GPIO_7_make_in();	
	
     HAL_StartAppTimer(&buttonTimer);*/
	 
	                      		
     initializeConfigurationServer();
     			     
     registerEndpoints();                          
			
	//join a network
	startNetworkReq.ZDO_StartNetworkConf = networkStartConfirm;
	ZDO_StartNetworkReq(&startNetworkReq);	     		
	
     initializePWM();
	initializeRotaryEncoder();			
}

void initializeRotaryEncoder()
{  
  // IRQ pin is input ... need to set D1, D2, and D3 as inputs
  DDRD &= ~(0x07 << 1);
  PORTD |= (0x07 << 1);
  
  //Disable our interrupts before messing with ISn    
  EIMSK &= ~(0x0E);
  
  // Clear previous settings of corresponding interrupt sense control
  EICRA &= ~(0xFC);
  
  // Setup corresponding interrupt sense control .. any edge should trigger it for RE and falling edge trigger for Button
  EICRA |= (0x58);
  
  //Clear the INTn interrupt flag
  EIFR |= (0x0E);
  
  //Go ahead and enable them    
  EIMSK |= (0x0E);
}

void initializePWM()
{
	//setup pwm
	HAL_OpenPwm(PWM_UNIT_3);			
	pwmChannel1.unit = PWM_UNIT_3;
	pwmChannel1.channel  = PWM_CHANNEL_0;
	pwmChannel1.polarity = PWM_POLARITY_INVERTED;			
	HAL_SetPwmFrequency(PWM_UNIT_3, MAX_DIMMER_BRIGHTNESS , PWM_PRESCALER_64 );			
	HAL_StartPwm(&pwmChannel1);	
	HAL_SetPwmCompareValue(&pwmChannel1, (MAX_DIMMER_BRIGHTNESS * intensity)/100);     
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
}

void readButton()
{		
	lampOn = !lampOn;
	appState = APP_NETWORK_SEND_STATUS;
	SYS_PostTask(APL_TASK_ID);				
}

ISR(INT1_vect)
{
  //lets cancel our interrupts while we service
  EIMSK &= ~(0x0E);  
  readButton();
  EIMSK |= (0x0E);  
}

ISR(INT2_vect)
{
  //lets cancel our interrupts while we service
  EIMSK &= ~(0x0E);
  EIMSK |= (0x0E);  
}

ISR(INT3_vect)
{
  //lets cancel our interrupts while we service
  EIMSK &= ~(0x0E);   
  readGreyCode();
  EIMSK |= (0x0E);  
}


void readGreyCode()
{
	bool newEn1, newEn2, sendMessage = false;	
	newEn1 = ((PIND & (1 << PIND2)) != 0);
	newEn2 = ((PIND & (1 << PIND3)) != 0);
	GPIO_1_clr();
	GPIO_2_clr();
	
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
				if(intensity < 100)
				{				  
				  intensity++;
				  sendMessage = true;
				}						
			}
			else
			{
		
				if(intensity > 0)
				{
                      intensity--;
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
			if(intensity > 0)
			{
				intensity--;
				sendMessage = true;				
			}						
		}
		else
		{		
			if(intensity < 100)
			{
				intensity++;
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
