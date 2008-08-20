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
#ifndef _ENESIM_EXTENDER_H
#define _ENESIM_EXTENDER_H

typedef struct _Enesim_Extender_Int
{
	int max;
	int min;
} Enesim_Extender_Int;

static inline void enesim_extender_int_reset(Enesim_Extender_Int *e)
{
	e->min = INT_MAX;
	e->max = INT_MIN;
}

static inline void enesim_extender_int_add(Enesim_Extender_Int *e, int start, int end)
{
	if (start < e->min)
		e->min = start;
	if (end > e->max)
		e->max = end;
}

static inline void enesim_extender_int_unsorted_add(Enesim_Extender_Int *e, int start, int end)
{
	if (start <= end)
		enesim_extender_int_add(e, start, end);
	else
		enesim_extender_int_add(e, end, start);
}

#endif
