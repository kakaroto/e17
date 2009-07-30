/* EKEKO - Object and property system
 * Copyright (C) 2007-2009 Jorge Luis Zapata
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
#include "Ekeko.h"
#include "ekeko_private.h"
/* TODO add a is_inside function, useful for transformed objects */
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
#define PRIVATE(rend) ((Ekeko_Renderable_Private*)((Ekeko_Renderable *)(rend))->private)

#define TYPE_NAME "Renderable"

struct _Ekeko_Renderable_Private
{
	Ekeko_Canvas *canvas;
	/* the geometry */
	struct
	{

		Eina_Rectangle curr;
		Eina_Rectangle prev;
		char changed;
	} geometry;
	/* the visibility */
	struct
	{
		Eina_Bool curr;
		Eina_Bool prev;
		char changed;
	} visibility;
	int zindex;
	/* When the canvas manages the list of renderables it needs to know if
	 * a renderable is appended */
	Eina_Bool appended;
	/* TODO we should have a way to inform the canvas that this
	 * renderable needs the lower object to draw in that case
	 * also render the bottom one
	 */
};

static Ekeko_Object * _prev_renderable_get(Ekeko_Object *o)
{
	Ekeko_Object *last;

	/* check if the object is a canvas, if so, dont go down */
	if (ekeko_type_instance_is_of(o, "Canvas"))
		return o;
	last = ekeko_object_child_last_get(o);
	if (!last)
	{
		if (ekeko_type_instance_is_of(o, "Renderable"))
			return o;
		else
			return NULL;
	}
	else
	{
		do
		{
			Ekeko_Object *g;

			g = _prev_renderable_get(last);
			if (g)
				return g;
		} while (last = ekeko_object_prev(last));
		/* any of the childs is a renderable, check the object itself */
		if (ekeko_type_instance_is_of(o, "Renderable"))
			return o;
		else
			return NULL;
	}
}

static Ekeko_Object * _prev_renderable_left(Ekeko_Object *r)
{
	while (r = ekeko_object_prev(r))
	{
		Ekeko_Object *g;

		g = _prev_renderable_get(r);
		if (g)
			return g;
	}
	return NULL;
}

static Ekeko_Object * _prev_renderable_up(Ekeko_Object *o)
{
	Ekeko_Object *parent;

	while (parent = ekeko_object_parent_get(o))
	{
		if (ekeko_type_instance_is_of(parent, "Renderable"))
			return parent;
		else
		{
			Ekeko_Object *l;

			l = _prev_renderable_left(parent);
			if (l) return l;
		}
	}
}

/* called whenever a double state property has changed */
static void _visibility_change(const Ekeko_Object *o, Ekeko_Event *e, void *data)
{
	Ekeko_Event_Mutation *em = (Ekeko_Event_Mutation *)e;
	Ekeko_Renderable_Private *prv = PRIVATE(o);

#ifdef EKEKO_DEBUG
	printf("[Ekeko_Renderable] %s prop updated %s\n", ekeko_object_type_name_get(o), em->prop);
#endif
	if (em->state != EVENT_MUTATION_STATE_POST)
		return;
	if (!prv->canvas)
		return;
	/* TODO Check that the geometry is inside the pointer */
	/* if it is visible, send a mouse in, if it was visible send a mouse out */
	/* visibility changed */
	/* TODO check that the renderable is appended? */
	ekeko_canvas_damage_add(prv->canvas, &prv->geometry.curr);
	ekeko_canvas_damage_add(prv->canvas, &prv->geometry.prev);
}

static void _geometry_change(const Ekeko_Object *o, Ekeko_Event *e, void *data)
{
	Ekeko_Event_Mutation *em = (Ekeko_Event_Mutation *)e;
	Ekeko_Renderable_Private *prv = PRIVATE(o);

#ifdef EKEKO_DEBUG
	printf("[Ekeko_Renderable] %s geometry updated\n", ekeko_object_type_name_get(o));
#endif
	if (em->state != EVENT_MUTATION_STATE_POST)
		return;
	if (!prv->canvas)
		return;
	/* TODO Check that the new geometry is inside the pointer */
	/* geometry changed */
	/* TODO check that the renderable is appended? */
	ekeko_canvas_damage_add(prv->canvas, &em->curr->value.rect);
	ekeko_canvas_damage_add(prv->canvas, &em->prev->value.rect);
}

static void _parent_set_cb(const Ekeko_Object *o, Ekeko_Event *e, void *data)
{
	Ekeko_Event_Mutation *em = (Ekeko_Event_Mutation *)e;
	Ekeko_Renderable_Private *prv;
	Ekeko_Object *p = (Ekeko_Object *)em->related;
	Ekeko_Object *last;

	/* check that the upper hierarchy is of type canvas */
	while (p && !ekeko_type_instance_is_of(p, "Canvas"))
	{
		p = ekeko_object_parent_get(p);
	}
	if (!p)
	{
#ifdef EKEKO_DEBUG
		printf("[Ekeko_Renderable] %s Is not of type canvas\n", ekeko_object_type_name_get(o));
#endif
		return;
	}
#ifdef EKEKO_DEBUG
	printf("[Ekeko_Renderable] %s Some parent %p (%s) is a canvas? %p!!!\n", ekeko_object_type_name_get(o), p, ekeko_object_type_name_get(p), ekeko_renderable_canvas_get((Ekeko_Renderable *)p));
#endif
	prv = PRIVATE(((Ekeko_Renderable *)o));
#ifdef EKEKO_DEBUG
	printf("[Ekeko_Renderable] %p has a canvas  at %p\n", o, p);
#endif
	prv->canvas = (Ekeko_Canvas *)p;
	last = _prev_renderable_left(o);
	/* no sibling with a renderable below or itself */
	if (!last)
		last = _prev_renderable_up(o);
	/* no parent with a renderable below */
	if (!last || last == p)
	{
		/* first element */
		/* Set the zindex */
#ifdef EKEKO_DEBUG
		printf("[Ekeko_Renderable] %p no Z index found, first element\n", o);
#endif
	}
	else
	{
		int z;
		z = ekeko_renderable_zindex_get((Ekeko_Renderable *)last);
#ifdef EKEKO_DEBUG
		printf("[Ekeko_Renderable] %p Z index found at %d in %p\n", o, z, last);
#endif
		ekeko_renderable_zindex_set((Ekeko_Renderable *)o, z +1);
	}
	/* TODO propagate the change of zindex locally in case the object is not a canvas */
	/* TODO propagate the change of zindex to the next sibling */
}

static void _ctor(void *instance)
{
	Ekeko_Renderable *rend;
	Ekeko_Renderable_Private *prv;

	rend = EKEKO_RENDERABLE(instance);
	rend->private = prv = ekeko_type_instance_private_get(ekeko_renderable_type_get(), instance);
	/* register to an event where this child is appended to a canvas parent */
	ekeko_event_listener_add(EKEKO_OBJECT(rend), EKEKO_RENDERABLE_VISIBILITY_CHANGED, _visibility_change, EINA_FALSE, NULL);
	ekeko_event_listener_add(EKEKO_OBJECT(rend), EKEKO_RENDERABLE_GEOMETRY_CHANGED, _geometry_change, EINA_FALSE, NULL);
	ekeko_event_listener_add(EKEKO_OBJECT(rend), EKEKO_EVENT_OBJECT_APPEND, _parent_set_cb, EINA_FALSE, NULL);
#ifdef EKEKO_DEBUG
	printf("[Ekeko_Renderable] ctor canvas = %p\n", prv->canvas);
#endif
}

static void _dtor(void *instance)
{

}
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
Eina_Bool renderable_appended_get(Ekeko_Renderable *r)
{
	Ekeko_Renderable_Private *prv;

	prv = PRIVATE(r);
	return prv->appended;
}
void renderable_appended_set(Ekeko_Renderable *r, Eina_Bool appended)
{
	Ekeko_Renderable_Private *prv;

	prv = PRIVATE(r);
	prv->appended = appended;
}
int ekeko_renderable_zindex_get(Ekeko_Renderable *r)
{
	Ekeko_Renderable_Private *prv;

	prv = PRIVATE(r);
	return prv->zindex;
}

void ekeko_renderable_zindex_set(Ekeko_Renderable *r, int zindex)
{
	Ekeko_Renderable_Private *prv;

	prv = PRIVATE(r);
	prv->zindex = zindex;
}

Eina_Bool ekeko_renderable_intersect(Ekeko_Renderable *r, int x, int y)
{
	if (r->is_inside)
		return r->is_inside(r, x, y);
	return EINA_TRUE;
}
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
Ekeko_Property_Id EKEKO_RENDERABLE_GEOMETRY;
Ekeko_Property_Id EKEKO_RENDERABLE_VISIBILITY;

Ekeko_Type *ekeko_renderable_type_get(void)
{
	static Ekeko_Type *renderable_type = NULL;

	if (!renderable_type)
	{
		renderable_type = ekeko_type_new(TYPE_NAME, sizeof(Ekeko_Renderable),
				sizeof(Ekeko_Renderable_Private), ekeko_object_type_get(),
				_ctor, _dtor, NULL);
		/* the properties */
		EKEKO_RENDERABLE_GEOMETRY = EKEKO_TYPE_PROP_DOUBLE_ADD(renderable_type, "geometry", EKEKO_PROPERTY_RECTANGLE,
				OFFSET(Ekeko_Renderable_Private, geometry.curr), OFFSET(Ekeko_Renderable_Private, geometry.prev),
				OFFSET(Ekeko_Renderable_Private, geometry.changed));
		EKEKO_RENDERABLE_VISIBILITY = EKEKO_TYPE_PROP_DOUBLE_ADD(renderable_type, "visibility", EKEKO_PROPERTY_BOOL,
				OFFSET(Ekeko_Renderable_Private, visibility.curr), OFFSET(Ekeko_Renderable_Private, visibility.prev),
				OFFSET(Ekeko_Renderable_Private, visibility.changed));
	}

	return renderable_type;
}

EAPI void ekeko_renderable_move(Ekeko_Renderable *r, int x, int y)
{
	Ekeko_Renderable_Private *prv;
	Eina_Rectangle move;
	Ekeko_Value value;

	/* TODO avoid this duplicate */
	prv = PRIVATE(r);
	move.x = x;
	move.y = y;
	move.w = prv->geometry.curr.w;
	move.h = prv->geometry.curr.h;
	ekeko_value_rectangle_from(&value, &move);
	ekeko_object_property_value_set((Ekeko_Object *)r, "geometry", &value);
}

EAPI void ekeko_renderable_resize(Ekeko_Renderable *r, int w, int h)
{
	Ekeko_Renderable_Private *prv;
	Eina_Rectangle resize;
	Ekeko_Value value;

	/* TODO avoid this duplicate */
	prv = PRIVATE(r);
	resize.x = prv->geometry.curr.x;
	resize.y = prv->geometry.curr.y;
	resize.w = w;
	resize.h = h;
	ekeko_value_rectangle_from(&value, &resize);
	ekeko_object_property_value_set((Ekeko_Object *)r, "geometry", &value);
}

EAPI void ekeko_renderable_geometry_set(Ekeko_Renderable *r, Eina_Rectangle *rect)
{
	Ekeko_Value value;

#ifdef EKEKO_DEBUG
	printf("[Ekeko_Renderable] geometry_set %d %d %d %d\n", rect->x, rect->y, rect->w, rect->h);
#endif
	ekeko_value_rectangle_from(&value, rect);
	ekeko_object_property_value_set((Ekeko_Object *)r, "geometry", &value);
}

EAPI void ekeko_renderable_geometry_get(Ekeko_Renderable *r, Eina_Rectangle *rect)
{
	Ekeko_Renderable_Private *prv;

	prv = PRIVATE(r);
	*rect = prv->geometry.curr;
}

EAPI void ekeko_renderable_visibility_get(Ekeko_Renderable *r, Eina_Bool *visible)
{
	Ekeko_Renderable_Private *prv;

	prv = PRIVATE(r);
#ifdef EKEKO_DEBUG
	printf("[Ekeko_Renderable] %s visibility get %d (%d %d)\n", ekeko_object_type_name_get((Ekeko_Object *)r), prv->visibility.curr, EINA_FALSE, EINA_TRUE);
#endif
	*visible = prv->visibility.curr;
}

EAPI void ekeko_renderable_show(Ekeko_Renderable *r)
{
	Ekeko_Renderable_Private *prv;
	Ekeko_Value value;

	prv = PRIVATE(r);
#ifdef EKEKO_DEBUG
	printf("[Ekeko_Renderable] show\n");
#endif
	if (prv->visibility.curr)
		return;
	ekeko_value_bool_from(&value, EINA_TRUE);
	ekeko_object_property_value_set((Ekeko_Object *)r, "visibility", &value);
}

EAPI void ekeko_renderable_hide(Ekeko_Renderable *r)
{
	Ekeko_Renderable_Private *prv;
	Ekeko_Value value;

	prv = PRIVATE(r);
#ifdef EKEKO_DEBUG
	printf("[Ekeko_Renderable] hide\n");
#endif
	if (!prv->visibility.curr)
		return;
	ekeko_value_bool_from(&value, EINA_FALSE);
	ekeko_object_property_value_set((Ekeko_Object *)r, "visibility", &value);
}

EAPI void ekeko_renderable_visibility_set(Ekeko_Renderable *r, Eina_Bool visible)
{
	Ekeko_Renderable_Private *prv;
	Ekeko_Value value;

	prv = PRIVATE(r);
	if (prv->visibility.curr == visible)
		return;
	ekeko_value_bool_from(&value, visible);
#ifdef EKEKO_DEBUG
	printf("[renderable %s] visibility set %d\n", ekeko_object_type_name_get((Ekeko_Object *)r), value.value.bool_value);
#endif
	ekeko_object_property_value_set((Ekeko_Object *)r, "visibility", &value);
}

EAPI Ekeko_Canvas * ekeko_renderable_canvas_get(Ekeko_Renderable *r)
{
	Ekeko_Renderable_Private *prv;

	prv = PRIVATE(r);
	return prv->canvas;
}
