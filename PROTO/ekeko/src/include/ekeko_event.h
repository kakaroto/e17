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
	Ekeko_Object *target;
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
#define EKEKO_EVENT_PARENT_UNSET "ParentUnset"
/* The EVENT_PARENT_UNSET event gets triggered whenever an object's parent
 * has been set (Event_Mutation).
 * target: Child
 * related: Parent
 */
#define EKEKO_EVENT_OBJECT_REMOVE "ObjectRemove"
/* Whenever an object gets removed from its parent
 * target: Parent
 * related: Child
 */
#define EKEKO_EVENT_OBJECT_APPEND "ObjectAppend"
/* The EVENT_OBJECT_APPEND event gets triggered whenever an object has been
 * appended to another object (Event_Mutation).
 * target: Parent
 * related: Child
 */
#define EKEKO_EVENT_OBJECT_PROCESS "ObjectProcess"

#define EKEKO_EVENT_OBJECT_REFERENCED "ObjectReferenced"
/* Whenever an object gets referenced by another via a property of type
 * object
 * target: Object that gets referenced
 * related: The object that has the property
 */
#define EKEKO_EVENT_OBJECT_UNREFERENCED "ObjectUnreferenced"
/* Whenever an object gets unreferenced by another via a property of type
 * object
 * target: Object that gets referenced
 * related: The object that has the property
 */

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
	Ekeko_Object *related; /* parent in case of child_append/remove */
	Ekeko_Value *prev; /* previous value */
	Ekeko_Value *curr; /* current value */
	const char *prop; /* property name */
	Ekeko_Property_Id prop_id; /* property id */
} Ekeko_Event_Mutation;

typedef void (*Event_Listener)(Ekeko_Object *, Ekeko_Event *, void * data);

EAPI void ekeko_event_listener_add(Ekeko_Object *o, const char *type, Event_Listener el, Eina_Bool bubble, void *data);
EAPI void ekeko_event_listener_remove(Ekeko_Object *o, const char *type, Event_Listener el, Eina_Bool bubble, void *data);
EAPI void ekeko_event_dispatch(Ekeko_Event *e);
EAPI void ekeko_event_stop(Ekeko_Event *e);
EAPI void ekeko_event_init(Ekeko_Event *e, const char *type, Ekeko_Object *o, Eina_Bool bubbles);

#endif /* EKEKO_EVENT_H_ */
