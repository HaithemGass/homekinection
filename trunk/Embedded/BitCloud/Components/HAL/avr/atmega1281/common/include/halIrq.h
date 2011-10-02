/***************************************************************************//**
  \file  halIrq.h

  \brief Declaration of HWD IRQ interface.

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

#ifndef _HALIRQ_H
#define _HALIRQ_H

/******************************************************************************
                   Includes section
******************************************************************************/
#include <types.h>
#include <halTaskManager.h>
#include <irq.h>

/******************************************************************************
                   Define(s) section
******************************************************************************/
#if defined(PLATFORM_ZIGBIT)
  #define MAX_NUM_LINES             2
  #define MIN_VALID_IRQ_NUMBER  IRQ_6
#else
  #define MAX_NUM_LINES             3
  #define MIN_VALID_IRQ_NUMBER  IRQ_5
#endif
/** \brief number valid interrupt. */
#define HAL_NUM_IRQ_LINES       MAX_NUM_LINES
/** \brief first valid interrupt. */
#define HAL_FIRST_VALID_IRQ     MIN_VALID_IRQ_NUMBER

/******************************************************************************
                   Types section
******************************************************************************/
/** \brief user's callback type. */
typedef void (* IrqCallback_t)(void);

/******************************************************************************
                   Prototypes section
******************************************************************************/
/**************************************************************************//**
\brief Sets configuration of pins and the registers.
\param[in]
  irqNumber - number of interrupt.
\param[in]
  irqMode - mode of interrupt.
******************************************************************************/
void halSetIrqConfig(uint8_t irqNumber, uint8_t irqMode);

/**************************************************************************//**
\brief Clears configuration of pins and the registers.
\param[in]
  irqNumber - number of interrupt.
******************************************************************************/
void halClrIrqConfig(uint8_t irqNumber);

/******************************************************************************
                   Inline static functions section
******************************************************************************/
/**************************************************************************//**
\brief Enables external interrupt
\param[in]
  irqNumber - number of external interrupt.
******************************************************************************/
INLINE void halEnableIrqInterrupt(uint8_t irqNumber)
{
  // Enable external interrupt request
  EIMSK |= (1 << irqNumber);
}

/**************************************************************************//**
\brief Disables external interrupt
\param[in]
  irqNumber - number of external interrupt.
******************************************************************************/
INLINE void halDisableIrqInterrupt(uint8_t irqNumber)
{
  // Disable external interrupt request
  EIMSK &= ~(1 << irqNumber);
}

#endif /* _HALIRQ_H */
//eof halirq.h

