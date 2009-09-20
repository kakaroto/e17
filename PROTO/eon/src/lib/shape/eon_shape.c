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
#define BOUNDING_DEBUG 0
#define PRIVATE(d) ((Eon_Shape_Private *)((Eon_Shape *)(d))->private)

struct _Eon_Shape_Private
{
	Eon_Color color; /* FIXME the color should be double state? */
	Eon_Filter *filter;
	Enesim_Rop rop;
 	/* TODO we'll only support clipping to a rect */
	Eina_Rectangle *clip;
	/* TODO add the engine data here instead of on every shape subclass */
	struct
	{
		Eon_Paint *paint;
		Eon_Color color;
		float width;
	} stroke;
	struct
	{
		Eon_Paint *paint;
		Eon_Color color;
	} fill;
	Enesim_Shape_Draw_Mode draw_mode;
	Enesim_Matrix matrix, imatrix;
	/* this is for the children classes that go through our
	 * matrix interface, here we store the untransformed geometry
	 */
	Eina_Rectangle srect;

	void *engine_data;
};

static void _child_append_cb(const Ekeko_Object *o, Ekeko_Event *e, void *data)
{
	Ekeko_Event_Mutation *em = (Ekeko_Event_Mutation *)e;
	Eon_Shape *s;
	Eon_Shape_Private *prv;
	Eon_Document *d;
	Eon_Engine *eng;

	/* when this shape is appended to a canvas */
	d = eon_canvas_document_get((Eon_Canvas *)em->related);
	/* FIXME in case the canvas doesnt have a document */
	eng = eon_document_engine_get(d);
	s = (Eon_Shape *)o;
	prv = PRIVATE(s);
	prv->engine_data = s->create(eng, s);
}

/* Render wrapper */
static void _render(Ekeko_Renderable *r, Eina_Rectangle *rect)
{
	Eon_Shape *s;
	Eon_Shape_Private *prv;
	Eon_Document *d;
	Eon_Canvas *c;
	Eon_Engine *eng;
	Eon_Surface *surface;

	s = (Eon_Shape *)r;
	prv = PRIVATE(s);
	c = (Eon_Canvas *)ekeko_renderable_canvas_get(r);
	d = eon_canvas_document_get(c);
	eng = eon_document_engine_get(d);
	surface = eon_canvas_surface_get(c);
#if BOUNDING_DEBUG
	printf("RENDERING %s\n", ekeko_object_type_name_get(r));
	eon_engine_debug_rect(eng, surface, 0xffaaaaaa, rect->x, rect->y, rect->w, rect->h);
#endif
	/* Setup the paint in case it has one */
	/* FIXME by now we are avoding the case where the image isnt loaded yet, we should
	 * find a good way to handle that
	 */
	if (prv->fill.paint && !prv->fill.paint->setup(eng, eon_paint_engine_data_get(prv->fill.paint), s))
		return;
	if (prv->stroke.paint && !prv->stroke.paint->setup(eng, eon_paint_engine_data_get(prv->stroke.paint), s))
		return;
	/* Call the shape's render function */
	s->render(s, eng, prv->engine_data, surface, rect);
}

static void _ext_paint_change(const Ekeko_Object *o, Ekeko_Event *e, void *data)
{
	/* mark this shape a changed */
	eon_shape_change((Eon_Shape *)data);
}

static void _rop_change(const Ekeko_Object *o, Ekeko_Event *e, void *data)
{
	/* before adding the damage check that the rop has changed */
	eon_shape_change((Eon_Shape *)o);
}

static void _color_change(const Ekeko_Object *o, Ekeko_Event *e, void *data)
{
	/* before adding the damage check that the color has changed */
	eon_shape_change((Eon_Shape *)o);
}

static void _filter_change(const Ekeko_Object *o, Ekeko_Event *e, void *data)
{
}

static void _trans_append(const Ekeko_Object *o, Ekeko_Event *e, void *data)
{
        if (ekeko_type_instance_is_of(o, EON_TYPE_PAINT))
	{
		ekeko_event_listener_add(o, EKEKO_EVENT_PROP_MODIFY,
				_ext_paint_change, EINA_FALSE, data);
	}
}

static void _trans_removed(const Ekeko_Object *o, Ekeko_Event *e, void *data)
{
        if (ekeko_type_instance_is_of(o, EON_TYPE_PAINT))
	{
		ekeko_event_listener_remove(o, EKEKO_EVENT_PROP_MODIFY,
				_ext_paint_change, EINA_FALSE, data);
	}
}


static void _paint_change(const Ekeko_Object *o, Ekeko_Event *e, void *data)
{
	Ekeko_Event_Mutation *em = (Ekeko_Event_Mutation *)e;
	Eon_Shape *s = (Eon_Shape *)o;
	Ekeko_Object *prev, *curr;

	/* FIXME instead of this recursive changes, use the bubbling! :) */
	/* in case the paint reference has changed, unregister all the callbacks
	 * from the previous one and register the callbacks for the new one
	 */
	prev = em->prev->value.object;
	curr = em->curr->value.object;
	if (prev)
	{
		ekeko_event_listener_remove(prev, EKEKO_EVENT_PROP_MODIFY,
				_ext_paint_change, EINA_FALSE, o);
		ekeko_event_listener_remove(prev, EKEKO_EVENT_PROP_MODIFY,
				_ext_paint_change, EINA_TRUE, o);
		/* in case a child is appended we change */
		ekeko_event_listener_remove(prev,
				EKEKO_EVENT_OBJECT_APPEND,
				_trans_append, EINA_FALSE, o);
		/* TODO in case a child is removed we change */
		/* TODO for each child get notified too */
	}
	if (curr)
	{
		/* in case the paint object changes we change */
		ekeko_event_listener_add(curr, EKEKO_EVENT_PROP_MODIFY,
				_ext_paint_change, EINA_FALSE, o);
		/* in case some child's property changes we change */
		ekeko_event_listener_add(curr, EKEKO_EVENT_PROP_MODIFY,
				_ext_paint_change, EINA_TRUE, o);
		/* in case a child is appended we change */
		ekeko_event_listener_add(curr, EKEKO_EVENT_OBJECT_APPEND,
				_trans_append, EINA_FALSE, o);
		/* TODO in case a child is removed we change */
		/* TODO for each child get notified too */
	}
	eon_shape_change(s);
}

static void _matrix_change(const Ekeko_Object *o, Ekeko_Event *e, void *data)
{
	Ekeko_Event_Mutation *em = (Ekeko_Event_Mutation *)e;
	Eon_Shape_Private *prv = PRIVATE(o);
	Enesim_Matrix *m;

	m = em->curr->value.pointer_value;
	enesim_matrix_inverse(m, &prv->imatrix);
}

static Eina_Bool _is_inside(Ekeko_Renderable *r, int x, int y)
{
	Eon_Shape *s = r;
	Eon_Shape_Private *prv = PRIVATE(s);
	Enesim_Matrix_Type mtype;

	/* handle the transformation */
	mtype = enesim_matrix_type_get(&prv->imatrix);
	if (mtype != ENESIM_MATRIX_IDENTITY)
	{
		float fx, fy;

		x -= prv->srect.x;
		y -= prv->srect.y;
		enesim_matrix_point_transform(&prv->imatrix, x, y, &fx, &fy);
		x = fx + prv->srect.x;
		y = fy + prv->srect.y;
	}
	/* call the shape's is_inside */
	if (s->is_inside)
		return s->is_inside(s, x, y);
	return EINA_TRUE;
}

static void _ctor(void *instance)
{
	Eon_Shape *s;
	Eon_Shape_Private *prv;

	s = (Eon_Shape*) instance;
	s->private = prv = ekeko_type_instance_private_get(eon_shape_type_get(), instance);
	/* methods override */
	s->parent.render = _render;
	s->parent.is_inside = _is_inside;
	/* default values */
	prv->rop = ENESIM_BLEND;
	prv->draw_mode = ENESIM_SHAPE_DRAW_MODE_STROKE_FILL;
	prv->color = 0xffffffff;
	enesim_matrix_identity(&prv->matrix);
	enesim_matrix_inverse(&prv->matrix, &prv->imatrix);
	/* the events */
	ekeko_event_listener_add((Ekeko_Object *)s, EKEKO_EVENT_OBJECT_APPEND, _child_append_cb, EINA_FALSE, NULL);
	ekeko_event_listener_add((Ekeko_Object *)s, EON_SHAPE_COLOR_CHANGED, _color_change, EINA_FALSE, NULL);
	ekeko_event_listener_add((Ekeko_Object *)s, EON_SHAPE_ROP_CHANGED, _rop_change, EINA_FALSE, NULL);
	ekeko_event_listener_add((Ekeko_Object *)s, EON_SHAPE_FILTER_CHANGED, _filter_change, EINA_FALSE, NULL);
	ekeko_event_listener_add((Ekeko_Object *)s, EON_SHAPE_FILL_PAINT_CHANGED, _paint_change, EINA_FALSE, NULL);
	ekeko_event_listener_add((Ekeko_Object *)s, EON_SHAPE_STROKE_PAINT_CHANGED, _paint_change, EINA_FALSE, NULL);
	ekeko_event_listener_add((Ekeko_Object *)s, EON_SHAPE_MATRIX_CHANGED, _matrix_change, EINA_FALSE, NULL);
}

static void _dtor(void *shape)
{

}
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
void * eon_shape_engine_data_get(Eon_Shape *s)
{
	Eon_Shape_Private *prv;

	prv = PRIVATE(s);
	return prv->engine_data;
}

void eon_shape_engine_data_set(Eon_Shape *s, void *engine_data)
{
	Eon_Shape_Private *prv;

	prv = PRIVATE(s);
	prv->engine_data = engine_data;
}

Eon_Canvas * eon_shape_canvas_get(Eon_Shape *s)
{
	return (Eon_Canvas *)ekeko_renderable_canvas_get((Ekeko_Renderable *)s);
}

Eon_Canvas * eon_shape_canvas_topmost_get(Eon_Shape *s)
{
	Eon_Canvas *c, *last;

	c = last = ekeko_renderable_canvas_get((Ekeko_Renderable *)s);
	while (c)
	{
		last = c;
		c = ekeko_renderable_canvas_get((Ekeko_Renderable *)last);
	}
	return last;
}

void eon_shape_change(Eon_Shape *s)
{
	Ekeko_Object *parent;
	Eina_Rectangle geom;

	/* add a damage here */
	if (!(parent = ekeko_object_parent_get((Ekeko_Object *)s)))
		return;

	ekeko_renderable_geometry_get((Ekeko_Renderable *)s, &geom);
	ekeko_canvas_damage_add((Ekeko_Canvas *)parent, &geom);
}

void eon_shape_inverse_matrix_get(Eon_Shape *s, Enesim_Matrix *m)
{
	Eon_Shape_Private *prv;

	prv = PRIVATE(s);
	*m = prv->imatrix;
}

/* Sets the geometry for a renderable, if the shape has a transformation
 * matrix, it is aplied on the following way:
 * - multiply w,h * matrix
 * - translate the result to x, y
 *
 */
void eon_shape_geometry_set(Eon_Shape *s, Eina_Rectangle *rect)
{
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
}

void eon_shape_geometry_get(Eon_Shape *s, Eina_Rectangle *rect)
{
	Eon_Shape_Private *prv;

	prv = PRIVATE(s);
	*rect = prv->srect;
}

Eina_Bool eon_shape_appendable(void *instance, void *child)
{
	if (!ekeko_type_instance_is_of(child, EON_TYPE_ANIMATION))
	{
		return EINA_FALSE;
	}
	return EINA_TRUE;
}
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
Ekeko_Property_Id EON_SHAPE_COLOR;
Ekeko_Property_Id EON_SHAPE_ROP;
Ekeko_Property_Id EON_SHAPE_FILTER;
Ekeko_Property_Id EON_SHAPE_FILL_COLOR;
Ekeko_Property_Id EON_SHAPE_FILL_PAINT;
Ekeko_Property_Id EON_SHAPE_STROKE_COLOR;
Ekeko_Property_Id EON_SHAPE_STROKE_PAINT;
Ekeko_Property_Id EON_SHAPE_STROKE_WIDTH;
Ekeko_Property_Id EON_SHAPE_DRAW_MODE;
Ekeko_Property_Id EON_SHAPE_MATRIX;

EAPI Ekeko_Type *eon_shape_type_get(void)
{
	static Ekeko_Type *type = NULL;

	if (!type)
	{
		type = ekeko_type_new(EON_TYPE_SHAPE, sizeof(Eon_Shape),
				sizeof(Eon_Shape_Private), ekeko_renderable_type_get(),
				_ctor, _dtor, NULL);
		EON_SHAPE_COLOR = EKEKO_TYPE_PROP_SINGLE_ADD(type, "color", EON_PROPERTY_COLOR, OFFSET(Eon_Shape_Private, color));
		EON_SHAPE_ROP = EKEKO_TYPE_PROP_SINGLE_ADD(type, "rop", EKEKO_PROPERTY_INT, OFFSET(Eon_Shape_Private, rop));
		EON_SHAPE_FILTER = EKEKO_TYPE_PROP_SINGLE_ADD(type, "filter", EKEKO_PROPERTY_OBJECT, OFFSET(Eon_Shape_Private, filter));
		EON_SHAPE_FILL_PAINT = EKEKO_TYPE_PROP_SINGLE_ADD(type, "fill_paint", EKEKO_PROPERTY_OBJECT, OFFSET(Eon_Shape_Private, fill.paint));
		EON_SHAPE_FILL_COLOR = EKEKO_TYPE_PROP_SINGLE_ADD(type, "fill_color", EON_PROPERTY_COLOR, OFFSET(Eon_Shape_Private, fill.color));
		EON_SHAPE_STROKE_PAINT = EKEKO_TYPE_PROP_SINGLE_ADD(type, "stroke_paint", EKEKO_PROPERTY_OBJECT, OFFSET(Eon_Shape_Private, stroke.paint));
		EON_SHAPE_STROKE_COLOR = EKEKO_TYPE_PROP_SINGLE_ADD(type, "stroke_color", EON_PROPERTY_COLOR, OFFSET(Eon_Shape_Private, stroke.color));
		EON_SHAPE_STROKE_WIDTH = EKEKO_TYPE_PROP_SINGLE_ADD(type, "stroke_width", EKEKO_PROPERTY_FLOAT, OFFSET(Eon_Shape_Private, stroke.width));
		EON_SHAPE_DRAW_MODE = EKEKO_TYPE_PROP_SINGLE_ADD(type, "draw_mode", EKEKO_PROPERTY_INT, OFFSET(Eon_Shape_Private, draw_mode));
		EON_SHAPE_MATRIX = EKEKO_TYPE_PROP_SINGLE_ADD(type, "matrix", EON_PROPERTY_MATRIX, OFFSET(Eon_Shape_Private, matrix));
	}

	return type;
}

EAPI void eon_shape_color_set(Eon_Shape *s, Eon_Color color)
{
	Ekeko_Value v;

	eon_value_color_from(&v, color);
	ekeko_object_property_value_set((Ekeko_Object *)s, "color", &v);
}

EAPI Eon_Color eon_shape_color_get(Eon_Shape *s)
{
	Eon_Shape_Private *prv;

	prv = PRIVATE(s);
	return prv->color;
}

EAPI void eon_shape_rop_set(Eon_Shape *s, Enesim_Rop rop)
{
	Ekeko_Value v;

	ekeko_value_int_from(&v, rop);
	ekeko_object_property_value_set((Ekeko_Object *)s, "rop", &v);
}

EAPI Enesim_Rop eon_shape_rop_get(Eon_Shape *s)
{
	Eon_Shape_Private *prv;

	prv = PRIVATE(s);
	return prv->rop;
}

EAPI void eon_shape_filter_set(Eon_Shape *s, Eon_Filter *filter)
{
	Ekeko_Value v;

	ekeko_value_object_from(&v, (Ekeko_Object *)filter);
	ekeko_object_property_value_set((Ekeko_Object *)s, "filter", &v);
}

EAPI Eon_Filter * eon_shape_filter_get(Eon_Shape *s)
{
	Eon_Shape_Private *prv;

	prv = PRIVATE(s);
	return prv->filter;
}

EAPI void eon_shape_fill_paint_set(Eon_Shape *s, Eon_Paint *paint)
{
	Ekeko_Value v;

	ekeko_value_object_from(&v, (Ekeko_Object *)paint);
	ekeko_object_property_value_set((Ekeko_Object *)s, "fill_paint", &v);
}

EAPI Eon_Paint * eon_shape_fill_paint_get(Eon_Shape *s)
{
	Eon_Shape_Private *prv;

	prv = PRIVATE(s);
	return prv->fill.paint;
}

EAPI void eon_shape_fill_color_set(Eon_Shape *s, Eon_Color color)
{
	Ekeko_Value v;

	eon_value_color_from(&v, color);
	ekeko_object_property_value_set((Ekeko_Object *)s, "fill_color", &v);
}

EAPI Eon_Color eon_shape_fill_color_get(Eon_Shape *s)
{
	Eon_Shape_Private *prv;

	prv = PRIVATE(s);
	return prv->fill.color;
}

EAPI void eon_shape_stroke_paint_set(Eon_Shape *s, Eon_Paint *paint)
{
	Ekeko_Value v;

	ekeko_value_object_from(&v, (Ekeko_Object *)paint);
	ekeko_object_property_value_set((Ekeko_Object *)s, "stroke_paint", &v);
}

EAPI Eon_Paint * eon_shape_stroke_paint_get(Eon_Shape *s)
{
	Eon_Shape_Private *prv;

	prv = PRIVATE(s);
	return prv->stroke.paint;
}

EAPI void eon_shape_stroke_color_set(Eon_Shape *s, Eon_Color color)
{
	Ekeko_Value v;

	eon_value_color_from(&v, color);
	ekeko_object_property_value_set((Ekeko_Object *)s, "stroke_color", &v);
}

EAPI Eon_Color eon_shape_stroke_color_get(Eon_Shape *s)
{
	Eon_Shape_Private *prv;

	prv = PRIVATE(s);
	return prv->stroke.color;
}

EAPI void eon_shape_stroke_width_set(Eon_Shape *s, float width)
{
	Ekeko_Value v;

	ekeko_value_float_from(&v, width);
	ekeko_object_property_value_set((Ekeko_Object *)s, "stroke_width", &v);
}

EAPI float eon_shape_stroke_width_get(Eon_Shape *s)
{
	Eon_Shape_Private *prv;

	prv = PRIVATE(s);
	return prv->stroke.width;
}

EAPI void eon_shape_draw_mode_set(Eon_Shape *s, Enesim_Shape_Draw_Mode draw_mode)
{
	Ekeko_Value v;

	ekeko_value_int_from(&v, draw_mode);
	ekeko_object_property_value_set((Ekeko_Object *)s, "draw_mode", &v);
}

EAPI Enesim_Shape_Draw_Mode eon_shape_draw_mode_get(Eon_Shape *s)
{
	Eon_Shape_Private *prv;

	prv = PRIVATE(s);
	return prv->draw_mode;
}

EAPI void eon_shape_matrix_set(Eon_Shape *s, Enesim_Matrix *m)
{
	Ekeko_Value v;

	eon_value_matrix_from(&v, m);
	ekeko_object_property_value_set((Ekeko_Object *)s, "matrix", &v);
}

EAPI void eon_shape_matrix_get(Eon_Shape *s, Enesim_Matrix *m)
{
	Eon_Shape_Private *prv;

	prv = PRIVATE(s);
	*m = prv->matrix;
}

