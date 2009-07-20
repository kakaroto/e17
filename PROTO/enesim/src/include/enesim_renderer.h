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
#ifndef ENESIM_RENDERER_H_
#define ENESIM_RENDERER_H_

/**
 * @defgroup Enesim_Renderer_Group Renderer
 *
 * @{
 */
typedef struct _Enesim_Renderer Enesim_Renderer; /**< Renderer Handler */
EAPI void enesim_renderer_delete(Enesim_Renderer *r);
EAPI void enesim_renderer_span_fill(Enesim_Renderer *r, int x, int y,
	unsigned int len, uint32_t *dst);


EAPI Enesim_Renderer * enesim_renderer_hswitch_new(void);
EAPI void enesim_renderer_hswitch_w_set(Enesim_Renderer *r, int w);
EAPI void enesim_renderer_hswitch_h_set(Enesim_Renderer *r, int h);
EAPI void enesim_renderer_hswitch_left_set(Enesim_Renderer *r,
		Enesim_Surface *s);
EAPI void enesim_renderer_hswitch_right_set(Enesim_Renderer *r,
		Enesim_Surface *s);
EAPI void enesim_renderer_hswitch_step_set(Enesim_Renderer *r, float step);


EAPI Enesim_Renderer * enesim_renderer_surface_new(void);
EAPI void enesim_renderer_surface_x_set(Enesim_Renderer *r, int x);
EAPI void enesim_renderer_surface_y_set(Enesim_Renderer *r, int y);
EAPI void enesim_renderer_surface_w_set(Enesim_Renderer *r, int w);
EAPI void enesim_renderer_surface_h_set(Enesim_Renderer *r, int h);
EAPI void enesim_renderer_surface_src_set(Enesim_Renderer *r, Enesim_Surface *src);

/**
 * @}
 */

#endif /*ENESIM_RENDERER_H_*/
