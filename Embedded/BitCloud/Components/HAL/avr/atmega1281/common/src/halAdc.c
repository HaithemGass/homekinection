/**************************************************************************//**
  \file  halAdc.c

  \brief Implementation of hardware depended ADC interface.

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
#include <halAdc.h>
#include <halW1.h>
#include <halDbg.h>
#include <halDiagnostic.h>

/******************************************************************************
                   Defines section
******************************************************************************/
#define ALL_CHANNEL_MASK          0x1F
#define CHANNEL_MASK_1            0x01
#define CHANNEL_MASK_2            0x03
#define CHANNEL_MASK_3            0x04
#define CHANNEL_MASK_4            0x0C
#define DELAY_FOR_STABILIZE       125

/******************************************************************************
                   Constants section
******************************************************************************/
#if F_CPU == 4000000
  PROGMEM_DECLARE(const uint8_t halAdcDivider[5]) = {2, 3, 4, 5, 6};
#elif F_CPU == 8000000
  PROGMEM_DECLARE(const uint8_t halAdcDivider[5]) = {3, 4, 5, 6, 7};
#endif

/******************************************************************************
                   Global variables section
******************************************************************************/
static volatile uint8_t halAdcResolution = RESOLUTION_8_BIT;
static volatile void *halAdcDataPointer = NULL;
static volatile uint16_t halAdcCurCount = 0;
static volatile uint16_t halAdcMaxCount = 0;

/******************************************************************************
                   Implementations section
******************************************************************************/
/******************************************************************************
Initializations the ADC.
Parameters:
  param - pointer to parameter structure
Returns:
  none.
******************************************************************************/
void  halOpenAdc(HAL_AdcParams_t *param)
{
  halAdcMaxCount = param->selectionsAmount;
  halAdcResolution = param->resolution;
  halAdcDataPointer = param->bufferPointer;

  /* sets voltage reference */
  ADMUX = param->voltageReference;
  /* Enable left adjust result */
  if (RESOLUTION_8_BIT == halAdcResolution)
    ADMUX |= (1 << ADLAR);

  uint8_t tmp;
  memcpy_P(&tmp, &(halAdcDivider[param->sampleRate]), 1);
  ADCSRA = tmp | (1 << ADEN);
}

/******************************************************************************
Starts convertion on the ADC channel.
Parameters:
  channel - channel number.
Returns:
  none.
******************************************************************************/
void halStartAdc(uint8_t channel)
{
  halAdcCurCount = 0;
  /* disable digital buffers */
  if (HAL_ADC_CHANNEL3 >= channel)
  {
    DIDR0 = (1 << channel);
  }
  else
  {
    if ((HAL_ADC_DIFF_CHANNEL0 == channel) || (HAL_ADC_DIFF_CHANNEL2 == channel))
      DIDR0 = CHANNEL_MASK_1;
    else if ((HAL_ADC_DIFF_CHANNEL1 == channel) || (HAL_ADC_DIFF_CHANNEL3 == channel))
      DIDR0 = CHANNEL_MASK_2;
    else if ((HAL_ADC_DIFF_CHANNEL4 == channel) || (HAL_ADC_DIFF_CHANNEL6 == channel))
      DIDR0 = CHANNEL_MASK_3;
    else if ((HAL_ADC_DIFF_CHANNEL5 == channel) || (HAL_ADC_DIFF_CHANNEL7 == channel))
      DIDR0 = CHANNEL_MASK_4;
  }

  uint8_t tmp = ADMUX & ALL_CHANNEL_MASK;

  /* clear previous channel number */
  ADMUX &= ~ALL_CHANNEL_MASK;
  /* set current channel number */
  ADMUX |= channel;

  /* if new differential channel is settled then must make 125 us delay for gain stabilize. */
  if ((tmp != channel) && (HAL_ADC_CHANNEL3 < channel))
    __delay_us(DELAY_FOR_STABILIZE);

  if (halAdcMaxCount > 1)
    ADCSRA |= ((1 << ADIE)  | (1 << ADATE) | (1 << ADSC));  // Starts running mode
  else
    ADCSRA |= ((1 << ADIE) | (1 << ADSC)); // Starts one conversion
}

/******************************************************************************
Closes the ADC.
Parameters:
  none
Returns:
  none
******************************************************************************/
void halCloseAdc(void)
{
  ADMUX  = 0;
  ADCSRA = 0;
  // Digital input enable
  DIDR0 = 0;
}

/******************************************************************************
ADC conversion complete interrupt handler.
******************************************************************************/
ISR(ADC_vect)
{
  BEGIN_MEASURE
  // Read ADC conversion result
  if (RESOLUTION_8_BIT == halAdcResolution)
    ((uint8_t *)halAdcDataPointer)[halAdcCurCount++] = ADCH;
  else
    ((uint16_t *)halAdcDataPointer)[halAdcCurCount++] = ADC;

  if (halAdcCurCount == halAdcMaxCount)
  {
    // Disable ADC Interrupt
    ADCSRA &= ~(1 << ADIE);
    halSigAdcInterrupt();
  }
  END_MEASURE(HALISR_ADC_TIME_LIMIT)
}
// eof halAdc.c
