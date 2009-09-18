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
 * + add coordinate space flag, object space or canvas space for relative and
 * absolute coordinates
 */
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
#define PRIVATE(d) ((Eon_Paint_Private *)((Eon_Paint *)(d))->private)
struct _Eon_Paint_Private
{
	/* TODO move x, y, w, h and coordspace to the image object */
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
	/* when this shape is appended to a canvas, try to setup the context */
	d = eon_canvas_document_get(c);
	/* FIXME in case the canvas doesnt have a document */
	eng = eon_document_engine_get(d);
	p = (Eon_Paint *)o;
	prv = PRIVATE(p);
	prv->engine_data = p->create(eng, p);
}

static void _matrix_change(const Ekeko_Object *o, Ekeko_Event *e, void *data)
{
	Ekeko_Event_Mutation *em = (Ekeko_Event_Mutation *)e;
	Eon_Paint *i = (Eon_Paint *)o;
	Eon_Paint_Private *prv = PRIVATE(i);
	Enesim_Matrix *m;
	Ekeko_Object *parent;

	m = em->curr->value.pointer_value;
	enesim_matrix_inverse(m, &prv->inverse);
}

static void _ctor(void *instance)
{
	Eon_Paint *p;
	Eon_Paint_Private *prv;

	p = (Eon_Paint*) instance;
	p->private = prv = ekeko_type_instance_private_get(eon_paint_type_get(), instance);
	enesim_matrix_identity(&prv->matrix);
	enesim_matrix_inverse(&prv->matrix, &prv->inverse);
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

/*
 * There are paints which might have other paints as childs, so we should
 * handle a recursive canvas finding
 */
Eon_Canvas * eon_paint_canvas_get(Eon_Paint *p)
{
	Ekeko_Object *o;

	o = ekeko_object_parent_get((Ekeko_Object *)p);
	while (!ekeko_type_instance_is_of(o, EON_TYPE_CANVAS))
	{
		o = ekeko_object_parent_get(o);
	}
	return (Eon_Canvas *)o;
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
	Eon_Paint_Private *prv = PRIVATE(p);
	Eon_Canvas *c;
	Eon_Document *d;
	Eon_Engine *eng;

	c = eon_paint_canvas_get(p);
	d = eon_canvas_document_get(c);
	/* FIXME in case the canvas doesnt have a document */
	eng = eon_document_engine_get(d);
	return p->setup(eng, prv->engine_data, s);
}

Eina_Bool eon_paint_appendable(Ekeko_Object *p, Ekeko_Object *child)
{
	if (!ekeko_type_instance_is_of(child, EON_TYPE_ANIMATION))
		return EINA_FALSE;
	else
		return EINA_TRUE;
}

void eon_paint_inverse_matrix_get(Eon_Paint *p, Eon_Shape *s, Enesim_Matrix *m)
{
	Eon_Paint_Private *prv = PRIVATE(p);

	switch (prv->matrixspace)
	{
		case EON_MATRIXSPACE_OBJECT:
		eon_shape_inverse_matrix_get(s, m);
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
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
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
				sizeof(Eon_Paint_Private), ekeko_object_type_get(),
				_ctor, _dtor, NULL);
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

