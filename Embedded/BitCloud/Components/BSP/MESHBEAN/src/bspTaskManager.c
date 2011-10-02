/**************************************************************************//**
\file  bspTaskManager.c

\brief Implemenattion of BSP task manager.

\author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2011, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

\internal
  History:
    29/05/07 E. Ivanov - Created
******************************************************************************/
/******************************************************************************
                   Includes section
******************************************************************************/
#include <bspTaskManager.h>
#include <atomic.h>

/******************************************************************************
                   Prototypes section
******************************************************************************/
/**************************************************************************//**
\brief BSP button handler.
******************************************************************************/
void bspButtonsHandler(void);

/**************************************************************************//**
\brief BSP lm73 handler.
******************************************************************************/
void bspLM73Handler(void);
void bspTemperatureSensorHandler(void);

/**************************************************************************//**
\brief BSP tsl2550 handler.
******************************************************************************/
void bspTsl2550Handler(void);
void bspLightSensorHandler(void);

/**************************************************************************//**
\brief BSP battery handler.
******************************************************************************/
void bspBatteryHandler(void);
void bspEmptyBatteryHandler(void);

/******************************************************************************
                   Global variables section
******************************************************************************/
volatile uint8_t bspTaskFlags = 0;

/******************************************************************************
                   Implementations section
******************************************************************************/
/**************************************************************************//**
\brief BSP task handler.
******************************************************************************/
#if APP_DISABLE_BSP != 1

void BSP_TaskHandler(void)
{
  if (bspTaskFlags & BSP_BUTTONS)
  {
    bspTaskFlags &= (~BSP_BUTTONS);
    bspButtonsHandler();
  }
  if (bspTaskFlags & BSP_TEMPERATURE)
  {
    bspTaskFlags &= (~BSP_TEMPERATURE);
    bspLM73Handler();
  }
  if (bspTaskFlags & BSP_LIGHT)
  {
    bspTaskFlags &= (~BSP_LIGHT);
    bspTsl2550Handler();
  }
  if (bspTaskFlags & BSP_BATTERY)
  {
    bspTaskFlags &= (~BSP_BATTERY);
#if BSP_MNZB_EVB_SUPPORT == 1
    bspBatteryHandler();
#else
    bspEmptyBatteryHandler();
#endif // BSP_MNZB_EVB_SUPPORT == 1
  }
  if (bspTaskFlags)
  {
    SYS_PostTask(BSP_TASK_ID);
  }
}

#else  // APP_DISABLE_BSP != 1

void BSP_TaskHandler(void)
{
  if (bspTaskFlags & BSP_TEMPERATURE)
  {
    bspTaskFlags &= (~BSP_TEMPERATURE);
    bspTemperatureSensorHandler();
  }
  if (bspTaskFlags & BSP_LIGHT)
  {
    bspTaskFlags &= (~BSP_LIGHT);
    bspLightSensorHandler();
  }
  if (bspTaskFlags & BSP_BATTERY)
  {
    bspTaskFlags &= (~BSP_BATTERY);
    bspEmptyBatteryHandler();
  }
  if (bspTaskFlags)
  {
    SYS_PostTask(BSP_TASK_ID);
  }
} 

#endif // APP_DISABLE_BSP != 1

// eof bspTaskManager.c
