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
 *                                 Global                                     *
 *============================================================================*/
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
enesim_surface_new(Enesim_Surface_Format f, int w, int h)
{
	Enesim_Surface *s;

	s = calloc(1, sizeof(Enesim_Surface));
	s->w = w;
	s->h = h;
	s->sdata.format = f;

	ENESIM_MAGIC_SET(s, ENESIM_SURFACE_MAGIC);
	switch (f)
	{
		case ENESIM_SURFACE_ARGB8888:
		s->sdata.data.argb8888.plane0 = calloc(w * h, sizeof(unsigned int));
		break;

		case ENESIM_SURFACE_ARGB8888_UNPRE:
		s->sdata.data.argb8888_unpre.plane0 = calloc(w * h, sizeof(unsigned int));
		break;

		case ENESIM_SURFACE_RGB565_XA5:
		s->sdata.data.rgb565_xa5.plane0 = calloc(w * h, sizeof(unsigned short int));
		s->sdata.data.rgb565_xa5.plane1 = calloc(w * h, sizeof(unsigned char));
		break;

		case ENESIM_SURFACE_RGB565_B1A3:
		s->sdata.data.rgb565_b1a3.plane0 = calloc(w * h, sizeof(unsigned short int));
		/* TODO FIX THIS */
		s->sdata.data.rgb565_b1a3.plane1 = calloc(w * h, sizeof(unsigned char));
		break;

		case ENESIM_SURFACE_RGB888_A8:
		break;

		case ENESIM_SURFACE_A8:
		break;

		case ENESIM_SURFACE_b1A3:
		break;

		default:
		goto err;
	}
	return s;
err:
	free(s);
	return NULL;

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
EAPI Enesim_Surface_Format
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
	free(s->sdata.data.argb8888.plane0);
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
	ENESIM_ASSERT(sdata, ENESIM_ERROR_HANDLE_INVALID);
	switch (sdata->format)
	{
	case ENESIM_SURFACE_ARGB8888:
		argb8888_data_increment(sdata, len);
		break;
#ifdef BUILD_SURFACE_ARGB8888_UNPRE
	case ENESIM_SURFACE_ARGB8888_UNPRE:
		argb8888_unpre_data_increment(sdata, len);
		break;
#endif
#ifdef BUILD_SURFACE_RGB565_XA5
	case ENESIM_SURFACE_RGB565_XA5:
		rgb565_xa5_data_increment(sdata, len);
		break;
#endif
#ifdef BUILD_SURFACE_RGB565_B1A3
	case ENESIM_SURFACE_RGB565_B1A3:
		rgb565_b1a3_data_increment(sdata, len);
		break;
#endif
	default:
		break;
	}
}
/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI uint32_t
enesim_surface_data_argb_to(Enesim_Surface_Data *sdata)
{
	uint32_t argb;

	ENESIM_ASSERT(sdata, ENESIM_ERROR_HANDLE_INVALID);

	switch (sdata->format)
	{
	case ENESIM_SURFACE_ARGB8888:
		argb8888_to_argb(&argb, *(sdata->data.argb8888.plane0));
		break;
#ifdef BUILD_SURFACE_ARGB8888_UNPRE
	case ENESIM_SURFACE_ARGB8888_UNPRE:
		argb8888_unpre_to_argb(&argb, *(sdata->data.argb8888_unpre.plane0));
		break;
#endif
#ifdef BUILD_SURFACE_RGB565_XA5
	case ENESIM_SURFACE_RGB565_XA5:
		rgb565_xa5_to_argb(&argb, *(sdata->data.rgb565_xa5.plane0), *(sdata->data.rgb565_xa5.plane1));
		break;
#endif
#ifdef BUILD_SURFACE_RGB565_B1A3
	case ENESIM_SURFACE_RGB565_B1A3:
		rgb565_b1a3_to_argb(&argb, *(sdata->data.rgb565_b1a3.plane0), *(sdata->data.rgb565_b1a3.plane1), sdata->data.rgb565_b1a3.pixel_plane1);
		break;
#endif
	default:
		argb = 0;
		break;
	}
	return argb;
}
/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void
enesim_surface_data_argb_from(Enesim_Surface_Data *sdata, uint32_t argb)
{
	/* FILL THIS */
	switch (sdata->format)
	{
	default:
		break;
	}
}
/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void
enesim_surface_data_pixel_set(Enesim_Surface_Data *d, Enesim_Surface_Pixel *p)
{
	switch (d->format)
	{
		case ENESIM_SURFACE_ARGB8888:
		*d->data.argb8888.plane0 = p->pixel.argb8888.plane0;
		break;

#ifdef BUILD_SURFACE_ARGB8888_UNPRE
		case ENESIM_SURFACE_ARGB8888_UNPRE:
		*d->data.argb8888_unpre.plane0 = p->pixel.argb8888_unpre.plane0;
		break;
#endif

#ifdef BUILD_SURFACE_RGB565_XA5
		case ENESIM_SURFACE_RGB565_XA5:
		break;
#endif

#ifdef BUILD_SURFACE_RGB565_B1A3
		case ENESIM_SURFACE_RGB565_B1A3:
		break;
#endif

		default:
		break;
	}
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void
enesim_surface_data_pixel_get(Enesim_Surface_Data *d, Enesim_Surface_Pixel *p)
{
	switch (d->format)
	{
		case ENESIM_SURFACE_ARGB8888:
		p->pixel.argb8888.plane0 = *d->data.argb8888.plane0;
		break;

#ifdef BUILD_SURFACE_ARGB8888_UNPRE
		case ENESIM_SURFACE_ARGB8888_UNPRE:
		p->pixel.argb8888_unpre.plane0 = *d->data.argb8888_unpre.plane0;
		break;
#endif

#ifdef BUILD_SURFACE_RGB565_XA5
		case ENESIM_SURFACE_RGB565_XA5:
		break;
#endif

#ifdef BUILD_SURFACE_RGB565_B1A3
		case ENESIM_SURFACE_RGB565_B1A3:
		break;
#endif

		default:
		break;
	}
	p->format = d->format;
}
/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI uint32_t
enesim_surface_pixel_argb_to(Enesim_Surface_Pixel *sp)
{
	uint32_t argb;

	switch (sp->format)
	{
	case ENESIM_SURFACE_ARGB8888:
		argb = sp->pixel.argb8888.plane0;
		break;
#ifdef BUILD_SURFACE_ARGB8888_UNPRE
	case ENESIM_SURFACE_ARGB8888_UNPRE:
		argb8888_unpre_to_argb(&argb, sp->pixel.argb8888_unpre.plane0);
		break;
#endif
#if 0
#ifdef BUILD_SURFACE_RGB565_XA5
	case ENESIM_SURFACE_RGB565_XA5:
		rgb565_xa5_to_argb(&argb, *(sdata->rgb565_xa5.plane0), *(sdata->rgb565_xa5.plane1));
		break;
#endif
#ifdef BUILD_SURFACE_RGB565_B1A3
	case ENESIM_SURFACE_RGB565_B1A3:
		rgb565_b1a3_to_argb(&argb, *(sdata->rgb565_b1a3.plane0), *(sdata->rgb565_b1a3.plane1), sdata->rgb565_b1a3.pixel_plane1);
		break;
#endif
#endif
	default:
		argb = 0;
		break;
	}
	return argb;
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void
enesim_surface_pixel_argb_from(Enesim_Surface_Pixel *dp, Enesim_Surface_Format df, uint32_t argb)
{
	switch (df)
	{
	case ENESIM_SURFACE_ARGB8888:
		dp->pixel.argb8888.plane0 = argb;
		break;
#ifdef BUILD_SURFACE_ARGB8888_UNPRE
	case ENESIM_SURFACE_ARGB8888_UNPRE:
		argb8888_unpre_from_argb(argb, &dp->pixel.argb8888_unpre.plane0);
		break;
#endif
#if 0
#ifdef BUILD_SURFACE_RGB565_XA5
	case ENESIM_SURFACE_RGB565_XA5:
		rgb565_xa5_from_argb(argb, &(dp->pixel.rgb565_xa5.plane0), &(dp->pixel.rgb565_xa5.plane1));
		break;
#endif
#ifdef BUILD_SURFACE_RGB565_B1A3
	case ENESIM_SURFACE_RGB565_B1A3:
		rgb565_b1a3_from_argb(argb, &(dp->pixel.rgb565_b1a3.plane0), &(dp->pixel.rgb565_b1a3.plane1), dp->rgb565_b1a3.pixel_plane1);
		break;
#endif
#endif
	default:
		break;
	}
	dp->format = df;
}


/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void
enesim_surface_pixel_convert(Enesim_Surface_Pixel *sp, Enesim_Surface_Pixel *dp, Enesim_Surface_Format df)
{
	uint32_t argb;

	if (sp->format == df)
	{
		*dp = *sp;
		return;
	}
	/* convert to intermediate format */
	if (sp->format != ENESIM_SURFACE_ARGB8888)
	{
		argb = enesim_surface_pixel_argb_to(sp);
	}
	else
	{
		argb = sp->pixel.argb8888.plane0;
	}
	/* convert from intermediate format */
	if (df != ENESIM_SURFACE_ARGB8888)
	{
		enesim_surface_pixel_argb_from(dp, df, argb);
	}
	else
	{
		dp->pixel.argb8888.plane0 = argb;
		dp->format = ENESIM_SURFACE_ARGB8888;
	}
}

/**
 * Create a pixel from the given unpremultiplied components
 */
EAPI void enesim_surface_pixel_components_from(Enesim_Surface_Pixel *color,
		Enesim_Surface_Format f, uint8_t a, uint8_t r, uint8_t g, uint8_t b)
{
	switch (f)
	{
		case ENESIM_SURFACE_ARGB8888:
			argb8888_pixel_components_from(color, a, r, g, b);
			break;
	#ifdef BUILD_SURFACE_ARGB8888_UNPRE
		case ENESIM_SURFACE_ARGB8888_UNPRE:

			break;
	#endif
	#if 0
	#ifdef BUILD_SURFACE_RGB565_XA5
		case ENESIM_SURFACE_RGB565_XA5:
			//rgb565_xa5_from_argb(argb, &(dp->pixel.rgb565_xa5.plane0), &(dp->pixel.rgb565_xa5.plane1));
			break;
	#endif
	#ifdef BUILD_SURFACE_RGB565_B1A3
		case ENESIM_SURFACE_RGB565_B1A3:
			//rgb565_b1a3_from_argb(argb, &(dp->pixel.rgb565_b1a3.plane0), &(dp->pixel.rgb565_b1a3.plane1), dp->rgb565_b1a3.pixel_plane1);
			break;
	#endif
	#endif
		default:
			break;
	}
	color->format = f;
}
/**
 *
 */
EAPI void enesim_surface_pixel_components_to(Enesim_Surface_Pixel *color,
		uint8_t *a, uint8_t *r, uint8_t *g, uint8_t *b)
{
	switch (color->format)
	{
		case ENESIM_SURFACE_ARGB8888:
			argb8888_pixel_components_to(color, a, r, g, b);
			break;
	#ifdef BUILD_SURFACE_ARGB8888_UNPRE
		case ENESIM_SURFACE_ARGB8888_UNPRE:

			break;
	#endif
	#if 0
	#ifdef BUILD_SURFACE_RGB565_XA5
		case ENESIM_SURFACE_RGB565_XA5:
			//rgb565_xa5_from_argb(argb, &(dp->pixel.rgb565_xa5.plane0), &(dp->pixel.rgb565_xa5.plane1));
			break;
	#endif
	#ifdef BUILD_SURFACE_RGB565_B1A3
		case ENESIM_SURFACE_RGB565_B1A3:
			//rgb565_b1a3_from_argb(argb, &(dp->pixel.rgb565_b1a3.plane0), &(dp->pixel.rgb565_b1a3.plane1), dp->rgb565_b1a3.pixel_plane1);
			break;
	#endif
	#endif
		default:
			break;
	}
}
/**
 * Debug routine that returns the format of a surface
 * as a string
 */
EAPI const char * enesim_surface_format_name_get(Enesim_Surface_Format f)
{
	switch (f)
	{
		case ENESIM_SURFACE_ARGB8888:
		return "argb8888";
		break;

		case ENESIM_SURFACE_ARGB8888_UNPRE:
		return "argb8888_unpre";
		break;

		case ENESIM_SURFACE_RGB565_XA5:
		return "rgb565_xa5";
		break;

		case ENESIM_SURFACE_RGB565_B1A3:
		return "rgb565_b1a3";
		break;

		case ENESIM_SURFACE_RGB888_A8:
		return "rgb888_a8";
		break;

		case ENESIM_SURFACE_A8:
		return "a8";
		break;

		case ENESIM_SURFACE_b1A3:
		return "b1a3";
		break;

		default:
		return NULL;
		break;
	}
}

/**
 *
 */
EAPI void enesim_surface_private_set(Enesim_Surface *s, void *data)
{
	s->user = data;
}

/**
 *
 */
EAPI void * enesim_surface_private_get(Enesim_Surface *s)
{
	return s->user;
}
