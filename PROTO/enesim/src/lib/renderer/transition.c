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
typedef struct _Transition {
	Enesim_Renderer base;

	int interp;
	struct {
		int x, y;
	} offset;

	struct {
		Enesim_F16p16_Matrix m;
		Enesim_Matrix_Type mtype;
		Enesim_Renderer *rend;
	} r0, r1;
} Transition;

static void _span_general(Enesim_Renderer *r, int x, int y, unsigned int len, uint32_t *dst)
{
	Transition *t = (Transition *)r;
	Enesim_Renderer *s0 = t->r0.rend, *s1 = t->r1.rend;
	int interp = t->interp;
	unsigned int *d = dst, *e = d + len;
	unsigned int *buf;

	if (interp == 0)
	{
		s0->span(s0, x, y, len, d);
		return;
	}
	if (interp == 256)
	{
		s1->span(s1, t->offset.x + x, t->offset.y + y, len, d);
		return;
	}
	buf = alloca(len * sizeof(unsigned int));
	s1->span(s1, t->offset.x + x, t->offset.y + y, len, buf);
	s0->span(s0, x, y, len, d);

	while (d < e)
	{
		uint32_t p0 = *d, p1 = *buf;

		*d++ = INTERP_256(interp, p1, p0);
		buf++;
	}
}

static Eina_Bool _state_setup(Enesim_Renderer *r)
{
	Transition *t = (Transition *)r;

	if (!t || !t->r0.rend || !t->r1.rend)
		return EINA_FALSE;

	/* compound the matrices of the transition renderer and each
	 * child renderer
	 */
	if (r->matrix.type != ENESIM_MATRIX_IDENTITY)
	{
		/* keep the original matrix values for later */
		t->r0.m = t->r0.rend->matrix.values;
		t->r0.mtype = t->r0.rend->matrix.type;
		t->r1.m = t->r1.rend->matrix.values;
		t->r1.mtype = t->r1.rend->matrix.type;

		enesim_f16p16_matrix_compose(&t->r0.m, &r->matrix.values,
				&t->r0.rend->matrix.values);
		t->r0.rend->matrix.type = enesim_f16p16_matrix_type_get(
				&t->r0.rend->matrix.values);
		enesim_f16p16_matrix_compose(&t->r1.m, &r->matrix.values,
				&t->r1.rend->matrix.values);
		t->r1.rend->matrix.type = enesim_f16p16_matrix_type_get(
				&t->r1.rend->matrix.values);
	}
	if (!enesim_renderer_state_setup(t->r0.rend))
		return EINA_FALSE;
	if (!enesim_renderer_state_setup(t->r1.rend))
		return EINA_FALSE;

	r->span = ENESIM_RENDERER_SPAN_DRAW(_span_general);

	return EINA_TRUE;
}

static void _state_cleanup(Enesim_Renderer *r)
{
	Transition *trans = (Transition *)r;

	enesim_renderer_state_cleanup(trans->r0.rend);
	enesim_renderer_state_cleanup(trans->r1.rend);
	/* restore the original matrices
	 */
	if (r->matrix.type != ENESIM_MATRIX_IDENTITY)
	{
		trans->r0.rend->matrix.values = trans->r0.m;
		trans->r0.rend->matrix.type = trans->r0.mtype;
		trans->r1.rend->matrix.values = trans->r1.m;
		trans->r1.rend->matrix.type = trans->r1.mtype;
	}
}

static void _free(Enesim_Renderer *r)
{
}
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
/**
 * Creates a transition renderer
 * @return The new renderer
 */
EAPI Enesim_Renderer * enesim_renderer_transition_new(void)
{
	Enesim_Renderer *r;
	Transition *t;

	t = calloc(1, sizeof(Transition));
	r = (Enesim_Renderer *)t;

	enesim_renderer_init(r);
	r->state_setup = ENESIM_RENDERER_STATE_SETUP(_state_setup);
	r->state_cleanup = ENESIM_RENDERER_STATE_CLEANUP(_state_cleanup);
	r->free = ENESIM_RENDERER_DELETE(_free);
	r->changed = EINA_TRUE;

	return r;
}
/**
 * Sets the transition level
 * @param[in] r The transition renderer
 * @param[in] interp_value The transition level. A value of 0 will render
 * the source renderer, a value of 1 will render the target renderer
 * and any other value will inteprolate between both renderers
 */
EAPI void enesim_renderer_transition_value_set(Enesim_Renderer *r, float interp_value)
{
	Transition *t = (Transition *)r;

	if (interp_value < 0.0000001)
		interp_value = 0;
	if (interp_value > 0.999999)
		interp_value = 1;
	if (t->interp == interp_value)
		return;
	t->interp = 1 + (255 * interp_value);
}
/**
 * Sets the source renderer
 * @param[in] r The transition renderer
 * @param[in] r0 The source renderer
 */
EAPI void enesim_renderer_transition_source_set(Enesim_Renderer *r, Enesim_Renderer *r0)
{
	Transition *t;

	if (r0 == r)
		return;
	t = (Transition *)r;
	if (t->r0.rend == r0)
		return;
	t->r0.rend = r0;
}
/**
 * Sets the target renderer
 * @param[in] r The transition renderer
 * @param[in] r1 The target renderer
 */
EAPI void enesim_renderer_transition_target_set(Enesim_Renderer *r, Enesim_Renderer *r1)
{
	Transition *t;

	if (r1 == r)
		return;
	t = (Transition *)r;
	if (t->r1.rend == r1)
		return;
	t->r1.rend = r1;
}
/**
 * To be documented
 * FIXME: To be fixed
 * FIXME why do we need this?
 */
EAPI void enesim_renderer_transition_offset_set(Enesim_Renderer *r, int x, int y)
{
	Transition *t = (Transition *)r;

	if ((t->offset.x == x) && (t->offset.y == y))
		return;
	t->offset.x = x;
	t->offset.y = y;
}
