/**************************************************************************//**
  \file  halAssert.h

  \brief Implementation of avr assert algorithm.

  \author
      Atmel Corporation: http://www.atmel.com \n
      Support email: avr@atmel.com

    Copyright (c) 2008-2011, Atmel Corporation. All rights reserved.
    Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
    History:
      18/08/08 A. Khromykh - Created
 ******************************************************************************/
/******************************************************************************
 *   WARNING: CHANGING THIS FILE MAY AFFECT CORE FUNCTIONALITY OF THE STACK.  *
 *   EXPERT USERS SHOULD PROCEED WITH CAUTION.                                *
 ******************************************************************************/

#ifndef _HALASSERT_H
#define _HALASSERT_H

#ifdef  __IAR_SYSTEMS_ICC__
#ifndef _SYSTEM_BUILD
#pragma system_include
#endif
#endif

/******************************************************************************
                   Includes section
******************************************************************************/
#include <halClkCtrl.h>
#include <wdtCtrl.h>

/******************************************************************************
                   Define(s) section
******************************************************************************/
#define DELAY_VALUE 0x000000ul

#ifdef _SYS_ASSERT_ON_

#if defined(_HAL_ASSERT_INTERFACE_UART0_)
  #define UBRRNH UBRR0H
  #define UBRRNL UBRR0L
  #define UCSRNA UCSR0A
  #define UCSRNB UCSR0B
  #define UCSRNC UCSR0C
  #define UDRN   UDR0
#elif defined(_HAL_ASSERT_INTERFACE_UART1_)
  #define UBRRNH UBRR1H
  #define UBRRNL UBRR1L
  #define UCSRNA UCSR1A
  #define UCSRNB UCSR1B
  #define UCSRNC UCSR1C
  #define UDRN   UDR1
#else
  #error " Unknown assert interface "
#endif

/******************************************************************************
                   Inline static functions section
******************************************************************************/
INLINE void halAssert(uint8_t condition, uint16_t dbgCode)
{
  if (!condition)
  {
    uint32_t delay;

    HAL_StopWdt();
    asm("cli");
    DDRB |= 0xE0;
    /* Init UART */
    UBRRNH = 0;
    if (4000000ul == HAL_ReadFreq())
      UBRRNL = 12;
    else
      UBRRNL = 25;
    UCSRNA = (1 << U2X1);
    UCSRNB = (1 << TXEN1);
    UCSRNC = (3 << UCSZ10);
    while(1)
    {
      PORTB &= ~0xE0;
      /* Send high byte of message to UART */
      while (!(UCSRNA & (1 << UDRE1)));
      UDRN = (dbgCode >> 8);
      /* Send low byte of message to UART */
      while (!(UCSRNA & (1 << UDRE1)));
      UDRN = dbgCode;
      delay = DELAY_VALUE;
      while (delay--);

      PORTB |= 0xE0;
      delay = (DELAY_VALUE / 2);
      while(delay--);
    }
  }
}

#else /* _SYS_ASSERT_ON_ */
  #define halAssert(condition, dbgCode)
#endif /* _SYS_ASSERT_ON_ */

#endif /* _HALASSERT_H */

// eof halAssert.h
