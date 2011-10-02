/************************************************************************//**
  \file clusters.h

  \brief
    The header file describes the ZCLclusters

    The file describes the ZCL clusters

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2011, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
    History:
    01.12.08 I. Fedina & A. Potashov - Created.
******************************************************************************/

#ifndef _CLUSTERS_H
#define _CLUSTERS_H

#include <zcl.h>
#include <bcEndian.h>

/******************************************************************************
                   Define(s) section
******************************************************************************/
#define DEFINE_ATTRIBUTE(name, aid, atype) \
  .name = {.id = aid, .type = atype }

#define DEFINE_REPORTABLE_ATTRIBUTE(name, aid, atype, min, max) \
  .name = {.id = aid, .type = atype, .reportable = 1, .minReportInterval = min, .maxReportInterval = max}

#define COMMAND_OPTIONS(cldirection, clwaitingResponse) \
  {.direction = ZCL_FRAME_CONTROL_DIRECTION_##cldirection, .waitingResponse = clwaitingResponse}

#define DEFINE_COMMAND(cname, cid, coptions, cind) \
  .cname = {.id = cid, .options = coptions, cind}

/***************************************************************************//**
\brief The type describes possible parts of a ZCL cluster.

A cluster is a related collection of commands and attributes, which together define
an interface to specific functionality. Typically, the entity that stores the attributes
of a cluster is referred to as the server of that cluster and an entity that affects or
manipulates those attributes is referred to as the client of that cluster. However, if
required, attributes may also be present on the client of a cluster.
Commands that allow devices to manipulate attributes, e.g. the read attribute or
write attribute commands, are (typically) sent from a client device and received
by the server device. Any response to those commands, e.g. the read attribute response
or the write attribute response commands, are sent from the server device and
received by the client device.
*******************************************************************************/
#define ZCL_SERVER_CLUSTER_TYPE  0 //!< server part of a cluster
#define ZCL_CLIENT_CLUSTER_TYPE  1 //!< client part of a cluster

/*ZCL Header Direction sub-field value*/
#define ZCL_FRAME_CONTROL_DIRECTION_CLIENT_TO_SERVER    0x00
#define ZCL_FRAME_CONTROL_DIRECTION_SERVER_TO_CLIENT    0x01

/*ZCL cluster side possible values*/
#define ZCL_CLUSTER_SIDE_SERVER ZCL_FRAME_CONTROL_DIRECTION_CLIENT_TO_SERVER
#define ZCL_CLUSTER_SIDE_CLIENT ZCL_FRAME_CONTROL_DIRECTION_SERVER_TO_CLIENT

/*ZCL Header Disable Default Response sub-field value*/
#define ZCL_FRAME_CONTROL_DISABLE_DEFAULT_RESPONSE      0x01
#define ZCL_FRAME_CONTROL_ENABLE_DEFAULT_RESPONSE       0x00

/* There is other relevant response specified for the command or not */
#define ZCL_THERE_IS_RELEVANT_RESPONSE                  0x01
#define ZCL_THERE_IS_NO_RELEVANT_RESPONSE               0x00

#define ZCL_NETWORK_KEY_CLUSTER_SECURITY                0
#define ZCL_APPLICATION_LINK_KEY_CLUSTER_SECURITY       1

#ifdef _ZCL_SECURITY_
  #define  ZCL_DEFAULT_CLUSTER_SECURITY    ZCL_APPLICATION_LINK_KEY_CLUSTER_SECURITY
#else
  #define  ZCL_DEFAULT_CLUSTER_SECURITY    ZCL_NETWORK_KEY_CLUSTER_SECURITY
#endif

/*************************************************************************//**
  \brief Clusters Id definition
*****************************************************************************/
enum
{
  /* General clusters defined by ZCL Specification */
  BASIC_CLUSTER_ID = CCPU_TO_LE16(0x0000),                            //!<Basic cluster Id
  POWER_CONFIGURATION_CLUSTER_ID = CCPU_TO_LE16(0x0001),              //!<Power configuration cluster Id
  IDENTIFY_CLUSTER_ID = CCPU_TO_LE16(0x0003),                         //!<Identify cluster Id
  GROUPS_CLUSTER_ID = CCPU_TO_LE16(0x0004),                           //!<Groups cluster Id
  SCENES_CLUSTER_ID = CCPU_TO_LE16(0x0005),                           //!<Scenes cluster Id
  ONOFF_CLUSTER_ID = CCPU_TO_LE16(0x0006),                            //!<OnOff cluster id
  ONOFF_SWITCH_CONFIGURATION_CLUSTER_ID = CCPU_TO_LE16(0x0007),       //!<OnOff Switch Configuration cluster id
  LEVEL_CONTROL_CLUSTER_ID = CCPU_TO_LE16(0x0008),                    //!<Level Control cluster id
  TEMPERATURE_MEASUREMENT_CLUSTER_ID = CCPU_TO_LE16(0x0402),          //!<Temperature measurement cluster id
  HUMIDITY_MEASUREMENT_CLUSTER_ID = CCPU_TO_LE16(0x0405),             //!<Humidity measurement cluster id
  OCCUPANCY_SENSING_CLUSTER_ID = CCPU_TO_LE16(0x0406),                //!<Occupancy Sensing cluster id
  TIME_CLUSTER_ID = CCPU_TO_LE16(0x000a),                             //!<Time cluster Id
  OTAU_CLUSTER_ID = CCPU_TO_LE16(0x0019),                             //!<OTAU cluster Id
  GENERIC_TUNNEL_CLUSTER_ID = CCPU_TO_LE16(0x0600),                   //!<Generic tunnel cluster Id
  BACNET_PROTOCOL_TUNNEL_CLUSTER_ID = CCPU_TO_LE16(0x0601),           //!<BACnet protocol tunnel cluster Id
  THERMOSTAT_CLUSTER_ID = CCPU_TO_LE16(0x0201),                       //!<Thermostat cluster Id
  /* Smart Energy Profile specific clusters */
  PRICE_CLUSTER_ID = CCPU_TO_LE16(0x0700),                            //!<Price cluster Id
  DEMAND_RESPONSE_AND_LOAD_CONTROL_CLUSTER_ID = CCPU_TO_LE16(0x0701), //!<Demand Response and Load Control cluster Id
  SIMPLE_METERING_CLUSTER_ID = CCPU_TO_LE16(0x0702),                  //!<Simple Metering cluster Id
  MESSAGE_CLUSTER_ID = CCPU_TO_LE16(0x0703),                          //!<Message Cluster Id
  ZCL_KEY_ESTABLISHMENT_CLUSTER_ID = CCPU_TO_LE16(0x0800),            //!<ZCL Key Establishment Cluster Id
  /* Smart Lighting Profile clusters */
  ZSL_COMMISSIONING_CLUSTER_ID = CCPU_TO_LE16(0x1000)                 //!<ZSL Commissioning Cluster Id
};

/******************************************************************************
                   Types section
******************************************************************************/
/*************************************************************************//**
  \brief ZCL Command options

  For internal use
*****************************************************************************/
typedef struct
{
  uint8_t   direction          :1; //!<Direction sub-field (ZCL_FRAME_CONTROL_DIRECTION_CLIENT_TO_SERVER or ZCL_FRAME_CONTROL_DIRECTION_SERVER_TO_CLIENT)
  uint8_t   defaultResponse    :1; //!<Should be 0 (#ZCL_FRAME_CONTROL_ENABLE_DEFAULT_RESPONSE) if the ZCL Default Response required and 0 (ZCL_FRAME_CONTROL_DISABLE_DEFAULT_RESPONSE) if no.
  uint8_t   waitingResponse    :1; //!<Should be 1 (#ZCL_THERE_IS_RELEVANT_RESPONSE) if there is other relevant response specified for the command else 0 (#ZCL_THERE_IS_NO_RELEVANT_RESPONSE)
  uint8_t   reserved           :5; //!<Reserved for future use
} ZclCommandOptions_t;

/*************************************************************************//**
  \brief ZCL Cluster command descriptor as command Id and options

  For internal use
*****************************************************************************/
typedef struct
{
  ZCL_CommandId_t id;
  ZclCommandOptions_t options;
  ZCL_Status_t (*callback)(ZCL_Addressing_t *addressing, uint8_t payloadLength, uint8_t *payload);
} ZclCommand_t;

BEGIN_PACK
/*************************************************************************//**
  \brief ZCL Cluster attribute descriptor

  For internal use
*****************************************************************************/
typedef struct PACK
{
  ZCL_AttributeId_t id;           //!<Attribute Id
  uint8_t           type;         //!<Attribute data type
  uint8_t           reportable;   //!<Reportable property
  uint8_t           value[1];     //!<Immediate attribute value
} ZclAttribute_t;

/*************************************************************************//**
  \brief Extra fields of reportable attribute descriptor

  For internal use
*****************************************************************************/
typedef struct PACK
{
  ZCL_ReportTime_t reportCounter; //!<For internal use only
  ZCL_ReportTime_t minReportInterval; //!<Minimum reporting interval field value
  ZCL_ReportTime_t maxReportInterval; //!<Maximum reporting interval field value
} ZclReportableAttributeTail_t;
END_PACK

#endif // _CLUSTERS_H
