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

/** @file etk_marshallers.h */
#ifndef _ETK_MARSHALLERS_H_
#define _ETK_MARSHALLERS_H_

#include <stdarg.h>

#include "etk_types.h"

#ifdef __cplusplus
extern "C" {
#endif

void etk_marshaller_VOID(Etk_Callback callback, Etk_Object *object, void *data, void *return_value, va_list arguments);
void etk_marshaller_INT(Etk_Callback callback, Etk_Object *object, void *data, void *return_value, va_list arguments);
void etk_marshaller_INT_INT(Etk_Callback callback, Etk_Object *object, void *data, void *return_value, va_list arguments);
void etk_marshaller_DOUBLE(Etk_Callback callback, Etk_Object *object, void *data, void *return_value, va_list arguments);
void etk_marshaller_OBJECT(Etk_Callback callback, Etk_Object *object, void *data, void *return_value, va_list arguments);
void etk_marshaller_POINTER(Etk_Callback callback, Etk_Object *object, void *data, void *return_value, va_list arguments);
void etk_marshaller_POINTER_POINTER(Etk_Callback callback, Etk_Object *object, void *data, void *return_value, va_list arguments);
void etk_marshaller_INT_POINTER(Etk_Callback callback, Etk_Object *object, void *data, void *return_value, va_list arguments);

#ifdef __cplusplus
}
#endif

#endif
