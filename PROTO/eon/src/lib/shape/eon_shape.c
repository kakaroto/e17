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
	Eon_Filter *filter;
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
	/* this is for the children classes that go through our
	 * matrix interface, here we store the untransformed geometry
	 */
	Eina_Rectangle srect;
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
	//prv->engine_data = s->create(eng, s);
}

static void _ext_paint_change(const Ekeko_Object *o, Ekeko_Event *e, void *data)
{
	/* mark this shape a changed */
	eon_paint_change((Eon_Paint *)data);
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
	Eon_Paint *p = (Eon_Paint *)o;
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
	eon_paint_change(p);
}

static void _ctor(void *instance)
{
	Eon_Shape *s;
	Eon_Shape_Private *prv;

	s = (Eon_Shape*) instance;
	s->private = prv = ekeko_type_instance_private_get(eon_shape_type_get(), instance);
	/* default values */
	prv->draw_mode = ENESIM_SHAPE_DRAW_MODE_STROKE_FILL;
	/* the events */
	ekeko_event_listener_add((Ekeko_Object *)s, EKEKO_EVENT_OBJECT_APPEND, _child_append_cb, EINA_FALSE, NULL);
	ekeko_event_listener_add((Ekeko_Object *)s, EON_SHAPE_FILTER_CHANGED, _filter_change, EINA_FALSE, NULL);
	ekeko_event_listener_add((Ekeko_Object *)s, EON_SHAPE_FILL_PAINT_CHANGED, _paint_change, EINA_FALSE, NULL);
	ekeko_event_listener_add((Ekeko_Object *)s, EON_SHAPE_STROKE_PAINT_CHANGED, _paint_change, EINA_FALSE, NULL);
}

static void _dtor(void *shape)
{

}
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
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
Ekeko_Property_Id EON_SHAPE_FILTER;
Ekeko_Property_Id EON_SHAPE_FILL_COLOR;
Ekeko_Property_Id EON_SHAPE_FILL_PAINT;
Ekeko_Property_Id EON_SHAPE_STROKE_COLOR;
Ekeko_Property_Id EON_SHAPE_STROKE_PAINT;
Ekeko_Property_Id EON_SHAPE_STROKE_WIDTH;
Ekeko_Property_Id EON_SHAPE_DRAW_MODE;

EAPI Ekeko_Type *eon_shape_type_get(void)
{
	static Ekeko_Type *type = NULL;

	if (!type)
	{
		type = ekeko_type_new(EON_TYPE_SHAPE, sizeof(Eon_Shape),
				sizeof(Eon_Shape_Private), eon_paint_type_get(),
				_ctor, _dtor, NULL);
		EON_SHAPE_FILTER = EKEKO_TYPE_PROP_SINGLE_ADD(type, "filter", EKEKO_PROPERTY_OBJECT, OFFSET(Eon_Shape_Private, filter));
		EON_SHAPE_FILL_PAINT = EKEKO_TYPE_PROP_SINGLE_ADD(type, "fill_paint", EKEKO_PROPERTY_OBJECT, OFFSET(Eon_Shape_Private, fill.paint));
		EON_SHAPE_FILL_COLOR = EKEKO_TYPE_PROP_SINGLE_ADD(type, "fill_color", EON_PROPERTY_COLOR, OFFSET(Eon_Shape_Private, fill.color));
		EON_SHAPE_STROKE_PAINT = EKEKO_TYPE_PROP_SINGLE_ADD(type, "stroke_paint", EKEKO_PROPERTY_OBJECT, OFFSET(Eon_Shape_Private, stroke.paint));
		EON_SHAPE_STROKE_COLOR = EKEKO_TYPE_PROP_SINGLE_ADD(type, "stroke_color", EON_PROPERTY_COLOR, OFFSET(Eon_Shape_Private, stroke.color));
		EON_SHAPE_STROKE_WIDTH = EKEKO_TYPE_PROP_SINGLE_ADD(type, "stroke_width", EKEKO_PROPERTY_FLOAT, OFFSET(Eon_Shape_Private, stroke.width));
		EON_SHAPE_DRAW_MODE = EKEKO_TYPE_PROP_SINGLE_ADD(type, "draw_mode", EKEKO_PROPERTY_INT, OFFSET(Eon_Shape_Private, draw_mode));
	}

	return type;
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
