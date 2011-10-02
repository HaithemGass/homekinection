/****************************************************************************//**
  \file abstractSerializer.h

  \brief Declaration of abstract serialize interface.

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2011, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
  History:
    5.02.11 A. Khromykh - Created
*******************************************************************************/
#ifndef _ABSTRACTSERIALIZER_H
#define _ABSTRACTSERIALIZER_H

/******************************************************************************
                   Includes section
******************************************************************************/
#include <hardwareInit.h>
#if defined(_USART0_) || defined(_USART1_) || defined(_USARTD0_) || defined(_USARTF0_)
  #include <uartSerializer.h>
#endif
#if defined(_SPIE_)
  #include <spiSerializer.h>
#endif
#if defined(_USB_FIFO_)
  #include <usbFifoSerializer.h>
#endif

/******************************************************************************
                   Types section
******************************************************************************/
/* Some serial interface descriptor. */
typedef struct
{
  void (* hwInit)(void);
  void (* hwUnInit)(void);
  bool (* getByte)(uint8_t *p);
  void (* setByte)(uint16_t len, uint8_t *p);
} SerialInterface_t;

/******************************************************************************
                   Prototypes section
******************************************************************************/
/**************************************************************************//**
\brief Clear interface setting.
******************************************************************************/
void bootUnInitSerializer(void);

/**************************************************************************//**
\brief Read byte from interface.

\param[out]
  p - pointer to data buffer;
\param[in]
  flag - flag about unlimited or limited reading time.

\return
  true - byte was read, false - byte was not read.
******************************************************************************/
bool bootGetChar(uint8_t *p, bool flag);

/**************************************************************************//**
\brief Write byte to interface.

\param[in]
  len - data length;
\param[in]
  p - pointer to data buffer.
******************************************************************************/
void bootSetChar(uint16_t len, uint8_t *p);

/**************************************************************************//**
\brief Perform search interface activity and handshake message.

\return
  true - handshake was read, false - there is no activity.
******************************************************************************/
bool bootScanInterfaces(void);

#endif //_ABSTRACTSERIALIZER_H
