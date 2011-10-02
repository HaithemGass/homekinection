/**************************************************************************//**
\file  lm73.h

\brief Declarations of lm73 interface.

\author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2011, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

\internal
  History:
    29/05/07 E. Ivanov - Created
*******************************************************************************/

#ifndef _LM73_SENSOR_H
#define _LM73_SENSOR_H

/******************************************************************************
                   Includes section
******************************************************************************/
#include <types.h>

/******************************************************************************
                   Prototypes section
******************************************************************************/
/**************************************************************************//**
\brief Opens the component to use.

\return
  BC_SUCCESS - the component is ready to been use. \n
  BC_FAIL - otherwise.
******************************************************************************/
result_t openLm73(void);

/**************************************************************************//**
\brief Performs the test if the component have completed request.

\return
  BC_FAIL - the previous request is not completed.
  BC_SUCCESS - otherwise.
******************************************************************************/
result_t closeLm73(void);

/**************************************************************************//**
\brief Reads data from lm73 sensor.
\param[in]
  f - callback method.
\param[in]
  result - the result of the requested operation.
           true - operation finished successfully,
           false - some error has occured.
\param[in]
  data - sensor data.
\return
  BC_FAIL - the previous request was not completed,
         the address of callback is 0, i2c interface is busy,
         there is error on i2c interface.
  BC_SUCCESS - other case.
******************************************************************************/
result_t readLm73Data(void (*f)(bool result, int16_t data));

#endif /* _LM73_SENSOR_H */
// eof lm73.h
