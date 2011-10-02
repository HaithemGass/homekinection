/************************************************************************//**
  \file zclMemoryManager.h

  \brief
    The header file describes the ZCL Simple Metering Cluster

    The header file describes the ZCL Simple Metering Cluster

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2011, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
    History:
    27.11.08 A. Potashov - Created.
******************************************************************************/

#ifndef _SIMPLEMETERINGCLUSTER_H
#define _SIMPLEMETERINGCLUSTER_H

#include <zcl.h>
#include <clusters.h>

#define CURRENT_SUMMATION_DELIVERED_ATTRIBUTE_ID 0x00

/*************************************************************************//**
  \brief Simple Metering Cluster attributes amount
*****************************************************************************/
#define SIMPLE_METERING_CLUSTER_CLIENT_ATTRIBUTES_AMOUNT     0
#define SIMPLE_METERING_CLUSTER_SERVER_ATTRIBUTES_AMOUNT     5

/*************************************************************************//**
  \brief Simple Metering Cluster commands amount
*****************************************************************************/
#define SIMPLE_METERING_CLUSTER_CLIENT_COMMANDS_AMOUNT       0
#define SIMPLE_METERING_CLUSTER_SERVER_COMMANDS_AMOUNT       0

#define DEFINE_SIMPLE_METERING_SERVER_ATTRIBUTES(min, max) \
  DEFINE_REPORTABLE_ATTRIBUTE(currentSummationDelivered, CCPU_TO_LE16(0x0000), ZCL_U48BIT_DATA_TYPE_ID, min, max), \
  DEFINE_ATTRIBUTE(meterStatus,          CCPU_TO_LE16(0x0200), ZCL_8BIT_BITMAP_DATA_TYPE_ID),      \
  DEFINE_ATTRIBUTE(unitofMeasure,        CCPU_TO_LE16(0x0300), ZCL_8BIT_ENUM_DATA_TYPE_ID),        \
  DEFINE_ATTRIBUTE(summationFormatting,  CCPU_TO_LE16(0x0303), ZCL_8BIT_BITMAP_DATA_TYPE_ID),      \
  DEFINE_ATTRIBUTE(meteringDeviceType,   CCPU_TO_LE16(0x0306), ZCL_8BIT_ENUM_DATA_TYPE_ID)

#define SIMPLE_METERING_CLUSTER_ZCL_CLIENT_CLUSTER_TYPE(clattributes, clcommands) \
  { \
    .id = SIMPLE_METERING_CLUSTER_ID, \
    .options = {.type = ZCL_CLIENT_CLUSTER_TYPE, .security = ZCL_APPLICATION_LINK_KEY_CLUSTER_SECURITY, .ackRequest = 1}, \
    .attributesAmount = SIMPLE_METERING_CLUSTER_CLIENT_ATTRIBUTES_AMOUNT, \
    .attributes = (uint8_t *)clattributes, \
    .commandsAmount = SIMPLE_METERING_CLUSTER_CLIENT_COMMANDS_AMOUNT, \
    .commands = NULL \
  }

#define SIMPLE_METERING_CLUSTER_ZCL_SERVER_CLUSTER_TYPE(clattributes, clcommands) \
  { \
    .id = SIMPLE_METERING_CLUSTER_ID, \
    .options = {.type = ZCL_SERVER_CLUSTER_TYPE, .security = ZCL_APPLICATION_LINK_KEY_CLUSTER_SECURITY, .ackRequest = 1}, \
    .attributesAmount = SIMPLE_METERING_CLUSTER_SERVER_ATTRIBUTES_AMOUNT, \
    .attributes = (uint8_t *)clattributes, \
    .commandsAmount = SIMPLE_METERING_CLUSTER_SERVER_COMMANDS_AMOUNT, \
    .commands = NULL \
  }

#define DEFINE_SIMPLE_METERING_CLUSTER(cltype, clattributes, clcommands) SIMPLE_METERING_CLUSTER_##cltype(clattributes, clcommands)

BEGIN_PACK

/*************************************************************************//**
  \brief ZCL Simple Metering Cluster Descriptor

  For internal use
*****************************************************************************/
typedef struct PACK
{
  //!Reading information attribute set (Id = 0x00)
  //!Current Summation Delivered Attribute descriptor
  struct PACK
  {
    ZCL_AttributeId_t id;                         //!<Attribute Id (0x0000)
    uint8_t           type;                       //!<Attribute data type (Unsignet 48 bit Integer)
    uint8_t           reportable;                 //!<Reportable prroperty
    uint8_t           value[6];                   //!<Attribute value
    ZCL_ReportTime_t  reportCounter;              //!<For internal use only
    ZCL_ReportTime_t  minReportInterval;          //!<Minimum reporting interval field value
    ZCL_ReportTime_t  maxReportInterval;          //!<Maximum reporting interval field value
    uint8_t           reportableChange[6];        //!<Reporting change field value
    ZCL_ReportTime_t  timeoutPeriod;              //!<Timeout period field value
  } currentSummationDelivered;

  //!Meter status attribute set (Id = 0x02)
  //!Meter Status Attribute descriptor
  struct PACK
  {
    ZCL_AttributeId_t id;                         //!<Attribute Id (0x0200)
    uint8_t           type;                       //!<Attribute data type (8 bit Bitmap)
    uint8_t           reportable;                 //!<Reportable property
    //!Attribute value
    struct PACK
    {
      LITTLE_ENDIAN_OCTET(8,(
        uint8_t         checkMeter            :1,   //!<Check Meter
        uint8_t         lowBattery            :1,   //!<Low Battery
        uint8_t         tamperDetect          :1,   //!<Tamper Detect
        uint8_t         powerFailure          :1,   //!<Power Failure
        uint8_t         powerQuality          :1,   //!<Power Quality
        uint8_t         leakDetect            :1,   //!<Leak Detect
        uint8_t         serviceDisconnectOpen :1,   //!<Service Disconnect Open
        uint8_t         reserved              :1    //!<Reserved
      ))
    } value;
  } meterStatus;

  //!Formating attribute set (Id = 0x03)
  //!UnitofMeasure Attribute descriptor
  struct PACK
  {
    ZCL_AttributeId_t id;                         //!<Attribute Id (0x0300)
    uint8_t           type;                       //!<Attribute type (8-bit Enumeration)
    uint8_t           reportable;                 //!<Reportable property
    uint8_t           value;              //!<Attribute value
  } unitofMeasure;

  //!Summation Formatting Attribute descriptor
  struct PACK
  {
    ZCL_AttributeId_t id;                         //!<Attribute Id (0x0303)
    uint8_t           type;                       //!<Attribute type (8 bit Bitmap)
    uint8_t           reportable;                 //!<Reportable property
    /**
    \brief Attribute value
    Summation Formatting provides a method to properly decipher the number of digits
    and the decimal location of the values found in the Summation Information Set
    of the attributes.
    */
    struct PACK
    {
      LITTLE_ENDIAN_OCTET(3,(
        uint8_t         right   :3,                 //!<Number of Digits to the right of the Decimal Point
        uint8_t         left    :4,                 //!<Number of Digits to the left of the Decimal Point
        uint8_t         zeros   :1                  //!<If set, suppress leading zeros
      ))
    } value;
  } summationFormatting;

  //!Metering Device Type Attribute descriptor
  struct PACK
  {
    ZCL_AttributeId_t id;                         //!<Attribute Id (0x0306)
    uint8_t type;                                 //!<Attribute type (8 bit Bitmap)
    uint8_t reportable;                           //!<Reportable property
    /**
      \brief Attribute value
      MeteringDeviceType provides a lable for identifying of metering device present.
      The attribute are enumerated values presenting Energy, GAs, Water, Thermal, and
      mirrored metering device.
    */
    struct PACK
    {
      LITTLE_ENDIAN_OCTET(6,(
        uint8_t         electricMetering  :1,       //!<Electric Metering     (0)
        uint8_t         gasMetering       :1,       //!<Gas Metering          (1)
        uint8_t         waterMetering     :1,       //!<Water Metering        (2)
        uint8_t         thermalMetering   :1,       //!<Thermal Metering      (3)
        uint8_t         pressureMetering  :1,       //!<Pressure Metering     (4)
        uint8_t         reserved          :3        //!<Reserved
      ))
    } value;
  } meteringDeviceType;
} SimpleMeteringServerClusterAttributes_t;
END_PACK
#endif // _SIMPLEMETERINGCLUSTER_H
