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
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
void enesim_renderer_shape_init(Enesim_Renderer *r)
{
	Enesim_Renderer_Shape *s = (Enesim_Renderer_Shape *)r;

	s->fill.color = 0xffffffff;
	s->stroke.color = 0xffffffff;
	enesim_renderer_init(r);
}
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void enesim_renderer_shape_outline_weight_set(Enesim_Renderer *r, float weight)
{
	Enesim_Renderer_Shape *s;

	if (weight < 1)
		weight = 1;
	s = (Enesim_Renderer_Shape *)r;
	s->stroke.weight = weight;
}
/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void enesim_renderer_shape_outline_color_set(Enesim_Renderer *r, Enesim_Color color)
{
	Enesim_Renderer_Shape *s;

	s = (Enesim_Renderer_Shape *)r;
	s->stroke.color = color;
}
/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void enesim_renderer_shape_outline_renderer_set(Enesim_Renderer *r, Enesim_Renderer *outline)
{
	Enesim_Renderer_Shape *s;

	s = (Enesim_Renderer_Shape *)r;
	s->stroke.rend = outline;
}
/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void enesim_renderer_shape_fill_color_set(Enesim_Renderer *r, Enesim_Color color)
{
	Enesim_Renderer_Shape *s;

	s = (Enesim_Renderer_Shape *)r;
	s->fill.color = color;
}
/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void enesim_renderer_shape_fill_renderer_set(Enesim_Renderer *r, Enesim_Renderer *fill)
{
	Enesim_Renderer_Shape *s;

	s = (Enesim_Renderer_Shape *)r;
	s->fill.rend = fill;
}
/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void enesim_renderer_shape_draw_mode_set(Enesim_Renderer *r, Enesim_Shape_Draw_Mode draw_mode)
{
	Enesim_Renderer_Shape *s;

	s = (Enesim_Renderer_Shape *)r;
	s->draw_mode = draw_mode;
}
