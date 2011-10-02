/***************************************************************************//**
  \file zclOccupancySensingCluster.h

  \brief
    The header file describes the ZCL Occupancy Sensing Cluster and its interface

    The file describes the types and interface of the ZCL Occupancy Sensing Configuration Cluster

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2011, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
    History:
    12.02.09 D. Kasyanov - Created.
*******************************************************************************/

#ifndef _ZCLOCCUPANCYSENSINGCLUSTER_H
#define _ZCLOCCUPANCYSENSINGCLUSTER_H

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
 * \brief Occupancy Sensing cluster attributes values
*/

#define OCCUPANCY_ATTRIBUTE_VALUE_OCCUPIED 1
#define OCCUPANCY_ATTRIBUTE_VALUE_UNOCCUPIED 0

#define OCCUPANYC_SENSOR_TYPE_ATTRIBUTE_VALUE_PIR 0x00
#define OCCUPANYC_SENSOR_TYPE_ATTRIBUTE_VALUE_ULTRASONIC 0x01
#define OCCUPANYC_SENSOR_TYPE__ATTRIBITE_VALUE_PIR_AND_ULTRASONIC 0x02

/**
 * \brief Occupancy Sensing  Cluster server attributes amount
*/

#define ZCL_OCCUPANCY_SENSING_CLUSTER_SERVER_ATTRIBUTES_AMOUNT 2

/**
 * \brief Occupancy Sensing Cluster client attributes amount. Clinet doesn't have attributes.
*/

#define ZCL_OCCUPANCY_SENSING_CLUSTER_CLIENT_ATTRIBTUES_AMOUNT 0

/**
 * \brief On/Off Switch Configuration Cluster commands amount
*/

#define ZCL_OCCUPANCY_SENSING_CLUSTER_COMMANDS_AMOUNT 0

/**
 * \brief Occupancy Sensing Cluster server's attributes identifiers
*/

#define ZCL_OCCUPANCY_SENSING_CLUSTER_OCCUPANCY_SERVER_ATTRIBUTE_ID             CCPU_TO_LE16(0x0000)
#define ZCL_OCCUPANCY_SENSING_CLUSTER_OCCUPANCY_SENSOR_TYPE_SERVER_ATTRIBUTE_ID CCPU_TO_LE16(0x0001)

/**
 * \brief Occupancy Sensing Cluster server define attributes macros
*/

#define ZCL_DEFINE_OCCUPANCY_SENSING_CLUSTER_SERVER_ATTRIBUTES(min, max) \
    DEFINE_REPORTABLE_ATTRIBUTE(occupancy, ZCL_OCCUPANCY_SENSING_CLUSTER_OCCUPANCY_SERVER_ATTRIBUTE_ID, ZCL_8BIT_BITMAP_DATA_TYPE_ID, min, max), \
    DEFINE_ATTRIBUTE(occupancySensorType, ZCL_OCCUPANCY_SENSING_CLUSTER_OCCUPANCY_SENSOR_TYPE_SERVER_ATTRIBUTE_ID, ZCL_8BIT_ENUM_DATA_TYPE_ID)

/**
 * \brief Occupancy Sensing Cluster definition macros
*/

#define OCCUPANCY_SENSING_CLUSTER_ZCL_CLIENT_CLUSTER_TYPE(clattributes, clcommands) \
{ \
   .id = OCCUPANCY_SENSING_CLUSTER_ID, \
   .options = { \
                .type = ZCL_CLIENT_CLUSTER_TYPE, \
                .security = ZCL_APPLICATION_LINK_KEY_CLUSTER_SECURITY, \
                .ackRequest = 1 \
              }, \
   .attributesAmount = ZCL_OCCUPANCY_SENSING_CLUSTER_CLIENT_ATTRIBTUES_AMOUNT, \
   .attributes = (uint8_t *)clattributes, \
   .commandsAmount = ZCL_OCCUPANCY_SENSING_CLUSTER_COMMANDS_AMOUNT, \
   .commands = (uint8_t *)clcommands \
}

#define OCCUPANCY_SENSING_CLUSTER_ZCL_SERVER_CLUSTER_TYPE(clattributes, clcommands) \
{ \
   .id = OCCUPANCY_SENSING_CLUSTER_ID, \
   .options = { \
                .type = ZCL_SERVER_CLUSTER_TYPE, \
                .security = ZCL_APPLICATION_LINK_KEY_CLUSTER_SECURITY, \
                .ackRequest = 1 \
              }, \
   .attributesAmount = ZCL_OCCUPANCY_SENSING_CLUSTER_SERVER_ATTRIBUTES_AMOUNT, \
   .attributes = (uint8_t *)clattributes, \
   .commandsAmount = ZCL_OCCUPANCY_SENSING_CLUSTER_COMMANDS_AMOUNT, \
   .commands = (uint8_t *)clcommands \
}

#define DEFINE_OCCUPANCY_SENSING_CLUSTER(cltype, clattributes, clcommands) \
    OCCUPANCY_SENSING_CLUSTER_##cltype(clattributes, clcommands)

/******************************************************************************
                    Types section
******************************************************************************/

BEGIN_PACK

/**
 * \brief On/Off Switch Configuration Cluster server's attributes
*/

typedef struct PACK
{
  struct PACK
  {
    ZCL_AttributeId_t id;
    uint8_t type;
    uint8_t reportable;
    uint8_t value;
    ZCL_ReportTime_t  reportCounter;              //!<For internal use only
    ZCL_ReportTime_t  minReportInterval;          //!<Minimum reporting interval field value
    ZCL_ReportTime_t  maxReportInterval;          //!<Maximum reporting interval field value
    uint8_t           reportableChange;        //!<Reporting change field value
    ZCL_ReportTime_t  timeoutPeriod;              //!<Timeout period field value
  } occupancy;

  struct PACK
  {
     ZCL_AttributeId_t id;
     uint8_t type;
     uint8_t reportable;
     uint8_t value;
  } occupancySensorType;
} ZCL_OccupancySensingClusterServerAttributes_t;

END_PACK


#endif /* _ZCLOCCUPANCYSENSINGCLUSTER_H */

