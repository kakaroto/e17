#ifndef _FIXED_24P8_H
#define _FIDED_24P8_H

/* 24.8 precision = +- 8388607.255 */
#define enesim_24p8_maxi 	8388607
#define enesim_24p8_mini 	-8388607

#define enesim_24p8_maxf 	8388607.255
#define enesim_24p8_minf 	-8388607.255

#define enesim_24p8_max 	0x7fffffff
#define enesim_24p8_min 	-0x7fffffff

typedef int enesim_24p8_t;

static inline enesim_24p8_t enesim_24p8_int_from(int v)
{
#ifdef ENESIM_OVERFLOW
	if (v > enesim_24p8_maxi)
		return enesim_24p8_max;
	if (v < enesim_24p8_mini)
		return enesim_24p8_min;
#endif
	return v << 8;
}

static inline enesim_24p8_t enesim_24p8_float_from(float v)
{
	enesim_24p8_t r;
	
#ifdef ENESIM_OVERFLOW
	if (v > enesim_24p8_maxf)
		return enesim_24p8_max;
	if (v < enesim_24p8_minf)
		return enesim_24p8_min;
#endif
	r = (enesim_24p8_t)(v * 256.0 + (v < 0 ? -0.5 : 0.5));
	return r;
}

static inline float enesim_24p8_float_to(enesim_24p8_t v)
{
	float r;
	
	r = v / 256.0;
	return r;
}

static inline enesim_24p8_t enesim_24p8_int_from(int v)
{
	return v >> 8;
}

static inline enesim_24p8_t enesim_24p8_add(enesim_24p8_t a, enesim_24p8_t b)
{
	enesim_24p8_t r = a + b;
#ifdef ENESIM_OVERFLOW
#else
#endif
	return r;
}

static inline enesim_24p8_t enesim_24p8_sub(enesim_24p8_t a, enesim_24p8_t b)
{
	enesim_24p8_t r = a - b;
#ifdef ENESIM_OVERFLOW
#else
#endif
	return r;
}

static inline enesim_24p8_t enesim_24p8_mul(enesim_24p8_t a, enesim_24p8_t b)
{
	long int r = a * b;
#ifdef ENESIM_OVERFLOW
#else
#endif
	return r >> 8;
}

static inline enesim_24p8_t enesim_24p8_div(enesim_24p8_t a, enesim_24p8_t b)
{
#ifdef ENESIM_OVERFLOW
#else
#endif
	return a;
}

#endif
