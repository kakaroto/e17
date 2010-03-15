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
#ifndef EKEKO_TYPE_H
#define EKEKO_TYPE_H

typedef void (*Ekeko_Type_Constructor)(Ekeko_Object *);
typedef void (*Ekeko_Type_Destructor)(Ekeko_Object *);
typedef Eina_Bool (*Ekeko_Type_Appendable)(Ekeko_Object *parent, Ekeko_Object *child);

EAPI Ekeko_Type *ekeko_type_new(char *name, size_t size, size_t priv_size, Ekeko_Type *parent,
		Ekeko_Type_Constructor ctor, Ekeko_Type_Destructor dtor, Ekeko_Type_Appendable append);
EAPI void *ekeko_type_instance_new(Ekeko_Type *type);
EAPI void ekeko_type_instance_delete(void *instance);

EAPI void * ekeko_type_instance_private_get(Ekeko_Type *type, void *instance);
EAPI Eina_Bool ekeko_type_instance_is_of(void *instance, const char *type);

#endif /* EKEKO_TYPE_H */
