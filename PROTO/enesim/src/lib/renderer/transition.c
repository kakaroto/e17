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
		unsigned char is_identity : 1;
		unsigned char is_affine : 1;
	} t0, t1;
} Transition;

static void _span_general(Enesim_Renderer *r, int x, int y, unsigned int len, uint32_t *dst)
{
	Transition *t = (Transition *)r;
	Enesim_Renderer *s0 = t->r0, *s1 = t->r1;
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
		unsigned int p0 = *d, p1 = *buf;

		*d++ = INTERP_256(interp, p1, p0);
		buf++;
	}
}

static Eina_Bool _state_setup(Enesim_Renderer *r)
{
	Transition *t = (Transition *)r;

	if (!t || !t->r0 || !t->r1)
		return EINA_FALSE;

	if (!r->matrix.is_identity)
	{
		Enesim_Renderer  *r0 = t->r0, *r1 = t->r1;

		t->t0.axx = r0->matrix.axx;  t->t0.axy = r0->matrix.axy;  t->t0.axz = r0->matrix.axz;
		t->t0.ayx = r0->matrix.ayx;  t->t0.ayy = r0->matrix.ayy;  t->t0.ayz = r0->matrix.ayz;
		t->t0.azx = r0->matrix.azx;  t->t0.azy = r0->matrix.azy;  t->t0.azz = r0->matrix.azz;
		t->t0.is_identity = r0->matrix.is_identity;
		t->t0.is_affine = r0->matrix.is_affine;

		t->t1.axx = r1->matrix.axx;  t->t1.axy = r1->matrix.axy;  t->t1.axz = r1->matrix.axz;
		t->t1.ayx = r1->matrix.ayx;  t->t1.ayy = r1->matrix.ayy;  t->t1.ayz = r1->matrix.ayz;
		t->t1.azx = r1->matrix.azx;  t->t1.azy = r1->matrix.azy;  t->t1.azz = r1->matrix.azz;
		t->t1.is_identity = r1->matrix.is_identity;
		t->t1.is_affine = r1->matrix.is_affine;

		// ... multiply r0,r1->matrices with r->matrix ...
		r0->matrix.axx = (((long long int)t->t0.axx * (r->matrix.axx)) >> 16) +
		                 (((long long int)t->t0.axy * (r->matrix.ayx)) >> 16) +
	        	         (((long long int)t->t0.axz * (r->matrix.azx)) >> 16);
		r0->matrix.axy = (((long long int)t->t0.axx * (r->matrix.axy)) >> 16) +
		                 (((long long int)t->t0.axy * (r->matrix.ayy)) >> 16) +
		                 (((long long int)t->t0.axz * (r->matrix.azy)) >> 16);
		r0->matrix.axz = (((long long int)t->t0.axx * (r->matrix.axz)) >> 16) +
		                 (((long long int)t->t0.axy * (r->matrix.ayz)) >> 16) +
		                 (((long long int)t->t0.axz * (r->matrix.azz)) >> 16);
		r0->matrix.ayx = (((long long int)t->t0.ayx * (r->matrix.axx)) >> 16) +
		                 (((long long int)t->t0.ayy * (r->matrix.ayx)) >> 16) +
		                 (((long long int)t->t0.ayz * (r->matrix.azx)) >> 16);
		r0->matrix.ayy = (((long long int)t->t0.ayx * (r->matrix.axy)) >> 16) +
		                 (((long long int)t->t0.ayy * (r->matrix.ayy)) >> 16) +
		                 (((long long int)t->t0.ayz * (r->matrix.azy)) >> 16);
		r0->matrix.ayz = (((long long int)t->t0.ayx * (r->matrix.axz)) >> 16) +
		                 (((long long int)t->t0.ayy * (r->matrix.ayz)) >> 16) +
		                 (((long long int)t->t0.ayz * (r->matrix.azz)) >> 16);
		r0->matrix.azx = (((long long int)t->t0.azx * (r->matrix.axx)) >> 16) +
		                 (((long long int)t->t0.azy * (r->matrix.ayx)) >> 16) +
		                 (((long long int)t->t0.azz * (r->matrix.azx)) >> 16);
		r0->matrix.azy = (((long long int)t->t0.azx * (r->matrix.axy)) >> 16) +
		                 (((long long int)t->t0.azy * (r->matrix.ayy)) >> 16) +
		                 (((long long int)t->t0.azz * (r->matrix.azy)) >> 16);
		r0->matrix.azz = (((long long int)t->t0.azx * (r->matrix.axz)) >> 16) +
		                 (((long long int)t->t0.azy * (r->matrix.ayz)) >> 16) +
		                 (((long long int)t->t0.azz * (r->matrix.azz)) >> 16);
		// need to actually check this, but ok for now
		r0->matrix.is_identity = 0;
		r0->matrix.is_affine = t->t0.is_affine & r->matrix.is_affine;

		r1->matrix.axx = (((long long int)t->t1.axx * (r->matrix.axx)) >> 16) +
		                 (((long long int)t->t1.axy * (r->matrix.ayx)) >> 16) +
		                 (((long long int)t->t1.axz * (r->matrix.azx)) >> 16);
		r1->matrix.axy = (((long long int)t->t1.axx * (r->matrix.axy)) >> 16) +
		                 (((long long int)t->t1.axy * (r->matrix.ayy)) >> 16) +
		                 (((long long int)t->t1.axz * (r->matrix.azy)) >> 16);
		r1->matrix.axz = (((long long int)t->t1.axx * (r->matrix.axz)) >> 16) +
		                 (((long long int)t->t1.axy * (r->matrix.ayz)) >> 16) +
		                 (((long long int)t->t1.axz * (r->matrix.azz)) >> 16);
		r1->matrix.ayx = (((long long int)t->t1.ayx * (r->matrix.axx)) >> 16) +
		                 (((long long int)t->t1.ayy * (r->matrix.ayx)) >> 16) +
		                 (((long long int)t->t1.ayz * (r->matrix.azx)) >> 16);
		r1->matrix.ayy = (((long long int)t->t1.ayx * (r->matrix.axy)) >> 16) +
		                 (((long long int)t->t1.ayy * (r->matrix.ayy)) >> 16) +
		                 (((long long int)t->t1.ayz * (r->matrix.azy)) >> 16);
		r1->matrix.ayz = (((long long int)t->t1.ayx * (r->matrix.axz)) >> 16) +
		                 (((long long int)t->t1.ayy * (r->matrix.ayz)) >> 16) +
		                 (((long long int)t->t1.ayz * (r->matrix.azz)) >> 16);
		r1->matrix.azx = (((long long int)t->t1.azx * (r->matrix.axx)) >> 16) +
		                 (((long long int)t->t1.azy * (r->matrix.ayx)) >> 16) +
		                 (((long long int)t->t1.azz * (r->matrix.azx)) >> 16);
		r1->matrix.azy = (((long long int)t->t1.azx * (r->matrix.axy)) >> 16) +
		                 (((long long int)t->t1.azy * (r->matrix.ayy)) >> 16) +
		                 (((long long int)t->t1.azz * (r->matrix.azy)) >> 16);
		r1->matrix.azz = (((long long int)t->t1.azx * (r->matrix.axz)) >> 16) +
		                 (((long long int)t->t1.azy * (r->matrix.ayz)) >> 16) +
		                 (((long long int)t->t1.azz * (r->matrix.azz)) >> 16);
		// need to actually check this, but ok for now
		r1->matrix.is_identity = 0;
		r1->matrix.is_affine = t->t1.is_affine & r->matrix.is_affine;
	}
	if (!enesim_renderer_state_setup(t->r0))
		return EINA_FALSE;
	if (!enesim_renderer_state_setup(t->r1))
		return EINA_FALSE;

	r->span = ENESIM_RENDERER_SPAN_DRAW(_span_general);
	r->changed = EINA_FALSE;
	return EINA_TRUE;
}

static void _state_cleanup(Enesim_Renderer *r)
{
	Transition *trans = (Transition *)r;

	enesim_renderer_state_cleanup(trans->r0);
	enesim_renderer_state_cleanup(trans->r1);
	if (!r->matrix.is_identity)
	{
		Enesim_Renderer  *r0 = trans->r0, *r1 = trans->r1;

		r0->matrix.axx = trans->t0.axx;  r0->matrix.axy = trans->t0.axy;  r0->matrix.axz = trans->t0.axz;
		r0->matrix.ayx = trans->t0.ayx;  r0->matrix.ayy = trans->t0.ayy;  r0->matrix.ayz = trans->t0.ayz;
		r0->matrix.azx = trans->t0.azx;  r0->matrix.azy = trans->t0.azy;  r0->matrix.azz = trans->t0.azz;
		r0->matrix.is_identity = trans->t0.is_identity;
		r0->matrix.is_affine = trans->t0.is_affine;

		r1->matrix.axx = trans->t1.axx;  r1->matrix.axy = trans->t1.axy;  r1->matrix.axz = trans->t1.axz;
		r1->matrix.ayx = trans->t1.ayx;  r1->matrix.ayy = trans->t1.ayy;  r1->matrix.ayz = trans->t1.ayz;
		r1->matrix.azx = trans->t1.azx;  r1->matrix.azy = trans->t1.azy;  r1->matrix.azz = trans->t1.azz;
		r1->matrix.is_identity = trans->t1.is_identity;
		r1->matrix.is_affine = trans->t1.is_affine;
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
EAPI Enesim_Renderer * enesim_renderer_transition_new(void)
{
	Enesim_Renderer *r;
	Transition *t;

	t = calloc(1, sizeof(Transition));
	if (!t)
		return NULL;
	r = (Enesim_Renderer *) t;
	r->matrix.axx = r->matrix.ayy = r->matrix.azz = 65536;
	r->matrix.is_identity = r->matrix.is_affine = 1;
	r->state_setup = ENESIM_RENDERER_STATE_SETUP(_state_setup);
	r->state_cleanup = ENESIM_RENDERER_STATE_CLEANUP(_state_cleanup);
	r->free = ENESIM_RENDERER_DELETE(_free);
	r->changed = EINA_TRUE;
	//   if (!transition_setup_state(p, 0)) { free(t); return NULL; }
	return r;
}

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
//	t->changed = EINA_TRUE;
}

EAPI void enesim_renderer_transition_source_set(Enesim_Renderer *r, Enesim_Renderer *r0)
{
	Transition *t;

	if (r0 == r)
		return;
	t = (Transition *)r;
	if (t->r0 == r0)
		return;
	t->r0 = r0;
//	r->changed = EINA_TRUE;
}

EAPI void enesim_renderer_transition_target_set(Enesim_Renderer *r, Enesim_Renderer *r1)
{
	Transition *t;

	if (r1 == r)
		return;
	t = (Transition *)r;
	if (t->r1 == r1)
		return;
	t->r1 = r1;
//	r->changed = EINA_TRUE;
}

EAPI void enesim_renderer_transition_offset_set(Enesim_Renderer *r, int x, int y)
{
	Transition *t = (Transition *)r;

	if ((t->offset.x == x) && (t->offset.y == y))
		return;
	t->offset.x = x;
	t->offset.y = y;
//	r->changed = EINA_TRUE;
}
