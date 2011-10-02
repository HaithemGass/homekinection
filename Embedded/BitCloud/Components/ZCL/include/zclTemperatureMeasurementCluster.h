/*****************************************************************************
  \file  zclTemperatureMeasurement.h

  \brief
    The file describes the types and interface of the Temperature Measurement cluster

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2011, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
  History:
    18.01.2010 I.Vagulin - Created
******************************************************************************/


#ifndef _ZCLTEMPERATUREMEASUREMENT_H_
#define _ZCLTEMPERATUREMEASUREMENT_H_

/*******************************************************************************
                   Includes section
*******************************************************************************/
#include <types.h>
#include <clusters.h>

/*******************************************************************************
                   Define(s) section
*******************************************************************************/
/**
* \brief server attributes amount
*/
#define ZCL_TEMPERATURE_MEASUREMENT_CLUSTER_SERVER_ATTRIBUTES_AMOUNT 3

/**
* \brief server commands amount
*/
#define ZCL_TEMPERATURE_MEASUREMENT_CLUSTER_SERVER_COMMANDS_AMOUNT 0

/**
* \brief client attributes amount
*/
#define ZCL_TEMPERATURE_MEASUREMENT_CLUSTER_CLIENT_ATTRIBUTES_AMOUNT 0

/**
* \brief client commands amount
*/
#define ZCL_TEMPERATURE_MEASUREMENT_CLUSTER_CLIENT_COMMANDS_AMOUNT 0

/**
* \brief Supported attribue id
*/
#define ZCL_TEMPERATURE_MEASUREMENT_CLUSTER_SERVER_MEASURED_VALUE_ATTRIBUTE_ID     CCPU_TO_LE16(0x0000)
#define ZCL_TEMPERATURE_MEASUREMENT_CLUSTER_SERVER_MIN_MEASURED_VALUE_ATTRIBUTE_ID CCPU_TO_LE16(0x0001)
#define ZCL_TEMPERATURE_MEASUREMENT_CLUSTER_SERVER_MAX_MEASURED_VALUE_ATTRIBUTE_ID CCPU_TO_LE16(0x0002)

/**
 * \brief server define attributes macros
*/
#define ZCL_DEFINE_TEMPERATURE_MEASUREMENT_CLUSTER_SERVER_ATTRIBUTES(reportMin, reportMax) \
  DEFINE_REPORTABLE_ATTRIBUTE(measuredValue, ZCL_TEMPERATURE_MEASUREMENT_CLUSTER_SERVER_MEASURED_VALUE_ATTRIBUTE_ID, ZCL_S16BIT_DATA_TYPE_ID, reportMin, reportMax), \
  DEFINE_ATTRIBUTE(minMeasuredValue,  ZCL_TEMPERATURE_MEASUREMENT_CLUSTER_SERVER_MIN_MEASURED_VALUE_ATTRIBUTE_ID,  ZCL_S16BIT_DATA_TYPE_ID), \
  DEFINE_ATTRIBUTE(maxMeasuredValue, ZCL_TEMPERATURE_MEASUREMENT_CLUSTER_SERVER_MAX_MEASURED_VALUE_ATTRIBUTE_ID, ZCL_S16BIT_DATA_TYPE_ID)

#define ZCL_DEFINE_TEMPERATURE_MEASUREMENT_CLUSTER_SERVER(clattributes) \
  { \
    .id = TEMPERATURE_MEASUREMENT_CLUSTER_ID, \
    .options = {.type = ZCL_SERVER_CLUSTER_TYPE, .security = ZCL_NETWORK_KEY_CLUSTER_SECURITY, .ackRequest = 1}, \
    .attributesAmount = ZCL_TEMPERATURE_MEASUREMENT_CLUSTER_SERVER_ATTRIBUTES_AMOUNT, \
    .attributes = (uint8_t *) (clattributes), \
    .commandsAmount = ZCL_TEMPERATURE_MEASUREMENT_CLUSTER_SERVER_COMMANDS_AMOUNT, \
    .commands = NULL \
  }

#define ZCL_DEFINE_TEMPERATURE_MEASUREMENT_CLUSTER_CLIENT() \
  { \
    .id = TEMPERATURE_MEASUREMENT_CLUSTER_ID, \
    .options = {.type = ZCL_CLIENT_CLUSTER_TYPE, .security = ZCL_NETWORK_KEY_CLUSTER_SECURITY, .ackRequest = 1}, \
    .attributesAmount = ZCL_TEMPERATURE_MEASUREMENT_CLUSTER_CLIENT_ATTRIBUTES_AMOUNT, \
    .attributes = NULL, \
    .commandsAmount = ZCL_TEMPERATURE_MEASUREMENT_CLUSTER_CLIENT_COMMANDS_AMOUNT, \
    .commands = NULL \
  }

/******************************************************************************
                    Types section
******************************************************************************/
BEGIN_PACK
typedef struct PACK {
  struct PACK {
    ZCL_AttributeId_t id;
    uint8_t type;
    uint8_t reportable;
    int16_t value;
    ZCL_ReportTime_t reportCounter; //!<For internal use only
    ZCL_ReportTime_t minReportInterval; //!<Minimum reporting interval field value
    ZCL_ReportTime_t maxReportInterval; //!<Maximum reporting interval field value
    int16_t reportableChange; //!<Reporting change field value
    ZCL_ReportTime_t timeoutPeriod; //!<Timeout period field value
  } measuredValue ;

  struct PACK {
    ZCL_AttributeId_t id;
    uint8_t type;
    uint8_t reportable;
    int16_t value;
  } minMeasuredValue ;

  struct PACK {
    ZCL_AttributeId_t   id;
    uint8_t type;
    uint8_t reportable;
    int16_t value;
  } maxMeasuredValue ;
} ZCL_TemperatureMeasurementClusterAttributes_t ;
END_PACK


#endif /* _ZCLTEMPERATUREMEASUREMENT_H_ */
