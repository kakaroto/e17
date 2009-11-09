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
#ifndef EKEKO_PRIVATE_H
#define EKEKO_PRIVATE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DBG(...) EINA_LOG_DOM_DBG(ekeko_dom, __VA_ARGS__)
#define INF(...) EINA_LOG_DOM_INFO(ekeko_dom, __VA_ARGS__)
#define WRN(...) EINA_LOG_DOM_WARN(ekeko_dom, __VA_ARGS__)
#define ERR(...) EINA_LOG_DOM_ERR(ekeko_dom, __VA_ARGS__)

extern int ekeko_dom;

#define OFFSET(type, mem) ((size_t) ((char *)&((type *) 0)->mem - (char *)((type *) 0)))

/* object */
Ekeko_Type * object_private_type_get(Ekeko_Object *object);
void object_construct(Ekeko_Type *type, void *instance);
void object_event_listener_add(Ekeko_Object *obj, const char *type, Event_Listener el, Eina_Bool bubble, void *data);
void object_event_listener_remove(Ekeko_Object *obj, const char *type, Event_Listener el, Eina_Bool bubble, void *data);

/* type */
Eina_Bool type_appendable(Ekeko_Type *t, void *instance, void *child);
void type_construct(Ekeko_Type *t, void *instance);
void * type_instance_private_get_internal(Ekeko_Type *final, Ekeko_Type *t, void *instance);
Eina_Bool type_instance_property_value_set(Ekeko_Type *type, void *instance, char *prop_name, Ekeko_Value *value, Ekeko_Value *old);
const char * type_name_get(Ekeko_Type *t);
void type_instance_property_pointers_get(Ekeko_Type *t, Ekeko_Property *prop, void *instance,
		void **curr, void **prev, char **changed);

/* property */
typedef struct _Property_Iterator Property_Iterator;
Property_Iterator * type_property_iterator_new(Ekeko_Type *t);
Eina_Bool type_property_iterator_next(Property_Iterator *pit, Ekeko_Property **prop);
void type_property_iterator_free(Property_Iterator *pit);

/* value */
void ekeko_value_init(void);
void ekeko_value_shutdown(void);
void ekeko_value_create(Ekeko_Value *value, Ekeko_Value_Type type);
void ekeko_value_pointer_double_to(Ekeko_Value *value, Ekeko_Value_Type type, void *ptr,
		void *prev, char *changed);
void ekeko_value_pointer_set(Ekeko_Value *v, Ekeko_Value_Type vtype, void *ptr);
void ekeko_value_pointer_get(Ekeko_Value *v, Ekeko_Value_Type vtype, void *ptr);

/* property */
Ekeko_Property * property_new(Ekeko_Type *type, char *prop_name, Type_Property_Type prop_type,
		Ekeko_Value_Type value_type, ssize_t curr_offset, ssize_t prev_offset,
		ssize_t changed_offset);
Ekeko_Type * property_type_get(Ekeko_Property *p);
ssize_t property_curr_offset_get(Ekeko_Property *p);
ssize_t property_prev_offset_get(Ekeko_Property *p);
ssize_t property_changed_offset_get(Ekeko_Property *p);
Type_Property_Type property_ptype_get(Ekeko_Property *p);

/* event */
void event_mutation_init(Ekeko_Event_Mutation *em, const char *type,
		Ekeko_Object *o, Ekeko_Object *rel,
		const Ekeko_Property *prop, Ekeko_Value *prev,
		Ekeko_Value *curr, Ekeko_Event_Mutation_State state);
void event_ui_init(Ekeko_Event_Ui *eui, const char *type, Ekeko_Object *o,
		Ekeko_Object *related, const Ekeko_Input *i);
void event_mouse_move_init(Ekeko_Event_Mouse *em, Ekeko_Object *o,
		Ekeko_Object *related, const Ekeko_Input *i,
		unsigned int sx, unsigned int sy,
		unsigned int px, unsigned int py);
void event_mouse_in_init(Ekeko_Event_Mouse *em, Ekeko_Object *o,
		Ekeko_Object *related, const Ekeko_Input *i);
void event_mouse_out_init(Ekeko_Event_Mouse *em, Ekeko_Object *o,
		Ekeko_Object *related, const Ekeko_Input *i);
void event_mouse_down_init(Ekeko_Event_Mouse *em, Ekeko_Object *o,
		Ekeko_Object *related, const Ekeko_Input *i);
void event_mouse_up_init(Ekeko_Event_Mouse *em, Ekeko_Object *o,
		Ekeko_Object *related, const Ekeko_Input *i);
void event_mouse_click_init(Ekeko_Event_Mouse *em, Ekeko_Object *o,
		Ekeko_Object *related, const Ekeko_Input *i);

void ekeko_key_up_init(Ekeko_Event_Key *ek, Ekeko_Object *o,
		const Ekeko_Input *i, Ekeko_Key key, Ekeko_Key_Mod mod);
void ekeko_key_down_init(Ekeko_Event_Key *ek, Ekeko_Object *o,
		const Ekeko_Input *i, Ekeko_Key key, Ekeko_Key_Mod mod);

#endif
