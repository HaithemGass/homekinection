/***************************************************************************//**
  \file  halDbg.h

  \brief Declarations of hal , bsb mistake interface.

  \author
      Atmel Corporation: http://www.atmel.com \n
      Support email: avr@atmel.com

    Copyright (c) 2008-2011, Atmel Corporation. All rights reserved.
    Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
    History:
      09/11/07 A. Khromykh - Created
 ******************************************************************************/
/******************************************************************************
 *   WARNING: CHANGING THIS FILE MAY AFFECT CORE FUNCTIONALITY OF THE STACK.  *
 *   EXPERT USERS SHOULD PROCEED WITH CAUTION.                                *
 ******************************************************************************/

#ifndef _HALDBG_H
#define _HALDBG_H

#include <dbg.h>

/******************************************************************************
                   Define(s) section
******************************************************************************/
enum
{
  APPTIMER_MISTAKE                         = 0x2000,
  INCORRECT_EEPROM_ADDRESS                 = 0x2001,
  MEMORY_CANNOT_WRITE                      = 0x2002,
  USARTC_HALUSARTRXBUFFERFILLER_0          = 0x2003,
  USARTC_HALSIGUSARTTRANSMISSIONCOMPLETE_0 = 0x2004,
  USARTC_HALSIGUSARTRECEPTIONCOMPLETE_0    = 0x2005,
  HALUSARTH_HALCLOSEUSART_0                = 0X2006,
  HALUSARTH_HALENABLEUSARTDREMINTERRUPT_0  = 0X2007,
  HALUSARTH_HALDISABLEUSARTDREMINTERRUPT_0 = 0X2008,
  HALUSARTH_HALENABLEUSARTTXCINTERRUPT_0   = 0X2009,
  HALUSARTH_HALDISABLEUSARTTXCINTERRUPT_0  = 0X200A,
  HALUSARTH_HALENABLEUSARTRXCINTERRUPT_0   = 0X200B,
  HALUSARTH_HALDISABLEUSARTRXCINTERRUPT_0  = 0X200C,
  HALUSARTH_HALSENDUSARTBYTE_0             = 0X200D,
  USARTC_HALUSARTSAVEERRORREASON           = 0x200E,
  USARTC_HALSIGUSARTERROROCCURED_0         = 0x200F,
  USARTC_HALUNKNOWNERRORREASON_0           = 0x2010,

  HAL_USART_TX_EMPTY_LIMIT                 = 0x2FDB,
  HAL_USART_TRANS_COMPLETE_LIMIT           = 0x2FDC,
  HAL_USART_HW_CONTROLLER_LIMIT            = 0x2FDD,
  HAL_SLEEP_TIMER_SYNCHRONIZE_LIMIT        = 0x2FDE,
  HAL_GET_SLEEP_TIME_LIMIT                 = 0x2FDF,
  HALISR_EEPROM_WRITE_TIME_LIMIT           = 0x2FE0,
  HAL_APP_TIMER_SYNCHRONIZE_LIMIT          = 0x2FE1,
  HALISR_INT5_VECT_TIME_LIMIT              = 0x2FE2,
  HALISR_ADC_TIME_LIMIT                    = 0x2FE3,
  HALISR_TIMER4_COMPA_TIME_LIMIT           = 0x2FE4,
  HALATOM_SETLOWFUSES_TIME_LIMIT           = 0x2FE5,
  HALATOM_INITFREQ_TIME_LIMIT              = 0x2FE6,
  HALISR_EEPROM_READY_TIME_LIMIT           = 0x2FE7,
  HALISR_INT6_VECT_TIME_LIMIT              = 0x2FE8,
  HALISR_INT7_VECT_TIME_LIMIT              = 0x2FE9,
  HALISR_TIMER2_COMPA_TIME_LIMIT           = 0x2FEA,
  HALISR_TIMER2_OVF_TIME_LIMIT             = 0x2FEB,
  HALISR_USART0_UDR_TIME_LIMIT             = 0x2FEC,
  HALISR_USART0_TX_TIME_LIMIT              = 0x2FED,
  HALISR_USART0_RX_TIME_LIMIT              = 0x2FEE,
  HALISR_USART1_UDRE_TIME_LIMIT            = 0x2FEF,
  HALISR_USART1_TX_TIME_LIMIT              = 0x2FF0,
  HALISR_USART1_RX_TIME_LIMIT              = 0x2FF1,
  HALISR_INT4_TIME_LIMIT                   = 0x2FF2,
  HALISR_TWI_TIME_LIMIT                    = 0x2FF3,
  HALATOM_STARTWDT_TIME_LIMIT              = 0x2FF4,
  HALISR_WDT_TIME_LIMIT                    = 0x2FF5,
  HALATOM_WRITEBYTE_RFSPI_TIME_LIMIT       = 0x2FF6,
  HALISR_TIMER3_COMPA_TIME_LIMIT           = 0x2FF7,
  HALISR_PHYDISPATCH_RFINT_TIME_LIMIT      = 0x2FF8,
  HALATOM_GETTIME_OF_APPTIMER_1_TIME_LIMIT = 0x2FF9,
  HALATOM_GETTIME_OF_APPTIMER_2_TIME_LIMIT = 0x2FFA,
  HALATOM_GETTIME_OF_APPTIMER_3_TIME_LIMIT = 0x2FFB,
  HALATOM_WRITE_USART_TIME_LIMIT           = 0x2FFC,
  HALATOM_READ_USART_TIME_LIMIT            = 0x2FFD,
  HALATOM_USART_RX_COMPLETE_TIME_LIMIT     = 0x2FFE,
  HALATOM_CLEAR_TIME_CONTROL_TIME_LIMIT    = 0x2FFF
};

/******************************************************************************
                   Prototypes section
******************************************************************************/

#endif /* _HALDBG_H */

// eof halDbg.h
