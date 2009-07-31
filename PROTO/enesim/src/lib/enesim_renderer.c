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
EAPI void enesim_renderer_transform_set(Enesim_Renderer *r, Enesim_Matrix *m)
{
	if (!r)
		return;
	if (!m)
	{
		r->matrix.axx = r->matrix.ayy = r->matrix.azz = 65536;
		r->matrix.axy = r->matrix.axz = 0;
		r->matrix.ayx = r->matrix.ayz = 0;
		r->matrix.azx = r->matrix.azy = 0;
		r->matrix.is_identity = r->matrix.is_affine = 1;
		return;
	}
	r->matrix.axx = m->xx * 65536;
	r->matrix.axy = m->xy * 65536;
	r->matrix.axz = m->xz * 65536;
	r->matrix.ayx = m->yx * 65536;
	r->matrix.ayy = m->yy * 65536;
	r->matrix.ayz = m->yz * 65536;
	r->matrix.azx = m->zx * 65536;
	r->matrix.azy = m->zy * 65536;
	r->matrix.azz = m->zz * 65536;

	r->matrix.is_affine = r->matrix.is_identity = 0;
	if ( (r->matrix.azx == 0) && (r->matrix.azy == 0) && (r->matrix.azz == 65536) )
	{
		r->matrix.is_affine = 1;
		if ( (r->matrix.axx == 65536) &&
		      (r->matrix.ayy == 65536) &&
		      (r->matrix.axy == 0) &&
		      (r->matrix.axz == 0) &&
		      (r->matrix.ayx == 0) &&
		      (r->matrix.ayz == 0) )
			r->matrix.is_identity = 1;
	}
}
/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void enesim_renderer_delete(Enesim_Renderer *r)
{
	if (r && r->free)
		r->free(r);
	free(r);
}
/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI Eina_Bool enesim_renderer_state_setup(Enesim_Renderer *r)
{
	if (!r) return EINA_FALSE;
	if (!r->state_setup) return EINA_TRUE;
	return r->state_setup(r);
}
/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void enesim_renderer_state_cleanup(Enesim_Renderer *r)
{
	if (r && r->state_cleanup)
		r->state_cleanup(r);
}
/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void enesim_renderer_span_fill(Enesim_Renderer *r, int x, int y,
	unsigned int len, uint32_t *dst)
{
	if (r && r->span)
		r->span(r, x, y, len, dst);

// This won't work well since some renderers may use others
// and they may use calls to the others' internal span funcs.
// It also hides the use of the setup func, making it confusing
// when to use the cleanup one.
// The best use of the 'changed' property is by the renderers
// themselves in their setup function.. If they feel nothing's
// changed, they the setup function may do little or nothing.

#if 0
	if (r->changed && r->state_setup)
	{
		if (!r->state_setup(r))
			return;
	}

	r->span(r, x, y, len, dst);
	r->changed = EINA_FALSE;
#endif
}

EAPI void enesim_renderer_origin_set(Enesim_Renderer *r, int x, int y)
{
	r->ox = x;
	r->oy = y;
}
