/************************************************************************//**
  \file zclPriceCluster.h

  \brief
    The header file describes the Price Cluster.

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2011, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
    History:
    05.02.09 A. Mandychev - Changed.
******************************************************************************/

#ifndef _ZCLPRICECLUSTER_H
#define _ZCLPRICECLUSTER_H

/******************************************************************************
                        Includes section
******************************************************************************/
#include <zcl.h>
#include <clusters.h>

/******************************************************************************
                        Defines section
******************************************************************************/
/**
\brief Macros defines duration of Publish Price command,
 * which means 'until change'.
 * According to ZB AMI Spec r15, D.4.2.4.1.1, page 197.
*/
#define PUBLISH_PRICE_DURATION_UNTIL_CHANGED (0xFFFF)

/**
\brief Macros defines start time of Publish Price command,
 * which means 'now'.
 * According to ZB AMI Spec r15, D.4.2.4.1.1, page 197.
*/
#define PUBLISH_PRICE_START_TIME_NOW (0x0000)

/**
\brief Macros defines Price Ratio field special value,
 * which means 'not used'.
 * According to ZB AMI Spec r15, D.4.2.4.1.1, page 198.
*/
#define PUBLISH_PRICE_PRICE_RATIO_IS_NOT_USED (0xFF)

/**
\brief Macros defines Generation Price Ratio field special value,
 * which means 'not used'.
 * According to ZB AMI Spec r15, D.4.2.4.1.1, page 198.
*/
#define PUBLISH_PRICE_GENERATION_PRICE_RATIO_IS_NOT_USED (0xFF)

/**
\brief Macros defines Generation Price field special value,
 * which means 'not used'.
 * According to ZB AMI Spec r15, D.4.2.4.1.1, page 198.
*/
#define PUBLISH_PRICE_GENERATION_PRICE_IS_NOT_USED (0xFFFFFFFF)

/**
\brief Macros defines Number Of Block Thresholds field special value,
 * which means 'not used'.
 * According to ZigBee-095310r23ZB, page 65.
*/
#define PUBLISH_NUMBER_OF_BLOCK_THRESHOLDS_IS_NOT_USED (0xFF)

/**
\brief Macros defines Price Control field special value,
 * which means 'not used'.
 * According to ZigBee-095310r23ZB, page 65.
*/
#define PUBLISH_PRICE_CONTROL_IS_NOT_USED (0x00)

/**
\brief Macros defines Standing Charge field special value,
 * which means 'not used'.
 * According to ZigBee-095310r23ZB, page 65.
*/
#define PUBLISH_STANDING_CHARGE_IS_NOT_USED (0xFFFFFFFF)

/**
 * \brief Price Server Cluster attributes amount.
*/
#define PRICE_CLUSTER_SERVER_ATTRIBUTES_AMOUNT     0

/**
 * \brief Price Client Cluster attributes amount.
*/
#define PRICE_CLUSTER_CLIENT_ATTRIBUTES_AMOUNT     0

/**
 * \brief Price Client Cluster commands amount.
*/
#define PRICE_CLUSTER_COMMANDS_AMOUNT  3

/**
 * \brief Price Server Cluster commands identifiers.
*/
#define PUBLISH_PRICE_COMMAND_ID          0x00

/**
 * \brief Price Client Cluster commands identifiers.
*/
#define GET_CURRENT_PRICE_COMMAND_ID      0x00
#define GET_SCHEDULED_PRICES_COMMAND_ID   0x01

#define RATE_LABEL_MAX_LEN 12

/*
#define DEMAND_RESPONSE_CLUSTER_SERVER_COMMANDS(publishPriceInd) \
  DEFINE_COMMAND(publishPriceCommand, 0x00, COMMAND_OPTIONS(SERVER_TO_CLIENT), publishPriceInd)


#define DEMAND_RESPONSE_CLUSTER_CLIENT_COMMANDS(getCurrentPriceInd) \
  DEFINE_COMMAND(getCurrentPriceCommand, 0x00, COMMAND_OPTIONS(CLIENT_TO_SERVER), getCurrentPriceInd)
*/

#define PRICE_CLUSTER_COMMANDS(getCurrentPriceInd, publishPriceInd, getScheduledPricesInd) \
    DEFINE_COMMAND(getCurrentPriceCommand, 0x00, COMMAND_OPTIONS(CLIENT_TO_SERVER, ZCL_THERE_IS_RELEVANT_RESPONSE), getCurrentPriceInd), \
    DEFINE_COMMAND(publishPriceCommand, 0x00, COMMAND_OPTIONS(SERVER_TO_CLIENT, ZCL_THERE_IS_NO_RELEVANT_RESPONSE), publishPriceInd), \
    DEFINE_COMMAND(getScheduledPricesCommand, GET_SCHEDULED_PRICES_COMMAND_ID, COMMAND_OPTIONS(CLIENT_TO_SERVER, ZCL_THERE_IS_RELEVANT_RESPONSE), getScheduledPricesInd)

#define PRICE_CLUSTER_ZCL_CLIENT_CLUSTER_TYPE(clattributes, clcommands) \
  { \
    .id = PRICE_CLUSTER_ID, \
    .options = {.type = ZCL_CLIENT_CLUSTER_TYPE, .security = ZCL_APPLICATION_LINK_KEY_CLUSTER_SECURITY, .ackRequest = 1}, \
    .attributesAmount = PRICE_CLUSTER_CLIENT_ATTRIBUTES_AMOUNT, \
    .attributes = (uint8_t *)clattributes, \
    .commandsAmount = PRICE_CLUSTER_COMMANDS_AMOUNT, \
    .commands = (uint8_t *)clcommands \
  }

#define PRICE_CLUSTER_ZCL_SERVER_CLUSTER_TYPE(clattributes, clcommands) \
  { \
    .id = PRICE_CLUSTER_ID, \
    .options = {.type = ZCL_SERVER_CLUSTER_TYPE, .security = ZCL_APPLICATION_LINK_KEY_CLUSTER_SECURITY, .ackRequest = 1}, \
    .attributesAmount = PRICE_CLUSTER_SERVER_ATTRIBUTES_AMOUNT, \
    .attributes = (uint8_t *)clattributes, \
    .commandsAmount = PRICE_CLUSTER_COMMANDS_AMOUNT, \
    .commands = (uint8_t *)clcommands \
  }
#define DEFINE_PRICE_CLUSTER(cltype, clattributes, clcommands) PRICE_CLUSTER_##cltype(clattributes, clcommands)

/******************************************************************************
                    Types section
******************************************************************************/
BEGIN_PACK
/**
 * \brief Publish Price Command Payload format.
*/
typedef struct PACK
{
  uint32_t        providerId;
  uint8_t         rateLabelLength;
  uint8_t         rateLabel[RATE_LABEL_MAX_LEN];
  uint32_t        issuerEventId;
  ZCL_UTCTime_t   currentTime;
  uint8_t         unitOfMeasure;
  uint16_t        currency;
  struct PACK
  {
    LITTLE_ENDIAN_OCTET(2,(
      uint8_t priceTrailDigit : 4,
      uint8_t priceTier : 4
    ))
  } priceTrailDigitPriceTier;
  struct PACK
  {
    LITTLE_ENDIAN_OCTET(2,(
      uint8_t numberOfPriceTiers : 4,
      uint8_t registerTier : 4
    ))
  } numberOfPriceTiersRegisterTier;
  ZCL_UTCTime_t   startTime;
  uint16_t        durationInMinutes;
  uint32_t        price;
  uint8_t         priceRatio;
  uint32_t        generationPrice;
  uint8_t         generationPriceRatio;
  uint32_t        alternateCostDelivered;
  uint8_t         alternateCostUnit;
  uint8_t         alternateCostTrailingDigit;
  uint8_t         numberOfBlockThresholds;
  uint8_t         priceControl;
  uint32_t        standingCharge;
} ZCL_PublishPrice_t;

/**
 * \brief Get Current Price Command Payload format.
*/
typedef struct PACK
{
  LITTLE_ENDIAN_OCTET(2,(
    uint8_t requestorRxOnWhenIdle :1, //!<The requestor Rx On When Idle sub-field
    uint8_t reserved              :7  //!<Reserved field
  ))
} ZCL_GetCurrentPrice_t;

/**
 * \brief Get Scheduled Prices Command Payload format.
*/
typedef struct PACK
{
  ZCL_UTCTime_t   startTime;
  uint8_t         numberOfEvents;
} ZCL_GetScheduledPrices_t;
END_PACK


/**
 * \brief ZCL Price Cluster Commands
 */
typedef struct
{
  //! Get Current Price Command descriptor
  struct
  {
    ZCL_CommandId_t       id;       //!<Command Id (0x00)
    ZclCommandOptions_t   options;  //!<Command options
    /**
      \brief Command indication handler
      \param payloadLength - length of payload in cotets
      \param payload - payload
      \return none
    */
    ZCL_Status_t (*getCurrentPrice)(ZCL_Addressing_t *addressing, uint8_t payloadLength, ZCL_GetCurrentPrice_t *payload);
  } getCurrentPriceCommand;

  //! Get Scheduled Prices Command descriptor
  struct
  {
    ZCL_CommandId_t       id;       //!<Command Id (0x01)
    ZclCommandOptions_t   options;  //!<Command options
    /**
      \brief Command indication handler
      \param payloadLength - length of payload in octets
      \param payload - payload
      \return none
    */
    ZCL_Status_t (*getScheduledPrices)(ZCL_Addressing_t *addressing, uint8_t payloadLength, ZCL_GetScheduledPrices_t *payload);
  } getScheduledPricesCommand;

  //! Publish Price Command descriptor
  struct
  {
    ZCL_CommandId_t       id;       //!<Command Id (0x00)
    ZclCommandOptions_t   options;  //!<Command options
    /**
      \brief Command indication handler
      \param payloadLength - length of payload in cotets
      \param payload - payload
      \return none
    */
    ZCL_Status_t (*publishPrice)(ZCL_Addressing_t *addressing, uint8_t payloadLength, ZCL_PublishPrice_t *payload);
  } publishPriceCommand;

} ZCL_PriceClusterCommands_t;



#endif // _ZCLPRICECLUSTER_H
