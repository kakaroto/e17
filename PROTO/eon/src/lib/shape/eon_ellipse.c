/*
 * eon_ellipse.c
 *
 *  Created on: 28-jun-2009
 *      Author: jl
 */

#include "Eon.h"
#include "eon_private.h"
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
#define PRIVATE(d) ((Eon_Ellipse_Private *)((Eon_Ellipse *)(d))->private)
struct _Eon_Ellipse_Private
{
	Eon_Coord x;
	Eon_Coord y;
	int radius;
	void *engine_data;
};

static void _update_geometry(Eon_Ellipse *p)
{
	Eon_Ellipse_Private *prv = PRIVATE(p);
	Eina_Rectangle geom;

	eina_rectangle_coords_from(&geom, prv->x.final - prv->radius, prv ->y.final - prv->radius, prv->radius << 1, prv->radius << 1);
	ekeko_renderable_geometry_set((Ekeko_Renderable *)p, &geom);
}

/* Just informs that the x.final property has to be recalculated */
static void _x_inform(const Ekeko_Object *o, Ekeko_Event *e, void *data)
{
	Eon_Ellipse_Private *prv = PRIVATE(data);
	Ekeko_Value v;

	printf("[Eon_Ellipse] Informing X change\n");
	eon_value_coord_from(&v, &prv->x);
	ekeko_object_property_value_set((Ekeko_Object *)data, "x", &v);
}

/* Just informs that the y.final property has to be recalculated */
static void _y_inform(const Ekeko_Object *o, Ekeko_Event *e, void *data)
{
	Eon_Ellipse_Private *prv = PRIVATE(data);
	Ekeko_Value v;

	printf("[Eon_Ellipse] Informing Y change\n");
	eon_value_coord_from(&v, &prv->y);
	ekeko_object_property_value_set((Ekeko_Object *)data, "y", &v);
}


static void _x_change(const Ekeko_Object *o, Ekeko_Event *e, void *data)
{
	Ekeko_Event_Mutation *em = (Ekeko_Event_Mutation *)e;
	Eon_Ellipse *s = (Eon_Ellipse *)o;
	Eon_Ellipse_Private *prv = PRIVATE(o);
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
	Eon_Ellipse *s = (Eon_Ellipse *)o;
	Eon_Ellipse_Private *prv = PRIVATE(o);
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
	Eon_Ellipse *s = (Eon_Ellipse *)o;
	_update_geometry(s);
}

static void _render(Eon_Shape *s, Eon_Engine *eng, void *engine_data, void *canvas_data, Eina_Rectangle *clip)
{
	Eon_Ellipse *p;
	Eon_Ellipse_Private *prv;

	p = (Eon_Ellipse *)s;

#ifdef EON_DEBUG
	printf("[Eon_Ellipse] Rendering polygon %p into canvas %p\n", r, c);
#endif
	eon_engine_ellipse_render(eng, engine_data, canvas_data, clip);
}

static void _ctor(void *instance)
{
	Eon_Ellipse *p;
	Eon_Ellipse_Private *prv;

	p = (Eon_Ellipse*) instance;
	p->private = prv = ekeko_type_instance_private_get(eon_polygon_type_get(), instance);
	p->parent.render = _render;
	p->parent.create = eon_engine_ellipse_create;
	/* events */
	ekeko_event_listener_add((Ekeko_Object *)p, EON_CIRCLE_X_CHANGED, _x_change, EINA_FALSE, NULL);
	ekeko_event_listener_add((Ekeko_Object *)p, EON_CIRCLE_Y_CHANGED, _y_change, EINA_FALSE, NULL);
	ekeko_event_listener_add((Ekeko_Object *)p, EON_CIRCLE_RADIUS_CHANGED, _radius_change, EINA_FALSE, NULL);
}

static void _dtor(void *polygon)
{

}

static Eina_Bool _appendable(void *instance, void *child)
{
	if (!ekeko_type_instance_is_of(child, EON_TYPE_ANIMATION))
	{
		return EINA_FALSE;
	}
	return EINA_TRUE;
}
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
Ekeko_Property_Id EON_CIRCLE_X;
Ekeko_Property_Id EON_CIRCLE_Y;
Ekeko_Property_Id EON_CIRCLE_RADIUS;

EAPI Ekeko_Type *eon_ellipse_type_get(void)
{
	static Ekeko_Type *type = NULL;

	if (!type)
	{
		type = ekeko_type_new(EON_TYPE_CIRCLE, sizeof(Eon_Ellipse),
				sizeof(Eon_Ellipse_Private), eon_shape_type_get(),
				_ctor, _dtor, eon_shape_appendable);
		EON_CIRCLE_X = EKEKO_TYPE_PROP_SINGLE_ADD(type, "x", EON_PROPERTY_COORD, OFFSET(Eon_Ellipse_Private, x));
		EON_CIRCLE_Y = EKEKO_TYPE_PROP_SINGLE_ADD(type, "y", EON_PROPERTY_COORD, OFFSET(Eon_Ellipse_Private, y));
		EON_CIRCLE_RADIUS = EKEKO_TYPE_PROP_SINGLE_ADD(type, "radius", EKEKO_PROPERTY_INT, OFFSET(Eon_Ellipse_Private, radius));
	}

	return type;
}

EAPI Eon_Ellipse * eon_ellipse_new(Eon_Canvas *c)
{
	Eon_Ellipse *p;

	p = ekeko_type_instance_new(eon_ellipse_type_get());
	ekeko_object_child_append((Ekeko_Object *)c, (Ekeko_Object *)p);

	return p;
}



