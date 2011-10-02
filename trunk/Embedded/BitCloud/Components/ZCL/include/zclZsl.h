/**************************************************************************//**
  \file zclZsl.h

  \brief
    ZSL functions interface.

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2011, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
    History:
    17.03.10 A. Taradov - Created.
******************************************************************************/
#ifndef _ZCLZSL_H
#define	_ZCLZSL_H

/******************************************************************************
                        Includes section
******************************************************************************/
#include <intrpData.h>
#include <zcl.h>
#include <clusters.h>

/******************************************************************************
                        Definitions section
******************************************************************************/
/* Default network parameters, for debug purpose */
#define ZCL_ZSL_DEFAULT_WORKING_CHANNEL 20U
#define ZCL_ZSL_DEFAULT_NWK_PANID       0xAAAAU
#define ZCL_ZSL_DEFAULT_EXT_PANID       0xAAAAAAAAAAAAAAAAULL

/******************************************************************************
                        Types section
******************************************************************************/
typedef enum _ZCL_ZslStatus_t
{
  ZCL_ZSL_SUCCESS_STATUS          = 0x00,
  ZCL_ZSL_SCAN_RESULT_STATUS      = 0xf0,
  ZCL_ZSL_SCAN_FINISHED_STATUS    = 0xf1,
  ZCL_ZSL_SCAN_ABORTED_STATUS     = 0xf2,
  ZCL_ZSL_IDENTIFY_START_STATUS   = 0xf3,
  ZCL_ZSL_IDENTIFY_STOP_STATUS    = 0xf4,
  ZCL_ZSL_INVALID_SCENARIO_STATUS = 0xf5,
} ZCL_ZslStatus_t;

/******************************************************************************
                        Prototypes section
******************************************************************************/

/**************************************************************************//**
\brief Reset ZSL layer.
******************************************************************************/
void ZCL_ZslReset(void);

/**************************************************************************//**
\brief Get next output sequence number.
\returns next output sequence number.
******************************************************************************/
uint8_t ZCL_ZslGetSeq(void);

/**************************************************************************//**
\brief Generate new Transaction Identifier and start transaction timer for it.
******************************************************************************/
void ZCL_ZslGenerateTransactionId(void);

/**************************************************************************//**
\brief Get current Transaction Identifier.
\returns Current Transaction Identifier.
******************************************************************************/
uint32_t ZCL_ZslTransactionId(void);

/**************************************************************************//**
\brief Generate new Response Identifier.
******************************************************************************/
void ZCL_ZslGenerateResponseId(void);

/**************************************************************************//**
\brief Set Response Identifier to specified value.
\param[in] rid - Response Identifier value to be set
******************************************************************************/
void ZCL_ZslSetResponseId(uint32_t rid);

/**************************************************************************//**
\brief Get current Response Identifier.
\returns Current Response Identifier.
******************************************************************************/
uint32_t ZCL_ZslResponseId(void);

/**************************************************************************//**
\brief Get other device address (after Scan Request have been received).
\returns Other device extended address.
******************************************************************************/
uint64_t ZCL_ZslOtherDeviceAddress(void);

/**************************************************************************//**
\brief Check if Scan Response already have been received from device.
\returns true if Scan Response already have been received from device.
******************************************************************************/
bool ZCL_ZslDublicateRejection(ExtAddr_t ieee);

/**************************************************************************//**
\brief Assign new address from the range of free addressed.
\returns Newly assigned address or 0 in case of an error.
******************************************************************************/
ShortAddr_t ZCL_ZslAssignAddress(void);

/**************************************************************************//**
\brief Assign new group ID from the range of free group IDs.
\returns Newly assigned group ID or 0 in case of an error.
******************************************************************************/
uint16_t ZCL_ZslAssignGroupId(void);

#endif // _ZCLZSL_H

// eof zclZsl.h
