/**************************************************************************//**
  \file  adc.c

  \brief Implementation of ADC interface.

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
#include <types.h>
#include <halAdc.h>

/******************************************************************************
                   Types section
******************************************************************************/
typedef enum
{
  IDLE,      // idle
  DATA,      // performs request
  BUSY       // the module is ready to start conversion
} AdcStates_t;

typedef struct
{
  void (*callback)(void); // address of callback
} HalAdcControl_t;

/******************************************************************************
                   Global variables section
******************************************************************************/
AdcStates_t halAdcState = IDLE; // Monitors current state
HalAdcControl_t halAdcControl;

/******************************************************************************
                   Implementations section
******************************************************************************/
/******************************************************************************
Opens the ADC to make the measuring on a ADC channel.
Parameters:
  param - pointer to parameter structure
Returns:
  -1 - unsupported parameter or ADC is busy.
   0 - on success.
******************************************************************************/
int HAL_OpenAdc(HAL_AdcParams_t *param)
{
  if (IDLE != halAdcState)
    return -1;
  if (NULL == param)
    return -1;
  if (NULL == param->bufferPointer)
    return -1;
  if (param->resolution > RESOLUTION_10_BIT)
    return -1;
  /* unsupported voltage reference */
  if (param->voltageReference & 0x3F)
    return -1;
  /* adc speed must be only 9600 or 4800 SPS for 10 bit resolution */
  if ((RESOLUTION_10_BIT == param->resolution) && (param->sampleRate < ADC_9600SPS))
    return -1;

  halAdcState = BUSY;
  halOpenAdc(param);
  halAdcControl.callback = param->callback;
  return 0;
}

/******************************************************************************
Starts ADC with the parameters that were defined at HAL_OpenAdc.
Parameters:
  channel - number of channel
Returns:
  -1 - the ADC was not opened, unsupported channel number.
   0 - on success.
******************************************************************************/
int HAL_ReadAdc(HAL_AdcChannelNumber_t channel)
{
  if (BUSY != halAdcState)
    return -1;
  if (((channel > HAL_ADC_CHANNEL3) && (channel < HAL_ADC_DIFF_CHANNEL0)) || (channel > HAL_ADC_DIFF_CHANNEL7))
    return -1;

  halAdcState = DATA;
  halStartAdc(channel);
  return 0;
}

/******************************************************************************
Closes the ADC.
Parameters:
  none.
Returns:
  -1  - the module was not opened to be used.
   0  - on success.
******************************************************************************/
int HAL_CloseAdc(void)
{
  if (IDLE == halAdcState)
    return -1;

  halAdcState = IDLE;
  halCloseAdc();
  return 0;
}

/******************************************************************************
 ADC interrupt handler.
******************************************************************************/
void halSigAdcHandler(void)
{
  if (DATA == halAdcState)
  {
    halAdcState = BUSY;
    if (NULL != halAdcControl.callback)
      halAdcControl.callback();
  }
}
// eof adc.c
