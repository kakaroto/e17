/** @file etk_type.c */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "etk_type.h"

#include <stdlib.h>
#include <string.h>

#include "etk_signal.h"
#include "etk_signal_callback.h"
#include "etk_property.h"
#include "etk_object.h"

/**
 * @addtogroup Etk_Type
 * @{
 */

static void _etk_type_free(Etk_Type *type);
static Evas_Bool _etk_type_free_cb(const Evas_Hash *hash, const char *key, void *data, void *fdata);
static Evas_Bool _etk_type_property_free_cb(const Evas_Hash *hash, const char *key, void *data, void *fdata);
static Evas_Bool _etk_type_property_add_to_list(const Evas_Hash *hash, const char *key, void *data, void *fdata);

static Evas_Hash *_etk_type_types_hash = NULL;

/**************************
 *
 * Implementation
 *
 **************************/

/**
 * @brief Deletes all the created types
 * @warning Shouldn't be called manually, etk_shutdown() calls it
 */
void etk_type_shutdown(void)
{
   evas_hash_foreach(_etk_type_types_hash, _etk_type_free_cb, NULL);
   evas_hash_free(_etk_type_types_hash);
   _etk_type_types_hash = NULL;
}

/**
 * @brief Creates a new type. Will be automatically deleted with the etk_type_shutdown() call
 * @param type_name the name of the new type
 * @param parent_type the type whom the new type inherits from (NULL if the new type is a root type)
 * @param type_size the size of an instance
 * @param constructor the constructor function
 * @param destructor the destructor function
 * @param signals array of Etk_Signal_Description that will be used to describe type signals at this level (parent signals will be inherited), this array should be terminated with ETK_SIGNAL_DESCRIPTION_SENTINEL. May be NULL if no signals should be defined.
 * @return Returns the new type on success, NULL on failure
 */
Etk_Type *etk_type_new(const char *type_name,
   Etk_Type *parent_type, int type_size,
   Etk_Constructor constructor, Etk_Destructor destructor,
   const Etk_Signal_Description *signals)
{
   Etk_Type *new_type;
   unsigned int type_signals;

   if (!type_name)
      return NULL;

   new_type = malloc(sizeof(Etk_Type));
   new_type->name = strdup(type_name);
   new_type->type_size = type_size;
   new_type->constructor = constructor;
   new_type->destructor = destructor;
   new_type->property_set = NULL;
   new_type->property_get = NULL;
   new_type->properties_hash = NULL;
   new_type->signals = NULL;

   /* Count how many new signals we have. */
   type_signals = 0;
   if (signals)
   {
      const Etk_Signal_Description *s;
      for (s = signals; s->signal_code_store; s++)
         type_signals++;
   }

   if (!parent_type)
   {
      new_type->hierarchy_depth = 0;
      new_type->hierarchy = NULL;
      new_type->signals_count = 0;
      if (type_signals > 0)
         new_type->signals = (Etk_Signal **) malloc(type_signals * sizeof(Etk_Signal *));
   }
   else
   {
      /* Build the type hierarchy */
      new_type->hierarchy_depth = parent_type->hierarchy_depth + 1;
      new_type->hierarchy = malloc(sizeof(Etk_Type *) * new_type->hierarchy_depth);
      new_type->hierarchy[0] = parent_type;
      memcpy(new_type->hierarchy + 1, parent_type->hierarchy,
	     parent_type->hierarchy_depth * sizeof(Etk_Type *));

      new_type->signals_count = parent_type->signals_count;

      if (parent_type->signals_count > 0 || type_signals > 0)
      {
         new_type->signals = (Etk_Signal **) malloc((type_signals + new_type->signals_count) * sizeof(Etk_Signal *));

         /* Copy parent signals to our array */
         if (parent_type->signals_count > 0)
            memcpy(new_type->signals, parent_type->signals,
               sizeof(Etk_Type *) * parent_type->signals_count);
      }
   }

   /* Add new signals to the type */
   if (signals)
   {
      const Etk_Signal_Description *s;

      for (s = signals; s->signal_code_store; s++)
         etk_signal_new_with_desc(new_type, s);
   }

   _etk_type_types_hash = evas_hash_add(_etk_type_types_hash, new_type->name, new_type);

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

   _etk_type_types_hash = evas_hash_del(_etk_type_types_hash, type->name, NULL);
   _etk_type_free(type);
}

/**
 * @brief Calls the constructors of the object, from the constructor of the root parent type up to the one of the object type
 * @param type the type of the object
 * @param object the object that will be passed as argument to the constructor
 */
void etk_type_object_construct(Etk_Type *type, Etk_Object *object)
{
   Evas_List *properties = NULL;
   Etk_Property *property;
   int i;

   if (!type || !object)
      return;

   /* We first allocate space for signal callbacks lists */
   if (type->signals_count > 0)
      object->signal_callbacks = (Evas_List **) calloc(type->signals_count, sizeof(Evas_List *));

   /* Then call the constructors */
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

      etk_type_property_list(type->hierarchy[i], &properties);
      while (properties)
      {
         property = properties->data;
         if (property->default_value && (property->flags & ETK_PROPERTY_CONSTRUCT))
            type->hierarchy[i]->property_set(object, property->id, property->default_value);
         properties = evas_list_remove_list(properties, properties);
      }
   }
   if (type->property_set)
   {
      etk_type_property_list(type, &properties);
      while (properties)
      {
         property = properties->data;
         if (property->default_value && (property->flags & ETK_PROPERTY_CONSTRUCT))
            type->property_set(object, property->id, property->default_value);
         properties = evas_list_remove_list(properties, properties);
      }
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

   /* Free all the signal callbacks */
   for (i = 0; i < type->signals_count; i++)
   {
      Evas_List *lst;

      for (lst = object->signal_callbacks[i]; lst; lst = lst->next)
         etk_signal_callback_del(lst->data);
      evas_list_free(object->signal_callbacks[i]);
   }

   if (object->signal_callbacks)
      free(object->signal_callbacks);
}

/**
 * @brief Checks if the type inherits from the parent type
 * @param type the type we check on
 * @param parent the parent type we check on
 * @return Returns ETK_TRUE if @a type inerits from @a parent, ETK_FALSE otherwise
 */
Etk_Bool etk_type_inherits_from(Etk_Type *type, Etk_Type *parent)
{
   int i;

   if (!type || !parent)
      return ETK_FALSE;

   if (type == parent)
      return ETK_TRUE;

   for (i = 0; i < type->hierarchy_depth; i++)
   {
      if (type->hierarchy[i] == parent)
         return ETK_TRUE;
   }

   return ETK_FALSE;
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
 * @brief Gets the Etk_Type from the type's name
 * @param name the type's name
 * @return Returns the Etk_Type of the type's name, or NULL on failure
 */
Etk_Type *etk_type_get_from_name(const char *name)
{
   if (!name)
      return NULL;
   return evas_hash_find(_etk_type_types_hash, name);
}

/**
 * @brief Gets the signal corresponding to the type and the signal name
 *
 * @param type the type that is associated to the signal to get
 * @param signal_name the name of the signal to get
 *
 * @return Returns the signal corresponding to the type and the signal name,
 *         or NULL on failure
 */
Etk_Signal *etk_type_signal_get_by_name(Etk_Type *type, const char *signal_name)
{
   unsigned i;

   if (!type || !type->signals || !signal_name)
      return NULL;

   for (i = 0; i < type->signals_count; i++)
     if (strcmp(type->signals[i]->name, signal_name) == 0)
       return type->signals[i];

   return NULL;
}

/**
 * @brief Gets the signal corresponding to the type and the signal code
 *
 * @param type the type that is associated to the signal to get
 * @param signal_code the code of the signal to get
 *
 * @return Returns the signal corresponding to the type and the signal name,
 *         or NULL on failure
 */
Etk_Signal *etk_type_signal_get(Etk_Type *type, int signal_code)
{
   if (!type || signal_code < 0 || signal_code >= type->signals_count)
      return NULL;
   return type->signals[signal_code];
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
   type->properties_hash = evas_hash_add(type->properties_hash, name, new_property);

   return new_property;
}

/**
 * @brief Finds the property called @a name in the type @a type
 * @param type the type that has the property
 * @param name the name of the property to find
 * @param property_owner the location where the type that has the property should be return (it can be a child type of @a type)
 * @param property the location where the property should be return
 * @return Returns ETK_TRUE if the property has been found, ETK_FALSE on failure (if the type doesn't have a property of that name)
 */
Etk_Bool etk_type_property_find(Etk_Type *type, const char *name, Etk_Type **property_owner, Etk_Property **property)
{
   Etk_Type *t;

   if (!type || !name || !property)
      return ETK_FALSE;

   for (t = type; t; t = etk_type_parent_type_get(t))
   {
      if ((*property = evas_hash_find(t->properties_hash, name)))
      {
         if (property_owner)
            *property_owner = t;
         return ETK_TRUE;
      }
   }
   return ETK_FALSE;
}

/**
 * @brief Lists the properties of the type and appends them to @a properties
 * @param type the type that has the properties to list
 * @param properties the location of the list where the properties will be appended
 */
void etk_type_property_list(Etk_Type *type, Evas_List **properties)
{
   if (!type || !properties)
      return;
   evas_hash_foreach(type->properties_hash, _etk_type_property_add_to_list, properties);
}

/**************************
 *
 * Private functions
 *
 **************************/

/* Frees the type (called when it's removed from the hash table) */
static void _etk_type_free(Etk_Type *type)
{
   if (!type)
      return;

   evas_hash_foreach(type->properties_hash, _etk_type_property_free_cb, NULL);
   evas_hash_free(type->properties_hash);

   /* The signals themselves will be freed by etk_signal_shutdown() */
   if (type->signals)
      free(type->signals);

   if (type->hierarchy)
      free(type->hierarchy);
   free(type->name);
   free(type);
}

/* Used by etk_type_shutdown() */
static Evas_Bool _etk_type_free_cb(const Evas_Hash *hash, const char *key, void *data, void *fdata)
{
   _etk_type_free(data);
   return 1;
}

/* Used by _etk_type_free() */
static Evas_Bool _etk_type_property_free_cb(const Evas_Hash *hash, const char *key, void *data, void *fdata)
{
   etk_property_delete(data);
   return 1;
}

/* Used by etk_type_property_list() */
static Evas_Bool _etk_type_property_add_to_list(const Evas_Hash *hash, const char *key, void *data, void *fdata)
{
   Evas_List **properties;

   if (data && (properties = fdata))
      *properties = evas_list_append(*properties, data);
   return 1;
}

/** @} */
