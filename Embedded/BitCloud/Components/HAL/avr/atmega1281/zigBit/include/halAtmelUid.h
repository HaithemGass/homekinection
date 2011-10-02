/**************************************************************************//**
  \file  halAtmelUid.h

  \brief The header file describes the UID interface for Atmel MeshBean.

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2011, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
  History:
    10/12/10 A. Malkin - Created
 ******************************************************************************/
/******************************************************************************
 *   WARNING: CHANGING THIS FILE MAY AFFECT CORE FUNCTIONALITY OF THE STACK.  *
 *   EXPERT USERS SHOULD PROCEED WITH CAUTION.                                *
 ******************************************************************************/

#ifndef _HALATMELUID_H
#define _HALATMELUID_H

/******************************************************************************
                   Includes section
******************************************************************************/
#include <types.h>
#include <gpio.h>

/******************************************************************************
                   Definitions section
******************************************************************************/
#define SINGLE_WIRE_SUCCESS_STATUS 0
#define SINGLE_WIRE_ERROR_STATUS -1

/******************************************************************************
                   Types section
******************************************************************************/
/** \brief uid type. */
typedef union
{
  uint64_t uid;
  uint8_t array[sizeof(uint64_t)];
} HalUid_t;

/******************************************************************************
                 Inline static functions prototypes section.
******************************************************************************/
HAL_ASSIGN_PIN(SINGLE_WIRE, G, 5);  // Macros for the Single-wire pin actions.

/******************************************************************************
                   Prototypes section
******************************************************************************/
/**************************************************************************//**
\brief Reads Atmel MeshBean UID from ATTiny13A

\param[in] uidBuffer - memory for unique ID.

\return
    SINGLE_WIRE_SUCCESS_STATUS - if UID read successfully;
    SINGLE_WIRE_ERROR_STATUS - if error occured during UID read.
******************************************************************************/
int halReadAtmelMeshbeanUid(uint8_t *uidBuffer);

#endif /* _HALATMELUID_H */

// eof halAtmelUid.h
