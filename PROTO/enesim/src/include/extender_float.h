/* ENESIM - Direct Rendering Library
 * Copyright (C) 2007-2008 Jorge Luis Zapata
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
