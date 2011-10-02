/***************************************************************************//**
  \file  bspDbg.h

  \brief Assert codes for BSP

  \author
      Atmel Corporation: http://www.atmel.com \n
      Support email: avr@atmel.com

    Copyright (c) 2008-2011, Atmel Corporation. All rights reserved.
    Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
    History:
      10/06/10 D. Loskutnikov - Created
 ******************************************************************************/
/******************************************************************************
 *   WARNING: CHANGING THIS FILE MAY AFFECT CORE FUNCTIONALITY OF THE STACK.  *
 *   EXPERT USERS SHOULD PROCEED WITH CAUTION.                                *
 ******************************************************************************/

#ifndef _BSPDBG_H
#define _BSPDBG_H

/******************************************************************************
                   Includes section
******************************************************************************/
#include <dbg.h>

/******************************************************************************
                   Definitions section
******************************************************************************/
enum
{
  TSL2550_UNEXPECTED_STATE = 0x9000,
};

#endif /* _BSPDBG_H */

// eof bspDbg.h
