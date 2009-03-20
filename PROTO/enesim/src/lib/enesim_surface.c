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
 *                                   API                                      *
 *============================================================================*/
/**
 * To be documented
 * FIXME: To be fixed
 * FIXME I dont like much this function as the user must allocate the sdata himself
 */
EAPI Enesim_Surface *
enesim_surface_new_data_from(int w, int h, Enesim_Surface_Data *sdata)
{
	Enesim_Surface *s;

	s = calloc(1, sizeof(Enesim_Surface));
	s->w = w;
	s->h = h;
	s->sdata = *sdata;
	ENESIM_MAGIC_SET(s, ENESIM_SURFACE_MAGIC);

	return s;
}
/**
 *
 */
EAPI Enesim_Surface *
enesim_surface_new(Enesim_Format *f, int w, int h)
{
	Enesim_Surface *s;

	if (!f)
	{
		printf("surface new unknown format\n");
		return NULL;
	}
	s = calloc(1, sizeof(Enesim_Surface));
	s->w = w;
	s->h = h;
	s->sdata.format = f;
	f->create(&s->sdata, w, h);
	ENESIM_MAGIC_SET(s, ENESIM_SURFACE_MAGIC);
	return s;
}
/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void
enesim_surface_size_get(const Enesim_Surface *s, int *w, int *h)
{
	ENESIM_ASSERT(s, ENESIM_ERROR_HANDLE_INVALID);
	ENESIM_MAGIC_CHECK(s, ENESIM_SURFACE_MAGIC);
	if (w) *w = s->w;
	if (h) *h = s->h;
}
/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void enesim_surface_size_set(Enesim_Surface *s, int w, int h)
{
	ENESIM_ASSERT(s, ENESIM_ERROR_HANDLE_INVALID);
	ENESIM_ASSERT(w >= 0, ENESIM_ERROR_GEOMETRY_INVALID);
	ENESIM_ASSERT(h >= 0, ENESIM_ERROR_GEOMETRY_INVALID);
	ENESIM_MAGIC_CHECK(s, ENESIM_SURFACE_MAGIC);
	s->w = w;
	s->h = h;
}
/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI Enesim_Format *
enesim_surface_format_get(const Enesim_Surface *s)
{
	ENESIM_ASSERT(s, ENESIM_ERROR_HANDLE_INVALID);
	ENESIM_MAGIC_CHECK(s, ENESIM_SURFACE_MAGIC);
	return s->sdata.format;
}
/**
 * To be documented
 * FIXME: To be fixed
 * This should be a wrapper of the convert / transform / scale
 * functions
 */
EAPI void
enesim_surface_convert(Enesim_Surface *s, Enesim_Surface *d, Eina_Rectangle *dr)
{
	Enesim_Drawer_Span sp;
	Enesim_Surface_Data sdata;
	Enesim_Surface_Data ddata;
	int h = d->h;

	sdata = s->sdata;
	ddata = d->sdata;
	sp = enesim_drawer_span_pixel_get(ENESIM_FILL, ddata.format, sdata.format);
	if (!sp)
		return;
	while (h--)
	{
		sp(&ddata, d->w, &sdata, NULL, NULL);
		sdata.format->increment(&sdata, s->w);
		ddata.format->increment(&ddata, d->w);
	}

#if 0
	Enesim_Transformation *tx;
	Enesim_Matrix matrix;
	Eina_Rectangle sr;


	ENESIM_ASSERT(s, ENESIM_ERROR_HANDLE_INVALID);
	ENESIM_ASSERT(d, ENESIM_ERROR_HANDLE_INVALID);
	ENESIM_MAGIC_CHECK(s, ENESIM_SURFACE_MAGIC);
	ENESIM_MAGIC_CHECK(d, ENESIM_SURFACE_MAGIC);


	tx = enesim_transformation_new();
#if 0
	if ((s->w != d->w) || (s->h != d->h))
	{
		//printf("scaling %d %d (%f) %d %d (%f)\n", d->w, s->w, (float)d->w/s->w, d->h, s->h, (float)d->h/s->h);
		enesim_matrix_scale(&matrix, d->w / s->w, d->h / s->h);
	}
	else
	{

	}
#endif
	enesim_matrix_identity(&matrix);
	/* TODO call the correct convert function based on the src
	 * and dst format, the src and dst flags, etc
	 */

	enesim_transformation_matrix_set(tx, &matrix);
	enesim_transformation_rop_set(tx, ENESIM_FILL);
#if 0
	eina_rectangle_coords_from(&sr, 0, 0, s->w, s->h);
	eina_rectangle_coords_from(&dr, 0, 0, d->w, d->h);
	enesim_transformation_apply(tx, s, &sr, d, &dr);
#endif
	enesim_transformation_apply(tx, s, NULL, d, dr);
#endif
}
/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void
enesim_surface_delete(Enesim_Surface *s)
{
	assert(s);
	/* FIXME delete correctly, when a user provides the data
	 * we should NOT delete it */
	if (!s->external)
	{
		s->sdata.format->delete(&s->sdata);
	}
	free(s);
}
/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void
enesim_surface_data_get(const Enesim_Surface *s, Enesim_Surface_Data *sdata)
{
	ENESIM_ASSERT(s, ENESIM_ERROR_HANDLE_INVALID);
	ENESIM_ASSERT(sdata, ENESIM_ERROR_HANDLE_INVALID);
	ENESIM_MAGIC_CHECK(s, ENESIM_SURFACE_MAGIC);
	*sdata = s->sdata;
}
/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void
enesim_surface_data_set(Enesim_Surface *s, const Enesim_Surface_Data *sdata)
{
	ENESIM_ASSERT(s, ENESIM_ERROR_HANDLE_INVALID);
	ENESIM_ASSERT(sdata, ENESIM_ERROR_HANDLE_INVALID);
	ENESIM_MAGIC_CHECK(s, ENESIM_SURFACE_MAGIC);
	/* TODO check if we already had data */
	s->sdata = *sdata;
}
/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void
enesim_surface_data_increment(Enesim_Surface_Data *sdata, unsigned int len)
{
	sdata->format->increment(sdata, len);
}
/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI uint32_t
enesim_surface_data_argb_to(Enesim_Surface_Data *sdata)
{
	return sdata->format->argb_to(sdata);
}
/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void
enesim_surface_data_argb_from(Enesim_Surface_Data *sdata, uint32_t argb)
{
	sdata->format->argb_from(sdata, argb);
}
/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void
enesim_surface_data_pixel_set(Enesim_Surface_Data *d, Enesim_Surface_Pixel *p)
{
	d->format->pixel_set(d, p);
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void
enesim_surface_data_pixel_get(Enesim_Surface_Data *d, Enesim_Surface_Pixel *p)
{
	d->format->pixel_get(d, p);
	p->format = d->format;
}
/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI uint32_t
enesim_surface_pixel_argb_to(Enesim_Surface_Pixel *sp)
{
	return sp->format->pixel_argb_to(sp);
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void
enesim_surface_pixel_argb_from(Enesim_Surface_Pixel *dp, Enesim_Format *df, uint32_t argb)
{
	df->pixel_argb_from(dp, argb);
	dp->format = df;
}


/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void
enesim_surface_pixel_convert(Enesim_Surface_Pixel *sp, Enesim_Surface_Pixel *dp, Enesim_Format *df)
{
	uint32_t argb;

	if (sp->format == df)
	{
		*dp = *sp;
		return;
	}
	/* convert to intermediate format */
	if (sp->format != enesim_format_argb8888_get())
	{
		argb = enesim_surface_pixel_argb_to(sp);
	}
	else
	{
		argb = sp->plane0;
	}
	/* convert from intermediate format */
	if (df != enesim_format_argb8888_get())
	{
		enesim_surface_pixel_argb_from(dp, df, argb);
	}
	else
	{
		dp->plane0 = argb;
		dp->format = enesim_format_argb8888_get();
	}
}

/**
 * Create a pixel from the given unpremultiplied components
 */
EAPI void enesim_surface_pixel_components_from(Enesim_Surface_Pixel *color,
		Enesim_Format *f, uint8_t a, uint8_t r, uint8_t g, uint8_t b)
{
	uint32_t argb;
	uint16_t alpha = a + 1;

	argb = (a << 24) | (((r * alpha) >> 8) << 16) | (((g * alpha) >> 8) << 8)
			| ((b * alpha) >> 8);
	if (f != enesim_format_argb8888_get())
	{
		f->pixel_argb_from(color, argb);
	}
	else
	{
		color->plane0 = argb;
	}
	color->format = f;
}
/**
 *
 */
EAPI void enesim_surface_pixel_components_to(Enesim_Surface_Pixel *color,
		uint8_t *a, uint8_t *r, uint8_t *g, uint8_t *b)
{
	uint32_t argb;
	uint8_t pa;

	if (color->format != enesim_format_argb8888_get())
	{
		argb = color->format->pixel_argb_to(color);
	}
	else
	{
		argb = color->plane0;
	}
	pa = (argb >> 24);
	if ((pa > 0) && (pa < 255))
	{
		if (a) *a = pa;
		if (r) *r = (argb8888_red_get(argb) * 255) / pa;
		if (g) *g = (argb8888_green_get(argb) * 255) / pa;
		if (b) *b = (argb8888_blue_get(argb) * 255) / pa;
	}
	else
	{
		if (a) *a = pa;
		if (r) *r = argb8888_red_get(argb);
		if (g) *g = argb8888_green_get(argb);
		if (b) *b = argb8888_blue_get(argb);
	}
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
