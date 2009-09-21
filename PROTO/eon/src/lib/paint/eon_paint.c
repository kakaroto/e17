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

#define EON_PAINT_DEBUG 0
/* TODO
 * add hints: min-w, max-w, min-h, max-h
 */
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
#define PRIVATE(d) ((Eon_Paint_Private *)((Eon_Paint *)(d))->private)
struct _Eon_Paint_Private
{
	Eon_Color color; /* FIXME the color should be double state? */
	Enesim_Rop rop;
	Eon_Coord x, y, w, h;
	Enesim_Matrix matrix;
	Enesim_Matrix inverse;
	Eon_Paint_Coordspace coordspace;
	Eon_Paint_Matrixspace matrixspace;
	void *engine_data;
};

static void _child_append_cb(const Ekeko_Object *o, Ekeko_Event *e, void *data)
{
	Ekeko_Event_Mutation *em = (Ekeko_Event_Mutation *)e;
	Eon_Paint *p;
	Eon_Paint_Private *prv;
	Eon_Document *d;
	Eon_Engine *eng;
	Eon_Canvas *c;

	c = eon_paint_canvas_get((Eon_Paint *)o);
	if (!c)
		return;

	d = eon_canvas_document_get(c);
	/* FIXME in case the canvas doesnt have a document */
	eng = eon_document_engine_get(d);
	p = (Eon_Paint *)o;
	prv = PRIVATE(p);
	prv->engine_data = p->create(eng, p);
}

static void _rop_change(const Ekeko_Object *o, Ekeko_Event *e, void *data)
{
	/* before adding the damage check that the rop has changed */
	eon_paint_change((Eon_Paint *)o);
}

static void _color_change(const Ekeko_Object *o, Ekeko_Event *e, void *data)
{
	/* before adding the damage check that the color has changed */
	eon_paint_change((Eon_Paint *)o);
}

static void _matrix_change(const Ekeko_Object *o, Ekeko_Event *e, void *data)
{
	Ekeko_Event_Mutation *em = (Ekeko_Event_Mutation *)e;
	Eon_Paint_Private *prv = PRIVATE(o);
	Enesim_Matrix *m;

	m = em->curr->value.pointer_value;
	enesim_matrix_inverse(m, &prv->inverse);
	eon_paint_change((Eon_Paint *)o);
}

static Eina_Bool _is_inside(Ekeko_Renderable *r, int x, int y)
{
	Eon_Paint *p = (Eon_Paint *)r;
	Eon_Paint_Private *prv = PRIVATE(p);
	Enesim_Matrix_Type mtype;

	if (!p->is_inside)
		return EINA_TRUE;

	/* handle the transformation */
	mtype = enesim_matrix_type_get(&prv->inverse);
	if (mtype != ENESIM_MATRIX_IDENTITY)
	{
		float fx, fy;
#if 0
		x -= prv->srect.x;
		y -= prv->srect.y;
		enesim_matrix_point_transform(&prv->inverse, x, y, &fx, &fy);
		x = fx + prv->srect.x;
		y = fy + prv->srect.y;
#endif
	}
	/* call the shape's is_inside */
	return p->is_inside(p, x, y);
}

/* Renderable render wrapper
 * this will only be called in case it is appended to a canvas
 */
static void _render(Ekeko_Renderable *r, Eina_Rectangle *rect)
{
	Eon_Paint *p;
	Eon_Paint_Private *prv;
	Eon_Document *d;
	Eon_Canvas *c;
	Eon_Engine *eng;
	Eon_Surface *surface;

	p = (Eon_Paint *)r;
	prv = PRIVATE(p);
	c = (Eon_Canvas *)ekeko_renderable_canvas_get(r);
	d = eon_canvas_document_get(c);
	eng = eon_document_engine_get(d);
	surface = eon_canvas_engine_data_get(c);
#if BOUNDING_DEBUG
	printf("RENDERING %s\n", ekeko_object_type_name_get(r));
	eon_engine_debug_rect(eng, surface, 0xffaaaaaa, rect->x, rect->y, rect->w, rect->h);
#endif
	/* Call the paint's render function */
	p->render(p, eng, prv->engine_data, surface, rect);
}

static void _ctor(void *instance)
{
	Eon_Paint *p;
	Eon_Paint_Private *prv;

	p = (Eon_Paint*) instance;
	p->private = prv = ekeko_type_instance_private_get(eon_paint_type_get(), instance);
	/* default values */
	prv->rop = ENESIM_BLEND;
	prv->color = 0xffffffff;
	enesim_matrix_identity(&prv->matrix);
	enesim_matrix_inverse(&prv->matrix, &prv->inverse);
	ekeko_event_listener_add((Ekeko_Object *)p, EON_PAINT_COLOR_CHANGED, _color_change, EINA_FALSE, NULL);
	ekeko_event_listener_add((Ekeko_Object *)p, EON_PAINT_ROP_CHANGED, _rop_change, EINA_FALSE, NULL);
	ekeko_event_listener_add((Ekeko_Object *)p, EON_PAINT_MATRIX_CHANGED, _matrix_change, EINA_FALSE, NULL);
	ekeko_event_listener_add((Ekeko_Object *)p, EKEKO_EVENT_OBJECT_APPEND, _child_append_cb, EINA_FALSE, NULL);
}

static void _dtor(void *paint)
{

}
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
void * eon_paint_engine_data_get(Eon_Paint *p)
{
	Eon_Paint_Private *prv = PRIVATE(p);
	return prv->engine_data;
}

void eon_paint_create(Eon_Paint *p)
{
	Eon_Paint_Private *prv = PRIVATE(p);
	Eon_Canvas *c;
	Eon_Document *d;
	Eon_Engine *eng;

	c = eon_paint_canvas_get(p);
	d = eon_canvas_document_get(c);
	/* FIXME in case the canvas doesnt have a document */
	eng = eon_document_engine_get(d);
	prv->engine_data = p->create(eng, p);
}

Eina_Bool eon_paint_setup(Eon_Paint *p, Eon_Shape *s)
{
#if 0

	Eon_Paint_Private *prv = PRIVATE(p);
	Eon_Canvas *c;
	Eon_Document *d;
	Eon_Engine *eng;

	c = eon_paint_canvas_get(p);
	d = eon_canvas_document_get(c);
	/* FIXME in case the canvas doesnt have a document */
	eng = eon_document_engine_get(d);
	return p->setup(eng, prv->engine_data, s);
#endif
}


Eina_Bool eon_paint_appendable(Ekeko_Object *p, Ekeko_Object *child)
{
	if (!ekeko_type_instance_is_of(child, EON_TYPE_ANIMATION))
		return EINA_FALSE;
	else
		return EINA_TRUE;
}

#if 0
/*
 * There are paints which might have other paints as childs, so we should
 * handle a recursive canvas finding
 */
Eon_Canvas * eon_paint_canvas_get(Eon_Paint *p)
{
	Ekeko_Object *o = (Ekeko_Object *)p;

	do
	{
		o = ekeko_object_parent_get(o);
		if (!o)
			break;
	} while (!ekeko_type_instance_is_of(o, EON_TYPE_CANVAS));
	return (Eon_Canvas *)o;
}
#endif
/* Every paint object is a subclass of a renderable, just return the one there
 * which might be null in case this paint is attached to a style container
 */
Eon_Canvas * eon_paint_canvas_get(Eon_Paint *s)
{
	return (Eon_Canvas *)ekeko_renderable_canvas_get((Ekeko_Renderable *)s);
}

Eon_Canvas * eon_shape_canvas_topmost_get(Eon_Shape *s)
{
	Eon_Canvas *c, *last;

	c = last = (Eon_Canvas *)ekeko_renderable_canvas_get((Ekeko_Renderable *)s);
	while (c)
	{
		last = c;
		c = (Eon_Canvas *)ekeko_renderable_canvas_get((Ekeko_Renderable *)last);
	}
	return last;
}

void eon_paint_change(Eon_Paint *p)
{
	Eon_Canvas *c;
	Ekeko_Object *parent;
	Eina_Rectangle geom;

	/* TODO only add a damage in case its parent is a canvas */
	/* add a damage here */
	c = eon_paint_canvas_get(p);
	if (!c)
		return;

	ekeko_renderable_geometry_get((Ekeko_Renderable *)p, &geom);
	ekeko_canvas_damage_add((Ekeko_Canvas *)c, &geom);
}

#if 0
void eon_paint_inverse_matrix_get(Eon_Paint *p, Eon_Shape *s, Enesim_Matrix *m)
{
	Eon_Paint_Private *prv = PRIVATE(p);

	switch (prv->matrixspace)
	{
		case EON_MATRIXSPACE_OBJECT:
		eon_paint_inverse_matrix_get(s, m);
		break;

		case EON_MATRIXSPACE_USER:
		*m = prv->inverse;
		break;

		case EON_MATRIXSPACE_COMPOSE:
		/* TODO */
		*m = prv->inverse;
		break;
	}
}
#endif
void eon_paint_inverse_matrix_get(Eon_Paint *p, Enesim_Matrix *m)
{
	Eon_Paint_Private *prv;

	prv = PRIVATE(p);
	*m = prv->inverse;
}

/* Sets the geometry for a renderable, if the shape has a transformation
 * matrix, it is aplied on the following way:
 * - multiply w,h * matrix
 * - translate the result to x, y
 *
 */
void eon_shape_geometry_set(Eon_Shape *s, Eina_Rectangle *rect)
{
#if 0
	Eon_Shape_Private *prv;
	Enesim_Matrix_Type mtype;

	prv = PRIVATE(s);

	mtype = enesim_matrix_type_get(&prv->matrix);
	if (mtype == ENESIM_MATRIX_IDENTITY)
	{
		prv->srect = *rect;
		ekeko_renderable_geometry_set((Ekeko_Renderable *)s, rect);
	}
	else
	{
		Eina_Rectangle r;
		Enesim_Quad q;
		float x1, y1, x2, y2, x3, y3, x4, y4;

		eina_rectangle_coords_from(&r, 0, 0, rect->w, rect->h);
		/* get the largest rectangle that fits on the matrix */
		enesim_matrix_rect_transform(&prv->matrix, &r, &q);
		enesim_quad_coords_get(&q, &x1, &y1, &x2, &y2, &x3, &y3, &x4, &y4);
		enesim_quad_rectangle_to(&q, &r);
		r.x += rect->x - 1;
		r.y += rect->y - 1;
		r.w += 2;
		r.h += 2;

		prv->srect = *rect;
		ekeko_renderable_geometry_set((Ekeko_Renderable *)s, &r);
	}
#endif
}

void eon_shape_geometry_get(Eon_Shape *s, Eina_Rectangle *rect)
{
#if 0
	Eon_Shape_Private *prv;

	prv = PRIVATE(s);
	*rect = prv->srect;
#endif
}

/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
Ekeko_Property_Id EON_PAINT_COLOR;
Ekeko_Property_Id EON_PAINT_ROP;
Ekeko_Property_Id EON_PAINT_MATRIX;
Ekeko_Property_Id EON_PAINT_X;
Ekeko_Property_Id EON_PAINT_Y;
Ekeko_Property_Id EON_PAINT_W;
Ekeko_Property_Id EON_PAINT_H;
Ekeko_Property_Id EON_PAINT_COORDSPACE;
Ekeko_Property_Id EON_PAINT_MATRIXSPACE;

EAPI Ekeko_Type *eon_paint_type_get(void)
{
	static Ekeko_Type *type = NULL;

	if (!type)
	{
		type = ekeko_type_new(EON_TYPE_PAINT, sizeof(Eon_Paint),
				sizeof(Eon_Paint_Private), ekeko_renderable_type_get(),
				_ctor, _dtor, NULL);
		EON_PAINT_COLOR = EKEKO_TYPE_PROP_SINGLE_ADD(type, "color", EON_PROPERTY_COLOR, OFFSET(Eon_Paint_Private, color));
		EON_PAINT_ROP = EKEKO_TYPE_PROP_SINGLE_ADD(type, "rop", EKEKO_PROPERTY_INT, OFFSET(Eon_Paint_Private, rop));
		EON_PAINT_MATRIX = EKEKO_TYPE_PROP_SINGLE_ADD(type, "matrix", EON_PROPERTY_MATRIX, OFFSET(Eon_Paint_Private, matrix));
		EON_PAINT_X = EKEKO_TYPE_PROP_SINGLE_ADD(type, "x", EON_PROPERTY_COORD, OFFSET(Eon_Paint_Private, x));
		EON_PAINT_Y = EKEKO_TYPE_PROP_SINGLE_ADD(type, "y", EON_PROPERTY_COORD, OFFSET(Eon_Paint_Private, y));
		EON_PAINT_W = EKEKO_TYPE_PROP_SINGLE_ADD(type, "w", EON_PROPERTY_COORD, OFFSET(Eon_Paint_Private, w));
		EON_PAINT_H = EKEKO_TYPE_PROP_SINGLE_ADD(type, "h", EON_PROPERTY_COORD, OFFSET(Eon_Paint_Private, h));
		EON_PAINT_COORDSPACE = EKEKO_TYPE_PROP_SINGLE_ADD(type, "coordspace", EKEKO_PROPERTY_INT, OFFSET(Eon_Paint_Private, coordspace));
		EON_PAINT_MATRIXSPACE = EKEKO_TYPE_PROP_SINGLE_ADD(type, "matrixspace", EKEKO_PROPERTY_INT, OFFSET(Eon_Paint_Private, matrixspace));
	}

	return type;
}

EAPI Eon_Paint * eon_paint_new(Eon_Canvas *c)
{
	Eon_Paint *r;

	r = ekeko_type_instance_new(eon_paint_type_get());
	ekeko_object_child_append((Ekeko_Object *)c, (Ekeko_Object *)r);

	return r;
}

EAPI void eon_paint_matrix_set(Eon_Paint *p, Enesim_Matrix *m)
{
	Ekeko_Value v;

	eon_value_matrix_from(&v, m);
	ekeko_object_property_value_set((Ekeko_Object *)p, "matrix", &v);
}

EAPI void eon_paint_matrix_get(Eon_Paint *p, Enesim_Matrix *m)
{
	Eon_Paint_Private *prv = PRIVATE(p);
	*m = prv->matrix;
}

EAPI void eon_paint_coordspace_set(Eon_Paint *p, Eon_Paint_Coordspace cs)
{
	Ekeko_Value v;

	ekeko_value_int_from(&v, cs);
	ekeko_object_property_value_set((Ekeko_Object *)p, "coordspace", &v);
}

EAPI Eon_Paint_Coordspace eon_paint_coordspace_get(Eon_Paint *p)
{
	Eon_Paint_Private *prv = PRIVATE(p);

	return prv->coordspace;
}

EAPI void eon_paint_x_rel_set(Eon_Paint *p, int x)
{
	Eon_Coord coord;
	Ekeko_Value v;

	eon_coord_set(&coord, x, EON_COORD_RELATIVE);
	eon_value_coord_from(&v, &coord);
	ekeko_object_property_value_set((Ekeko_Object *)p, "x", &v);
}

EAPI void eon_paint_x_set(Eon_Paint *p, int x)
{
	Eon_Coord coord;
	Ekeko_Value v;

	eon_coord_set(&coord, x, EON_COORD_ABSOLUTE);
	eon_value_coord_from(&v, &coord);
	ekeko_object_property_value_set((Ekeko_Object *)p, "x", &v);
}

EAPI void eon_paint_y_set(Eon_Paint *p, int y)
{
	Eon_Coord coord;
	Ekeko_Value v;

	eon_coord_set(&coord, y, EON_COORD_ABSOLUTE);
	eon_value_coord_from(&v, &coord);
	ekeko_object_property_value_set((Ekeko_Object *)p, "y", &v);
}

EAPI void eon_paint_y_rel_set(Eon_Paint *p, int y)
{
	Eon_Coord coord;
	Ekeko_Value v;

	eon_coord_set(&coord, y, EON_COORD_RELATIVE);
	eon_value_coord_from(&v, &coord);
	ekeko_object_property_value_set((Ekeko_Object *)p, "y", &v);
}

EAPI void eon_paint_w_set(Eon_Paint *p, int w)
{
	Eon_Coord coord;
	Ekeko_Value v;

	eon_coord_set(&coord, w, EON_COORD_ABSOLUTE);
	eon_value_coord_from(&v, &coord);
	ekeko_object_property_value_set((Ekeko_Object *)p, "w", &v);
}

EAPI void eon_paint_w_rel_set(Eon_Paint *p, int w)
{
	Eon_Coord coord;
	Ekeko_Value v;

	eon_coord_set(&coord, w, EON_COORD_RELATIVE);
	eon_value_coord_from(&v, &coord);
	ekeko_object_property_value_set((Ekeko_Object *)p, "w", &v);
}

EAPI void eon_paint_h_set(Eon_Paint *p, int h)
{
	Eon_Coord coord;
	Ekeko_Value v;

	eon_coord_set(&coord, h, EON_COORD_ABSOLUTE);
	eon_value_coord_from(&v, &coord);
	ekeko_object_property_value_set((Ekeko_Object *)p, "h", &v);
}

EAPI void eon_paint_h_rel_set(Eon_Paint *p, int h)
{
	Eon_Coord coord;
	Ekeko_Value v;

	eon_coord_set(&coord, h, EON_COORD_RELATIVE);
	eon_value_coord_from(&v, &coord);
	ekeko_object_property_value_set((Ekeko_Object *)p, "h", &v);
}

EAPI void eon_paint_coords_get(Eon_Paint *p, Eon_Coord *x, Eon_Coord *y, Eon_Coord *w, Eon_Coord *h)
{
	Eon_Paint_Private *prv = PRIVATE(p);

	*x = prv->x;
	*y = prv->y;
	*w = prv->w;
	*h = prv->h;
}

EAPI void eon_paint_matrixspace_set(Eon_Paint *p, Eon_Paint_Matrixspace cs)
{
	Ekeko_Value v;

	ekeko_value_int_from(&v, cs);
	ekeko_object_property_value_set((Ekeko_Object *)p, "matrixspace", &v);
}

EAPI Eon_Paint_Matrixspace eon_paint_matrixspace_get(Eon_Paint *p)
{
	Eon_Paint_Private *prv = PRIVATE(p);

	return prv->matrixspace;
}

EAPI void eon_paint_color_set(Eon_Paint *p, Eon_Color color)
{
	Ekeko_Value v;

	eon_value_color_from(&v, color);
	ekeko_object_property_value_set((Ekeko_Object *)p, "color", &v);
}

EAPI Eon_Color eon_paint_color_get(Eon_Paint *p)
{
	Eon_Paint_Private *prv;

	prv = PRIVATE(p);
	return prv->color;
}

EAPI void eon_paint_rop_set(Eon_Paint *p, Enesim_Rop rop)
{
	Ekeko_Value v;

	ekeko_value_int_from(&v, rop);
	ekeko_object_property_value_set((Ekeko_Object *)p, "rop", &v);
}

EAPI Enesim_Rop eon_paint_rop_get(Eon_Paint *p)
{
	Eon_Paint_Private *prv;

	prv = PRIVATE(p);
	return prv->rop;
}
