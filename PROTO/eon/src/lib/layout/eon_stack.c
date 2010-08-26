/* EON - Stack and Toolkit library
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
/* FIXME
 * the code only works for paint_square objects
 * the code only works whenever all the childs are paint_squares
 * as an animation could be part of the childs we wont support animations here
 */
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
#define PRIVATE(d) ((Eon_Stack_Private *)((Eon_Stack *)(d))->prv)

static Ekeko_Type *_type;

struct _Eon_Stack_Private
{
	Eina_List *childs;
	Eon_Stack_Orientation orientation;
};

/*----------------------------------------------------------------------------*
 *                          Stack paint functions                             *
 *----------------------------------------------------------------------------*/
static inline void _hmove(Eon_Stack *s, Eon_Paint *prev, Eon_Paint *curr, int *final)
{
	Eon_Stack_Private *prv;
	Eina_Rectangle geom;

	if (!curr)
		return;

	prv = PRIVATE(s);
	/* instead of the geometry should we use the boundings? */
	if (!prev)
		eina_rectangle_coords_from(&geom, 0, 0, 0, 0);
	else
		eon_paint_geometry_get(prev, &geom);

	*final = geom.x + geom.w;
}

static inline void _vmove(Eon_Stack *s, Eon_Paint *prev, Eon_Paint *curr, int *final)
{
	Eon_Stack_Private *prv;
	Eina_Rectangle geom;

	if (!curr)
		return;

	prv = PRIVATE(s);
	/* instead of the geometry should we use the boundings? */
	if (!prev)
		eina_rectangle_coords_from(&geom, 0, 0, 0, 0);
	else
		eon_paint_geometry_get(prev, &geom);

	*final = geom.y + geom.h;
}
/*----------------------------------------------------------------------------*
 *                                  Events                                    *
 *----------------------------------------------------------------------------*/
static void _stack_paint_property_get(Ekeko_Object *o, Ekeko_Event *e, void *data)
{
	Ekeko_Event_Mutation *em = (Ekeko_Event_Mutation *)e;
	//Eon_Paint *p = (Ekeko_Object *o);

	/* if property is margin */
	/* if property is margin-top */
	/* if property is margin-left */
	/* if property is margin-bottom */
	/* if property is margin-right */
}

static void _child_x_change(Ekeko_Object *o, Ekeko_Event *e, void *data)
{
	Ekeko_Event_Mutation *em = (Ekeko_Event_Mutation *)e;
	Eon_Stack *s = (Eon_Stack *)data;
	Eon_Stack_Private *prv;
	Eon_Coord *prev, *curr;

	prev = em->prev->value.pointer_value;
	curr = em->curr->value.pointer_value;

	prv = PRIVATE(data);
	if (prv->orientation == EON_STACK_HORIZONTAL)
	{
		Eina_Rectangle geom;
		int final;

		_hmove(s, ekeko_object_prev(o), (Eon_Paint *)o, &final);
		eon_coord_absolute_set(curr, final);
		eon_paint_geometry_get(o, &geom);
		eon_paint_geometry_set(o, final, geom.y, geom.w, geom.h);
	}
}

static void _child_y_change(Ekeko_Object *o, Ekeko_Event *e, void *data)
{
	Ekeko_Event_Mutation *em = (Ekeko_Event_Mutation *)e;
	Eon_Stack *s = (Eon_Stack *)data;
	Eon_Stack_Private *prv;
	Eon_Coord *prev, *curr;

	prev = em->prev->value.pointer_value;
	curr = em->curr->value.pointer_value;

	prv = PRIVATE(data);
	if (prv->orientation == EON_STACK_VERTICAL)
	{
		Eina_Rectangle geom;
		int final;

		_vmove(s, ekeko_object_prev(o), (Eon_Paint *)o, &final);
		eon_coord_absolute_set(curr, final);
		eon_paint_geometry_get(o, &geom);
		eon_paint_geometry_set(o, geom.x, final, geom.w, geom.h);
	}
}

static void _child_w_change(Ekeko_Object *o, Ekeko_Event *e, void *data)
{
	Eon_Stack *s = (Eon_Stack *)data;
	Eon_Stack_Private *prv;

	prv = PRIVATE(data);
	if (prv->orientation == EON_STACK_HORIZONTAL)
	{
		Ekeko_Object *curr;
		int final;

		curr = ekeko_object_next(o);
		_hmove(s, (Eon_Paint *)o, (Eon_Paint *)curr, &final);
		eon_paint_square_x_set(curr, final);
	}
}

static void _child_h_change(Ekeko_Object *o, Ekeko_Event *e, void *data)
{
	Eon_Stack *s = (Eon_Stack *)data;
	Eon_Stack_Private *prv;

	prv = PRIVATE(data);
	if (prv->orientation == EON_STACK_VERTICAL)
	{
		Ekeko_Object *curr;
		int final;

		curr = ekeko_object_next(o);
		_vmove(s, (Eon_Paint *)o, (Eon_Paint *)curr, &final);
		eon_paint_square_y_set(curr, final);
	}
}

static void _orientation_cb(Ekeko_Object *o, Ekeko_Event *e, void *data)
{
	Eina_List *l;
	Eon_Paint *p, *prev = NULL;
	Eon_Stack_Private *prv;

	prv = PRIVATE(o);
	EINA_LIST_FOREACH(prv->childs, l, p)
	{
		int final;

		/* move the object to the desired location */
		if (prv->orientation == EON_STACK_HORIZONTAL)
		{
			_hmove(o, prev, p, &final);
			eon_paint_square_x_set(p, final);
		}
		else
		{
			_vmove(o, prev, p, &final);
			eon_paint_square_y_set(p, final);
		}
		prev = p;
	}
}

static void _child_append_cb(Ekeko_Object *o, Ekeko_Event *e, void *data)
{
	Ekeko_Event_Mutation *em = (Ekeko_Event_Mutation *)e;
	Eon_Stack_Private *prv;
	Eon_Paint *prev;
	int final;

	if ((!ekeko_type_instance_is_of(em->related, EON_TYPE_PAINT_SQUARE)))
		return;

	prv = PRIVATE(o);
	prev = eina_list_data_get(eina_list_last(prv->childs));
	prv->childs = eina_list_append(prv->childs, em->related);
	/* move the object to the desired location */
	if (prv->orientation == EON_STACK_HORIZONTAL)
	{
		_hmove(o, prev, em->related, &final);
		eon_paint_square_x_set(em->related, final);
	}
	else
	{
		_vmove(o, prev, em->related, &final);
		eon_paint_square_y_set(em->related, final);
	}
	/* add the position callbacks to force a certain position */
	ekeko_event_listener_add(em->related, EON_PAINT_SQUARE_X_CHANGED,
			_child_x_change, EINA_FALSE, o);
	ekeko_event_listener_add(em->related, EON_PAINT_SQUARE_Y_CHANGED,
			_child_y_change, EINA_FALSE, o);
	ekeko_event_listener_add(em->related, EON_PAINT_SQUARE_W_CHANGED,
			_child_w_change, EINA_FALSE, o);
	ekeko_event_listener_add(em->related, EON_PAINT_SQUARE_H_CHANGED,
			_child_h_change, EINA_FALSE, o);
}

static void _child_remove_cb(Ekeko_Object *o, Ekeko_Event *e, void *data)
{
	Ekeko_Event_Mutation *em = (Ekeko_Event_Mutation *)e;
	/* restack all the next elements */
	/* remove all the listeners */
	ekeko_event_listener_remove(em->related, EON_PAINT_SQUARE_X_CHANGED,
			_child_x_change, EINA_FALSE, o);
	ekeko_event_listener_remove(em->related, EON_PAINT_SQUARE_Y_CHANGED,
			_child_y_change, EINA_FALSE, o);
	ekeko_event_listener_remove(em->related, EON_PAINT_SQUARE_W_CHANGED,
			_child_w_change, EINA_FALSE, o);
	ekeko_event_listener_remove(em->related, EON_PAINT_SQUARE_H_CHANGED,
			_child_h_change, EINA_FALSE, o);
}

/*----------------------------------------------------------------------------*
 *                           Base Type functions                              *
 *----------------------------------------------------------------------------*/
static Eina_Bool _appendable(Ekeko_Object *o, Ekeko_Object *child)
{
	if ((!ekeko_type_instance_is_of(child, EON_TYPE_PAINT_SQUARE)) &&
			(!ekeko_type_instance_is_of(child, EON_TYPE_SHAPE_SQUARE)) &&
			(!ekeko_type_instance_is_of(child, EON_TYPE_ANIMATION)))
		return EINA_FALSE;
	return EINA_TRUE;
}

static void _ctor(Ekeko_Object *o)
{
	Eon_Stack *c;
	Eon_Stack_Private *prv;

	c = (Eon_Stack *)o;
	c->prv = prv = ekeko_type_instance_private_get(_type, o);
	ekeko_event_listener_add(o, EKEKO_EVENT_OBJECT_APPEND,
			_child_append_cb, EINA_FALSE, NULL);
	ekeko_event_listener_add(o, EKEKO_EVENT_OBJECT_REMOVE,
			_child_remove_cb, EINA_FALSE, NULL);
	ekeko_event_listener_add(o, EON_STACK_ORIENTATION_CHANGED,
			_orientation_cb, EINA_FALSE, NULL);
}

static void _dtor(Ekeko_Object *o)
{

}
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
Ekeko_Property_Id EON_STACK_ORIENTATION;

void eon_stack_init(void)
{
	_type = ekeko_type_new(EON_TYPE_STACK, sizeof(Eon_Stack),
			sizeof(Eon_Stack_Private), eon_layout_type_get(),
			_ctor, _dtor, _appendable);
	EON_STACK_ORIENTATION = EKEKO_TYPE_PROP_SINGLE_ADD(_type, "orientation",
				EKEKO_PROPERTY_INT,
				OFFSET(Eon_Stack_Private, orientation));

	eon_type_register(_type, EON_TYPE_STACK);
}

void eon_stack_shutdown(void)
{
	eon_type_unregister(_type);
}
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
/**
 * Creates a new stack layout
 * @param d The document this stack will be created on
 * @return The newly created stack
 */
EAPI Eon_Stack * eon_stack_new(Eon_Document *d)
{
	Eon_Stack *s;

	s = eon_document_object_new(d, EON_TYPE_STACK);

	return s;
}
