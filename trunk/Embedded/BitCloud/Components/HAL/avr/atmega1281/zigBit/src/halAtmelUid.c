/**************************************************************************//**
  \file  halAtmelUid.c

  \brief The header file describes the UID interface for Atmel MeshBean.

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2011, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
  History:
    10/12/10 A. Malkin - Created
 ******************************************************************************/
/******************************************************************************
 *   WARNING: CHANGING THIS FILE MAY AFFECT CORE FUNCTIONALITY OF THE STACK.  *
 *   EXPERT USERS SHOULD PROCEED WITH CAUTION.                                *
 ******************************************************************************/

/******************************************************************************
                   Includes section
******************************************************************************/
#include <halAtmelUid.h>
#include <halW1.h>

/******************************************************************************
                   Define(s) section
******************************************************************************/
#define UID_LEN  0x08  // size of UID (bytes)

#define SINGLE_WIRE_ZERO_BIT  0x00  // logical level zero
#define SINGLE_WIRE_ONE_BIT   0x01  // logical level one

#define BIT_DURATION  208                 // bit duration - 208 us
#define BIT_DURATION_HALF BIT_DURATION/2
#define MAX_START_BIT_WAITING     10      // waiting time(in bit) of Start bit

// Commands for Single-wire UART (Atmel MeshBean)
#define CMD_SYNCH         0xAA  // Synchronization
#define CMD_READ_MAC64_0  0x60  // Read 64-bit MAC address
#define NUM_OF_MAC64      0x00  // Number of MAC64 slot
/******************************************************************************
                   Prototypes section
******************************************************************************/
static void halWriteSingleWireBit(uint8_t value);
static void halWriteSingleWire(uint8_t value);
static int halWaitSingleWireStartBit(void);
static uint8_t halReadSingleWireBit(void);
static int halReadSingleWire(uint8_t *data);

/******************************************************************************
                   Global variables section
******************************************************************************/

/******************************************************************************
                   Implementations section
******************************************************************************/
/***************************************************************************//**
\brief Writes bit to the bus

\param[in]
    value - byte to write. The bit is placed to position of LSB.
*******************************************************************************/
static void halWriteSingleWireBit(uint8_t value)
{
  if (value)
    GPIO_SINGLE_WIRE_set();
  else
    GPIO_SINGLE_WIRE_clr();

  __delay_us(BIT_DURATION);
}

/***************************************************************************//**
\brief Writes byte to the bus

\param[in]
    value - byte to write.
*******************************************************************************/
static void halWriteSingleWire(uint8_t value)
{
  uint8_t i;

  // write Start bit
  halWriteSingleWireBit(SINGLE_WIRE_ZERO_BIT);

  // write Data
  for (i = 0; i < UID_LEN; i++)
  {
    halWriteSingleWireBit(value & 0x01);
    value >>= 1;
  }

  // add 2 Stop bits
  halWriteSingleWireBit(SINGLE_WIRE_ONE_BIT);
  halWriteSingleWireBit(SINGLE_WIRE_ONE_BIT);
}

/***************************************************************************//**
\brief Wait for Start bit from the bus.

\return
   SINGLE_WIRE_SUCCESS_STATUS - if Start bit has been found successfully; \n
   SINGLE_WIRE_ERROR_STATUS - if Start bit has been not found.
*******************************************************************************/
static int halWaitSingleWireStartBit(void)
{
  uint16_t i;

  for (i = BIT_DURATION * MAX_START_BIT_WAITING; i > 0; i--)
  {
    if (!GPIO_SINGLE_WIRE_read())
      return SINGLE_WIRE_SUCCESS_STATUS;
    __delay_us(1);
  }

  return SINGLE_WIRE_ERROR_STATUS;
}

/***************************************************************************//**
\brief Reads bit from the bus.

\return
  Read bit is placed to position of last significant bit.
*******************************************************************************/
static uint8_t halReadSingleWireBit(void)
{
  uint8_t result;

  // read pin level in half of bit period
  if (GPIO_SINGLE_WIRE_read())
    result = SINGLE_WIRE_ONE_BIT;
  else
    result = SINGLE_WIRE_ZERO_BIT;

  // wait for bit period before next bit reading
  __delay_us(BIT_DURATION);

  return result;
}

/***************************************************************************//**
\brief Reads byte from the Atmel Single-wire bus.

\param[in]
    data - byte read from the bus.

\return
    SINGLE_WIRE_SUCCESS_STATUS - if byte read without error;
    SINGLE_WIRE_ERROR_STATUS - if there are some errors during byte read.
*******************************************************************************/
static int halReadSingleWire(uint8_t *data)
{
  uint8_t reg = 0;
  uint8_t bit;
  uint8_t i;
  int result;

  // wait for Start bit of response
  result = halWaitSingleWireStartBit();

  if (result)
    return SINGLE_WIRE_ERROR_STATUS;

  // wait for half of bit period before reading Start bit
  __delay_us(BIT_DURATION_HALF);

  // read Start bit
  bit = halReadSingleWireBit();
  if (SINGLE_WIRE_ZERO_BIT != bit)
    return SINGLE_WIRE_ERROR_STATUS;

  // read byte
  for (i = 0; i < UID_LEN; i++)
  {
    if (SINGLE_WIRE_ONE_BIT == halReadSingleWireBit())
      reg |= (1 << i);
  }

  // read and check 1'st Stop bit
  bit = halReadSingleWireBit();
  if (SINGLE_WIRE_ONE_BIT != bit)
    return SINGLE_WIRE_ERROR_STATUS;

  // wait for bit period after reading
  __delay_us(BIT_DURATION_HALF);

  *data = reg;

   return SINGLE_WIRE_SUCCESS_STATUS;
}

/**************************************************************************//**
\brief Reads Atmel MeshBean UID from ATTiny13A

\param[in] uidBuffer - memory for unique ID.

\return
    SINGLE_WIRE_SUCCESS_STATUS - if UID read successfully;
    SINGLE_WIRE_ERROR_STATUS - if error occured during UID read.
******************************************************************************/
int halReadAtmelMeshbeanUid(uint8_t *uidBuffer)
{
  uint8_t i;
  uint8_t reg = 0;

  // port sets as output.
  GPIO_SINGLE_WIRE_make_out();

  // send synchronization byte
  halWriteSingleWire(CMD_SYNCH);

  // write command
  halWriteSingleWire(CMD_READ_MAC64_0);

  // write UID location
  halWriteSingleWire(NUM_OF_MAC64);

  // Tri-state (external pullup)
  GPIO_SINGLE_WIRE_make_in();

  // wait for synchronization
  if (halReadSingleWire(&reg))
    return SINGLE_WIRE_ERROR_STATUS;

  if (CMD_SYNCH != reg)
    return SINGLE_WIRE_ERROR_STATUS;

  // wait for response with same command ID
  if (halReadSingleWire(&reg))
    return SINGLE_WIRE_ERROR_STATUS;

  if (CMD_READ_MAC64_0 == reg)
  {
    // wait for 8 bytes of UID
    for (i = UID_LEN; i > 0; i--)
    {
      // fill array in reversionary order
      if (halReadSingleWire(uidBuffer + i - 1))
        return SINGLE_WIRE_ERROR_STATUS;
    }
  }
  return SINGLE_WIRE_SUCCESS_STATUS;
}

// eof halAtmelUid.c
