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
typedef struct _Compound
{
	Enesim_Renderer base;
	Eina_List *layers;
} Compound;

typedef struct _Layer
{
	Enesim_Compositor_Span span;
	Enesim_Renderer *r;
} Layer;

static void _span_identity(Enesim_Renderer *r, int x, int y, unsigned int len, uint32_t *dst)
{
	Compound *c = (Compound *)r;
	Eina_List *ll;
	uint32_t *tmp = alloca(sizeof(uint32_t) * len);

	for (ll = c->layers; ll; ll = eina_list_next(ll))
	{
		Layer *l;

		l = eina_list_data_get(ll);
		if (!l->span)
		{
			enesim_renderer_span_fill(l->r, x, y, len, dst);
		}
		else
		{
			enesim_renderer_span_fill(l->r, x, y, len, tmp);
			l->span(dst, len, tmp, 0, NULL);
		}
	}
}

static Eina_Bool _state_setup(Enesim_Renderer *r)
{
	Compound *c = (Compound *)r;
	Eina_List *ll;

	/* setup every layer */
	for (ll = c->layers; ll; ll = eina_list_next(ll))
	{
		Layer *l = eina_list_data_get(ll);

		enesim_renderer_state_setup(l->r);
	}

	return EINA_TRUE;
}
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
/**
 * Creates a compound renderer
 * @return The new renderer
 */
EAPI Enesim_Renderer * enesim_renderer_compound_new(void)
{
	Compound *c;
	Enesim_Renderer *r;

	c = calloc(1, sizeof(Compound));

	r = (Enesim_Renderer *)c;
	enesim_renderer_init(r);
	r->state_setup = ENESIM_RENDERER_STATE_SETUP(_state_setup);
	r->span = ENESIM_RENDERER_SPAN_DRAW(_span_identity);

	return r;
}
/**
 * Adds a layer
 * @param[in] r The compound renderer
 * @param[in] rend The renderer for the new layer
 * @param[in] rop The raster operation for the new layer
 */
EAPI void enesim_renderer_compound_layer_add(Enesim_Renderer *r,
		Enesim_Renderer *rend, Enesim_Rop rop)
{
	Compound *c = (Compound *)r;
	Layer *l;
	Enesim_Format fmt = ENESIM_FORMAT_ARGB8888;
	/* FIXME fix the resulting format */

	l = malloc(sizeof(Layer));
	l->r = rend;
	/* FIXME what about the surface formats here? */
	if (rop != ENESIM_FILL)
		l->span = enesim_compositor_span_get(rop, &fmt, ENESIM_FORMAT_ARGB8888,
				ENESIM_COLOR_FULL, ENESIM_FORMAT_NONE);
	else
		l->span = NULL;
	c->layers = eina_list_append(c->layers, l);
}

/**
 * Clears up all the layers
 * @param[in] r The compound renderer
 */
EAPI void enesim_renderer_compound_clear(Enesim_Renderer *r)
{
	Layer *layer;
	Eina_List *list;
	Eina_List *l;
	Eina_List *l_next;

	EINA_LIST_FOREACH_SAFE(list, l, l_next, layer)
	{
		free(layer);
		list = eina_list_remove_list(list, l);
	}
}
