/**************************************************************************//**
  \file  halAppClock.c

  \brief Implementation of appTimer hardware-dependent module.

  \author
      Atmel Corporation: http://www.atmel.com \n
      Support email: avr@atmel.com

    Copyright (c) 2008-2011, Atmel Corporation. All rights reserved.
    Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
    History:
      5/12/07 A. Khromykh - Created
 ******************************************************************************/
/******************************************************************************
 *   WARNING: CHANGING THIS FILE MAY AFFECT CORE FUNCTIONALITY OF THE STACK.  *
 *   EXPERT USERS SHOULD PROCEED WITH CAUTION.                                *
 ******************************************************************************/
/******************************************************************************
                   Includes section
******************************************************************************/
#include <halAppClock.h>
#include <halDbg.h>
#include <halDiagnostic.h>

/******************************************************************************
                   Global variables section
******************************************************************************/
static uint32_t halAppTime = 0ul;     // time of application timer
uint8_t halAppTimeOvfw = 0;
static volatile uint8_t halAppIrqCount = 0;

/******************************************************************************
                   Implementations section
******************************************************************************/
/******************************************************************************
Initialization appTimer clock.
******************************************************************************/
void halInitAppClock(void)
{
  OCR4A = TOP_TIMER_COUNTER_VALUE; // 1 millisecond timer interrupt interval.
  TCCR4B = (1 << WGM12);           // CTC mode
  halStartAppClock();              // clock source is cpu divided by 8
  TIMSK4 |= (1 << OCIE4A);         // Enable TC4 interrupt
}

/******************************************************************************
Return time of sleep timer.

Returns:
  time in ms.
******************************************************************************/
uint32_t halGetTimeOfAppTimer(void)
{
  halAppSystemTimeSynchronize();
  return halAppTime;
}

/******************************************************************************
Return system time in us

Parameters:
  mem - memory for system time
Returns:
  none.
******************************************************************************/
void halGetSystemTimeUs(uint64_t *mem)
{
#if (F_CPU == 4000000ul)
  *mem = 1000ul * halAppTime + (TCNT4 << 1);
#endif
#if (F_CPU == 8000000ul)
  *mem = 1000ul * halAppTime + TCNT4;
#endif
}

/**************************************************************************//**
\brief Takes account of the sleep interval.

\param[in]
  interval - time of sleep
******************************************************************************/
void halAdjustSleepInterval(uint32_t interval)
{
  halAppTime += interval;
  halPostTask4(HAL_TIMER4_COMPA);
}

/**************************************************************************//**
Synchronization system time which based on application timer.
******************************************************************************/
void halAppSystemTimeSynchronize(void)
{
  uint8_t tmpCounter;
  uint32_t tmpValue;

  ATOMIC_SECTION_ENTER
  BEGIN_MEASURE
    tmpCounter = halAppIrqCount;
    halAppIrqCount = 0;
  END_MEASURE(HAL_APP_TIMER_SYNCHRONIZE_LIMIT)
  ATOMIC_SECTION_LEAVE

  tmpValue = tmpCounter * HAL_APPTIMERINTERVAL;
  halAppTime += tmpValue;
  if (halAppTime < tmpValue)
    halAppTimeOvfw++;
}

/******************************************************************************
Interrupt handler
******************************************************************************/
ISR(TIMER4_COMPA_vect)
{
  BEGIN_MEASURE
  halAppIrqCount++;
  halPostTask4(HAL_TIMER4_COMPA);
  // infinity loop spy
  SYS_InfinityLoopMonitoring();
  END_MEASURE(HALISR_TIMER4_COMPA_TIME_LIMIT)
}
// eof halAppClock.c
