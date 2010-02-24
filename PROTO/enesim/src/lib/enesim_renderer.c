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
#define ENESIM_MAGIC_RENDERER 0xe7e51402
#define ENESIM_MAGIC_CHECK_RENDERER(d)\
	do {\
		if (!EINA_MAGIC_CHECK(d, ENESIM_MAGIC_RENDERER))\
			EINA_MAGIC_FAIL(d, ENESIM_MAGIC_RENDERER);\
	} while(0)
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
void enesim_renderer_init(Enesim_Renderer *r)
{
	EINA_MAGIC_SET(r, ENESIM_MAGIC_RENDERER);
	/* common properties */
	r->ox = 0;
	r->oy = 0;
	enesim_f16p16_matrix_identity(&r->matrix.values);
}

Eina_Bool enesim_renderer_state_setup(Enesim_Renderer *r)
{
	if (!r->state_setup) return EINA_TRUE;
	return r->state_setup(r);
}

void enesim_renderer_state_cleanup(Enesim_Renderer *r)
{
	if (r->state_cleanup)
		r->state_cleanup(r);
}

void enesim_renderer_span_fill(Enesim_Renderer *r, int x, int y,
	unsigned int len, uint32_t *dst)
{
	if (r->span)
		r->span(r, x, y, len, dst);
}
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
/**
 * Sets the transformation matrix of a renderer
 * @param[in] r The renderer to set the transformation matrix on
 * @param[in] m The transformation matrix to set
 */
EAPI void enesim_renderer_transform_set(Enesim_Renderer *r, Enesim_Matrix *m)
{
	ENESIM_MAGIC_CHECK_RENDERER(r);

	if (!m)
	{
		enesim_f16p16_matrix_identity(&r->matrix.values);
		r->matrix.type = ENESIM_MATRIX_IDENTITY;
		return;
	}
	enesim_matrix_f16p16_matrix_to(m, &r->matrix.values);
	r->matrix.type = enesim_f16p16_matrix_type_get(&r->matrix.values);
}
/**
 * Deletes a renderer
 * @param[in] r The renderer to delete
 */
EAPI void enesim_renderer_delete(Enesim_Renderer *r)
{
	ENESIM_MAGIC_CHECK_RENDERER(r);
	enesim_renderer_state_cleanup(r);
	if (r->free)
		r->free(r);
	free(r);
}
/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void enesim_renderer_origin_set(Enesim_Renderer *r, int x, int y)
{
	ENESIM_MAGIC_CHECK_RENDERER(r);
	r->ox = x;
	r->oy = y;
}
/**
 * To be documented
 * FIXME: To be fixed
 * What about the mask?
 */
EAPI void enesim_renderer_surface_draw(Enesim_Renderer *r, Enesim_Surface *s,
		Enesim_Rop rop, Enesim_Color color, Eina_Rectangle *clip)
{
	Enesim_Compositor_Span span;
	int x = 0, y = 0, h, w;
	uint32_t *ddata;
	int stride;
	Enesim_Format dfmt;

	ENESIM_MAGIC_CHECK_RENDERER(r);
	ENESIM_MAGIC_CHECK_SURFACE(s);

	enesim_renderer_state_setup(r);
	if (!clip)
		enesim_surface_size_get(s, &w, &h);
	else
	{
		x = clip->x;
		y = clip->y;
		w = clip->w;
		h = clip->h;
	}

	dfmt = enesim_surface_format_get(s);
	ddata = enesim_surface_data_get(s);
	stride = enesim_surface_stride_get(s);
	ddata = ddata + (y * stride) + x;

	/* fill the new span */
	if ((rop == ENESIM_FILL) && (color == ENESIM_COLOR_FULL))
	{
		while (h--)
		{
			enesim_renderer_span_fill(r, x, y, w, ddata);
			y++;
			ddata += stride;
		}
	}
	else
	{
		uint32_t *fdata;

		span = enesim_compositor_span_get(rop, &dfmt, ENESIM_FORMAT_ARGB8888,
				color, ENESIM_FORMAT_NONE);

		fdata = alloca(clip->w * sizeof(uint32_t));
		while (h--)
		{
			enesim_renderer_span_fill(r, x, y, w, fdata);
			/* compose the filled and the destination spans */
			span(ddata, w, fdata, color, NULL);
			y++;
			ddata += stride;
		}
	}
	/* TODO set the format again */
}
