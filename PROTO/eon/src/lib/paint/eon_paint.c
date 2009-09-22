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
	/* this is for the children classes that go through our
	 * matrix interface, here we store the untransformed geometry
	 */
	Eina_Rectangle srect;
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
	Eon_Style *s;

	/* FIXME we should fix how an object is actually created on the engine */
	c = eon_paint_canvas_get((Eon_Paint *)o);
	if (c)
		d = eon_canvas_document_get(c);
	else
	{
		//s = ekeko_object_parent_get((Ekeko_Object *)o);
		return;
	}


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

		x -= prv->srect.x;
		y -= prv->srect.y;
		enesim_matrix_point_transform(&prv->inverse, x, y, &fx, &fy);
		x = fx + prv->srect.x;
		y = fy + prv->srect.y;

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

/*
 * This code is taken from eon_square.c
 * as the paint objects now support x, y, w, h coords we should support this
 * too
 */
#if 0
/* Just informs that the x.final property has to be recalculated */
static void _x_inform(const Ekeko_Object *o, Ekeko_Event *e, void *data)
{
	Eon_Paint_Private *prv = PRIVATE(data);
	Ekeko_Value v;

	printf("[Eon_Square] Informing X change\n");
	eon_value_coord_from(&v, &prv->x);
	ekeko_object_property_value_set((Ekeko_Object *)data, "x", &v);
}

/* Just informs that the y.final property has to be recalculated */
static void _y_inform(const Ekeko_Object *o, Ekeko_Event *e, void *data)
{
	Eon_Paint_Private *prv = PRIVATE(data);
	Ekeko_Value v;

	printf("[Eon_Square] Informing Y change\n");
	eon_value_coord_from(&v, &prv->y);
	ekeko_object_property_value_set((Ekeko_Object *)data, "y", &v);
}

/* Just informs that the w.final property has to be recalculated */
static void _w_inform(const Ekeko_Object *o, Ekeko_Event *e, void *data)
{
	Eon_Paint_Private *prv = PRIVATE(data);
	Ekeko_Value v;

	printf("[Eon_Square] Informing W change\n");
	eon_value_coord_from(&v, &prv->w);
	ekeko_object_property_value_set((Ekeko_Object *)data, "w", &v);
}

/* Just informs that the h.final property has to be recalculated */
static void _h_inform(const Ekeko_Object *o, Ekeko_Event *e, void *data)
{
	Eon_Paint_Private *prv = PRIVATE(data);
	Ekeko_Value v;

	printf("[Eon_Square] Informing H change\n");
	eon_value_coord_from(&v, &prv->h);
	ekeko_object_property_value_set((Ekeko_Object *)data, "h", &v);
}

/* Called whenever the x property changes */
static void _x_change(const Ekeko_Object *o, Ekeko_Event *e, void *data)
{
	Ekeko_Event_Mutation *em = (Ekeko_Event_Mutation *)e;
	Eon_Paint *s = (Eon_Paint *)o;
	Eon_Paint_Private *prv = PRIVATE(o);
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
	Eon_Paint *s = (Eon_Paint *)o;
	Eon_Paint_Private *prv = PRIVATE(o);
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
	Eon_Paint *s = (Eon_Paint *)o;
	Eon_Paint_Private *prv = PRIVATE(o);
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
	Eon_Paint *s = (Eon_Paint *)o;
	Eon_Paint_Private *prv = PRIVATE(o);
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
#endif

static void _ctor(void *instance)
{
	Eon_Paint *p;
	Eon_Paint_Private *prv;

	p = (Eon_Paint*) instance;
	p->private = prv = ekeko_type_instance_private_get(eon_paint_type_get(), instance);
	p->parent.is_inside = _is_inside;
	p->parent.render = _render;
	/* default values */
	prv->rop = ENESIM_BLEND;
	prv->color = 0xffffffff;
	enesim_matrix_identity(&prv->matrix);
	enesim_matrix_inverse(&prv->matrix, &prv->inverse);
	ekeko_event_listener_add((Ekeko_Object *)p, EON_PAINT_COLOR_CHANGED, _color_change, EINA_FALSE, NULL);
	ekeko_event_listener_add((Ekeko_Object *)p, EON_PAINT_ROP_CHANGED, _rop_change, EINA_FALSE, NULL);
	ekeko_event_listener_add((Ekeko_Object *)p, EON_PAINT_MATRIX_CHANGED, _matrix_change, EINA_FALSE, NULL);
	ekeko_event_listener_add((Ekeko_Object *)p, EKEKO_EVENT_OBJECT_APPEND, _child_append_cb, EINA_FALSE, NULL);
	/* code taken from eon_square */
#if 0
	ekeko_event_listener_add((Ekeko_Object *)p, EON_PAINT_X_CHANGED, _x_change, EINA_FALSE, NULL);
	ekeko_event_listener_add((Ekeko_Object *)p, EON_PAINT_Y_CHANGED, _y_change, EINA_FALSE, NULL);
	ekeko_event_listener_add((Ekeko_Object *)p, EON_PAINT_W_CHANGED, _w_change, EINA_FALSE, NULL);
	ekeko_event_listener_add((Ekeko_Object *)p, EON_PAINT_H_CHANGED, _h_change, EINA_FALSE, NULL);
#endif
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

/* Every paint object is a subclass of a renderable, just return the one there
 * which might be null in case this paint is attached to a style container
 */
Eon_Canvas * eon_paint_canvas_get(Eon_Paint *p)
{
	return (Eon_Canvas *)ekeko_renderable_canvas_get((Ekeko_Renderable *)p);
}

Eon_Canvas * eon_paint_canvas_topmost_get(Eon_Paint *p)
{
	Eon_Canvas *c, *last;

	c = last = (Eon_Canvas *)ekeko_renderable_canvas_get((Ekeko_Renderable *)p);
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

void eon_paint_style_inverse_matrix_get(Eon_Paint *p, Eon_Paint *rel,
		Enesim_Matrix *m)
{
	Eon_Paint_Private *prv = PRIVATE(p);
	Eon_Paint_Private *rprv;

	switch (prv->matrixspace)
	{
		case EON_MATRIXSPACE_OBJECT:
		rprv = PRIVATE(rel);
		*m = rprv->inverse;
		break;

		case EON_MATRIXSPACE_USER:
		*m = prv->inverse;
		break;

		case EON_MATRIXSPACE_COMPOSE:
		rprv = PRIVATE(rel);
		enesim_matrix_compose(m, &rprv->inverse, &prv->inverse);
		break;
	}
}

void eon_paint_style_coords_get(Eon_Paint *p, Eon_Paint *rel, int *x, int *y,
		int *w, int *h)
{
	Eon_Coord px, py, pw, ph;
	Eina_Rectangle geom;

	/* FIXME when a paint has relative coordinates and the parent is not
	 * renderable but another style what to do?
	 */
	/* setup the renderer correctly */
	if (eon_paint_coordspace_get(p) == EON_COORDSPACE_OBJECT)
	{
		eon_paint_geometry_get(rel, &geom);
	}
	else
	{
		Ekeko_Renderable *r;

		/* FIXME we should get the topmost canvas units not the parent
		 * canvas
		 */
		r = (Ekeko_Renderable *)eon_paint_canvas_topmost_get(rel);
		ekeko_renderable_geometry_get(r, &geom);
	}
	/* THIS IS WRONG, we are always using the paint's coords which are
	 * always 0
	 */
	eon_paint_coords_get(p, &px, &py, &pw, &ph);
	if (x) eon_coord_calculate(&px, geom.x, geom.w, x);
	if (y) eon_coord_calculate(&py, geom.y, geom.h, y);
	if (w) eon_coord_length_calculate(&pw, geom.w, w);
	if (h) eon_coord_length_calculate(&ph, geom.h, h);
}


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
void eon_paint_geometry_set(Eon_Paint *p, Eina_Rectangle *rect)
{
	Eon_Paint_Private *prv;
	Enesim_Matrix_Type mtype;

	prv = PRIVATE(p);

	mtype = enesim_matrix_type_get(&prv->matrix);
	if (mtype == ENESIM_MATRIX_IDENTITY)
	{
		prv->srect = *rect;
		ekeko_renderable_geometry_set((Ekeko_Renderable *)p, rect);
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
		ekeko_renderable_geometry_set((Ekeko_Renderable *)p, &r);
	}
}

void eon_paint_geometry_get(Eon_Paint *p, Eina_Rectangle *rect)
{
	Eon_Paint_Private *prv;

	prv = PRIVATE(p);
	*rect = prv->srect;
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
