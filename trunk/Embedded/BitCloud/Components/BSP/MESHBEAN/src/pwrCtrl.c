/***************************************************************************//**
\file  pwrCtrl.c

\brief The module to control the power on periphery.

\author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2011, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

\internal
  History:
    29/05/07 E. Ivanov - Created
*******************************************************************************/
/******************************************************************************
                   Includes section
******************************************************************************/
#include <types.h>
#include <pwrCtrl.h>
#include <gpio.h>

#if BSP_MNZB_EVB_SUPPORT == 1
/******************************************************************************
                    Prototypes section
******************************************************************************/
/**************************************************************************//**
\brief Checks if amplifier is used.
\return true - is used, \n
 false - is not used.
******************************************************************************/
bool HAL_IsAmplifierUsed(void);

/******************************************************************************
                   Global variables section
******************************************************************************/
uint8_t bspPowerControl = 0;

/******************************************************************************
                   Implementations section
******************************************************************************/
/**************************************************************************//**
\brief  Powers on periphery.

\param[in]
  id - periphery id.
******************************************************************************/
void bspOnPeriphery(uint8_t id)
{
  if (!bspPowerControl)
  {
    GPIO_8_make_out();

    if (HAL_IsAmplifierUsed())
      GPIO_8_clr();
    else
      GPIO_8_set();

    GPIO_7_make_out();
    GPIO_7_set();
  }
  bspPowerControl |= (1 << id);
}


/**************************************************************************//**
\brief  Powers off periphery.

\param[in]
  id - periphery id.
******************************************************************************/
void bspOffPeriphery(uint8_t id)
{
  bspPowerControl &= ~(1 << id);
  if (bspPowerControl)
    return;

  GPIO_8_make_out();

  if (HAL_IsAmplifierUsed())
    GPIO_8_set();
  else
    GPIO_8_clr();

  GPIO_7_make_out();
  GPIO_7_clr();
}

#endif /* BSP_MNZB_EVB_SUPPORT */
// eof pwrCtrl.c
