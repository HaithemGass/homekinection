/***************************************************************************//**
  \file zclThermostatCluster.h

  \brief
    The header file describes the ZCL Thermostat Cluster and its interface

    The file describes the types and interface of the ZCL Thermostat Cluster

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2011, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
    History:
    01.07.10 A. Egorov - Created.
*******************************************************************************/

#ifndef _ZCLTHERMOSTATCLUSTER_H
#define _ZCLTHERMOSTATCLUSTER_H

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
\brief Thermostat Cluster server attributes amount
*******************************************************************************/

#define ZCL_THERMOSTAT_CLUSTER_SERVER_ATTRIBUTES_AMOUNT 5

/***************************************************************************//**
\brief Thermostat Cluster client attributes amount. Clinet doesn't have attributes
*******************************************************************************/

#define ZCL_THERMOSTAT_CLUSTER_CLIENT_ATTRIBTUES_AMOUNT 0

/***************************************************************************//**
\brief Thermostat Cluster commands amount
*******************************************************************************/

#define ZCL_THERMOSTAT_CLUSTER_COMMANDS_AMOUNT 1

/***************************************************************************//**
\brief Thermostat Cluster server's attributes identifiers
*******************************************************************************/

#define ZCL_THERMOSTAT_CLUSTER_LOCAL_TEMPERATURE_SERVER_ATTRIBUTE_ID    CCPU_TO_LE16(0x0000)
#define ZCL_THERMOSTAT_CLUSTER_OCCUPIED_COOLING_SETPOINT_SERVER_ATTRIBUTE_ID  CCPU_TO_LE16(0x0011)
#define ZCL_THERMOSTAT_CLUSTER_OCCUPIED_HEATING_SETPOINT_SERVER_ATTRIBUTE_ID  CCPU_TO_LE16(0x0012)
#define ZCL_THERMOSTAT_CLUSTER_CONTROL_SEQUENCE_OF_OPERATION_SERVER_ATTRIBUTE_ID    CCPU_TO_LE16(0x001b)
#define ZCL_THERMOSTAT_CLUSTER_SYSTEM_MODE_SERVER_ATTRIBUTE_ID   CCPU_TO_LE16(0x001c)

/***************************************************************************//**
\brief Thermostat Cluster client's command identifiers
*******************************************************************************/

#define ZCL_THERMOSTAT_CLUSTER_SETPOINT_RAISE_LOWER_COMMAND_ID            0x00

/***************************************************************************//**
\brief Thermostat Cluster server define attributes macros
*******************************************************************************/

#define ZCL_DEFINE_THERMOSTAT_CLUSTER_SERVER_ATTRIBUTES(min, max) \
    DEFINE_REPORTABLE_ATTRIBUTE(localTemperature, ZCL_THERMOSTAT_CLUSTER_LOCAL_TEMPERATURE_SERVER_ATTRIBUTE_ID, ZCL_S16BIT_DATA_TYPE_ID, min, max),\
    DEFINE_ATTRIBUTE(occupiedCoolingSetpoint, ZCL_THERMOSTAT_CLUSTER_OCCUPIED_COOLING_SETPOINT_SERVER_ATTRIBUTE_ID, ZCL_S16BIT_DATA_TYPE_ID), \
    DEFINE_ATTRIBUTE(occupiedHeatingSetpoint, ZCL_THERMOSTAT_CLUSTER_OCCUPIED_HEATING_SETPOINT_SERVER_ATTRIBUTE_ID, ZCL_S16BIT_DATA_TYPE_ID), \
    DEFINE_ATTRIBUTE(controlSequenceofOperation, ZCL_THERMOSTAT_CLUSTER_CONTROL_SEQUENCE_OF_OPERATION_SERVER_ATTRIBUTE_ID, ZCL_8BIT_ENUM_DATA_TYPE_ID), \
    DEFINE_ATTRIBUTE(systemMode, ZCL_THERMOSTAT_CLUSTER_SYSTEM_MODE_SERVER_ATTRIBUTE_ID, ZCL_8BIT_ENUM_DATA_TYPE_ID)


/***************************************************************************//**
\brief Thermostat Cluster define commands macros
*******************************************************************************/

#define ZCL_DEFINE_THERMOSTAT_CLUSTER_COMMANDS(setpointCommandInd) \
    DEFINE_COMMAND(setpointCommand, ZCL_THERMOSTAT_CLUSTER_SETPOINT_RAISE_LOWER_COMMAND_ID,  \
                     COMMAND_OPTIONS(CLIENT_TO_SERVER, ZCL_THERE_IS_NO_RELEVANT_RESPONSE), \
                     setpointCommandInd)


/***************************************************************************//**
\brief Thermostat Cluster definition macros
*******************************************************************************/

#define THERMOSTAT_CLUSTER_ZCL_CLIENT_CLUSTER_TYPE(clattributes, clcommands) \
{ \
   .id = THERMOSTAT_CLUSTER_ID, \
   .options = { \
                .type = ZCL_CLIENT_CLUSTER_TYPE, \
                .security = ZCL_APPLICATION_LINK_KEY_CLUSTER_SECURITY, \
                .ackRequest = 1, \
              }, \
   .attributesAmount = ZCL_THERMOSTAT_CLUSTER_CLIENT_ATTRIBTUES_AMOUNT, \
   .attributes = (uint8_t *)clattributes, \
   .commandsAmount = ZCL_THERMOSTAT_CLUSTER_COMMANDS_AMOUNT, \
   .commands = (uint8_t *)clcommands \
}

#define THERMOSTAT_CLUSTER_ZCL_SERVER_CLUSTER_TYPE(clattributes, clcommands) \
{ \
   .id = THERMOSTAT_CLUSTER_ID, \
   .options = { \
                .type = ZCL_SERVER_CLUSTER_TYPE, \
                .security = ZCL_APPLICATION_LINK_KEY_CLUSTER_SECURITY, \
                .ackRequest = 1, \
              }, \
   .attributesAmount = ZCL_THERMOSTAT_CLUSTER_SERVER_ATTRIBUTES_AMOUNT, \
   .attributes = (uint8_t *)clattributes, \
   .commandsAmount = ZCL_THERMOSTAT_CLUSTER_COMMANDS_AMOUNT, \
   .commands = (uint8_t *)clcommands \
}

#define DEFINE_THERMOSTAT_CLUSTER(cltype, clattributes, clcommands) \
    THERMOSTAT_CLUSTER_##cltype(clattributes, clcommands)

/******************************************************************************
                    Types section
******************************************************************************/
BEGIN_PACK

/***************************************************************************//**
\brief Setpoint Raise/Lower Command Payload format
*******************************************************************************/
typedef struct PACK
{
  int8_t mode;
  int8_t amount;
} ZCL_SetpointRaiseLower_t;

/***************************************************************************//**
\brief Thermostat Cluster server's attributes
*******************************************************************************/

typedef struct PACK
{ 
  struct PACK 
  {
    ZCL_AttributeId_t id;
    uint8_t type;
    uint8_t reportable;
    int16_t value;
    ZCL_ReportTime_t reportCounter; //!<For internal use only
    ZCL_ReportTime_t minReportInterval; //!<Minimum reporting interval field value
    ZCL_ReportTime_t maxReportInterval; //!<Maximum reporting interval field value
    int16_t reportableChange; //!<Reporting change field value
    ZCL_ReportTime_t timeoutPeriod; //!<Timeout period field value
  } localTemperature ;
  
  
  struct PACK
  {
    ZCL_AttributeId_t id;
    uint8_t type;
    uint8_t reportable;
    int16_t value;
  } occupiedCoolingSetpoint;

  struct PACK
  {
    ZCL_AttributeId_t id;
    uint8_t type;
    uint8_t reportable;
    int16_t value;
  } occupiedHeatingSetpoint;

  struct PACK
  {
    ZCL_AttributeId_t id;
    uint8_t type;
    uint8_t reportable;
    int8_t value;
  } controlSequenceofOperation;

  struct PACK
  {
    ZCL_AttributeId_t id;
    uint8_t type;
    uint8_t reportable;
    int8_t value;
  } systemMode;
} ZCL_ThermostatClusterServerAttributes_t;

END_PACK

/***************************************************************************//**
\brief Thermostat Cluster commands
*******************************************************************************/
typedef struct
{
  struct
  {
    ZCL_CommandId_t id;
    ZclCommandOptions_t options;
    ZCL_Status_t(*setpointCommand)(ZCL_Addressing_t *addressing, uint8_t payloadLength, ZCL_SetpointRaiseLower_t * payload);
  } setpointCommand;
} ZCL_ThermostatClusterCommands_t;


#endif /* _ZCLTHERMOSTATCLUSTER_H */

