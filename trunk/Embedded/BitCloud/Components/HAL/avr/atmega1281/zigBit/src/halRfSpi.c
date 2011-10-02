/**************************************************************************//**
  \file  halrfSpi.c

  \brief SPI interface routines.

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

#include <halRfSpi.h>
#include <halRfPio.h>

/******************************************************************************
  Performs initialization of SPI interface.
******************************************************************************/
void HAL_InitRfSpi(void)
{
  GPIO_SPI_CS_set();
  GPIO_SPI_MISO_make_in();
  GPIO_SPI_MOSI_make_out();
  GPIO_SPI_SCK_make_out();
  GPIO_SPI_CS_make_out();
  SPCR = ((1 << SPE) | (1 << MSTR));               // SPI enable, master mode.
  SPSR = (1 << SPI2X);                             // rate = fosc/2
}

/******************************************************************************
  Writes/reads byte to/from SPI. CPU clock critical function (4 MHz only).
  parameters: value - byte to write.
  Returns:    the byte which was read.
******************************************************************************/
uint8_t HAL_WriteByteRfSpi(uint8_t value)
{
  uint8_t tmp8;

  SPDR = value; // Write data.
  asm("nop"); // 1
  asm("nop"); // 2
  asm("nop"); // 3
  asm("nop"); // 4
  asm("nop"); // 5
  asm("nop"); // 5
  asm("nop"); // 7
  asm("nop"); // 8
  asm("nop"); // 9
  asm("nop"); // 10
  asm("nop"); // 11
  asm("nop"); // 12
  asm("nop"); // 13
  asm("nop"); // 14
  asm("nop"); // 15
  asm("nop"); // 16
  asm("nop"); // 17
  tmp8 = SPSR;
  (void)tmp8;
  return SPDR;
}

/******************************************************************************
  Deselects a slave device.
******************************************************************************/
void HAL_DeselectRfSpi(void)
{
  GPIO_SPI_CS_set();
}

/******************************************************************************
  Selects a slave device.
******************************************************************************/
void HAL_SelectRfSpi(void)
{
  GPIO_SPI_CS_clr();
}

// eof halrfSpi.c
