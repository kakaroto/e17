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
static void _discrete(Etch_Data *da, Etch_Data *db, double m, Etch_Data *res, void *data)
{
	res->data.argb = da->data.argb;
}
static void _linear(Etch_Data *da, Etch_Data *db, double m, Etch_Data *res, void *data)
{
	unsigned int range;
	unsigned int a, b, ag, rb;
	
	a = da->data.argb;
	b = db->data.argb;
	
	/* handle specific case where a and b are equal (constant) */
	if (a == b)
	{
		res->data.u32 = a;
		return;
	}
	/* b - a*m + a */
	range = rint(256 * m);
	ag = ((((((b >> 8) & 0xff00ff) - ((a >> 8) & 0xff00ff)) * range) + (a & 0xff00ff00)) & 0xff00ff00);  
	rb = ((((((b & 0xff00ff) - (a & 0xff00ff)) * (range)) >> 8) + (a & 0xff00ff)) & 0xff00ff);
	
	res->data.u32 = ag + rb;
}
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
Etch_Interpolator etch_interpolator_argb = {
	.funcs[ETCH_ANIMATION_DISCRETE] = _discrete,
	.funcs[ETCH_ANIMATION_LINEAR] = _linear,
};
