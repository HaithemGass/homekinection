/**************************************************************************//**
  \file  halSleep.c

  \brief Implementation of sleep modes.

  \author
      Atmel Corporation: http://www.atmel.com \n
      Support email: avr@atmel.com

    Copyright (c) 2008-2011, Atmel Corporation. All rights reserved.
    Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
    History:
      1/12/09 A. Khromykh - Created
 ******************************************************************************/
/******************************************************************************
 *   WARNING: CHANGING THIS FILE MAY AFFECT CORE FUNCTIONALITY OF THE STACK.  *
 *   EXPERT USERS SHOULD PROCEED WITH CAUTION.                                *
 ******************************************************************************/

/******************************************************************************
                   Includes section
******************************************************************************/
#include <sleepTimer.h>
#include <halSleepTimerClock.h>
#include <halRfPio.h>
#include <halRfCtrl.h>
#include <halSleep.h>
#include <halIrq.h>
#include <halDbg.h>
#include <halAppClock.h>
#include <halEeprom.h>

/******************************************************************************
                   Prototypes section
******************************************************************************/
/**************************************************************************//**
\brief Performs calibration of the main clock generator.
******************************************************************************/
void halStartingCalibrate(void);

#ifdef _HAL_RF_RX_TX_INDICATOR_
/**************************************************************************//**
\brief  Turn on pin 1 (DIG3) and pin 2 (DIG4) to indicate the transmit state of
the radio transceiver.
******************************************************************************/
void phyRxTxSwitcherOn(void);

/**************************************************************************//**
\brief  Turn off pin 1 (DIG3) and pin 2 (DIG4) to indicate the transmit state of
the radio transceiver.
******************************************************************************/
void phyRxTxSwitcherOff(void);

#endif

#ifdef _HAL_ANT_DIVERSITY_
/**************************************************************************//**
\brief  Turn on pin 9 (DIG1) and pin 10 (DIG2) to enable antenna select.
******************************************************************************/
void phyAntennaSwitcherOn(void);

/**************************************************************************//**
\brief  Turn off pin 9 (DIG1) and pin 10 (DIG2) to disable antenna select.
******************************************************************************/
void phyAntennaSwitcherOff(void);

#endif // _HAL_ANT_DIVERSITY_

/******************************************************************************
                   External global variables section
******************************************************************************/
extern volatile bool halEnableDtrWakeUp;
extern HalSleepControl_t halSleepControl;

/******************************************************************************
                   Global variables section
******************************************************************************/
static uint32_t halTimeStartOfSleep = 0ul;

/******************************************************************************
                   Implementations section
******************************************************************************/
/**************************************************************************//**
\brief Switch on system power.

\param[in]
  wakeupSource - wake up source
******************************************************************************/
void halPowerOn(const uint8_t wakeupSource)
{
  halSleepControl.wakeupStation = HAL_ACTIVE_MODE;
  halSleepControl.wakeupSource = wakeupSource;

  if (INTERNAL_RC == halGetClockSource())
  {
    GPIO_RF_SLP_TR_clr();
  }
  else
  {
    GPIO_RF_SLP_TR_make_in();
    TCCR2A &= ~((1 << COM2A1) | (1 << COM2A0)); // no compare
    while (ASSR & HAL_ASSR_FLAGS);
  }
  GPIO_RF_SLP_TR_make_out();

  #ifdef _HAL_USE_AMPLIFIER_
    // set one on pin. Enable power amplifier.
    GPIO_POW_AMPLF_SLP_set();
  #endif

  halPostTask4(HAL_WAKEUP);
}

/******************************************************************************
Shutdowns system.
  NOTES:
  the application should be sure the poweroff will not be
  interrupted after the execution of the sleep().
******************************************************************************/
void halPowerOff(void)
{
  if (HAL_ACTIVE_MODE == halSleepControl.wakeupStation)
    return;  // it is a too late to sleep.

  // stop application timer clock
  halStopAppClock(); // will be shutdown
  if (0ul == halTimeStartOfSleep)
  { // start of sleep procedure
    // save time of stopping of the application timer
    halTimeStartOfSleep = halGetTimeOfSleepTimer();
  }

  #ifdef _HAL_USE_AMPLIFIER_
    // set zero on pin. Disable power amplifier.
    GPIO_POW_AMPLF_SLP_clr();
  #endif

  #ifdef _HAL_RF_RX_TX_INDICATOR_   
   
  // disable front end driver if that is supported
  phyRxTxSwitcherOff();

  #endif
  
  #ifdef _HAL_ANT_DIVERSITY_
  
  // disable antenna diversity switcher
  phyAntennaSwitcherOff();
  
  #endif 
  
  if (halEnableDtrWakeUp)
  { /* enable DTR (irq 4) wake up */
    halEnableIrqInterrupt(IRQ_4);
  } /* enable DTR (irq 4) wake up */

  // wait for end of eeprom writing
  halWaitEepromReady();

  if (INTERNAL_RC == halGetClockSource())
  {
    GPIO_RF_SLP_TR_set();
    GPIO_RF_SLP_TR_make_out();

    if (HAL_SLEEP_TIMER_IS_STARTED == halSleepControl.sleepTimerState)
    { // sleep timer is started
      SMCR = (1 << SM1) | (1 << SM0) | (1 << SE); // power-save
      __SLEEP;
      SMCR = 0;
    }
    else
    {
      halStopSleepTimerClock();
      SMCR = (1 << SM1) | (1 << SE); // power-down
      __SLEEP;
      SMCR = 0;
      halStartSleepTimerClock();
      halStartingCalibrate();
    }
  }
  else
  {
    uint8_t timsk4 = TIMSK4;
    uint8_t twcr = TWCR;
    uint8_t adcsra =  ADCSRA;
    TIMSK4 = 0;
    TWCR = 0;
    ADCSRA = 0;
    GPIO_RF_SLP_TR_make_out();
    SMCR = (1 << SM1) | (1 << SM0) | (1 << SE); // power-save
    __SLEEP;
    SMCR = 0;
    TIMSK4 = timsk4;
    TWCR = twcr;
    ADCSRA = adcsra;
  }

  // wait for time about 1 TOSC1 cycle for correct re-entering from power save mode to power save mode
  // wait for time about 1 TOSC1 cycle for correct reading TCNT2 after wake up to
  OCR2B = SOME_VALUE_FOR_SYNCHRONIZATION;
  while (ASSR & HAL_ASSR_FLAGS);
}

/******************************************************************************
  Prepares system for power-save, power-down.
  Power-down the mode is possible only when internal RC is used
  Parameters:
  none.
  Returns:
  -1 there is no possibility to power-down system.
******************************************************************************/
int HAL_Sleep(void)
{
  if (INTERNAL_RC != halGetClockSource())
  {
    if (HAL_SLEEP_TIMER_IS_STOPPED == halSleepControl.sleepTimerState)
    { // sleep timer isn't started
      return -1;
    }
    GPIO_RF_SLP_TR_make_in();

    while (ASSR & HAL_ASSR_FLAGS);
    if (!(TIMSK2 & (1 << OCIE2A)))
    { // compare interrupt is disabled
      OCR2A = 0xFF;
      while (ASSR & HAL_ASSR_FLAGS);
    }

    TCCR2A |= ((1 << COM2A1) | (1 << COM2A0)); // set OC2A on compare
    while (ASSR & HAL_ASSR_FLAGS);
    TCCR2B |= (1 << FOC2A); // force output to set OC2A
    while (ASSR & HAL_ASSR_FLAGS);
    TCCR2A &= ~((1 << COM2A1) | (1 << COM2A0)); // no compare
    while (ASSR & HAL_ASSR_FLAGS);
    TCCR2A |= (1 << COM2A1); // clear OC2A on compare
    while (ASSR & HAL_ASSR_FLAGS);
  }

  halSleepControl.wakeupStation = HAL_SLEEP_MODE;  // the reset of sign of entry to the sleep mode.
  while (ASSR & HAL_ASSR_FLAGS);
  halPostTask5(HAL_SLEEP);
  return 0;
}

/******************************************************************************
 Handler for task manager. It is executed when system has waked up.
******************************************************************************/
void halWakeupHandler(void)
{
  uint32_t timeEndOfSleep;

  // save time of stopping of the application timer
  timeEndOfSleep = halGetTimeOfSleepTimer();

  timeEndOfSleep -= halTimeStartOfSleep;  // time of sleep
  halTimeStartOfSleep = 0ul;
  // adjust application timer interval
  halAdjustSleepInterval(timeEndOfSleep);
  // start application timer clock
  halStartAppClock();
  // Wait for when radio will be waked up.
  halWaitRadio();
  
  #ifdef _HAL_ANT_DIVERSITY_
  
  // enable antenna diversity switcher
  phyAntennaSwitcherOn();
  
  #endif
  
  #ifdef _HAL_RF_RX_TX_INDICATOR_
  
  // enable front end driver if that is supported
  phyRxTxSwitcherOn();
  
  #endif
  
  if (HAL_SLEEP_TIMER_IS_WAKEUP_SOURCE == halSleepControl.wakeupSource)
  {
    if (halSleepControl.callback)
      halSleepControl.callback();
  }
}

/*******************************************************************************
  Makes MCU enter Idle mode.
*******************************************************************************/
void HAL_IdleMode(void)
{
  SMCR = 0x1;
  __SLEEP;
  SMCR = 0;
}

// eof sleep.c
