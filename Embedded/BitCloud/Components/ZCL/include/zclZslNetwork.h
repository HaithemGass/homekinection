/**************************************************************************//**
  \file zclZslNetwork.h

  \brief
    ZSL network functionality interface.

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2011, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
    History:
    22.03.10 A. Taradov - Created.
******************************************************************************/
#ifndef _ZCLZSLNETWORK_H
#define	_ZCLZSLNETWORK_H

/******************************************************************************
                    Includes section
******************************************************************************/
#include <zclZsl.h>
#include <zclZslIB.h>
#include <zclZslScan.h>
#include <intrpData.h>

/******************************************************************************
                    Definitions section
******************************************************************************/

/******************************************************************************
                    Types section
******************************************************************************/
typedef struct _ZCL_ZslStartNetworkConf_t
{
  ZCL_ZslStatus_t status;
} ZCL_ZslStartNetworkConf_t;

typedef struct _ZCL_ZslStartNetworkReq_t
{
  ZCL_ZslStartNetworkConf_t confirm;
  ZCL_ZslScanDeviceInfo_t   *otherDevice;
  void (*ZCL_ZslStartNetworkConf)(ZCL_ZslStartNetworkConf_t *conf);
} ZCL_ZslStartNetworkReq_t;

/* Select ZSL device confirm parameters */
typedef struct _ZCL_SelectDeviceConf_t
{
  /* Operation status */
  ZCL_ZslStatus_t status;
} ZCL_SelectDeviceConf_t;

/* Select ZSL device request parameters */
typedef struct _ZCL_ZslSelectDeviceReq_t
{
  /* Selected device information */
  ZCL_ZslScanDeviceInfo_t *deviceInfo;
  /* MAC-SET.request primitive */
  MAC_SetReq_t macSetReq;
  /* Confirm callback pointer */
  void (*ZCL_ZslSelectDeviceConf)(ZCL_SelectDeviceConf_t *conf);
  /* Select ZSL device confirm info */
  ZCL_SelectDeviceConf_t confirm;
} ZCL_ZslSelectDeviceReq_t;

/******************************************************************************
                    Prototypes section
******************************************************************************/

/**************************************************************************//**
\brief Reset ZSL network module.
******************************************************************************/
void ZCL_ZslNetworkReset(void);

/**************************************************************************//**
\brief Start ZSL network request. Should be called only by End Devices.
\param req - request parameters
******************************************************************************/
void ZCL_ZslStartNetworkReq(ZCL_ZslStartNetworkReq_t *req);

/**************************************************************************//**
\brief INTRP-DATA.indication primitive
\param[in] ind - indication parameters
******************************************************************************/
void ZCL_ZslNetworkDataInd(INTRP_DataInd_t *ind);

/**************************************************************************//**
\brief ZSL device select request
\param[in] req - request parameters
******************************************************************************/
void ZCL_ZslSelectDeviceReq(ZCL_ZslSelectDeviceReq_t *req);

#endif // _ZCLZSLNETWORK_H

// eof zclZslNetwork.h
