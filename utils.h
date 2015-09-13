#ifndef _COMMON_UTILS_H
#define _COMMON_UTILS_H
#include "conf.h"
#include <stdint.h>
#include <stdbool.h>

#define LSBFIRST 0
#define MSBFIRST 1

/* set, clear bit B of port P */
#define setbit(P, B) ((P) |= (_BV(B)))
#define clibit(P, B) ((P) &= ~(_BV(B)))
#define getbit(P, B) (((P) & (_BV(B)))? 1:0)

#endif
