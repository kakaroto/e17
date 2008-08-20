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
#include "enesim_common.h"
#include "Enesim.h"
#include "enesim_private.h"

/*============================================================================*
 *                                 Global                                     * 
 *============================================================================*/
/**
 * To be documented
 * FIXME: To be fixed
 */
Enesim_Rasterizer * enesim_rasterizer_new(void *data, Enesim_Rasterizer_Func
		*funcs, Eina_Rectangle boundaries, int types)
{
	Enesim_Rasterizer *r;

	r = calloc(1, sizeof(Enesim_Rasterizer));
	r->funcs = funcs;
	r->data = data;
	r->boundaries = boundaries;
	r->types = types;
	return r;
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
	assert(r);
	assert(r->funcs);
	assert(r->funcs->vertex_add);
	
	r->funcs->vertex_add(r->data, x, y);
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI Eina_Bool enesim_rasterizer_generate(Enesim_Rasterizer *r,
		Enesim_Scanline_Callback cb, void *data)
{
	assert(r);
	assert(r->funcs);
	assert(r->funcs->generate);
	assert(cb);

	r->user_data = data;
	r->scanline_callback = cb;
	r->funcs->generate(r->data);
	return EINA_TRUE;
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void enesim_rasterizer_delete(Enesim_Rasterizer *r)
{
	assert(r);
	assert(r->funcs);
	assert(r->funcs->delete);
	
	r->funcs->delete(r->data);
	free(r);
}
