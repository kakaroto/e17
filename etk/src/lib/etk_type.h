/* ETK - The Enlightened ToolKit
 * Copyright (C) 2006-2008 Simon Treny, Hisham Mardam-Bey, Vincent Torri, Viktor Kojouharov
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

/** @file etk_type.h */
#ifndef _ETK_TYPE_H_
#define _ETK_TYPE_H_

#include <Evas.h>

#include "etk_signal.h"
#include "etk_property.h"
#include "etk_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup Etk_Type Etk_Type
 * @{
 */

/** Casts the function pointer to an Etk_Constructor */
#define ETK_CONSTRUCTOR(constructor)                ((Etk_Constructor)(constructor))
/** Casts the function pointer to an Etk_Constructor */
#define ETK_DESTRUCTOR(destructor)                  ((Etk_Destructor)(destructor))


/**
 * @struct Etk_Type
 * @brief The type of an Etk_Object: it can have a constructor and a destructor and can inherit from another Etk_Type @n
 * It also has a list of properties that will describe the state of the instanciated object
 */
struct Etk_Type
{
   /* private: */
   char *name;
   int hierarchy_depth;
   Etk_Type **hierarchy;
   Etk_Constructor constructor;
   Etk_Destructor destructor;
   void (*property_set)(Etk_Object *object, int property_id, Etk_Property_Value *value);
   void (*property_get)(Etk_Object *object, int property_id, Etk_Property_Value *value);
   int type_size;

   unsigned int signals_count;

   Etk_Signal **signals;
   Evas_Hash *properties_hash;
};


void          etk_type_shutdown(void);

Etk_Type     *etk_type_new(const char *type_name, Etk_Type *parent_type,
                           int type_size, Etk_Constructor constructor,
                           Etk_Destructor destructor,
                           const Etk_Signal_Description *signals);
void          etk_type_delete(Etk_Type *type);

void          etk_type_object_construct(Etk_Type *type, Etk_Object *object);
void          etk_type_destructors_call(Etk_Type *type, Etk_Object *object);
Etk_Bool      etk_type_inherits_from(Etk_Type *type, Etk_Type *parent);
Etk_Type     *etk_type_parent_type_get(Etk_Type *type);
const char   *etk_type_name_get(Etk_Type *type);
Etk_Type     *etk_type_get_from_name(const char *name);

Etk_Signal   *etk_type_signal_get(Etk_Type *type, int signal_code);
Etk_Signal   *etk_type_signal_get_by_name(Etk_Type *type, const char *signal_name);

Etk_Property *etk_type_property_add(Etk_Type *type, const char *name, int property_id, Etk_Property_Type property_type, Etk_Property_Flags flags, Etk_Property_Value *default_value);
Etk_Bool      etk_type_property_find(Etk_Type *type, const char *name, Etk_Type **property_owner, Etk_Property **property);
void          etk_type_property_list(Etk_Type *type, Eina_List **properties);

/** @} */

#ifdef __cplusplus
}
#endif

#endif
