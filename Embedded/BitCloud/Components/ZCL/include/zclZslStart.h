/**************************************************************************//**
  \file zclZslStart.h

  \brief
    ZSL startup functionality interface.

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2011, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
    History:
    17.03.10 A. Taradov - Created.
******************************************************************************/
#ifndef _ZCLZSLSTART_H
#define	_ZCLZSLSTART_H

/******************************************************************************
                    Includes section
******************************************************************************/
#include <zclZsl.h>

/******************************************************************************
                    Definitions section
******************************************************************************/

/******************************************************************************
                    Types section
******************************************************************************/
typedef struct _ZCL_ZslStartConf_t
{
  ZCL_ZslStatus_t         status;
} ZCL_ZslStartConf_t;

typedef struct _ZCL_ZslStartReq_t
{
  ZCL_ZslStartConf_t confirm;
  void (*ZCL_ZslStartConf)(ZCL_ZslStartConf_t *conf);
} ZCL_ZslStartReq_t;

/******************************************************************************
                        Prototypes section
******************************************************************************/

/**************************************************************************//**
\brief Reset ZSL Start module.
******************************************************************************/
void ZCL_ZslStartReset(void);

/**************************************************************************//**
\brief Initiate ZSL startup procedures.
\param req - request parameters
******************************************************************************/
void ZCL_ZslStartReq(ZCL_ZslStartReq_t *req);

#endif // _ZCLZSLSTART_H

// eof zclZslStart.h
