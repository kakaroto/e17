/* ETCH - Timeline Based Animation Library
 * Copyright (C) 2007-2008 Jorge Luis Zapata, Hisham Mardam-Bey
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library.
 * If not, see <http://www.gnu.org/licenses/>.
 */
#include "Etch.h"
#include "etch_private.h"

/**
 * @todo
 * - make functions to interpolate between data types,
 * possible animations:
 * sin
 * exp
 * log
 * linear
 * bezier based (1 and 2 control points)
 * - make every interpolator work for every data type, so better a function table
 * - define animatinos based on two properties: PERIODIC, UNIQUE, PERIODIC_MIRROR
 * - the integer return values of the interpolators should be rounded?
 */
extern Etch_Interpolator etch_interpolator_uint32;
extern Etch_Interpolator etch_interpolator_argb;
/*============================================================================*
 *                                  Local                                     * 
 *============================================================================*/
static void _keyframe_debug(Etch_Animation_Keyframe *k)
{
	printf("Keyframe at %g of type %d\n", k->time, k->type);
	switch (k->value.type)
	{
		case ETCH_UINT32:
		printf("value = %u\n", k->value.data.u32);
		break;

		case ETCH_ARGB:
		printf("value = 0x%8x\n", k->value.data.argb);
		break;
		
		default:
		break;
	}
}

static void _animation_debug(Etch_Animation *a)
{
	Eina_Inlist *l;

	printf("Animation that interpolates data of type %d, with the following keyframes:\n", a->dtype);
	l = (Eina_Inlist *)a->keys;
	while (l)
	{
		Etch_Animation_Keyframe *k = (Etch_Animation_Keyframe *)l;
	
		_keyframe_debug(k);
		l = l->next;
	}
}

Etch_Interpolator *_interpolators[ETCH_DATATYPES] = {
		[ETCH_UINT32] = &etch_interpolator_uint32,
		[ETCH_ARGB] = &etch_interpolator_argb,
};
/*============================================================================*
 *                                 Global                                     * 
 *============================================================================*/
/**
 * To be documented
 * FIXME: To be fixed
 */
void etch_animation_animate(Etch_Animation *a, double curr)
{
	Eina_Inlist *l;
	Etch_Animation_Keyframe *start;
	Etch_Animation_Keyframe *end;
	
	/* check that the time is between two keyframes */
	if (!a->keys)
		return;
	
	/* TODO instead of checking everytime every keyframe we can translate the
	 * keyframes based on the frame, when a keyframe has passed move it before
	 * like a circular list */
	l = (Eina_Inlist *)a->keys;
	while (l)
	{
		start = (Etch_Animation_Keyframe *)l;
		end = (Etch_Animation_Keyframe *)(l->next);
		if (!end)
			break;
		/* get the keyframe affected */
		//printf("-> [%g] %g %g\n", curr, start->time, end->time);
		if ((curr >= start->time) && (curr <= end->time))
		{
			Etch_Interpolator_Func ifnc;
			Etch_Data old;
			double m;
			
			/* get the interval between 0 and 1 based on current frame and two keyframes */
			if (curr == start->time)
				m = 0;
			else if (curr == end->time)
				m = 1;
			else
				m = (curr - start->time)/(end->time - start->time);
			/* accelerate the calculations if we get the same m as the previous call */
			if (m == a->m)
			{
				a->cb(&a->curr, &a->curr, a->data);
				return;
			}
			/* store old value */
			old = a->curr;
			/* interpolate the new value */
			ifnc = _interpolators[a->dtype]->funcs[start->type];
			if (!ifnc)
				return;
			ifnc(&(start->value), &(end->value), m, &a->curr, &start->data);
			/* once the value has been set, call the callback */
			a->cb(&a->curr, &old, a->data);
			return;
		}
		l = l->next;
	}
}

Etch_Animation * etch_animation_new(Etch_Data_Type dtype, Etch_Animation_Callback cb, void *data)
{
	Etch_Animation *a;
		
	a = calloc(1, sizeof(Etch_Animation));
	/* common values */
	a->m = -1; /* impossible, so the first keyframe will overwrite this */
	a->start = DBL_MAX;
	a->dtype = dtype;
	a->keys = NULL;
	a->cb = cb;
	a->data = data;
}

/*============================================================================*
 *                                   API                                      * 
 *============================================================================*/
/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void etch_animation_delete(Etch_Animation *a)
{
	assert(a);
	/* TODO delete the list of keyframes */
	free(a);
}
/**
 * Set the number of times the animation should repeat
 */
EAPI void etch_animation_repeat_set(Etch_Animation *a, unsigned int times)
{
	
}
/**
 * Add a new mark to the animation
 */
EAPI Etch_Animation_Keyframe * etch_animation_keyframe_add(Etch_Animation *a)
{
	Etch_Animation_Keyframe *k;
	
	assert(a);
	k = calloc(1, sizeof(Etch_Animation_Keyframe));
	k->animation = a;

	/* add the new keyframe to the list of keyframes */
	a->keys = eina_inlist_prepend(a->keys, EINA_INLIST_GET(k));
	
	return k;
}
/**
 * Delete the mark from the animation
 */
EAPI void etch_animation_keyframe_del(Etch_Animation *a, Etch_Animation_Keyframe *k)
{
	assert(a);
	assert(k);
	/* remove the keyframe from the list */
	a->keys = eina_inlist_remove(a->keys, EINA_INLIST_GET(k));
	/* TODO recalculate the start and end if necessary */
	free(k);
}
/**
 * Set the type of animation keyframe
 */
EAPI void etch_animation_keyframe_type_set(Etch_Animation_Keyframe *k, Etch_Animation_Type t)
{
	assert(k);
	k->type = t;
}
/**
 * Get the type of animation keyframe
 */
EAPI Etch_Animation_Type etch_animation_keyframe_type_get(Etch_Animation_Keyframe *k)
{
	assert(k);
	return k->type;
}
/**
 * Set the time for a mark
 */
EAPI void etch_animation_keyframe_time_set(Etch_Animation_Keyframe *k, unsigned long secs, unsigned long usecs)
{
	Etch_Animation *a;
	Etch_Time t;
	double new_time;
	Eina_Inlist *l;
	
	assert(k);
	
	t.secs = secs;
	t.usecs = usecs;
	new_time = etch_time_double_to(&t);
	/* if the time is the same, do nothing */
	if (new_time == k->time)
		return;
	a = k->animation;
	/* find the greater element with the value less than the one to set */
	l = (Eina_Inlist *)(a->keys);
	while (l)
	{
		Etch_Animation_Keyframe *k2 = (Etch_Animation_Keyframe *)l;
		
		if (k2->time >= new_time)
			break;
		l = l->next;
	}
	/* if the element to remove is the same as the element to change, do
	 * nothing */
	if ((Etch_Animation_Keyframe*)l == k)
		goto update;
	a->keys = eina_inlist_remove(a->keys, EINA_INLIST_GET(k));
	/* k is the greatest */
	if (!l)
	{
		a->keys = eina_inlist_append(a->keys, EINA_INLIST_GET(k));
	}
	/* k is between two keyframes */
	else
	{	
		a->keys = eina_inlist_prepend_relative(a->keys, EINA_INLIST_GET(k), l);	
	}
	/* update the start and end values */
update:
	k->time = new_time;
	a->start = ((Etch_Animation_Keyframe *)a->keys)->time;
	a->end = ((Etch_Animation_Keyframe *)((Eina_Inlist *)(a->keys))->last)->time;
}
/**
 * Get the value for a mark
 */ 
EAPI void etch_animation_keyframe_value_get(Etch_Animation_Keyframe *k, ...)
{
	assert(k);
	/* TODO */
}
/**
 * 
 */ 
EAPI void etch_animation_offset_add(Etch_Animation *a, unsigned long secs, unsigned long usecs)
{
	Etch_Time t;
	double new_time;
	Eina_Inlist *l;
		
	assert(a);
		
	t.secs = secs;
	t.usecs = usecs;
	new_time = etch_time_double_to(&t);
	l = (Eina_Inlist *)(a->keys);
	/* increment every keyframe by secs.usecs */
	while (l)
	{
		Etch_Animation_Keyframe *k = (Etch_Animation_Keyframe *)l;
		
		k->time += new_time;
		l = l->next;
	}
}
/**
 * Set the value for a mark
 */
EAPI void etch_animation_keyframe_value_set(Etch_Animation_Keyframe *k, ...)
{
	va_list va;

	assert(k);
	va_start(va, k);
	/* now get the type specific data, for example the bezier forms need 
	 * control points, etc */
	switch (k->type)
	{
		case ETCH_ANIMATION_DISCRETE:
		case ETCH_ANIMATION_LINEAR:
		{
			switch (k->animation->dtype)
			{	
				case ETCH_UINT32:
					k->value.data.u32 = va_arg(va, unsigned int);
					break;
				case ETCH_ARGB:
					k->value.data.argb = va_arg(va, unsigned int);
					break;
				default:
					break;
			}
			break;
		}
		case ETCH_ANIMATION_QUADRATIC:
		{
			switch (k->animation->dtype)
			{	
				case ETCH_UINT32:
					k->value.data.u32 = va_arg(va, unsigned int);
					k->data.q.cp.data.u32 = va_arg(va, unsigned int);
					break;
				default:
					break;
			}
			break;
		}
		case ETCH_ANIMATION_CUBIC:
		{
			switch (k->animation->dtype)
			{	
				case ETCH_UINT32:
					k->value.data.u32 = va_arg(va, unsigned int);
					k->data.c.cp1.data.u32 = va_arg(va, unsigned int);
					k->data.c.cp2.data.u32 = va_arg(va, unsigned int);
					break;
				default:
					break;
			}
			break;
		}
		default:
			break;
	}
	va_end(va);		
}
