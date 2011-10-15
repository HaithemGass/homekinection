/**************************************************************************//**
\file  halSpi.c

\brief Implementation of USART SPI mode.

\author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2011, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

\internal
  History:
    29/06/07 E. Ivanov - Created
******************************************************************************/
/******************************************************************************
 *   WARNING: CHANGING THIS FILE MAY AFFECT CORE FUNCTIONALITY OF THE STACK.  *
 *   EXPERT USERS SHOULD PROCEED WITH CAUTION.                                *
 ******************************************************************************/
/******************************************************************************
                   Includes section
******************************************************************************/
#include <spi.h>
#include "helpers.h"

/******************************************************************************
                   Define(s) section
******************************************************************************/
#define UDORD0                  2
#define UCPHA0                  1
#define UCPOL0                  0
#define SPI_CLOCK_MODE_AMOUNT   4
#define SPI_DATA_ORDER_AMOUNT   2

/******************************************************************************
                   Implementations section
******************************************************************************/
/******************************************************************************
Set the parameters of USART to work at SPI mode.
Parameters:
  descriptor - pointer to the spi descriptor.
Returns:
  none.
******************************************************************************/
void halSetUsartSpiConfig(HAL_SpiDescriptor_t *descriptor)
{
  uint8_t clockMode[SPI_CLOCK_MODE_AMOUNT] = {((0 << UCPOL0) | (0 << UCPHA0)),
                                              ((0 << UCPOL0) | (1 << UCPHA0)),
                                              ((1 << UCPOL0) | (0 << UCPHA0)),
                                              ((1 << UCPOL0) | (1 << UCPHA0))};
  uint8_t dataOrder[SPI_DATA_ORDER_AMOUNT] = {(0 << UDORD0),
                                              (1 << UDORD0)};

  // setting of the spi gpio direct
  if (SPI_CHANNEL_0 == descriptor->tty)
    GPIO_USART0_EXTCLK_make_out();
  else
    GPIO_USART1_EXTCLK_make_out();

  UBRRn(descriptor->tty) = 0;
  // Set MSPI mode
  UCSRnC(descriptor->tty) = (1 << UMSEL01) | (1 << UMSEL00);
  // Set clock mode and data order
  UCSRnC(descriptor->tty) |= (dataOrder[descriptor->dataOrder] | clockMode[descriptor->clockMode]);
  // Enable receiver and transmitter
  UCSRnB(descriptor->tty) = (1 << RXEN0) | (1 << TXEN0);
  // Set baud rate
  UBRRn(descriptor->tty) = descriptor->baudRate;
}

/******************************************************************************
Disables USART channel.
Parameters:
  tty  -  spi channel.
******************************************************************************/
void halClearUsartSpi(SpiChannel_t tty)
{
  if (SPI_CHANNEL_0 == tty)
    GPIO_USART0_EXTCLK_make_in();
  else
    GPIO_USART1_EXTCLK_make_in();

  UCSRnB(tty) = 0x00; // disable
}

/******************************************************************************
Write a length bytes to the SPI.
Parameters:
  tty    -  spi channel
  buffer -  pointer to application data buffer;
  length -  number bytes for transfer;
Returns:
  number of written bytes
******************************************************************************/
uint16_t halSyncUsartSpiWriteData(SpiChannel_t tty, uint8_t *buffer, uint16_t length)
{
  uint16_t i;
  uint8_t temp;

  for (i = 0; i < length; i++)
  {
    // Wait for empty transmit buffer
    while (!(UCSRnA(tty) & (1 << UDRE0)));
    // Send data
    UDRn(tty) = *(buffer + i);
    // Wait for data to be received
    while (!(UCSRnA(tty) & (1 << RXC0)));
    // receives data to clear received usart buffer
    temp = UDRn(tty);
    (void)temp;
  }
  return i;
}

/******************************************************************************
Write & read a length bytes to & from the SPI.
Parameters:
  tty    -  spi channel
  buffer -  pointer to application data buffer;
  length -  number bytes for transfer;
Returns:
  number of written & read bytes
******************************************************************************/
uint16_t halSyncUsartSpiReadData(SpiChannel_t tty, uint8_t *buffer, uint16_t length)
{
  uint16_t i;  
  for (i = 0; i < length; i++)
  {
    // Wait for empty transmit buffer
    while (!(UCSRnA(tty) & (1 << UDRE0)));
    // Send data
    UDRn(tty) = *(buffer + i);
    // Wait for data to be received
    while (!(UCSRnA(tty) & (1 << RXC0)));
    // Receive data
    *(buffer + i) = UDRn(tty);
  }
  return i;
}

//end of halSpi.c
