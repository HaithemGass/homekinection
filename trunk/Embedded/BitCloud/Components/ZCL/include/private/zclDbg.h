/************************************************************************//**
  \file zclDbg.h

  \brief
    The header file describes ZCL Debug Module

    The file describes the ZCL Debug Module

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2011, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
    History:
    02.12.08 A. Poptashov - Created.
******************************************************************************/

#ifndef _ZCLDBG_H
#define _ZCLDBG_H

/******************************************************************************
                   Includes section
******************************************************************************/
#include <dbg.h>

/******************************************************************************
                   Define(s) section
******************************************************************************/
#if defined _SYS_ASSERT_ON_

#define  ZCL_SET_STATE(state, newState)  (state) = (newState)
#define  ZCL_CHECK_STATE(state, checkState, nameOfAssert) \
  assert((checkState) == (state), nameOfAssert)

#else /* _SYS_ASSERT_ON_ */

#define  ZCL_SET_STATE(state, newState)
#define  ZCL_CHECK_STATE(state, waitState, nameOfAssert)
#if defined assert
#undef assert
#endif /* assert */
#define  assert(condition, dbgCode)  (void)0

#endif /* _SYS_ASSERT_ON_ */

/******************************************************************************
                   Types section
******************************************************************************/

/******************************************************************************
                   Constants section
******************************************************************************/
typedef enum
{
  /***********************************
    ZCL section. Range 0x8000 - 0x8fff
  ************************************/

  //ZCL Task Manager Id       (0x8000 - 0x80ff)
  ZCLTASKMANAGER_TASKHANDLER_0 = 0x8000,
  //ZCL CAPS Id               (0x8100 - 0x81ff)

  //ZCL ZCL Id                (0x8200 - 0x82ff)
  ZCLGETCLUSTER_0 = 0x8200,
  PARSEDATAIND_0  = 0x8201,
  ZCLREPORTIND_0  = 0x8202,
  DBG_ZCL_GET_TRUST_CENTER_ADDRESS = 0x8203,
  ZCL_UNDEFINED_CLUSTER_IN_REQUEST = 0x8204,
  ZCL_UNEXPECTED_ASDU_LENGTH       = 0x8205,
  UNKNOWN_DAT_TYPE_DESCR           = 0x8206,
  //ZCL Memory Manager        (0x8300 - 0x83ff)
  ZCL_DATAIND_0 = 0x8300,
  ZCL_THERE_ARE_NO_BUFFERS = 0x8301,
  //OTAU                      (0x8400 - 0x84ff)
  ZCL_OTAU_NULL_POINTER_TO_INIT_STRUCTURE = 0x8400,
  ZCL_OTAU_DOUBLE_START                   = 0x8401,
  ZCL_OTAU_UNEXPECTED_SERVER_DISCOVERY    = 0x8402,
  ZCL_UNKNOWN_CUSTOM_MESSAGE              = 0x8403,
  ZCL_OTAU_UNKNOWN_SERVER_TRANSACTUION_ID = 0x8404,
  ZCL_UNKNOWN_ISD_MESSAGE                 = 0x8405,
  // KE
  KE_WRONG_STATE_0 = 0x8500,
  KE_WRONG_STATE_1 = 0x8501,
  KE_WRONG_STATE_2 = 0x8502,
  KE_WRONG_STATE_3 = 0x8503,
  KE_WRONG_STATE_4 = 0x8504,
  KE_WRONG_STATE_5 = 0x8505,

  ZCL_MEMORY_CORRUPTION_0 = 0x8600,
  ZCL_MEMORY_CORRUPTION_1 = 0x8601,
  ZCL_MEMORY_CORRUPTION_2 = 0x8602,
  ZCL_MEMORY_CORRUPTION_3 = 0x8603
} ZclDbgCodeId_t;

typedef enum
{
  OTAU_STOPPED_STATE,
  OTAU_STARTED_STATE,
  OTAU_OFD_INITIALIZATION_STATE,
  OTAU_BROADCAST_MATCH_DESC_REQ,
  OTAU_UNICAST_MATCH_DESC_REQ,
  OTAU_SHORT_ADDR_REQ,
  OTAU_EXT_ADDR_REQ,
  OTAU_GET_TRUST_CENTRE_ADDR,
  OTAU_GET_LINK_KEY,
  OTAU_QUERY_NEXT_IMAGE_TRANSAC,
  OTAU_WAITING_FOR_SERVER_DISCOVERY,
  OTAU_ERASE_IMAGE,
  OTAU_IMAGE_BLOCK_TRANSAC,
  OTAU_WRITE_DATA_TO_FLASH,
  OTAU_FLUSH_DATA_TO_FLASH,
  OTAU_UPGRADE_END_TRANSAC,
  OTAU_WAITING_FOR_UPGRADE_TIMEOUT,
  OTAU_WAITING_FOR_UPGRADE_UNLIMITED,
  OTAU_SWITCH_IMAGE
} ZclOtauStateMachine_t;

/******************************************************************************
                   External variables section
******************************************************************************/

/******************************************************************************
                   Prototypes section
******************************************************************************/

/******************************************************************************
                   Inline static functions section
******************************************************************************/


#endif  //#ifndef _ZCLDBG_H

//eof zclDbg.h
