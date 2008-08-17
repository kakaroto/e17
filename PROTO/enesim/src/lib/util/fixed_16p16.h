#ifndef _FIXED_16P16_H
#define _FIXED_16P16_H

/* 16.16 precision = +- 32767.65535 */
#define enesim_16p16_maxi 	32767
#define enesim_16p16_mini 	-32767

#define enesim_16p16_maxf 	32767.65535
#define enesim_16p16_minf 	-32767.65535

#define enesim_16p16_max 	0x7fffffff
#define enesim_16p16_min 	-0x7fffffff

typedef int enesim_16p16_t;

#undef FIXED_INT_MAX
#undef FIXED_INT_MIN
#undef FIXED_FLOAT_MAX
#undef FIXED_FLOAT_MIN
#undef FIXED_FLOAT_BITS
#undef FIXED_TYPE
#undef FIXED_DIV
#undef FIXED_ITERS

#define FIXED_INT_MAX enesim_16p16_maxi
#define FIXED_INT_MIN enesim_16p16_mini
#define FIXED_FLOAT_MAX enesim_16p16_maxf
#define FIXED_FLOAT_MIN enesim_16p16_minf
#define FIXED_FLOAT_BITS 16
#define FIXED_TYPE enesim_16p16_t
#define FIXED_DIV 65536.0
#define FIXED_ITERS (15 + (FIXED_FLOAT_BITS >> 1))

#define FIXED_INT_FROM enesim_16p16_int_from
#define FIXED_INT_TO enesim_16p16_int_to
#define FIXED_FLOAT_FROM enesim_16p16_float_from
#define FIXED_FLOAT_TO enesim_16p16_float_to
#define FIXED_ADD enesim_16p16_add
#define FIXED_SUB enesim_16p16_sub
#define FIXED_MUL enesim_16p16_mul
#define FIXED_SQRT enesim_16p16_sqrt
#define FIXED_FRACC_GET enesim_16p16_fracc_get

#include "fixed_math.h"

#endif
