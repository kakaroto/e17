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
Enesim_Renderer * enesim_renderer_new(void)
{
	Enesim_Renderer *r;

	r = calloc(1, sizeof(Enesim_Renderer));
	ENESIM_MAGIC_SET(r, ENESIM_RENDERER_MAGIC);
	return r;
}
/*============================================================================*
 *                                   API                                      * 
 *============================================================================*/
/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void enesim_renderer_rop_set(Enesim_Renderer *r, int rop)
{
	ENESIM_ASSERT(r, ENESIM_ERROR_HANDLE_INVALID);
	ENESIM_MAGIC_CHECK(r, ENESIM_RENDERER_MAGIC);
	r->rop = rop;
}
/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void enesim_renderer_delete(Enesim_Renderer *r)
{
	ENESIM_ASSERT(r, ENESIM_ERROR_HANDLE_INVALID);
	ENESIM_MAGIC_CHECK(r, ENESIM_RENDERER_MAGIC);
	r->funcs->free(r);
	free(r);
}
/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI Eina_Bool enesim_renderer_draw(Enesim_Renderer *r, int type, void *sl, Enesim_Surface *dst)
{
	ENESIM_ASSERT(r, ENESIM_ERROR_HANDLE_INVALID);
	ENESIM_ASSERT(sl, ENESIM_ERROR_HANDLE_INVALID);
	ENESIM_ASSERT(dst, ENESIM_ERROR_HANDLE_INVALID);
	ENESIM_MAGIC_CHECK(r, ENESIM_RENDERER_MAGIC);
	return r->funcs->draw(r, type, sl, dst);
}
