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
#ifndef ENESIM_SURFACE_H_
#define ENESIM_SURFACE_H_

/**
 * @defgroup Enesim_Surface_Group Surface
 * @{
 *
 * TODO
 * + Add a pitch, this is good for different planes, still the width and height
 * are good things to have so the pitch
 * + Add a data provider: as a parameter to the new, so the destruction is
 * handled by the library itself, same for data get, etc.
 * + Add a surface iterator
 * + Normalize the occurences of argb, colors, etc. Always premul or flat
 * argb8888?
 * + Be able to pass an Eina_Allocator to surface_new
 */
typedef struct _Enesim_Surface 	Enesim_Surface; /**< Surface Handler */

EAPI Enesim_Surface * enesim_surface_new_data_from(int w, int h, void *data);
EAPI Enesim_Surface * enesim_surface_new(Enesim_Format f, int w, int h);
EAPI void enesim_surface_size_get(const Enesim_Surface *s, int *w, int *h);
EAPI void enesim_surface_size_set(Enesim_Surface *s, int w, int h);
EAPI Enesim_Format enesim_surface_format_get(const Enesim_Surface *s);
EAPI void enesim_surface_delete(Enesim_Surface *s);

EAPI void enesim_surface_private_set(Enesim_Surface *s, void *data);
EAPI void * enesim_surface_private_get(Enesim_Surface *s);

EAPI void * enesim_surface_data_get(const Enesim_Surface *s);
EAPI void enesim_surface_data_set(Enesim_Surface *s, void *data);

EAPI void enesim_surface_pixel_components_from(uint32_t *color,
		Enesim_Format f, uint8_t a, uint8_t r, uint8_t g, uint8_t b);
EAPI void enesim_surface_pixel_components_to(uint32_t color,
		Enesim_Format f, uint8_t *a, uint8_t *r, uint8_t *g, uint8_t *b);
#if 0
EAPI uint32_t enesim_surface_pixel_argb_to(Enesim_Surface_Pixel *sp);
EAPI void enesim_surface_pixel_argb_from(Enesim_Surface_Pixel *dp, Enesim_Format *df, uint32_t argb);
EAPI void enesim_surface_pixel_convert(Enesim_Surface_Pixel *sp, Enesim_Surface_Pixel *dp, Enesim_Format *df);

EAPI void enesim_surface_pixel_components_to(Enesim_Surface_Pixel *color,
		uint8_t *a, uint8_t *r, uint8_t *g, uint8_t *b);
#endif
/** @} */ //End of Enesim_Surface_Group


#endif /*ENESIM_SURFACE_H_*/
