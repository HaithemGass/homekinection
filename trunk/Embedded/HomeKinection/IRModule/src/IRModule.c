/**************************************************************************//**
  \file irModule.c
  
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
#include <halPwm.h>
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
HAL_PwmDescriptor_t pwmChannel1;

static IRCommandPacket irMessage; // ir Message buffer
static StatusMessagePacket statusMessage; // ir Message buffer
static NetworkJoinPacket networkPacket; // ir Message buffer

static ShortAddr_t myAddr;

static HAL_AppTimer_t retryTimer;
static HAL_AppTimer_t remoteTimer;

static uint16_t intensity = 0;
static bool ableToSend = true;
static uint8_t sequenceIndex = 0;
static bool recording = false;
static bool waitingForFirstPulse = false;
static bool playingIR = false;
static bool pwmOn = false;
static remoteSequence currentSequence = {};

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
		        setLED(LED_COLOR_BLUE);
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
		case APP_NETWORK_SEND_IR:
                if(ableToSend)
			 {
		        setLED(LED_COLOR_BLUE);
			   sendStatusPacket(CPU_TO_LE16(0));	 				 
			 }else{
			   HAL_StopAppTimer(&retryTimer);
			   retryTimer.callback = retryIRPacket;
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
	
	packet.srcEndpoint = irEndpoint.endpoint;
	packet.dstEndpoint = networkJoinEndpoint.endpoint;
  
	packet.clusterId = CPU_TO_LE16(0);	
	packet.txOptions.acknowledgedTransmission = 1;
	packet.txOptions.fragmentationPermitted = 1;
	packet.radius = 0x0;
	packet.APS_DataConf = networkTransmissionConfirm;
	APS_DataReq(&packet);
	ableToSend = false; 	
}

void sendStatusPacket(ShortAddr_t addr)
{	   	
     statusMessage.data.deviceType = DEVICE_TYPE;
     statusMessage.data.statusMessageType = 0x0000;
     statusMessage.data.shortAddress = myAddr ;     
	 
	stuffStatusPacket((uint8_t*) &currentSequence, sizeof(currentSequence), &statusMessage)	 ;
     
	packet.asdu = (uint8_t *)(&statusMessage.data);
	packet.asduLength = sizeof(statusMessage.data);
	packet.profileId = 1;
	packet.dstAddrMode = APS_SHORT_ADDRESS;
	packet.dstAddress.shortAddress = addr;
	
	packet.srcEndpoint = irEndpoint.endpoint;
	packet.dstEndpoint = statusEndpoint.endpoint;
  
	packet.clusterId = CPU_TO_LE16(0);	
	packet.txOptions.acknowledgedTransmission = 0;
	packet.radius = 0x0;
	packet.APS_DataConf = networkTransmissionConfirm;
	APS_DataReq(&packet);
	ableToSend = false; 	
}


void sendIRPacket(ShortAddr_t addr)
{	     
 
    irMessage.data.sequence=currentSequence;
	packet.asdu = (uint8_t *)(&irMessage.data);
	packet.asduLength = sizeof(irMessage.data);
	packet.profileId = 1;
	packet.dstAddrMode = APS_SHORT_ADDRESS;
	packet.dstAddress.shortAddress = addr;


     packet.srcEndpoint = irEndpoint.endpoint;
	packet.dstEndpoint = irEndpoint.endpoint;
  
	packet.clusterId = CPU_TO_LE16(0);	
	packet.txOptions.acknowledgedTransmission = 1;
	packet.txOptions.fragmentationPermitted = 1;
	packet.radius = 0x0;
	packet.APS_DataConf = networkTransmissionConfirm;
	
	
	APS_DataReq(&packet); 	
}


void retryStatusPacket()
{
	appState = APP_NETWORK_SEND_STATUS;
	SYS_PostTask(APL_TASK_ID);	
}

void retryIRPacket()
{
	appState = APP_NETWORK_SEND_IR;
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
	 setLED(LED_COLOR_BLUE)  ;	
}

void networkJoinMessageReceived(APS_DataInd_t* indData)
{	
     indData = indData;   
	 setLED(LED_COLOR_PURPLE);    	
}
/*******************************************************************************
  Description: Callback For Handling Data Frame Reception

  Parameters: are not used.
  
  Returns: nothing.
*******************************************************************************/
void irCommandReceived(APS_DataInd_t* indData)
{
	setLED(LED_COLOR_YELLOW);
	IRCommandData *data = (IRCommandData *)(indData->asdu);
	uint8_t record = (uint8_t) data->record;		 
	
	if(record)
	{
		// start recording
		EIMSK |= (0x02);
		waitingForFirstPulse=true;
		sequenceIndex=0;		
		setLED(LED_COLOR_CRIMSON);
	}
	else
	{
		// send enclosed command
		remoteSequence sequence = data->sequence;
		uint8_t length = sequence.length;
		remoteTransition *transitions =  sequence.transitions ;	

		currentSequence.length=length;				
		for(uint8_t i = 0; i< currentSequence.length; i++)
		{
			currentSequence.transitions[i] = transitions[i];			
		}
		startLEDBlink(LED_COLOR_GREEN,LED_BLINK_FAST);
		playIR();	
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
	//setLED(LED_COLOR_OFF);
	ableToSend = true;
	switch(result->status)
       {
              case APS_SUCCESS_STATUS:         
                 startLEDBlink(LED_COLOR_GREEN,LED_BLINK_SLOW_HEARTBEAT);        
                     break;
              case APS_NO_SHORT_ADDRESS_STATUS:       
                 setLED(LED_COLOR_ORANGE);     
                     break;
              case APS_DEFRAG_DEFERRED_STATUS:
                 setLED(LED_COLOR_PINK);       
                     break;
              case APS_NOT_SUPPORTED_STATUS:
                 setLED(LED_COLOR_PURPLE);     
                     break;
              case APS_ASDU_TOO_LONG_STATUS:
                   setLED(LED_COLOR_YELLOW);
                     break;     
				case APS_SECURED_LINK_KEY_STATUS:
					setLED(LED_COLOR_CERULEAN);
					break; 
              default:
                   setLED(result->status,result->status,result->status);
              break;
       }
	
}

void retryNetwork()
{
	sendNetworkPacket(CPU_TO_LE16(0)) ;	
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
        startLEDBlink(LED_COLOR_GREEN, LED_BLINK_SLOW_HEARTBEAT);	
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
	
     initializePWM();
	initializeTimer();
	initializeIR();
	
	remoteTimer.callback=playIR;
	remoteTimer.interval=50;
	remoteTimer.mode = TIMER_REPEAT_MODE;
	
}


void initializeIR()
{  
  // IRQ pin is input ... need to set D1, D2, and D3 as inputs
  DDRD &= ~(0x01 << 1);
  PORTD |= (0x01 << 1);
  
  //Disable our interrupts before messing with ISn    
  EIMSK &= ~(0x0E);
  
  // Clear previous settings of corresponding interrupt sense control
  EICRA &= ~(0xFC);
  
  // Setup corresponding interrupt sense control .. any edge should trigger it for RE and falling edge trigger for Button
  EICRA |= (0x04);
  
  //Clear the INTn interrupt flag
  EIFR |= (0x0E);
  
  //Go ahead and enable them    
  //EIMSK |= (0x02);
  
  
}

void initializePWM()
{
	//setup pwm
	GPIO_3_make_out(); // hehe
	GPIO_8_make_out();
	HAL_OpenPwm(PWM_UNIT_3);			
	pwmChannel1.unit = PWM_UNIT_3;
	pwmChannel1.channel  = PWM_CHANNEL_0;
	pwmChannel1.polarity = PWM_POLARITY_NON_INVERTED;			
	HAL_SetPwmFrequency(PWM_UNIT_3, PWM_FREQUENCY, PWM_PRESCALER_1 );			
	HAL_StartPwm(&pwmChannel1);
     HAL_SetPwmCompareValue(&pwmChannel1, PWM_FREQUENCY/2); 	   	
	 
	 TCCRnA((&pwmChannel1)->unit) &= ~((1 << COMnx1((&pwmChannel1))) | (1 << COMnx0((&pwmChannel1))));	 //turn of to start
}



void playIR()
{
	//startLEDBlink(LED_COLOR_GREEN, LED_BLINK_FAST);
	if(!playingIR) 
	{
	sequenceIndex=0;
	TCNT5=0x0000;
	OCR5C = currentSequence.transitions[sequenceIndex].duration;
	TCCRnA((&pwmChannel1)->unit) |=
    ((1 << COMnx1((&pwmChannel1))) | (1 << COMnx0((&pwmChannel1))));
	pwmOn=true;
	}	
	playingIR = true;	
	
}

void initializeTimer()
{	
	//TCCR5B = (1 << CS51);
	TCCR5B = (1 << CS51);
	OCR5C = RECORD_TIMEOUT;
	TIMSK5 |= (1 << 3);
	TCNT5=0x0000;
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

          irEndpointParams.simpleDescriptor = &irEndpoint;
	     irEndpointParams.APS_DataInd = irCommandReceived;			
	     APS_RegisterEndpointReq(&irEndpointParams);                         	         						                                                   
}

void onButtonDown(uint8_t button)
{
	if(button != BSP_KEY1)
	{
		HAL_StartAppTimer(&remoteTimer);	
	}
	
}

void onStartRecording(uint8_t button)
{
	
	if(button == BSP_KEY1)
	{
		setLED(LED_COLOR_RED);
		OCR5C = RECORD_TIMEOUT;
		waitingForFirstPulse=true;
		sequenceIndex=0;
	}
	else
	{
		HAL_StopAppTimer(&remoteTimer);
		
	}

	
}

void handleIRChange()
{
	
	if(waitingForFirstPulse)
	{
		waitingForFirstPulse = false;
		recording = true;
		resetTimer();
//		TCCRnA((&pwmChannel1)->unit) |=
//    ((1 << COMnx1((&pwmChannel1))) | (1 << COMnx0((&pwmChannel1))));
		setLED(LED_COLOR_CRIMSON);
	}
	else if(recording)
	{
		currentSequence.transitions[sequenceIndex++].duration=TCNT5;
		TCNT5=0x0000;	
		
	}
	
}

void readButton()
{		
	
	appState = APP_NETWORK_SEND_STATUS;
	SYS_PostTask(APL_TASK_ID);				
}



ISR(INT1_vect)
{
  //lets cancel our interrupts while we service
//  EIMSK &= ~(0x0E);
  handleIRChange();
 // EIMSK |= (0x0E);  
}



ISR(TIMER5_COMPC_vect)
{
	// timer timed out
	    
	// TCNT5 = 0x0000;
	if(recording)
	{
		currentSequence.length=sequenceIndex;				
		EIMSK &= ~(0x02);
		recording=false;
		sendIRPacket(CPU_TO_LE16(0));
	}
	else if(playingIR)
	{
		if(pwmOn)
		{
			pwmOn=false;
		 TCCRnA((&pwmChannel1)->unit) &= ~((1 << COMnx1((&pwmChannel1))) | (1 << COMnx0((&pwmChannel1))));
		}
		else
		{
			pwmOn=true;
		TCCRnA((&pwmChannel1)->unit) |=
    ((1 << COMnx1((&pwmChannel1))) | (1 << COMnx0((&pwmChannel1))));
		}
		if(++sequenceIndex == currentSequence.length)
		{
			playingIR=false;
		//	stopLEDBlink();
		TCCRnA((&pwmChannel1)->unit) &= ~((1 << COMnx1((&pwmChannel1))) | (1 << COMnx0((&pwmChannel1))));	
			pwmOn=false;
			startLEDBlink(LED_COLOR_GREEN, LED_BLINK_SLOW_HEARTBEAT);
		}
		else
		{
		OCR5C = currentSequence.transitions[sequenceIndex].duration;
		TCNT5=0x0000;
		}		
		
	}


}



void resetTimer()
{
	 // halMoveWordToRegister(&TCNTn(TIMER_UNIT_5), 0x0000);
	 OCR5C = (RECORD_TIMEOUT);	
	 TCNT5= 0x0000; 
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
