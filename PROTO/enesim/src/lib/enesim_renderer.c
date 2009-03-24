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
EAPI void enesim_renderer_delete(Enesim_Renderer *r)
{

	if (r->free)
		r->free(r);
	else
		free(r);
}
/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI Enesim_Renderer_Span enesim_renderer_func_get(Enesim_Renderer *r, Enesim_Format *f)
{
	return r->get(r, f);
}
