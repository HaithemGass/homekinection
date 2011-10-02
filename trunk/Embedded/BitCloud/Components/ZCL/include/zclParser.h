/************************************************************************//**
  \file zclParser.h

  \brief
    The header file describes the ZCL Parser interface

    The file describes the interface and types of ZCL Parser

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2011, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
    History:
    02.12.08 A. Potashov - Created.
******************************************************************************/

#ifndef _ZCLPARSER_H
#define _ZCLPARSER_H

/******************************************************************************
                   Includes section
******************************************************************************/
#include <types.h>
#include <zclDbg.h>
#include <zcl.h>
#include <zclMemoryManager.h>
#include <queue.h>

/******************************************************************************
                   Define(s) section
******************************************************************************/
/*ZCL Header Frame Type sub-field value*/
#define ZCL_FRAME_CONTROL_FRAME_TYPE_UNIVERSAL_COMMAND            0x00
#define ZCL_FRAME_CONTROL_FRAME_TYPE_SPECIFIC_COMMAND             0x01

/*ZCL Header Manufacturer Specific sub-field value*/
#define ZCL_FRAME_CONTROL_MANUFACTURER_NONSPECIFIC                0x00
#define ZCL_FRAME_CONTROL_MANUFACTURER_SPECIFIC                   0x01

#define ZCL_FRAME_SIMPLE_HEADER_LENGTH                            3
#define ZCL_FRAME_HEADER_MANUFACTURER_CODE_LENGTH                 2
#define ZCL_FRAME_HEADER_WITH_MANUFACTURER_CODE_LENGTH            \
        (ZCL_FRAME_SIMPLE_HEADER_LENGTH + ZCL_FRAME_HEADER_MANUFACTURER_CODE_LENGTH)

#ifndef ZCL_STANDARD_REQ_TYPE
#define ZCL_STANDARD_REQ_TYPE                                     ZCL_FRAME_CONTROL_FRAME_TYPE_UNIVERSAL_COMMAND
#endif
#ifndef ZCL_SPECIAL_REQ_TYPE
#define ZCL_SPECIAL_REQ_TYPE                                      ZCL_FRAME_CONTROL_FRAME_TYPE_SPECIFIC_COMMAND
#endif

#ifndef ZCL_MAX_STANDARD_DATA_TYPE_SIZE
#define ZCL_MAX_STANDARD_DATA_TYPE_SIZE                           8
#endif

/*************************************************************************//**
 \brief Defines the Descrete Kind of ZCL Data Type (used in #ZCL_DataTypeDescriptor_t)
*****************************************************************************/
#ifndef ZCL_DATA_TYPE_DESCRETE_KIND
#define ZCL_DATA_TYPE_DESCRETE_KIND                               0x00
#endif

/*************************************************************************//**
 \brief Defines the Analog Kind of ZCL Data Type (used in #ZCL_DataTypeDescriptor_t)
*****************************************************************************/
#ifndef ZCL_DATA_TYPE_ANALOG_KIND
#define ZCL_DATA_TYPE_ANALOG_KIND                                 0x01
#endif

/******************************************************************************
                   Types section
******************************************************************************/

typedef enum
{
  ZCL_PARSER_IDLE_STATE,
  ZCL_PARSER_RECEIVED_PACKET_STATE,
  ZCL_PARSER_SENDING_RESPONSE_STATE,
} ZclParserState_t;

typedef struct
{
  //ZclParserState_t state;
  uint8_t dataIndAmount;
  QueueDescriptor_t dataIndQueue;
} ZclParserMem_t;

BEGIN_PACK
typedef struct PACK
{
  union PACK
  {
    struct PACK
    {
      LITTLE_ENDIAN_OCTET(5,(
        uint8_t frameType             :2, //Frame type sub-field
        uint8_t manufacturerSpecific  :1, //Manufacturer specific sub-field
        uint8_t direction             :1, //Direction sub-field
        uint8_t defaultResponse       :1, //Disable default response sub-field
        uint8_t reserved              :3  //Reserved bits. Must have a zero value
      ))
    };
    uint8_t uint8;
  };
} ZclFrameControlField_t;

typedef struct PACK
{
  ZclFrameControlField_t  frameControl;
  uint16_t                manufacturerCode;
  uint8_t                 sequenceNumber;
  uint8_t                 commandId;
} ZclFrameHeader_t;

typedef struct PACK
{
  ZclFrameHeader_t  header;
  uint8_t           payload[1];
} ZclFrame_t;

/******************************************************************************
 Read Attributes commands
******************************************************************************/
/** \brief ZCL read attributes command record */
typedef struct PACK
{
  uint16_t attributeId;
} ZclReadAttributesRecord_t;

/** \brief ZCL Read attributes command payload */
typedef struct PACK
{
  ZclReadAttributesRecord_t records[1];
} ZclReadAttributesCommand_t;

typedef struct PACK
{
  uint8_t   elementType;
  uint16_t  elementsAmount;
  struct PACK
  {
    uint8_t   value[1];
  } elements[1];
} ZclReadAttributesResponseArraySetBagValue_t;

typedef struct PACK
{
  uint16_t  elementsAmount;
  struct PACK
  {
    uint8_t   elementType;
    uint8_t   value[1];
  } elements [1];
} ZclReadAttributesResponseStructValue_t;

/** \brief ZCL read attributes response command record */
typedef struct PACK
{
  ZCL_AttributeId_t attributeId;
  uint8_t           status;
  uint8_t           type;
  union PACK
  {
    uint8_t         simpleValue[1];
    ZclReadAttributesResponseArraySetBagValue_t arraySetBagValue;
    ZclReadAttributesResponseStructValue_t      structValue;
  };
} ZclReadAttributesResponseRecord_t;

/** \brief ZCL read attributes response command payload */
typedef struct PACK
{
  ZclReadAttributesResponseRecord_t records[1];
} ZclReadAttributesResponseCommand_t;

/******************************************************************************
 Write Attributes commands
******************************************************************************/
/** \brief ZCL write attributes command record */
typedef struct PACK
{
  ZCL_AttributeId_t attributeId;
  uint8_t  type;
  uint8_t  value[1];
} ZclWriteAttributesRecord_t;

/** \brief ZCL Wrire attributes command payload */
typedef struct PACK
{
  ZclWriteAttributesRecord_t  records[1];
} ZclWriteAttributesCommand_t;

/** \brief ZCL Wrire attributes undivided command payload */
typedef ZclWriteAttributesCommand_t ZclWriteAttributesUndividedCommand_t;

/** \brief ZCL Wrire attributes no response command payload */
typedef ZclWriteAttributesCommand_t ZclWriteAttributesNoResponseCommand_t;

/** \brief ZCL write attributes response command record */
typedef struct PACK
{
  uint8_t  status;
  uint16_t attributeId;
} ZclWriteAttributesResponseRecord_t;

/** \brief ZCL Wrire attributes response command payload */
typedef struct PACK
{
  ZclWriteAttributesResponseRecord_t  records[1];
} ZclWriteAttributesResponseCommand_t;

/******************************************************************************
 Configure reporting command
******************************************************************************/
/** \brief ZCL configure reporting command record */
typedef struct PACK
{
  uint8_t   direction;
  uint16_t  attributeId;
  uint16_t  minimumReportingInterval;
  uint16_t  maximumReportingInterval;
  uint8_t   reportableChange[1];
  uint16_t  timeoutPeriod;
} ZclConfigureReportingRecord_t;

/** \brief ZCL configure reporting command payload */
typedef struct PACK
{
  ZclConfigureReportingRecord_t  records[1];
} ZclConfigureReportingCommand_t;

/** \brief ZCL configure reporting response command record */
typedef struct PACK
{
  uint8_t   status;
  uint8_t   direction;
  uint16_t  attributeId;
} ZclConfigureReportingResponseRecord_t;

/** \brief ZCL configure reporting response command payload */
typedef struct PACK
{
  ZclConfigureReportingResponseRecord_t  records[1];
} ZclConfigureReportingResponseCommand_t;

END_PACK

typedef struct
{
  ZCL_GeneralCommandId_t  commandId;
  void                    *primitive;
} ZclBuffer_t;

typedef enum
{
  ZCL_PARSER_SUCCESS_STATUS               = 0,
  ZCL_PARSER_UNKNOWN_COMMAND_STATUS       = 1,
  ZCL_PARSER_BAD_FORMAT_STATUS            = 2,
  ZCL_PARSER_INVALID_PARAMETER_STATUS     = 3
} ZclParserStatus_t;

typedef enum
{
  ZCL_REQUEST_TYPE,
  ZCL_RESPONSE_TYPE,
  ZCL_EVENT_TYPE,
  ZCL_UNSPECIFIED_COMMAND_TYPE
} ZCL_CommandType_t;

typedef struct
{
  uint8_t   headerLength;
  uint8_t   payloadLength;
  uint8_t   frameType;
  uint8_t   direction;
  uint8_t   defaultResponse;
  uint8_t   manufacturerSpecific;
  uint16_t  manufacturerCode;
  uint8_t   sequenceNumber;
  uint8_t   commandId;
  uint8_t   *header;
  uint8_t   *payload;
} ZclFrameDescriptor_t;

typedef struct
{
  ZclFrameDescriptor_t *frameDescriptor;
  APS_DataInd_t        *ind;
  ZCL_Status_t          commandStatus;
  bool                  noApsSecurity;
  bool                  relevantResponse;
} ZclAuxParseData_t;

/******************************************************************************
                   Constants section
******************************************************************************/

/******************************************************************************
                   External variables section
******************************************************************************/
/**************************************************************************//**
\brief ZCL memory global object
******************************************************************************/
extern ZclParserMem_t zclParserMem;

/******************************************************************************
                   Prototypes section
******************************************************************************/
/*************************************************************************//**
  \brief Reset parser queue and clear memory.
*****************************************************************************/
void zclParserInit(void);

/**************************************************************************//**
\brief Form zcl header.

\param[in] - dataReq - pointer to data request;
\param[in] - reqType - type of the request;
\param[in] - direction - cluster\server side;
\param[in] - commandId - zcl command identification;
\param[in] - defaultResponse - presence or absence of the default responde;
\param[in] - manufacSpecCode - manufacturer specific code;
\return header length
******************************************************************************/
uint8_t zclFormRequest(APS_DataReq_t *dataReq, uint8_t reqType, uint8_t direction, uint8_t commandId, uint8_t defaultResponse, uint16_t manufacSpecCode);

/*************************************************************************//**
  \brief Response has been received.

  \param[in] auxData - pointer to structure with command payload and
  descriptor of received command frame.
*****************************************************************************/
void zclResponseInd(ZclAuxParseData_t *auxData);

/*************************************************************************//**
\brief Routine of Read Reporting Configuration Command.

\param[in] apsDataInd - pointer to aps payload
\param[in] frameDescriptor - pointer to parsed data structure from payload
*****************************************************************************/
void zclReadReportingConfigurationHandler(APS_DataInd_t *apsDataInd, ZclFrameDescriptor_t *frameDescriptor);

/*************************************************************************//**
\brief Routine of Configure Reporting Command.

\param[in] apsDataInd - pointer to aps payload
\param[in] frameDescriptor - pointer to parsed data structure from payload
*****************************************************************************/
void zclConfigureReportingHandler(APS_DataInd_t *apsDataInd, ZclFrameDescriptor_t *frameDescriptor);

/*************************************************************************//**
  \brief Report has been received.

  \param[in] apsData - report payload
  \param[in] frameDesc - descriptor of received report frame
*****************************************************************************/
void zclReportInd(APS_DataInd_t *apsData, ZclFrameDescriptor_t *frameDescriptor);

/*************************************************************************//**
  \brief Special cluster command has been received.

  \param[in] auxData - pointer to structure with command payload and
  descriptor of received command frame.
*****************************************************************************/
void zclCommandInd(ZclAuxParseData_t *auxData);

/*************************************************************************//**
  \brief The attribute has been read or written.

  \param[in] apsDataInd - attribute payload
  \param[in] direction - direction
  \param[in] event - event type
  \param[in] attributeId - attribute identifier
*****************************************************************************/
void zclAttributeEventInd(APS_DataInd_t *apsDataInd, uint8_t direction, ZCL_AttributeEvent_t event, ZCL_AttributeId_t attributeId);

/*************************************************************************//**
  \brief Configuration has been received.

  \param[in] apsData - configuration payload
  \param[in] frameDesc - descriptor of received report frame
*****************************************************************************/
void zclConfigurationInd(APS_DataInd_t *apsData, ZclFrameDescriptor_t *frameDesc);

/*************************************************************************//**
  \brief Indication from CAPS.

  \param[in] ind - buffer for data indication.
*****************************************************************************/
void zclDataInd(APS_DataInd_t *ind);

/*************************************************************************//**
  \brief     Security Type get by Cluster Id function and by endpointId, on which
             was registered cluster.
             Returns Security Type should be used with ClusterId specified
  \param[in] clusterId - cluster identifier
  \param[in] endpointId - end point identifier
  \return    ZCL_NETWORK_KEY_CLUSTER_SECURITY - NWK Key Security should be used
             ZCL_APPLICATION_LINK_KEY_CLUSTER_SECURITY - APS Link Key Security
             shoud be used.
             If cluster is unknown, security key will be determined by security mode,
             i.e. ZCL_NETWORK_KEY_CLUSTER_SECURITY for Standard Security,
                  ZCL_APPLICATION_LINK_KEY_CLUSTER_SECURITY for High Security.
*****************************************************************************/
uint8_t zclGetSecurityTypeByClusterId(ClusterId_t clusterId, Endpoint_t endpointId);

/*************************************************************************//**
  \brief Find endpoint descriptor by endpoint id.

  \param[in] endpointId - endpoint unique identifier.
  \return endpoint descriptor if found, NULL otherwise.
*****************************************************************************/
ZCL_DeviceEndpoint_t *zclGetEndpoint(Endpoint_t endpointId);

/******************************************************************************
                   Inline static functions section
******************************************************************************/
/******************************************************************************
\brief Function returns point to ZCL memory object
******************************************************************************/
static inline ZclParserMem_t *zclParserMemReq(void)
{
  return ((ZclParserMem_t *)&zclParserMem);
}

#endif  //#ifndef _ZCLPARSER_H

//eof zclParser.h
