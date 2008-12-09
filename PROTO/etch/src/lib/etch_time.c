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
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
const char * etch_time_string_to(Etch_Time *t)
{
	
	
}

Eina_Bool etch_time_between(Etch_Time *first, Etch_Time *last, Etch_Time *cmp)
{
	if ((cmp->secs < first->secs) || (cmp->secs > last->secs))
		return EINA_FALSE;
	if ((cmp->secs == first->secs) && (cmp->usecs < first->usecs))
		return EINA_FALSE;
	if ((cmp->secs == last->secs) && (cmp->usecs > last->usecs))
		return EINA_FALSE;
	return EINA_TRUE;
}

void etch_time_increment(Etch_Time *t, Etch_Time *a)
{
	t->usecs += a->usecs;
	t->secs += a->secs;
	if (t->usecs >= 1000000)
	{
		t->usecs -= 1000000;
		t->secs++;
	}
}

void etch_time_secs_from(Etch_Time *t, unsigned long int secs, unsigned long int usecs)
{
	t->usecs = usecs;
	t->secs = secs;
}

void etch_time_secs_to(Etch_Time *t, unsigned long int *secs, unsigned long int *usecs)
{
	if (usecs) *usecs = t->usecs;
	if (secs) *secs = t->secs;
}

double etch_time_double_to(Etch_Time *t)
{
	return (double)t->secs + (((double)t->usecs) / 1000000);
}

void etch_time_double_from(Etch_Time *t, double d)
{
	t->secs = (unsigned long int)d;
	t->usecs = (d - t->secs) * 1000000;
}
