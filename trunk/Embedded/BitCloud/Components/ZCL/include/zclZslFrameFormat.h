/**************************************************************************//**
  \file zclZslFrameFormat.h

  \brief
    InterPAN commands frame formats for ZSL.

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2011, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
    History:
    16.03.10 A. Taradov - Created.
******************************************************************************/
#ifndef _ZCLZSLFRAMEFORMAT_H
#define	_ZCLZSLFRAMEFORMAT_H

/******************************************************************************
                    Includes section
******************************************************************************/
#include <types.h>
#include <sspCommon.h>

/******************************************************************************
                    Definitions section
******************************************************************************/
#define MAX_DEVICE_INFO_ENTRIES_NUMBER   5

/******************************************************************************
                    Types section
******************************************************************************/
typedef enum _ZclZslCommandId_t
{
  ZCL_ZSL_SCAN_REQUEST_COMMAND_ID                    = 0x00,
  ZCL_ZSL_SCAN_RESPONSE_COMMAND_ID                   = 0x01,
  ZCL_ZSL_DEVICE_INFO_REQUEST_COMMAND_ID             = 0x02,
  ZCL_ZSL_DEVICE_INFO_RESPONSE_COMMAND_ID            = 0x03,
  ZCL_ZSL_IDENTIFY_REQUEST_COMMAND_ID                = 0x06,
  ZCL_ZSL_RESET_TO_FACTORY_NEW_REQUEST_COMMAND_ID    = 0x07,
  ZCL_ZSL_NETWORK_START_REQUEST_COMMAND_ID           = 0x10,
  ZCL_ZSL_NETWORK_START_RESPONSE_COMMAND_ID          = 0x11,
  ZCL_ZSL_NETWORK_JOIN_ROUTER_REQUEST_COMMAND_ID     = 0x12,
  ZCL_ZSL_NETWORK_JOIN_ROUTER_RESPONSE_COMMAND_ID    = 0x13,
  ZCL_ZSL_NETWORK_JOIN_ENDDEVICE_REQUEST_COMMAND_ID  = 0x14,
  ZCL_ZSL_NETWORK_JOIN_ENDDEVICE_RESPONSE_COMMAND_ID = 0x15,
  ZCL_ZSL_NETWORK_UPDATE_REQUEST_COMMAND_ID          = 0x16,
} ZclZslCommandId_t;

BEGIN_PACK
typedef struct PACK
{
  struct
  {
    uint8_t   zslVersion : 4;
    uint8_t   reserved    : 4;
  } frameControl;
  uint8_t     seq;
  uint8_t     commandId;
} ZclZslFrameHeader_t;

typedef struct PACK
{
  uint8_t logicalType  : 2;
  uint8_t rxOnWhenIdle : 1;
  uint8_t reserved     : 5;
} ZclZslZigBeeInfo_t;

typedef struct PACK
{
  uint8_t factoryNew          : 1;
  uint8_t addressAssignment   : 1;
  uint8_t compoundDevice      : 1;
  uint8_t relayedScanRequest  : 1;
  uint8_t touchLinkInitiator  : 1;
  uint8_t touchLinkTimeWindow : 1; // This field is reserved in Scan Request frame
  uint8_t reserved            : 2;
} ZclZslInfo_t;

typedef struct PACK
{
  uint64_t ieeeAddress;
  uint8_t  endpoint;
  uint16_t profileId;
  uint16_t deviceId;
  uint8_t  version;
  uint8_t  groupIds;
  uint8_t  sort;
} ZclZslDeviceInfoEntry_t;

typedef struct PACK
{
  ZclZslFrameHeader_t header;
  uint32_t            transactionId;
  ZclZslZigBeeInfo_t  zigBeeInfo;
  ZclZslInfo_t        zslInfo;
} ZclZslScanRequestFrame_t;

typedef struct PACK
{
  ZclZslFrameHeader_t header;
  uint32_t            transactionId;
  uint8_t             rssiCorrection;
  ZclZslZigBeeInfo_t  zigBeeInfo;
  ZclZslInfo_t        zslInfo;
  uint16_t            keyBitMask;
  uint32_t            responseId;
  uint64_t            extPanId;
  uint8_t             nwkUpdateId;
  uint8_t             channel;
  uint16_t            panId;
  uint16_t            networkAddress;
  uint8_t             numberSubDevices;
  uint8_t             totalGroupIds;
  // next fields are present only if numberSubDevices == 1
  union
  {
    struct
    {
      uint8_t         endpoint;
      uint16_t        profileId;
      uint16_t        deviceId;
      uint8_t         version;
      uint8_t         groupIds;
    };
    uint64_t          ieeeRelayerScanRequest1;
  };
  // next field is present only if zslInfo.relayedScanRequest == 1
  uint64_t            ieeeRelayerScanRequest2;
} ZclZslScanResponseFrame_t;

typedef struct PACK
{
  ZclZslFrameHeader_t header;
  uint32_t            transactionId;
  uint8_t             startIndex;
} ZclZslDeviceInfoRequestFrame_t;

typedef struct PACK
{
  ZclZslFrameHeader_t header;
  uint32_t            transactionId;
  uint8_t             numberSubDevices;
  uint8_t             startIndex;
  uint8_t             count;
  ZclZslDeviceInfoEntry_t entries[MAX_DEVICE_INFO_ENTRIES_NUMBER];
} ZclZslDeviceInfoResponseFrame_t;

typedef struct PACK
{
  ZclZslFrameHeader_t header;
  uint32_t            transactionId;
  uint16_t            identifyTime;
} ZclZslIdentifyRequestFrame_t;

typedef struct PACK
{
  ZclZslFrameHeader_t header;
  uint32_t            transactionId;
} ZclZslResetToFactoryNewRequestFrame_t;

typedef struct PACK
{
  ZclZslFrameHeader_t header;
  uint32_t            transactionId;
  uint64_t            extendedPanId;
  uint8_t             keyIndex;
  uint8_t             encryptedNwkKey[SECURITY_KEY_SIZE];
  uint8_t             channel;
  uint16_t            panId;
  uint16_t            nwkAddress;
  uint16_t            groupIdsBegin;
  uint16_t            groupIdsEnd;
  uint16_t            freeNwkAddressRangeBegin;
  uint16_t            freeNwkAddressRangeEnd;
  uint16_t            freeGroupIdRangeBegin;
  uint16_t            freeGroupIdRangeEnd;
  uint64_t            edIeeeAddress;
  uint16_t            edNwkAddress;
} ZclZslNetworkStartRequestFrame_t;

typedef struct PACK
{
  ZclZslFrameHeader_t header;
  uint32_t            transactionId;
  uint8_t             status;
  uint64_t            extendedPanId;
  uint8_t             nwkUpdateId;
  uint8_t             channel;
  uint16_t            panId;
} ZclZslNetworkStartResponseFrame_t;

typedef struct PACK
{
  ZclZslFrameHeader_t header;
  uint32_t            transactionId;
  uint64_t            extendedPanId;
  uint8_t             keyIndex;
  uint8_t             encryptedNwkKey[SECURITY_KEY_SIZE];
  uint8_t             nwkUpdateId;
  uint8_t             channel;
  uint16_t            panId;
  uint16_t            nwkAddress;
  uint16_t            groupIdsBegin;
  uint16_t            groupIdsEnd;
  uint16_t            freeNwkAddressRangeBegin;
  uint16_t            freeNwkAddressRangeEnd;
  uint16_t            freeGroupIdRangeBegin;
  uint16_t            freeGroupIdRangeEnd;
} ZclZslNetworkJoinRouterRequestFrame_t;

typedef struct PACK
{
  ZclZslFrameHeader_t header;
  uint32_t            transactionId;
  uint8_t             status;
} ZclZslNetworkJoinRouterResponseFrame_t;

typedef struct PACK
{
  ZclZslFrameHeader_t header;
  uint32_t            transactionId;
  uint64_t            extendedPanId;
  uint8_t             keyIndex;
  uint8_t             encryptedNwkKey[SECURITY_KEY_SIZE];
  uint8_t             nwkUpdateId;
  uint8_t             channel;
  uint16_t            panId;
  uint16_t            nwkAddress;
  uint16_t            groupIdsBegin;
  uint16_t            groupIdsEnd;
  uint16_t            freeNwkAddressRangeBegin;
  uint16_t            freeNwkAddressRangeEnd;
  uint16_t            freeGroupIdRangeBegin;
  uint16_t            freeGroupIdRangeEnd;
} ZclZslNetworkJoinEndDeviceRequestFrame_t;

typedef struct PACK
{
  ZclZslFrameHeader_t header;
  uint32_t            transactionId;
  uint8_t             status;
} ZclZslNetworkJoinEndDeviceResponseFrame_t;

typedef struct PACK
{
  ZclZslFrameHeader_t header;
  uint32_t            transactionId;
  uint64_t            extendedPanId;
  uint8_t             nwkUpdateId;
  uint8_t             channel;
  uint16_t            panId;
  uint16_t            nwkAddress;
} ZclZslNetworkUpdateRequestFrame_t;

END_PACK

#endif // _ZCLZSLFRAMEFORMAT_H

// eof zclZslFrameFormat.h
