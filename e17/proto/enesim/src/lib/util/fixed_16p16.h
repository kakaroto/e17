#ifndef _FIXED_16P16_H
#define _FIDED_16P16_H

/* 16.16 precision = +- 32767.65535 */
#define enesim_16p16_maxi 	32767
#define enesim_16p16_mini 	-32767

#define enesim_16p16_maxf 	32767.65535
#define enesim_16p16_minf 	-32767.65535

#define enesim_16p16_max 	0x7fffffff
#define enesim_16p16_min 	-0x7fffffff

typedef int enesim_16p16_t;

static inline enesim_16p16_t enesim_16p16_int_from(int v)
{
#ifdef ENESIM_OVERFLOW
	if (v > enesim_16p16_maxi)
		return enesim_16p16_max;
	if (v < enesim_16p16_mini)
		return enesim_16p16_min;
#endif
	return v << 16;
}

static inline enesim_16p16_t enesim_16p16_int_to(int v)
{
#ifdef ENESIM_OVERFLOW
#endif
	return v >> 16;
}

static inline enesim_16p16_t enesim_16p16_float_from(float v)
{
	enesim_16p16_t r;
	
#ifdef ENESIM_OVERFLOW
	if (v > enesim_16p16_maxf)
		return enesim_16p16_max;
	if (v < enesim_16p16_minf)
		return enesim_16p16_min;
#endif
	r = (enesim_16p16_t)(v * 65536.0 + (v < 0 ? -0.5 : 0.5));
	return r;
}

static inline float enesim_16p16_float_to(enesim_16p16_t v)
{
	float r;
	
	r = v / 65536.0;
	return r;
}

static inline enesim_16p16_t enesim_16p16_add(enesim_16p16_t a, enesim_16p16_t b)
{
	enesim_16p16_t r = a + b;
#ifdef ENESIM_OVERFLOW
#else
#endif
	return r;
}

static inline enesim_16p16_t enesim_16p16_sub(enesim_16p16_t a, enesim_16p16_t b)
{
	enesim_16p16_t r = a - b;
#ifdef ENESIM_OVERFLOW
#else
#endif
	return r;
}

static inline enesim_16p16_t enesim_16p16_mul(enesim_16p16_t a, enesim_16p16_t b)
{
	long int r = a * b;
#ifdef ENESIM_OVERFLOW
#else
#endif
	return r >> 16;
}

static inline enesim_16p16_t enesim_16p16_div(enesim_16p16_t a, enesim_16p16_t b)
{
#ifdef ENESIM_OVERFLOW
#else
#endif
	return a;
}

#endif
