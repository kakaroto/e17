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
#include "Enesim.h"
#include "enesim_private.h"
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
typedef struct _Enesim_Renderer_Shape {
	Enesim_Renderer base;
	struct {
		unsigned int color;
		Enesim_Renderer *paint;
		float weight;
	} stroke;

	struct {
		unsigned int color;
		Enesim_Renderer *paint;
	} fill;

	int draw_mode;
} Enesim_Renderer_Shape;


EAPI void enesim_renderer_shape_outline_weight_set(Enesim_Renderer *p, float weight)
{

}

EAPI void enesim_renderer_shape_outline_color_set(Enesim_Renderer *p, unsigned int stroke_color)
{

}

EAPI void enesim_renderer_shape_outline_paint_set(Enesim_Renderer *p, Enesim_Renderer *paint)
{

}

EAPI void enesim_renderer_shape_fill_color_set(Enesim_Renderer *p, unsigned int fill_color)
{

}

EAPI void enesim_renderer_shape_fill_paint_set(Enesim_Renderer *p, Enesim_Renderer *paint)
{

}

EAPI void enesim_renderer_shape_draw_mode_set(Enesim_Renderer *p, Enesim_Shape_Draw_Mode draw_mode)
{

}
