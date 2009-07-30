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
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
void event_mutation_init(Ekeko_Event_Mutation *em, const char *type, const Ekeko_Object *o,
		const Ekeko_Object *rel, const Ekeko_Property *prop, Ekeko_Value *prev, Ekeko_Value *curr,
		Ekeko_Event_Mutation_State state)
{
	ekeko_event_init((Ekeko_Event *)em, type, o, EINA_TRUE);
	em->related = rel;
	em->prev = prev;
	em->curr = curr;
	em->state = state;
	if (prop)
	{
		em->prop = ekeko_property_name_get(prop);
		em->prop_id = ekeko_property_id_get(prop);
	}
}
void event_ui_init(Ekeko_Event_Ui *eui, const char *type, const Ekeko_Object *o,
		const Ekeko_Object *related, const Ekeko_Input *i)
{
	ekeko_event_init((Ekeko_Event *)eui, type, o, EINA_FALSE);
	eui->related = related;
	eui->i = i;
}

void event_mouse_move_init(Ekeko_Event_Mouse *em, const Ekeko_Object *o,
		const Ekeko_Object *related, const Ekeko_Input *i,
		unsigned int sx, unsigned int sy)
{
	event_ui_init((Ekeko_Event_Ui*)em, EKEKO_EVENT_UI_MOUSE_MOVE, o, related, i);
	em->screen.x = sx;
	em->screen.y = sy;
}

void event_mouse_in_init(Ekeko_Event_Mouse *em, const Ekeko_Object *o, const Ekeko_Object *related,
		const Ekeko_Input *i)
{
	event_ui_init((Ekeko_Event_Ui*)em, EKEKO_EVENT_UI_MOUSE_IN, o, related, i);
}
void event_mouse_out_init(Ekeko_Event_Mouse *em, const Ekeko_Object *o, const Ekeko_Object *related,
		const Ekeko_Input *i)
{
	event_ui_init((Ekeko_Event_Ui*)em, EKEKO_EVENT_UI_MOUSE_OUT, o, related, i);
}

void event_mouse_down_init(Ekeko_Event_Mouse *em, const Ekeko_Object *o, const Ekeko_Object *related,
		const Ekeko_Input *i)
{
	event_ui_init((Ekeko_Event_Ui*)em, EKEKO_EVENT_UI_MOUSE_DOWN, o, related, i);
}

void event_mouse_up_init(Ekeko_Event_Mouse *em, const Ekeko_Object *o, const Ekeko_Object *related,
		const Ekeko_Input *i)
{
	event_ui_init((Ekeko_Event_Ui*)em, EKEKO_EVENT_UI_MOUSE_UP, o, related, i);
}

void event_mouse_click_init(Ekeko_Event_Mouse *em, const Ekeko_Object *o, const Ekeko_Object *related,
		const Ekeko_Input *i)
{
	event_ui_init((Ekeko_Event_Ui*)em, EKEKO_EVENT_UI_MOUSE_CLICK, o, related, i);
}


/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
/**
 * Adds a listener for the event type ...
 */
EAPI void ekeko_event_listener_add(Ekeko_Object *o, const char *type,
		Event_Listener el, Eina_Bool bubble, void *data)
{
	object_event_listener_add(o, type, el, bubble, data);
}

EAPI void ekeko_event_listener_remove(Ekeko_Object *o, const char *type,
		Event_Listener el, Eina_Bool bubble, void *data)
{
	object_event_listener_remove(o, type, el, bubble, data);
}

EAPI void ekeko_event_dispatch(Ekeko_Event *e)
{
	ekeko_object_event_dispatch(e->target, e);
}

EAPI void ekeko_event_init(Ekeko_Event *e, const char *type, const Ekeko_Object *o, Eina_Bool bubbles)
{
	e->target = o;
	e->type = type;
	e->bubbles = bubbles;
	e->stop = EINA_FALSE;
}

EAPI void ekeko_event_stop(Ekeko_Event *e)
{
	e->stop = EINA_TRUE;
}
