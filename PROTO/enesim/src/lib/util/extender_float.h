#ifndef _EXTENDER_FLOAT_H_
#define _EXTENDER_FLOAT_H_

typedef struct _Enesim_Extender_Float
{
	float max;
	float min;
} Enesim_Extender_Float;

static inline enesim_extender_float_reset(Enesim_Extender_Float *e)
{
	e->min = FLOAT_MAX;
	e->max = FLOAT_MIN;
}

static inline enesim_extender_float_add(Enesim_Extender_Float *e, float start, float end)
{
	if (start < e->min)
		e->min = start;
	if (end > e->max)
		e->max = end;
}

static inline enesim_extender_float_unsorted_add(Enesim_Extender_Float *e, float start, float end)
{
	if (start <= end)
		enesim_extender_float_add(e, start, end);
	else
		enesim_extender_float_add(e, end, start);
}

#endif /*_EXTENDER_FLOAT_H_*/
