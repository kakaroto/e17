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
#include "enesim_common.h"
#include "Enesim.h"
#include "enesim_private.h"
/*============================================================================*
 *                                  Local                                     * 
 *============================================================================*/
static inline void midpoint_get(Eina_F16p16 c1, Eina_F16p16 c2, Eina_F16p16 *r)
{
	*r = eina_f16p16_float_from(eina_f16p16_float_to(eina_f16p16_add(c1, c2)) / 2);
}

static void curve3_recursive(Eina_F16p16 x1, Eina_F16p16 y1, Eina_F16p16 x2,
		Eina_F16p16 y2, Eina_F16p16 x3, Eina_F16p16 y3, int level,
		Enesim_Curve_Vertex_Add_Callback cb, void *data)
{
	Eina_F16p16 x12, y12, x23, y23, x123, y123;
	Eina_F16p16 rx, ry;

	/* mid points */
	midpoint_get(x1, x2, &x12);
	midpoint_get(y1, y2, &y12);
	midpoint_get(x2, x3, &x23);
	midpoint_get(y2, y3, &y23);
	midpoint_get(x12, x23, &x123);
	midpoint_get(y12, y23, &y123);

	rx = abs(x1 + x3 - x2 - x2);
	ry = abs(y1 + y3 - y2 - y2);
	if ((rx + ry) <= (1 << 16))
	{
		cb(eina_f16p16_float_to(x123), eina_f16p16_float_to(y123), data);
		return;
	}
	
	curve3_recursive(x1, y1, x12, y12, x123, y123, level + 1, cb, data); 
	curve3_recursive(x123, y123, x23, y23, x3, y3, level + 1, cb, data); 
}
/*============================================================================*
 *                                 Global                                     * 
 *============================================================================*/
void enesim_curve3_decasteljau_generate(float x1, float y1, float x2, float y2,
		float x3, float y3, Enesim_Curve_Vertex_Add_Callback cb, void *data)
{
	cb(x1, y1, data);
	curve3_recursive(eina_f16p16_float_from(x1), eina_f16p16_float_from(y1),
			eina_f16p16_float_from(x2), eina_f16p16_float_from(y2), 
			eina_f16p16_float_from(x3), eina_f16p16_float_from(y3), 0, cb, data);
	cb(x3, y3, data);
}

void enesim_curve4_decasteljau_generate(float x1, float y1, float x2, float y2,
		float x3, float y3, float x4, float y4, Enesim_Curve_Vertex_Add_Callback
		cb, void *data)
{
	
}
