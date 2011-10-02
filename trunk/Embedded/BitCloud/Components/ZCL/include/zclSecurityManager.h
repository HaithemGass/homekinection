/***************************************************************************//**
  \file zclSecurityManager.h

  \brief
    The header file describes the ZCL Security Manager and its interface

    The file describes the types and interface of the ZCL Security Manager

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2011, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
    History:
    17.03.09 A. Potashov - Created.
*******************************************************************************/

#ifndef _ZCLSECURITYMANAGER_H
#define _ZCLSECURITYMANAGER_H

#ifdef _ZCL_SECURITY_
#include <zcl.h>
#include <zdo.h>
#include <genericEcc.h>

typedef enum
{
  ZCL_SECURITY_STATUS_SUCCESS                = 0x00,
#ifdef _ZCL_KE_CLUSTER_
  ZCL_SECURITY_STATUS_DISCOVERY_FAIL         = 0x01,
  ZCL_SECURITY_STATUS_APS_BINDING_FAIL       = 0x02,
  ZCL_SECURITY_STATUS_TIMEOUT                = 0x03,
  ZCL_SECURITY_STATUS_TERMINATED             = 0x04,
  ZCL_SECURITY_STATUS_SEND_COMMAND_FAIL      = 0x05,
  ZCL_SECURITY_STATUS_INVALID_SETTINGS       = 0x06,
#endif //#ifdef _ZCL_KE_CLUSTER_
} ZCL_SecurityStatus_t;


/*************************************************************************//**
  Status of ZCL operation
  \brief
    Used to return status of ZCL operation.
*****************************************************************************/
/*
typedef enum
{
  ZCL_SUCCESS_STATUS                    = 0x00,
  ZCL_UNSUPPORTED_ATTRIBUTE_STATUS      = 0x01,
  ZCL_INVALID_ATTRIBUTE_VALUE_STATUS    = 0x02,
  ZCL_TC_PERMISSION_TABLE_ERROR_STATUS  = 0x03,
  ZCL_APS_LINK_KEY_ERROR_STATUS         = 0x04
} ZCL_Status_t;
*/

BEGIN_PACK

typedef struct PACK
{
  ExtAddr_t addr;                   //!<Destination node 64-bit IEEE address
  uint8_t   key[SECURITY_KEY_SIZE]; //!<APS Link Key
} ZCL_LinkKeyDesc_t;

typedef struct PACK
{
  ProfileId_t   profileId;
  uint8_t       additional[8];
} ZclKEProfileAttributeData_t;

typedef struct PACK
{
  uint8_t                     publicReconstrKey[SECT163K1_COMPRESSED_PUBLIC_KEY_SIZE];  //22-byte
  ExtAddr_t                   subject;                                                  //8-bytes
  ExtAddr_t                   issuer;                                                   //8-byte
  ZclKEProfileAttributeData_t profileAttributeData;                                     //10-byte
} ZclCertificate_t;

#ifdef _ZCL_KE_CLUSTER_
typedef struct PACK
{
  uint8_t publicKey[SECT163K1_COMPRESSED_PUBLIC_KEY_SIZE]; //!<CA Public Key shared between all nodes in network
  uint8_t privateKey[SECT163K1_PRIVATE_KEY_SIZE];          //!<Node secret key
  uint8_t certificate[SECT163K1_CERTIFICATE_SIZE     ];    //!<Node certificate derived from CA
} ZCL_CertificateDesc_t;

typedef struct PACK
{
  uint8_t           publicKey[SECT163K1_COMPRESSED_PUBLIC_KEY_SIZE];  //!<CA Public Key shared between all nodes in network
  uint8_t           privateKey[SECT163K1_PRIVATE_KEY_SIZE];           //!<Node secret key
  ZclCertificate_t  certificate;                                      //!<Node certificate derived from CA
} ZCL_KECertificateDescriptor_t;

#endif //#ifdef _ZCL_KE_CLUSTER_

END_PACK


/*************************************************************************//**
  Security variable identifier type
  \brief
    Used by functions ZCL_Set and ZCL_Get. To define variable to write or read.
*****************************************************************************/
typedef enum
{
#ifdef _ZCL_KE_CLUSTER_
  /* Certificate for Key Establishment */
  ZCL_CERTIFICATE_DESC_ID     = 0x00,
#endif //#ifdef _ZCL_KE_CLUSTER_
  /* Link key descriptor*/
  ZCL_LINK_KEY_DESC_ID        = 0x01,
  /* Not used */
  ZCL_REMOVE_LINK_KEY_ID      = 0x02,
#ifdef _ZCL_KE_CLUSTER_
  /* Endpoint asssigned for Key Establishment cluster */
  ZCL_KE_ACTIVE_ENDPOINT_ID   = 0x03,
#endif //#ifdef _ZCL_KE_CLUSTER_
  /* Network key */
  ZCL_NETWORK_KEY_ID          = 0x04
} ZCL_SecurityAttrId_t;

typedef union
{
#ifdef _ZCL_KE_CLUSTER_
  ZCL_CertificateDesc_t *certificateDesc;   //Used with ZCL_CERTIFICATE_DESC_ID
  Endpoint_t            endpoint;           //Used with ZCL_KE_ACTIVE_ENDPOINT_ID
#endif //#ifdef _ZCL_KE_CLUSTER_
  ZCL_LinkKeyDesc_t     *linkKeyDesc;       //Used with ZCL_LINK_KEY_DESC_ID
  uint8_t               *networkKey;        //Used with ZCL_NETWORK_KEY_ID
  ExtAddr_t             extAddr;            //Used with ZCL_REMOVE_LINK_KEY_ID
} ZCL_SecurityAttrValue_t;

typedef struct
{
  ZCL_SecurityAttrId_t    id;
  uint8_t                 length;
  ZCL_SecurityAttrValue_t value;
} ZCL_SecurityAttr_t;

typedef struct
{
  ZCL_SecurityAttr_t attr;
  ZCL_Status_t       status;
} ZCL_Set_t;

typedef struct
{
  ZCL_SecurityAttr_t attr;
  ZCL_Status_t       status;
} ZCL_Get_t;

typedef struct
{
  ZCL_SecurityStatus_t status;
} ZCL_StartSecurityConf_t;

typedef struct
{
  ZCL_StartSecurityConf_t confirm;
  void (* ZCL_StartSecurityConf)(ZCL_StartSecurityConf_t *conf);
} ZCL_StartSecurityReq_t;
//API

extern bool keSrvMode;
extern Endpoint_t clKEEndpoint;
extern Endpoint_t srvKEEndpoint;

void ZCL_ResetSecurity(void);

void ZCL_Set(ZCL_Set_t *req);

void ZCL_Get(ZCL_Get_t *req);

void ZCL_StartSecurityReq(ZCL_StartSecurityReq_t *req);

#ifdef _ZCL_KE_CLUSTER_
/*************************************************************************************//**
  \brief Notifies that Key Establishment is finished with status specified in parameters.

  \param status - status of Key Establishment.
******************************************************************************************/
void keNotification(ZCL_SecurityStatus_t status);
#endif

#endif //#ifdef _ZCL_SECURITY_
#endif //#ifndef _ZCLSECURITYMANAGER_H

//eof zclSecurityManager.h
