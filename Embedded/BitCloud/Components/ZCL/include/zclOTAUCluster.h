/**************************************************************************//**
  \file zclOTAUCluster.h

  \brief The header file describes the OTAU cluster interface.
  ZigBee Document 095264r17.
  Revision 17 Version 0.7 March 14, 2010

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2011, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
  History:
    15.01.10 A. Khromykh - Created.
*******************************************************************************/
#ifndef _ZCLOTAUCLUSTER_H
#define _ZCLOTAUCLUSTER_H

/******************************************************************************
                        Includes section
******************************************************************************/
#include <zcl.h>
#include <clusters.h>
#include <ofdExtMemory.h>

/******************************************************************************
                        Defines section
******************************************************************************/
//#define _OTAU_DEBUG_

/***************************************************************************//**
  \brief ZCL OTAU Cluster atrebutes
*******************************************************************************/
#define ZCL_OTAU_CLUSTER_SERVER_ATTRIBUTES_AMOUNT      0
#define ZCL_OTAU_CLUSTER_CLIENT_ATTRIBUTES_AMOUNT      2

/***************************************************************************//**
  \brief ZCL OTAU Cluster commands amount
*******************************************************************************/
#define ZCL_OTAU_CLUSTER_COMMANDS_AMOUNT               6

//!ZCL OTAU Cluster client side attributes id
#define ZCL_OTAU_CLUSTER_CLIENT_UPGRADE_SERVER_ID                  0x0000
#define ZCL_OTAU_CLUSTER_CLIENT_IMAGE_UPGRADE_STATUS_ID            0x0006

#define DEFINE_OTAU_CLIENT_ATTRIBUTES \
        DEFINE_ATTRIBUTE(upgradeServerIEEEAddr, ZCL_OTAU_CLUSTER_CLIENT_UPGRADE_SERVER_ID, ZCL_IEEE_ADDRESS_DATA_TYPE_ID), \
        DEFINE_ATTRIBUTE(imageUpgradeStatus, ZCL_OTAU_CLUSTER_CLIENT_IMAGE_UPGRADE_STATUS_ID, ZCL_8BIT_ENUM_DATA_TYPE_ID)

#define OTAU_HEADER_STRING_SIZE                                      32

// OTAU commands
// list of commands id
#define QUERY_NEXT_IMAGE_REQUEST_ID                 0x01
#define QUERY_NEXT_IMAGE_RESPONSE_ID                0x02
#define IMAGE_BLOCK_REQUEST_ID                      0x03
#define IMAGE_BLOCK_RESPONSE_ID                     0x05
#define UPGRADE_END_REQUEST_ID                      0x06
#define UPGRADE_END_RESPONSE_ID                     0x07

#define DEFINE_QUERY_NEXT_IMAGE_REQUEST(queryNextImageReqInd) \
        DEFINE_COMMAND(queryNextImageReq, QUERY_NEXT_IMAGE_REQUEST_ID, \
        COMMAND_OPTIONS(CLIENT_TO_SERVER, ZCL_THERE_IS_RELEVANT_RESPONSE), \
        queryNextImageReqInd)

#define DEFINE_QUERY_NEXT_IMAGE_RESPONSE(queryNextImageRespInd) \
        DEFINE_COMMAND(queryNextImageResp, QUERY_NEXT_IMAGE_RESPONSE_ID, \
        COMMAND_OPTIONS(SERVER_TO_CLIENT, ZCL_THERE_IS_NO_RELEVANT_RESPONSE), \
        queryNextImageRespInd)

#define DEFINE_IMAGE_BLOCK_REQUEST(imageBlockReqInd)  \
        DEFINE_COMMAND(imageBlockReq, IMAGE_BLOCK_REQUEST_ID, \
        COMMAND_OPTIONS(CLIENT_TO_SERVER, ZCL_THERE_IS_RELEVANT_RESPONSE), \
        imageBlockReqInd)

#define DEFINE_IMAGE_BLOCK_RESPONSE(imageBlockRespInd) \
        DEFINE_COMMAND(imageBlockResp, IMAGE_BLOCK_RESPONSE_ID, \
        COMMAND_OPTIONS(SERVER_TO_CLIENT, ZCL_THERE_IS_NO_RELEVANT_RESPONSE), \
        imageBlockRespInd)

#define DEFINE_UPGRADE_END_REQUEST(upgradeEndReqInd) \
        DEFINE_COMMAND(upgradeEndReq, UPGRADE_END_REQUEST_ID, \
        COMMAND_OPTIONS(CLIENT_TO_SERVER, ZCL_THERE_IS_RELEVANT_RESPONSE), \
        upgradeEndReqInd)

#define DEFINE_UPGRADE_END_RESPONSE(upgradeEndRespInd) \
        DEFINE_COMMAND(upgradeEndResp, UPGRADE_END_RESPONSE_ID, \
        COMMAND_OPTIONS(SERVER_TO_CLIENT, ZCL_THERE_IS_NO_RELEVANT_RESPONSE), \
        upgradeEndRespInd)

#define OTAU_COMMANDS(queryNextImageReqInd, queryNextImageRespInd, imageBlockReqInd, imageBlockRespInd,  \
                      upgradeEndReqInd, upgradeEndRespInd)                                               \
        DEFINE_QUERY_NEXT_IMAGE_REQUEST(queryNextImageReqInd),                                           \
        DEFINE_QUERY_NEXT_IMAGE_RESPONSE(queryNextImageRespInd),                                         \
        DEFINE_IMAGE_BLOCK_REQUEST(imageBlockReqInd),                                                    \
        DEFINE_IMAGE_BLOCK_RESPONSE(imageBlockRespInd),                                                  \
        DEFINE_UPGRADE_END_REQUEST(upgradeEndReqInd),                                                    \
        DEFINE_UPGRADE_END_RESPONSE(upgradeEndRespInd)

/***************************************************************************//**
  \brief ZCL OTAU Cluster server side defining macros

  \param attributes - pointer to cluster server attributes (ZCL_OtauClusterClientAttributes_t)
         or NULL if there are no attributes
  \param clcommands - pointer to cluster server commands
         or NULL if there are no commands

  \return None
*******************************************************************************/
#define OTAU_CLUSTER_ZCL_SERVER_CLUSTER_TYPE(clattributes, clcommands)        \
  {                                                                           \
    .id = OTAU_CLUSTER_ID,                                                    \
    .options = {                                                              \
      .type = ZCL_SERVER_CLUSTER_TYPE,                                        \
      .security = ZCL_DEFAULT_CLUSTER_SECURITY,                               \
      .ackRequest = 1,                                                        \
      .reserved = 0},                                                         \
    .attributesAmount = ZCL_OTAU_CLUSTER_SERVER_ATTRIBUTES_AMOUNT,            \
    .attributes = (uint8_t *) clattributes,                                   \
    .commandsAmount = ZCL_OTAU_CLUSTER_COMMANDS_AMOUNT,                       \
    .commands = (uint8_t *) clcommands                                        \
  }

/***************************************************************************//**
  \brief ZCL OTAU Cluster client side defining macros

  \param attributes - pointer to cluster client attributes (ZCL_OtauClusterClientAttributes_t)
          or NULL if there are no attributes
  \param clcommands - pointer to cluster client commands
          or NULL if there are no commands

  \return None
*******************************************************************************/
#define OTAU_CLUSTER_ZCL_CLIENT_CLUSTER_TYPE(clattributes, clcommands) \
  {                                                                    \
    .id = OTAU_CLUSTER_ID,                                             \
    .options = {                                                       \
      .type = ZCL_CLIENT_CLUSTER_TYPE,                                 \
      .security = ZCL_DEFAULT_CLUSTER_SECURITY,                        \
      .ackRequest = 1,                                                 \
      .reserved = 0},                                                  \
    .attributesAmount = ZCL_OTAU_CLUSTER_CLIENT_ATTRIBUTES_AMOUNT,     \
    .attributes = (uint8_t *) clattributes,                            \
    .commandsAmount = ZCL_OTAU_CLUSTER_COMMANDS_AMOUNT,                \
    .commands = (uint8_t *) clcommands                                 \
  }

#define DEFINE_OTAU_CLUSTER(cltype, clattributes, clcommands) OTAU_CLUSTER_##cltype(clattributes, clcommands)
/*******************************************************************************
                   Types section
*******************************************************************************/
typedef uint8_t  OtauHeaderString_t[OTAU_HEADER_STRING_SIZE];

typedef enum PACK
{
  OTAU_SERVER_HAS_BEEN_FOUND,
  OTAU_SERVER_HAS_NOT_BEEN_FOUND,
  OTAU_SERVER_HAS_BEEN_LOST,
  OTAU_OFD_DRIVER_ERROR,
  OTAU_SERVER_RECEIVED_MALFORMED_COMMAND,
  OTAU_SERVER_NO_IMAGE_AVAILABLE,
  OTAU_SERVER_RECEIVED_UNSUPPORTED_COMMAND,
  OTAU_SERVER_RECEIVED_UNKNOWN_STATUS,
  OTAU_SERVER_IS_NOT_AUTHORIZED_TO_UPDATE,
  OTAU_DOWNLOAD_STARTED,
  OTAU_DOWNLOAD_ABORTED,
  OTAU_DOWNLOAD_FINISHED,
  OTAU_DEVICE_SHALL_CHANGE_IMAGE,
  OTAU_ISD_IMAGE_CACHE_IS_FOUND,
  OTAU_ISD_HARDWARE_FAULT,
  OTAU_ISD_COMMUNICATION_LOST,
  OTAU_MAX_MESSAGE_NUMBER
} ZCL_OtauAction_t;

typedef void (* ZCL_OtauStatInd_t)(ZCL_OtauAction_t action);

typedef enum
{
  OTAU_ZB_STACK_VERSION_2006 = 0x0000,
  OTAU_ZB_STACK_VERSION_2007 = 0x0001,
  OTAU_ZB_STACK_VERSION_PRO  = 0x0002,
  OTAU_ZB_STACK_VERSION_IP   = 0x0003,
  OTAU_FAKE_VERSION          = 0xFFFF
} ZCL_OtauZigbeeStackVersion_t;

typedef enum
{
  OTAU_SPECIFIC_IMAGE_TYPE      = 0x0000,
  OTAU_SECURITY_CREDENTIAL_TYPE = 0xFFC0,
  OTAU_CONFIGURATION_TYPE       = 0xFFC1,
  OTAU_LOG_TYPE                 = 0xFFC2,
  OTAU_WILD_CARD_TYPE           = 0xFFFF
} ZCL_OtauImageType_t;

typedef enum
{
  OTAU_UPGRADE_IMAGE             = 0x0000,
  OTAU_ECDSA_SIGNATURE           = 0x0001,
  OTAU_ECDSA_SIGNING_CERTIFICATE = 0x0002,
  OTAU_FAKE_NUMBER               = 0xFFFF
} ZCL_OtauTagID_t;

BEGIN_PACK
typedef enum PACK
{
  OTAU_NORMAL,
  OTAU_DOWNLOAD_IN_PROGRESS,
  OTAU_DOWNLOAD_COMPLETE,
  OTAU_WAITING_TO_UPGRADE,
  OTAU_COUNT_DOWN,
  OTAU_WAIT_FOR_MORE
} ZCL_ImageUpdateStatus_t;

typedef union PACK
{
  uint16_t memAlloc;
  struct PACK
  {
    uint16_t securityCredentialVersionPresent : 1;
    uint16_t deviceSpecificFile               : 1;
    uint16_t hardwareVersionPresent           : 1;
    uint16_t reserved                         : 13;
  };
} ZCL_OtauHeaderFieldControl_t;

typedef struct PACK
{
  ZCL_OtauTagID_t tagID;
  uint32_t        lengthField;
} ZCL_OtauSubElementHeader_t;

typedef union PACK
{
  uint32_t memAlloc;
  struct PACK
  {
    uint8_t appRelease;
    uint8_t appBuild;
    uint8_t stackRelease;
    uint8_t stackBuild;
  };
} ZCL_OtauFirmwareVersion_t;

/***************************************************************************//**
  \brief
  ZCL OTAU upgrade image header
*******************************************************************************/
typedef struct PACK
{
  uint32_t                     magicNumber;
  uint16_t                     headerVersion;
  uint16_t                     headerLength;
  ZCL_OtauHeaderFieldControl_t fieldControl;
  uint16_t                     manufacturerId;
  uint16_t                     imageType;
  ZCL_OtauFirmwareVersion_t    firmwareVersion;
  ZCL_OtauZigbeeStackVersion_t zigbeeStackVersion;
  OtauHeaderString_t           headerString;
  uint32_t                     totalImageSize;
} ZCL_OtauUpgradeImageHeader_t;

/***************************************************************************//**
  \brief
  ZCL OTAU Cluster attributes.
  Currently, all attributes are client side attributes (only stored on the client).
*******************************************************************************/
typedef struct PACK
{
  /*!
  \brief The attribute is used to store the IEEE address of the upgrade server
  resulted from the discovery of the upgrade server's identity. If the value is set
  to a non-zero value and corresponds to an IEEE address of a device that is no longer
  accessible, a device may choose to discover a new Upgrade Server depending on its own
  security policies. The attribute is mandatory because it serves as a placeholder in a case
  where the client is programmed, during manufacturing time, its upgrade server ID.
  In addition, the attribute is used to identify the current upgrade server the client is using
  in a case where there are multiple upgrade servers in the network.  The attribute is also helpful
  in a case when a client has temporarily lost connection to the network (for example, via a reset
  or a rejoin), it shall try to rediscover the upgrade server via network address discovery using
  the IEEE address stored in the attribute. By default the value is 0xffffffffffffffff, which is
  an invalid IEEE address.  The attribute is a client-side attribute and stored on the client.
  */
  struct PACK
  {
    ZCL_AttributeId_t       id;           //!<Attribute Id (0x0000)
    uint8_t                 type;         //!<Attribute type (ExtAddr_t - 64-bit) (typeId = 0xf0, #ZCL_IEEE_ADDRESS_DATA_TYPE_ID)
    uint8_t                 reportable;   //!<Reportable property (not reportable, should be 0)
    ExtAddr_t               value;        //!<Attribute value (default none)
  } upgradeServerIEEEAddr;

  struct PACK
  {
    ZCL_AttributeId_t       id;           //!<Attribute Id (0x0006)
    uint8_t                 type;         //!<Attribute type (ZCL_ImageUpdateStatus_t - 8-bit enum) (typeId = 0x30)
    uint8_t                 reportable;   //!<Reportable property (not reportable, should be 0)
    ZCL_ImageUpdateStatus_t value;        //!<Attribute value (default none)
  } imageUpgradeStatus;
} ZCL_OtauClusterClientAttributes_t;
END_PACK

/***************************************************************************//**
  \brief
  ZCL OTAU Cluster commands.
*******************************************************************************/
BEGIN_PACK
/**
 * \brief Control field of query next image request.
*/
typedef struct PACK
{
  uint8_t hardwareVersionPresent  : 1;
  uint8_t reserved                : 7;
} ZCL_QueryNextImageReqControlField_t;

/**
 * \brief Query Next Image Request Command.
*/
typedef struct PACK
{
  ZCL_QueryNextImageReqControlField_t  controlField;
  uint16_t                             manufacturerId;
  ZCL_OtauImageType_t                  imageType;
  ZCL_OtauFirmwareVersion_t            currentFirmwareVersion;
} ZCL_OtauQueryNextImageReq_t;

/**
 * \brief Query Next Image Response Command.
*/
typedef struct PACK
{
  ZCL_Status_t              status;
  uint16_t                  manufacturerId;
  ZCL_OtauImageType_t       imageType;
  ZCL_OtauFirmwareVersion_t currentFirmwareVersion;
  uint32_t                  imageSize;
} ZCL_OtauQueryNextImageResp_t;

/**
 * \brief Control field of image block request.
*/
typedef struct PACK
{
  uint8_t reqNodeIeeeAddrPresent  : 1;
  uint8_t reserved                : 7;
} ZCL_QueryImageBlockReqControlField_t;


/**
 * \brief Image Block Request Command.
*/
typedef struct PACK
{
  ZCL_QueryImageBlockReqControlField_t  controlField;
  uint16_t                              manufacturerId;
  ZCL_OtauImageType_t                   imageType;
  ZCL_OtauFirmwareVersion_t             firmwareVersion;
  uint32_t                              fileOffset;
  uint8_t                               maxDataSize;
} ZCL_OtauImageBlockReq_t;

/**
 * \brief Image Block Response Command.
*/
typedef struct PACK
{
  ZCL_Status_t                 status;
  union PACK
  {
    struct PACK
    {
      uint16_t                  manufacturerId;
      ZCL_OtauImageType_t       imageType;
      ZCL_OtauFirmwareVersion_t firmwareVersion;
      uint32_t                  fileOffset;
      uint8_t                   dataSize;
      uint8_t                   imageData[OFD_BLOCK_SIZE];
    };
    struct PACK
    {
      uint32_t                  currentTime;
      uint32_t                  requestTime;
    };
  };
} ZCL_OtauImageBlockResp_t;

/**
 * \brief Upgrade End Request Command.
*/
typedef struct PACK
{
  ZCL_Status_t              status;
  uint16_t                  manufacturerId;
  ZCL_OtauImageType_t       imageType;
  ZCL_OtauFirmwareVersion_t firmwareVersion;
} ZCL_OtauUpgradeEndReq_t;

/**
 * \brief Upgrade End Response Command.
*/
typedef struct PACK
{
  uint16_t                  manufacturerId;
  ZCL_OtauImageType_t       imageType;
  ZCL_OtauFirmwareVersion_t firmwareVersion;
  uint32_t                  currentTime;
  uint32_t                  upgradeTime;
} ZCL_OtauUpgradeEndResp_t;
END_PACK

/**
 * \brief ZCL OTAU Cluster Commands
 */
typedef struct
{
  //! Query Next Image Request Command descriptor
  struct
  {
    ZCL_CommandId_t       id;
    ZclCommandOptions_t   options;
    ZCL_Status_t (* pQueryNextImageReqInd)(ZCL_Addressing_t *addressing, uint8_t payloadLength, ZCL_OtauQueryNextImageReq_t *payload);
  } queryNextImageReq;

  //! Query Next Image Response Command descriptor
  struct
  {
    ZCL_CommandId_t       id;
    ZclCommandOptions_t   options;
    ZCL_Status_t (* pQueryNextImageRespInd)(ZCL_Addressing_t *addressing, uint8_t payloadLength, ZCL_OtauQueryNextImageResp_t *payload);
  } queryNextImageResp;

  //! Image Block Request Command descriptor
  struct
  {
    ZCL_CommandId_t       id;
    ZclCommandOptions_t   options;
    ZCL_Status_t (* pImageBlockReqInd)(ZCL_Addressing_t *addressing, uint8_t payloadLength, ZCL_OtauImageBlockReq_t *payload);
  } imageBlockReq;

  //! Image Block Response Command descriptor
  struct
  {
    ZCL_CommandId_t       id;
    ZclCommandOptions_t   options;
    ZCL_Status_t (* pImageBlockRespInd)(ZCL_Addressing_t *addressing, uint8_t payloadLength, ZCL_OtauImageBlockResp_t *payload);
  } imageBlockResp;

  //! Upgrade End Request Command descriptor
  struct
  {
    ZCL_CommandId_t       id;
    ZclCommandOptions_t   options;
    ZCL_Status_t (* pUpgradeEndReqInd)(ZCL_Addressing_t *addressing, uint8_t payloadLength, ZCL_OtauUpgradeEndReq_t *payload);
  } upgradeEndReq;

  //! Upgrade End Response Command descriptor
  struct
  {
    ZCL_CommandId_t       id;
    ZclCommandOptions_t   options;
    ZCL_Status_t (* pUpgradeEndRespInd)(ZCL_Addressing_t *addressing, uint8_t payloadLength, ZCL_OtauUpgradeEndResp_t *payload);
  } upgradeEndResp;
} ZCL_OtauClusterCommands_t;

/**//**
 \brief  Initialization parameters for OTAU service
 */
typedef struct
{
/** \brief Cluster side. Must be chosen from: \n
    ZCL_SERVER_CLUSTER_TYPE \n
    ZCL_CLIENT_CLUSTER_TYPE */
  uint8_t                    clusterSide;
/** \brief Endpoint for cluster. */
  Endpoint_t                 otauEndpoint;
/** \brief Profile ID, that uses otau service. */
  ProfileId_t                profileId;
/** \brief Current firmware version. Parameter is valid only for client. */
  ZCL_OtauFirmwareVersion_t  firmwareVersion;
} ZCL_OtauInitParams_t;

/******************************************************************************
                   Prototypes section
******************************************************************************/
/***************************************************************************//**
\brief Gets Otau's cluster

\return Otau's client cluster
******************************************************************************/
ZCL_Cluster_t ZCL_GetOtauClientCluster(void);

/***************************************************************************//**
\brief Gets Otau's cluster

\return Otau's server cluster
******************************************************************************/
ZCL_Cluster_t ZCL_GetOtauServerCluster(void);

/***************************************************************************//**
\brief OTAU parametres initialization. Start server discovery procedure
 on client side or start server.

\param[in] pInitParam - pointer to init structure.
\param[in] ind        - indication for OTAU actions \n
("ind" can be NULL if indication is not required.)

\return Service status
******************************************************************************/
ZCL_Status_t ZCL_StartOtauService(ZCL_OtauInitParams_t *pInitParam, ZCL_OtauStatInd_t ind);

/***************************************************************************//**
\brief Stop OTAU service. (It is not implemented for client)

\param[in] pInitParam - pointer to init structure.

\return Service status
******************************************************************************/
ZCL_Status_t ZCL_StopOtauService(ZCL_OtauInitParams_t *pInitParam);

/***************************************************************************//**
\brief Checks whether OTAU cluster is busy or not.

\return true - if OTAU cluster is busy, false - otherwise.
******************************************************************************/
bool zclIsOtauBusy(void);

/***************************************************************************//**
\brief The server may send an unsolicited Upgrade End Response command to the client.
This may be used for example if the server wants to synchronize the upgrade
on multiple clients simultaneously.

\param[in] addressing - pointer to structure with addressing information;
\param[in] resp - pointer to upgrade end response payload

\return true - data is sending, false - server is busy.
******************************************************************************/
bool ZCL_UnsolicitedUpgradeEndResp(ZCL_Addressing_t *addressing, ZCL_OtauUpgradeEndResp_t *resp);

#endif /* _ZCLOTAUCLUSTER_H */
