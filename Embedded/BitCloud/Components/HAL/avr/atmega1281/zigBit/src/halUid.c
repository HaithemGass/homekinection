/**************************************************************************//**
  \file   uid.c

  \brief  Implementation of UID interface.

  \author
      Atmel Corporation: http://www.atmel.com \n
      Support email: avr@atmel.com

    Copyright (c) 2008-2011, Atmel Corporation. All rights reserved.
    Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
    History:
      7/12/07 A. Khromykh - Created
 ******************************************************************************/
/******************************************************************************
 *   WARNING: CHANGING THIS FILE MAY AFFECT CORE FUNCTIONALITY OF THE STACK.  *
 *   EXPERT USERS SHOULD PROCEED WITH CAUTION.                                *
 ******************************************************************************/

/******************************************************************************
                   Includes section
******************************************************************************/
#include <w1.h>
#include <uid.h>
#include <halAtmelUid.h>

/******************************************************************************
                   Global variables section
******************************************************************************/
static HalUid_t halUid = {.uid = 0ull};

/******************************************************************************
                   Implementations section
******************************************************************************/
/******************************************************************************
 Reads uid from ds2411 (meshnetics meshbean) or ATTiny13A (atmel meshbean).
******************************************************************************/
void halReadUid(void)
{
#ifdef _HAL_TINY_UID_
  // Atmel Single-Wire Software UART UID
  halReadAtmelMeshbeanUid(halUid.array);
#else
  // 1-Wire UID
  if (W1_SUCCESS_STATUS == HAL_SearchW1Device(W1_ANY_FAMILY, halUid.array, 1, NULL))
  {
    *(halUid.array + 7) = *(halUid.array + 0);
    halUid.uid >>= 8;
  }
#endif
}

/******************************************************************************
 UID discovery.
 Parameters:
   id - UID buffer pointer.
 Returns:
   0 - if unique ID has been found without error;
  -1 - if there are some errors during UID discovery.
******************************************************************************/
int HAL_ReadUid(uint64_t *id)
{
  if (!id)
    return -1;

  *id = halUid.uid;
  return 0;
}

// eof uid.c
