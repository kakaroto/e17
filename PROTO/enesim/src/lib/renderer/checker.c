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
/* TODO instead of colors we can also use renderers? for that we'll need the
 * get_pixel() on each renderer
 */
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
typedef struct _Checker
{
	Enesim_Renderer base;
	uint32_t color1;
	uint32_t color2;
	int sw;
	int sh;
} Checker;

static void _span(Enesim_Renderer *r, int x, int y, unsigned int len, uint32_t *dst)
{
	Checker *s = (Checker *)r;
	uint32_t color[2];

	if (abs(((y - s->base.oy) / s->sh) % 2) == 0)
	{
		color[0] = s->color1;
		color[1] = s->color2;
	}
	else
	{
		color[1] = s->color1;
		color[0] = s->color2;
	}
	x -= s->base.ox;
	while (len--)
	{
		*dst = color[abs((x / s->sw) % 2)];
		dst++;
		x++;
	}
}

static void _state_cleanup(Checker *s)
{
}

static Eina_Bool _state_setup(Checker *s)
{
	return EINA_TRUE;
}

static void _free(Checker *s)
{

}
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
EAPI Enesim_Renderer * enesim_renderer_checker_new(void)
{
	Enesim_Renderer *r;
	Checker *s;

	s = calloc(1, sizeof(Checker));
	r = (Enesim_Renderer *)s;

	r->free = ENESIM_RENDERER_DELETE(_free);
	r->span = ENESIM_RENDERER_SPAN_DRAW(_span);
	r->state_cleanup = ENESIM_RENDERER_STATE_CLEANUP(_state_cleanup);
	r->state_setup = ENESIM_RENDERER_STATE_SETUP(_state_setup);

	return r;
}

EAPI void enesim_renderer_checker_color1_set(Enesim_Renderer *r, uint32_t color)
{
	Checker *s = (Checker *)r;

	s->color1 = color;
}

EAPI void enesim_renderer_checker_color2_set(Enesim_Renderer *r, uint32_t color)
{
	Checker *s = (Checker *)r;

	s->color2 = color;
}

EAPI void enesim_renderer_checker_size_set(Enesim_Renderer *r, int w, int h)
{
	Checker *s = (Checker *)r;

	s->sw = w;
	s->sh = h;
}
