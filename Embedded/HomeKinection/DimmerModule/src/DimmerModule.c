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
#include "helpers.h"
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

static DimmerCommandPacket dimmerMessage; // Dimmer Message buffer
static StatusMessagePacket statusMessage; // Dimmer Message buffer
static NetworkJoinPacket networkPacket; // Dimmer Message buffer

static ShortAddr_t myAddr;

static HAL_AppTimer_t retryTimer;
static HAL_AppTimer_t fadeTimer;

static bool encoderChannel1, encoderChannel2;
static uint16_t intensity = 50;
static uint16_t fadeValue = 50;
static bool ableToSend = true;
static bool lampOn = true;
static bool switchEncoder = true;

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
			   setLED(LED_COLOR_LIME);       
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
			   //sendStatusPacket(CPU_TO_LE16(0));	 				 
			 }else{
			   HAL_StopAppTimer(&retryTimer);
			   retryTimer.callback = retryDimmerPacket;
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

void setDimmerBrightness(uint16_t i)
{

	uint16_t temp;
	if(!lampOn)
	{
		i = 1;
	}		
	//to avoid overflow
	if(i > 50)
	{
		temp = (MAX_DIMMER_BRIGHTNESS * (i-50)/100);
		temp = temp + (MAX_DIMMER_BRIGHTNESS)/2;		  	   
	}
	else
	{
		temp = ((MAX_DIMMER_BRIGHTNESS * i)/100); 
	}					    			 

	//HAL_SetPwmCompareValue(&pwmChannel1,(MAX_DIMMER_BRIGHTNESS - temp) ); 		
	halMoveWordToRegister(&OCR5C, MAX_DIMMER_BRIGHTNESS - temp);
}


void sendNetworkPacket(ShortAddr_t addr)
{	    		
     networkPacket.data.deviceType = DEVICE_TYPE;		
	networkPacket.data.shortAddr = myAddr;
	networkPacket.data.deviceUID = CS_UID;
	     
	packet.asdu = (uint8_t *)(&networkPacket.data);
	packet.asduLength = sizeof(networkPacket.data);
	packet.profileId = 1;
	packet.dstAddrMode = APS_SHORT_ADDRESS;
	packet.dstAddress.shortAddress = addr;
	
	packet.srcEndpoint = dimmerEndpoint.endpoint;
	packet.dstEndpoint = networkJoinEndpoint.endpoint;
  
	packet.clusterId = CPU_TO_LE16(0);	
	packet.txOptions.acknowledgedTransmission = 1;
	packet.radius = 0x0;
	packet.APS_DataConf = networkTransmissionConfirm;
	APS_DataReq(&packet);
	ableToSend = false; 	
}


void sendStatusPacket(ShortAddr_t addr)
{	    		
	static uint8_t toSend;
     statusMessage.data.deviceType = DEVICE_TYPE;
     statusMessage.data.statusMessageType = 0x0000;
     statusMessage.data.shortAddress = myAddr ;     
	toSend = (lampOn) ? intensity : 0;
	stuffStatusPacket((uint8_t*) &toSend, sizeof(toSend), &statusMessage)	 ;
     
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


void retryStatusPacket()
{
	appState = APP_NETWORK_SEND_STATUS;
	SYS_PostTask(APL_TASK_ID);	
}

void retryDimmerPacket()
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

void networkJoinMessageReceived(APS_DataInd_t* indData)
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
	
	DimmerCommandData *data = (StatusMessageData*)(indData->asdu);
	lampOn = true;
	if(!data->fadeToValue){
		setLED(LED_COLOR_BLUE);
          intensity = data->intensity;
	     if(intensity == 0)
	     {
               intensity = 1;
     	} 
     	setDimmerBrightness(intensity);
	}		 
	else
	{
		setLED(LED_COLOR_LIME);
		fadeTimer.callback = fadeToValue;
		fadeTimer.interval = 50;
	     fadeTimer.mode = TIMER_REPEAT_MODE;			   
		fadeValue = data->intensity;
		if(fadeValue == 0)
		{
			fadeValue = 1;			
		}
		HAL_StartAppTimer(&fadeTimer);
	}
	setLED(LED_COLOR_OFF);
	
}	

void fadeToValue()
{
	
	if(fadeValue == intensity)
	{
	     HAL_StopAppTimer(&fadeTimer)	;
	}		
	if(fadeValue > intensity)
	{
		intensity ++;
	}		
	if(fadeValue < intensity)
	{
		intensity --;
	}
	setDimmerBrightness(intensity);
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
          myAddr = confirmInfo->shortAddr;
		appState = APP_NETWORK_JOINED;
		SYS_PostTask(APL_TASK_ID);
          setLED(LED_COLOR_GREEN);
		if(ableToSend)
		{
		     sendNetworkPacket(CPU_TO_LE16(0)) ;	
		}
		else
		{
			HAL_StopAppTimer(&retryTimer);
			retryTimer.callback = retryNetwork;
			retryTimer.interval = 10;
			retryTimer.mode = TIMER_ONE_SHOT_MODE;
			HAL_StartAppTimer(&retryTimer);
			
		}				  
		
	}else{
		startLEDBlink(LED_COLOR_RED, LED_BLINK_MEDIUM);
	}
}


void retryNetwork()
{
	sendNetworkPacket(CPU_TO_LE16(0)) ;	
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
	
	initializeLED();	
	setLED(LED_COLOR_RED);
		 	                      		
     initializeConfigurationServer();
     			     
     registerEndpoints();                          
			
	//join a network
	startNetworkReq.ZDO_StartNetworkConf = networkStartConfirm;
	ZDO_StartNetworkReq(&startNetworkReq);	     		
	

	
	encoderChannel1 = ((PIND & (1 << PIND2)) != 0);
	encoderChannel2 = ((PIND & (1 << PIND3)) != 0);  
    //initializePWM();
    initializeTimer();
    initializeRotaryEncoder();	
    initializeZeroDetect();	
    setDimmerBrightness(intensity);	
}

void initializeTimer()
{
	GPIO_8_make_out();
	GPIO_8_clr();
	TCCR5B = (1 << CS51);
	TCCR5B |= (1 << CS50);
	OCR5C = MAX_DIMMER_BRIGHTNESS;
	TIMSK5 |= (1 << 3);	
}

ISR(TIMER5_COMPC_vect)
{
	GPIO_8_set();
}	

void initializeZeroDetect()
{	
	HAL_RegisterIrq(IRQ_7,IRQ_RISING_EDGE,resetTimer);
	HAL_EnableIrq(IRQ_7);      
}

void initializeRotaryEncoder()
{  
  // IRQ pin is input ... need to set D1, D2, and D3 as inputs
  DDRD &= ~(0x07 << 1);
  
  //D3 and D2 should be pull-ups 
  PORTD &= ~(1 << 0); 
  //PORTD |= (1 << 1);
  PORTD |= (1 << 2);
  PORTD |= (1 << 3);
  
  
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
	networkJoinEndpointParams.simpleDescriptor = &networkJoinEndpoint;
	networkJoinEndpointParams.APS_DataInd = networkJoinMessageReceived;			
	APS_RegisterEndpointReq(&networkJoinEndpointParams);
	
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
	setDimmerBrightness(intensity);
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

ISR(TIMER3_COMPA_vect)
{
     setLED(LED_COLOR_LIME);
}

void readGreyCode()
{
	bool newEn1, newEn2, sendMessage = false;	
	newEn1 = ((PIND & (1 << PIND2)) != 0);
	newEn2 = ((PIND & (1 << PIND3)) != 0);
	if(!lampOn) return;
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
				if(switchEncoder)
				{
				     if(intensity > 1)
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
			else
			{
		
				if(!switchEncoder)
				{
				     if(intensity > 1)
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
		}
		
	}
	else
	{
		encoderChannel1 = newEn1;
		if(newEn1 == newEn2)
		{		
			if(!switchEncoder)
				{
				     if(intensity > 1)
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
		else
		{		
			if(switchEncoder)
				{
				     if(intensity > 1)
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
		
	}
	encoderChannel1 = newEn1;
	encoderChannel2 = newEn2;
	if(sendMessage)
	{        				
		setDimmerBrightness(intensity);
		appState = APP_NETWORK_SEND_STATUS;
		SYS_PostTask(APL_TASK_ID);		
	}
}

void resetPWM()
{
	  halMoveWordToRegister(&TCNTn(PWM_UNIT_3), 0x0000);	 
}

void resetTimer()
{
	  TCNT5 = 0x0000;
	  if(intensity != 100)
	  {
          GPIO_8_clr();
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
