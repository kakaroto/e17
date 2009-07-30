/*
 * eon_square_shape.c
 *
 *  Created on: 26-feb-2009
 *      Author: jl
 */
#include "Eon.h"
#include "eon_private.h"
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
#define PRIVATE(d) ((Eon_Square_Private *)((Eon_Square *)(d))->private)
#define EON_TYPE_SQUARE "Eon_Square"
struct _Eon_Square_Private
{
	Eon_Coord x, y, w, h;
};

/* Just informs that the x.final property has to be recalculated */
static void _x_inform(const Ekeko_Object *o, Ekeko_Event *e, void *data)
{
	Eon_Square_Private *prv = PRIVATE(data);
	Ekeko_Value v;

	printf("[Eon_Square] Informing X change\n");
	eon_value_coord_from(&v, &prv->x);
	ekeko_object_property_value_set((Ekeko_Object *)data, "x", &v);
}

/* Just informs that the y.final property has to be recalculated */
static void _y_inform(const Ekeko_Object *o, Ekeko_Event *e, void *data)
{
	Eon_Square_Private *prv = PRIVATE(data);
	Ekeko_Value v;

	printf("[Eon_Square] Informing Y change\n");
	eon_value_coord_from(&v, &prv->y);
	ekeko_object_property_value_set((Ekeko_Object *)data, "y", &v);
}

/* Just informs that the w.final property has to be recalculated */
static void _w_inform(const Ekeko_Object *o, Ekeko_Event *e, void *data)
{
	Eon_Square_Private *prv = PRIVATE(data);
	Ekeko_Value v;

	printf("[Eon_Square] Informing W change\n");
	eon_value_coord_from(&v, &prv->w);
	ekeko_object_property_value_set((Ekeko_Object *)data, "w", &v);
}

/* Just informs that the h.final property has to be recalculated */
static void _h_inform(const Ekeko_Object *o, Ekeko_Event *e, void *data)
{
	Eon_Square_Private *prv = PRIVATE(data);
	Ekeko_Value v;

	printf("[Eon_Square] Informing H change\n");
	eon_value_coord_from(&v, &prv->h);
	ekeko_object_property_value_set((Ekeko_Object *)data, "h", &v);
}

/* Called whenever the x property changes */
static void _x_change(const Ekeko_Object *o, Ekeko_Event *e, void *data)
{
	Ekeko_Event_Mutation *em = (Ekeko_Event_Mutation *)e;
	Eon_Square *s = (Eon_Square *)o;
	Eon_Square_Private *prv = PRIVATE(o);
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
	Eon_Square *s = (Eon_Square *)o;
	Eon_Square_Private *prv = PRIVATE(o);
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
	Eon_Square *s = (Eon_Square *)o;
	Eon_Square_Private *prv = PRIVATE(o);
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
	Eon_Square *s = (Eon_Square *)o;
	Eon_Square_Private *prv = PRIVATE(o);
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


static void _ctor(void *instance)
{
	Eon_Square *s;
	Eon_Square_Private *prv;

	s = (Eon_Square*) instance;
	s->private = prv = ekeko_type_instance_private_get(eon_square_type_get(), instance);
	ekeko_event_listener_add((Ekeko_Object *)s, EON_SQUARE_X_CHANGED, _x_change, EINA_FALSE, NULL);
	ekeko_event_listener_add((Ekeko_Object *)s, EON_SQUARE_Y_CHANGED, _y_change, EINA_FALSE, NULL);
	ekeko_event_listener_add((Ekeko_Object *)s, EON_SQUARE_W_CHANGED, _w_change, EINA_FALSE, NULL);
	ekeko_event_listener_add((Ekeko_Object *)s, EON_SQUARE_H_CHANGED, _h_change, EINA_FALSE, NULL);
}

static void _dtor(void *rect)
{

}

/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/

/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
Ekeko_Property_Id EON_SQUARE_X;
Ekeko_Property_Id EON_SQUARE_Y;
Ekeko_Property_Id EON_SQUARE_W;
Ekeko_Property_Id EON_SQUARE_H;

EAPI Ekeko_Type *eon_square_type_get(void)
{
	static Ekeko_Type *type = NULL;

	if (!type)
	{
		type = ekeko_type_new(EON_TYPE_SQUARE, sizeof(Eon_Square),
				sizeof(Eon_Square_Private), eon_shape_type_get(),
				_ctor, _dtor, NULL);
		EON_SQUARE_X = EKEKO_TYPE_PROP_SINGLE_ADD(type, "x", EON_PROPERTY_COORD, OFFSET(Eon_Square_Private, x));
		EON_SQUARE_Y = EKEKO_TYPE_PROP_SINGLE_ADD(type, "y", EON_PROPERTY_COORD, OFFSET(Eon_Square_Private, y));
		EON_SQUARE_W = EKEKO_TYPE_PROP_SINGLE_ADD(type, "w", EON_PROPERTY_COORD, OFFSET(Eon_Square_Private, w));
		EON_SQUARE_H = EKEKO_TYPE_PROP_SINGLE_ADD(type, "h", EON_PROPERTY_COORD, OFFSET(Eon_Square_Private, h));
#if 0
	ekeko_event_listener_add((Ekeko_Object *)i, EON_PAINT_X_CHANGED, _geometry_calc, EINA_FALSE, NULL);
	ekeko_event_listener_add((Ekeko_Object *)i, EON_PAINT_Y_CHANGED, _geometry_calc, EINA_FALSE, NULL);
	ekeko_event_listener_add((Ekeko_Object *)i, EON_PAINT_W_CHANGED, _geometry_calc, EINA_FALSE, NULL);
	ekeko_event_listener_add((Ekeko_Object *)i, EON_PAINT_H_CHANGED, _geometry_calc, EINA_FALSE, NULL);
#endif
	}

	return type;
}

EAPI void eon_square_x_rel_set(Eon_Square *s, int x)
{
	Eon_Coord coord;
	Ekeko_Value v;

	eon_coord_set(&coord, x, EON_COORD_RELATIVE);
	eon_value_coord_from(&v, &coord);
	ekeko_object_property_value_set((Ekeko_Object *)s, "x", &v);
}

EAPI void eon_square_x_set(Eon_Square *s, int x)
{
	Eon_Coord coord;
	Ekeko_Value v;

	eon_coord_set(&coord, x, EON_COORD_ABSOLUTE);
	eon_value_coord_from(&v, &coord);
	ekeko_object_property_value_set((Ekeko_Object *)s, "x", &v);
}

EAPI void eon_square_y_set(Eon_Square *s, int y)
{
	Eon_Coord coord;
	Ekeko_Value v;

	eon_coord_set(&coord, y, EON_COORD_ABSOLUTE);
	eon_value_coord_from(&v, &coord);
	ekeko_object_property_value_set((Ekeko_Object *)s, "y", &v);
}

EAPI void eon_square_y_rel_set(Eon_Square *s, int y)
{
	Eon_Coord coord;
	Ekeko_Value v;

	eon_coord_set(&coord, y, EON_COORD_RELATIVE);
	eon_value_coord_from(&v, &coord);
	ekeko_object_property_value_set((Ekeko_Object *)s, "y", &v);
}

EAPI void eon_square_w_set(Eon_Square *s, int w)
{
	Eon_Coord coord;
	Ekeko_Value v;

	eon_coord_set(&coord, w, EON_COORD_ABSOLUTE);
	eon_value_coord_from(&v, &coord);
	ekeko_object_property_value_set((Ekeko_Object *)s, "w", &v);
}

EAPI void eon_square_w_rel_set(Eon_Square *s, int w)
{
	Eon_Coord coord;
	Ekeko_Value v;

	eon_coord_set(&coord, w, EON_COORD_RELATIVE);
	eon_value_coord_from(&v, &coord);
	ekeko_object_property_value_set((Ekeko_Object *)s, "w", &v);
}

EAPI void eon_square_h_set(Eon_Square *s, int h)
{
	Eon_Coord coord;
	Ekeko_Value v;

	eon_coord_set(&coord, h, EON_COORD_ABSOLUTE);
	eon_value_coord_from(&v, &coord);
	ekeko_object_property_value_set((Ekeko_Object *)s, "h", &v);
}

EAPI void eon_square_h_rel_set(Eon_Square *s, int h)
{
	Eon_Coord coord;
	Ekeko_Value v;

	eon_coord_set(&coord, h, EON_COORD_RELATIVE);
	eon_value_coord_from(&v, &coord);
	ekeko_object_property_value_set((Ekeko_Object *)s, "h", &v);
}

EAPI void eon_square_coords_get(Eon_Square *s, Eon_Coord *x, Eon_Coord *y, Eon_Coord *w, Eon_Coord *h)
{
	Eon_Square_Private *prv = PRIVATE(s);

	*x = prv->x;
	*y = prv->y;
	*w = prv->w;
	*h = prv->h;
}
