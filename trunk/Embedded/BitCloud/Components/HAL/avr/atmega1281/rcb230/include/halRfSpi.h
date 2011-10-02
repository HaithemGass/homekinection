/***************************************************************************//**
  \file  halRfSpi.h

  \brief SPI interface routines header.

  \author
      Atmel Corporation: http://www.atmel.com \n
      Support email: avr@atmel.com

    Copyright (c) 2008-2011, Atmel Corporation. All rights reserved.
    Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
    History:
      29/05/07 A. Luzhetsky - Created
 ******************************************************************************/
/******************************************************************************
 *   WARNING: CHANGING THIS FILE MAY AFFECT CORE FUNCTIONALITY OF THE STACK.  *
 *   EXPERT USERS SHOULD PROCEED WITH CAUTION.                                *
 ******************************************************************************/

#ifndef _HALRFSPI_H
#define _HALRFSPI_H

/******************************************************************************
                        Includes section.
******************************************************************************/
#include <types.h>
#include <atomic.h>
#include <halDbg.h>
#include <halDiagnostic.h>

/******************************************************************************
                    Prototypes section
******************************************************************************/
/******************************************************************************
  Performs initialization of SPI interface.
******************************************************************************/
void HAL_InitRfSpi(void);

/******************************************************************************
  Writes/reads byte to/from SPI.
  parameters: value - byte to write.
  Returns:    the byte which was read.
******************************************************************************/
uint8_t HAL_WriteByteRfSpi(uint8_t value);

/******************************************************************************
  Deselects a slave device.
******************************************************************************/
void HAL_DeselectRfSpi(void);

/******************************************************************************
  Selects a slave device.
******************************************************************************/
void HAL_SelectRfSpi(void);

/******************************************************************************
  Inline function (to use in critical sections)
  Writes/reads byte to/from SPI.
  parameters: value - byte to write.
  Returns:    the byte which was read.
******************************************************************************/
INLINE uint8_t HAL_WriteByteInlineRfSpi(uint8_t value)
{
  ATOMIC_SECTION_ENTER
  BEGIN_MEASURE
    SPDR = value; // Write data.
    asm("nop");   // This "nop" tunes up the "while" to reduce time for SPIF flag
                  // detecting.
    while (!(SPSR&(1 << SPIF)));
  END_MEASURE(HALATOM_WRITEBYTE_RFSPI_TIME_LIMIT)
  ATOMIC_SECTION_LEAVE
  return SPDR;
}


#endif /* _HALRFSPI_H */

//eof halRfSpi.h
