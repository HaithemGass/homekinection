/**************************************************************************//**
  \file zclZslScan.h

  \brief
    ZSL Scan functions interface.

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2011, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
    History:
    17.03.10 A. Taradov - Created.
******************************************************************************/
#ifndef _ZCLZSLSCAN_H
#define	_ZCLZSLSCAN_H

/******************************************************************************
                    Includes section
******************************************************************************/
#include <zclZsl.h>
#include <zclZslIB.h>
#include <intrpData.h>

/******************************************************************************
                    Definitions section
******************************************************************************/

/******************************************************************************
                    Types section
******************************************************************************/

typedef struct _ZCL_ZslScanDeviceInfo_t
{
  // Device Information Table entry
  uint64_t     ieee;
  uint8_t      ep;
  uint16_t     profileId;
  uint16_t     deviceId;
  uint8_t      version;
  uint8_t      groupIds;
  uint8_t      sort;

  // Common fields
  uint8_t      rssiCorrection;
  ZclZslZigBeeInfo_t zigBeeInfo;
  ZclZslInfo_t zslInfo;
  uint16_t     keyBitMask;
  uint32_t     responseId;
  uint64_t     extPanId;
  uint8_t      nwkUpdateId;
  uint8_t      channel;
  uint16_t     panId;
  uint16_t     networkAddress;
  uint8_t      numberSubDevices;
  uint8_t      totalGroupIds;
  uint64_t     ieeeRelayerScanRequest;
  uint8_t      rssi;
} ZCL_ZslScanDeviceInfo_t;

typedef struct _ZCL_ZslScanConf_t
{
  ZCL_ZslScanDeviceInfo_t info;
  ZCL_ZslStatus_t         status;
  bool                    stopScan;
} ZCL_ZslScanConf_t;

typedef struct _ZCL_ZslScanReq_t
{
  ZCL_ZslScanConf_t confirm;
  void (*ZCL_ZslScanConf)(ZCL_ZslScanConf_t *conf);
} ZCL_ZslScanReq_t;

/******************************************************************************
                        Prototypes section
******************************************************************************/

/**************************************************************************//**
\brief Reset ZSL Scan.
******************************************************************************/
void ZCL_ZslScanReset(void);

/**************************************************************************//**
\brief ZSL Scan request. Should be called only by End Devices.
\param req - request parameters
******************************************************************************/
void ZCL_ZslScanReq(ZCL_ZslScanReq_t *req);

/**************************************************************************//**
\brief INTRP-DATA.indication primitive
\param[in] ind - indication parameters
******************************************************************************/
void ZCL_ZslScanReqDataInd(INTRP_DataInd_t *ind);

#endif // _ZCLZSLSCAN_H

// eof zclZslScan.h
