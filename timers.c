#include "conf.h"

#if defined(__AVR_ATmega8__)
#  error Does not supported yet!
//#  include "timers8.c"
#elif defined(__AVR_ATmega168P__)
//#  include "timers168p.c"
#elif defined(__AVR_ATmega128__)
#  include "timers128.c"
#else
#  error timers not supported for this chip!
#endif

//#undef NDEBUG
//#define DEBUG_NUART 1
//#include "dbuart.h"
/* Calculates prescale (divisor of timer: 1,8,...) and start
 * counter value and total cycles. Returns 0 on success, -1 on faulire.
 * D is the delay in microseconds.  timer is number of timer.
 * Out values are saved in corresponding timerc_t.
 */
int
_alarm_setup(int timer,
             unsigned long D)
{
        int i;
        int max_psc; // max. value of prescaler of timer
        int *psc; // prescalers of timer
        unsigned long T; // period of tick
        unsigned long Tclk; // period of clock
        unsigned long Tall; // period of timer cycle (from 0 to bits)

        psc = _timers[timer].prescalers;
        max_psc = psc[_timers[timer].nprescalers-1];

        /* Base frequency is F_CPU. Real frequency is F_CPU/prescaler.
           One tick occupes 1/freq seconds (freq in MHz). N ticks occupes
           N/freq. N is max ticks number in counter = bits of this counter =
           _timers[timer].bits. So, total time to count all ticks of counter is
           N/freq = N/(F_CPU/prescaler) = N*prescaler/F_CPU. Freq is in MHz, to
           use microseconds instead of seconds, * 1000000.

           T is the total time of count all the counter with slowest freq (with
           last prescaler - max_psc). cycles is how many counter cycles are in
           D with slowest prescaler on this timer-counter.

           Then D set to D without cycles time.
           */

        // 1000000 * max_psc > F_CPU, so T/F_CPU != 0
        T = (1000000 * max_psc / F_CPU) * _timers[timer].bits;
        _timers[timer].cycles = D/T;

        if (_timers[timer].cycles > 0) {
                D -= (_timers[timer].cycles) * T;
                _timers[timer].cycles_prescaler = max_psc;
        }
        else {
                // If does not need cycles, cycles_prescaler will be 0
                // (may be used as flag of this)
                _timers[timer].cycles_prescaler = 0;
        }

        D *= 1000; // in nanoseconds
        Tclk = 1000000000 / F_CPU; // in nanoseconds
        for (i=0; i<_timers[timer].nprescalers; i++) {
                T = Tclk * psc[i]; // time of one tick
                Tall = T * _timers[timer].bits; // time to count all ticks of counter
                if (D > Tall) {
                        // delay D is too long for this Tall
                        continue;
                }
                else {
                        _timers[timer].prescaler = psc[i];
                        // D/T how many ticks in D, cn is ticks from 0
                        _timers[timer].cn = _timers[timer].bits - D/T;
                        return (0);
                }
        }
        return (-1);
}

inline int
alarm_action(int timer, timer_handler_t alarm_handler) {
        _timers[timer].alarm_handler = alarm_handler;
        return alarm(timer, 0); // turn-OFF prev. alarm
}

/* Setup alarm and enable alarm interrupt. Alarm will occur after usec
 * microseconds. If usec will be 0, alarm will be reset (turn off). On
 * success returns 0, -1 otherwise. Alarm occurs once only!
 */
inline int
_alarm(int timer, unsigned long usec) {
        // first, stop timer
        _alarm_regsetup(timer, 0, 0);
        if (!usec) {
                // only stop, notning else
                return (0);
        }

        // setup timerc_t fields
        if (_alarm_setup(timer, usec)) {
                return (-1);
        }

        // If need total cycles of timer-counter (delay is too long for one cycle)...
        if (_timers[timer].cycles_prescaler) {
                _alarm_regsetup(timer, 0, _timers[timer].cycles_prescaler);
        }
        else {
                _alarm_regsetup(timer, _timers[timer].cn, _timers[timer].prescaler);
        }
        return (0);
}

/* Same as _alarm() but first cli() and sei() at end! User should use it
 */
inline int
alarm(int timer, unsigned long usec) {
        int ret;
        unsigned long old_sreg = SREG;
        cli();
        ret = _alarm(timer, usec);
        SREG = old_sreg;
        return (ret);
}

/* Returns how many milliseconds passed from last call. If result is 0, then
 * passed 0 milliseconds (and several microseconds)
 */
unsigned long _prevticks = 0;
inline unsigned long
millis(void) {
        unsigned long nowticks = clock();
        unsigned long ret = (1000UL * (nowticks - _prevticks)) / CLOCKS_PER_SEC;
        _prevticks = nowticks;
        return (ret);
}

/* Returns how many microseconds passed from last call.
 */
inline unsigned long
micros(void) {
        unsigned long nowticks = clock();
        unsigned long ret = (1000000UL * (nowticks - _prevticks)) / CLOCKS_PER_SEC;
        _prevticks = nowticks;
        return (ret);
}

