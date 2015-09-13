/* Timers (alarms) for ATMEGA128
 */

#include "conf.h"
#include <avr/interrupt.h>
#include <stdio.h>
#include "timers128.h"

static int prescalers0[] = {1, 8, 32, 64, 128, 256, 1024}; // for timer0
static int prescalers[] = {1, 8, 64, 256, 1024}; // for other timers

timerc_t _timers[] = {
        __TIMERC_INITIALIZER(TBITS8, prescalers0, sizeof prescalers0/sizeof prescalers0[0]),
        __TIMERC_INITIALIZER(TBITS16, prescalers, sizeof prescalers/sizeof prescalers[0]),
        __TIMERC_INITIALIZER(TBITS8, prescalers, sizeof prescalers/sizeof prescalers[0]),
        __TIMERC_INITIALIZER(TBITS16, prescalers, sizeof prescalers/sizeof prescalers[0]),
};

// calculate prescale bits representation by prescale value (1, 8, ...)
// ... for timer0
#define calc_pscbits0(PSC) ( \
        (PSC)==1?    ((0 << CS02)|(0 << CS01)|(1 << CS00)) : \
        (PSC)==8?    ((0 << CS02)|(1 << CS01)|(0 << CS00)) : \
        (PSC)==32?   ((0 << CS02)|(1 << CS01)|(1 << CS00)) : \
        (PSC)==64?   ((1 << CS02)|(0 << CS01)|(0 << CS00)) : \
        (PSC)==128?  ((1 << CS02)|(0 << CS01)|(1 << CS00)) : \
        (PSC)==256?  ((1 << CS02)|(1 << CS01)|(0 << CS00)) : \
        (PSC)==1024? ((1 << CS02)|(1 << CS01)|(1 << CS00)) : 0)
// ... for other timers
#define calc_pscbits(T, PSC) ( \
        (PSC)==1?    ((0 << CS##T##2)|(0 << CS##T##1)|(1 << CS##T##0)) : \
        (PSC)==8?    ((0 << CS##T##2)|(1 << CS##T##1)|(0 << CS##T##0)) : \
        (PSC)==64?   ((0 << CS##T##2)|(1 << CS##T##1)|(1 << CS##T##0)) : \
        (PSC)==256?  ((1 << CS##T##2)|(0 << CS##T##1)|(0 << CS##T##0)) : \
        (PSC)==1024? ((1 << CS##T##2)|(0 << CS##T##1)|(1 << CS##T##0)) : 0)

/* setup registers of timer for prescaler and initial counter value cn
 */
inline void
_alarm_regsetup(int timer, unsigned long cn, int prescaler) {
        switch (timer) {
                case 0:
                        // stop timer
                        clibit(TIMSK, TOIE0); // disable INT from timer0
                        TCCR0 = calc_pscbits0(0);
                        TCNT0 = 0;
                        // if there is prescaler, setup registers
                        if (prescaler) {
                                TCNT0 = cn;
                                TCCR0 = calc_pscbits0(prescaler);
                                setbit(TIMSK, TOIE0); // enable INT from timer0
                        }
                        break;
                case 1:
                        clibit(TIMSK, TOIE1);
                        TCCR1B = calc_pscbits(1, 0);
                        TCNT1 = 0;
                        if (prescaler) {
                                TCNT1 = cn;
                                TCCR1B = calc_pscbits(1, prescaler);
                                setbit(TIMSK, TOIE1);
                        }
                        break;
                case 2:
                        clibit(TIMSK, TOIE2);
                        TCCR2 = calc_pscbits(2, 0);
                        TCNT2 = 0;
                        if (prescaler) {
                                TCNT2 = cn;
                                TCCR2 = calc_pscbits(2, prescaler);
                                setbit(TIMSK, TOIE2);
                        }
                        break;
                case 3:
                        clibit(TIMSK, TOIE3);
                        TCCR3B = calc_pscbits(3, 0);
                        TCNT3 = 0;
                        if (prescaler) {
                                TCNT3 = cn;
                                TCCR3B = calc_pscbits(3, prescaler);
                                setbit(TIMSK, TOIE3);
                        }
                        break;
        }
}

/* like clock(3) returns specified timer's ticks
 */
inline unsigned long
tclock(int timer) {
        unsigned long cnt;
        unsigned long old_sreg;
        old_sreg = SREG;
        if (timer==0) {
                cli(); cnt = TCNT0;
        }
        else if (timer==1) {
                cli(); cnt = TCNT1;
        }
        else if (timer==2) {
                cli(); cnt = TCNT2;
        }
        else if (timer==3) {
                cli(); cnt = TCNT3;
        }
        else {
                return 0;
        }

        SREG = old_sreg;
        return (unsigned long)(cnt + _timers[timer].nover * _timers[timer].bits);
}

// ISRs {{{
// when alarm handler is used, reset alarm after raising, so
// NEVER use clock and alarm on the same timer
ISR(TIMER0_OVF_vect) {
        __TIMER_IRQ(0);
}

ISR(TIMER1_OVF_vect) {
        __TIMER_IRQ(1);
}

ISR(TIMER2_OVF_vect) {
        __TIMER_IRQ(2);
}

ISR(TIMER3_OVF_vect) {
        __TIMER_IRQ(3);
}
// }}}

#undef calc_pscbits0
#undef calc_pscbits
