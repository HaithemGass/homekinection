/**************************************************************************//**
\file  buttons.c

\brief Implementation of buttons interface.

\author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2011, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

\internal
  History:
    30/05/08 A. Khromykh - Created
*******************************************************************************/
#if APP_DISABLE_BSP != 1

/******************************************************************************
                   Includes section
******************************************************************************/
#include <types.h>
#include <buttons.h>
#include <irq.h>
#include <appTimer.h>
#include <bspTaskManager.h>
#include <gpio.h>

/******************************************************************************
                   Define(s) section
******************************************************************************/
#define BSP_readKEY0() GPIO_IRQ_6_read()
#define BSP_readKEY1() GPIO_IRQ_7_read()
#define PRESSED           1
#define RELEASED          0
#define BSP_BUTTONS_IDLE  0
#define BSP_BUTTONS_BUSY  1

/******************************************************************************
                   Types section
******************************************************************************/
typedef struct
{
  uint8_t currentState0 : 1;
  uint8_t currentState1 : 1;
  uint8_t wasPressed0   : 1;
  uint8_t wasPressed1   : 1;
  uint8_t waitReleased0 : 1;
  uint8_t waitReleased1 : 1;
} BSP_buttonsAction_t;

/******************************************************************************
                   Prototypes section
******************************************************************************/
/**************************************************************************//**
\brief  HAL's event handlers about KEY 0 has changed state.
******************************************************************************/
void bspKey0InterruptHandler(void);

/**************************************************************************//**
\brief  HAL's event handlers about KEY 1 has changed state.
******************************************************************************/
void bspKey1InterruptHandler(void);

/******************************************************************************
                   Global variables section
******************************************************************************/
static uint8_t state = BSP_BUTTONS_IDLE;
static volatile BSP_buttonsAction_t buttonsAction;
static BSP_ButtonsEventFunc_t bspButtonPressHandle;   // callback
static BSP_ButtonsEventFunc_t bspButtonReleaseHandle; // callback

/******************************************************************************
                   Implementations section
******************************************************************************/
/**************************************************************************//**
\brief Initializes buttons module.
******************************************************************************/
static void bspInitButtons(void)
{
  HAL_RegisterIrq(IRQ_6, IRQ_LOW_LEVEL, bspKey0InterruptHandler);
  HAL_RegisterIrq(IRQ_7, IRQ_LOW_LEVEL, bspKey1InterruptHandler);

  if (BSP_readKEY0())
    buttonsAction.currentState0 = RELEASED;
  else
    buttonsAction.currentState0 = PRESSED;

  if (BSP_readKEY1())
    buttonsAction.currentState1 = RELEASED;
  else
    buttonsAction.currentState1 = PRESSED;

  HAL_EnableIrq(IRQ_6);
  HAL_EnableIrq(IRQ_7);
}

/**************************************************************************//**
\brief Registers handlers for button events.

\param[in]
    pressed - the handler to process pressing the button
\param[in]
    released - the handler to process releasing the button
\param[in]
    bn - button number.
\return
  BC_FAIL - buttons module is busy, \n
  BC_SUCCESS in other case.
******************************************************************************/
result_t BSP_OpenButtons(void (*pressed)(uint8_t bn), void (*released)(uint8_t bn))
{
  if (state != BSP_BUTTONS_IDLE)
    return BC_FAIL;
  state = BSP_BUTTONS_BUSY;
  bspButtonPressHandle = pressed;
  bspButtonReleaseHandle = released;
  bspInitButtons();
  return BC_SUCCESS;
};

/**************************************************************************//**
\brief Cancel buttons handlers.
\return
  BC_FAIL - buttons module was not opened, \n
  BC_SUCCESS in other case.
******************************************************************************/
result_t BSP_CloseButtons(void)
{
  if (state != BSP_BUTTONS_BUSY)
    return BC_FAIL;
  HAL_UnregisterIrq(IRQ_6);
  HAL_UnregisterIrq(IRQ_7);
  bspButtonPressHandle = NULL;
  bspButtonReleaseHandle = NULL;
  state = BSP_BUTTONS_IDLE;
  return BC_SUCCESS;
};

/**************************************************************************//**
\brief Reads state of buttons.

\return
    Current buttons state in a binary way. \n
    Bit 0 defines state of the button 1, \n
    bit 1 defines state of the button 2.
******************************************************************************/
uint8_t BSP_ReadButtonsState(void)
{
  uint8_t state = 0;

  if (buttonsAction.currentState0)
    state = 0x01;

  if (buttonsAction.currentState1)
    state |= 0x02;

  return state;
}

/**************************************************************************//**
\brief  HAL's event about KEY has changed state.
******************************************************************************/
void bspKey0InterruptHandler(void)
{
  HAL_DisableIrq(IRQ_6);
  buttonsAction.currentState0 = PRESSED;
  buttonsAction.wasPressed0 = 1;
  bspPostTask(BSP_BUTTONS);
}

/**************************************************************************//**
\brief  HAL's event about KEY has changed state.
******************************************************************************/
void bspKey1InterruptHandler(void)
{
  HAL_DisableIrq(IRQ_7);
  buttonsAction.currentState1 = PRESSED;
  buttonsAction.wasPressed1 = 1;
  bspPostTask(BSP_BUTTONS);
}

/**************************************************************************//**
\brief  BSP's event about KEY has changed state.
******************************************************************************/
void bspButtonsHandler(void)
{
  if (buttonsAction.wasPressed0)
  {
    buttonsAction.wasPressed0 = 0;
    buttonsAction.waitReleased0 = 1;
    if (NULL != bspButtonPressHandle)
      bspButtonPressHandle(BSP_KEY0);
  }

  if (buttonsAction.waitReleased0)
  {
    if (BSP_readKEY0())
    {
      buttonsAction.waitReleased0 = 0;
      buttonsAction.currentState0 = RELEASED;
      if (NULL != bspButtonReleaseHandle)
        bspButtonReleaseHandle(BSP_KEY0);
      HAL_EnableIrq(IRQ_6);
    }
    else
    {
      bspPostTask(BSP_BUTTONS);
    }
  }

  if (buttonsAction.wasPressed1)
  {
    buttonsAction.wasPressed1 = 0;
    buttonsAction.waitReleased1 = 1;
    if (NULL != bspButtonPressHandle)
      bspButtonPressHandle(BSP_KEY1);
  }

  if (buttonsAction.waitReleased1)
  {
    if (BSP_readKEY1())
    {
      buttonsAction.waitReleased1 = 0;
      buttonsAction.currentState1 = RELEASED;
      if (NULL != bspButtonReleaseHandle)
        bspButtonReleaseHandle(BSP_KEY1);
      HAL_EnableIrq(IRQ_7);
    }
    else
    {
      bspPostTask(BSP_BUTTONS);
    }
  }
}

#endif // APP_DISABLE_BSP != 1

// end of buttons.c
