/**************************************************************************//**
\file  pwrCtrl.h

\brief Interface to the power control module.

\author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2011, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

\internal
  History:
    29/05/07 E. Ivanov - Created
*******************************************************************************/

#ifndef _POWERCTRL_H
#define _POWERCTRL_H

/******************************************************************************
                   Includes section
******************************************************************************/
#include <types.h>

/******************************************************************************
                   Prototypes section
******************************************************************************/
/**************************************************************************//**
\brief  Powers on periphery.

\param[in]
    id - number of periphery
******************************************************************************/
void bspOnPeriphery(uint8_t id);

/**************************************************************************//**
\brief  Powers off periphery.

\param[in]
    id - number of periphery
******************************************************************************/
void bspOffPeriphery(uint8_t id);

#endif /* _POWERCTRL_H */
// eof pwrCtrl.h

