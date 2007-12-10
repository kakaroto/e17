#ifndef _ENESIM_EXTENDER_H
#define _ENESIM_EXTENDER_H

typedef struct _Enesim_Extender_Int
{
	int max;
	int min;
} Enesim_Extender_Int;

static inline enesim_extender_int_reset(Enesim_Extender_Int *e)
{
	e->min = INT_MAX;
	e->max = INT_MIN;
}

static inline enesim_extender_int_add(Enesim_Extender_Int *e, int start, int end)
{
	if (start < e->min)
		e->min = start;
	if (end > e->max)
		e->max = end;
}

static inline enesim_extender_int_unsorted_add(Enesim_Extender_Int *e, int start, int end)
{
	if (start <= end)
		enesim_extender_int_add(e, start, end);
	else
		enesim_extender_int_add(e, end, start);
}

#endif
