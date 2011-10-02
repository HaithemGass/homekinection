/**************************************************************************//**
  \file  halDiagnostic.h

  \brief Implementation of diagnostics defines.

  \author
      Atmel Corporation: http://www.atmel.com \n
      Support email: avr@atmel.com

    Copyright (c) 2008-2011, Atmel Corporation. All rights reserved.
    Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
    History:
      20/05/09 D. Kasyanov - Created
 ******************************************************************************/

#ifndef _HALDIAGNOSTIC_H
#define _HALDIAGNOSTIC_H

#include <halFCPU.h>
#include <halDbg.h>

#if defined (MEASURE)
        #define TCNT5_ACCESS_TIME 8
        #define DEFALUT_TIME_LIMIT 100
        #define TIMER3_COMPA_TIME_LIMIT 150
        #define PHYDISPATCH_RFINT_TIME_LIMIT 210

        #define BEGIN_MEASURE { \
                        uint16_t timeLimit = DEFALUT_TIME_LIMIT; \
                        uint16_t start = TCNT5; uint16_t offset;

        #define END_MEASURE(code) offset = (TCNT5 - start - TCNT5_ACCESS_TIME) / (F_CPU/1000000ul); \
                          if (HALISR_TIMER3_COMPA_TIME_LIMIT == code) timeLimit = TIMER3_COMPA_TIME_LIMIT; \
                          if (HALISR_PHYDISPATCH_RFINT_TIME_LIMIT == code) timeLimit = PHYDISPATCH_RFINT_TIME_LIMIT; \
                          if (timeLimit != 0) { \
                            if (offset > timeLimit) { \
                             TCCR5B = 0; TCNT5 = offset; assert(0,code); \
                            } \
                          } \
                       }

#else
    #define BEGIN_MEASURE
    #define END_MEASURE(code)
#endif


#endif /* _HALDIAGNOSTIC_H */

