/************************************************************************//**
  \file caps.h

  \brief
    The header file describes the CAPS interface

    The file describes the interface and types of CAPS

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2011, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
    History:
    27.11.08 A. Potashov - Created.
******************************************************************************/

#ifndef _CAPS_H
#define _CAPS_H

/******************************************************************************
                   Includes section
******************************************************************************/
#include <types.h>
#include <aps.h>
#include <appFramework.h>
#include <macAddr.h>
//#include <usart.h>

/******************************************************************************
                   Define(s) section
******************************************************************************/
#define CAPS_SERIAL_CHANNEL   UART_CHANNEL_1
#ifdef _ZCL_SECURITY_
  #define CAPS_DO_NOT_USE_APS_SECURITY     false
#else /* _ZCL_SECURITY_ */
  #define CAPS_DO_NOT_USE_APS_SECURITY     true
#endif /* _ZCL_SECURITY_ */

/******************************************************************************
                   Types section
******************************************************************************/


/*****************************************************************************
  Special CAPS data types definition
*****************************************************************************/
typedef enum
{
  CAPS_SERIAL_NONE     = 0x00,
  CAPS_SERIAL_UART     = 0x01
} CAPS_Serial_t;


/******************************************************************************
                   Constants section
******************************************************************************/

/******************************************************************************
                   External variables section
******************************************************************************/

/******************************************************************************
                   Prototypes section
******************************************************************************/


/*****************************************************************************
  CAPS API Prototypes
*****************************************************************************/

/**************************************************************************//**
  \brief Register endpoint in ZCL.

  \param[in] endpointDescriptor The APS_RegisterEndpoint_t primitive pointer as endpoint descriptor.
  \param[in] serial The serial flag for redirection to the serial interface. CAPS_Serial_t must be used.
  \return         None.
*****************************************************************************/
extern void CAPS_RegisterEndpoint(APS_RegisterEndpointReq_t *endpointDescriptor, uint8_t serial);

/**************************************************************************//**
  \brief
    Unregister endpoint in ZCL.

  \param[in] endpoint The 8-bit endpoint number.
  \return         None.
*****************************************************************************/
extern void CAPS_UnregisterEndpoint(Endpoint_t endpoint);

/**************************************************************************//**
  \brief CAPS Data Request interface.

  APS_DataConf field in APS_DataReq must be an actual confirm handler pointer.

  \param[in] req The APS_DataReq_t primitive pointer.
  \param[in] serial The serial flag for redirection to the serial interface. CAPS_Serial_t must be used.
  \param[in] noApsSecurity If true APS security is switched off.
  \return    None.
*****************************************************************************/
extern void CAPS_DataReq(APS_DataReq_t *req, uint8_t serial, bool noApsSecurity);

#endif  //#ifndef _CAPS_H


//eof caps.h
