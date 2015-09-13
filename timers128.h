#ifndef _COMMON_TIMERS128_H
#define _COMMON_TIMERS128_H

#include "conf.h"
#include "utils.h"
#include "_timers.h"

#define NTIMERS 4

// Init system clock: clock for clock(3) function. Used timer
// is T/C0 (timer0) with freq. F_CPU (prescaled 1) {{{
#define init_clock() do { \
        TCNT0 = 0; /* reset counter */ \
        TCCR0 = 1<<CS00; /* freq. is F_CPU */ \
        setbit(TIMSK, TOIE0); /* enable INT from this timer on TCNT overload */ \
} while (0)
#define CLOCKS_PER_SEC F_CPU
#define clock() tclock(0)
// }}}

#endif
