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
#define ENESIM_MAGIC_RASTERIZER 0xe7e51401
#define ENESIM_MAGIC_CHECK_RASTERIZER(d)\
	do {\
		if (!EINA_MAGIC_CHECK(d, ENESIM_MAGIC_RASTERIZER))\
			EINA_MAGIC_FAIL(d, ENESIM_MAGIC_RASTERIZER);\
	} while(0)
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
void enesim_rasterizer_init(Enesim_Rasterizer *r)
{
	EINA_MAGIC_SET(r, ENESIM_MAGIC_RASTERIZER);
}
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
/**
 * To be documented
 * FIXME: To be fixed
 */
/* TODO replace this call with edge add */
EAPI void enesim_rasterizer_vertex_add(Enesim_Rasterizer *r, float x, float y)
{
	ENESIM_MAGIC_CHECK_RASTERIZER(r);
	assert(r->vertex_add);

	r->vertex_add(r->data, x, y);
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI Eina_Bool enesim_rasterizer_generate(Enesim_Rasterizer *r, Eina_Rectangle *rect,
		Enesim_Scanline_Callback cb, void *data)
{
	ENESIM_MAGIC_CHECK_RASTERIZER(r);
	assert(r->generate);
	assert(cb);

	r->generate(r->data, rect, cb, data);
	return EINA_TRUE;
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void enesim_rasterizer_delete(Enesim_Rasterizer *r)
{
	ENESIM_MAGIC_CHECK_RASTERIZER(r);
	assert(r->delete);

	r->delete(r->data);
}
