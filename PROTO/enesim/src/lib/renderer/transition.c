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

	Enesim_Renderer *r0, *r1;
	int interp;
	struct {
		int x, y;
	} offset;

	struct {
		int axx, axy, axz;
		int ayx, ayy, ayz;
		int azx, azy, azz;
	} t0, t1;
} Transition;

static void _span_general(Transition *t, int x, int y, int len, unsigned int *dst)
{
	Enesim_Renderer *s0 = t->r0, *s1 = t->r1;
	int interp = t->interp;
	unsigned int *d = dst, *e = d + len;
	unsigned int *buf;

	buf = alloca(len * sizeof(unsigned int));
	/* TODO check if the interp value is 0 or 1 (special cases) */
	s1->span(s1, t->offset.x + x, t->offset.y + y, len, buf);
	s0->span(s0, x, y, len, d);

	while (d < e)
	{
		unsigned int p0 = *d, p1 = *buf;

		*d++ = INTERP_256(interp, p0, p1);
		buf++;
	}
}

static Eina_Bool _state_setup(Enesim_Renderer *r)
{
	Transition *trans = (Transition *)r;

	if (!trans || !trans->r0 || !trans->r1)
		return EINA_FALSE;

	// need to save trans->p0,p1 transforms into
	// trans->t0,t1 and reset the trans->p0,p1
	// to their composition with p's transform.
	// etc...
	if (!enesim_renderer_state_setup(trans->r0))
		return EINA_FALSE;
	if (!enesim_renderer_state_setup(trans->r1))
		return EINA_FALSE;

	r->span = ENESIM_RENDERER_SPAN_DRAW(_span_general);
	/*
	 if ((paint_type_id(trans->p0) == IMAGE) &&
	 (paint_type_id(trans->p1) == IMAGE))
	 p->span_func = trans_images;
	 */
	return EINA_TRUE;
}

static void _state_cleanup(Enesim_Renderer *r)
{
	Transition *trans = (Transition *)r;

	// need to also reset trans->p0,p1 transforms
	// from the saved ones in trans->t0,t1...
	enesim_renderer_state_cleanup(trans->r0);
	enesim_renderer_state_cleanup(trans->r1);
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
EAPI Enesim_Renderer * enesim_renderer_transition_new(void)
{
	Enesim_Renderer *r;
	Transition *trans;

	trans = calloc(1, sizeof(Transition));
	if (!trans)
		return NULL;
	r = (Enesim_Renderer *) trans;
	r->state_setup = ENESIM_RENDERER_STATE_SETUP(_state_setup);
	r->state_cleanup = ENESIM_RENDERER_STATE_CLEANUP(_state_cleanup);
	r->free = ENESIM_RENDERER_DELETE(_free);
	// r->axx = p->ayy = p->azz = 65536;
	//   if (!transition_setup_state(p, 0)) { free(trans); return NULL; }
	return r;
}

EAPI void enesim_renderer_transition_value_set(Enesim_Renderer *r, float interp_value)
{
	Transition *trans;

	trans = (Transition *)r;
	if (interp_value < 0.0000001)
		interp_value = 0;
	if (interp_value > 0.999999)
		interp_value = 1;
	trans->interp = 1 + (255 * interp_value);
}

EAPI void enesim_renderer_transition_source_set(Enesim_Renderer *r, Enesim_Renderer *r0)
{
	Transition *trans;

	if (r0 == r)
		return;
	trans = (Transition *)r;
	trans->r0 = r0;
	r->changed = EINA_TRUE;
}

EAPI void enesim_renderer_transition_target_set(Enesim_Renderer *r, Enesim_Renderer *r1)
{
	Transition *trans;

	if (r1 == r)
		return;
	trans = (Transition *)r;
	trans->r1 = r1;
	r->changed = EINA_TRUE;

}

EAPI void enesim_renderer_transition_offset_set(Enesim_Renderer *r, int x, int y)
{
	Transition *trans;

	trans = (Transition *)r;
	trans->offset.x = x;
	trans->offset.y = y;
}
