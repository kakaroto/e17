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

/** @file etk_signal_callback.c */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "etk_signal_callback.h"

#include <stdlib.h>
#include <stdarg.h>

#include "etk_signal.h"
#include "etk_type.h"
#include "etk_object.h"

/**
 * @addtogroup Etk_Signal_Callback
 * @{
 */

/**************************
 *
 * Implementation
 *
 **************************/

/**
 * @internal
 * @brief Creates a new signal-callback for the signal @a signal, using the callback function @a callback, and
 * associated to user data @a data
 * @param callback the callback function to call when the signal is emitted
 * @param data the user data to pass to the callback function
 * @param swapped if @a swapped is ETK_TRUE, the callback function will be called with the data as the only argument
 * @return Returns the new signal-callback on success or NULL on failure
 * @warning The new signal-callback has to be freed with etk_signal_callback_del()
 */
Etk_Signal_Callback *etk_signal_callback_new(Etk_Callback callback, void *data, Etk_Bool swapped)
{
   Etk_Signal_Callback *new_callback;

   if (!callback)
      return NULL;

   new_callback = malloc(sizeof(Etk_Signal_Callback));
   new_callback->callback = callback;
   new_callback->data = data;
   new_callback->swapped = swapped;
   new_callback->blocked = ETK_FALSE;

   return new_callback;
}

/**
 * @internal
 * @brief Deletes the signal-callback
 * @param signal_callback the signal-callback to delete
 */
void etk_signal_callback_del(Etk_Signal_Callback *signal_callback)
{
   free(signal_callback);
}

/**
 * @internal
 * @brief Calls the callback @a callback on the object @a object
 *
 * @param signal signal of the signal-callback to call
 * @param callback the signal-callback to call
 * @param object the object to call the callback on
 * @param return_value the location for the return value (if none, it can be
 *        NULL)
 * @param args the arguments to pass to the callback
 * @note if the callback is blocked, it won't be called
 */
void etk_signal_callback_call_valist(Etk_Signal *signal,
   Etk_Signal_Callback *callback, Etk_Object *object, void *return_value,
   va_list args)
{
   Etk_Marshaller marshaller;

   if (!callback || !callback->callback || callback->blocked ||
       !(marshaller = etk_signal_marshaller_get(signal)))
      return;

   if (callback->swapped)
   {
      Etk_Callback_Swapped swapped_callback = callback->callback;
      swapped_callback(callback->data);
   }
   else
   {
      va_list args2;

      va_copy(args2, args);
      marshaller(callback->callback, object, callback->data, return_value,
                 args2);
      va_end(args2);
   }
}

/**
 * @internal
 * @brief Prevents the callback from being called: etk_signal_callback_call() will have no effect on the callback
 * @param callback the callback to block
 */
void etk_signal_callback_block(Etk_Signal_Callback *callback)
{
   if (!callback)
      return;
   callback->blocked = ETK_TRUE;
}

/**
 * @internal
 * @brief Unblocks the callback. It can no be called again by calling etk_signal_callback_call()
 * @param callback the callback to unblock
 */
void etk_signal_callback_unblock(Etk_Signal_Callback *callback)
{
   if (!callback)
      return;
   callback->blocked = ETK_FALSE;
}

/**
 * @internal
 * @brief Gets whether or not the callback is blocked
 * @param callback a callback
 * @return Returns ETK_TRUE if the callback is blocked, ETK_FALSE otherwise
 */
Etk_Bool etk_signal_callback_is_blocked(Etk_Signal_Callback *callback)
{
   if (!callback)
      return ETK_FALSE;
   return callback->blocked;
}

/** @} */
