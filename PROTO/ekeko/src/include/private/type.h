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
#ifndef TYPE_H_
#define TYPE_H_

Eina_Bool type_appendable(Ekeko_Type *t, void *instance, void *child);
void type_construct(Ekeko_Type *t, void *instance);
void * type_instance_private_get_internal(Ekeko_Type *final, Ekeko_Type *t, void *instance);
Eina_Bool type_instance_property_value_set(Ekeko_Type *type, void *instance, char *prop_name, Ekeko_Value *value, Ekeko_Value *old);
void type_instance_property_value_get(Ekeko_Type *type, void *instance, char *prop_name, Ekeko_Value *value);
Ekeko_Property * type_property_get(Ekeko_Type *t, const char *name);
const char * type_name_get(Ekeko_Type *t);
void type_instance_property_pointers_get(Ekeko_Type *t, Ekeko_Property *prop, void *instance,
		void **curr, void **prev, char **changed);

typedef struct _Property_Iterator Property_Iterator;
Property_Iterator * type_property_iterator_new(Ekeko_Type *t);
Eina_Bool type_property_iterator_next(Property_Iterator *pit, Ekeko_Property **prop);
void type_property_iterator_free(Property_Iterator *pit);

#endif /* TYPE_H_ */
