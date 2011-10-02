/***************************************************************************//**
  \file zclGenericTunnelCluster.h

  \brief
    The header file describes the ZCL Generic Tunnel Cluster and its interface

    The file describes the types and interface of the ZCL Generic Tunnel Cluster

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2011, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
    History:
    20.10.09 D. Kasyanov - Created.
*******************************************************************************/

#ifndef _ZCLGENERICTUNNELCLUSTER_H
#define _ZCLGENERICTUNNELCLUSTER_H

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
/***************************************************************************//**
  \brief ZCL Generic Tunnel Cluster server side attributes amount
*******************************************************************************/
#define ZCL_GENERIC_TUNNEL_CLUSTER_SERVER_ATTRIBUTES_AMOUNT        3
/***************************************************************************//**
  \brief ZCL Generic Tunnel Cluster client side attributes amount
*******************************************************************************/
#define ZCL_GENERIC_TUNNEL_CLUSTER_CLIENT_ATTRIBUTES_AMOUNT        0
//There are no any attributes at the client side
/***************************************************************************//**
  \brief ZCL Generic Tunnel Cluster derver side commands amount
*******************************************************************************/
#define ZCL_GENERIC_TUNNEL_COMMANDS_AMOUNT          3
/**
 * \brief Generic tunnel server side attributes identifiers.
*/
//!ZCL Generic tunnel server side MaximumIncomingTransferSize attribute id
#define ZCL_GENERIC_TUNNEL_CLUSTER_SERVER_MITS_ATTRIBUTE_ID        CCPU_TO_LE16(0x0001)
//!ZCL Generic tunnel server side MaximumOutgoingTransferSize attribute id
#define ZCL_GENERIC_TUNNEL_CLUSTER_SERVER_MOTS_ATTRIBUTE_ID        CCPU_TO_LE16(0x0002)
//!ZCL Generic tunnel server side ProtocolAddress attribute id
#define ZCL_GENERIC_TUNNEL_CLUSTER_SERVER_PROTOCOL_ADDR_ATTRIBUTE_ID     CCPU_TO_LE16(0x0003)
/**
 * \brief Generic tunnel server side commands identifiers.
*/
#define ZCL_GENERIC_TUNNEL_MATCH_PROTOCOL_ADDRESS_RESPONSE_COMMAND_ID 0x00
#define ZCL_GENERIC_TUNNEL_ADVERTISE_PROTOCOL_ADDRESS_COMMAND_ID 0x01
/**
 * \brief Generic tunnel client side commands identifiers.
*/
#define ZCL_GENERIC_TUNNEL_MATCH_PROTOCOL_ADDRESS_COMMAND_ID 0x00

#define ZCL_DEFINE_GENERIC_TUNNEL_CLUSTER_SERVER_ATTRIBUTES() \
    DEFINE_ATTRIBUTE(maximumIncomingTransferSize, ZCL_GENERIC_TUNNEL_CLUSTER_SERVER_MITS_ATTRIBUTE_ID, ZCL_U16BIT_DATA_TYPE_ID), \
    DEFINE_ATTRIBUTE(maximumOutgoingTransferSize, ZCL_GENERIC_TUNNEL_CLUSTER_SERVER_MOTS_ATTRIBUTE_ID, ZCL_U16BIT_DATA_TYPE_ID), \
    DEFINE_ATTRIBUTE(protocolAddress, ZCL_GENERIC_TUNNEL_CLUSTER_SERVER_PROTOCOL_ADDR_ATTRIBUTE_ID, ZCL_OCTET_STRING_DATA_TYPE_ID)

#define ZCL_DEFINE_GENERIC_TUNNEL_CLUSTER_COMMANDS(matchProtocolAddressCommandInd, matchProtocolAddressResponseCommandInd, advertiseProtocolAddressInd) \
    DEFINE_COMMAND(matchProtocolAddressCommand, ZCL_GENERIC_TUNNEL_MATCH_PROTOCOL_ADDRESS_COMMAND_ID, COMMAND_OPTIONS(CLIENT_TO_SERVER, ZCL_THERE_IS_RELEVANT_RESPONSE), matchProtocolAddressCommandInd), \
    DEFINE_COMMAND(matchProtocolAddressResponseCommand, ZCL_GENERIC_TUNNEL_MATCH_PROTOCOL_ADDRESS_RESPONSE_COMMAND_ID, COMMAND_OPTIONS(SERVER_TO_CLIENT, ZCL_THERE_IS_NO_RELEVANT_RESPONSE), matchProtocolAddressResponseCommandInd), \
    DEFINE_COMMAND(advertiseProtocolAddress, ZCL_GENERIC_TUNNEL_ADVERTISE_PROTOCOL_ADDRESS_COMMAND_ID, COMMAND_OPTIONS(SERVER_TO_CLIENT, ZCL_THERE_IS_NO_RELEVANT_RESPONSE), advertiseProtocolAddressInd)

#define GENERIC_TUNNEL_ZCL_CLIENT_CLUSTER_TYPE(clattributes, clcommands) \
{   \
  .id = GENERIC_TUNNEL_CLUSTER_ID, \
  .options = {.type = ZCL_CLIENT_CLUSTER_TYPE, .security = ZCL_APPLICATION_LINK_KEY_CLUSTER_SECURITY}, \
  .options.ackRequest = 1,                                                  \
  .attributesAmount = ZCL_GENERIC_TUNNEL_CLUSTER_CLIENT_ATTRIBUTES_AMOUNT, \
  .attributes = (uint8_t *)clattributes, \
  .commandsAmount = ZCL_GENERIC_TUNNEL_COMMANDS_AMOUNT, \
  .commands = (uint8_t *)clcommands \
}

#define GENERIC_TUNNEL_ZCL_SERVER_CLUSTER_TYPE(clattributes, clcommands) \
{ \
  .id = GENERIC_TUNNEL_CLUSTER_ID, \
  .options = {.type = ZCL_SERVER_CLUSTER_TYPE, .security = ZCL_APPLICATION_LINK_KEY_CLUSTER_SECURITY}, \
  .options.ackRequest = 1,                                                  \
  .attributesAmount = ZCL_GENERIC_TUNNEL_CLUSTER_SERVER_ATTRIBUTES_AMOUNT, \
  .attributes = (uint8_t *)clattributes, \
  .commandsAmount = ZCL_GENERIC_TUNNEL_COMMANDS_AMOUNT, \
  .commands = (uint8_t *)clcommands \
}

#define DEFINE_GENERIC_TUNNEL_CLUSTER(cltype, clattributes, clcommands) GENERIC_TUNNEL_CLUSTER##cltype(clattributes, clcommands)
/*******************************************************************************
                    Types section
*******************************************************************************/
BEGIN_PACK
/**
 * \brief Match Protocol Address Command Payload format.
*/
typedef struct PACK
{
    uint8_t address[1]; //!<address (octet string)
} ZCL_MatchProtocolAddress_t;
/**
 * \brief Advertise Protocol Address Command Payload format.
*/
typedef struct PACK
{
    uint8_t address[1]; //!<address (octet string)
} ZCL_AdvertiseProtocolAddress_t;
/**
 * \brief Generic tunnel cluster server attributes.
*/
typedef struct PACK
{
    struct PACK
    {
        ZCL_AttributeId_t id;
        uint8_t type;
        uint8_t reportable;
        uint16_t value;
    } maximumIncomingTransferSize;

    struct PACK
    {
        ZCL_AttributeId_t id;
        uint8_t type;
        uint8_t reportable;
        uint16_t value;
    } maximumOutgoingTransferSize;

    struct PACK
    {
        ZCL_AttributeId_t id;
        uint8_t type;
        uint8_t reportable;
        uint8_t value[OCTET_STRING_MAX_SIZE];
    } protocolAddress;
} ZCL_GenericTunnelClusterServerAttributes_t;
END_PACK
/**
 * \brief Generic tunnel cluster commands.
*/
typedef struct
{
    struct
    {
        ZCL_CommandId_t id;
        ZclCommandOptions_t options;
        ZCL_Status_t (*matchProtocolAddressCommand)(ZCL_Addressing_t *addressing, uint8_t payloadLength, ZCL_MatchProtocolAddress_t *payload);
    } matchProtocolAddressCommand;

    struct
    {
        ZCL_CommandId_t id;
        ZclCommandOptions_t options;
        ZCL_Status_t (*matchProtocolAddressResponseCommand)(ZCL_Addressing_t *addressing, uint8_t payloadLength, uint8_t *payload);
    } matchProtocolAddressResponseCommand;

    struct
    {
        ZCL_CommandId_t id;
        ZclCommandOptions_t options;
        ZCL_Status_t (*advertiseProtocolAddress)(ZCL_Addressing_t *addressing, uint8_t payloadLength, ZCL_AdvertiseProtocolAddress_t *payload);
    } advertiseProtocolAddress;
} ZCL_GenericTunnelClusterCommands_t;

#endif /* _ZCLGENERICTUNNELCLUSTER_H */

