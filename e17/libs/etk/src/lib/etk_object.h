/** @file etk_object.h */
#ifndef _ETK_OBJECT_H_
#define _ETK_OBJECT_H_

#include <Evas.h>
#include <stdarg.h>
#include "etk_type.h"
#include "etk_types.h"

/**
 * @defgroup Etk_Object Etk_Object
 * @brief The Etk_Object class is the base class of all the objects and the widgets of Etk
 * @{
 */

#ifndef ETK_DISABLE_CAST_CHECKS
   #define ETK_OBJECT_CAST(obj, etk_type, c_type)     ((c_type *)etk_object_check_cast((Etk_Object *)(obj), (etk_type)))
#else
   #define ETK_OBJECT_CAST(obj, etk_type, c_type)     ((c_type *)(obj))
#endif
#define ETK_OBJECT_CHECK_TYPE(obj, etk_type)          (etk_type_inherits_from(((Etk_Object *)(obj))->type, (etk_type)))

/** Gets the type of an object */
#define ETK_OBJECT_TYPE       (etk_object_type_get())
/** Casts the object to an Etk_Object */
#define ETK_OBJECT(obj)       (ETK_OBJECT_CAST((obj), ETK_OBJECT_TYPE, Etk_Object))
/** Checks if the object is an Etk_Object */
#define ETK_IS_OBJECT(obj)    (ETK_OBJECT_CHECK_TYPE((obj), ETK_OBJECT_TYPE))

/**
 * @brief A callback called each time the value of the associated property is modified
 * @structinfo
 */
struct Etk_Notification_Callback
{
   /* private: */
   Etk_Notification_Callback_Function callback;
   void *data;
};

/**
 * @brief @widget The structure of an object
 * @structinfo
 */
struct Etk_Object
{
   /* private: */
   Etk_Type *type;
   Evas_Hash *data_hash;
   Evas_List *before_signal_callbacks_list;
   Evas_List *after_signal_callbacks_list;
   Evas_Hash *notification_callbacks_hash;
   Evas_List *weak_pointers_list;
};

Etk_Type *etk_object_type_get();
Etk_Object *etk_object_new(Etk_Type *object_type, const char *first_property, ...);
Etk_Object *etk_object_new_valist(Etk_Type *object_type, const char *first_property, va_list args);
void etk_object_destroy(Etk_Object *object);
void etk_object_destroy_all_objects();

Etk_Object *etk_object_check_cast(Etk_Object *object, Etk_Type *type);
Etk_Type *etk_object_object_type_get(Etk_Object *object);

void etk_object_signal_callback_add(Etk_Object *object, Etk_Signal_Callback *signal_callback, Etk_Bool after);
void etk_object_signal_callback_remove(Etk_Object *object, Etk_Signal_Callback *signal_callback);
void etk_object_signal_callbacks_get(Etk_Object *object, Etk_Signal *signal, Evas_List **callbacks, Etk_Bool after);

void etk_object_weak_pointer_add(Etk_Object *object, void **pointer_location);
void etk_object_weak_pointer_remove(Etk_Object *object, void **pointer_location);

void etk_object_data_set(Etk_Object *object, const char *key, void *value);
void etk_object_data_set_full(Etk_Object *object, const char *key, void *value, void (*free_cb)(void *data));
void *etk_object_data_get(Etk_Object *object, const char *key);

void etk_object_property_reset(Etk_Object *object, const char *property_name);
void etk_object_properties_set(Etk_Object *object, const char *first_property, ...);
void etk_object_properties_set_valist(Etk_Object *object, const char *first_property, va_list args);
void etk_object_properties_get(Etk_Object *object, const char *first_property, ...);
void etk_object_properties_get_valist(Etk_Object *object, const char *first_property, va_list args);

void etk_object_notify(Etk_Object *object, const char *property_name);
void etk_object_notification_callback_add(Etk_Object *object, const char *property_name, Etk_Notification_Callback_Function callback, void *data);
void etk_object_notification_callback_remove(Etk_Object *object, const char *property_name, Etk_Notification_Callback_Function callback);

/** @} */

#endif
