/** @file etk_type.h */
#ifndef _ETK_TYPE_H_
#define _ETK_TYPE_H_

#include <Ecore_Data.h>
#include "etk_property.h"
#include "etk_types.h"

/**
 * @defgroup Etk_Type Etk_Type
 * @{
 */

/** @brief  Casts the function pointer to an Etk_Constructor */
#define ETK_CONSTRUCTOR(constructor)                ((Etk_Constructor)(constructor))
/** @brief  Casts the function pointer to an Etk_Constructor */
#define ETK_DESTRUCTOR(destructor)                  ((Etk_Destructor)(destructor))
/** @brief  Casts the function pointer to an Etk_Copy_Constructor */
#define ETK_COPY_CONSTRUCTOR(copy_constructor)      ((Etk_Copy_Constructor)(copy_constructor))

/**
 * @struct Etk_Type
 * @brief The type of an Etk_Object: it has constructors and a destructors and can inherit from another Etk_Type @n
 * It also has a list of properties that will describe the state of the instanciated object, thtrough the property values
 */
struct _Etk_Type
{
   /* private: */
   char *name;
   int hierarchy_depth;
   Etk_Type **hierarchy;
   Etk_Constructor constructor;
   Etk_Destructor destructor;
   Etk_Copy_Constructor copy_constructor;
   void (*property_set)(Etk_Object *object, int property_id, Etk_Property_Value *value);
   void (*property_get)(Etk_Object *object, int property_id, Etk_Property_Value *value);
   int type_size;
   Ecore_Hash *signals_hash;
   Ecore_Hash *properties_hash;
};

Etk_Bool etk_type_init();
void etk_type_shutdown();

Etk_Type *etk_type_new(const char *type_name, Etk_Type *parent_type, int type_size,
   Etk_Constructor constructor, Etk_Destructor destructor, Etk_Copy_Constructor copy_constructor);
void etk_type_delete(Etk_Type *type);

void etk_type_object_construct(Etk_Type *type, Etk_Object *object);
void etk_type_copy_constructors_call(Etk_Type *type, Etk_Object *dst, const Etk_Object *src);
void etk_type_destructors_call(Etk_Type *type, Etk_Object *object);
Etk_Bool etk_type_inherits_from(Etk_Type *type, Etk_Type *parent);
Etk_Type *etk_type_parent_type_get(Etk_Type *type);
const char *etk_type_name_get(Etk_Type *type);

void etk_type_signal_add(Etk_Type *type, Etk_Signal *signal);
void etk_type_signal_remove(Etk_Type *type, Etk_Signal *signal);
Etk_Signal *etk_type_signal_get(Etk_Type *type, const char *signal_name);

Etk_Property *etk_type_property_add(Etk_Type *type, const char *name, int property_id, Etk_Property_Type property_type, Etk_Property_Flags flags, Etk_Property_Value *default_value);
Etk_Bool etk_type_property_find(Etk_Type *type, const char *name, Etk_Type **property_owner, Etk_Property **property);
void etk_type_property_list(Etk_Type *type, Ecore_List *properties);

/** @} */

#endif
