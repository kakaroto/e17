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

/**
 * @defgroup Enesim_Compositor_Group Compositor
 * @{
 */

/**
 * Function to draw a point
 * @param d Destination surface data
 * @param s In case of using the surface as pixel source
 * @param color To draw with a color or as a multiplier color in case of using s
 * @param m In case of using a mask
 */
typedef void (*Enesim_Compositor_Point)(uint32_t *d, uint32_t s,
		Enesim_Color color, uint32_t m);
EAPI Enesim_Compositor_Point enesim_compositor_point_get(Enesim_Rop rop,
		Enesim_Format *dfmt, Enesim_Format sfmt, Enesim_Color color,
		Enesim_Format mfmt);
/**
 * Function to draw a span
 * @param d Destination surface data
 * @param len The length of the span
 * @param s In case of using the surface as pixel source
 * @param color To draw with a color or as a multiplier color in case of using s
 * @param m In case of using a mask
 */
typedef void (*Enesim_Compositor_Span)(uint32_t *d, uint32_t len, uint32_t *s,
		Enesim_Color color, uint32_t *m);

EAPI Enesim_Compositor_Span enesim_compositor_span_get(Enesim_Rop rop,
		Enesim_Format *dfmt, Enesim_Format sfmt, Enesim_Color color,
		Enesim_Format mfmt);

EAPI Enesim_Compositor_Point enesim_compositor_point_get(Enesim_Rop rop,
		Enesim_Format *dfmt, Enesim_Format sfmt, Enesim_Color color,
		Enesim_Format mfmt);

/** @} */
#endif /*ENESIM_COMPOSITOR_H_*/
