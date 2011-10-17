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
#include <spi.h>
#include "USB.h"
#include "MAX3420E.h"
#include <HIDModule.h>
#include "helpers.h"
#include <gpio.h>
#include <pwm.h>
#include <irq.h>


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

//Global AppTimer
HAL_AppTimer_t usbTimeOut;
HAL_AppTimer_t usbKeyboardTimer;
HAL_AppTimer_t usbMouseTimer;

//Gloabl Usart Channel
HAL_SpiDescriptor_t spiChannel0;
 
static ShortAddr_t children[CS_MAX_CHILDREN_AMOUNT];
static ShortAddr_t childToSendTo = 0;
static int c_children = 0;


static KEYBOARD_STATE usbState;
static uint8_t usbRWUEnabled;
static uint8_t usbConfigValue;

static bool usbEp3Stall;
static bool usbEp2Stall;

static bool usbKeyboard;
static bool usbMouse;

static bool usbSuspended;
static uint8_t usbSetupPacket[8];

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
		break;
          
          ///APP_NETWORK_IDLE
		case APP_NETWORK_IDLE:		
		break;
          
		default:
		break;
	}		
  
}

void hidCommandReceived(APS_DataInd_t* indData)
{		
     indData = indData;
}

void statusMessageReceived(APS_DataInd_t* indData)
{		
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
		//startLEDBlink(LED_COLOR_GREEN, LED_BLINK_SLOW_HEARTBEAT);
		// Configure blink timer
	}else{
		setLED(LED_COLOR_TURQUOISE);
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
	initializeLED();	
	setLED(LED_COLOR_RED);
							
     initializeConfigurationServer();
     			     
     registerEndpoints();                             
			
	//join a network
	startNetworkReq.ZDO_StartNetworkConf = networkStartConfirm;
	ZDO_StartNetworkReq(&startNetworkReq);	     		
	     			
	initializeSPI();
	GPIO_8_make_out();
	GPIO_8_set();
	initializeUSB();
	
}

void spiStartTransmission(uint8_t *message, uint16_t length)
{				
	HAL_WriteSpi(&spiChannel0, message, length);		
}

void spiStartRead(uint8_t *buffer, uint16_t length)
{					
     HAL_ReadSpi(&spiChannel0, buffer, length);		
}

void writeMAXBytes(uint8_t addr, uint8_t length, uint8_t *message)
{
	uint8_t i;
	uint8_t packet[length + 1];
	packet[0] = (addr) + 3;
	
	for(i = 0; i< length; i++)
	{
		packet[i+1] = message[i];		
	}
	GPIO_8_clr();
	spiStartTransmission(packet, length + 1);
	GPIO_8_set();
}

void resetMax()
{
	uint32_t i;
	uint32_t delay = 10000;
     writeMAXReg(rUSBCTL, 0x20);
	for(i = 0; i<delay; i++)
	{
		NOP;
     };
	writeMAXReg(rUSBCTL, 0x00);	
}	 


uint8_t readMAXReg(uint8_t addr)
{	
	uint8_t packet;
	packet = (addr) ;
	GPIO_8_clr();
	halSyncUsartSpiWriteData(spiChannel0.tty, &packet, sizeof(uint8_t));
	packet = 0;
	halSyncUsartSpiReadData(spiChannel0.tty, &packet, sizeof(uint8_t));
	GPIO_8_set();
	return packet;
	cli();
}

uint8_t readMAXRegAck(uint8_t addr)
{	
	uint8_t packet;
	packet = (addr) + 1;	
	GPIO_8_clr();
	halSyncUsartSpiWriteData(spiChannel0.tty, &packet, sizeof(uint8_t));	
	packet = 0;				
	halSyncUsartSpiReadData(spiChannel0.tty, &packet, sizeof(uint8_t));
	GPIO_8_set();
	return packet;
}

void readMAXBytes(uint8_t addr, uint8_t length, uint8_t *buffer)
{	
	uint8_t packet, i;
	packet = (addr);
	GPIO_8_clr();	
	halSyncUsartSpiWriteData(spiChannel0.tty, &packet, sizeof(uint8_t));
	for(i = 0; i<length; i++)
	{
		buffer[i] = 0;
	}
	halSyncUsartSpiReadData(spiChannel0.tty, buffer, length);	
	GPIO_8_set();
}


void writeMAXReg(uint8_t addr, uint8_t data)
{
	uint8_t packet[2];
	packet[0] = (addr) + 2;
	packet[1] = data;
	GPIO_8_clr();
	spiStartTransmission(packet, 2);
	GPIO_8_set();
}

void writeMAXRegAck(uint8_t addr, uint8_t data)
{	
	uint8_t packet[2];
	packet[0] = (addr) + 3;
	packet[1] = data;
	GPIO_8_clr();
	spiStartTransmission(packet, 2);
	GPIO_8_set();
}

void testSpiFnct()
{
	writeMAXReg(0x4,0xAF);
}

void enableUSBInit()
{
	writeMAXReg(rEPIEN, (bmSUDAVIE | bmIN3BAVIE));
	writeMAXReg(rUSBIEN, (bmURESIE | bmURESDNIE));
}

void initializeUSB()
{
	//setup INT callback	
	writeMAXReg(rPINCTL, (bmFDUPSPI | bmPOSINT)); //default interrupt is edge-active 1->0
	resetMax();	
	
	usbState = KEYBOARD_STATE_IDLE;		
	
	while(!testMAXChip()); //keep trying till we are successful.
	

	
	writeMAXReg(rUSBCTL,(bmCONNECT | bmVBGATE));
     enableUSBInit();
	writeMAXReg(rCPUCTL,bmIE);
	
	
	HAL_RegisterIrq(IRQ_7,IRQ_RISING_EDGE,USBHandleINT);
	HAL_EnableIrq(IRQ_7);	   
	
	usbTimeOut.callback = USBHandleTimeOut;
	usbTimeOut.interval = 8000;	
	usbTimeOut.mode = TIMER_REPEAT_MODE;
	HAL_StartAppTimer(&usbTimeOut);	  
	
	usbKeyboard = false;	
	
	//prep the first key up;
	writeMAXReg(rEP3INFIFO,0);			// send the "keys up" code
	writeMAXReg(rEP3INFIFO,0);
	writeMAXReg(rEP3INFIFO,0);
	writeMAXReg(rEP3INBC,3);
}

bool testMAXChip()
{
	bool ret = false;
	uint8_t j,wr,rd;
	uint8_t correct[] = {0x01,0x02,0x04,0x08,0x10,0x20,0x40,0x80};
	//lets test our connection
     wr=0x01; // initial register write value	
	setLED(LED_COLOR_RED);
     for(j=0; j<8; j++)
     {
          writeMAXReg(rUSBIEN,wr);
          rd = readMAXReg(rUSBIEN);
          wr <<= 1; // Put a breakpoint here. Values of 'rd' should be 01,02,04,08,10,20,40,80
		if(rd == correct[j])
		{
			switch(j)
			{
				case 0:
				     //setLED(LED_COLOR_ORANGE);
					 break;
				case 1:
				     //setLED(LED_COLOR_WHITE);
					 break;
				case 2:
				     //setLED(LED_COLOR_PINK);
					 break;
				case 3:
				     //setLED(LED_COLOR_PURPLE);
					 break;
				case 4:
				     //setLED(LED_COLOR_BLUE);
					 break;
				case 5:
				     //setLED(LED_COLOR_TURQUOISE);
					 break;
				case 6:
				     //setLED(LED_COLOR_GREEN);
					 break;
				case 7:
				     setLED(LED_COLOR_WHITE);
					 ret = true;
					 break;
			}
		}			  
     }
	 return ret;
}

void USBHandleTimeOut()
{
	//usbKeyboard = (~(readMAXReg(rGPIO)) & 0x10);
	//if(usbSuspended)
	//{
		//if(usbKeyboard)
		//{
			//SETBIT(rUSBCTL, bmSIGRWU);
			//while((readMAXReg(rUSBIRQ) & bmSIGRWU) == 0);
			//CLRBIT(rUSBCTL, bmSIGRWU);
			//usbSuspended = false;
		//}
	//}
}

void USBHandleFakeKeyboard()
{
	usbKeyboard = true;
}	

void USBHandleINT()
{	
	static uint8_t itest1,itest2;
	setLED(LED_COLOR_BLUE);
	HAL_StopAppTimer(&usbTimeOut);
	HAL_StartAppTimer(&usbTimeOut);	
	
	itest1 = readMAXReg(rEPIEN) & readMAXReg(rEPIRQ);
	itest2 = readMAXReg(rUSBIEN) & readMAXReg(rUSBIRQ);
	
	if(itest1 & bmSUDAVIE)
	{
		writeMAXReg(rEPIRQ, bmSUDAVIRQ);
		USBHandleSetup();
		//setLED(LED_COLOR_GREEN);
	}
	else if(itest1 & bmIN3BAVIE)
	{
		USBHandleINT3();
		//setLED(LED_COLOR_PURPLE);
	}
	else if(itest1 & bmIN2BAVIE)
	{
		USBHandleINT2();
//		setLED(LED_COLOR_PURPLE);
	}
	else if(itest2 & bmSUSPIE)
	{
	     writeMAXReg(rUSBIRQ, bmSUSPIRQ);
		CLRBIT(rUSBIEN, bmSUSPIE);
		
		writeMAXReg(rUSBIRQ, bmBUSACTIRQ);
		SETBIT(rUSBIEN, bmBUSACTIE);
		usbSuspended = true;
//		setLED(LED_COLOR_YELLOW);
	}
	else if(itest2 & bmBUSACTIE)
	{
		writeMAXReg(rUSBIRQ, bmBUSACTIRQ);
		CLRBIT(rUSBIEN, bmBUSACTIE);
		SETBIT(rUSBIEN, bmSUSPIE);
		usbSuspended = false;
//		setLED(LED_COLOR_TURQUOISE);
	}
	else if(itest2 & bmURESIE)
	{
		writeMAXReg(rUSBIRQ, bmURESIRQ);
//		setLED(LED_COLOR_RED);
	}
	else if(itest2 & bmURESDNIE)
	{
		writeMAXReg(rUSBIRQ, bmURESDNIRQ);
		enableUSBInit();
//		setLED(LED_COLOR_ORANGE);
	}else{

	}
	
}

void USBHandleSetup()
{
	readMAXBytes(rSUDFIFO, 8, usbSetupPacket);
	switch(usbSetupPacket[bmRequestType] & 0x60) //only check bits 6 & 5
	{
		case 0x00:
		     USBStdRequest();
			break;
		case 0x20:
		     USBClassRequest();
			break;
		case 0x40:
		     USBVendorRequest();
			break;
		default:
		     STALL_EP0;
			break;
	}			
}


void USBHandleINT2()
{
     //do nothing with the mouse right now.
	writeMAXReg(rEP2INFIFO, 0x00); //Buttons
	writeMAXReg(rEP2INFIFO, 0x00);//x
	writeMAXReg(rEP2INFIFO, 0x00); //y
	writeMAXReg(rEP2INBC, 3);	 
}


void USBHandleINT3()
{
	switch(usbState)
	{
		case KEYBOARD_STATE_IDLE:
		     setLED(LED_COLOR_ORANGE);
		     if(usbKeyboard)
			{
				 usbKeyboard = false;
				 writeMAXReg(rEP3INFIFO, 0x08);
				 writeMAXReg(rEP3INFIFO, 0x0);
				 writeMAXReg(rEP3INFIFO, 0x07);
				 writeMAXReg(rEP3INBC, 3);
				 usbState = KEYBOARD_STATE_RELEASE;				 				
			}
			else
			{
			     writeMAXReg(rEP3INFIFO, 0x00); //KEY UP
			     writeMAXReg(rEP3INFIFO, 0x00);
			     writeMAXReg(rEP3INFIFO, 0x00); //KEY UP
			     writeMAXReg(rEP3INBC, 3);
			}
			break;
		case KEYBOARD_STATE_RELEASE:		     
		     setLED(LED_COLOR_TURQUOISE);
			writeMAXReg(rEP3INFIFO, 0x00); //KEY UP
			writeMAXReg(rEP3INFIFO, 0x00);
			writeMAXReg(rEP3INFIFO, 0x00); //KEY UP
			writeMAXReg(rEP3INBC, 3);
			usbState = KEYBOARD_STATE_IDLE;
			break;
		case KEYBOARD_STATE_WAIT:
		     setLED(LED_COLOR_PINK);
		     if(!usbKeyboard)
			{
			     usbState = KEYBOARD_STATE_IDLE;
			}
			break;
		default:
		     
		     usbState = KEYBOARD_STATE_IDLE;				 			 
	}
}

void USBStdRequest()
{
	uint8_t temp;
	switch(usbSetupPacket[bRequest])
	{		
		case SR_GET_DESCRIPTOR:
		     USBSendDescriptor();
			break;
		case SR_SET_FEATURE:
		     USBFeature(true);
			break;
		case SR_CLEAR_FEATURE:
		     USBFeature(false);
			break;
		case SR_GET_STATUS:
		     USBGetStatus();
			break;
		case SR_SET_INTERFACE:
		     USBSetInterface();
			break;
		case SR_GET_INTERFACE:
		     USBGetInterface();
			break;
		case SR_GET_CONFIGURATION:
		     USBGetConfiguration();
			break;
		case SR_SET_CONFIGURATION:
		     USBSetConfiguration();
			break;
		case SR_SET_ADDRESS:
		     temp = readMAXRegAck(rFNADDR);
			break;
		default:
		     STALL_EP0; //NAK
			break;
	}
}

void USBSetConfiguration()
{
	uint8_t temp;
	usbConfigValue = usbSetupPacket[wValueL];
	if(usbConfigValue != 0)
	{
		SETBIT(rUSBIEN, bmSUSPIE);
		temp = readMAXRegAck(rFNADDR); //ACK
	}
}

void USBGetConfiguration()
{
	writeMAXReg(rEP0FIFO, usbConfigValue);
	writeMAXRegAck(rEP0BC, 1);
}

void USBSetInterface()
{
	uint8_t temp;
	if( (usbSetupPacket[wValueL] == 0) &
	    (usbSetupPacket[wIndexL] == 0))
	{
	     temp = readMAXRegAck(rFNADDR); // dummy just to ACK
	}
	else
	{
		STALL_EP0;
	}
}

void USBGetInterface()
{
	if(usbSetupPacket[wIndexL] == 0x00)
	{
		writeMAXReg(rEP0FIFO, 0);
		writeMAXRegAck(rEP0BC,1);
	}
	else
	{
		STALL_EP0;
	}
}

void USBGetStatus()
{
	uint8_t testByte = usbSetupPacket[bmRequestType];	
	switch(testByte)
	{
		case 0x80: //DEVICE
		     writeMAXReg(rEP0FIFO, usbRWUEnabled + 1); // +1 means self powered
			writeMAXReg(rEP0FIFO, 0x00);
			writeMAXRegAck(rEP3INBC, 2);
			break;
		case 0x81: //INTERFACE
		     writeMAXReg(rEP0FIFO, 0x00);
			writeMAXReg(rEP0FIFO, 0x00);
			writeMAXRegAck(rEP3INBC, 2);
			break;
		case 0x82: //ENDPOINT
		     if(usbSetupPacket[wIndexL] == 0x83)
			{
				 uint8_t stall = (usbEp3Stall) ? (1):(0);
				 writeMAXReg(rEP0FIFO, stall);
				 writeMAXReg(rEP0FIFO, 0x00);
			      writeMAXRegAck(rEP0BC, 2);
			     break;
			}
			else if(usbSetupPacket[wIndexL] == 0x82)
			{
				 uint8_t stall = (usbEp2Stall) ? (1):(0);
				 writeMAXReg(rEP0FIFO, stall);
				 writeMAXReg(rEP0FIFO, 0x00);
			    writeMAXRegAck(rEP0BC, 2);
			    break;
			}
			else
			{
				STALL_EP0;
				break;
			}
		default: 
		     STALL_EP0;
			break;
	}				
}

void USBFeature(bool sc)
{
	uint8_t maskData = 0;
	if(  (usbSetupPacket[bmRequestType] == 0x02) && //ENPOINT
	     (usbSetupPacket[wValueL] == 0x00)) //EP Halt
	{
		if(usbSetupPacket[wIndexL] == 0x83)//EP3
		{
		     maskData = readMAXReg(rEPSTALLS);
		     (sc) ? (maskData |= bmSTLEP3IN) : (maskData &= ~(bmSTLEP3IN));
		     usbEp3Stall = sc;		
		     writeMAXReg(rEPSTALLS, (maskData | bmACKSTAT) );					
		}
		else if(usbSetupPacket[wIndexL] == 0x82)
		{
			maskData = readMAXReg(rEPSTALLS);
		     (sc) ? (maskData |= bmSTLEP2IN) : (maskData &= ~(bmSTLEP2IN));
		     usbEp2Stall = sc;		
		     writeMAXReg(rEPSTALLS, (maskData | bmACKSTAT) );					
		}						
	     
			
	}
	else if( (usbSetupPacket[bmRequestType] == 0x00) &&
	         (usbSetupPacket[wValueL] == 0x01))	
	{
		uint8_t temp;
	     usbRWUEnabled = (sc) ? (2) : (0);
		temp = readMAXRegAck(rFNADDR); //throw away value just so we ACK
	}
	else
	{
		STALL_EP0; //NAK
	}
	
}

void USBSendDescriptor()
{
	uint16_t reqlen, sendlen, desclen;
	uint8_t *p_Descriptor;
	
	desclen = 0;
	reqlen = usbSetupPacket[wLengthL] + 256 * usbSetupPacket[wLengthH];
	switch(usbSetupPacket[wValueH])
	{
		case GD_DEVICE:
		     setLED(LED_COLOR_PURPLE);
		     desclen = DD[0];
			p_Descriptor = DD;
			break;
		case GD_CONFIGURATION:
		     setLED(LED_COLOR_YELLOW);
		     desclen = CD[2]; 
			p_Descriptor = CD;
			break;
		case GD_STRING:
		     setLED(LED_COLOR_BLUE);
		     switch(usbSetupPacket[wValueL])
			{
			     case 0:
				 desclen = STR0[0];
				 p_Descriptor = STR0;
				 break;	 
				case 1:
				 desclen = STR1[0];
				 p_Descriptor = STR1;
				 break;
				case 2:
				 desclen = STR2[0];
				 p_Descriptor = STR2;
				 break;
				case 3:
				 desclen = STR3[0];
				 p_Descriptor = STR3;
				 break;
		     }
			break;
		case GD_HID:		
		     if(usbSetupPacket[wIndexL] == 0)
			{
				setLED(LED_COLOR_ORANGE);
				desclen = CD[18];
			     p_Descriptor = &(CD[18]);
			 }
			 else if(usbSetupPacket[wIndexL] == 1)
			 {
				 setLED(LED_COLOR_TURQUOISE);
				desclen = CD[44];
			     p_Descriptor = &(CD[44]);
			 }
			 else
			 {
				 setLED(LED_COLOR_PURPLE);
			 }				 			 			 						 
		     
			break;
		case GD_REPORT:
		     if(usbSetupPacket[wIndexL] == 0)
			{
				setLED(LED_COLOR_RED);
				desclen = CD[25];
			     p_Descriptor = RepD_Keyboard;
			}
			else if(usbSetupPacket[wIndexL] == 1)
			{
				setLED(LED_COLOR_WHITE);
				desclen = CD[51];
			     p_Descriptor = RepD_Mouse;
			}
			else										     
			{
			     setLED(LED_COLOR_BLUE);	
			}							
			break;
		default:
		     setLED(wValueH,wValueH,wValueH);
	}
	
	if(desclen != 0)
	{
		sendlen = (reqlen <= desclen) ? (reqlen) : (desclen);
		writeMAXBytes(rEP0FIFO, sendlen, p_Descriptor); //send smaller or two
		writeMAXRegAck(rEP0BC, sendlen); //arm it
	}else{
		STALL_EP0;
	}
	
	
	
	
	usbKeyboardTimer.callback = USBHandleFakeKeyboard;
	usbKeyboardTimer.interval = 1000;	
	usbKeyboardTimer.mode = TIMER_REPEAT_MODE;
	HAL_StartAppTimer(&usbKeyboardTimer);	
	
}

void USBClassRequest()
{
	STALL_EP0;
}

void USBVendorRequest()
{
	STALL_EP0;
}

void initializeSPI()
{	
	spiChannel0.baudRate = SPI_CLOCK_RATE_500;
	spiChannel0.tty = SPI_CHANNEL_0;
	spiChannel0.dataOrder = SPI_DATA_MSB_FIRST;
	spiChannel0.clockMode = SPI_CLOCK_MODE0;	
	spiChannel0.callback = NULL;	
	HAL_OpenSpi(&spiChannel0);	
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
