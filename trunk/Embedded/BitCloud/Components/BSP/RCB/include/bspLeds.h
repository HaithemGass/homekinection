/***************************************************************************//**
\file  bspLeds.h

\brief Declaration of leds defines.

\author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2011, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

\internal
  History:
    26.08.09 A. Taradov - Created
*******************************************************************************/

#ifndef _BSPLEDS_H
#define _BSPLEDS_H

/******************************************************************************
                   Includes section
******************************************************************************/
// \cond
#include <gpio.h>
#include <leds.h>
// \endcond

/******************************************************************************
                   Define(s) section
******************************************************************************/

#define halInitFirstLed()       GPIO_E2_make_out()
#define halUnInitFirstLed()     GPIO_E2_make_in()
#define halOnFirstLed()         GPIO_E2_clr()
#define halOffFirstLed()        GPIO_E2_set()
#define halReadFirstLed()       GPIO_E2_read()
#define halToggleFirstLed()     GPIO_E2_toggle()

#define halInitSecondLed()      GPIO_E3_make_out()
#define halUnInitSecondLed()    GPIO_E3_make_in()
#define halOnSecondLed()        GPIO_E3_clr()
#define halOffSecondLed()       GPIO_E3_set()
#define halReadSecondLed()      GPIO_E3_read()
#define halToggleSecondLed()    GPIO_E3_toggle()

#define halInitThirdLed()       GPIO_E4_make_out()
#define halUnInitThirdLed()     GPIO_E4_make_in()
#define halOnThirdLed()         GPIO_E4_clr()
#define halOffThirdLed()        GPIO_E4_set()
#define halReadThirdLed()       GPIO_E4_read()
#define halToggleThirdLed()     GPIO_E4_toggle()

#endif /*_BSPLEDS_H*/
// eof bspLeds.h
