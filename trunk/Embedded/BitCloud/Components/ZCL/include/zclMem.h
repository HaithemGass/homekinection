/************************************************************************//**
  \file zclMem.h

  \brief
    The header file describes the ZCL memory structure

    The file describes the structure of ZCL memory

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2011, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
    History:
    03.12.08 I. Fedina - Created.
******************************************************************************/

#ifndef _ZCLMEM_H
#define _ZCLMEM_H

#include <queue.h>
#include <appTimer.h>

typedef enum
{
  ZCL_IDLE_STATE,
  ZCL_REQUEST_STATE,
  ZCL_WAITING_FOR_CONFIRM_STATE,
  ZCL_WAITING_FOR_RESPONSE_STATE,
  ZCL_CONFIRM_STATE,
} ZclState_t;


/*
 * Describes ZCL memory structure
 * */
typedef struct
{
  ZclState_t state;
  QueueDescriptor_t endpointList;
  QueueDescriptor_t requestQueue;
  uint8_t *request;
  HAL_AppTimer_t waitTimer;
  HAL_AppTimer_t reportTimer;
} ZclMem_t;

/*
 * ZCL memory global object
 * */
extern ZclMem_t zclMem;

/*
 * Function returns point to ZCL memory object
 * */
static inline ZclMem_t * zclMemReq()
{
  return ((ZclMem_t *)&zclMem);
}

#endif // _ZCLMEM_H
