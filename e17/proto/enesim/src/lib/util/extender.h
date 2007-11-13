#ifndef _ENESIM_EXTENDER_H
#define _ENESIM_EXTENDER_H

typedef struct _Enesim_Extender
{
	int max;
	int min;
} Enesim_Extender;

static inline enesim_extender_reset(Enesim_Extender *e)
{
	e->min = INT_MAX;
	e->max = INT_MIN;
}

static inline enesim_extender_add(Enesim_Extender *e, int min, int max)
{
	if (min < e->min)
		e->min = min;
	if (max > e->max)
		e->max = max;
}

#endif
