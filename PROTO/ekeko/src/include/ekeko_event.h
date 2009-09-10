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
#ifndef EKEKO_EVENT_H_
#define EKEKO_EVENT_H_

typedef struct _Ekeko_Event
{
	const char *type;
	const Ekeko_Object *target;
	Eina_Bool bubbles;
	Eina_Bool stop;
} Ekeko_Event;

#define EKEKO_EVENT_OBJECT_DELETE "ObjectDelete"
/* This event uses the core Ekeko_Event iand it is triggered before the
 * destructor of the object is called
 */

#define EKEKO_EVENT_PROP_MODIFY "PropModify"
#define EKEKO_EVENT_PARENT_SET "ParentSet"
/* The EVENT_PARENT_SET event gets triggered whenever an object's parent
 * has been set (Event_Mutation).
 * target: Child
 * related: Parent
 */
#define EKEKO_EVENT_OBJECT_REMOVE "ObjectRemove"
/* Whenever an object gets removed from its parent
 * FIXME right now is the inverse
 * target: Parent
 * related: Child
 */
#define EKEKO_EVENT_OBJECT_APPEND "ObjectAppend"
/* The EVENT_OBJECT_APPEND event gets triggered whenever an object has been
 * appended to another object (Event_Mutation).
 * FIXME right now is the inverse
 * target: Parent
 * related: Child
 */
#define EKEKO_EVENT_OBJECT_REMOVE "ObjectRemove"
#define EKEKO_EVENT_OBJECT_PROCESS "ObjectProcess"

typedef enum _Ekeko_Event_Mutation_State
{
	EVENT_MUTATION_STATE_PRE,
	EVENT_MUTATION_STATE_CURR,
	EVENT_MUTATION_STATE_POST,
} Ekeko_Event_Mutation_State;

typedef struct _Ekeko_Event_Mutation
{
	Ekeko_Event event;
	Ekeko_Event_Mutation_State state; /* pre/post (async) curr (async/sync) */
	const Ekeko_Object *related; /* parent in case of child_append/remove */
	Ekeko_Value *prev; /* previous value */
	Ekeko_Value *curr; /* current value */
	const char *prop; /* property name */
	Ekeko_Property_Id prop_id; /* property id */
} Ekeko_Event_Mutation;


/*
 * TODO change the name of this
 */

#define EKEKO_EVENT_UI_MOUSE_MOVE "mousemove"
#define EKEKO_EVENT_UI_MOUSE_IN "mousein"
#define EKEKO_EVENT_UI_MOUSE_OUT "mouseout"
#define EKEKO_EVENT_UI_MOUSE_DOWN "mousedown"
#define EKEKO_EVENT_UI_MOUSE_UP "mouseup"
#define EKEKO_EVENT_UI_MOUSE_CLICK "click"
#define EKEKO_EVENT_UI_KEY_DOWN "keydown"
#define EKEKO_EVENT_UI_KEY_UP "keyup"

typedef struct _Ekeko_Event_Ui
{
	Ekeko_Event event;
	const Ekeko_Object *related;
	const Ekeko_Input *i;
	/* TODO for now this isnt needed but whenever the keyboard event
	 * is finished we might find duplicate things on the mouse
	 */
} Ekeko_Event_Ui;

typedef struct _Ekeko_Event_Mouse
{
	Ekeko_Event_Ui ui;
	/* screen */
	struct
	{
		unsigned int x;
		unsigned int y;
		unsigned int prev_x;
		unsigned int prev_y;
	} screen;
} Ekeko_Event_Mouse;

typedef enum _Ekeko_Key
{
	EKEKO_KEYS,
} Ekeko_Key;

typedef enum _Ekeko_Key_Mod
{
	EKEKO_KEY_MOD_NONE  = 0x0000,
	EKEKO_KEY_MOD_LSHIFT= 0x0001,
	EKEKO_KEY_MOD_RSHIFT= 0x0002,
	EKEKO_KEY_MOD_LCTRL = 0x0040,
	EKEKO_KEY_MOD_RCTRL = 0x0080,
	EKEKO_KEY_MOD_LALT  = 0x0100,
	EKEKO_KEY_MOD_RALT  = 0x0200,
	EKEKO_KEY_MOD_LMETA = 0x0400,
	EKEKO_KEY_MOD_RMETA = 0x0800,
	EKEKO_KEY_MOD_NUM   = 0x1000,
	EKEKO_KEY_MOD_CAPS  = 0x2000,
	EKEKO_KEY_MOD_MODE  = 0x4000,
} Ekeko_Key_Mod;

#define EKEKO_KEY_MOD_CTRL  (EKEKO_KEY_MOD_LCTRL | EKEKO_KEY_MOD_RCTRL)
#define EKEKO_KEY_MOD_SHIFT (EKEKO_KEY_MOD_LSHIFT | EKEKO_KEY_MOD_RSHIFT)
#define EKEKO_KEY_MOD_ALT   (EKEKO_KEY_MOD_LALT | EKEKO_KEY_MOD_RALT)
#define EKEKO_KEY_MOD_META  (EKEKO_KEY_MOD_LMETA | EKEKO_KEY_MOD_RMETA)

typedef struct _Ekeko_Event_Key
{
	Ekeko_Event_Ui ui;
	Ekeko_Key_Mod mod;
	Ekeko_Key key;
} Ekeko_Event_Key;

typedef void (*Event_Listener)(const Ekeko_Object *, Ekeko_Event *, void * data);

EAPI void ekeko_event_listener_add(Ekeko_Object *o, const char *type, Event_Listener el, Eina_Bool bubble, void *data);
EAPI void ekeko_event_listener_remove(Ekeko_Object *o, const char *type, Event_Listener el, Eina_Bool bubble, void *data);
EAPI void ekeko_event_dispatch(Ekeko_Event *e);
EAPI void ekeko_event_stop(Ekeko_Event *e);
EAPI void ekeko_event_init(Ekeko_Event *e, const char *type, const Ekeko_Object *o, Eina_Bool bubbles);

#endif /* EKEKO_EVENT_H_ */
