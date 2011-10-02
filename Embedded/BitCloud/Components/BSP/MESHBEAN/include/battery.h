/**************************************************************************//**
\file  battery.h

\brief Interface to the battery sensor.

\author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2011, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

\internal
  History:
    29/06/07 E. Ivanov - Created
******************************************************************************/

#ifndef _BATTERY_H
#define _BATTERY_H

/******************************************************************************
                   Includes section
******************************************************************************/
// \cond
#include <types.h>
#include <sensors.h>
// \endcond

/******************************************************************************
                   Prototypes section
******************************************************************************/
/**************************************************************************//**
\brief  Opens the component to use.
\return
  BC_SUCCESS - the component is ready to been use. \n
  BC_FAIL - otherwise.
******************************************************************************/
result_t openBattery(void);

/**************************************************************************//**
\brief  Closes component.
\return
  BC_SUCCESS - always.
******************************************************************************/
result_t closeBattery(void);

/**************************************************************************//**
\brief  Starts ADC request on battery channel.
\param[in]
    callback - callback method.
\param[in]
    data - battery data.
    Can use (4ul * data * 125ul * 3ul) / (1024ul * 100ul) formula to count \n
    battery data in Volts.
\return
    BC_FAIL - battery component was not opened.
    BC_SUCCESS - other case.
******************************************************************************/
result_t readBatteryData(BspBatteryCb_t);

#endif /* _BATTERY_H */
// eof battery.h

