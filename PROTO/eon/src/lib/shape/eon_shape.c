/*
 * eon_shape.c
 *
 *  Created on: 04-feb-2009
 *      Author: jl
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
	void *context;
	Eon_Color color; /* FIXME the color should be double state? */
	Eon_Paint *fill;
	Eon_Filter *filter;
	Enesim_Rop rop;
 	/* TODO we'll only support clipping to a rect */
	Eina_Rectangle *clip;
	/* TODO add the engine data here instead of on every shape subclass */
	void *engine_data;
};

static void _child_append_cb(const Ekeko_Object *o, Ekeko_Event *e, void *data)
{
	Ekeko_Event_Mutation *em = (Ekeko_Event_Mutation *)e;
	Eon_Shape *s;
	Eon_Shape_Private *prv;
	Eon_Document *d;
	Eon_Engine *eng;

	/* when this shape is appended to a canvas, try to setup the context */
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
	if (prv->fill && !prv->fill->setup(eng, eon_paint_engine_data_get(prv->fill), s))
		return;

	/* Call the shape's render function */
	s->render(s, eng, prv->engine_data, surface, rect);
}

static void _paint_change(const Ekeko_Object *o, Ekeko_Event *e, void *data)
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
	/* before adding the damage check that the clor has changed */
	eon_shape_change((Eon_Shape *)o);
}

static void _filter_change(const Ekeko_Object *o, Ekeko_Event *e, void *data)
{
	printf("CALLEEEEEEEEEEEEE\n");
}

static void _fill_change(const Ekeko_Object *o, Ekeko_Event *e, void *data)
{
	Ekeko_Event_Mutation *em = (Ekeko_Event_Mutation *)e;
	Eon_Shape *s = (Eon_Shape *)o;
	Ekeko_Object *prev, *curr;

	/* in case the paint reference has changed, unregister all the callbacks
	 * from the previous one and register the callbacks for the new one
	 */
	prev = em->prev->value.object;
	curr = em->curr->value.object;
	if (prev)
	{
		ekeko_event_listener_remove(prev, EKEKO_EVENT_PROP_MODIFY, _paint_change, EINA_FALSE, o);
		/* TODO check that the prev paint doesnt have any other reference, if so
		 * delete it
		 */
	}
	if (curr)
		ekeko_event_listener_add(curr, EKEKO_EVENT_PROP_MODIFY, _paint_change, EINA_FALSE, o);

	eon_shape_change(s);
}

static void _ctor(void *instance)
{
	Eon_Shape *s;
	Eon_Shape_Private *prv;

	s = (Eon_Shape*) instance;
	s->private = prv = ekeko_type_instance_private_get(eon_shape_type_get(), instance);
	s->parent.render = _render;
	prv->rop = ENESIM_BLEND;
	/* the default color, useful for pixel_color operations */
	prv->color = 0xffffffff;
	ekeko_event_listener_add((Ekeko_Object *)s, EKEKO_EVENT_OBJECT_APPEND, _child_append_cb, EINA_FALSE, NULL);
	ekeko_event_listener_add((Ekeko_Object *)s, EON_SHAPE_COLOR_CHANGED, _color_change, EINA_FALSE, NULL);
	ekeko_event_listener_add((Ekeko_Object *)s, EON_SHAPE_ROP_CHANGED, _rop_change, EINA_FALSE, NULL);
	ekeko_event_listener_add((Ekeko_Object *)s, EON_SHAPE_FILTER_CHANGED, _filter_change, EINA_FALSE, NULL);
	ekeko_event_listener_add((Ekeko_Object *)s, EON_SHAPE_FILL_CHANGED, _fill_change, EINA_FALSE, NULL);
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
Ekeko_Property_Id EON_SHAPE_FILL;
Ekeko_Property_Id EON_SHAPE_FILTER;

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
		EON_SHAPE_FILL = EKEKO_TYPE_PROP_SINGLE_ADD(type, "fill", EKEKO_PROPERTY_OBJECT, OFFSET(Eon_Shape_Private, fill));
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

EAPI void eon_shape_fill_set(Eon_Shape *s, Eon_Paint *paint)
{
	Ekeko_Value v;

	ekeko_value_object_from(&v, (Ekeko_Object *)paint);
	ekeko_object_property_value_set((Ekeko_Object *)s, "fill", &v);
}

EAPI Eon_Paint * eon_shape_fill_get(Eon_Shape *s)
{
	Eon_Shape_Private *prv;

	prv = PRIVATE(s);
	return prv->fill;
}

#if 0
/* Code taken from the old image object */
static void _geometry_calc(const Ekeko_Object *o, Ekeko_Event *e, void *data)
{
	Eon_Image *i = (Eon_Image *)o;
	Eon_Image_Private *prv = PRIVATE(i);
	Eina_Rectangle r;
	Eon_Coord x, y, w, h;
	Enesim_Quad q;
	float x1, y1, x2, y2, x3, y3, x4, y4;

	/* TODO matrix operation should be:
	 * - multiply w,h * matrix
	 * - translate the result to x, y
	 */
	/* TODO check that the matrix is not identity */
	/* compute the final geometry multiplying by the context matrix */
	eon_square_coords_get((Eon_Square *)i, &x, &y, &w, &h);
	eina_rectangle_coords_from(&r, 0, 0, w.final,
			h.final);
	/* get the largest rectangle that fits on the matrix */
	enesim_matrix_rect_transform(&prv->matrix, &r, &q);
	enesim_quad_coords_get(&q, &x1, &y1, &x2, &y2, &x3, &y3, &x4, &y4);
	enesim_quad_rectangle_to(&q, &r);
	r.x = x.final;
	r.y = y.final;

#if EON_DEBUG
	printf("[Eon_Image] Setting geometry of size %d %d %d %d\n",
			r.x, r.y, r.w, r.h);
#endif
	ekeko_renderable_geometry_set((Ekeko_Renderable *)i, &r);
}
#endif
