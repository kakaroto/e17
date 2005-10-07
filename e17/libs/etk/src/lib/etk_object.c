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

static void _etk_object_constructor(Etk_Object *object);
static void _etk_object_destructor(Etk_Object *object);

static Ecore_List *_etk_object_created_objects = NULL;
static Etk_Signal *_etk_object_signals[ETK_OBJECT_NUM_SIGNALS];

/**************************
 *
 * Implementation
 *
 **************************/

/**
 * @brief Initializes the objects system
 * @return Returns TRUE on success, FALSE on failure
 * @warning Shouldn't be called manually, etk_init() calls it
 */
Etk_Bool etk_object_init()
{
   if (!_etk_object_created_objects)
      _etk_object_created_objects = ecore_list_new();

   return TRUE;
}

/**
 * @brief Deletes the created objects
 * @warning Shouldn't be called manually, etk_shutdown() calls it
 */
void etk_object_shutdown()
{
   if (_etk_object_created_objects)
   {
      etk_object_destroy_all_objects();
      ecore_list_destroy(_etk_object_created_objects);
   }
}

/**
 * @brief Gets the type of an Etk_Object
 * @return Returns the type on an Etk_Object
 */
Etk_Type *etk_object_type_get()
{
   static Etk_Type *object_type = NULL;

   if (!object_type)
   {
      object_type = etk_type_new("Etk_Object", NULL, sizeof(Etk_Object), ETK_CONSTRUCTOR(_etk_object_constructor), ETK_DESTRUCTOR(_etk_object_destructor), NULL);

      _etk_object_signals[ETK_OBJECT_DESTROY_SIGNAL] = etk_signal_new("destroy", object_type, -1, etk_marshaller_VOID__VOID, NULL, NULL);
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

   if (!object_type)
      return NULL;

   new_object = malloc(object_type->type_size);
   new_object->type = object_type;
   etk_type_object_construct(object_type, new_object);
   etk_object_properties_set_valist(new_object, first_property, args);

   return new_object;
}

/**
 * @brief Destroys the object: emits the "destroy" signal, sets the weak pointers to NULL, calls the destructors and frees the object
 * @param object the object to destroy
 */
void etk_object_destroy(Etk_Object *object)
{
   void **weak_pointer;

   if (!object)
      return;

   etk_signal_emit(_etk_object_signals[ETK_OBJECT_DESTROY_SIGNAL], object, NULL);
   ecore_list_goto_first(object->weak_pointers_list);
   while ((weak_pointer = ecore_list_next(object->weak_pointers_list)))
      *weak_pointer = NULL;
   etk_type_destructors_call(object->type, object);
   free(object);
}

/**
 * @brief Destroys all the created objects
 */
void etk_object_destroy_all_objects()
{
   Etk_Object *object;

   if (!_etk_object_created_objects)
      return;

   ecore_list_goto_first(_etk_object_created_objects);
   while ((object = ecore_list_next(_etk_object_created_objects)))
      etk_object_destroy(object);
   ecore_list_clear(_etk_object_created_objects);
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
 * @param after if @a after == TRUE, the callback will be called after the default handler, otherwise, it will be called before
 */
void etk_object_signal_callback_add(Etk_Object *object, Etk_Signal_Callback *signal_callback, Etk_Bool after)
{
   if (!object || !signal_callback)
      return;

   if (after)
      ecore_list_append(object->after_signal_callbacks_list, signal_callback);
   else
      ecore_list_append(object->before_signal_callbacks_list, signal_callback);
}

/**
 * @brief Removes @a signal_callback from the list of signal callbacks of the object, and frees it
 * @param object the object to remove the signal callback from
 * @param signal_callback the signal callback to remove
 */
void etk_object_signal_callback_remove(Etk_Object *object, Etk_Signal_Callback *signal_callback)
{
   if (!object || !signal_callback)
      return;
   
   if (ecore_list_goto(object->before_signal_callbacks_list, signal_callback))
      ecore_list_remove(object->before_signal_callbacks_list);
   if (ecore_list_goto(object->after_signal_callbacks_list, signal_callback))
      ecore_list_remove(object->after_signal_callbacks_list);
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

   ecore_list_append(object->weak_pointers_list, pointer_location);
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

   if (ecore_list_goto(object->weak_pointers_list, pointer_location))
      ecore_list_remove(object->weak_pointers_list);
}

/**
 * @brief Append the signal callbacks associated to the object @a object and the signal @a signal to the list @a callbacks
 * @param object the object associated to the signal
 * @param signal the signal which we want the callbacks
 * @param callbacks the list where the callbacks will be appended
 * @param after if @a after == TRUE, it appends the callbacks to call after the default handler. @n
 * Otherwise, it appends the callbacks called before the default handler
 */
void etk_object_signal_callbacks_get(Etk_Object *object, Etk_Signal *signal, Ecore_List *callbacks, Etk_Bool after)
{
   Ecore_List *list;
   Etk_Signal_Callback *callback;

   if (!object || !signal || !callbacks)
      return;

   if (after)
      list = object->after_signal_callbacks_list;
   else
      list = object->before_signal_callbacks_list;

   ecore_list_goto_first(list);
   while ((callback = ecore_list_next(list)))
   {
      if (callback->signal == signal)
         ecore_list_append(callbacks, callback);
   }
}

/**
 * @brief Sets a datum associated to a key for the object
 * @param object the object to add the datum to
 * @param key the key associated to the datum
 * @param value the value of the datum
 */
void etk_object_data_set(Etk_Object *object, const char *key, void *value)
{
   char *new_key;

   if (!object || !key)
      return;

   new_key = strdup(key);
   ecore_hash_set(object->data_hash, new_key, value);
}

/**
 * @brief Gets the datum associated to the key
 * @param object the object to get the datum from
 * @param key the key associated to the datum
 * @return Returns the value of the datum, NULL on failure
 */
void *etk_object_data_get(Etk_Object *object, const char *key)
{
   if (!object || !key)
      return NULL;

   return ecore_hash_get(object->data_hash, key);
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
						va_end(args2);
         }
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
   Ecore_List *notification_callbacks;
   Etk_Notification_Callback *callback;

   if (!object || !property_name)
      return;

   if (!(notification_callbacks = ecore_hash_get(object->notification_callbacks_hash, property_name)))
      return;

   ecore_list_goto_first(notification_callbacks);
   while ((callback = ecore_list_next(notification_callbacks)))
   {
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
   Ecore_List *list;
   Etk_Notification_Callback *new_callback;

   if (!object || !property_name || !callback)
      return;

   if (!(list = ecore_hash_get(object->notification_callbacks_hash, property_name)))
   {
      list = ecore_list_new();
      ecore_list_set_free_cb(list, free);
      ecore_hash_set(object->notification_callbacks_hash, strdup(property_name), list);
   }

   new_callback = malloc(sizeof(Etk_Notification_Callback));
   new_callback->callback = callback;
   new_callback->data = data;
   ecore_list_append(list, new_callback);
}

/**
 * @brief Removes a notification callback associated to the object and the property
 * @param object an object
 * @param property_name the name of the property
 * @param callback the callback function
 */
void etk_object_notification_callback_remove(Etk_Object *object, const char *property_name, Etk_Notification_Callback_Function callback)
{
   Ecore_List *list;
   Etk_Notification_Callback *remove_callback;

   if (!object || !property_name || !callback)
      return;

   if (!(list = ecore_hash_get(object->notification_callbacks_hash, property_name)))
      return;

   ecore_list_goto_first(list);
   while ((remove_callback = ecore_list_next(list)))
   {
      if (remove_callback->callback == callback)
         ecore_list_remove(list);
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

   object->data_hash = ecore_hash_new(ecore_str_hash, ecore_str_compare);
   ecore_hash_set_free_key(object->data_hash, free);

   object->before_signal_callbacks_list = ecore_list_new();
   ecore_list_set_free_cb(object->before_signal_callbacks_list, ECORE_FREE_CB(etk_signal_callback_delete));
   object->after_signal_callbacks_list = ecore_list_new();
   ecore_list_set_free_cb(object->after_signal_callbacks_list, ECORE_FREE_CB(etk_signal_callback_delete));

   object->notification_callbacks_hash = ecore_hash_new(ecore_str_hash, ecore_str_compare);
   ecore_hash_set_free_key(object->notification_callbacks_hash, free);
   ecore_hash_set_free_value(object->notification_callbacks_hash, ECORE_FREE_CB(ecore_list_destroy));

   object->weak_pointers_list = ecore_list_new();
   
   ecore_list_append(_etk_object_created_objects, object);
}

/* Destroys the object */
static void _etk_object_destructor(Etk_Object *object)
{
   if (!object)
      return;

   ecore_hash_destroy(object->data_hash);
   ecore_list_destroy(object->before_signal_callbacks_list);
   ecore_list_destroy(object->after_signal_callbacks_list);
   ecore_hash_destroy(object->notification_callbacks_hash);
   ecore_list_destroy(object->weak_pointers_list);
   if (ecore_list_goto(_etk_object_created_objects, object))
      ecore_list_remove(_etk_object_created_objects);
}

/** @} */
