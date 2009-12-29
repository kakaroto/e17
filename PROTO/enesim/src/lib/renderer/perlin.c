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
typedef struct _Perlin
{
	Enesim_Renderer base;
	struct {
		float val;
		Eina_F16p16 *coeff;
	} xfreq, yfreq, ampl;
	float persistence;
	int octaves;
} Perlin;

static void _argb8888_span_affine(Enesim_Renderer *r, int x, int y, unsigned int len, uint32_t *dst)
{
	Perlin *p = (Perlin *)r;
	uint32_t *end = dst + len;
	Eina_F16p16 xx, yy;

	/* end of state setup */
	renderer_affine_setup(r, x, y, &xx, &yy);
	while (dst < end)
	{
		Eina_F16p16 per;
		uint32_t p0;
		uint8_t c;

		per = enesim_perlin_get(xx, yy, p->octaves, p->xfreq.coeff,
				p->yfreq.coeff, p->ampl.coeff);
		c = ((per & 0x1ffff) >> 9);
		/* FIXME the dispmap uses a and b for x and y displacement, we must
		 * define a better way for that, so this renderer can actually build
		 * displacement maps useful for dispmap renderer
		 */
		*dst++ = 0xff << 24 | c << 16 | c << 8 | c;
		xx += r->matrix.values.xx;
		yy += r->matrix.values.yx;
	}

}

static void _argb8888_span_identity(Enesim_Renderer *r, int x, int y, unsigned int len, uint32_t *dst)
{
	Perlin *p = (Perlin *)r;
	uint32_t *end = dst + len;
	Eina_F16p16 xx, yy;
	Eina_F16p16 *freq, *ampl, per;

	/* end of state setup */
	xx = eina_f16p16_int_from(x);
	yy = eina_f16p16_int_from(y);
	while (dst < end)
	{
		Eina_F16p16 per;
		uint32_t p0;
		uint8_t c;

		per = enesim_perlin_get(xx, yy, p->octaves, p->xfreq.coeff,
				p->yfreq.coeff, p->ampl.coeff);
		c = ((per & 0x1ffff) >> 9);
		/* FIXME the dispmap uses a and b for x and y displacement, we must
		 * define a better way for that, so this renderer can actually build
		 * displacement maps useful for dispmap renderer
		 */
		*dst++ = 0xff << 24 | c << 16 | c << 8 | c;
		xx += 65536;
	}
}

static void _span_affine(Enesim_Renderer *r, int x, int y, unsigned int len, uint8_t *dst)
{
	Perlin *p = (Perlin *)r;

}

static void _state_cleanup(Enesim_Renderer *r)
{

}

static Eina_Bool _state_setup(Enesim_Renderer *r)
{
	Perlin *p = (Perlin *)r;

	if (p->xfreq.coeff)
	{
		free(p->xfreq.coeff);
		free(p->yfreq.coeff);
		free(p->ampl.coeff);
	}
	p->xfreq.coeff = malloc((sizeof(Eina_F16p16) * p->octaves));
	p->yfreq.coeff = malloc((sizeof(Eina_F16p16) * p->octaves));
	p->ampl.coeff = malloc((sizeof(Eina_F16p16) * p->octaves));
	enesim_perlin_coeff_set(p->octaves, p->persistence, p->xfreq.val,
		p->yfreq.val, p->ampl.val, p->xfreq.coeff, p->yfreq.coeff,
		p->ampl.coeff);

	if (r->matrix.type == ENESIM_MATRIX_IDENTITY)
		r->span = ENESIM_RENDERER_SPAN_DRAW(_argb8888_span_identity);
	else if (r->matrix.type == ENESIM_MATRIX_AFFINE)
		r->span = ENESIM_RENDERER_SPAN_DRAW(_argb8888_span_affine);
	return EINA_TRUE;
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
 * To be documented
 * FIXME: To be fixed
 */
EAPI Enesim_Renderer * enesim_renderer_perlin_new(void)
{
	Enesim_Renderer *r;
	Perlin *p;

	p = calloc(1, sizeof(Perlin));
	p->xfreq.val = 1; /* 1 2 4 8 ... */
	p->yfreq.val = 1; /* 1 2 4 8 ... */
	p->ampl.val = 1; /* p p2 p3 p4 ... */
	r = (Enesim_Renderer *)p;

	enesim_renderer_init(r);
	r->free = ENESIM_RENDERER_DELETE(_free);
	r->state_cleanup = ENESIM_RENDERER_STATE_CLEANUP(_state_cleanup);
	r->state_setup = ENESIM_RENDERER_STATE_SETUP(_state_setup);

	return r;
}
/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void enesim_renderer_perlin_octaves_set(Enesim_Renderer *r, unsigned int octaves)
{
	Perlin *p = (Perlin *)r;

	p->octaves = octaves;
}
/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void enesim_renderer_perlin_persistence_set(Enesim_Renderer *r, float persistence)
{
	Perlin *p = (Perlin *)r;

	p->persistence = persistence;
}
/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void enesim_renderer_perlin_amplitude_set(Enesim_Renderer *r, float ampl)
{
	Perlin *p = (Perlin *)r;

	p->ampl.val = ampl;
}
/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void enesim_renderer_perlin_xfrequency_set(Enesim_Renderer *r, float freq)
{
	Perlin *p = (Perlin *)r;

	p->xfreq.val = freq;
}
/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void enesim_renderer_perlin_yfrequency_set(Enesim_Renderer *r, float freq)
{
	Perlin *p = (Perlin *)r;

	p->yfreq.val = freq;
}
