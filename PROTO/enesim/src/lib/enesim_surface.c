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
#define ENESIM_MAGIC_SURFACE 0xe7e51401
#define ENESIM_MAGIC_CHECK_SURFACE(d)					\
	do {								\
		if (!EINA_MAGIC_CHECK(d, ENESIM_MAGIC_SURFACE))		\
			EINA_MAGIC_FAIL(d, ENESIM_MAGIC_SURFACE);		\
	} while(0)
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
/**
 * To be documented
 * FIXME: To be fixed
 * FIXME I dont like much this function as the user must allocate the sdata himself
 */
EAPI Enesim_Surface *
enesim_surface_new_data_from(int w, int h, void *data)
{
	Enesim_Surface *s;

	s = calloc(1, sizeof(Enesim_Surface));
	s->w = w;
	s->h = h;
	s->data = data;
	EINA_MAGIC_SET(s, ENESIM_MAGIC_SURFACE);

	return s;
}
#if 0
EAPI Enesim_Surface *
enesim_surface_new_allocator_from(int w, int h, Eina_Mempool *mpool)
{
	Enesim_Surface *s;

	s = calloc(1, sizeof(Enesim_Surface));
	s->w = w;
	s->h = h;
	s->data = data;
	EINA_MAGIC_SET(s, ENESIM_MAGIC_SURFACE);

	return s;
}
#endif
/**
 *
 */
EAPI Enesim_Surface *
enesim_surface_new(Enesim_Format f, int w, int h)
{
	Enesim_Surface *s;

	s = calloc(1, sizeof(Enesim_Surface));
	s->w = w;
	s->h = h;
	s->stride = h;
	s->format = f;
	s->flags = ENESIM_ALPHA_ALL;
	switch (f)
	{
		case ENESIM_FORMAT_ARGB8888:
		s->data = calloc(w * h, sizeof(uint32_t));
		break;

		case ENESIM_FORMAT_A8:
		s->data = calloc(w * h, sizeof(uint8_t));
		break;

		default:
		break;
	}
	EINA_MAGIC_SET(s, ENESIM_MAGIC_SURFACE);
	return s;
}
/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void
enesim_surface_size_get(const Enesim_Surface *s, int *w, int *h)
{
	ENESIM_MAGIC_CHECK_SURFACE(s);
	if (w) *w = s->w;
	if (h) *h = s->h;
}
/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void enesim_surface_size_set(Enesim_Surface *s, int w, int h)
{
	ENESIM_MAGIC_CHECK_SURFACE(s);
	s->w = w;
	s->h = h;
}
/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI Enesim_Format enesim_surface_format_get(const Enesim_Surface *s)
{
	ENESIM_MAGIC_CHECK_SURFACE(s);
	return s->format;
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void
enesim_surface_delete(Enesim_Surface *s)
{
	ENESIM_MAGIC_CHECK_SURFACE(s);
	/* FIXME delete correctly, when a user provides the data
	 * we should NOT delete it */
	if (!s->external)
	{
		free(s->data);
	}
	free(s);
}
/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void *
enesim_surface_data_get(const Enesim_Surface *s)
{
	ENESIM_MAGIC_CHECK_SURFACE(s);
	return s->data;
}
/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void
enesim_surface_data_set(Enesim_Surface *s, void *data)
{
	ENESIM_MAGIC_CHECK_SURFACE(s);
	/* TODO check if we already had data */
	s->data = data;
}
/**
 * Create a pixel from the given unpremultiplied components
 */
EAPI void enesim_surface_pixel_components_from(uint32_t *color,
		Enesim_Format f, uint8_t a, uint8_t r, uint8_t g, uint8_t b)
{
	switch (f)
	{
		case ENESIM_FORMAT_ARGB8888:
		{
			uint16_t alpha = a + 1;
			*color = (a << 24) | (((r * alpha) >> 8) << 16)
					| (((g * alpha) >> 8) << 8)
					| ((b * alpha) >> 8);
		}
		break;

		case ENESIM_FORMAT_A8:
		*color = a;
		break;

		default:
		break;
	}
}

/**
 *
 */
EAPI void enesim_surface_pixel_components_to(uint32_t color,
		Enesim_Format f, uint8_t *a, uint8_t *r, uint8_t *g, uint8_t *b)
{
	switch (f)
	{
		case ENESIM_FORMAT_ARGB8888:
		{
			uint8_t pa;
			pa = (color >> 24);
			if ((pa > 0) && (pa < 255))
			{
				if (a) *a = pa;
				if (r) *r = (argb8888_red_get(color) * 255) / pa;
				if (g) *g = (argb8888_green_get(color) * 255) / pa;
				if (b) *b = (argb8888_blue_get(color) * 255) / pa;
			}
			else
			{
				if (a) *a = pa;
				if (r) *r = argb8888_red_get(color);
				if (g) *g = argb8888_green_get(color);
				if (b) *b = argb8888_blue_get(color);
			}
		}
		break;

		case ENESIM_FORMAT_A8:
		if (a) *a = (uint8_t)color;
		break;

		default:
		break;
	}
}

/**
 * Retrieve an area of the original surface
 */
EAPI Enesim_Surface * enesim_surface_sub_get(Enesim_Surface *s, Eina_Rectangle *r)
{
	Enesim_Surface *ss;

	ENESIM_MAGIC_CHECK_SURFACE(s);
	ss = malloc(sizeof(Enesim_Surface));
	ss->external = EINA_TRUE;
	ss->h = r->h;
	ss->w = r->w;
	switch (s->format)
	{
		case ENESIM_FORMAT_ARGB8888:
		ss->data = ((uint32_t *)s->data) + (r->y * s->stride) + r->x;
		break;

		case ENESIM_FORMAT_A8:
		ss->data = ((uint8_t *)s->data) + (r->y * s->stride) + r->x;
		break;

		default:
		break;
	}
	ss->flags = s->flags;
	ss->format = s->format;
	ss->stride = s->stride;
	EINA_MAGIC_SET(s, ENESIM_MAGIC_SURFACE);

	return ss;
}
/**
 * Store a private data pointer into the surface
 */
EAPI void enesim_surface_private_set(Enesim_Surface *s, void *data)
{
	s->user = data;
}

/**
 * Retrieve the private data pointer from the surface
 */
EAPI void * enesim_surface_private_get(Enesim_Surface *s)
{
	return s->user;
}
