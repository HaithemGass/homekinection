/**************************************************************************//**
  \file  halAdc.h

  \brief Declaration of hardware depended ADC interface.

  \author
      Atmel Corporation: http://www.atmel.com \n
      Support email: avr@atmel.com

    Copyright (c) 2008-2011, Atmel Corporation. All rights reserved.
    Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
    History:
      5/12/07 A. Khromykh - Created
 ******************************************************************************/
/******************************************************************************
 *   WARNING: CHANGING THIS FILE MAY AFFECT CORE FUNCTIONALITY OF THE STACK.  *
 *   EXPERT USERS SHOULD PROCEED WITH CAUTION.                                *
 ******************************************************************************/

#ifndef _HALADC_H
#define _HALADC_H

/******************************************************************************
                   Includes section
******************************************************************************/
#include <types.h>
#include <halTaskManager.h>
#include <halFCPU.h>
#include <adc.h>

/******************************************************************************
                   Prototypes section
******************************************************************************/
/**************************************************************************//**
\brief Initializations the ADC.
\param[in]
  param - pointer to parameter structure
******************************************************************************/
void halOpenAdc(HAL_AdcParams_t *param);

/**************************************************************************//**
\brief starts convertion on the ADC channel.
\param[in]
  channel - channel number.
******************************************************************************/
void halStartAdc(uint8_t channel);

/**************************************************************************//**
\brief Closes the ADC.
******************************************************************************/
void halCloseAdc(void);

/******************************************************************************
                   Inline static functions section
******************************************************************************/
/**************************************************************************//**
\brief SIG_ADC interrupt handler signal implementation
******************************************************************************/
INLINE void halSigAdcInterrupt(void)
{
  halPostTask3(HAL_ADC);
}

#endif /* _HALADC_H */

// eof halSdc.h
