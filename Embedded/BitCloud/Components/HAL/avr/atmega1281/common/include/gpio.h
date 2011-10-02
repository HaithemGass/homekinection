/***************************************************************************//**
  \file  gpio.h

  \brief Implementation of gpio defines.

  \author
      Atmel Corporation: http://www.atmel.com \n
      Support email: avr@atmel.com

    Copyright (c) 2008-2011, Atmel Corporation. All rights reserved.
    Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
    History:
      4/12/08 A. Khromykh - Created
 ******************************************************************************/
/******************************************************************************
 *   WARNING: CHANGING THIS FILE MAY AFFECT CORE FUNCTIONALITY OF THE STACK.  *
 *   EXPERT USERS SHOULD PROCEED WITH CAUTION.                                *
 ******************************************************************************/

#ifndef _GPIO_H
#define _GPIO_H

/******************************************************************************
                   Includes section
******************************************************************************/
// \cond
#include <types.h>
// \endcond

/******************************************************************************
                   Define(s) section
******************************************************************************/
/******************************************************************************
* void gpioX_set() sets GPIOX pin to logical 1 level.
* void gpioX_clr() clears GPIOX pin to logical 0 level.
* void gpioX_make_in makes GPIOX pin as input.
* void gpioX_make_in makes GPIOX pin as output.
* uint8_t gpioX_read() returns logical level GPIOX pin.
* uint8_t gpioX_state() returns configuration of GPIOX port.
*******************************************************************************/
#define HAL_ASSIGN_PIN(name, port, bit) \
INLINE void  GPIO_##name##_set()         {PORT##port |= (1 << bit);} \
INLINE void  GPIO_##name##_clr()         {PORT##port &= ~(1 << bit);} \
INLINE uint8_t  GPIO_##name##_read()     {return (PIN##port & (1 << bit)) != 0;} \
INLINE uint8_t  GPIO_##name##_state()    {return (DDR##port & (1 << bit)) != 0;} \
INLINE void  GPIO_##name##_make_out()    {DDR##port |= (1 << bit);} \
INLINE void  GPIO_##name##_make_in()     {DDR##port &= ~(1 << bit); PORT##port &= ~(1 << bit);} \
INLINE void  GPIO_##name##_make_pullup() {PORT##port |= (1 << bit);}\
INLINE void  GPIO_##name##_toggle()      {PORT##port ^= (1 << bit);}

/******************************************************************************
                   Inline static functions section
******************************************************************************/
// the macros for the manipulation by GPIO0
HAL_ASSIGN_PIN(0, B, 5);
// the macros for the manipulation by GPIO1
HAL_ASSIGN_PIN(1, B, 6);
// the macros for the manipulation by GPIO2
HAL_ASSIGN_PIN(2, B, 7);
// the macros for the manipulation by GPIO3
HAL_ASSIGN_PIN(3, G, 0);
// the macros for the manipulation by GPIO4
HAL_ASSIGN_PIN(4, G, 1);
// the macros for the manipulation by GPIO5
HAL_ASSIGN_PIN(5, G, 2);
// the macros for the manipulation by GPIO6
HAL_ASSIGN_PIN(6, D, 6);
// the macros for the manipulation by GPIO7
HAL_ASSIGN_PIN(7, D, 7);
// the macros for the manipulation by GPIO8
HAL_ASSIGN_PIN(8, E, 3);

// macroses only for STK500
// the macros for the manipulation by GPIO9
HAL_ASSIGN_PIN(9,  C, 0);
// the macros for the manipulation by GPIO10
HAL_ASSIGN_PIN(10, C, 1);
// the macros for the manipulation by GPIO11
HAL_ASSIGN_PIN(11, C, 2);
// the macros for the manipulation by GPIO12
HAL_ASSIGN_PIN(12, C, 3);
// the macros for the manipulation by GPIO13
HAL_ASSIGN_PIN(13, C, 4);
// the macros for the manipulation by GPIO14
HAL_ASSIGN_PIN(14, C, 5);
// the macros for the manipulation by GPIO15
HAL_ASSIGN_PIN(15, C, 6);
// the macros for the manipulation by GPIO16
HAL_ASSIGN_PIN(16, C, 7);
// macroses only for STK500

// macroses only for Rcb
// the macros for the manipulation by GPIOE2
HAL_ASSIGN_PIN(E2, E, 2);
// the macros for the manipulation by GPIOE3
HAL_ASSIGN_PIN(E3, E, 3);
// the macros for the manipulation by GPIOE4
HAL_ASSIGN_PIN(E4, E, 4);
// the macros for the manipulation by GPIOE5
HAL_ASSIGN_PIN(E5, E, 5);
// macroses only for Rcb

// the macros for the manipulation by GPIO_I2C_CLK
HAL_ASSIGN_PIN(I2C_CLK, D, 0);
// the macros for the manipulation by GPIO_I2C_DATA
HAL_ASSIGN_PIN(I2C_DATA, D, 1);
// the macros for the manipulation by GPIO_USART1_TXD
HAL_ASSIGN_PIN(USART1_TXD, D, 2);
// the macros for the manipulation by GPIO_USART1_RXD
HAL_ASSIGN_PIN(USART1_RXD, D, 3);
// the macros for the manipulation by GPIO_USART1_EXTCLK
HAL_ASSIGN_PIN(USART1_EXTCLK, D, 5);
// the macros for the manipulation by GPIO_USART_RTS
HAL_ASSIGN_PIN(USART_RTS, D, 4);
// the macros for the manipulation by GPIO_USART_CTS
HAL_ASSIGN_PIN(USART_CTS, D, 5);
// the macros for the manipulation by GPIO_ADC_INPUT_3
HAL_ASSIGN_PIN(ADC_INPUT_3, F, 3);
// the macros for the manipulation by GPIO_ADC_INPUT_2
HAL_ASSIGN_PIN(ADC_INPUT_2, F, 2);
// the macros for the manipulation by GPIO_ADC_INPUT_1
HAL_ASSIGN_PIN(ADC_INPUT_1, F, 1);
// the macros for the manipulation by GPIO_BAT
HAL_ASSIGN_PIN(BAT, F, 0);
// the macros for the manipulation by GPIO_1WR
HAL_ASSIGN_PIN(1WR, G, 5);
// the macros for the manipulation by GPIO_USART_DTR
HAL_ASSIGN_PIN(USART_DTR, E, 4);
// the macros for the manipulation by GPIO_USART0_TXD
HAL_ASSIGN_PIN(USART0_TXD, E, 0);
// the macros for the manipulation by GPIO_USART0_RXD
HAL_ASSIGN_PIN(USART0_RXD, E, 1);
// the macros for the manipulation by GPIO_USART0_EXTCLK
HAL_ASSIGN_PIN(USART0_EXTCLK, E, 2);
// the macros for the manipulation by GPIO_IRQ_7
HAL_ASSIGN_PIN(IRQ_7, E, 7);
// the macros for the manipulation by GPIO_IRQ_6
HAL_ASSIGN_PIN(IRQ_6, E, 6);

#ifdef _HAL_USE_AMPLIFIER_
  // the macros for the manipulation sleep power amplifier
  HAL_ASSIGN_PIN(POW_AMPLF_SLP, C, 1);
#endif

#endif /* _GPIO_H */
