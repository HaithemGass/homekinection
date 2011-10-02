/*****************************************************************************
  \file  zclPowerConfigurationCluster.h

  \brief
    The file describes the types and interface of the Power Configuration cluster

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2011, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
  History:
    18.01.2010 I.Vagulin - Created
******************************************************************************/


#ifndef _ZCLPOWERCONFIGURATIONCLUSTER_H_
#define _ZCLPOWERCONFIGURATIONCLUSTER_H_

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
#define ZCL_POWER_CONFIGURATION_CLUSTER_SERVER_ATTRIBUTES_AMOUNT 2

/**
* \brief server commands amount
*/
#define ZCL_POWER_CONFIGURATION_CLUSTER_SERVER_COMMANDS_AMOUNT 0

/**
* \brief client attributes amount
*/
#define ZCL_POWER_CONFIGURATION_CLUSTER_CLIENT_ATTRIBUTES_AMOUNT 0

/**
* \brief client commands amount
*/
#define ZCL_POWER_CONFIGURATION_CLUSTER_CLIENT_COMMANDS_AMOUNT 0


/**
* \brief Supported attribue id
*/
#define ZCL_POWER_CONFIGURATION_CLUSTER_SERVER_MAINS_VOLTAGE_ATTRIBUTE_ID   CCPU_TO_LE16(0x0000)
#define ZCL_POWER_CONFIGURATION_CLUSTER_SERVER_MAINS_FREQUENCY_ATTRIBUTE_ID CCPU_TO_LE16(0x0001)

/**
 * \brief server define attributes macros
*/
#define ZCL_DEFINE_POWER_CONFIGURATION_CLUSTER_SERVER_ATTRIBUTES() \
  DEFINE_ATTRIBUTE(mainsVoltage,  ZCL_POWER_CONFIGURATION_CLUSTER_SERVER_MAINS_VOLTAGE_ATTRIBUTE_ID,  ZCL_U16BIT_DATA_TYPE_ID), \
  DEFINE_ATTRIBUTE(mainsFrequency, ZCL_POWER_CONFIGURATION_CLUSTER_SERVER_MAINS_FREQUENCY_ATTRIBUTE_ID, ZCL_U8BIT_DATA_TYPE_ID)

#define ZCL_DEFINE_POWER_CONFIGURATION_CLUSTER_SERVER(clattributes) \
  { \
    .id = POWER_CONFIGURATION_CLUSTER_ID, \
    .options = {.type = ZCL_SERVER_CLUSTER_TYPE, .security = ZCL_NETWORK_KEY_CLUSTER_SECURITY, .ackRequest = 1}, \
    .attributesAmount = ZCL_POWER_CONFIGURATION_CLUSTER_SERVER_ATTRIBUTES_AMOUNT, \
    .attributes = (uint8_t *) (clattributes), \
    .commandsAmount = ZCL_POWER_CONFIGURATION_CLUSTER_SERVER_COMMANDS_AMOUNT, \
    .commands = NULL \
  }

#define ZCL_DEFINE_POWER_CONFIGURATION_CLUSTER_CLIENT() \
  { \
    .id = POWER_CONFIGURATION_CLUSTER_ID, \
    .options = {.type = ZCL_CLIENT_CLUSTER_TYPE, .security = ZCL_NETWORK_KEY_CLUSTER_SECURITY, .ackRequest = 1}, \
    .attributesAmount = ZCL_POWER_CONFIGURATION_CLUSTER_CLIENT_ATTRIBUTES_AMOUNT, \
    .attributes = NULL, \
    .commandsAmount = ZCL_POWER_CONFIGURATION_CLUSTER_CLIENT_COMMANDS_AMOUNT, \
    .commands = NULL \
  }

/******************************************************************************
                    Types section
******************************************************************************/
BEGIN_PACK
typedef struct PACK {
  struct PACK {
    ZCL_AttributeId_t   id;
    uint8_t             type;
    uint8_t             reportable;
    uint16_t            value;
  } mainsVoltage ;

  struct PACK {
    ZCL_AttributeId_t   id;
    uint8_t             type;
    uint8_t             reportable;
    uint8_t             value;
  } mainsFrequency ;
} ZCL_PowerConfigurationClusterAttributes_t ;
END_PACK

#endif /* _ZCLPOWERCONFIGURATIONCLUSTER_H_ */
