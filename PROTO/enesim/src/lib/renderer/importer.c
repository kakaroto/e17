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
typedef struct _Importer
{
	Enesim_Renderer base;
	Enesim_Converter_Data cdata;
	Enesim_Converter_Format cfmt;
	Enesim_Angle angle;
} Importer;

static void _span_argb8888_none_argb8888(Enesim_Renderer *r, int x, int y, unsigned int len, uint32_t *dst)
{
	Importer *i = (Importer *)r;
	uint32_t *ssrc = i->cdata.pixels.argb8888.plane0;

	ssrc = ssrc + (i->cdata.pixels.argb8888.plane0_stride * y) + x;
	while (len--)
	{
		uint16_t a = (*ssrc >> 24) + 1;

		if (a != 256)
		{
			*dst = (*ssrc & 0xff000000) + (((((*ssrc) >> 8) & 0xff) * a) & 0xff00) +
			(((((*ssrc) & 0x00ff00ff) * a) >> 8) & 0x00ff00ff);
		}
		else
			*dst = *ssrc;

		dst++;
		ssrc++;
	}
}

static void _span_a8_none_argb8888(Enesim_Renderer *r, int x, int y, unsigned int len, uint32_t *dst)
{
	Importer *i = (Importer *)r;
	uint8_t *ssrc = i->cdata.pixels.a8.plane0;

	ssrc = ssrc + (i->cdata.pixels.a8.plane0_stride * y) + x;
	while (len--)
	{
		*dst = *ssrc << 24;;

		dst++;
		ssrc++;
	}
}

static void _span_rgb888_none_argb8888(Enesim_Renderer *r, int x, int y, unsigned int len, uint32_t *dst)
{
	Importer *i = (Importer *)r;
	uint8_t *ssrc = i->cdata.pixels.rgb888.plane0;

	ssrc = ssrc + (i->cdata.pixels.rgb888.plane0_stride * y * 3) + (x * 3);
	while (len--)
	{
		uint8_t r, g, b;

		r = *ssrc++;
		g = *ssrc++;
		b = *ssrc++;
		*dst = 0xff000000 | r << 16 | g << 8 | b;

		dst++;
	}
}

static Eina_Bool _state_setup(Enesim_Renderer *r)
{
	Importer *i = (Importer *)r;

	/* TODO use a LUT for this */
	switch (i->cfmt)
	{
		case ENESIM_CONVERTER_ARGB8888:
		r->span = ENESIM_RENDERER_SPAN_DRAW(_span_argb8888_none_argb8888);
		break;

		case ENESIM_CONVERTER_A8:
		r->span = ENESIM_RENDERER_SPAN_DRAW(_span_a8_none_argb8888);
		break;

		case ENESIM_CONVERTER_RGB888:
		r->span = ENESIM_RENDERER_SPAN_DRAW(_span_rgb888_none_argb8888);
		break;

		default:
		break;
	}
	return EINA_TRUE;
}
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI Enesim_Renderer * enesim_renderer_importer_new(void)
{
	Importer *i;
	Enesim_Renderer *r;

	i = calloc(1, sizeof(Importer));

	r = (Enesim_Renderer *)i;
	enesim_renderer_init(r);
	r->state_setup = ENESIM_RENDERER_STATE_SETUP(_state_setup);
	return r;
}
/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void enesim_renderer_importer_angle_set(Enesim_Renderer *r, Enesim_Angle angle)
{
	Importer *i = (Importer *)r;

	i->angle = angle;
}
/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void enesim_renderer_importer_data_set(Enesim_Renderer *r, Enesim_Converter_Data *cdata)
{
	Importer *i = (Importer *)r;

	i->cdata = *cdata;
}
/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void enesim_renderer_importer_format_set(Enesim_Renderer *r, Enesim_Converter_Format fmt)
{
	Importer *i = (Importer *)r;

	i->cfmt = fmt;
}
