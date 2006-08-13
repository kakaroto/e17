/** @file etk_signal.c */
#include "etk_signal.h"
#include <Evas.h>
#include <stdlib.h>
#include <string.h>
#include "etk_type.h"
#include "etk_object.h"
#include "etk_signal_callback.h"
#include "etk_utils.h"

/**
 * @addtogroup Etk_Signal
 * @{
 */

typedef struct Etk_Signal_Emitted
{
   Etk_Signal *signal;
   Etk_Object *object;
   Etk_Bool stop_emission;
} Etk_Signal_Emitted;

static void _etk_signal_free(Etk_Signal *signal);

static Evas_List *_etk_signal_signals_list = NULL;
static Evas_List *_etk_signal_emitted_signals = NULL;

/**************************
 *
 * Implementation
 *
 **************************/

/**
 * @internal
 * @brief Shutdowns the signal system
 * @warning You must not call it manually, etk_shudown() calls it automatically
 */
void etk_signal_shutdown()
{
   while (_etk_signal_emitted_signals)
   {
      free(_etk_signal_emitted_signals->data);
      _etk_signal_emitted_signals = evas_list_remove_list(_etk_signal_emitted_signals, _etk_signal_emitted_signals);
   }
   
   while (_etk_signal_signals_list)
   {
      _etk_signal_free(_etk_signal_signals_list->data);
      _etk_signal_signals_list = evas_list_remove_list(_etk_signal_signals_list, _etk_signal_signals_list);
   }
}

/**
 * @brief Creates a new signal called @a signal_name, for the object type @a object_type
 * @param signal_name the name of the new signal
 * @param object_type the object type of the new signal
 * @param default_handler_offset the offset of the default handler in the object's struct 
 * (use ETK_MEMBER_OFFSET() to get it). -1 if there is no default handler
 * @param marshaller the marshaller of the signal: it will treat and pass the arguments to the callbacks
 * @param accumulator the accumulator used to combine together the different values returned by the callbacks.
 * Set it to NULL if the callbacks does not return any value or if you only want to keep the last returned value.
 * @param accum_data the value to pass to the accumulator
 * @return Returns the new signal, or NULL on failure
 */
Etk_Signal *etk_signal_new(const char *signal_name, Etk_Type *object_type, long default_handler_offset, Etk_Marshaller marshaller, Etk_Accumulator accumulator, void *accum_data)
{
   Etk_Signal *new_signal;

   if (!signal_name || !object_type || !marshaller)
      return NULL;

   new_signal = malloc(sizeof(Etk_Signal));
   new_signal->name = strdup(signal_name);
   new_signal->object_type = object_type;
   new_signal->default_handler_offset = default_handler_offset;
   new_signal->marshaller = marshaller;
   new_signal->accumulator = accumulator;
   new_signal->accum_data = accum_data;
   etk_type_signal_add(object_type, new_signal);

   _etk_signal_signals_list = evas_list_append(_etk_signal_signals_list, new_signal);

   return new_signal;
}

/**
 * @brief Deletes the signal. The signal could not be connected or emitted anymore
 * @param signal the signal to delete
 */
void etk_signal_delete(Etk_Signal *signal)
{
   Evas_List *l;
   
   if (!signal)
      return;
   
   if ((l = evas_list_find_list(_etk_signal_signals_list, signal)))
   {
      _etk_signal_free(l->data);
      _etk_signal_signals_list = evas_list_remove_list(_etk_signal_signals_list, l);
   }
}

/**
 * @brief Gets the the signal corresponding to the name and the object type
 * @param signal_name the name of the signal to return
 * @param type the object type of the signal to return
 * @return Returns the signal called @a signal_name, for the object type @a type, or NULL on failure
 */
Etk_Signal *etk_signal_lookup(const char *signal_name, Etk_Type *type)
{
   Etk_Type *t;
   Etk_Signal *signal;

   if (!signal_name)
      return NULL;

   for (t = type; t; t = etk_type_parent_type_get(t))
   {
      if ((signal = etk_type_signal_get(t, signal_name)))
         return signal;
   }
   return NULL;
}

/**
 * @brief Gets the name of the signal
 * @param signal a signal
 * @return Returns the name of the signal, or NULL on failure
 */
const char *etk_signal_name_get(Etk_Signal *signal)
{
   return signal ? signal->name : NULL;
}

/**
 * @brief Connects a callback to a signal of the object @a object. When the signal of the object will be emitted, this
 * callback will be automatically called
 * @param signal the signal to connect to the callback
 * @param object the object to connect to the callback
 * @param callback the callback to call when the signal is emitted
 * @param data the data to pass to the callback
 * @param swapped if @a swapped == ETK_TRUE, the callback will be called with @a data as the only argument.
 * It can be useful to set it to ETK_TRUE if you just want to call one function on an object when the signal is emitted
 * @param after if @a after == ETK_TRUE, the callback will be called after the default handler is called.
 * Otherwise, it will be called before the default handler. It can be useful to set it to ETK_TRUE if you want the
 * callback to be called after all the other callbacks connect to the signal.
 */
void etk_signal_connect_full(Etk_Signal *signal, Etk_Object *object, Etk_Signal_Callback_Function callback, void *data, Etk_Bool swapped, Etk_Bool after)
{
   Etk_Signal_Callback *new_callback;
   if (!object || !signal || !callback)
      return;

   if (!(new_callback = etk_signal_callback_new(signal, callback, data, swapped)))
      return;
   etk_object_signal_callback_add(object, new_callback, after);
}

/**
 * @brief Connects a callback to a signal of the object @a object. When the signal of the object will be emitted, this
 * callback will be automatically called <b>before</b> the default handler
 * @param signal the signal to connect to the callback
 * @param object the object to connect to the callback
 * @param callback the callback to call when the signal is emitted
 * @param data the data to pass to the callback
 */
void etk_signal_connect(const char *signal_name, Etk_Object *object, Etk_Signal_Callback_Function callback, void *data)
{
   Etk_Signal *signal;

   if (!object || !signal_name || !callback)
      return;

   if (!(signal = etk_signal_lookup(signal_name, etk_object_object_type_get(object))))
   {
      ETK_WARNING("Invalid signal connection: the object type \"%s\" doesn't have a signal called \"%s\"",
         object->type->name, signal_name);
      return;
   }

   etk_signal_connect_full(signal, object, callback, data, ETK_FALSE, ETK_FALSE);
}

/**
 * @brief Connects a callback to a signal of the object @a object. When the signal of the object will be emitted, this
 * callback will be automatically called <b>after</b> the default handler
 * @param signal the signal to connect to the callback
 * @param object the object to connect to the callback
 * @param callback the callback to call when the signal is emitted
 * @param data the data to pass to the callback
 */
void etk_signal_connect_after(const char *signal_name, Etk_Object *object, Etk_Signal_Callback_Function callback, void *data)
{
   Etk_Signal *signal;

   if (!object || !signal_name || !callback)
      return;

   if (!(signal = etk_signal_lookup(signal_name, etk_object_object_type_get(object))))
   {
      ETK_WARNING("Invalid signal connection: the object type \"%s\" doesn't have a signal called \"%s\"",
         object->type->name, signal_name);
      return;
   }

   etk_signal_connect_full(signal, object, callback, data, ETK_FALSE, ETK_TRUE);
}

/**
 * @brief Connects a callback to a signal of the object @a object. When the signal of the object will be emitted, this
 * callback will be automatically called <b>vefore</b> the default handler, with @a data as the only argument. @n
 * It can be useful if you just want to call one function on an object when the signal is emitted
 * @param signal_name the name of the signal to connect to the object to
 * @param object the object that will connect the signal
 * @param callback the callback to call when the signal is emitted
 * @param data the data to pass to the callback
 */ 
void etk_signal_connect_swapped(const char *signal_name, Etk_Object *object, Etk_Signal_Callback_Function callback, void *data)
{
   Etk_Signal *signal;

   if (!object || !signal_name || !callback)
      return;

   if (!(signal = etk_signal_lookup(signal_name, etk_object_object_type_get(object))))
   {
      ETK_WARNING("Invalid signal connection: the object type \"%s\" doesn't have a signal called \"%s\"",
         object->type->name, signal_name);
      return;
   }

   etk_signal_connect_full(signal, object, callback, data, ETK_TRUE, ETK_FALSE);
}

/**
 * @brief Disconnects a callback from a signal, the callback won't be called anymore when the signal is emitted
 * @param signal_name the name of the signal connected to the callback to disconnect
 * @param object the object connected to the callback to disconnect
 * @param callback the callback to disconnect
 */
void etk_signal_disconnect(const char *signal_name, Etk_Object *object, Etk_Signal_Callback_Function callback)
{
   Etk_Signal *signal;
   Evas_List *callbacks;
   Etk_Signal_Callback *signal_callback;

   if (!object || !signal_name || !callback)
      return;
   
   if (!(signal = etk_signal_lookup(signal_name, etk_object_object_type_get(object))))
   {
      ETK_WARNING("Invalid signal disconnection: the object type \"%s\" doesn't have a signal called \"%s\"",
         object->type->name, signal_name);
      return;
   }
   
   callbacks = NULL;
   etk_object_signal_callbacks_get(object, signal, &callbacks, ETK_FALSE);
   while (callbacks)
   {
      signal_callback = callbacks->data;
      if (signal_callback->callback == callback)
         etk_object_signal_callback_remove(object, signal_callback);
      callbacks = evas_list_remove_list(callbacks, callbacks);
   }
   
   etk_object_signal_callbacks_get(object, signal, &callbacks, ETK_TRUE);
   while (callbacks)
   {
      signal_callback = callbacks->data;
      if (signal_callback->callback == callback)
         etk_object_signal_callback_remove(object, signal_callback);
      callbacks = evas_list_remove_list(callbacks, callbacks);
   }   
}

/**
 * @brief Emits the signal: it will call the callbacks connected to the signal @a signal
 * @param signal the signal to emit
 * @param object the object which will emit the signal
 * @param return_value the location where store the return value ( @a return_value may be NULL)
 * @param ... the arguments to pass to the callback function
 */
void etk_signal_emit(Etk_Signal *signal, Etk_Object *object, void *return_value, ...)
{
   va_list args;

   if (!object || !signal)
      return;

   va_start(args, return_value);
   etk_signal_emit_valist(signal, object, return_value, args);
   va_end(args);
}

/**
 * @brief Emits the signal: it will call the callbacks connected to the signal @a signal
 * @param signal the name of the signal to emit
 * @param object the object which will emit the signal
 * @param return_value the location where store the return value ( @a return_value may be NULL)
 * @param ... the arguments to pass to the callback function
 */
void etk_signal_emit_by_name(const char *signal_name, Etk_Object *object, void *return_value, ...)
{
   va_list args;
   Etk_Signal *signal;

   if (!object || !signal_name)
      return;

   if (!(signal = etk_signal_lookup(signal_name, etk_object_object_type_get(object))))
   {
      ETK_WARNING("Invalid signal emission: the object type doesn't have a signal called \"%s\"", signal_name);
      return;
   }

   va_start(args, return_value);
   etk_signal_emit_valist(signal, object, return_value, args);
   va_end(args);
}

/**
 * @brief Emits the signal: it will call the callbacks connected to the signal @a signal
 * @param signal the signal to emit
 * @param object the object which will emit the signal
 * @param return_value the location where store the return value ( @a return_value may be NULL)
 * @param args the arguments to pass to the callback function
 * @return Returns @a object, or NULL if @a object has been destroyed by one of the callbacks
 */
void etk_signal_emit_valist(Etk_Signal *signal, Etk_Object *object, void *return_value, va_list args)
{
   Evas_List *callbacks;
   Etk_Signal_Callback *callback;
   Etk_Signal_Emitted *emitted_signal;
   Etk_Bool return_value_set = ETK_FALSE;
   void *result = NULL;
   va_list args2;
   void *object_ptr;

   if (!object || !signal)
      return;

   /* The pointer object will be set to NULL if the object is destroyed by a callback */
   object_ptr = object;
   etk_object_weak_pointer_add(object, &object_ptr);
   
   emitted_signal = malloc(sizeof(Etk_Signal_Emitted));
   emitted_signal->signal = signal;
   emitted_signal->object = object;
   emitted_signal->stop_emission = ETK_FALSE;
   _etk_signal_emitted_signals = evas_list_prepend(_etk_signal_emitted_signals, emitted_signal);
   
   va_copy(args2, args);

   /* We call the callbacks to call before the default handler */
   if (object_ptr)
   {
      callbacks = NULL;
      etk_object_signal_callbacks_get(object, signal, &callbacks, ETK_FALSE);
      while (!emitted_signal->stop_emission && callbacks && object_ptr)
      {
         callback = callbacks->data;
         if (!return_value_set || !signal->accumulator)
         {
            etk_signal_callback_call_valist(callback, object, return_value, args2);
            return_value_set = ETK_TRUE;
         }
         else
         {
            etk_signal_callback_call_valist(callback, object, result, args2);
            signal->accumulator(return_value, result, signal->accum_data);
         }
         callbacks = evas_list_remove_list(callbacks, callbacks);
      }
      callbacks = evas_list_free(callbacks);
   }

   /* Calls the default handler */
   if (object_ptr && !emitted_signal->stop_emission && signal->default_handler_offset >= 0 && signal->marshaller)
   {
      Etk_Signal_Callback_Function *default_handler;

      default_handler = (void *)object + signal->default_handler_offset;
      if (*default_handler)
      {
         if (!return_value_set || !signal->accumulator)
         {
            signal->marshaller(*default_handler, object, NULL, return_value, args2);
            return_value_set = ETK_TRUE;
         }
         else
         {
            signal->marshaller(*default_handler, object, NULL, result, args2);
            signal->accumulator(return_value, result, signal->accum_data);
         }
      }
   }
   
   /* We call the callbacks to call after the default handler */
   if (object_ptr)
   {
      callbacks = NULL;
      etk_object_signal_callbacks_get(object, signal, &callbacks, ETK_TRUE);
      while (!emitted_signal->stop_emission && callbacks && object_ptr)
      {
         callback = callbacks->data;
         if (!return_value_set || !signal->accumulator)
         {
            etk_signal_callback_call_valist(callback, object, return_value, args2);
            return_value_set = ETK_TRUE;
         }
         else
         {
            etk_signal_callback_call_valist(callback, object, result, args2);
            signal->accumulator(return_value, result, signal->accum_data);
         }
         callbacks = evas_list_remove_list(callbacks, callbacks);
      }
      callbacks = evas_list_free(callbacks);
   }
   
   if (object_ptr)
      etk_object_weak_pointer_remove(object, &object_ptr);
   _etk_signal_emitted_signals = evas_list_remove_list(_etk_signal_emitted_signals, _etk_signal_emitted_signals);
   free(emitted_signal);
   
   va_end(args2);
}

/**
 * @brief Gets the marshaller used by the signal
 * @param signal a signal
 * @return Returns the marshaller used by the signal or NULL on failure
 */
Etk_Marshaller etk_signal_marshaller_get(Etk_Signal *signal)
{
   if (!signal)
      return NULL;
   return signal->marshaller;
}

/**
 * @brief Stops the propagation of the last emitted signal: the remaining callbacks/handler won't be called. @n
 * It's usually called in a callback to avoid the other callbacks to be called.
 * @note It has no effect if no signal is being emitted
 */ 
void etk_signal_stop()
{
   Etk_Signal_Emitted *last_emitted_signal;
   
   if (_etk_signal_emitted_signals)
   {
      last_emitted_signal = _etk_signal_emitted_signals->data;
      last_emitted_signal->stop_emission = ETK_TRUE;
   }
}

/**************************
 *
 * Private functions
 *
 **************************/

/* Frees the signal */
static void _etk_signal_free(Etk_Signal *signal)
{
   if (!signal)
      return;

   if (signal->object_type)
      etk_type_signal_remove(signal->object_type, signal);
   free(signal->name);
   free(signal);
}

/** @} */

/**************************
 *
 * Documentation
 *
 **************************/

/**
 * @addtogroup Etk_Signal
 *
 * TODO: write doc for Etk_Signal!!
 */
