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

typedef struct Etk_Object_Data
{
   void *value;
   void (*free_cb)(void *data);
} Etk_Object_Data;

enum Etk_Object_Signal_Id
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
 * @return Returns the type of an Etk_Object
 */
Etk_Type *etk_object_type_get()
{
   static Etk_Type *object_type = NULL;

   if (!object_type)
   {
      object_type = etk_type_new("Etk_Object", NULL, sizeof(Etk_Object),
         ETK_CONSTRUCTOR(_etk_object_constructor), ETK_DESTRUCTOR(_etk_object_destructor));

      _etk_object_signals[ETK_OBJECT_DESTROYED_SIGNAL] = etk_signal_new("destroyed",
         object_type, -1, etk_marshaller_VOID__VOID, NULL, NULL);
   }

   return object_type;
}

/**
 * @brief Creates a new object: it calls the corresponding constructors (from the constructor of the base class to the
 * constructor of the more derived class) and then sets the values of the given properties
 * @param object_type the type of the object to create
 * @param first_property the name of the first property to set
 * @param ... the value of the first property, followed by any number of property-name/property-value pairs,
 * terminated with NULL
 * @return Returns the new Etk_Object of type @a object_type
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
 * @brief Creates a new object: it calls the corresponding constructors (from the constructor of the base class to the
 * constructor of the more derived class) and then sets the values of the given properties
 * @param object_type the type of the object to create
 * @param first_property the name of the first property to set
 * @param args the value of the first property, followed by any number of property-name/property-value pairs,
 * terminated with NULL
 * @return Returns the new Etk_Object of type @a object_type
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
 * @brief Destroys the object: if first emits the "destroyed" signal, sets the weak pointers to NULL and then
 * calls the destructors (from the destructor of the more derived class to the destructor of the ultimate base class)
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
 * @brief Destroys all the created objects. You do not need to call it manually, etk_shutdown() calls it automatically
 */
void etk_object_destroy_all_objects()
{
   while (_etk_object_created_objects)
      etk_object_destroy(ETK_OBJECT(_etk_object_created_objects->data));
}

/**
 * @brief Checks if @a object can be cast to @a type.
 * If @a object doesn't inherit from @a type, a warning is displayed in the console but the object is returned anyway.
 * @param object the object to cast
 * @param type the type to which we cast the object
 * @return Returns the object
 * @note You usually do not need to call this function, use specific macros instead (ETK_WIDGET() for example)
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
 * @param object an object
 * @return Returns the type of @a object (NULL on failure)
 */
Etk_Type *etk_object_object_type_get(Etk_Object *object)
{
   if (!object)
      return NULL;
   return object->type;
}

/**
 * @brief Adds @a signal_callback to the list of the signal callbacks of the object
 * @param object an object
 * @param signal_callback the signal callback to add
 * @param after if @a after == ETK_TRUE, the callback will be called after the default handler.
 * Otherwise, it will be called before.
 * @note You usually do not need to call this function, use etk_signal_connect() instead.
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
 * @brief Removes @a signal_callback from the list of the signal callbacks of the object
 * @param object an object
 * @param signal_callback the signal callback to remove
 * @note You usually do not need to call this function, use etk_signal_disconnect() instead.
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
 * @brief Adds a weak pointer to the object. A weak pointer is pointer that is automatically set
 * to NULL when the object is destroyed
 * @param object an object
 * @param pointer_location the location of the weak pointer to add
 * @warning if the @a pointer_location is not accessible when the object is destroyed, it may segfaults. So you have
 * to use etk_object_weak_pointer_remove() when @a pointer_location becomes inaccessible
 * @see etk_object_weak_pointer_remove()
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
 * @see etk_object_weak_pointer_add()
 */
void etk_object_weak_pointer_remove(Etk_Object *object, void **pointer_location)
{
   if (!object || !pointer_location)
      return;
   object->weak_pointers_list = evas_list_remove(object->weak_pointers_list, pointer_location);
}

/**
 * @brief Gets the signal callbacks connected to the signal @a signal of the object @a object
 * @param object the object connected to the signal
 * @param signal the signal of which we want the callbacks
 * @param callbacks the location of a list where the signal callbacks will be appended
 * @param after if @a after == ETK_TRUE, it appends only the callbacks that have to be called after the default handler. @n
 * Otherwise, it appends the callbacks called before the default handler
 * @note You usually do not need to call this function manually, it is used by etk_signal_emit()
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
 * @brief Sets the data associated to a key for the object. The data could be retrieved later with etk_object_data_get()
 * @param object the object to which the data will be added
 * @param key the key to associate to the data
 * @param value the value of the data
 * @note If you want the data to be freed when the object is destroyed or when the value is changed,
 * use etk_object_data_set_full() instead.
 */
void etk_object_data_set(Etk_Object *object, const char *key, void *value)
{
   etk_object_data_set_full(object, key, value, NULL);
}

/**
 * @brief Sets the data associated to a key for the object. The data could be retrieved later with etk_object_data_get()
 * @param object the object to which the data will be added
 * @param key the key to associate to the data
 * @param free_cb the function to call on the data when the object is destroyed or when the value is changed
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
 * @param object the object which has the data
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
 * @brief Resets the default value of a property
 * @param object the object that has the property to reset
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
 * @brief Sets the values of several properties
 * @param object the object that has the properties to set
 * @param first_property the name of the first property value
 * @param ... the value of the first property, followed by any number of property-name/property-value pairs,
 * terminated with NULL
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
 * @brief Sets the values of several properties
 * @param object the object that has the properties to set
 * @param first_property the name of the first property value
 * @param args the value of the first property, followed by any number of property-name/property-value pairs,
 * terminated with NULL
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
 * @brief Gets the values of several properties
 * @param object the object that has the properties
 * @param first_property the name of the first property value
 * @param ... the value of the first property, followed by any number of property-name/property-value-location pairs,
 * terminated with NULL
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
 * @brief Gets the values of several properties
 * @param object the object that has the properties
 * @param first_property the name of the first property value
 * @param args the value of the first property, followed by any number of property-name/property-value-location pairs,
 * terminated with NULL
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
 * @brief Calls the notification callbacks associated the property of the object.
 * It should be called each time the value of the property is changed
 * @param object an object
 * @param property_name the name of the property
 * @note This function is mainly used in object implementations, you usually do not need to call it manually
 */
void etk_object_notify(Etk_Object *object, const char *property_name)
{
   Evas_List *l;
   Evas_List **notification_callbacks;
   Etk_Notification_Callback *callback;
   void *object_ptr;

   if (!object || !property_name)
      return;
   if (!(notification_callbacks = evas_hash_find(object->notification_callbacks_hash, property_name)))
      return;

   object_ptr = object;
   etk_object_weak_pointer_add(object, &object_ptr);
   for (l = *notification_callbacks; l; l = l->next)
   {
      callback = l->data;
      if (callback->callback)
         callback->callback(object, property_name, callback->data);
      
      /* If the object has been destroyed by the notification callback, we return */
      if (!object_ptr)
         return;
   }
   etk_object_weak_pointer_remove(object, &object_ptr);
}

/**
 * @brief Adds a notification callback associated to a property of the object.
 * The callback will be called each time the value of the property is changed
 * (each time etk_object_notify(object, property_name) is called).
 * @param object an object
 * @param property_name the name of the property
 * @param callback the callback function
 * @param data the data to pass to the callback
 */
void etk_object_notification_callback_add(Etk_Object *object, const char *property_name, void (*callback)(Etk_Object *object, const char *property_name, void *data), void *data)
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
 * @brief Removes a notification callback associated to a property of the object
 * @param object an object
 * @param property_name the name of the property
 * @param callback the callback function
 */
void etk_object_notification_callback_remove(Etk_Object *object, const char *property_name, void (*callback)(Etk_Object *object, const char *property_name, void *data))
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

/* Initializes the object */
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
      object->before_signal_callbacks_list = evas_list_remove_list(object->before_signal_callbacks_list,
         object->before_signal_callbacks_list);
   }
   while (object->after_signal_callbacks_list)
   {
      etk_signal_callback_del(object->after_signal_callbacks_list->data);
      object->after_signal_callbacks_list = evas_list_remove_list(object->after_signal_callbacks_list,
         object->after_signal_callbacks_list);
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

/**************************
 *
 * Documentation
 *
 **************************/

/**
 * @addtogroup Etk_Object
 * Etk_Object implements advanced features such as inheritance, constructors/destructors, signals and properties. @n @n
 *
 * <hr>
 * <b>Constructors/Destructors:</b> @n
 * A new object can be created with etk_object_new(). For example:
 * @code
 * //Creates a new focusable slider, for the range [1.0 - 3.0] and with the initial value 2.0
 * slider = etk_object_new(ETK_SLIDER_TYPE, "focusable", ETK_TRUE, "lower", 1.0, "upper", 3.0, "value", 2.0, NULL);
 * @endcode
 * The first argument is the type of the object to create, followed by any number of property-name/property-value pairs,
 * and terminated with NULL. @n
 * etk_object_new() automatically calls the corresponding constructors on the object, from the constructor of
 * the base class to the constructor of the more derived class. @n
 *
 * You can also destroy an object with etk_object_destroy(). It sets the weak pointers of the object to NULL
 * (see etk_object_weak_pointer_add()) and then calls the destructors from the destructor of the more derived class
 * to the destructor of the ultimate base class. @n @n
 *
 * <b>Signal concept:</b> @n
 * Each object has a list of signals that can be connected to one or several callbacks. The callbacks connected to
 * a signal will automatically be called when the signal is emitted with etk_signal_emit(). @n
 * You can connect a callback to a signal with etk_signal_connect(). For example:
 * @code
 * //Callback prototype
 * void clicked_cb(Etk_Button *button, void *data);
 *
 * //Connects the callback "clicked_cb()" to the signal "clicked" of the button
 * etk_signal_connect("clicked", ETK_OBJECT(button), ETK_CALLBACK(clicked_cb), user_data);
 * @endcode
 *
 * You can also disconnect a callback from a signal with etk_signal_disconnect(). For instance: 
 * @code
 * //Disconnects the callback "clicked_cb()" from the signal "clicked"
 * etk_signal_disconnect("clicked", ETK_OBJECT(button), ETK_CALLBACK(clicked_cb));
 * @endcode
 *
 * Each object inherits the signals of its parent classes (for instance, an Etk_Button has the signals of Etk_Object,
 * Etk_Widget, Etk_Container, Etk_Bin and Etk_Button).
 * Each object's documentation page has a list of its signals with the associated callback prototype and a short
 * explanation. @n
 * For more information about signals, see the documentation page of Etk_Signal. @n @n
 *
 * <b>Property concept:</b> @n
 * Each object also has a list of properties. Each property has a specific type, a default value, and can be either
 * readable, writable or both. You can set or get the value of a property with etk_object_properties_set() and
 * etk_object_properties_set(). @n
 *
 * Another important point about the property system is that you can add a notification callback to a property. The
 * callback will be called each time the value of the property is changed (i.e. each time etk_object_notify() is called
 * on that property). This can be done with etk_object_notification_callback_add(). For example:
 * @code
 * //Notification callback prototype
 * void value_changed_cb(Etk_Object *object, const char *property_name, void *data);
 *
 * //Adds a notification callback to the property "upper" of the slider.
 * //It will be called when the upper bound of the slider is changed
 * etk_object_notification_callback_add(ETK_OBJECT(slider), "upper", value_changed_cb, user_data);
 * @endcode
 *
 * Each object inherits the properties of its parent classes (for instance, an Etk_Button has the properties of
 * Etk_Object, Etk_Widget, Etk_Container, Etk_Bin and Etk_Button).
 * <hr>
 * @n @n
 * 
 * \par Object Hierarchy:
 * - Etk_Object
 *
 * \par Signals:
 * @signal_name "destroyed": Emitted just before all the destructors of the object are called
 * @signal_cb void callback(Etk_Object *object, void *data)
 * @signal_arg object: the object which is about to be destroyed
 * @signal_data
 */
