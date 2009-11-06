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
/* TODO The callbacks should be registered only when the object is attached to a canvas */
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
#define PRIVATE(d) ((Eon_Shape_Square_Private *)((Eon_Shape_Square *)(d))->private)
#define EON_TYPE_SHAPE_SQUARE "Eon_Shape_Square"
struct _Eon_Shape_Square_Private
{
	Eon_Coord x, y, w, h;
};

/* Just informs that the x.final property has to be recalculated */
static void _x_inform(const Ekeko_Object *o, Ekeko_Event *e, void *data)
{
	Eon_Shape_Square_Private *prv = PRIVATE(data);
	Ekeko_Value v;

	printf("[Eon_Shape_Square] Informing X change\n");
	eon_value_coord_from(&v, &prv->x);
	ekeko_object_property_value_set((Ekeko_Object *)data, "x", &v);
}

/* Just informs that the y.final property has to be recalculated */
static void _y_inform(const Ekeko_Object *o, Ekeko_Event *e, void *data)
{
	Eon_Shape_Square_Private *prv = PRIVATE(data);
	Ekeko_Value v;

	printf("[Eon_Shape_Square] Informing Y change\n");
	eon_value_coord_from(&v, &prv->y);
	ekeko_object_property_value_set((Ekeko_Object *)data, "y", &v);
}

/* Just informs that the w.final property has to be recalculated */
static void _w_inform(const Ekeko_Object *o, Ekeko_Event *e, void *data)
{
	Eon_Shape_Square_Private *prv = PRIVATE(data);
	Ekeko_Value v;

	printf("[Eon_Shape_Square] Informing W change\n");
	eon_value_coord_from(&v, &prv->w);
	ekeko_object_property_value_set((Ekeko_Object *)data, "w", &v);
}

/* Just informs that the h.final property has to be recalculated */
static void _h_inform(const Ekeko_Object *o, Ekeko_Event *e, void *data)
{
	Eon_Shape_Square_Private *prv = PRIVATE(data);
	Ekeko_Value v;

	printf("[Eon_Shape_Square] Informing H change\n");
	eon_value_coord_from(&v, &prv->h);
	ekeko_object_property_value_set((Ekeko_Object *)data, "h", &v);
}

/* Called whenever the x property changes */
static void _x_change(const Ekeko_Object *o, Ekeko_Event *e, void *data)
{
	Ekeko_Event_Mutation *em = (Ekeko_Event_Mutation *)e;
	Eon_Shape_Square *s = (Eon_Shape_Square *)o;
	Eon_Shape_Square_Private *prv = PRIVATE(o);
	Eon_Canvas *c;
	Eon_Coord x, w;

	if (em->state == EVENT_MUTATION_STATE_POST)
		return;

	if (!(c = eon_paint_canvas_get((Eon_Paint *)o)))
		return;

	eon_canvas_x_get(c, &x);
	eon_canvas_w_get(c, &w);
	eon_coord_change(o, &prv->x, em->curr->value.pointer_value,
			em->prev->value.pointer_value, x.final, w.final, c,
			EON_CANVAS_X_CHANGED, EON_CANVAS_W_CHANGED,
			_x_inform);
}

/* Called whenever the y property changes */
static void _y_change(const Ekeko_Object *o, Ekeko_Event *e, void *data)
{
	Ekeko_Event_Mutation *em = (Ekeko_Event_Mutation *)e;
	Eon_Shape_Square *s = (Eon_Shape_Square *)o;
	Eon_Shape_Square_Private *prv = PRIVATE(o);
	Eon_Canvas *c;
	Eon_Coord y, h;

	if (em->state == EVENT_MUTATION_STATE_POST)
		return;

	if (!(c = eon_paint_canvas_get((Eon_Paint *)o)))
		return;

	eon_canvas_y_get(c, &y);
	eon_canvas_h_get(c, &h);
	eon_coord_change(o, &prv->y, em->curr->value.pointer_value,
			em->prev->value.pointer_value, y.final, h.final, c,
			EON_CANVAS_Y_CHANGED, EON_CANVAS_H_CHANGED,
			_y_inform);
}

/* Called whenever the w property changes */
static void _w_change(const Ekeko_Object *o, Ekeko_Event *e, void *data)
{
	Ekeko_Event_Mutation *em = (Ekeko_Event_Mutation *)e;
	Eon_Shape_Square *s = (Eon_Shape_Square *)o;
	Eon_Shape_Square_Private *prv = PRIVATE(o);
	Eon_Canvas *c;
	Eon_Coord w;

	if (em->state == EVENT_MUTATION_STATE_POST)
		return;

	if (!(c = eon_paint_canvas_get((Eon_Paint *)o)))
		return;

	eon_canvas_w_get(c, &w);
	eon_coord_length_change(o, &prv->w, em->curr->value.pointer_value,
			em->prev->value.pointer_value, w.final, c,
			EON_CANVAS_W_CHANGED, _w_inform);
}

/* Called whenever the h property changes */
static void _h_change(const Ekeko_Object *o, Ekeko_Event *e, void *data)
{
	Ekeko_Event_Mutation *em = (Ekeko_Event_Mutation *)e;
	Eon_Shape_Square *s = (Eon_Shape_Square *)o;
	Eon_Shape_Square_Private *prv = PRIVATE(o);
	Eon_Canvas *c;
	Eon_Coord h;

	if (em->state == EVENT_MUTATION_STATE_POST)
		return;

	if (!(c = eon_paint_canvas_get((Eon_Paint *)o)))
		return;

	eon_canvas_h_get(c, &h);
	eon_coord_length_change(o, &prv->h, em->curr->value.pointer_value,
			em->prev->value.pointer_value, h.final, c,
			EON_CANVAS_H_CHANGED, _h_inform);
}

/* whenever the parent changes, we should set the new real geometry
 * in case some coord is relative
 */
static void _parent_set(const Ekeko_Object *o, Ekeko_Event *e, void *data)
{
	Ekeko_Event_Mutation *em = (Ekeko_Event_Mutation *)e;
	Ekeko_Object *p = (Ekeko_Object *)em->related;
	Eon_Coord x, y, w, h, zero;
	Eon_Shape_Square_Private *prv = PRIVATE(o);

	/* FIXME, fix this, the parent can be a canvas or any other
	 * square type (paint or square)
	 */
	if (!ekeko_type_instance_is_of(p, "canvas"))
		return;

	eon_canvas_x_get(p, &x);
	eon_canvas_y_get(p, &y);
	eon_canvas_w_get(p, &w);
	eon_canvas_h_get(p, &h);

	zero.type = EON_COORD_ABSOLUTE;
	zero.value = 0;
	zero.final = 0;

	printf("Setting the new parent\n");
	eon_coord_length_change(o, &prv->h, &prv->h, &zero, h.final, p,
			EON_CANVAS_H_CHANGED, _h_inform);
	eon_coord_length_change(o, &prv->w, &prv->w, &zero, w.final, p,
			EON_CANVAS_W_CHANGED, _w_inform);
	eon_coord_change(o, &prv->y, &prv->y, &zero, y.final, h.final, p,
			EON_CANVAS_Y_CHANGED, EON_CANVAS_H_CHANGED, _y_inform);
	eon_coord_change(o, &prv->x, &prv->x, &zero, x.final, w.final, p,
			EON_CANVAS_X_CHANGED, EON_CANVAS_W_CHANGED, _x_inform);
}

static void _ctor(Ekeko_Object *o)
{
	Eon_Shape_Square *s;
	Eon_Shape_Square_Private *prv;

	s = (Eon_Shape_Square *)o;
	s->private = prv = ekeko_type_instance_private_get(eon_shape_square_type_get(), o);
	ekeko_event_listener_add(o, EKEKO_EVENT_OBJECT_APPEND, _parent_set, EINA_FALSE, NULL);
	ekeko_event_listener_add(o, EON_SHAPE_SQUARE_X_CHANGED, _x_change, EINA_FALSE, NULL);
	ekeko_event_listener_add(o, EON_SHAPE_SQUARE_Y_CHANGED, _y_change, EINA_FALSE, NULL);
	ekeko_event_listener_add(o, EON_SHAPE_SQUARE_W_CHANGED, _w_change, EINA_FALSE, NULL);
	ekeko_event_listener_add(o, EON_SHAPE_SQUARE_H_CHANGED, _h_change, EINA_FALSE, NULL);
}

static void _dtor(void *rect)
{

}

/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
void eon_shape_square_style_coords_get(Eon_Shape_Square *s, Eon_Paint *p, int *x, int *y,
		int *w, int *h)
{
	Eon_Coord px, py, pw, ph;
	Eina_Rectangle geom;
	Eon_Paint *sp = (Eon_Paint *)s;

	/* FIXME when a paint has relative coordinates and the parent is not
	 * renderable but another style what to do?
	 */
	/* setup the renderer correctly */
	if (eon_paint_coordspace_get(sp) == EON_COORDSPACE_OBJECT)
	{
		eon_paint_geometry_get(p, &geom);
	}
	else
	{
		Ekeko_Renderable *r;

		/* FIXME we should get the topmost canvas units not the parent
		 * canvas
		 */
		r = (Ekeko_Renderable *)eon_paint_canvas_topmost_get(p);
		ekeko_renderable_geometry_get(r, &geom);
	}
	eon_shape_square_coords_get(s, &px, &py, &pw, &ph);

	if (x) eon_coord_calculate(&px, geom.x, geom.w, x);
	if (y) eon_coord_calculate(&py, geom.y, geom.h, y);
	if (w) eon_coord_length_calculate(&pw, geom.w, w);
	if (h) eon_coord_length_calculate(&ph, geom.h, h);
}
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
Ekeko_Property_Id EON_SHAPE_SQUARE_X;
Ekeko_Property_Id EON_SHAPE_SQUARE_Y;
Ekeko_Property_Id EON_SHAPE_SQUARE_W;
Ekeko_Property_Id EON_SHAPE_SQUARE_H;

EAPI Ekeko_Type *eon_shape_square_type_get(void)
{
	static Ekeko_Type *type = NULL;

	if (!type)
	{
		type = ekeko_type_new(EON_TYPE_SHAPE_SQUARE, sizeof(Eon_Shape_Square),
				sizeof(Eon_Shape_Square_Private), eon_shape_type_get(),
				_ctor, _dtor, NULL);
		EON_SHAPE_SQUARE_X = EKEKO_TYPE_PROP_SINGLE_ADD(type, "x", EON_PROPERTY_COORD, OFFSET(Eon_Shape_Square_Private, x));
		EON_SHAPE_SQUARE_Y = EKEKO_TYPE_PROP_SINGLE_ADD(type, "y", EON_PROPERTY_COORD, OFFSET(Eon_Shape_Square_Private, y));
		EON_SHAPE_SQUARE_W = EKEKO_TYPE_PROP_SINGLE_ADD(type, "w", EON_PROPERTY_COORD, OFFSET(Eon_Shape_Square_Private, w));
		EON_SHAPE_SQUARE_H = EKEKO_TYPE_PROP_SINGLE_ADD(type, "h", EON_PROPERTY_COORD, OFFSET(Eon_Shape_Square_Private, h));
	}

	return type;
}

EAPI void eon_shape_square_x_rel_set(Eon_Shape_Square *s, int x)
{
	Eon_Coord coord;
	Ekeko_Value v;

	eon_coord_set(&coord, x, EON_COORD_RELATIVE);
	eon_value_coord_from(&v, &coord);
	ekeko_object_property_value_set((Ekeko_Object *)s, "x", &v);
}

EAPI void eon_shape_square_x_set(Eon_Shape_Square *s, int x)
{
	Eon_Coord coord;
	Ekeko_Value v;

	eon_coord_set(&coord, x, EON_COORD_ABSOLUTE);
	eon_value_coord_from(&v, &coord);
	ekeko_object_property_value_set((Ekeko_Object *)s, "x", &v);
}

EAPI void eon_shape_square_y_set(Eon_Shape_Square *s, int y)
{
	Eon_Coord coord;
	Ekeko_Value v;

	eon_coord_set(&coord, y, EON_COORD_ABSOLUTE);
	eon_value_coord_from(&v, &coord);
	ekeko_object_property_value_set((Ekeko_Object *)s, "y", &v);
}

EAPI void eon_shape_square_y_rel_set(Eon_Shape_Square *s, int y)
{
	Eon_Coord coord;
	Ekeko_Value v;

	eon_coord_set(&coord, y, EON_COORD_RELATIVE);
	eon_value_coord_from(&v, &coord);
	ekeko_object_property_value_set((Ekeko_Object *)s, "y", &v);
}

EAPI void eon_shape_square_w_set(Eon_Shape_Square *s, int w)
{
	Eon_Coord coord;
	Ekeko_Value v;

	eon_coord_set(&coord, w, EON_COORD_ABSOLUTE);
	eon_value_coord_from(&v, &coord);
	ekeko_object_property_value_set((Ekeko_Object *)s, "w", &v);
}

EAPI void eon_shape_square_w_rel_set(Eon_Shape_Square *s, int w)
{
	Eon_Coord coord;
	Ekeko_Value v;

	eon_coord_set(&coord, w, EON_COORD_RELATIVE);
	eon_value_coord_from(&v, &coord);
	ekeko_object_property_value_set((Ekeko_Object *)s, "w", &v);
}

EAPI void eon_shape_square_h_set(Eon_Shape_Square *s, int h)
{
	Eon_Coord coord;
	Ekeko_Value v;

	eon_coord_set(&coord, h, EON_COORD_ABSOLUTE);
	eon_value_coord_from(&v, &coord);
	ekeko_object_property_value_set((Ekeko_Object *)s, "h", &v);
}

EAPI void eon_shape_square_h_rel_set(Eon_Shape_Square *s, int h)
{
	Eon_Coord coord;
	Ekeko_Value v;

	eon_coord_set(&coord, h, EON_COORD_RELATIVE);
	eon_value_coord_from(&v, &coord);
	ekeko_object_property_value_set((Ekeko_Object *)s, "h", &v);
}

EAPI void eon_shape_square_coords_get(Eon_Shape_Square *s, Eon_Coord *x,
		Eon_Coord *y, Eon_Coord *w, Eon_Coord *h)
{
	Eon_Shape_Square_Private *prv = PRIVATE(s);

	if (x) *x = prv->x;
	if (y) *y = prv->y;
	if (w) *w = prv->w;
	if (h) *h = prv->h;
}
