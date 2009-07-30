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
#ifndef EKEKO_OBJECT_H
#define EKEKO_OBJECT_H

/*============================================================================*
 *                                 Events                                     *
 *============================================================================*/
#define EKEKO_OBJECT_ID_CHANGED "idChanged"
/*============================================================================*
 *                               Properties                                   *
 *============================================================================*/
extern Ekeko_Property_Id EKEKO_OBJECT_ID;
/*============================================================================*
 *                                 Class                                      *
 *============================================================================*/
#define EKEKO_OBJECT(o) EKEKO_CAST(o, ekeko_object_type_get(), Ekeko_Object)
typedef struct _Ekeko_Object_Private Ekeko_Object_Private;

struct _Ekeko_Object
{
	Ekeko_Object_Private *private;
};
/*============================================================================*
 *                                Functions                                   *
 *============================================================================*/
EAPI Ekeko_Object * ekeko_object_cast(Ekeko_Object *o, Ekeko_Type *t);

EAPI Ekeko_Object *ekeko_object_new(void);
EAPI void ekeko_object_id_set(Ekeko_Object *object, const char *name);
EAPI const char *ekeko_object_id_get(Ekeko_Object *object);
EAPI Ekeko_Type *ekeko_object_type_get(void);
EAPI void ekeko_object_property_value_set(Ekeko_Object *object, char *prop_name, Ekeko_Value *value);
EAPI void ekeko_object_property_value_get(Ekeko_Object *object, char *prop_name, Ekeko_Value *value);

EAPI Eina_Bool ekeko_object_child_append(Ekeko_Object *p, Ekeko_Object *o);
EAPI void ekeko_object_child_remove(Ekeko_Object *p, Ekeko_Object *o);
EAPI int ekeko_object_child_count(Ekeko_Object *o);
EAPI Ekeko_Object * ekeko_object_child_get_at(Ekeko_Object *o, unsigned int index);
EAPI Ekeko_Object * ekeko_object_child_first_get(Ekeko_Object *o);
EAPI Ekeko_Object * ekeko_object_child_last_get(Ekeko_Object *o);
EAPI Ekeko_Object * ekeko_object_next(Ekeko_Object *o);
EAPI Ekeko_Object * ekeko_object_prev(Ekeko_Object *o);

EAPI void ekeko_object_event_dispatch(const Ekeko_Object *object, Ekeko_Event *evt);
EAPI const char * ekeko_object_type_name_get(const Ekeko_Object *obj);
EAPI Ekeko_Object * ekeko_object_parent_get(const Ekeko_Object *o);
EAPI Ekeko_Property * ekeko_object_property_get(Ekeko_Object *o, const char *prop_name);
EAPI void ekeko_object_process(Ekeko_Object *o);

typedef void (*Ekeko_Object_Dump)(Ekeko_Object *o, int level);
EAPI void ekeko_object_dump_printf(Ekeko_Object *o, int level);
EAPI void ekeko_object_dump(Ekeko_Object *o, Ekeko_Object_Dump dump);

#endif
