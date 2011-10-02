/**************************************************************************//**
\file  battery.c

\brief This module is used for measurment battery voltage.

\author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2011, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

\internal
  History:
    29/06/07 E. Ivanov - Created
******************************************************************************/
#if APP_DISABLE_BSP != 1

/******************************************************************************
                   Includes section
******************************************************************************/
#include <battery.h>
#include <adc.h>
#include <bspTaskManager.h>
#include <types.h>

#if BSP_MNZB_EVB_SUPPORT == 1

/******************************************************************************
                   Define(s) section
******************************************************************************/
#define BATTERY_AMOUNT_DISPERSION       2
#define MAX_AMOUNT_COUNTER              20

/******************************************************************************
                   Types section
******************************************************************************/
// battery states
typedef enum
{
  IDLE,      // idle
  BUSY,      // opened and ready to be used
  DATA       // performs request
} batteryStates_t;

// battery descriptor
typedef struct
{
  uint8_t batteryData;
  BspBatteryCb_t batteryCallback; // callback
} BatteryControl_t;

/******************************************************************************
                   Global variables section
******************************************************************************/
// Monitors current state
static batteryStates_t batteryState = IDLE;
static BatteryControl_t batteryControl;
static HAL_AdcParams_t adcParam;
static uint8_t batExpectation;
static uint8_t batAmountCnt;

/******************************************************************************
                   Implementations section
******************************************************************************/
/**************************************************************************//**
\brief Callback about ADC request was completed.
\param[in]
  data - result of ADC.
******************************************************************************/
void batteryCallback(void)
{
  batAmountCnt++;
  if ((batteryControl.batteryData > (batExpectation + BATTERY_AMOUNT_DISPERSION)) ||
      (batteryControl.batteryData < (batExpectation - BATTERY_AMOUNT_DISPERSION)))
  {
    batExpectation = batteryControl.batteryData;
    batAmountCnt = 0;
  }

  if (MAX_AMOUNT_COUNTER == batAmountCnt)
  {
    batAmountCnt = 0;
    batExpectation = 0;
    bspPostTask(BSP_BATTERY);
    return;
  }

  batteryState = BUSY;
  readBatteryData(batteryControl.batteryCallback);
}

/**************************************************************************//**
\brief Opens the component to use.
\return
  BC_SUCCESS - the component is ready to been use. \n
  BC_FAIL - otherwise.
******************************************************************************/
result_t openBattery(void)
{
  if (IDLE == batteryState)
  {
    adcParam.bufferPointer = &(batteryControl.batteryData);
    adcParam.callback = batteryCallback;
    adcParam.resolution = RESOLUTION_8_BIT;
    adcParam.sampleRate = ADC_4800SPS;
    adcParam.selectionsAmount = 1;
    adcParam.voltageReference = AREF;
    batteryState = BUSY;
    HAL_OpenAdc(&adcParam);
    return BC_SUCCESS;
  }
  return BC_FAIL;
}

/**************************************************************************//**
\brief  Closes component.
\return
  BC_SUCCESS - always.
******************************************************************************/
result_t closeBattery(void)
{
  if (IDLE == batteryState)
    return BC_FAIL;
  batteryState = IDLE;
  HAL_CloseAdc();
  return BC_SUCCESS;
}

/**************************************************************************//**
\brief  Starts ADC request on battery channel.
\param[in]
    data - sensor data.
\return
    BC_FAIL - battery component was not opened. \n
    BC_SUCCESS - other case.
******************************************************************************/
result_t readBatteryData(BspBatteryCb_t cb)
{
  if (BUSY != batteryState)
    return BC_FAIL;
  batteryState = DATA;
  batteryControl.batteryCallback = cb;
  HAL_ReadAdc(HAL_ADC_CHANNEL0);
  return BC_SUCCESS;
}

/**************************************************************************//**
\brief BSP battery handler.
******************************************************************************/
void bspBatteryHandler(void)
{
  int16_t value;

  // 1250 - 1.25 V reference voltage expressed in mV (1250 mV)
  // 3 - battery voltage is divided by 3 on the MeshBean board
  // 256 - steps for 8 bit ADC
  // Resulting value is a true battery voltage expressed in mV
  value = (batteryControl.batteryData * 1250ul * 3ul) / 256ul;

  batteryState = BUSY;
  batteryControl.batteryCallback(value);
}
#endif /* BSP_MNZB_EVB_SUPPORT */

#endif // APP_DISABLE_BSP != 1

// eof battery.c
