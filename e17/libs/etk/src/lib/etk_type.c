/** @file etk_type.c */
#include "etk_type.h"
#include <stdlib.h>
#include <string.h>
#include "etk_signal.h"
#include "etk_property.h"

/**
 * @addtogroup Etk_Type
 * @{
 */

static void _etk_type_type_free(void *value);
static void _etk_type_property_add_to_list(void *value, void *data);

static Ecore_Hash *_etk_type_types_hash = NULL;

/**************************
 *
 * Implementation
 *
 **************************/

/**
 * @brief Initializes the type system
 * @return Returns TRUE on success, FALSE on failure
 * @warning Shouldn't be called manually, etk_init() calls it
 */
Etk_Bool etk_type_init()
{
   if (!_etk_type_types_hash)
   {
      _etk_type_types_hash = ecore_hash_new(ecore_str_hash, ecore_str_compare);
      ecore_hash_set_free_value(_etk_type_types_hash, _etk_type_type_free);
   }
   return TRUE;
}

/**
 * @brief Deletes all the created types
 * @warning Shouldn't be called manually, etk_shutdown() calls it
 */
void etk_type_shutdown()
{
   if (_etk_type_types_hash)
      ecore_hash_destroy(_etk_type_types_hash);
}

/**
 * @brief Creates a new type. Will be automatically deleted with the etk_type_shutdown() call
 * @param type_name the name of the new type
 * @param parent_type the type whom the new type inherits from (NULL if the new type is a root type)
 * @param type_size the size of an instance
 * @param constructor the constructor function
 * @param destructor the destructor function
 * @return Returns the new type on success, NULL on failure
 */
Etk_Type *etk_type_new(const char *type_name, Etk_Type *parent_type, int type_size,
   Etk_Constructor constructor, Etk_Destructor destructor)
{
   Etk_Type *new_type;

   if (!type_name || !_etk_type_types_hash)
      return NULL;

   new_type = malloc(sizeof(Etk_Type));
   new_type->name = strdup(type_name);
   new_type->type_size = type_size;
   new_type->constructor = constructor;
   new_type->destructor = destructor;
   new_type->property_set = NULL;
   new_type->property_get = NULL;
   new_type->signals_hash = ecore_hash_new(ecore_str_hash, ecore_str_compare);
   new_type->properties_hash = ecore_hash_new(ecore_str_hash, ecore_str_compare);
   ecore_hash_set_free_value(new_type->properties_hash, ECORE_FREE_CB(etk_property_delete));

   if (!parent_type)
   {
      new_type->hierarchy_depth = 0;
      new_type->hierarchy = NULL;
   }
   /* Build the type hierarchy */
   else
   {
      int i;

      new_type->hierarchy_depth = parent_type->hierarchy_depth + 1;
      new_type->hierarchy = malloc(sizeof(Etk_Type *) * new_type->hierarchy_depth);
      new_type->hierarchy[0] = parent_type;
      for (i = 1; i < new_type->hierarchy_depth; i++)
         new_type->hierarchy[i] = parent_type->hierarchy[i - 1];
   }

   ecore_hash_set(_etk_type_types_hash, new_type->name, new_type);

   return new_type;
}

/**
 * @brief Deletes the type
 * @param type the type to delete
 * @warning Use it at your own risk you can safely call it only if all the objects of derivating type are already destroyed
 */
void etk_type_delete(Etk_Type *type)
{
   if (!type)
      return;

   if (_etk_type_types_hash)
      ecore_hash_remove(_etk_type_types_hash, type->name);
}

/**
 * @brief Calls the constructors of the object, from the constructor of the root parent type up to the one of the object type
 * @param type the type of the object
 * @param object the object that will be passed as argument to the constructor
 */
void etk_type_object_construct(Etk_Type *type, Etk_Object *object)
{
   Ecore_List *properties;
   Etk_Property *property;
   int i;

   if (!type || !object)
      return;

   /* We first call the constructors */
   for (i = type->hierarchy_depth - 1; i >= 0; i--)
   {
      if (type->hierarchy[i]->constructor)
         type->hierarchy[i]->constructor(object);
   }
   if (type->constructor)
      type->constructor(object);

   /* Then we set the default property values */
   for (i = type->hierarchy_depth - 1; i >= 0; i--)
   {
      if (!type->hierarchy[i]->property_set)
         continue;

      properties = ecore_list_new();
      etk_type_property_list(type->hierarchy[i], properties);
      ecore_list_goto_first(properties);
      while ((property = ecore_list_next(properties)))
      {
         if (property->default_value && (property->flags & ETK_PROPERTY_CONSTRUCT))
            type->hierarchy[i]->property_set(object, property->id, property->default_value);
      }
      ecore_list_destroy(properties);
   }
   if (type->property_set)
   {
      properties = ecore_list_new();
      etk_type_property_list(type, properties);
      ecore_list_goto_first(properties);
      while ((property = ecore_list_next(properties)))
      {
         if (property->default_value && (property->flags & ETK_PROPERTY_CONSTRUCT))
            type->property_set(object, property->id, property->default_value);
      }
      ecore_list_destroy(properties);
   }
}

/**
 * @brief Calls the destructors of the object, from the destructor of the object type up to the one of the root parent type
 * @param type the type of the object
 * @param object the object that will be passed as argument to the destructor
 */
void etk_type_destructors_call(Etk_Type *type, Etk_Object *object)
{
   int i;

   if (!type || !object)
      return;

   if (type->destructor)
      type->destructor(object);
   for (i = 0; i < type->hierarchy_depth; i++)
   {
      if (type->hierarchy[i]->destructor)
         type->hierarchy[i]->destructor(object);
   }
}

/**
 * @brief Checks if the type inherits from the parent type
 * @param type the type we check on
 * @param parent the parent type we check on
 * @return Returns TRUE if @a type inerits from @a parent, FALSE otherwise
 */
Etk_Bool etk_type_inherits_from(Etk_Type *type, Etk_Type *parent)
{
   int i;

   if (!type || !parent)
      return FALSE;

   if (type == parent)
      return TRUE;

   for (i = 0; i < type->hierarchy_depth; i++)
   {
      if (type->hierarchy[i] == parent)
         return TRUE;
   }

   return FALSE;
}

/**
 * @brief Gets the parent type of the type @a type
 * @param type the type we want the parent type
 * @return Returns the parent type of the type @a type or NULL if @a type doesn't inherit from another type
 */
Etk_Type *etk_type_parent_type_get(Etk_Type *type)
{
   if (!type || !type->hierarchy)
      return NULL;

   return type->hierarchy[0];
}

/**
 * @brief Gets the name of the type
 * @param type a type
 * @return Returns the name of the type, or NULL on failure
 */
const char *etk_type_name_get(Etk_Type *type)
{
   if (!type)
      return NULL;
   return type->name;
}

/**
 * @brief Adds a signal associated to the type
 * @param type the type to add the signal to
 * @param signal the signal to add
 */
void etk_type_signal_add(Etk_Type *type, Etk_Signal *signal)
{
   const char *signal_name;

   if (!type || !signal || !(signal_name = etk_signal_name_get(signal)))
      return;

   ecore_hash_set(type->signals_hash, signal_name, signal);
}

/**
 * @brief Removes the signal from the list of signals of the type
 * @param type the type that is associated to the signal to remove
 * @param signal the signal to remove
 */
void etk_type_signal_remove(Etk_Type *type, Etk_Signal *signal)
{
   const char *signal_name;

   if (!type || !signal || !(signal_name = etk_signal_name_get(signal)))
      return;

   ecore_hash_remove(type->signals_hash, signal_name);
}

/**
 * @brief Gets the signal corresponding to the type and the signal name
 * @param type the type that is associated to the signal to get
 * @param signal_name the name of the signal to get
 * @return Returns the signal corresponding to the type and the signal name, or NULL on failure
 */
Etk_Signal *etk_type_signal_get(Etk_Type *type, const char *signal_name)
{
   if (!type || !signal_name)
      return NULL;

   return ecore_hash_get(type->signals_hash, signal_name);
}

/**
 * @brief Adds a new property to the type
 * @param type the type to add the property to
 * @param name the name of the new property
 * @param property_id the id you want to associate to the property
 * @param property_type the type of the property
 * @param flags describes if the property value should be writable (ETK_PROPERTY_WRITABLE), readable (ETK_PROPERTY_READABLE) or both (ETK_PROPERTY_WRITABLE | ETK_PROPERTY_READABLE)
 * @param default_value the default value of the property
 * @return Returns the new property on success, or NULL on failure
 */
Etk_Property *etk_type_property_add(Etk_Type *type, const char *name, int property_id, Etk_Property_Type property_type, Etk_Property_Flags flags, Etk_Property_Value *default_value)
{
   Etk_Property *new_property;

   if (!type)
      return NULL;

   new_property = etk_property_new(name, property_id, property_type, flags, default_value);
   ecore_hash_set(type->properties_hash, name, new_property);

   return new_property;
}

/**
 * @brief Finds the property called @a name in the type @a type
 * @param type the type that has the property
 * @param name the name of the property to find
 * @param property_owner the location where the type that has the property should be return (it can be a child type of @a type)
 * @param property the location where the property should be return
 * @return Returns TRUE if the property has been found, FALSE on failure (if the type doesn't have a property of that name)
 */
Etk_Bool etk_type_property_find(Etk_Type *type, const char *name, Etk_Type **property_owner, Etk_Property **property)
{
   Etk_Type *t;

   if (!type || !name || !property)
      return FALSE;

   for (t = type; t; t = etk_type_parent_type_get(t))
   {
      if ((*property = ecore_hash_get(t->properties_hash, name)))
      {
         if (property_owner)
            *property_owner = t;
         return TRUE;
      }
   }
   return FALSE;
}

/**
 * @brief Lists the properties of the type and appends them to @a properties
 * @param type the type that has the properties to list
 * @param properties the list where the properties will be put
 */
void etk_type_property_list(Etk_Type *type, Ecore_List *properties)
{
   if (!type || !properties)
      return;

   ecore_hash_for_each_node(type->properties_hash, _etk_type_property_add_to_list, properties);
}

/**************************
 *
 * Private functions
 *
 **************************/

/* Frees the type (called when it's removed from the types hash table) */
static void _etk_type_type_free(void *value)
{
   Etk_Type *type;

   if (!(type = value))
      return;

   ecore_hash_destroy(type->signals_hash);
   ecore_hash_destroy(type->properties_hash);
   free(type->hierarchy);
   free(type->name);
   free(type);
}

/* Used by etk_type_property_list() */
static void _etk_type_property_add_to_list(void *value, void *data)
{
   Ecore_List *properties;
   Ecore_Hash_Node *node;

   if (!(node = value) || !(properties = data))
      return;

   ecore_list_append(properties, node->value);
}

/** @} */
