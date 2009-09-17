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

/** @file etk_signal_callback.h */
#ifndef _ETK_SIGNAL_CALLBACK_H_
#define _ETK_SIGNAL_CALLBACK_H_

#include <stdarg.h>

#include "etk_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup Etk_Signal_Callback Etk_Signal_Callback
 * @brief For more info, see the page about Etk_Signal
 * @{
 */

/** Casts the function pointer to an Etk_Signal_Callback_Function */
#define ETK_CALLBACK(callback)    ((Etk_Callback)(callback))

/**
 * @brief A callback that will be called when the corresponding signal is emitted
 * @structinfo
 */
struct Etk_Signal_Callback
{
   /* private: */
   Etk_Callback callback;
   void *data;
   Etk_Bool swapped:1;
   Etk_Bool blocked:1;
};


Etk_Signal_Callback *etk_signal_callback_new(Etk_Callback callback, void *data, Etk_Bool swapped);
void     etk_signal_callback_del(Etk_Signal_Callback *signal_callback);

void     etk_signal_callback_call_valist(Etk_Signal *signal,
            Etk_Signal_Callback *callback, Etk_Object *object,
            void *return_value, va_list args);
void     etk_signal_callback_block(Etk_Signal_Callback *callback);
void     etk_signal_callback_unblock(Etk_Signal_Callback *callback);
Etk_Bool etk_signal_callback_is_blocked(Etk_Signal_Callback *callback);


/** @} */

#ifdef __cplusplus
}
#endif

#endif
