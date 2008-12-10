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
#define USECS_MAX 1000000
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
const char * etch_time_string_to(Etch_Time *t)
{

}

void etch_time_init_max(Etch_Time *t)
{
	//a->start.secs = DBL_MAX;
	t->secs = ULONG_MAX;
	t->usecs = USECS_MAX;
}

double etch_time_interpolate(Etch_Time *first, Etch_Time *last, Etch_Time *curr)
{
	Etch_Time tmp1, tmp2;
	double d1, d2;
	
	//m = (curr - start->time)/(end->time - start->time);
	etch_time_sub(last, first, &tmp1);
	etch_time_sub(curr, first, &tmp2);
	/* TODO fix the division */
	d1 = etch_time_double_to(&tmp1);
	d2 = etch_time_double_to(&tmp2);

	return d2/d1;
}

Eina_Bool etch_time_equal(Etch_Time *t, Etch_Time *cmp)
{
	if ((t->secs == cmp->secs) && (t->usecs == cmp->usecs))
		return EINA_TRUE;
	else
		return EINA_FALSE;
}

Eina_Bool etch_time_ge(Etch_Time *l, Etch_Time *r)
{
	if (l->secs > r->secs)
		return EINA_TRUE;
	else if (l->secs == r->secs && l->usecs >= r->usecs)
		return EINA_TRUE;
	else
		return EINA_FALSE;
}

Eina_Bool etch_time_le(Etch_Time *l, Etch_Time *r)
{
	if (l->secs < r->secs)
		return EINA_TRUE;
	else if (l->secs == r->secs && l->usecs <= r->usecs)
		return EINA_TRUE;
	else
		return EINA_FALSE;
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
	if (t->usecs >= USECS_MAX)
	{
		t->usecs -= USECS_MAX;
		t->secs++;
	}
}

/* this suppose that a is > b */
void etch_time_sub(Etch_Time *a, Etch_Time *b, Etch_Time *res)
{
	long int r;

	res->secs = a->secs - b->secs;
	r = a->usecs - b->usecs;
	if (r > 0)
		res->usecs = r;
	else
	{
		res->secs--;
		res->usecs =  USECS_MAX + r;
	}
}

void etch_time_multiply(Etch_Time *t, unsigned int scalar)
{
	t->usecs *= scalar;
	t->secs *= scalar;
	if (t->usecs >= USECS_MAX)
	{
		t->secs += t->usecs / USECS_MAX;
		t->usecs = t->usecs % USECS_MAX;
	}
}

void etch_time_mod(Etch_Time *t, Etch_Time *m, Etch_Time *res)
{
	double a, b;
	/* TODO we should use the integer version */
	a = etch_time_double_to(t);
	b = etch_time_double_to(m);
	a = fmod(a, b);
	etch_time_double_from(res, a);
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
	return (double)t->secs + (((double)t->usecs) / USECS_MAX);
}

void etch_time_double_from(Etch_Time *t, double d)
{
	t->secs = (unsigned long int)d;
	t->usecs = (d - t->secs) * USECS_MAX;
}
