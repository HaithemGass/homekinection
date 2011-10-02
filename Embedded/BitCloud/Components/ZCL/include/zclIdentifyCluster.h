/***************************************************************************//**
  \file zclIdentifyCluster.h

  \brief
    The header file describes the ZCL Identify Cluster and its interface

    The file describes the types and interface of the ZCL Identify Cluster

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2011, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
    History:
    11.03.09 D. Kasyanov - Created.
*******************************************************************************/

#ifndef _ZCLIDENTIFYCLUSTER_H
#define _ZCLIDENTIFYCLUSTER_H

/*!
Attributes and commands for determining basic information about a device,
setting user device information such as location, enabling a device and resetting it
to factory defaults.
*/

/*******************************************************************************
                   Includes section
*******************************************************************************/

#include <zcl.h>
#include <clusters.h>

/*******************************************************************************
                   Define(s) section
*******************************************************************************/

/**
 * \brief Identify Server Cluster attributes amount.
*/

#define ZCL_IDENTIFY_CLUSTER_SERVER_ATTRIBUTES_AMOUNT 1

/**
 * \brief Identify Client Cluster attributes amount.
*/

#define ZCL_IDENTIFY_CLUSTER_CLIENT_ATTRIBUTES_AMOUNT 0

/**
 * \brief Identify Client Cluster commands amount.
*/

#define ZCL_IDENTIFY_CLUSTER_COMMANDS_AMOUNT 3

/**
 * \brief Identify Server Cluster attributes identifiers.
*/

#define ZCL_IDENTIFY_CLUSTER_IDENTIFY_TIME_ATTRIBUTE_ID  CCPU_TO_LE16(0x0000)

/**
 * \brief Identify Server Cluster commands identifiers.
*/

#define ZCL_IDENTIFY_CLUSTER_IDENTIFY_QUERY_RESPONSE_COMMAND_ID 0x00

/**
 * \brief Identify Client Cluster commands identifiers.
*/

#define ZCL_IDENTIFY_CLUSTER_IDENTIFY_COMMAND_ID 0x00
#define ZCL_IDENTIFY_CLUSTER_IDENTIFY_QUERY_COMMAND_ID 0x01

/*
 *\brief ZCL Idetify Cluster server side attributes defining macros
 */

#define ZCL_DEFINE_IDENTIFY_CLUSTER_SERVER_ATTRIBUTES() \
  DEFINE_ATTRIBUTE(identifyTime, ZCL_IDENTIFY_CLUSTER_IDENTIFY_TIME_ATTRIBUTE_ID, ZCL_U16BIT_DATA_TYPE_ID)

/*
 * \brief ZCL Identify Cluster commands defining macros
 */

#define IDENTIFY_CLUSTER_COMMANDS(identifyCommandInd, identifyQueryCommandInd, identifyQueryResponseCommandInd) \
    DEFINE_COMMAND(identifyCommand, 0x00, COMMAND_OPTIONS(CLIENT_TO_SERVER, ZCL_THERE_IS_NO_RELEVANT_RESPONSE), identifyCommandInd), \
    DEFINE_COMMAND(identifyQueryCommand, 0x01, COMMAND_OPTIONS(CLIENT_TO_SERVER, ZCL_THERE_IS_RELEVANT_RESPONSE), identifyQueryCommandInd), \
    DEFINE_COMMAND(identifyQueryResponseCommand, 0x00, COMMAND_OPTIONS(SERVER_TO_CLIENT, ZCL_THERE_IS_NO_RELEVANT_RESPONSE), identifyQueryResponseCommandInd)

#define IDENTIFY_CLUSTER_ZCL_CLIENT_CLUSTER_TYPE(clattributes, clcommands) \
  { \
    .id = IDENTIFY_CLUSTER_ID, \
    .options = {.type = ZCL_CLIENT_CLUSTER_TYPE, .security = ZCL_NETWORK_KEY_CLUSTER_SECURITY, .ackRequest = 1}, \
    .attributesAmount = ZCL_IDENTIFY_CLUSTER_CLIENT_ATTRIBUTES_AMOUNT, \
    .attributes = (uint8_t *)clattributes, \
    .commandsAmount = ZCL_IDENTIFY_CLUSTER_COMMANDS_AMOUNT, \
    .commands = (uint8_t *)clcommands \
  }

#define IDENTIFY_CLUSTER_ZCL_SERVER_CLUSTER_TYPE(clattributes, clcommands) \
  { \
    .id = IDENTIFY_CLUSTER_ID, \
    .options = {.type = ZCL_SERVER_CLUSTER_TYPE, .security = ZCL_NETWORK_KEY_CLUSTER_SECURITY, .ackRequest = 1}, \
    .attributesAmount = ZCL_IDENTIFY_CLUSTER_SERVER_ATTRIBUTES_AMOUNT, \
    .attributes = (uint8_t *)clattributes, \
    .commandsAmount = ZCL_IDENTIFY_CLUSTER_COMMANDS_AMOUNT, \
    .commands = (uint8_t *)clcommands \
  }

#define DEFINE_IDENTIFY_CLUSTER(cltype, clattributes, clcommands) IDENTIFY_CLUSTER_##cltype(clattributes, clcommands)

/******************************************************************************
                    Types section
******************************************************************************/

BEGIN_PACK

/**
 * \brief Identify Command Payload format.
*/
typedef struct PACK
{
  uint16_t identifyTime;
} ZCL_Identify_t;

/**
 * \brief Identify Query Response Payload format.
*/

typedef struct PACK
{
  uint16_t timeout;
} ZCL_IdentifyQueryResponse_t;

typedef struct PACK
{
  struct PACK
  {
    ZCL_AttributeId_t   id;
    uint8_t             type;
    uint8_t             reportable;
    uint16_t            value;
  } identifyTime;
} ZCL_IdentifyClusterAttributes_t;

END_PACK

typedef struct
{
  struct
  {
    ZCL_CommandId_t       id;
    ZclCommandOptions_t   options;
    ZCL_Status_t (*identifyCommand)(ZCL_Addressing_t *addressing, uint8_t payloadLength, ZCL_Identify_t *payload);
  } identifyCommand;

  struct
  {
    ZCL_CommandId_t       id;
    ZclCommandOptions_t   options;
    ZCL_Status_t (*identifyQueryCommand)(ZCL_Addressing_t *addressing, uint8_t payloadLength, uint8_t *payload);
  } identifyQueryCommand;

  struct
  {
    ZCL_CommandId_t       id;
    ZclCommandOptions_t   options;
    ZCL_Status_t (*identifyQueryResponseCommand)(ZCL_Addressing_t *addressing, uint8_t payloadLength, ZCL_IdentifyQueryResponse_t *payload);
  } identifyQueryResponseCommand;
} ZCL_IdentifyClusterCommands_t;


#endif

