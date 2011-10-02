/**************************************************************************//**
  \file  irq.c

  \brief Implementation of IRQ interface.

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

/******************************************************************************
                   Includes section
******************************************************************************/
#include <halIrq.h>

/******************************************************************************
                   External global variables section
******************************************************************************/
extern IrqCallback_t IrqCallbackList[HAL_NUM_IRQ_LINES];

/******************************************************************************
                   Implementations section
******************************************************************************/
/******************************************************************************
 Registers user's irqNumber interrupt
 Parameters:
   irqNumber - IRQ number
   irqMode   - Interrupt sence control
   f         - user's interrupt handler. Handler must be executed less than 100 us.
 Returns:
  -1 - if irqNumber is out of range, not valid irq mode, invalid interrupt handler
       or such interrupt has been already registered.
   0 - otherwise.
******************************************************************************/
int HAL_RegisterIrq(HAL_IrqNumber_t irqNumber, HAL_IrqMode_t irqMode, void (*f)(void))
{
  uint8_t irqOffsetNumber = irqNumber - HAL_FIRST_VALID_IRQ;

  // irqNumber is out of range
  if (irqOffsetNumber >= HAL_NUM_IRQ_LINES)
    return -1;
  // Such interrupt has been already register
  if (NULL != IrqCallbackList[irqOffsetNumber])
    return -1;
  // not valid irq mode
  if (IRQ_HIGH_LEVEL == irqMode)
    return -1;
  // invalid interrupt handler
  if (NULL == f)
    return -1;

  /* type HAL_IrqMode_t has not valid IRQ_HIGH_LEVEL state,
   * because for saving to register all states need decrement after IRQ_LOW_LEVEL state. */
  if (IRQ_LOW_LEVEL == irqMode)
    halSetIrqConfig(irqNumber, irqMode);
  else
    halSetIrqConfig(irqNumber, irqMode - 1);

  IrqCallbackList[irqOffsetNumber] = f;
  return 0;
}

/******************************************************************************
 Enables irqNumber interrupt
 Parameters:
   irqNumber - IRQ number
 Returns:
   -1 - if irqNumber is out of range or has not been
             registered yet.
   0 - otherwise.
******************************************************************************/
int HAL_EnableIrq(HAL_IrqNumber_t irqNumber)
{
  uint8_t irqOffsetNumber = irqNumber - HAL_FIRST_VALID_IRQ;
  // irqNumber is out of range
  if (irqOffsetNumber >= HAL_NUM_IRQ_LINES)
    return -1;
  // Interrupt has not been opened yet
  if (NULL == IrqCallbackList[irqOffsetNumber])
    return -1;
  halEnableIrqInterrupt(irqNumber);
  return 0;
}

/******************************************************************************
 Disables irqNumber interrupt
 Parameters:
   irqNumber - IRQ number
 Returns:
   -1 - if irqNumber is out of range or has not been
             registered yet.
   0 - otherwise.
******************************************************************************/
int HAL_DisableIrq(HAL_IrqNumber_t irqNumber)
{
  uint8_t irqOffsetNumber = irqNumber - HAL_FIRST_VALID_IRQ;
  // irqNumber is out of range
  if (irqOffsetNumber >= HAL_NUM_IRQ_LINES)
    return -1;
  // Interrupt has not been opened yet
  if (NULL == IrqCallbackList[irqOffsetNumber])
    return -1;
  halDisableIrqInterrupt(irqNumber);
  return 0;
}

/******************************************************************************
 Unregisters user's irqNumber interrupt
 Parameters:
   irqNumber - IRQ number
 Returns:
   -1 - if irqNumber is out of range or has not been
             registered yet.
   0 - otherwise.
******************************************************************************/
int HAL_UnregisterIrq(HAL_IrqNumber_t irqNumber)
{
  uint8_t irqOffsetNumber = irqNumber - HAL_FIRST_VALID_IRQ;
  // irqNumber is out of range
  if (irqOffsetNumber >= HAL_NUM_IRQ_LINES)
    return -1;
  // Interrupt has not been opened yet
  if (NULL == IrqCallbackList[irqOffsetNumber])
    return -1;
  // Disable external interrupt request
  halDisableIrqInterrupt(irqNumber);
  halClrIrqConfig(irqNumber);
  IrqCallbackList[irqOffsetNumber] = NULL;
  return 0;
}

// eof irq.c
