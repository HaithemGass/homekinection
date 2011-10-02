/**************************************************************************//**
  \file zclZslMisc.h

  \brief
    ZSL miscellaneous functionality interface.

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2011, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
    History:
    19.03.10 A. Taradov - Created.
******************************************************************************/
#ifndef _ZCLZSLMISC_H
#define	_ZCLZSLMISC_H

/******************************************************************************
                    Includes section
******************************************************************************/
#include <zclZsl.h>
#include <zclZslIB.h>
#include <intrpData.h>

/******************************************************************************
                    Definitions section
******************************************************************************/

/******************************************************************************
                    Types section
******************************************************************************/

/******************************************************************************
                    Prototypes section
******************************************************************************/

/**************************************************************************//**
\brief Identify notification
\param[in] status - notification status
******************************************************************************/
extern void ZCL_ZslIdentifyInd(ZCL_ZslStatus_t status);

/**************************************************************************//**
\brief Reset ZSL miscellaneous module.
******************************************************************************/
void ZCL_ZslMiscReset(void);

/**************************************************************************//**
\brief Send Identify Request command.
\param[in] addr - destination device extended address pointer
\param[in] identifyTime - time in 1/10ths of a seconds
\returns true if command was sent
******************************************************************************/
bool ZCL_ZslIdentifyRequest(ExtAddr_t *addr, uint16_t identifyTime);

/**************************************************************************//**
\brief Send Reset To Factory New Request command.
\param[in] addr - destination short address
\returns true if command was sent.
******************************************************************************/
bool ZCL_ZslResetToFactoryNewRequest(ShortAddr_t addr);

/**************************************************************************//**
\brief INTRP-DATA.indication primitive
\param[in] ind - indication parameters
******************************************************************************/
void ZCL_ZslMiscDataInd(INTRP_DataInd_t *ind);

#endif // _ZCLZSLMISC_H

// eof zclZslMisc.h
