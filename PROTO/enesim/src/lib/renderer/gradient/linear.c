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
typedef struct _Linear
{
	Enesim_Renderer_Gradient base;
	float x0, x1, y0, y1;
	Eina_F16p16 fx0, fx1, fy0, fy1;
	Eina_F16p16 ayx, ayy;
} Linear;

static void _argb8888_pad_span(Enesim_Renderer *r, int x, int y,
		unsigned int len, uint32_t *dst)
{
	Linear *l = (Linear *)r;
	Enesim_Renderer_Gradient *g = (Enesim_Renderer_Gradient *)l;
	uint32_t *end = dst + len;
	Eina_F16p16 yy;
	Eina_F16p16 fx, fy;

	fx = eina_f16p16_int_from(x);
	fy = eina_f16p16_int_from(y);
	yy = eina_f16p16_mul(l->ayx, (fx - l->fx0 + 32768)) +
			eina_f16p16_mul(l->ayy, (fy - l->fy0 + 32768)) - 32768;
	//printf("yy = %d\n", eina_f16p16_int_to(yy));
	while (dst < end)
	{
		int sy;
		uint32_t p0;

		sy = eina_f16p16_int_to(yy);
		//printf("%d\n", sy);
		if (sy < 0)
		{
			p0 = g->src[0];
		}
		else if (sy >= g->slen - 1)
		{
			p0 = g->src[g->slen - 1];
		}
		else
		{
			uint16_t a;

			a = eina_f16p16_fracc_get(yy) >> 8;
			//printf("CALLED %d %08x %08x %d %08x\n", sy, g->src[sy + 1], g->src[sy], a, *g->src);
			p0 = argb8888_interp_256(1 + a, g->src[sy + 1], g->src[sy]);
		}
		*dst++ = p0;
		yy += l->ayx;
	}
	/* FIXME is there some mmx bug there? the interp_256 already calls this
	 * but the float support is fucked up
	 */
#ifdef EFL_HAVE_MMX
	_mm_empty();
#endif
}

static void _state_cleanup(Enesim_Renderer *r)
{

}

static Eina_Bool _state_setup(Enesim_Renderer *r)
{
	Linear *l = (Linear *)r;
	Eina_F16p16 x0, x1, y0, y1;
	Eina_F16p16 f;

	f = eina_f16p16_mul(r->matrix.values.xx, r->matrix.values.yy) -
			eina_f16p16_mul(r->matrix.values.xy, r->matrix.values.yx);
	/* TODO check that (xx * yy) - (xy * yx) < epsilon */
	f = ((int64_t)f << 16) / 65536;
	x0 = eina_f16p16_float_from(l->x0);
	x1 = eina_f16p16_float_from(l->x1);
	y0 = eina_f16p16_float_from(l->y0);
	y1 = eina_f16p16_float_from(l->y1);

	/* apply the transformation on each point */
	l->fx0 = eina_f16p16_mul(r->matrix.values.yy, x0) -
			eina_f16p16_mul(eina_f16p16_mul(r->matrix.values.xy, y0), f) -
			r->matrix.values.xz;
	l->fx1 = eina_f16p16_mul(r->matrix.values.yy, x1) -
			eina_f16p16_mul(eina_f16p16_mul(r->matrix.values.xy, y1), f) -
			r->matrix.values.xz;
	l->fy0 = eina_f16p16_mul(r->matrix.values.yx, x0) -
			eina_f16p16_mul(eina_f16p16_mul(r->matrix.values.xx, y0), f) -
			r->matrix.values.yz;
	l->fy1 = eina_f16p16_mul(r->matrix.values.yx, x1) -
			eina_f16p16_mul(eina_f16p16_mul(r->matrix.values.xx, y1), f) -
			r->matrix.values.yz;
#if 0
	printf("x0 %d y0 %d x1 %d y1 %d %d\n",
		eina_f16p16_int_to(l->fx0), 
		eina_f16p16_int_to(l->fy0), 
		eina_f16p16_int_to(l->fx1), 
		eina_f16p16_int_to(l->fy1),
		eina_f16p16_int_to(f));
#endif
	/* get the length of the transformed points */
	x0 = l->fx1 - l->fx0;
	y0 = l->fy1 - l->fy0;
	/* we need to use floats because of the limitation of 16.16 values */
	f = eina_f16p16_float_from(hypot(eina_f16p16_float_to(x0), eina_f16p16_float_to(y0)));
	f += 32768;
	//printf("len = %d %d\n", eina_f16p16_int_to(f), f);
	l->ayx = ((int64_t)x0 << 16) / f;
	l->ayy = ((int64_t)y0 << 16) / f;
	/* TODO check that the difference between x0 - x1 and y0 - y1 is
	 * < tolerance
	 */
	enesim_renderer_gradient_state_setup(r, eina_f16p16_int_to(f));
	r->span = ENESIM_RENDERER_SPAN_DRAW(_argb8888_pad_span);
	return EINA_TRUE;
}
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
EAPI Enesim_Renderer * enesim_renderer_gradient_linear_new(void)
{
	Linear *l;
	Enesim_Renderer *r;

	l = calloc(1, sizeof(Linear));

	r = (Enesim_Renderer *)l;
	enesim_renderer_gradient_init(r);
	r->state_setup = ENESIM_RENDERER_STATE_SETUP(_state_setup);

	return r;
}

EAPI void enesim_renderer_gradient_linear_pos_set(Enesim_Renderer *r, float x0,
		float y0, float x1, float y1)
{
	Linear *l = (Linear *)r;

	l->x0 = x0;
	l->x1 = x1;
	l->y0 = y0;
	l->y1 = y1;
}
