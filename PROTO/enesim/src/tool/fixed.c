#include "enesim_generator.h"

#if 0

/* 16.16 precision = +- 32767.65535 */
#define enesim_16p16_maxi 	32767
#define enesim_16p16_mini 	-32767

#define enesim_16p16_maxf 	32767.65535
#define enesim_16p16_minf 	-32767.65535

#define enesim_16p16_max 	0x7fffffff
#define enesim_16p16_min 	-0x7fffffff

typedef int Eina_F16p16;

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


static inline FIXED_TYPE FIXED_INT_FROM(int v)
{
	return v << FIXED_FLOAT_BITS;
}

static inline FIXED_TYPE FIXED_INT_TO(FIXED_TYPE v)
{
	return v >> FIXED_FLOAT_BITS;
}
	
static inline FIXED_TYPE FIXED_FLOAT_FROM(float v)
{
	FIXED_TYPE r;
	r = (FIXED_TYPE)(v * FIXED_DIV + (v < 0 ? -0.5 : 0.5));
	return r;
}

static inline float FIXED_FLOAT_TO(FIXED_TYPE v)
{
	float r;
	r = v / FIXED_DIV;
	return r;
}

static inline FIXED_TYPE FIXED_ADD(FIXED_TYPE a, FIXED_TYPE b)
{
	return a + b;
}

static inline FIXED_TYPE FIXED_SUB(a, b)
{
	return a - b;
}

static inline FIXED_TYPE FIXED_MUL(a, b)
{
	return (a * b) >> FIXED_FLOAT_BITS;
}

static inline FIXED_TYPE FIXED_SQRT(FIXED_TYPE a)
{
	unsigned int root, remHi, remLo, testDiv, count;
	root = 0; /* Clear root */
	remHi = 0; /* Clear high part of partial remainder */
	remLo = a; /* Get argument into low part of partial remainder */
	count = FIXED_ITERS; /* Load loop counter */
	do
	{
		remHi = (remHi<<2) | (remLo>>30);
		remLo <<= 2; /* get 2 bits of arg */
		root <<= 1; /* Get ready for the next bit in the root */
		testDiv = (root << 1) + 1; /* Test radical */
		if (remHi >= testDiv)
		{
			remHi -= testDiv;
			root++;
		}
	} while (count-- != 0);
	return (root);	
}

static inline unsigned int FIXED_FRACC_GET(FIXED_TYPE v)
{
	return (v & 0xffff);
}
#endif

/*
 * File that generates fixed point functions for different
 * formats, based on the Fixed struct description. 
 */

typedef struct _Fixed
{
	unsigned int ibits;
	unsigned int dbits;
} Fixed;


Fixed fixed_16p16 = {
	.ibits = 16,
	.dbits = 16,
};

Fixed fixed_24p8 = {
	.ibits = 24,
	.dbits = 8,
};

Fixed *_fixed[] = {
	&fixed_24p8,
	&fixed_16p16,
	NULL,
};

static void fixed_core(Fixed *f)
{
	
}

static void fixed_int_to(Fixed *f)
{
	
}

static void fixed_int_from(Fixed *f)
{
	
}

static void fixed_float_to(Fixed *f)
{
	
}

static void fixed_float_from(Fixed *f)
{
	
}

static void fixed_add(Fixed *f)
{
	
}

static void fixed_sub(Fixed *f)
{
	
}

static void fixed_mul(Fixed *f)
{
	
}

static void fixed_sqrt(Fixed *f)
{
	
}

static void fixed_fracc_get(Fixed *f)
{
	
}


void fixed_generate(void)
{
	int i = 0;
	Fixed *f;
	
	f = _fixed[0];
	while (f)
	{
		fixed_core(f);
		fixed_int_to(f);
		fixed_int_from(f);
		fixed_float_to(f);
		fixed_float_from(f);
		fixed_add(f);
		fixed_sub(f);
		fixed_mul(f);
		f = _fixed[++i];
	}
}

