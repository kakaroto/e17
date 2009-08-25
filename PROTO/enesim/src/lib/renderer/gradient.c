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
typedef struct _Stop
{
	Enesim_Color color;
	float pos;
	/* TODO replace float with Eina_F16p16 */
} Stop;
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
void enesim_renderer_gradient_init(Enesim_Renderer *r)
{
	Enesim_Renderer_Gradient *g = (Enesim_Renderer_Gradient *)r;
	g->stops = NULL;
	enesim_renderer_init(r);
}

void enesim_renderer_gradient_state_setup(Enesim_Renderer *r, int len)
{
	Enesim_Renderer_Gradient *g = (Enesim_Renderer_Gradient *)r;
	Eina_List *tmp;
	Stop *curr, *next;
	Eina_F16p16 xx, inc;
	int end = len;
	uint32_t *dst;

	/* TODO check that we have at least two stops */
	/* TODO check that we have one at 0 and one at 1 */
	curr = eina_list_data_get(g->stops);
	tmp = eina_list_next(g->stops);
	next = eina_list_data_get(tmp);
	/* Check that we dont divide by 0 */
	inc = eina_f16p16_float_from(1.0 / ((next->pos - curr->pos) * len));
	xx = 0;

	g->src = dst = malloc(sizeof(uint32_t) * len);
	memset(g->src, 0xff, len);
	/* FIXME Im not sure if we increment xx by the 1 / ((next - curr) * len) value
	 * as it might not be too accurate
	 */
	while (end--)
	{
		uint16_t off;
		uint32_t p0;

		/* advance the curr and next */
		if (xx >= 65536)
		{
			tmp = eina_list_next(tmp);
			curr = next;
			next = eina_list_data_get(tmp);
			inc = eina_f16p16_float_from(1.0 / ((next->pos - curr->pos) * len));
			xx = 0;
		}
		off = 1 + (eina_f16p16_fracc_get(xx) >> 8);
		p0 = argb8888_interp_256(off, next->color, curr->color);
		*dst++ = p0;
		xx += inc;
	}
	g->slen = len;
}
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
EAPI void enesim_renderer_gradient_stop_add(Enesim_Renderer *r, Enesim_Color c,
		float pos)
{
	Enesim_Renderer_Gradient *g = (Enesim_Renderer_Gradient *)r;
	Stop *s;

	if (pos < 0)
		pos = 0;
	else if (pos > 1)
		pos = 1;

	s = malloc(sizeof(Stop));
	s->color = c;
	s->pos = pos;
	/* if pos == 0.0 set to first */
	if (pos == 0.0)
	{
		g->stops = eina_list_prepend(g->stops, s);
	}
	/* if pos == 1.0 set to last */
	else if (pos == 1.0)
	{
		g->stops = eina_list_append(g->stops, s);
	}
	/* else iterate until pos > prev && pos < next */
	else
	{
		Eina_List *tmp;

		for (tmp = g->stops; tmp; tmp = eina_list_next(tmp))
		{
			Stop *p = eina_list_data_get(tmp);

			if (p->pos > s->pos)
				break;
		}
		g->stops = eina_list_append_relative_list(g->stops, s, tmp);
	}
}

EAPI void enesim_renderer_gradient_clear(Enesim_Renderer *r)
{

}
