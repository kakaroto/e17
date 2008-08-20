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
 * TODO
 * remove the fill prefix, all of the renderers fill! :)
 * @{
 */
typedef struct _Enesim_Renderer Enesim_Renderer; /**< Renderer Handler */

EAPI void enesim_renderer_rop_set(Enesim_Renderer *r, int rop);
EAPI void enesim_renderer_delete(Enesim_Renderer *r);
EAPI Eina_Bool enesim_renderer_draw(Enesim_Renderer *r, int type, void *sl, Enesim_Surface *dst);
/**
 * @defgroup Enesim_Renderer_Fill_Color_Group Fill Color
 * @{
 */
EAPI Enesim_Renderer * enesim_renderer_color_new(void);
EAPI void enesim_renderer_color_color_set(Enesim_Renderer *r, uint32_t color);
/** 
 * @} 
 * @defgroup Enesim_Renderer_Fill_Surface_Group Fill Surface
 * @{
 */
enum
{
	ENESIM_SURFACE_REPEAT_NONE  = 0, /**< Don't repeat on any axis */
	ENESIM_SURFACE_REPEAT_X     = (1 << 0), /**< Repeat on X axis */
	ENESIM_SURFACE_REPEAT_Y     = (1 << 1), /**< Repeat on Y axis */
};
EAPI Enesim_Renderer * enesim_renderer_surface_new(void);
EAPI void enesim_renderer_surface_surface_set(Enesim_Renderer *r, Enesim_Surface *s);
EAPI void enesim_renderer_surface_mode_set(Enesim_Renderer *r, int mode);
EAPI void enesim_renderer_surface_dst_area_set(Enesim_Renderer *r, int x, int y, int w, int h);
EAPI void enesim_renderer_surface_src_area_set(Enesim_Renderer *r, int x, int y, int w, int h);
/** 
 * @}
 * @}
 */

#endif /*ENESIM_RENDERER_H_*/
