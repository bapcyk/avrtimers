#ifndef _COMMON_TIMERS_H
#define _COMMON_TIMERS_H

#if defined(__AVR_ATmega8__)
#  error Does not supported yet!
//#  include "timers8.h"
#elif defined(__AVR_ATmega168P__)
#  error Does not supported yet!
//#  include "timers168p.h"
#elif defined(__AVR_ATmega128__)
#  include "timers128.h"
#else
#  error timers not supported for this chip!
#endif

#endif
