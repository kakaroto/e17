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
#ifndef ENESIM_PRIMITIVES_H_
#define ENESIM_PRIMITIVES_H_

EAPI void enesim_rect_draw(Enesim_Surface *s, Enesim_Context *c,
		uint32_t x, uint32_t y, uint32_t w, uint32_t h);
EAPI void enesim_image_draw(Enesim_Surface *d, Enesim_Context *c,
		Enesim_Surface *s, Eina_Rectangle *srect);
#endif /* ENESIM_PRIMITIVES_H_ */
