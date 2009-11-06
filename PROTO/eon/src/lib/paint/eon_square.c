/* EON - Canvas and Toolkit library
 * Copyright (C) 2008-2009 Jorge Luis Zapata
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
#include "Eon.h"
#include "eon_private.h"
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
struct _Eon_Square
{
	Eon_Coord x, y, w, h;
};

struct _Eon_Square_Setup
{
	const char *xevent;
	const char *yevent;
	const char *wevent;
	const char *hevent;
};

/* Just informs that the x.final property has to be recalculated */
static void _x_inform(const Ekeko_Object *o, Ekeko_Event *e, void *data)
{
	Eon_Square_Private *prv = PRIVATE(data);
	Ekeko_Value v;

	printf("[Eon_Square] Informing X change\n");
	eon_value_coord_from(&v, &prv->x);
	ekeko_object_property_value_set((Ekeko_Object *)data, "x", &v);
}

/* Just informs that the y.final property has to be recalculated */
static void _y_inform(const Ekeko_Object *o, Ekeko_Event *e, void *data)
{
	Eon_Square_Private *prv = PRIVATE(data);
	Ekeko_Value v;

	printf("[Eon_Square] Informing Y change\n");
	eon_value_coord_from(&v, &prv->y);
	ekeko_object_property_value_set((Ekeko_Object *)data, "y", &v);
}

/* Just informs that the w.final property has to be recalculated */
static void _w_inform(const Ekeko_Object *o, Ekeko_Event *e, void *data)
{
	Eon_Square_Private *prv = PRIVATE(data);
	Ekeko_Value v;

	printf("[Eon_Square] Informing W change\n");
	eon_value_coord_from(&v, &prv->w);
	ekeko_object_property_value_set((Ekeko_Object *)data, "w", &v);
}

/* Just informs that the h.final property has to be recalculated */
static void _h_inform(const Ekeko_Object *o, Ekeko_Event *e, void *data)
{
	Eon_Square_Private *prv = PRIVATE(data);
	Ekeko_Value v;

	printf("[Eon_Square] Informing H change\n");
	eon_value_coord_from(&v, &prv->h);
	ekeko_object_property_value_set((Ekeko_Object *)data, "h", &v);
}

/* Called whenever the x property changes */
static void _x_change(const Ekeko_Object *o, Ekeko_Event *e, void *data)
{
	Ekeko_Event_Mutation *em = (Ekeko_Event_Mutation *)e;
	Eon_Square *s = (Eon_Square *)o;
	Eon_Square_Private *prv = PRIVATE(o);
	Ekeko_Object *parent;
	Eon_Coord x, w;

	if (em->state == EVENT_MUTATION_STATE_POST)
		return;

	if (!(parent = ekeko_object_parent_get(o)))
		return;

	eon_canvas_x_get((Eon_Canvas *)parent, &x);
	eon_canvas_w_get((Eon_Canvas *)parent, &w);
	eon_coord_change(o, &prv->x, em->curr->value.pointer_value,
			em->prev->value.pointer_value, x.final, w.final, parent,
			EON_CANVAS_X_CHANGED, EON_CANVAS_W_CHANGED,
			_x_inform);
}

/* Called whenever the y property changes */
static void _y_change(const Ekeko_Object *o, Ekeko_Event *e, void *data)
{
	Ekeko_Event_Mutation *em = (Ekeko_Event_Mutation *)e;
	Eon_Square *s = (Eon_Square *)o;
	Eon_Square_Private *prv = PRIVATE(o);
	Ekeko_Object *parent;
	Eon_Coord y, h;

	if (em->state == EVENT_MUTATION_STATE_POST)
		return;

	if (!(parent = ekeko_object_parent_get(o)))
		return;

	eon_canvas_y_get((Eon_Canvas *)parent, &y);
	eon_canvas_h_get((Eon_Canvas *)parent, &h);
	eon_coord_change(o, &prv->y, em->curr->value.pointer_value,
			em->prev->value.pointer_value, y.final, h.final, parent,
			EON_CANVAS_Y_CHANGED, EON_CANVAS_H_CHANGED,
			_y_inform);
}

/* Called whenever the w property changes */
static void _w_change(const Ekeko_Object *o, Ekeko_Event *e, void *data)
{
	Ekeko_Event_Mutation *em = (Ekeko_Event_Mutation *)e;
	Eon_Square *s = (Eon_Square *)o;
	Eon_Square_Private *prv = PRIVATE(o);
	Ekeko_Object *parent;
	Eon_Coord w;

	if (em->state == EVENT_MUTATION_STATE_POST)
		return;

	if (!(parent = ekeko_object_parent_get(o)))
		return;

	eon_canvas_w_get((Eon_Canvas *)parent, &w);
	eon_coord_length_change(o, &prv->w, em->curr->value.pointer_value,
			em->prev->value.pointer_value, w.final, parent,
			EON_CANVAS_W_CHANGED, _w_inform);
}

/* Called whenever the h property changes */
static void _h_change(const Ekeko_Object *o, Ekeko_Event *e, void *data)
{
	Ekeko_Event_Mutation *em = (Ekeko_Event_Mutation *)e;
	Eon_Square *s = (Eon_Square *)o;
	Eon_Square_Private *prv = PRIVATE(o);
	Ekeko_Object *parent;
	Eon_Coord h;

	if (em->state == EVENT_MUTATION_STATE_POST)
		return;

	if (!(parent = ekeko_object_parent_get(o)))
		return;

	eon_canvas_h_get((Eon_Canvas *)parent, &h);
	eon_coord_length_change(o, &prv->h, em->curr->value.pointer_value,
			em->prev->value.pointer_value, h.final, parent,
			EON_CANVAS_H_CHANGED, _h_inform);
}

/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
void eon_square_setup(Ekeko_Object *o, const char *xchange, const char *ychange
		const char *wchange, const char *hchange)
{
	ekeko_event_listener_add(o, xchange, _x_change, EINA_FALSE, NULL);
	ekeko_event_listener_add(o, ychange, _y_change, EINA_FALSE, NULL);
	ekeko_event_listener_add(o, wchange, _w_change, EINA_FALSE, NULL);
	ekeko_event_listener_add(o, hchange, _h_change, EINA_FALSE, NULL);
}
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
