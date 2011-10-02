/**************************************************************************//**
  \file zclZslIB.h

  \brief
    ZSL Information Base interface.

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2011, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
    History:
    18.03.10 A. Taradov - Created.
******************************************************************************/
#ifndef _ZCLZSLIB_H
#define	_ZCLZSLIB_H

/******************************************************************************
                    Includes section
******************************************************************************/
#include <types.h>
#include <aps.h>
#include <zclZslFrameFormat.h>

/******************************************************************************
                        Definitions section
******************************************************************************/

/******************************************************************************
                    Types section
******************************************************************************/

// Device Information Table entry
typedef struct _DitEntry_t
{
  uint64_t ieee;
  uint8_t  ep;
  uint16_t profileId;
  uint16_t deviceId;
  uint8_t  version;
  uint8_t  groupIds;
  uint8_t  sort;
} ZCL_ZslDitEntry_t;

typedef struct _ZCL_ZslDevice_t
{
  uint8_t     factoryNew;
  uint8_t     channel;
  PanId_t     panId;
  ExtPanId_t  extPanId;
  ShortAddr_t nwkAddr;
  ExtAddr_t   extAddr;
  ShortAddr_t freeNwkAddressRangeBegin;
  ShortAddr_t freeNwkAddressRangeEnd;
  uint16_t    freeGroupIdRangeBegin;
  uint16_t    freeGroupIdRangeEnd;
  uint16_t    groupIdsBegin;
  uint16_t    groupIdsEnd;
  uint8_t     nwkKey[SECURITY_KEY_SIZE];
} ZCL_ZslDevice_t;

typedef struct _ZCL_ZslIb_t
{
  ZCL_ZslDevice_t     device;
  ZclZslZigBeeInfo_t  zigBeeInfo;
  ZclZslInfo_t        zslInfo;
  ZCL_ZslDitEntry_t   *dit;
  uint8_t             ditSize;
  uint8_t             numberSubDevices;
  uint8_t             totalGroupIds;
  MAC_CapabilityInf_t capabilityInf;
} ZCL_ZslIb_t;

/******************************************************************************
                        Global variables
******************************************************************************/
extern ZCL_ZslIb_t zclZslIB;

/******************************************************************************
                        Prototypes section
******************************************************************************/

/**************************************************************************//**
\brief Initialize Information Base
******************************************************************************/
void ZCL_ZslIbReset(void);

/**************************************************************************//**
\brief Save ZSL parameters to EEPROM.
******************************************************************************/
void ZCL_ZslIbSave(void);

/**************************************************************************//**
\brief Reset ZSL parameters to factory new settings and store them into EEPROM.
******************************************************************************/
void ZCL_ZslIbResetToFactoryNew(void);

/**************************************************************************//**
\brief Set Device Information Table.
\param[in] dit - pointer to the Device Information Table
\param[in] size - number of entries in the table
******************************************************************************/
void ZCL_ZslIbSetDeviceInfoTable(ZCL_ZslDitEntry_t dit[], uint8_t size);

#endif // _ZCLZSLIB_H

// eof zclZslIB.h
