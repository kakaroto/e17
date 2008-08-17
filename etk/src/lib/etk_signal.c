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

/** @file etk_signal.c */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "etk_signal.h"

#include <stdlib.h>
#include <string.h>

#include "etk_object.h"
#include "etk_signal_callback.h"
#include "etk_type.h"
#include "etk_utils.h"

/**
 * @addtogroup Etk_Signal
 * @{
 */

static void _etk_signal_free(Etk_Signal *signal);

static Evas_List *_etk_signal_signals_list = NULL;

/**************************
 *
 * Implementation
 *
 **************************/

/**
 * @internal
 * @brief Shutdowns the signal system: it destroys all the created signals
 */
void etk_signal_shutdown(void)
{
   Evas_List *lst;

   for (lst = _etk_signal_signals_list; lst; lst = lst->next)
      _etk_signal_free(lst->data);

   evas_list_free(_etk_signal_signals_list);
}

/**
 * @internal
 * @brief Creates a new signal in the system.
 */
static Etk_Signal *etk_signal_new_raw(const char *signal_name, Etk_Type *type,
   long handler_offset, Etk_Marshaller marshaller)
{
   Etk_Signal *new_signal;

   new_signal = malloc(sizeof(Etk_Signal));
   new_signal->name = strdup(signal_name);
   new_signal->code = type->signals_count;
   new_signal->handler_offset = handler_offset;
   new_signal->marshaller = marshaller;

   type->signals_count++;

   _etk_signal_signals_list = evas_list_append(_etk_signal_signals_list, new_signal);

   return new_signal;
}

/**
 * @brief Creates a new signal called @a signal_name, for the object type @a
 * type.
 *
 * @param signal_name the name of the new signal.
 * @param type the object type of the new signal.
 * @param handler_offset the offset of the default handler in the object's
 * struct (use ETK_MEMBER_OFFSET() to get it). -1 if there is no default
 * handler.
 * @param marshaller the marshaller of the signal: it will treat and pass the
 * arguments to the callbacks.
 *
 * @return Returns the new signal code, or -1 in case of failure
 */
int etk_signal_new(const char *signal_name, Etk_Type *type, long handler_offset,
                   Etk_Marshaller marshaller)
{
   Etk_Signal *new_signal;

   if (!signal_name || !type || !marshaller)
      return -1;

   new_signal = etk_signal_new_raw(signal_name, type, handler_offset,
                                   marshaller);

   if (!new_signal)
      return -1;

   type->signals =
      (Etk_Signal **) realloc(type->signals,
                              type->signals_count * sizeof(Etk_Signal *));

   type->signals[new_signal->code] = new_signal;

   return new_signal->code;
}

/**
 * @internal
 * @brief Creates a new signal called @a signal_name, for the object type @a
 * type, based on an @a Etk_Signal_Description.
 */
void etk_signal_new_with_desc(Etk_Type *type,
                              const Etk_Signal_Description *desc)
{
   Etk_Signal *new_signal;

   if (!desc || !desc->signal_code_store)
      return;

   if (!type || !desc->name || !desc->marshaller)
   {
      *(desc->signal_code_store) = -1;
      return;
   }

   new_signal = etk_signal_new_raw(desc->name, type, desc->handler_offset,
                                   desc->marshaller);

   if (!new_signal)
   {
      *(desc->signal_code_store) = -1;
      return;
   }

   type->signals[new_signal->code] = new_signal;
   *(desc->signal_code_store) = new_signal->code;
}

/**
 * @brief Gets the signal corresponding to the name and the object type.
 *
 * assumes @a signal_name and @a type to be valid.
 */
static Etk_Signal *etk_signal_lookup_by_name(const char *signal_name, Etk_Type *type)
{
   unsigned i;

   if (!type->signals)
      return NULL;

   for (i = 0; i < type->signals_count; i++)
      if (strcmp(type->signals[i]->name, signal_name) == 0)
         return type->signals[i];

   return NULL;
}

/**
 * @brief Gets the signal code corresponding to the name and the object type.
 * @internal
 * assumes @a signal_name and @a type to be valid.
 */
static int etk_signal_lookup_code(const char *signal_name, Etk_Type *type)
{
   Etk_Signal *signal;

   signal = etk_signal_lookup_by_name(signal_name, type);
   if (signal)
     return signal->code;
   else
     return -1;
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
 * @brief Connects a callback to a signal of the object @a object.
 *
 * When the signal of the object will be emitted, this callback will be
 * automatically called.
 *
 * @param signal_code the signal code to connect to the callback
 * @param object the object to connect to the callback
 * @param callback the callback to call when the signal is emitted. This
 *        callback should return Etk_Bool with ETK_TRUE to continue
 *        and ETK_FALSE to stop signal propagation to next callbacks
 *        during the current emission.
 * @param data the data to pass to the callback
 * @param swapped if @a swapped == ETK_TRUE, the callback will be called
 *        with @a data as the only argument. It can be useful to set it to
 *        ETK_TRUE if you just want to call one function on an object when the
 *        signal is emitted.
 * @param after if @a after == ETK_TRUE, the callback will be called after all
 *        the callbacks already connected to this signal. Otherwise, it will be
 *        called before all of them (default behavior)
 *
 * @return Returns a pointer identifying the connected callback, which can be
 * used later for disconnecting only this specific signal.
 */
const Etk_Signal_Callback *etk_signal_connect_full_by_code(int signal_code, Etk_Object *object, Etk_Callback callback, void *data, Etk_Bool swapped, Etk_Bool after)
{
   Etk_Signal_Callback *new_callback;
   if (!object || !callback)
      return NULL;

   if (signal_code < 0 || signal_code >= object->type->signals_count)
   {
      ETK_WARNING("Invalid signal connection: the object type \"%s\" doesn't "
                  "have a signal with code \"%d\"", object->type->name,
                  signal_code);
      return NULL;
   }

   if (!(new_callback = etk_signal_callback_new(callback, data, swapped)))
      return NULL;

   etk_object_signal_callback_add(object, signal_code, new_callback, after);
   return new_callback;
}

/**
 * @brief Connects a callback to a signal of the object @a object.
 *
 * When the signal of the object will be emitted, this callback will be
 * automatically called.
 *
 * @param signal_name the signal name to connect to the callback
 * @param object the object to connect to the callback
 * @param callback the callback to call when the signal is emitted
 * @param data the data to pass to the callback
 * @param swapped if @a swapped == ETK_TRUE, the callback will be called
 *        with @a data as the only argument. It can be useful to set it to
 *        ETK_TRUE if you just want to call one function on an object when
 *        the signal is emitted.
 * @param after if @a after == ETK_TRUE, the callback will be called after
 *        all the callbacks already connected to this signal. Otherwise, it
 *        will be called before all of them (default behavior)
 *
 * @return Returns a pointer identifying the connected callback, which can be
 * used later for disconnecting only this specific signal.
 */
const Etk_Signal_Callback *etk_signal_connect_full_by_name(const char *signal_name, Etk_Object *object, Etk_Callback callback, void *data, Etk_Bool swapped, Etk_Bool after)
{
   int signal_code;

   if (!object || !signal_name)
      return ETK_FALSE;

   signal_code = etk_signal_lookup_code(signal_name,
                                        etk_object_object_type_get(object));

   return etk_signal_connect_full_by_code(signal_code, object, callback, data,
                                          swapped, after);
}

inline const Etk_Signal_Callback *etk_signal_connect(const char *signal_name,
   Etk_Object *object, Etk_Callback callback, void *data)
{
   return etk_signal_connect_full_by_name(signal_name, object, callback, data,
                                          ETK_FALSE, ETK_FALSE);
}

inline const Etk_Signal_Callback *etk_signal_connect_after(
   const char *signal_name, Etk_Object *object, Etk_Callback callback,
   void *data)
{
   return etk_signal_connect_full_by_name(signal_name, object, callback, data,
                                          ETK_FALSE, ETK_TRUE);
}

inline const Etk_Signal_Callback *etk_signal_connect_swapped(
   const char *signal_name, Etk_Object *object, Etk_Callback callback,
   void *data)
{
   return etk_signal_connect_full_by_name(signal_name, object, callback, data,
                                          ETK_TRUE, ETK_FALSE);
}

inline const Etk_Signal_Callback *etk_signal_connect_by_code(int signal_code,
   Etk_Object *object, Etk_Callback callback, void *data)
{
   return etk_signal_connect_full_by_code(signal_code, object, callback, data,
                                          ETK_FALSE, ETK_FALSE);
}

inline const Etk_Signal_Callback *etk_signal_connect_after_by_code(
   int signal_code, Etk_Object *object, Etk_Callback callback, void *data)
{
   return etk_signal_connect_full_by_code(signal_code, object, callback, data,
                                          ETK_FALSE, ETK_TRUE);
}

inline const Etk_Signal_Callback *etk_signal_connect_swapped_by_code(
   int signal_code, Etk_Object *object, Etk_Callback callback, void *data)
{
   return etk_signal_connect_full_by_code(signal_code, object, callback, data,
                                          ETK_TRUE, ETK_FALSE);
}

void etk_signal_connect_multiple(Etk_Signal_Connect_Desc *desc,
                                 Etk_Object *object, void *data)
{
   Etk_Signal_Connect_Desc *s = desc;

   if (!object || !s)
      return;

   for (; s->signal_code >= 0; s++)
      s->scb = etk_signal_connect_by_code(s->signal_code, object, s->callback,
                                          data);
}

void etk_signal_disconnect_multiple(Etk_Signal_Connect_Desc *desc,
                                    Etk_Object *object)
{
   Etk_Signal_Connect_Desc *s = desc;

   if (!object || !s)
      return;

   for (; s->signal_code >= 0; s++)
      etk_signal_disconnect_scb_by_code(s->signal_code, object, s->scb);
}


/**
 * @brief Disconnects a callback from a signal, the callback won't be called anymore when the signal is emitted.
 * @param signal_name the name of the signal connected to the callback to disconnect
 * @param object the object connected to the callback to disconnect
 * @param callback the callback to disconnect
 * @param data passed to the callback when connected
 */
void etk_signal_disconnect_by_code(int signal_code, Etk_Object *object,
                                   Etk_Callback callback, void *data)
{
   Evas_List *c;
   Etk_Signal_Callback *signal_callback;

   if (!object || !callback)
      return;

   if (signal_code < 0 || signal_code >= object->type->signals_count)
   {
      ETK_WARNING("Invalid signal disconnection: the object type \"%s\" "
                  "doesn't have a signal with code \"%d\"", object->type->name,
                  signal_code);
      return;
   }

   c = NULL;
   etk_object_signal_callbacks_get(object, signal_code, &c);
   while (c)
   {
      signal_callback = c->data;
      c = c->next;

      if (signal_callback->callback == callback &&
          signal_callback->data == data)
      {
         etk_object_signal_callback_remove(object, signal_code,
                                           signal_callback);
         break;
      }
   }
}

/**
 * @brief Disconnects a callback from a signal, the callback won't be called
 * anymore when the signal is emitted. Removes one specific occurence of a
 * callback function for a signal, identified by the value returned in
 * connect() calls.
 * @param signal_code code of the signal to disconnect
 * @param object the object connected to the callback to disconnect
 * @param scb the unique signal callback to disconnect
 */
void etk_signal_disconnect_scb_by_code(int signal_code, Etk_Object *object,
                                       const Etk_Signal_Callback *scb)
{
   Evas_List *c;
   Etk_Signal_Callback *signal_callback;

   if (!object || !scb)
      return;

   if (signal_code < 0 || signal_code >= object->type->signals_count)
   {
      ETK_WARNING("Invalid signal disconnection: the object type \"%s\" "
                  "doesn't have a signal with code \"%d\"", object->type->name,
                  signal_code);
      return;
   }

   c = NULL;
   etk_object_signal_callbacks_get(object, signal_code, &c);
   while (c)
   {
      signal_callback = c->data;
      c = c->next;

      if (signal_callback == scb)
      {
         etk_object_signal_callback_remove(object, signal_code,
                                           signal_callback);
         break;
      }
   }
}

void etk_signal_disconnect(const char *signal_name, Etk_Object *object,
                           Etk_Callback callback, void *data)
{
   int signal_code;

   if (!object || !signal_name)
      return;

   signal_code = etk_signal_lookup_code(signal_name,
                                        etk_object_object_type_get(object));

   etk_signal_disconnect_by_code(signal_code, object, callback, data);
}

void etk_signal_disconnect_scb(const char *signal_name, Etk_Object *object,
                               const Etk_Signal_Callback *scb)
{
   int signal_code;

   if (!object || !signal_name)
      return;

   signal_code = etk_signal_lookup_code(signal_name,
                                        etk_object_object_type_get(object));

   etk_signal_disconnect_scb_by_code(signal_code, object, scb);
}

/**
 * @brief Disconnects all callbacks from a signal
 * @param signal_code the code of the signal for which all callbacks will be disconnected
 * @param object the object for which all callbacks will be disconnected
 */
void etk_signal_disconnect_all_by_code(int signal_code, Etk_Object *object)
{
   Evas_List *c;
   Etk_Signal_Callback *signal_callback;

   if (!object)
      return;

   if (signal_code < 0 || signal_code >= object->type->signals_count)
   {
      ETK_WARNING("Invalid signal disconnection: the object type \"%s\" "
                  "doesn't have a signal with code \"%d\"", object->type->name,
                  signal_code);
      return;
   }

   c = NULL;
   etk_object_signal_callbacks_get(object, signal_code, &c);
   while (c)
   {
      signal_callback = c->data;
      c = c->next;
      etk_object_signal_callback_remove(object, signal_code, signal_callback);
   }
}

/**
 * @brief Disconnects all callbacks from a signal
 * @param signal_name the name of the signal for which all callbacks will be disconnected
 * @param object the object for which all callbacks will be disconnected
 */
void etk_signal_disconnect_all(const char *signal_name, Etk_Object *object)
{
   int signal_code;

   if (!object || !signal_name)
      return;

   signal_code = etk_signal_lookup_code(signal_name,
                                        etk_object_object_type_get(object));

   etk_signal_disconnect_all_by_code(signal_code, object);
}

/**
 * @brief Blocks a callback from being called when the corresponding signal is
 * emitted. Unlike etk_signal_disconnect(), the callback is not removed, and
 * can be easily unblock with etk_signal_unblock().
 *
 * @param signal_code the code of the signal connected to the callback to block
 * @param object the object connected to the callback to block
 * @param callback the callback function to block
 * @param data passed to the callback when connected
 */
void etk_signal_block_by_code(int signal_code, Etk_Object *object,
                              Etk_Callback callback, void *data)
{
   Evas_List *c;
   Etk_Signal_Callback *signal_callback;

   if (!object || !callback)
      return;

   if (signal_code < 0 || signal_code >= object->type->signals_count)
   {
      ETK_WARNING("Invalid signal block: the object type \"%s\" doesn't "
                  "have a signal with code \"%d\"", object->type->name,
                  signal_code);
      return;
   }

   c = NULL;
   etk_object_signal_callbacks_get(object, signal_code, &c);
   while (c)
   {
      signal_callback = c->data;
      c = c->next;

      if (signal_callback->callback == callback &&
          signal_callback->data == data)
      {
         etk_signal_callback_block(signal_callback);
         break;
      }
   }
}

/**
 * @brief Blocks a callback from being called when the corresponding signal is
 * emitted. Unlike etk_signal_disconnect(), the callback is not removed, and
 * can be easily unblock with etk_signal_unblock().
 *
 * @param signal_name the name of the signal connected to the callback to block
 * @param object the object connected to the callback to block
 * @param callback the callback function to block
 * @param data passed to the callback when connected
 */
void etk_signal_block(const char *signal_name, Etk_Object *object,
                      Etk_Callback callback, void *data)
{
   int signal_code;

   if (!object || !signal_name)
      return;

   signal_code = etk_signal_lookup_code(signal_name,
                                        etk_object_object_type_get(object));

   etk_signal_block_by_code(signal_code, object, callback, data);
}

/**
 * @brief Blocks a callback from being called when the corresponding signal is
 * emitted. Unlike etk_signal_disconnect(), the callback is not removed, and
 * can be easily unblock with etk_signal_unblock().
 *
 * @param signal_code code of the signal connected to the callback to block
 * @param object the object connected to the callback to block
 * @param scb the unique signal callback to block
 */
void etk_signal_block_scb_by_code(int signal_code, Etk_Object *object,
                                  const Etk_Signal_Callback *scb)
{
   Evas_List *c;
   Etk_Signal_Callback *signal_callback;

   if (!object || !scb)
      return;

   if (signal_code < 0 || signal_code >= object->type->signals_count)
   {
      ETK_WARNING("Invalid signal block: the object type \"%s\" doesn't "
                  "have a signal with code \"%d\"", object->type->name,
                  signal_code);
      return;
   }

   c = NULL;
   etk_object_signal_callbacks_get(object, signal_code, &c);
   while (c)
   {
      signal_callback = c->data;
      c = c->next;

      if (signal_callback == scb)
      {
         etk_signal_callback_block(signal_callback);
         break;
      }
   }
}

/**
 * @brief Blocks a callback from being called when the corresponding signal is
 * emitted.
 *
 * Unlike etk_signal_disconnect(), the callback is not removed, and can be
 * easily unblock with etk_signal_unblock().
 *
 * @param signal_name the name of the signal connected to the callback to block
 * @param object the object connected to the callback to block
 * @param scb the unique signal callback to block
 */
void etk_signal_block_scb(const char *signal_name, Etk_Object *object,
                          const Etk_Signal_Callback *scb)
{
   int signal_code;

   if (!object || !signal_name)
      return;

   signal_code = etk_signal_lookup_code(signal_name,
                                        etk_object_object_type_get(object));

   etk_signal_block_scb_by_code(signal_code, object, scb);
}

void etk_signal_unblock_by_code(int signal_code, Etk_Object *object,
                                Etk_Callback callback, void *data)
{
   Evas_List *c;
   Etk_Signal_Callback *signal_callback;

   if (!object || !callback)
      return;

   if (signal_code < 0 || signal_code >= object->type->signals_count)
   {
      ETK_WARNING("Invalid signal unblock: the object type \"%s\" doesn't "
                  "have a signal with code \"%d\"", object->type->name,
                  signal_code);
      return;
   }

   c = NULL;
   etk_object_signal_callbacks_get(object, signal_code, &c);
   while (c)
   {
      signal_callback = c->data;
      c = c->next;

      if (signal_callback->callback == callback && signal_callback->data == data)
      {
         etk_signal_callback_unblock(signal_callback);
         break;
      }
   }
}

/**
 * @brief Unblocks a blocked callback.
 *
 * The callback will no longer be prevented from being called when the
 * corresponding signal is emitted.
 *
 * @param signal_name name of the signal connected to the callback to unblock
 * @param object the object connected to the callback to unblock
 * @param callback the callback function to unblock
 * @param data passed to the callback when connected
 */
void etk_signal_unblock(const char *signal_name, Etk_Object *object,
                        Etk_Callback callback, void *data)
{
   int signal_code;

   if (!object || !signal_name)
      return;

   signal_code = etk_signal_lookup_code(signal_name,
                                        etk_object_object_type_get(object));
   etk_signal_unblock_by_code(signal_code, object, callback, data);
}



/**
 * @brief Unblocks a blocked callback.
 *
 * See etk_signal_unblock().
 *
 * @param signal_code code of the signal connected to the callback to unblock
 * @param object the object connected to the callback to unblock
 * @param scb the unique signal callback to unblock
 */
void etk_signal_unblock_scb_by_code(int signal_code, Etk_Object *object,
                                    const Etk_Signal_Callback *scb)
{
   Evas_List *c;
   Etk_Signal_Callback *signal_callback;

   if (!object || !scb)
      return;

   if (signal_code < 0 || signal_code >= object->type->signals_count)
   {
      ETK_WARNING("Invalid signal unblock: the object type \"%s\" doesn't "
                  "have a signal with code \"%d\"", object->type->name,
                  signal_code);
      return;
   }

   c = NULL;
   etk_object_signal_callbacks_get(object, signal_code, &c);
   while (c)
   {
      signal_callback = c->data;
      c = c->next;

      if (signal_callback == scb)
      {
         etk_signal_callback_unblock(signal_callback);
         break;
      }
   }
}

/**
 * @brief Unblocks a blocked callback.
 *
 * See etk_signal_unblock().
 *
 * @param signal_name name of the signal connected to the callback to unblock
 * @param object the object connected to the callback to unblock
 * @param scb the unique signal callback to unblock
 */
void etk_signal_unblock_scb(const char *signal_name, Etk_Object *object,
                            const Etk_Signal_Callback *scb)
{
   int signal_code;

   if (!object || !signal_name)
      return;

   signal_code = etk_signal_lookup_code(signal_name,
                                        etk_object_object_type_get(object));
   etk_signal_unblock_scb_by_code(signal_code, object, scb);
}


/**
 * @brief Emits the signal: it will call the callbacks connected to the
 * signal @a signal.
 *
 * @param signal_code code of the signal to emit
 * @param object the object which will emit the signal
 * @param ... the arguments to pass to the callback function
 *
 * @return Returns ETK_FALSE if the signal has been stopped
 *         and ETK_TRUE otherwise
 */
Etk_Bool etk_signal_emit(int signal_code, Etk_Object *object, ...)
{
   va_list args;
   Etk_Signal *signal;
   Etk_Bool ret;

   if (!object)
      return ETK_FALSE;

   if (signal_code < 0 || signal_code >= object->type->signals_count ||
       !(signal = object->type->signals[signal_code]))
   {
      ETK_WARNING("Invalid signal emission: the object type \"%s\" "
                  "doesn't have a signal with code \"%d\"", object->type->name,
                  signal_code);
      return ETK_FALSE;
   }

   va_start(args, object);
   ret = etk_signal_emit_valist(signal, object, args);
   va_end(args);

   return ret;
}

/**
 * @brief Emits the signal: it will call the callbacks connected to the
 *        signal @a signal
 *
 * @param signal the name of the signal to emit
 * @param object the object which will emit the signal
 * @param ... the arguments to pass to the callback function
 *
 * @return Returns ETK_FALSE if the signal has been stopped
 *         and ETK_TRUE otherwise
 */
Etk_Bool etk_signal_emit_by_name(const char *signal_name, Etk_Object *object,
                                 ...)
{
   va_list args;
   Etk_Signal *signal;
   Etk_Bool ret;

   if (!object || !signal_name)
      return ETK_FALSE;

   signal = etk_signal_lookup_by_name(signal_name, etk_object_object_type_get(object));
   if (!signal)
   {
      ETK_WARNING("Invalid signal emission: the object type doesn't have "
                  "a signal called \"%s\"", signal_name);
      return ETK_FALSE;
   }

   va_start(args, object);
   ret = etk_signal_emit_valist(signal, object, args);
   va_end(args);

   return ret;
}

/**
 * @brief Emits the signal: it will call the callbacks connected to the
 *        signal @a signal
 *
 * @param signal the signal to emit
 * @param object the object which will emit the signal
 * @param args the arguments to pass to the callback function
 *
 * @return Returns @a object, or NULL if @a object has been destroyed by
 *         one of the callbacks
 * @return Returns ETK_FALSE if the signal has been stopped
 *         and ETK_TRUE otherwise
 */
Etk_Bool etk_signal_emit_valist(Etk_Signal *signal, Etk_Object *object,
                                va_list args)
{
   Evas_List *callbacks;
   Etk_Signal_Callback *callback;
   Etk_Bool keep_emission = ETK_TRUE;
   va_list args2;
   void *object_ptr;

   if (!object || !signal)
      return ETK_FALSE;

   /* The pointer object will be set to NULL if the object is destroyed
    * by a callback */
   object_ptr = object;
   etk_object_weak_pointer_add(object, &object_ptr);

   /* Calls the default handler */
   if (signal->handler_offset >= 0 && signal->marshaller)
   {
      Etk_Callback *default_handler;

      default_handler = (void *)object + signal->handler_offset;
      if (*default_handler)
      {
         va_copy(args2, args);
         signal->marshaller(*default_handler, object, NULL, &keep_emission,
                            args2);
         va_end(args2);
      }
   }

   /* Then we call the corresponding callbacks */
   if (object_ptr && keep_emission)
   {
      callbacks = NULL;
      etk_object_signal_callbacks_get(object, signal->code, &callbacks);
      while (keep_emission && callbacks && object_ptr)
      {
         callback = callbacks->data;
         callbacks = callbacks->next;

         va_copy(args2, args);
         etk_signal_callback_call_valist(signal, callback, object,
                                         &keep_emission, args2);
         va_end(args2);
      }
   }

   if (object_ptr)
      etk_object_weak_pointer_remove(object, &object_ptr);

   return keep_emission;
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
 * @brief Gets a list of all the current signals
 * @return Returns an Evas_List containing all the signals.
 */
Evas_List * etk_signal_get_all()
{
   return _etk_signal_signals_list;
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
