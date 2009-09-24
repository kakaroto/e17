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
#include <limits.h>
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
#define PRIVATE(d) ((Eon_Circle_Private *)((Eon_Circle *)(d))->private)
struct _Eon_Circle_Private
{
	Eon_Coord x;
	Eon_Coord y;
	float radius;
};

static void _update_geometry(Eon_Circle *p)
{
	Eon_Circle_Private *prv = PRIVATE(p);
	Eina_Rectangle geom;

	eina_rectangle_coords_from(&geom, prv->x.final - prv->radius, prv ->y.final - prv->radius, prv->radius * 2, prv->radius * 2);
	eon_paint_geometry_set((Eon_Shape *)p, &geom);
}

/* Just informs that the x.final property has to be recalculated */
static void _x_inform(const Ekeko_Object *o, Ekeko_Event *e, void *data)
{
	Eon_Circle_Private *prv = PRIVATE(data);
	Ekeko_Value v;

	printf("[Eon_Circle] Informing X change\n");
	eon_value_coord_from(&v, &prv->x);
	ekeko_object_property_value_set((Ekeko_Object *)data, "x", &v);
}

/* Just informs that the y.final property has to be recalculated */
static void _y_inform(const Ekeko_Object *o, Ekeko_Event *e, void *data)
{
	Eon_Circle_Private *prv = PRIVATE(data);
	Ekeko_Value v;

	printf("[Eon_Circle] Informing Y change\n");
	eon_value_coord_from(&v, &prv->y);
	ekeko_object_property_value_set((Ekeko_Object *)data, "y", &v);
}


static void _x_change(const Ekeko_Object *o, Ekeko_Event *e, void *data)
{
	Ekeko_Event_Mutation *em = (Ekeko_Event_Mutation *)e;
	Eon_Circle *s = (Eon_Circle *)o;
	Eon_Circle_Private *prv = PRIVATE(o);
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
	_update_geometry(s);
}

/* Called whenever the y property changes */
static void _y_change(const Ekeko_Object *o, Ekeko_Event *e, void *data)
{
	Ekeko_Event_Mutation *em = (Ekeko_Event_Mutation *)e;
	Eon_Circle *s = (Eon_Circle *)o;
	Eon_Circle_Private *prv = PRIVATE(o);
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
	_update_geometry(s);
}

static void _radius_change(const Ekeko_Object *o, Ekeko_Event *e, void *data)
{
	Eon_Circle *s = (Eon_Circle *)o;
	_update_geometry(s);
}

static void _matrix_change(const Ekeko_Object *o, Ekeko_Event *e, void *data)
{
	Eon_Circle *s = (Eon_Circle *)o;
	_update_geometry(s);
}

static Eina_Bool _is_inside(Eon_Shape *s, int x, int y)
{
	Eon_Circle *c = (Eon_Circle *)s;
	Eon_Circle_Private *prv = PRIVATE(c);
	Enesim_Shape_Draw_Mode mode;
	float x2, y2, r2;

	x2 = (x - prv->x.final);
	x2 *= x2;

	y2 = (y - prv->y.final);
	y2 *= y2;

	r2 = prv->radius * prv->radius;

	mode = eon_shape_draw_mode_get(s);
	if (mode == ENESIM_SHAPE_DRAW_MODE_STROKE)
	{
		float r3;
		float sw;

		/* the inner radius */
		sw = eon_shape_stroke_width_get(s);
		r3 = prv->radius - sw;
		r3 *= r3;

		if (x2 + y2 <= r2 && x2 + y2 >= r3)
			return EINA_TRUE;
		else
			return EINA_FALSE;
	}
	else
	{
		if (x2 + y2 <= r2)
			return EINA_TRUE;
		else
			return EINA_FALSE;
	}
}

static void _render(Eon_Shape *s, Eon_Engine *eng, void *engine_data, void *canvas_data, Eina_Rectangle *clip)
{
	Eon_Circle *p;
	Eon_Circle_Private *prv;

	p = (Eon_Circle *)s;

#ifdef EON_DEBUG
	printf("[Eon_Circle] Rendering polygon %p into canvas %p\n", r, c);
#endif
	eon_engine_circle_render(eng, engine_data, canvas_data, clip);
}

static void _ctor(void *instance)
{
	Eon_Circle *p;
	Eon_Circle_Private *prv;

	p = (Eon_Circle*) instance;
	p->private = prv = ekeko_type_instance_private_get(eon_polygon_type_get(), instance);
	p->parent.parent.render = _render;
	p->parent.parent.create = eon_engine_circle_create;
	p->parent.parent.is_inside = _is_inside;
	/* events */
	ekeko_event_listener_add((Ekeko_Object *)p, EON_CIRCLE_X_CHANGED, _x_change, EINA_FALSE, NULL);
	ekeko_event_listener_add((Ekeko_Object *)p, EON_CIRCLE_Y_CHANGED, _y_change, EINA_FALSE, NULL);
	ekeko_event_listener_add((Ekeko_Object *)p, EON_CIRCLE_RADIUS_CHANGED, _radius_change, EINA_FALSE, NULL);
	ekeko_event_listener_add((Ekeko_Object *)p, EON_PAINT_MATRIX_CHANGED, _matrix_change, EINA_FALSE, NULL);
}

static void _dtor(void *polygon)
{

}
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
void eon_circle_style_coords_get(Eon_Circle *c, Eon_Paint *p,
		int *cx, int *cy, float *rad)
{
	Eina_Rectangle geom;
	Eon_Paint *sc = (Eon_Paint *)c;
	Eon_Circle_Private *prv = PRIVATE(c);

	/* FIXME when a paint has relative coordinates and the parent is not
	 * renderable but another style what to do?
	 */
	/* setup the renderer correctly */
	if (eon_paint_coordspace_get(sc) == EON_COORDSPACE_OBJECT)
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
	if (cx) eon_coord_calculate(&prv->x, geom.x, geom.w, cx);
	if (cy) eon_coord_calculate(&prv->y, geom.y, geom.h, cy);
	if (rad) *rad = prv->radius;
}
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
Ekeko_Property_Id EON_CIRCLE_X;
Ekeko_Property_Id EON_CIRCLE_Y;
Ekeko_Property_Id EON_CIRCLE_RADIUS;

EAPI Ekeko_Type *eon_circle_type_get(void)
{
	static Ekeko_Type *type = NULL;

	if (!type)
	{
		type = ekeko_type_new(EON_TYPE_CIRCLE, sizeof(Eon_Circle),
				sizeof(Eon_Circle_Private), eon_shape_type_get(),
				_ctor, _dtor, eon_shape_appendable);
		EON_CIRCLE_X = EKEKO_TYPE_PROP_SINGLE_ADD(type, "x", EON_PROPERTY_COORD, OFFSET(Eon_Circle_Private, x));
		EON_CIRCLE_Y = EKEKO_TYPE_PROP_SINGLE_ADD(type, "y", EON_PROPERTY_COORD, OFFSET(Eon_Circle_Private, y));
		EON_CIRCLE_RADIUS = EKEKO_TYPE_PROP_SINGLE_ADD(type, "radius", EKEKO_PROPERTY_FLOAT, OFFSET(Eon_Circle_Private, radius));
	}

	return type;
}

EAPI Eon_Circle * eon_circle_new(Eon_Canvas *c)
{
	Eon_Circle *p;

	p = ekeko_type_instance_new(eon_circle_type_get());
	ekeko_object_child_append((Ekeko_Object *)c, (Ekeko_Object *)p);

	return p;
}


EAPI void eon_circle_radius_set(Eon_Circle *c, float radius)
{
	Ekeko_Value v;

	ekeko_value_float_from(&v, radius);
	ekeko_object_property_value_set((Ekeko_Object *)c, "radius", &v);
}

EAPI float eon_circle_radius_get(Eon_Circle *c)
{
	Eon_Circle_Private *prv;

	prv = PRIVATE(c);
	return prv->radius;
}

EAPI void eon_circle_x_rel_set(Eon_Circle *s, int x)
{
	Eon_Coord coord;
	Ekeko_Value v;

	eon_coord_set(&coord, x, EON_COORD_RELATIVE);
	eon_value_coord_from(&v, &coord);
	ekeko_object_property_value_set((Ekeko_Object *)s, "x", &v);
}

EAPI void eon_circle_x_set(Eon_Circle *s, int x)
{
	Eon_Coord coord;
	Ekeko_Value v;

	eon_coord_set(&coord, x, EON_COORD_ABSOLUTE);
	eon_value_coord_from(&v, &coord);
	ekeko_object_property_value_set((Ekeko_Object *)s, "x", &v);
}

EAPI void eon_circle_x_get(Eon_Circle *c, Eon_Coord *coord)
{
	Eon_Circle_Private *prv;

	prv = PRIVATE(c);
	*coord = prv->x;
}

EAPI void eon_circle_y_set(Eon_Circle *s, int y)
{
	Eon_Coord coord;
	Ekeko_Value v;

	eon_coord_set(&coord, y, EON_COORD_ABSOLUTE);
	eon_value_coord_from(&v, &coord);
	ekeko_object_property_value_set((Ekeko_Object *)s, "y", &v);
}

EAPI void eon_circle_y_rel_set(Eon_Circle *s, int y)
{
	Eon_Coord coord;
	Ekeko_Value v;

	eon_coord_set(&coord, y, EON_COORD_RELATIVE);
	eon_value_coord_from(&v, &coord);
	ekeko_object_property_value_set((Ekeko_Object *)s, "y", &v);
}

EAPI void eon_circle_y_get(Eon_Circle *c, Eon_Coord *coord)
{
	Eon_Circle_Private *prv;

	prv = PRIVATE(c);
	*coord = prv->y;
}

