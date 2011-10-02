/***************************************************************************//**
  \file zclTimeCluster.h

  \brief
    The header file describes the ZCL Time Cluster and its interface

    The file describes the types and interface of the ZCL Time Cluster

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2011, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
    History:
    02.01.09 A. Potashov - Created.
*******************************************************************************/
#ifndef _ZCLTIMECLUSTER_H
#define _ZCLTIMECLUSTER_H

/*!
This cluster provides a basic interface to a real-time clock. The clock time may be
read and also written, in order to synchronize the clock (as close as practical) to a
time standard. This time standard is the number of seconds since 0 hrs 0 mins 0
sec on 1st January 2000 UTC (Universal Coordinated Time).
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
  \brief ZCL Time Cluster server side commands amount
*******************************************************************************/
#define ZCL_TIME_CLUSTER_SERVER_ATTRIBUTES_AMOUNT      2
//The Time and TimeStatus attributes

/***************************************************************************//**
  \brief ZCL Time Cluster client side commands amount
*******************************************************************************/
#define ZCL_TIME_CLUSTER_CLIENT_ATTRIBUTES_AMOUNT      0
//There is no any attributes at client cluster side

/***************************************************************************//**
  \brief ZCL Time Cluster commands amount
*******************************************************************************/
#define ZCL_TIME_CLUSTER_COMMANDS_AMOUNT        0
//There is no any commands at client cluster side


//!ZCL Time Cluster server side Time attribute id
#define ZCL_TIME_CLUSTER_SERVER_TIME_ATTRIBUTE_ID             CCPU_TO_LE16(0x0000)
//!ZCL Time Cluster server side TimeStatus attribute id
#define ZCL_TIME_CLUSTER_SERVER_TIME_STATUS_ATTRIBUTE_ID      CCPU_TO_LE16(0x0001)
//!ZCL Time Cluster server side TimeZone attribute id
#define ZCL_TIME_CLUSTER_SERVER_TIME_ZONE_ATTRIBUTE_ID        CCPU_TO_LE16(0x0002)
//!ZCL Time Cluster server side DstStart attribute id
#define ZCL_TIME_CLUSTER_SERVER_DST_START_ATTRIBUTE_ID        CCPU_TO_LE16(0x0003)
//!ZCL Time Cluster server side DstEnd attribute id
#define ZCL_TIME_CLUSTER_SERVER_DST_END_ATRIBUTE_ID           CCPU_TO_LE16(0x0004)
//!ZCL Time Cluster server side DstShift attribute id
#define ZCL_TIME_CLUSTER_SERVER_DST_SHIFT_ATTRIBUTE_ID        CCPU_TO_LE16(0x0005)
//!ZCL Time Cluster server side StandardTime attribute id
#define ZCL_TIME_CLUSTER_SERVER_STANDARD_TIME_ATTRIBUTE_ID    CCPU_TO_LE16(0x0006)
//!ZCL Time Cluster server side LocalTime attribute id
#define ZCL__TIME_CLUSTER_SERVER_LOCAL_TIME_ATTRIBUTE_ID      CCPU_TO_LE16(0x0007)


/***************************************************************************//**
  \brief ZCL Time Cluster server side attributes defining macros

  This macros should be used for ZCL Time Cluster server side attributes defining.

  \return None

  \internal
  //The typical usage is:
  //Time Cluster server side related attributes
  ZCL_TimeClusterServerAttributes_t timeClusterAttributes = ZCL_DEFINE_TIME_CLUSTER_SERVER_ATTRIBUTES();
*******************************************************************************/
#define ZCL_DEFINE_TIME_CLUSTER_SERVER_ATTRIBUTES()                                                                 \
  DEFINE_ATTRIBUTE(time,        ZCL_TIME_CLUSTER_SERVER_TIME_ATTRIBUTE_ID,         ZCL_UTC_TIME_DATA_TYPE_ID),      \
  DEFINE_ATTRIBUTE(timeStatus,  ZCL_TIME_CLUSTER_SERVER_TIME_STATUS_ATTRIBUTE_ID,  ZCL_8BIT_BITMAP_DATA_TYPE_ID)    \

/***************************************************************************//**
  \brief ZCL Time Cluster server side defining macros

  This macros should be used with #DEFINE_ZCL_TIME_CLUSTER_SERVER_ATTRIBUTES for
  ZCL Time Cluster server side defining in application.

  \param attributes - pointer to cluster server attributes (ZCL_TimeClusterServerAttributes_t)

  \return None

  \internal
  //The typical code is:
  //Time Cluster server side related attributes
  ZCL_TimeClusterServerAttributes_t timeClusterServerAttributes = DEFINE_ZCL_TIME_CLUSTER_SERVER_ATTRIBUTES();
  ZCL_Cluster_t myClusters[] =
  {
    ZCL_DEFINE_TIME_CLUSTER_SERVER(&timeClusterServerAttributes),
    //... Any other cluster defining ...
  }
*******************************************************************************/
#define TIME_CLUSTER_ZCL_SERVER_CLUSTER_TYPE(clattributes, clcommands)                                                    \
  {                                                                                                                       \
    .id = TIME_CLUSTER_ID,                                                                                                \
    .options = {.type = ZCL_SERVER_CLUSTER_TYPE, .security = ZCL_APPLICATION_LINK_KEY_CLUSTER_SECURITY, .ackRequest = 1}, \
    .attributesAmount = ZCL_TIME_CLUSTER_SERVER_ATTRIBUTES_AMOUNT,                                                        \
    .attributes = (uint8_t *) clattributes,                                                                               \
    .commandsAmount = ZCL_TIME_CLUSTER_COMMANDS_AMOUNT,                                                                   \
    .commands = (uint8_t *) clcommands                                                                                    \
  }

/***************************************************************************//**
  \brief ZCL Time Cluster client side defining macros

  This macros should be used for ZCL Time Cluster client side defining in application.

  \return None

  \internal
  //The typical code is:
  ZCL_Cluster_t myClusters[] =
  {
    ZCL_DEFINE_TIME_CLUSTER_CLIENT(),
    //... Any other cluster defining ...
  }
*******************************************************************************/
#define TIME_CLUSTER_ZCL_CLIENT_CLUSTER_TYPE(clattributes, clcommands)                                                    \
  {                                                                                                                       \
    .id = TIME_CLUSTER_ID,                                                                                                \
    .options = {.type = ZCL_CLIENT_CLUSTER_TYPE, .security = ZCL_APPLICATION_LINK_KEY_CLUSTER_SECURITY, .ackRequest = 1}, \
    .attributesAmount = ZCL_TIME_CLUSTER_CLIENT_ATTRIBUTES_AMOUNT,                                                        \
    .attributes = (uint8_t *) clattributes,                                                                               \
    .commandsAmount = ZCL_TIME_CLUSTER_COMMANDS_AMOUNT,                                                                   \
    .commands = (uint8_t *) clcommands                                                                                    \
  }

#define DEFINE_TIME_CLUSTER(cltype, clattributes, clcommands) TIME_CLUSTER_##cltype(clattributes, clcommands)

/*******************************************************************************
                   Types section
*******************************************************************************/

BEGIN_PACK
/***************************************************************************//**
  \brief
  ZCL Time Cluster attributes
*******************************************************************************/
typedef struct PACK
{
  /*!
  \brief Time attribute (R/W, M)

    The Time attribute is 32-bits in length and holds the time value of a real time
  clock. This attribute has data type UTCTime, but note that it may not actually be
  synchronised to UTC - see discussion of the TimeStatus attribute below.
    If the Master bit of the TimeStatus attribute has a value of 0, writing to this
  attribute shall set the real time clock to the written value, otherwise it cannot be
  written. The value 0xffffffff indicates an invalid time.
  */
  struct PACK
  {
    ZCL_AttributeId_t   id;           //!<Attribute Id (0x0000)
    uint8_t             type;         //!<Attribute type (UTCTime - 32-bit) (typeId = 0xe2, #ZCL_UTC_TIME_DATA_TYPE_ID)
    uint8_t             reportable;   //!<Reportable property (not reportable, should be 0)
    ZCL_UTCTime_t       value;        //!<Attribute value (default none)
  } time;

  /*!
  \brief TimeStatus attribute (R/W, M)

    The Master and Synchronized bits together provide information on how closely
  the Time attribute conforms to the time standard.
    The Master bit specifies whether the real time clock corresponding to the Time
  attribute is internally set to the time standard. This bit is not writeable
  if a value is written to the TimeStatus attribute, this bit does not change.
    The Synchronized bit specifies whether Time has been set over the network to
  synchronize it (as close as may be practical) to the time standard. This
  bit must be explicitly written to indicate this i.e. it is not set automatically on
  writing to the Time attribute. If the Master bit is 1, the value of this bit is 0.
    If both the Master and Synchronized bits are 0, the real time clock has no defined
  relationship to the time standard (e.g. it may record the number of seconds since
  the device was initialized).
    The MasterZoneDst bit specifies whether the TimeZone, DstStart, DstEnd and
  DstShift attributes are set internally to correct values for the location of the clock.
  If not, these attributes need to be set over the network. This bit is not writeable if
  a value is written to the TimeStatus attribute, this bit does not change.
  */
  struct PACK
  {
    ZCL_AttributeId_t   id;               //!<Attribute Id (0x0001)
    uint8_t             type;             //!<Attribute type (8-bit bitmap, typeId = 0x18, #ZCL_8BIT_BITMAP_DATA_TYPE_ID)
    uint8_t             reportable;       //!<Reportable property (not reportable, should be 0)
    struct PACK                           //!<Attribute value (default 0b00000000)
    {
      LITTLE_ENDIAN_OCTET(4,(
        uint8_t           master        :1, //!<Master bit (1 - master clock, 0 - not master clock)
        uint8_t           synchronized  :1, //!<Synchronized bit (1- synchronized, 0 - not synchronized)
        uint8_t           masterZoneDst :1, //!<MasterZoneDst bit (1 - master for Time Zone and DST, 0 - not master for Time Zone and DST)
        uint8_t           reserved      :5  //!<Reserved field
      ))
    } value;
  } timeStatus;
} ZCL_TimeClusterServerAttributes_t;
END_PACK
#endif // #ifndef _ZCLTIMECLUSTER_H

//eof zclTimeCluster.h
