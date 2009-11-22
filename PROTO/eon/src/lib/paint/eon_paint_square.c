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
#define PRIVATE(d) ((Eon_Paint_Square_Private *)((Eon_Paint_Square *)(d))->private)
struct _Eon_Paint_Square_Private
{
	Eon_Coord x, y, w, h;
};

static void _geometry_update(Eon_Paint_Square *s)
{
	Eina_Rectangle geom;
	Eon_Coord x, y, w, h;

	eon_paint_square_coords_get(s, &x, &y, &w, &h);
	eina_rectangle_coords_from(&geom, x.final, y.final, w.final,
			h.final);
	eon_paint_geometry_set((Eon_Paint *)s, &geom);
}

/* Just informs that the x.final property has to be recalculated */
static void _x_inform(const Ekeko_Object *o, Ekeko_Event *e, void *data)
{
	Eon_Paint_Square_Private *prv = PRIVATE(data);
	Ekeko_Value v;

	eon_value_coord_from(&v, &prv->x);
	ekeko_object_property_value_set((Ekeko_Object *)data, "x", &v);
}

/* Just informs that the y.final property has to be recalculated */
static void _y_inform(const Ekeko_Object *o, Ekeko_Event *e, void *data)
{
	Eon_Paint_Square_Private *prv = PRIVATE(data);
	Ekeko_Value v;

	eon_value_coord_from(&v, &prv->y);
	ekeko_object_property_value_set((Ekeko_Object *)data, "y", &v);
}

/* Just informs that the w.final property has to be recalculated */
static void _w_inform(const Ekeko_Object *o, Ekeko_Event *e, void *data)
{
	Eon_Paint_Square_Private *prv = PRIVATE(data);
	Ekeko_Value v;

	eon_value_coord_from(&v, &prv->w);
	ekeko_object_property_value_set((Ekeko_Object *)data, "w", &v);
}

/* Just informs that the h.final property has to be recalculated */
static void _h_inform(const Ekeko_Object *o, Ekeko_Event *e, void *data)
{
	Eon_Paint_Square_Private *prv = PRIVATE(data);
	Ekeko_Value v;

	eon_value_coord_from(&v, &prv->h);
	ekeko_object_property_value_set((Ekeko_Object *)data, "h", &v);
}

/* Called whenever the x property changes */
static void _x_change(const Ekeko_Object *o, Ekeko_Event *e, void *data)
{
	Ekeko_Event_Mutation *em = (Ekeko_Event_Mutation *)e;
	Eon_Paint_Square *s = (Eon_Paint_Square *)o;
	Eon_Paint_Square_Private *prv = PRIVATE(o);
	Eon_Layout *l;
	Eon_Coord x, w;

	if (em->state == EVENT_MUTATION_STATE_POST)
		return;

	if (!(l = eon_paint_layout_get((Eon_Paint *)o)))
		return;

	eon_paint_square_x_get(l, &x);
	eon_paint_square_w_get(l, &w);
	eon_coord_change(o, &prv->x, em->curr->value.pointer_value,
			em->prev->value.pointer_value, x.final, w.final, l,
			EON_PAINT_SQUARE_X_CHANGED, EON_PAINT_SQUARE_W_CHANGED,
			_x_inform);
	_geometry_update(s);
}

/* Called whenever the y property changes */
static void _y_change(const Ekeko_Object *o, Ekeko_Event *e, void *data)
{
	Ekeko_Event_Mutation *em = (Ekeko_Event_Mutation *)e;
	Eon_Paint_Square *s = (Eon_Paint_Square *)o;
	Eon_Paint_Square_Private *prv = PRIVATE(o);
	Eon_Layout *l;
	Eon_Coord y, h;

	if (em->state == EVENT_MUTATION_STATE_POST)
		return;

	if (!(l = eon_paint_layout_get((Eon_Paint *)o)))
		return;

	eon_paint_square_y_get(l, &y);
	eon_paint_square_h_get(l, &h);
	eon_coord_change(o, &prv->y, em->curr->value.pointer_value,
			em->prev->value.pointer_value, y.final, h.final, l,
			EON_PAINT_SQUARE_Y_CHANGED, EON_PAINT_SQUARE_H_CHANGED,
			_y_inform);
	_geometry_update(s);
}

/* Called whenever the w property changes */
static void _w_change(const Ekeko_Object *o, Ekeko_Event *e, void *data)
{
	Ekeko_Event_Mutation *em = (Ekeko_Event_Mutation *)e;
	Eon_Paint_Square *s = (Eon_Paint_Square *)o;
	Eon_Paint_Square_Private *prv = PRIVATE(o);
	Eon_Layout *l;
	Eon_Coord w;

	if (em->state == EVENT_MUTATION_STATE_POST)
		return;

	if (!(l = eon_paint_layout_get((Eon_Paint *)o)))
		return;

	eon_paint_square_w_get(l, &w);
	eon_coord_length_change(o, &prv->w, em->curr->value.pointer_value,
			em->prev->value.pointer_value, w.final, l,
			EON_PAINT_SQUARE_W_CHANGED, _w_inform);
	_geometry_update(s);
}

/* Called whenever the h property changes */
static void _h_change(const Ekeko_Object *o, Ekeko_Event *e, void *data)
{
	Ekeko_Event_Mutation *em = (Ekeko_Event_Mutation *)e;
	Eon_Paint_Square *s = (Eon_Paint_Square *)o;
	Eon_Paint_Square_Private *prv = PRIVATE(o);
	Eon_Layout *l;
	Eon_Coord h;

	if (em->state == EVENT_MUTATION_STATE_POST)
		return;

	if (!(l = eon_paint_layout_get((Eon_Paint *)o)))
		return;

	eon_paint_square_h_get(l, &h);
	eon_coord_length_change(o, &prv->h, em->curr->value.pointer_value,
			em->prev->value.pointer_value, h.final, l,
			EON_PAINT_SQUARE_H_CHANGED, _h_inform);
	_geometry_update(s);
}

/* whenever the parent changes, we should set the new real geometry
 * in case some coord is relative
 */
static void _parent_set(const Ekeko_Object *o, Ekeko_Event *e, void *data)
{
	Ekeko_Event_Mutation *em = (Ekeko_Event_Mutation *)e;
	Ekeko_Object *p = (Ekeko_Object *)em->related;
	Eon_Coord x, y, w, h, zero;
	Eon_Paint_Square_Private *prv = PRIVATE(o);
	Eon_Paint_Square *s = (Eon_Paint_Square *)o;

	/* FIXME, fix this, the parent can be a canvas or any other
	 * square type (paint or square)
	 */
	if (!ekeko_type_instance_is_of(p, "layout"))
		return;

	eon_paint_square_x_get(p, &x);
	eon_paint_square_y_get(p, &y);
	eon_paint_square_h_get(p, &h);
	eon_paint_square_w_get(p, &w);

	zero.type = EON_COORD_ABSOLUTE;
	zero.value = 0;
	zero.final = 0;

	eon_coord_length_change(o, &prv->h, &prv->h, &zero, h.final, p,
			EON_PAINT_SQUARE_H_CHANGED, _h_inform);
	eon_coord_length_change(o, &prv->w, &prv->w, &zero, w.final, p,
			EON_PAINT_SQUARE_W_CHANGED, _w_inform);
	eon_coord_change(o, &prv->y, &prv->y, &zero, y.final, h.final, p,
			EON_PAINT_SQUARE_Y_CHANGED, EON_PAINT_SQUARE_H_CHANGED, _y_inform);
	eon_coord_change(o, &prv->x, &prv->x, &zero, x.final, w.final, p,
			EON_PAINT_SQUARE_W_CHANGED, EON_PAINT_SQUARE_W_CHANGED, _x_inform);
	_geometry_update(s);
}


static void _ctor(Ekeko_Object *o)
{
	Eon_Paint_Square *s;
	Eon_Paint_Square_Private *prv;

	s = (Eon_Paint_Square *)o;
	s->private = prv = ekeko_type_instance_private_get(eon_paint_square_type_get(), o);
	//ekeko_event_listener_add(o, EKEKO_EVENT_OBJECT_APPEND, _parent_set, EINA_FALSE, NULL);
	//ekeko_event_listener_add(o, EON_PAINT_SQUARE_X_CHANGED, _x_change, EINA_FALSE, NULL);
	//ekeko_event_listener_add(o, EON_PAINT_SQUARE_Y_CHANGED, _y_change, EINA_FALSE, NULL);
	//ekeko_event_listener_add(o, EON_PAINT_SQUARE_W_CHANGED, _w_change, EINA_FALSE, NULL);
	//ekeko_event_listener_add(o, EON_PAINT_SQUARE_H_CHANGED, _h_change, EINA_FALSE, NULL);
}

static void _dtor(void *rect)
{

}
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
void eon_paint_square_style_coords_get(Eon_Paint_Square *s, Eon_Paint *p, int *x, int *y,
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
		Eon_Layout *l;

		l = eon_paint_layout_get(p);
		eon_paint_boundings_get((Eon_Paint *)l, &geom);
	}
	eon_paint_square_coords_get(s, &px, &py, &pw, &ph);

	if (x) eon_coord_calculate(&px, geom.x, geom.w, x);
	if (y) eon_coord_calculate(&py, geom.y, geom.h, y);
	if (w) eon_coord_length_calculate(&pw, geom.w, w);
	if (h) eon_coord_length_calculate(&ph, geom.h, h);
}

void eon_paint_square_geometry_set(Eon_Paint_Square *p, Eina_Rectangle *geom)
{
	Eon_Paint_Square_Private *prv = PRIVATE(p);

	prv->x.final = geom->x;	
	prv->y.final = geom->y;	
	prv->w.final = geom->w;	
	prv->h.final = geom->h;	
	eon_paint_geometry_set((Eon_Paint *)p, geom);
}
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
Ekeko_Property_Id EON_PAINT_SQUARE_X;
Ekeko_Property_Id EON_PAINT_SQUARE_Y;
Ekeko_Property_Id EON_PAINT_SQUARE_W;
Ekeko_Property_Id EON_PAINT_SQUARE_H;

EAPI Ekeko_Type * eon_paint_square_type_get(void)
{
	static Ekeko_Type *type = NULL;

	if (!type)
	{
		type = ekeko_type_new(EON_TYPE_PAINT_SQUARE, sizeof(Eon_Paint_Square),
				sizeof(Eon_Paint_Square_Private), eon_paint_type_get(),
				_ctor, _dtor, NULL);
		EON_PAINT_SQUARE_X = EKEKO_TYPE_PROP_SINGLE_ADD(type, "x",
				EON_PROPERTY_COORD,
				OFFSET(Eon_Paint_Square_Private, x));
		EON_PAINT_SQUARE_Y = EKEKO_TYPE_PROP_SINGLE_ADD(type, "y",
				EON_PROPERTY_COORD,
				OFFSET(Eon_Paint_Square_Private, y));
		EON_PAINT_SQUARE_W = EKEKO_TYPE_PROP_SINGLE_ADD(type, "w",
				EON_PROPERTY_COORD,
				OFFSET(Eon_Paint_Square_Private, w));
		EON_PAINT_SQUARE_H = EKEKO_TYPE_PROP_SINGLE_ADD(type, "h",
				EON_PROPERTY_COORD,
				OFFSET(Eon_Paint_Square_Private, h));
	}

	return type;
}

EAPI void eon_paint_square_x_get(Eon_Paint_Square *s, Eon_Coord *x)
{
	Eon_Paint_Square_Private *prv;

	prv = PRIVATE(s);
	if (x) *x = prv->x;
}

EAPI void eon_paint_square_x_rel_set(Eon_Paint_Square *s, int x)
{
	Eon_Coord coord;
	Ekeko_Value v;

	eon_coord_relative_set(&coord, x);
	eon_value_coord_from(&v, &coord);
	ekeko_object_property_value_set((Ekeko_Object *)s, "x", &v);
}

EAPI void eon_paint_square_x_set(Eon_Paint_Square *s, int x)
{
	Eon_Coord coord;
	Ekeko_Value v;

	eon_coord_absolute_set(&coord, x);
	eon_value_coord_from(&v, &coord);
	ekeko_object_property_value_set((Ekeko_Object *)s, "x", &v);
}

EAPI void eon_paint_square_y_get(Eon_Paint_Square *s, Eon_Coord *y)
{
	Eon_Paint_Square_Private *prv;

	prv = PRIVATE(s);
	if (y) *y = prv->y;
}

EAPI void eon_paint_square_y_set(Eon_Paint_Square *s, int y)
{
	Eon_Coord coord;
	Ekeko_Value v;

	eon_coord_absolute_set(&coord, y);
	eon_value_coord_from(&v, &coord);
	ekeko_object_property_value_set((Ekeko_Object *)s, "y", &v);
}

EAPI void eon_paint_square_y_rel_set(Eon_Paint_Square *s, int y)
{
	Eon_Coord coord;
	Ekeko_Value v;

	eon_coord_relative_set(&coord, y);
	eon_value_coord_from(&v, &coord);
	ekeko_object_property_value_set((Ekeko_Object *)s, "y", &v);
}

EAPI void eon_paint_square_w_get(Eon_Paint_Square *s, Eon_Coord *w)
{
	Eon_Paint_Square_Private *prv;

	prv = PRIVATE(s);
	if (w) *w = prv->w;
}

EAPI void eon_paint_square_w_set(Eon_Paint_Square *s, int w)
{
	Eon_Coord coord;
	Ekeko_Value v;

	eon_coord_absolute_set(&coord, w);
	eon_value_coord_from(&v, &coord);
	ekeko_object_property_value_set((Ekeko_Object *)s, "w", &v);
}

EAPI void eon_paint_square_w_rel_set(Eon_Paint_Square *s, int w)
{
	Eon_Coord coord;
	Ekeko_Value v;

	eon_coord_relative_set(&coord, w);
	eon_value_coord_from(&v, &coord);
	ekeko_object_property_value_set((Ekeko_Object *)s, "w", &v);
}

EAPI void eon_paint_square_h_get(Eon_Paint_Square *s, Eon_Coord *h)
{
	Eon_Paint_Square_Private *prv;

	prv = PRIVATE(s);
	if (h) *h = prv->h;
}

EAPI void eon_paint_square_h_set(Eon_Paint_Square *s, int h)
{
	Eon_Coord coord;
	Ekeko_Value v;

	eon_coord_absolute_set(&coord, h);
	eon_value_coord_from(&v, &coord);
	ekeko_object_property_value_set((Ekeko_Object *)s, "h", &v);
}

EAPI void eon_paint_square_h_rel_set(Eon_Paint_Square *s, int h)
{
	Eon_Coord coord;
	Ekeko_Value v;

	eon_coord_relative_set(&coord, h);
	eon_value_coord_from(&v, &coord);
	ekeko_object_property_value_set((Ekeko_Object *)s, "h", &v);
}
/**
 * Gets the coordinates of a square paint object
 * @param ps The square paint object
 * @param x Pointer to store the horizontal coordinate
 * @param y Pointer to store the vertical coordinate
 * @param w Pointer to store the width coordinate
 * @param h Pointer to store the height coordiante
 */
EAPI void eon_paint_square_coords_get(Eon_Paint_Square *ps, Eon_Coord *x,
		Eon_Coord *y, Eon_Coord *w, Eon_Coord *h)
{
	Eon_Paint_Square_Private *prv = PRIVATE(ps);

	if (x) *x = prv->x;
	if (y) *y = prv->y;
	if (w) *w = prv->w;
	if (h) *h = prv->h;
}
