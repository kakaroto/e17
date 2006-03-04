/** @file etk_object.c */
#include "etk_object.h"
#include <stdlib.h>
#include <string.h>
#include "etk_type.h"
#include "etk_signal.h"
#include "etk_signal_callback.h"
#include "etk_utils.h"
#include "etk_property.h"

/**
 * @addtogroup Etk_Object
 * @{
 */

typedef struct _Etk_Object_Data
{
   void *value;
   void (*free_cb)(void *data);
} Etk_Object_Data;

enum _Etk_Object_Signal_Id
{
   ETK_OBJECT_DESTROYED_SIGNAL,
   ETK_OBJECT_NUM_SIGNALS
};

static void _etk_object_constructor(Etk_Object *object);
static void _etk_object_destructor(Etk_Object *object);
static Evas_Bool _etk_object_notification_callbacks_free_cb(Evas_Hash *hash, const char *key, void *data, void *fdata);
static Evas_Bool _etk_object_data_free_cb(Evas_Hash *hash, const char *key, void *data, void *fdata);

static Evas_List *_etk_object_created_objects = NULL;
static Etk_Signal *_etk_object_signals[ETK_OBJECT_NUM_SIGNALS];

/**************************
 *
 * Implementation
 *
 **************************/

/**
 * @brief Gets the type of an Etk_Object
 * @return Returns the type on an Etk_Object
 */
Etk_Type *etk_object_type_get()
{
   static Etk_Type *object_type = NULL;

   if (!object_type)
   {
      object_type = etk_type_new("Etk_Object", NULL, sizeof(Etk_Object), ETK_CONSTRUCTOR(_etk_object_constructor), ETK_DESTRUCTOR(_etk_object_destructor));

      _etk_object_signals[ETK_OBJECT_DESTROYED_SIGNAL] = etk_signal_new("destroyed", object_type, -1, etk_marshaller_VOID__VOID, NULL, NULL);
   }

   return object_type;
}

/**
 * @brief Creates a new object and call the constructors then set the properties, according to the type of the object
 * @param object_type the type of the object to create
 * @param first_property the name of the first property value
 * @param ... the value of the first argument, followed by any number of name/argument-value pairs, terminated with NULL
 * @return Returns the new Etk_Object of type object_type
 */
Etk_Object *etk_object_new(Etk_Type *object_type, const char *first_property, ...)
{
   Etk_Object *new_object;
   va_list args;

   if (!object_type)
      return NULL;

   va_start(args, first_property);
   new_object = etk_object_new_valist(object_type, first_property, args);
   va_end(args);

   return new_object;
}

/**
 * @brief Creates a new object and call the constructors then set the properties, according to the type of the object
 * @param object_type the type of the object to create
 * @param first_property the name of the first property value
 * @param args the value of the first argument, followed by any number of name/argument-value pairs, terminated with NULL
 * @return Returns the new Etk_Object of type object_type
 */
Etk_Object *etk_object_new_valist(Etk_Type *object_type, const char *first_property, va_list args)
{
   Etk_Object *new_object;
   va_list args2;

   if (!object_type)
      return NULL;

   new_object = malloc(object_type->type_size);
   new_object->type = object_type;
   _etk_object_created_objects = evas_list_append(_etk_object_created_objects, new_object);
   
   etk_type_object_construct(object_type, new_object);
   va_copy(args2, args);
   etk_object_properties_set_valist(new_object, first_property, args2);
   va_end(args2);

   return new_object;
}

/**
 * @brief Destroys the object: emits the "destroyed" signal, sets the weak pointers to NULL, calls the destructors and frees the object
 * @param object the object to destroy
 */
void etk_object_destroy(Etk_Object *object)
{
   if (!object)
      return;

   etk_signal_emit(_etk_object_signals[ETK_OBJECT_DESTROYED_SIGNAL], object, NULL);
   etk_type_destructors_call(object->type, object);
   _etk_object_created_objects = evas_list_remove(_etk_object_created_objects, object);
   free(object);
}

/**
 * @brief Destroys all the created objects
 */
void etk_object_destroy_all_objects()
{
   while (_etk_object_created_objects)
      etk_object_destroy(ETK_OBJECT(_etk_object_created_objects->data));
}

/**
 * @brief Checks if @a object can be cast to  @a type @n
 * If it can't be, we display a message in the console but we return it anyway (should we?)
 * @param object the object to cast
 * @param type the which we cast the object to
 * @return Returns the object
 */
Etk_Object *etk_object_check_cast(Etk_Object *object, Etk_Type *type)
{
   if (!object)
      return NULL;

   if (!etk_type_inherits_from(object->type, type))
      ETK_WARNING("Invalid cast from \"%s\" to \"%s\"", etk_type_name_get(object->type), etk_type_name_get(type));

   return object;
}

/**
 * @brief Gets the type of the object
 * @param object the object whose type is returned
 * @return Returns the type of the object @a object (NULL on failure)
 */
Etk_Type *etk_object_object_type_get(Etk_Object *object)
{
   if (!object)
      return NULL;

   return object->type;
}

/**
 * @brief Adds @a signal_callback to the list of signal callbacks of the object
 * @param object the object to add the signal callback to
 * @param signal_callback the signal callback to add
 * @param after if @a after == ETK_TRUE, the callback will be called after the default handler, otherwise, it will be called before
 */
void etk_object_signal_callback_add(Etk_Object *object, Etk_Signal_Callback *signal_callback, Etk_Bool after)
{
   if (!object || !signal_callback)
      return;

   if (after)
      object->after_signal_callbacks_list = evas_list_append(object->after_signal_callbacks_list, signal_callback);
   else
      object->before_signal_callbacks_list = evas_list_append(object->before_signal_callbacks_list, signal_callback);
}

/**
 * @brief Removes @a signal_callback from the list of signal callbacks of the object, and frees it
 * @param object the object to remove the signal callback from
 * @param signal_callback the signal callback to remove
 */
void etk_object_signal_callback_remove(Etk_Object *object, Etk_Signal_Callback *signal_callback)
{
   Evas_List *l;
   
   if (!object || !signal_callback)
      return;
   
   if ((l = evas_list_find_list(object->before_signal_callbacks_list, signal_callback)))
   {
      etk_signal_callback_del(l->data);
      object->before_signal_callbacks_list = evas_list_remove_list(object->before_signal_callbacks_list, l);
   }
   if ((l = evas_list_find_list(object->after_signal_callbacks_list, signal_callback)))
   {
      etk_signal_callback_del(l->data);
      object->after_signal_callbacks_list = evas_list_remove_list(object->after_signal_callbacks_list, l);
   }
}

/**
 * @brief Adds a weak pointer to the object. The pointer will be set to NULL when the object is destroyed
 * @param object an object
 * @param pointer_location the location of the weak pointer to add
 */
void etk_object_weak_pointer_add(Etk_Object *object, void **pointer_location)
{
   if (!object || !pointer_location)
      return;
   object->weak_pointers_list = evas_list_append(object->weak_pointers_list, pointer_location);
}

/**
 * @brief Removes a weak pointer from the object
 * @param object an object
 * @param pointer_location the location of the weak pointer to remove
 */
void etk_object_weak_pointer_remove(Etk_Object *object, void **pointer_location)
{
   if (!object || !pointer_location)
      return;
   object->weak_pointers_list = evas_list_remove(object->weak_pointers_list, pointer_location);
}

/**
 * @brief Append the signal callbacks associated to the object @a object and the signal @a signal to the list @a callbacks
 * @param object the object associated to the signal
 * @param signal the signal which we want the callbacks
 * @param callbacks the location of the list where the callbacks will be appended
 * @param after if @a after == ETK_TRUE, it appends the callbacks to call after the default handler. @n
 * Otherwise, it appends the callbacks called before the default handler
 */
void etk_object_signal_callbacks_get(Etk_Object *object, Etk_Signal *signal, Evas_List **callbacks, Etk_Bool after)
{
   Evas_List *l;
   Etk_Signal_Callback *callback;

   if (!object || !signal || !callbacks)
      return;

   l = after ? object->after_signal_callbacks_list : object->before_signal_callbacks_list;
   for ( ; l; l = l->next)
   {
      callback = l->data;
      if (callback->signal == signal)
         *callbacks = evas_list_append(*callbacks, callback);
   }
}

/**
 * @brief Sets data associated to a key for the object
 * @param object the object to add the data to
 * @param key the key associated to the data
 * @param value the value of the data
 */
void etk_object_data_set(Etk_Object *object, const char *key, void *value)
{
   etk_object_data_set_full(object, key, value, NULL);
}

/**
 * @brief Sets data associated to a key for the object
 * @param object the object to add the data to
 * @param key the key associated to the data
 * @param free_cb the function to call on the data when the object is destroyed
 * @param value the value of the data
 */
void etk_object_data_set_full(Etk_Object *object, const char *key, void *value, void (*free_cb)(void *data))
{
   Etk_Object_Data *data;

   if (!object || !key)
      return;

   if ((data = evas_hash_find(object->data_hash, key)))
   {
      if (data->free_cb)
         data->free_cb(data->value);
      object->data_hash = evas_hash_del(object->data_hash, key, NULL);
      free(data);
   }
   
   data = malloc(sizeof(Etk_Object_Data));
   data->value = value;
   data->free_cb = free_cb;
   object->data_hash = evas_hash_add(object->data_hash, key, data);
}

/**
 * @brief Gets the data associated to the key
 * @param object the object to get the data from
 * @param key the key associated to the data
 * @return Returns the value of the data, NULL on failure
 */
void *etk_object_data_get(Etk_Object *object, const char *key)
{
   Etk_Object_Data *data;
   
   if (!object || !key || !(data = evas_hash_find(object->data_hash, key)))
      return NULL;
   return data->value;
}

/**
 * @brief Reset the default value of a property
 * @param object the object that has the property
 * @param property_name the name of the property to reset
 */
void etk_object_property_reset(Etk_Object *object, const char *property_name)
{
   Etk_Type *type;
   Etk_Property *property;

   if (!object || !property_name)
      return;

   if (etk_type_property_find(object->type, property_name, &type, &property))
   {
      if (type->property_set)
         type->property_set(object, property->id, property->default_value);
   }
}

/**
 * @brief Sets several property values
 * @param object the object that have the property values
 * @param first_property the name of the first property value
 * @param ... the value of the first argument, followed by any number of name/value pairs, terminated with NULL
 */
void etk_object_properties_set(Etk_Object *object, const char *first_property, ...)
{
   va_list args;

   if (!object || !first_property)
      return;

   va_start(args, first_property);
   etk_object_properties_set_valist(object, first_property, args);
   va_end(args);
}

/**
 * @brief Sets several property values
 * @param object the object that have the property values
 * @param first_property the name of the first property value
 * @param args the value of the first argument, followed by any number of name/value pairs, terminated with NULL
 */
void etk_object_properties_set_valist(Etk_Object *object, const char *first_property, va_list args)
{
   const char *property_name;
   Etk_Type *type;
   Etk_Property *property;
   Etk_Property_Value *property_value;
   va_list args2;

   if (!object)
      return;

   va_copy(args2, args);
   for (property_name = first_property; property_name; property_name = va_arg(args2, const char *))
   {
      if (etk_type_property_find(object->type, property_name, &type, &property))
      {
         if (type->property_set)
         {
            property_value = etk_property_value_create_valist(etk_property_type_get(property), &args2);
            type->property_set(object, property->id, property_value);
            etk_property_value_delete(property_value);
         }
      }
      else
      {
         ETK_WARNING("The object %p of type \"%s\" has no property called \"%s\"",
           object, object->type->name, property_name);
         break;
      }
   }
   va_end(args2);
}

/**
 * @brief Gets several property values
 * @param object the object that have the property values
 * @param first_property the name of the first property value
 * @param ... the value of the first argument, followed by any number of name/value pairs, terminated with NULL
 */
void etk_object_properties_get(Etk_Object *object, const char *first_property, ...)
{
   va_list args;

   if (!object || !first_property)
      return;

   va_start(args, first_property);
   etk_object_properties_get_valist(object, first_property, args);
   va_end(args);
}

/**
 * @brief Gets several property values
 * @param object the object that have the property values
 * @param first_property the name of the first property value
 * @param args the value of the first argument, followed by any number of name/value pairs, terminated with NULL
 */
void etk_object_properties_get_valist(Etk_Object *object, const char *first_property, va_list args)
{
   void *value_location;
   const char *property_name;
   Etk_Type *type;
   Etk_Property *property;
   Etk_Property_Value *property_value;

   if (!object)
      return;

   property_value = etk_property_value_new();
   for (property_name = first_property; property_name; property_name = va_arg(args, const char *))
   {
      if (etk_type_property_find(object->type, property_name, &type, &property))
      {
         if (type->property_get)
         {
            type->property_get(object, property->id, property_value);
            
            value_location = va_arg(args, void *);
            etk_property_value_get(property_value, etk_property_value_type_get(property_value), value_location);
         }
      }
      else
      {
         ETK_WARNING("The object %p of type \"%s\" has no property called \"%s\"",
            object, object->type->name, property_name);
         break;
      }
   }
   etk_property_value_delete(property_value);
}

/**
 * @brief Calls the notification callbacks associated to the object and the property @n
 * It should be called each time the property value is changed
 * @param object an object
 * @param property_name the name of the property
 */
void etk_object_notify(Etk_Object *object, const char *property_name)
{
   Evas_List *l;
   Evas_List **notification_callbacks;
   Etk_Notification_Callback *callback;

   if (!object || !property_name)
      return;
   if (!(notification_callbacks = evas_hash_find(object->notification_callbacks_hash, property_name)))
      return;

   for (l = *notification_callbacks; l; l = l->next)
   {
      callback = l->data;
      if (callback->callback)
         callback->callback(object, property_name, callback->data);
   }
}

/**
 * @brief Adds a notification callback associated to the object and the property @n
 * It will be called each time the property value is changed
 * @param object an object
 * @param property_name the name of the property
 * @param callback the callback function
 * @param data the data to pass to the callback
 */
void etk_object_notification_callback_add(Etk_Object *object, const char *property_name, Etk_Notification_Callback_Function callback, void *data)
{
   Evas_List **list;
   Etk_Notification_Callback *new_callback;

   if (!object || !property_name || !callback)
      return;

   if (!(list = evas_hash_find(object->notification_callbacks_hash, property_name)))
   {
      list = malloc(sizeof(Evas_List *));
      *list = NULL;
      object->notification_callbacks_hash = evas_hash_add(object->notification_callbacks_hash, property_name, list);
   }

   new_callback = malloc(sizeof(Etk_Notification_Callback));
   new_callback->callback = callback;
   new_callback->data = data;
   *list = evas_list_append(*list, new_callback);
}

/**
 * @brief Removes a notification callback associated to the object and the property
 * @param object an object
 * @param property_name the name of the property
 * @param callback the callback function
 */
void etk_object_notification_callback_remove(Etk_Object *object, const char *property_name, Etk_Notification_Callback_Function callback)
{
   Evas_List *l;
   Evas_List **list;
   Etk_Notification_Callback *remove_callback;

   if (!object || !property_name || !callback)
      return;
   if (!(list = evas_hash_find(object->notification_callbacks_hash, property_name)))
      return;

   for (l = *list; l; )
   {
      remove_callback = l->data;
      l = l->next;
      if (remove_callback->callback == callback)
      {
         free(remove_callback);
         *list = evas_list_remove_list(*list, l);
      }
   }
}

/**************************
 *
 * Etk specific functions
 *
 **************************/

/* Initializes the members of the object */
static void _etk_object_constructor(Etk_Object *object)
{
   if (!object)
      return;

   object->data_hash = NULL;
   object->before_signal_callbacks_list = NULL;
   object->after_signal_callbacks_list = NULL;
   object->notification_callbacks_hash = NULL;
   object->weak_pointers_list = NULL;
}

/* Destroys the object */
static void _etk_object_destructor(Etk_Object *object)
{
   void **weak_pointer;
   
   if (!object)
      return;

   while (object->weak_pointers_list)
   {
      weak_pointer = object->weak_pointers_list->data;
      *weak_pointer =  NULL;
      object->weak_pointers_list = evas_list_remove_list(object->weak_pointers_list, object->weak_pointers_list);
   }
   
   evas_hash_foreach(object->data_hash, _etk_object_data_free_cb, NULL);
   evas_hash_free(object->data_hash);
   
   while (object->before_signal_callbacks_list)
   {
      etk_signal_callback_del(object->before_signal_callbacks_list->data);
      object->before_signal_callbacks_list = evas_list_remove_list(object->before_signal_callbacks_list, object->before_signal_callbacks_list);
   }
   while (object->after_signal_callbacks_list)
   {
      etk_signal_callback_del(object->after_signal_callbacks_list->data);
      object->after_signal_callbacks_list = evas_list_remove_list(object->after_signal_callbacks_list, object->after_signal_callbacks_list);
   }
   
   evas_hash_foreach(object->notification_callbacks_hash, _etk_object_notification_callbacks_free_cb, NULL);
   evas_hash_free(object->notification_callbacks_hash);
}

/**************************
 *
 * Private functions
 *
 **************************/

/* Frees a list of notification callbacks (called by _etk_object_destructor) */
static Evas_Bool _etk_object_notification_callbacks_free_cb(Evas_Hash *hash, const char *key, void *data, void *fdata)
{
   Evas_List **list;
   
   if (!(list = data))
      return 1;
   
   while (*list)
   {
      free((*list)->data);
      *list = evas_list_remove_list(*list, *list);
   }
   free(list);
   
   return 1;
}

/* Frees data from the data hash of the object */
static Evas_Bool _etk_object_data_free_cb(Evas_Hash *hash, const char *key, void *data, void *fdata)
{
   Etk_Object_Data *object_data;
   
   if (!(object_data = data))
      return 1;
   
   if (object_data->free_cb)
      object_data->free_cb(object_data->value);
   free(object_data);
   
   return 1;
}

/** @} */
