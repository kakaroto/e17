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
EAPI void enesim_renderer_transform_set(Enesim_Renderer *r, Enesim_Matrix *m);
EAPI void enesim_renderer_delete(Enesim_Renderer *r);
EAPI Eina_Bool enesim_renderer_state_setup(Enesim_Renderer *r);
EAPI void enesim_renderer_state_cleanup(Enesim_Renderer *r);
EAPI void enesim_renderer_span_fill(Enesim_Renderer *r, int x, int y,
	unsigned int len, uint32_t *dst);

typedef enum _Enesim_Figure_Draw_Mode
{
	ENESIM_FIGURE_DRAW_MODE_FILL = 0,
	ENESIM_FIGURE_DRAW_MODE_STROKE = 1,
	ENESIM_FIGURE_DRAW_MODE_STROKE_FILL = 2,
} Enesim_Figure_Draw_Mode;

EAPI Enesim_Renderer * enesim_renderer_hswitch_new(void);
EAPI void enesim_renderer_hswitch_w_set(Enesim_Renderer *r, int w);
EAPI void enesim_renderer_hswitch_h_set(Enesim_Renderer *r, int h);
EAPI void enesim_renderer_hswitch_left_set(Enesim_Renderer *r,
		Enesim_Renderer *left);
EAPI void enesim_renderer_hswitch_right_set(Enesim_Renderer *r,
		Enesim_Renderer *right);
EAPI void enesim_renderer_hswitch_step_set(Enesim_Renderer *r, float step);

EAPI Enesim_Renderer * enesim_renderer_surface_new(void);
EAPI void enesim_renderer_surface_x_set(Enesim_Renderer *r, int x);
EAPI void enesim_renderer_surface_y_set(Enesim_Renderer *r, int y);
EAPI void enesim_renderer_surface_w_set(Enesim_Renderer *r, int w);
EAPI void enesim_renderer_surface_h_set(Enesim_Renderer *r, int h);
EAPI void enesim_renderer_surface_src_set(Enesim_Renderer *r, Enesim_Surface *src);


EAPI Enesim_Renderer * enesim_renderer_circle_new(void);
EAPI void enesim_renderer_circle_center_set(Enesim_Renderer *r, float x, float y);
EAPI void enesim_renderer_circle_radius_set(Enesim_Renderer *r, float radius);
EAPI void enesim_renderer_circle_outline_weight_set(Enesim_Renderer *r, float weight);
EAPI void enesim_renderer_circle_outline_color_set(Enesim_Renderer *r, unsigned int stroke_color);
EAPI void enesim_renderer_circle_outline_renderer_set(Enesim_Renderer *r, Enesim_Renderer *o);
EAPI void enesim_renderer_circle_fill_color_set(Enesim_Renderer *r, unsigned int fill_color);
EAPI void enesim_renderer_circle_fill_renderer_set(Enesim_Renderer *r, Enesim_Renderer *f);
EAPI void enesim_renderer_circle_draw_mode_set(Enesim_Renderer *r, Enesim_Figure_Draw_Mode draw_mode);

EAPI Enesim_Renderer * enesim_renderer_checker_new(void);
EAPI void enesim_renderer_checker_color1_set(Enesim_Renderer *r, uint32_t color);
EAPI void enesim_renderer_checker_color2_set(Enesim_Renderer *r, uint32_t color);
EAPI void enesim_renderer_checker_size_set(Enesim_Renderer *r, int w, int h);

EAPI void enesim_renderer_transition_value_set(Enesim_Renderer *r, float value);
/**
 * @}
 */

#endif /*ENESIM_RENDERER_H_*/
