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
#ifndef ENESIM_COMPOSITOR_H_
#define ENESIM_COMPOSITOR_H_

/** @defgroup Enesim_Drawer_Group Enesim_Drawer */

/**
 * Function to draw a point
 * d = Destination surface data
 * s = In case of using the surface as pixel source
 * color = To draw with a color or as a multiplier color in case of using s
 * m = in case of using a mask
 */
typedef void (*Enesim_Compositor_Point)(uint32_t *d, uint32_t s,
		Enesim_Color color, uint32_t m);
#define ENESIM_DRAWER_POINT(f) ((Enesim_Drawer_Point)(f))
/**
 *
 */
EAPI Enesim_Compositor_Point enesim_compositor_point_color_get(Enesim_Rop rop,
		Enesim_Format dfmt, uint32_t color);
EAPI Enesim_Compositor_Point enesim_compositor_point_pixel_get(Enesim_Rop rop,
		Enesim_Format dfmt,	Enesim_Format sfmt);
EAPI Enesim_Compositor_Point enesim_compositor_point_mask_color_get(Enesim_Rop rop,
		Enesim_Format dfmt, uint32_t color, Enesim_Format mfmt);
EAPI Enesim_Compositor_Point enesim_compositor_point_pixel_mask_get(Enesim_Rop rop,
		Enesim_Format dfmt, Enesim_Format sfmt, Enesim_Format mfmt);
EAPI Enesim_Compositor_Point enesim_compositor_point_pixel_color_get(Enesim_Rop rop,
		Enesim_Format dfmt, Enesim_Format sfmt, Enesim_Color color);
/**
 *
 */
EAPI void enesim_compositor_point_color_register(Enesim_Compositor_Point pt,
		Enesim_Rop rop, Enesim_Format dfmt);
EAPI void enesim_compositor_point_pixel_register(Enesim_Compositor_Point pt,
		Enesim_Rop rop, Enesim_Format dfmt, Enesim_Format sfmt);
EAPI void enesim_compositor_point_mask_color_register(Enesim_Compositor_Point pt,
		Enesim_Rop rop, Enesim_Format dfmt, Enesim_Format mfmt);
EAPI void enesim_compositor_point_pixel_mask_register(Enesim_Compositor_Point pt,
		Enesim_Rop rop, Enesim_Format dfmt, Enesim_Format sfmt, Enesim_Format mfmt);
EAPI void enesim_compositor_point_pixel_color_register(Enesim_Compositor_Point pt,
		Enesim_Rop rop, Enesim_Format dfmt, Enesim_Format sfmt);
/**
 * Function to draw a span
 * d = Destination surface data
 * len = The length of the span
 * s = In case of using the surface as pixel source
 * color = To draw with a color or as a multiplier color in case of using s
 * m = in case of using a mask
 */
typedef void (*Enesim_Compositor_Span)(uint32_t *d, uint32_t len, uint32_t *s,
		Enesim_Color color, uint32_t *m);
#define ENESIM_DRAWER_SPAN(f) ((Enesim_Drawer_Span)(f))
/**
 *
 */
EAPI Enesim_Compositor_Span enesim_compositor_span_color_get(Enesim_Rop rop, Enesim_Format dfmt,
		Enesim_Color color);
EAPI Enesim_Compositor_Span enesim_compositor_span_pixel_get(Enesim_Rop rop, Enesim_Format dfmt,
		Enesim_Format sfmt);
EAPI Enesim_Compositor_Span enesim_compositor_span_mask_color_get(Enesim_Rop rop, Enesim_Format dfmt,
		Enesim_Format mfmt, Enesim_Color color);
EAPI Enesim_Compositor_Span enesim_compositor_span_pixel_mask_get(Enesim_Rop rop, Enesim_Format dfmt,
		Enesim_Format sfmt, Enesim_Format mfmt);
EAPI Enesim_Compositor_Span enesim_compositor_span_pixel_color_get(Enesim_Rop rop, Enesim_Format dfmt,
		Enesim_Format sfmt, Enesim_Color color);
/**
 *
 */
EAPI void enesim_compositor_span_color_register(Enesim_Compositor_Span sp,
		Enesim_Rop rop, Enesim_Format dfmt);
EAPI void enesim_compositor_span_pixel_register(Enesim_Compositor_Span sp,
		Enesim_Rop rop, Enesim_Format dfmt, Enesim_Format sfmt);
EAPI void enesim_compositor_span_mask_color_register(Enesim_Compositor_Span sp,
		Enesim_Rop rop, Enesim_Format dfmt, Enesim_Format mfmt);
EAPI void enesim_compositor_span_pixel_mask_register(Enesim_Compositor_Span sp,
		Enesim_Rop rop, Enesim_Format dfmt, Enesim_Format sfmt, Enesim_Format mfmt);
EAPI void enesim_compositor_span_pixel_color_register(Enesim_Compositor_Span sp,
		Enesim_Rop rop, Enesim_Format dfmt, Enesim_Format sfmt);

#define PT_C(f, op) enesim_compositor_##f##_pt_color_##op
#define PT_P(f, sf, op) enesim_compositor_##f##_pt_pixel_##sf##_##op
#define PT_MC(f, mf, op) enesim_compositor_##f##_pt_mask_color_##mf##_##op
#define PT_PC(f, sf, op) enesim_compositor_##f##_pt_pixel_color_##sf##_##op
#define PT_PM(f, sf, mf, op) enesim_compositor_##f##_pt_pixel_mask_##sf##_##mf##_##op

#define SP_C(f, op) enesim_compositor_##f##_sp_color_##op
#define SP_P(f, sf, op) enesim_compositor_##f##_sp_pixel_##sf##_##op
#define SP_MC(f, mf, op) enesim_compositor_##f##_sp_mask_color_##mf##_##op
#define SP_PC(f, sf, op) enesim_compositor_##f##_sp_pixel_color_##sf##_##op
#define SP_PM(f, sf, mf, op) enesim_compositor_##f##_sp_pixel_mask_##sf##_##mf##_##op

/** @} */
#endif /*ENESIM_COMPOSITOR_H_*/
