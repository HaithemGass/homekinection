/**************************************************************************//**
  \file  halRfCtrl.c

  \brief mac pin interface implementation.

  \author
      Atmel Corporation: http://www.atmel.com \n
      Support email: avr@atmel.com

    Copyright (c) 2008-2011, Atmel Corporation. All rights reserved.
    Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
    History:
      28/05/07 ALuzhetsky - Created.
      06/08/07 A. Mandychev, E. Ivanov - Modified.
 ******************************************************************************/
/******************************************************************************
 *   WARNING: CHANGING THIS FILE MAY AFFECT CORE FUNCTIONALITY OF THE STACK.  *
 *   EXPERT USERS SHOULD PROCEED WITH CAUTION.                                *
 ******************************************************************************/

/******************************************************************************
                        Includes section.
******************************************************************************/
#include <halClkCtrl.h>
#include <halW1.h>
#include <halRfPio.h>
#include <halRfSpi.h>
#include <halRfCtrl.h>
#include <halClkCtrl.h>

/******************************************************************************
                   Define(s) section
******************************************************************************/
#define RF_REGISTER_READ_ACCESS_MODE      0x80
#define RF_REGISTER_TRX_STATUS_ADDRESS    0x01
#define RF_TRX_OFF_STATE                  0x08

/******************************************************************************
                   Prototypes section
******************************************************************************/
#ifdef _HAL_RF_RX_TX_INDICATOR_
/**************************************************************************//**
\brief  Turn on pin 1 (DIG3) and pin 2 (DIG4) to indicate the transmit state of
the radio transceiver.
******************************************************************************/
void phyRxTxSwitcherOn(void);

#endif //_HAL_RF_RX_TX_INDICATOR_

/******************************************************************************
                    Implementations section
******************************************************************************/
/******************************************************************************
  Delay in us
******************************************************************************/
void HAL_Delay(uint8_t us)
{
  __delay_us(us);
}

/******************************************************************************
  Clears the irq.
******************************************************************************/
void HAL_ClearRfIrqFlag(void)
{
  TIFR1 = 1 << ICF1;
}

/******************************************************************************
  Enables the irq.
******************************************************************************/
void HAL_EnableRfIrq(void)
{
  TIMSK1 |= 1 << ICIE1;
}

/******************************************************************************
  Disables the irq.
******************************************************************************/
uint8_t HAL_DisableRfIrq(void)
{
  uint8_t tmp;

  tmp = TIMSK1 & (1 << ICIE1);
  TIMSK1 &= ~(1 << ICIE1);
  return tmp;
}

/******************************************************************************
  Sets SLP_TR pin to 1.
******************************************************************************/
void HAL_SetRfSlpTr(void)
{
  GPIO_RF_SLP_TR_set();
}

/******************************************************************************
  Clears SLP_TR pin to 0.
******************************************************************************/
void HAL_ClearRfSlpTr(void)
{
  GPIO_RF_SLP_TR_clr();
}

/******************************************************************************
  Makes SLP_TR pin as input.
******************************************************************************/
void HAL_MakeInRfSlpTr(void)
{
  GPIO_RF_SLP_TR_make_in();
}

/******************************************************************************
  Makes SLP_TR pin as input.
******************************************************************************/
void HAL_MakeOutRfSlpTr(void)
{
  GPIO_RF_SLP_TR_make_out();
}

/******************************************************************************
  Sets RST_TR pin to 1.
******************************************************************************/
void HAL_SetRfRst(void)
{
  GPIO_RF_RST_set();
}

/******************************************************************************
  Clears RST_TR pin to 1.
******************************************************************************/
void HAL_ClearRfRst(void)
{
  GPIO_RF_RST_clr();
}

/******************************************************************************
  Init pins that controls RF chip.
******************************************************************************/
void HAL_InitRfPins(void)
{
  GPIO_RF_TST_make_out();
  GPIO_RF_TST_clr();

  GPIO_RF_SLP_TR_clr();
  GPIO_RF_SLP_TR_make_out();

  GPIO_RF_RST_set();
  GPIO_RF_RST_make_out();

  GPIO_RF_IRQ_make_in();
}

/******************************************************************************
  Inits Atmega IRQ pin.
******************************************************************************/
void HAL_InitRfIrq(void)
{
  OCR1A = 0;
  TCCR1B = (1 << WGM12) | (1 << CS10);   // CTC mode, clock source is not prescaled
  TCCR1B |= (1 << ICNC1) | (1 << ICES1); // input noise canceler and rising edge are enabled
}

/******************************************************************************
  Returns current frequency code.
******************************************************************************/
HalSysFreq_t HAL_GetRfFreq(void)
{
/*  if (INTERNAL_RC == halGetClockSource())
  {
    return HAL_FREQ_NOCLK;
  }
  else*/
  {
    #if (F_CPU == 4000000ul)
      return HAL_FREQ_4MHZ;
    #endif
    #if (F_CPU == 8000000ul)
      return HAL_FREQ_8MHZ;
    #endif
  }
}

/**************************************************************************//**
  \brief Wait for when radio will be waked up.

  \param none.
  \return none.
******************************************************************************/
void halWaitRadio(void)
{
  uint8_t tempValue = 0;

  do {
    HAL_SelectRfSpi();
    HAL_WriteByteRfSpi(RF_REGISTER_READ_ACCESS_MODE | RF_REGISTER_TRX_STATUS_ADDRESS);
    tempValue = HAL_WriteByteRfSpi(tempValue);
    HAL_DeselectRfSpi();
  } while(RF_TRX_OFF_STATE != tempValue);
}

/**************************************************************************//**
  \brief Enables RX TX indicator for radio if that is supported.
******************************************************************************/
void HAL_EnableRxTxSwitcher(void)
{
  #ifdef _HAL_RF_RX_TX_INDICATOR_
    phyRxTxSwitcherOn();
  #endif //_HAL_RF_RX_TX_INDICATOR_
}

//eof halRfCtrl.c
