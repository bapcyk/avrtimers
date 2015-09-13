#ifndef _COMMON__TIMERS_H
#define _COMMON__TIMERS_H

#include "utils.h"

/* Bits of timers: value is power of 2 
 */
typedef enum timer_bits_t {
        TBITS8 = 256,
        TBITS16 = 65536,
} timer_bits_t;

struct timerc_t;
typedef void (*timer_handler_t)(struct timerc_t *tc);

typedef struct timerc_t {
        volatile timer_handler_t alarm_handler;
        volatile unsigned long nover; // number of overflow
        unsigned long bits; // number of bits
        volatile unsigned long cn; // start from this count value
        volatile unsigned long cycles; // cycles number
        volatile int prescaler; // prescaler (1, 8, 32... - depends on arch. and timer)
        volatile int cycles_prescaler; // prescaler (usually 1024) for total counter cycles
        int *prescalers; // array of timer-counter prescalers
        int nprescalers; // length of prescalers' array
} timerc_t;

#define __TIMERC_INITIALIZER(BITS, PRESCALERS, NPRESCALERS) \
        {NULL, 0, BITS, 0, 0, 0, 0, PRESCALERS, NPRESCALERS}

#define __TIMER_IRQ(T) do { \
        if (_timers[T].cycles_prescaler) { \
                /* needs total cycles */ \
                if (_timers[T].cycles) { \
                        _timers[T].cycles--; \
                } \
                else { \
                        _timers[T].cycles_prescaler = 0; \
                        /* switch to new prescaler */ \
                        _alarm_regsetup(T, _timers[T].cn, _timers[T].prescaler); \
                } \
                goto __anyway; \
        } \
        if (_timers[T].alarm_handler) { \
                alarm(T, 0); \
                _timers[T].alarm_handler(_timers); \
        } \
__anyway: \
        _timers[T].nover++; } while (0)


#ifdef CLOCKS_PER_SEC
#  error Another clock is used
#endif

#define MSECS2CLOCKS(ms) (((CLOCKS_PER_SEC)*(ms))/1000)
#define USECS2CLOCKS(ms) (((CLOCKS_PER_SEC)*(ms))/1000000)

int alarm_action(int timer, timer_handler_t alarm_handler);
int _alarm(int timer, unsigned long usec);
int alarm(int timer, unsigned long usec); // the same but with cli()/sei() wrap
unsigned long tclock(int timer);
unsigned long millis(void); // msec from last call
unsigned long micros(void); // usec from last call

#endif
