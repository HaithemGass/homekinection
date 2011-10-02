/***************************************************************************//**
\file  leds.c

\brief The module to access to the leds.

\author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2011, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

\internal
  History:
    05.08.09 A. Taradov - Created
*******************************************************************************/
#if APP_DISABLE_BSP != 1

/******************************************************************************
                   Includes section
******************************************************************************/
#include <bspLeds.h>

/******************************************************************************
                   Implementations section
******************************************************************************/

/**************************************************************************//**
\brief Opens leds module to use.

\return
    operation state
******************************************************************************/
result_t BSP_OpenLeds(void)
{
  halInitFirstLed();
  halInitSecondLed();
  halInitThirdLed();
  return BC_SUCCESS;
}

/**************************************************************************//**
\brief Closes leds module.

\return
    operation state
******************************************************************************/
result_t BSP_CloseLeds(void)
{
  halUnInitFirstLed();
  halUnInitSecondLed();
  halUnInitThirdLed();
  return BC_SUCCESS;
}

/**************************************************************************//**
\brief Turns the LED on.

\param[in]
    id - number of led
******************************************************************************/
void BSP_OnLed(uint8_t id)
{
  switch (id)
  {
    case LED_FIRST:
      halOnFirstLed();
      break;
    case LED_SECOND:
      halOnSecondLed();
      break;
    case LED_THIRD:
      halOnThirdLed();
      break;
  }
}

/**************************************************************************//**
\brief Turns the LED off.

\param[in]
      id - number of led
******************************************************************************/
void BSP_OffLed(uint8_t id)
{
  switch (id)
  {
    case LED_FIRST:
      halOffFirstLed();
      break;
    case LED_SECOND:
      halOffSecondLed();
      break;
    case LED_THIRD:
      halOffThirdLed();
      break;
  }
}

/**************************************************************************//**
\brief Toggles LED state.

\param[in]
      id - number of led
******************************************************************************/
void BSP_ToggleLed(uint8_t id)
{
  switch (id)
  {
    case LED_FIRST:
      halToggleFirstLed();
      break;
    case LED_SECOND:
      halToggleSecondLed();
      break;
    case LED_THIRD:
      halToggleThirdLed();
      break;
  }
}

#endif // APP_DISABLE_BSP != 1

// eof leds.c
