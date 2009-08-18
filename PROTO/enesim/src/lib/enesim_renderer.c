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
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
/**
 * To be documented
 * FIXME: To be fixed
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
 * To be documented
 * FIXME: To be fixed
 */
EAPI void enesim_renderer_delete(Enesim_Renderer *r)
{
	ENESIM_MAGIC_CHECK_RENDERER(r);
	if (r->free)
		r->free(r);
	free(r);
}
/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI Eina_Bool enesim_renderer_state_setup(Enesim_Renderer *r)
{
	ENESIM_MAGIC_CHECK_RENDERER(r);
	if (!r->state_setup) return EINA_TRUE;
	return r->state_setup(r);
}
/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void enesim_renderer_state_cleanup(Enesim_Renderer *r)
{
	ENESIM_MAGIC_CHECK_RENDERER(r);
	if (r->state_cleanup)
		r->state_cleanup(r);
}
/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void enesim_renderer_span_fill(Enesim_Renderer *r, int x, int y,
	unsigned int len, uint32_t *dst)
{
	ENESIM_MAGIC_CHECK_RENDERER(r);
	if (r->span)
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
	ENESIM_MAGIC_CHECK_RENDERER(r);
	r->ox = x;
	r->oy = y;
}
