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
/* TODO this needs to change:
 * pass x and len as pointers as maybe the renderer just drew on a subset of it
 * so the real x and len can be retrieved
 */
typedef Eina_Bool (*Enesim_Renderer_Span)(Enesim_Renderer *r, int x, int y, int len, Enesim_Surface_Data *dst);

EAPI void enesim_renderer_delete(Enesim_Renderer *r);
EAPI Enesim_Renderer_Span enesim_renderer_func_get(Enesim_Renderer *r, Enesim_Format *f);
/**
 * @defgroup Enesim_Renderer_Fill_Color_Group Fill Color
 * @{
 */
EAPI Enesim_Renderer * enesim_renderer_color_new(void);
EAPI void enesim_renderer_color_color_set(Enesim_Renderer *r, uint32_t color);
/**
 * @}
 */

EAPI Enesim_Renderer * enesim_renderer_repeater_new(void);
EAPI void enesim_renderer_repeater_mode_set(Enesim_Renderer *r, int mode);
EAPI void enesim_renderer_repeater_dst_area_set(Enesim_Renderer *r, int x, int y, int w, int h);

EAPI void enesim_renderer_repeater_src_area_set(Enesim_Renderer *r, int x, int y, int w, int h);
EAPI void enesim_renderer_repeater_src_set(Enesim_Renderer *r, Enesim_Surface_Data *sdata);
EAPI void enesim_renderer_repeater_src_unset(Enesim_Renderer *r);
EAPI Eina_Bool enesim_renderer_repeater_src_y(Enesim_Renderer *r, int ydst, int *ysrc);

EAPI void enesim_renderer_repeater_mask_area_set(Enesim_Renderer *r, int x, int y, int w, int h);
EAPI void enesim_renderer_repeater_mask_set(Enesim_Renderer *r, Enesim_Surface_Data *mdata);
EAPI void enesim_renderer_repeater_mask_unset(Enesim_Renderer *r);
EAPI Eina_Bool enesim_renderer_repeater_mask_y(Enesim_Renderer *r, int ydst, int *ysrc);


EAPI Enesim_Renderer * enesim_renderer_transformer_new(void);
EAPI void enesim_renderer_transformer_matrix_set(Enesim_Renderer *r, Enesim_Matrix *m);
EAPI void enesim_renderer_transformer_src_set(Enesim_Renderer *r, Enesim_Surface *src);
EAPI void enesim_renderer_transformer_quality_set(Enesim_Renderer *r, Enesim_Quality q);
EAPI Enesim_Quality enesim_renderer_transformer_quality_get(Enesim_Renderer *r);

/**
 * @}
 * @}
 */

#endif /*ENESIM_RENDERER_H_*/
