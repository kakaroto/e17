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
#ifndef OBJECT_H_
#define OBJECT_H_

Ekeko_Type * object_private_type_get(Ekeko_Object *object);
void object_construct(Ekeko_Type *type, void *instance);
void object_event_listener_add(Ekeko_Object *obj, const char *type, Event_Listener el, Eina_Bool bubble, void *data);
void object_event_listener_remove(Ekeko_Object *obj, const char *type, Event_Listener el, Eina_Bool bubble, void *data);

#endif /* OBJECT_H_ */
