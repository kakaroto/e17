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

/** @file etk_marshallers.c */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "etk_marshallers.h"

/* etk_marshaller_VOID */
void etk_marshaller_VOID(Etk_Callback callback, Etk_Object *object, void *data, void *return_value, va_list arguments)
{
   typedef Etk_Bool (*Etk_Callback_VOID)(Etk_Object *object, void *data);
   Etk_Callback_VOID callback_VOID;
   Etk_Bool result;

   callback_VOID = (Etk_Callback_VOID)callback;
   result = callback_VOID(object, data);

   *((Etk_Bool *)return_value) = result;
}

/* etk_marshaller_INT */
void etk_marshaller_INT(Etk_Callback callback, Etk_Object *object, void *data, void *return_value, va_list arguments)
{
   typedef Etk_Bool (*Etk_Callback_INT)(Etk_Object *object, int arg1, void *data);
   Etk_Callback_INT callback_INT;
   int arg1;
   Etk_Bool result;

   arg1 = va_arg(arguments, int);

   callback_INT = (Etk_Callback_INT)callback;
   result = callback_INT(object, arg1, data);

   *((Etk_Bool *)return_value) = result;
}

/* etk_marshaller_INT_INT */
void etk_marshaller_INT_INT(Etk_Callback callback, Etk_Object *object, void *data, void *return_value, va_list arguments)
{
   typedef Etk_Bool (*Etk_Callback_INT_INT)(Etk_Object *object, int arg1, int arg2, void *data);
   Etk_Callback_INT_INT callback_INT_INT;
   int arg1;
   int arg2;
   Etk_Bool result;

   arg1 = va_arg(arguments, int);
   arg2 = va_arg(arguments, int);

   callback_INT_INT = (Etk_Callback_INT_INT)callback;
   result = callback_INT_INT(object, arg1, arg2, data);

   *((Etk_Bool *)return_value) = result;
}

/* etk_marshaller_DOUBLE */
void etk_marshaller_DOUBLE(Etk_Callback callback, Etk_Object *object, void *data, void *return_value, va_list arguments)
{
   typedef Etk_Bool (*Etk_Callback_DOUBLE)(Etk_Object *object, double arg1, void *data);
   Etk_Callback_DOUBLE callback_DOUBLE;
   double arg1;
   Etk_Bool result;

   arg1 = va_arg(arguments, double);

   callback_DOUBLE = (Etk_Callback_DOUBLE)callback;
   result = callback_DOUBLE(object, arg1, data);

   *((Etk_Bool *)return_value) = result;
}

/* etk_marshaller_OBJECT */
void etk_marshaller_OBJECT(Etk_Callback callback, Etk_Object *object, void *data, void *return_value, va_list arguments)
{
   typedef Etk_Bool (*Etk_Callback_OBJECT)(Etk_Object *object, Etk_Object *arg1, void *data);
   Etk_Callback_OBJECT callback_OBJECT;
   Etk_Object *arg1;
   Etk_Bool result;

   arg1 = va_arg(arguments, Etk_Object *);

   callback_OBJECT = (Etk_Callback_OBJECT)callback;
   result = callback_OBJECT(object, arg1, data);

   *((Etk_Bool *)return_value) = result;
}

/* etk_marshaller_POINTER */
void etk_marshaller_POINTER(Etk_Callback callback, Etk_Object *object, void *data, void *return_value, va_list arguments)
{
   typedef Etk_Bool (*Etk_Callback_POINTER)(Etk_Object *object, void *arg1, void *data);
   Etk_Callback_POINTER callback_POINTER;
   void *arg1;
   Etk_Bool result;

   arg1 = va_arg(arguments, void *);

   callback_POINTER = (Etk_Callback_POINTER)callback;
   result = callback_POINTER(object, arg1, data);

   *((Etk_Bool *)return_value) = result;
}


/* etk_marshaller_POINTER_POINTER */
void etk_marshaller_POINTER_POINTER(Etk_Callback callback, Etk_Object *object, void *data, void *return_value, va_list arguments)
{
   typedef Etk_Bool (*Etk_Callback_POINTER_POINTER)(Etk_Object *object, void *arg1, void *arg2, void *data);
   Etk_Callback_POINTER_POINTER callback_POINTER_POINTER;
   void *arg1;
   void *arg2;
   Etk_Bool result;

   arg1 = va_arg(arguments, void *);
   arg2 = va_arg(arguments, void *);

   callback_POINTER_POINTER = (Etk_Callback_POINTER_POINTER)callback;
   result = callback_POINTER_POINTER(object, arg1, arg2, data);

   *((Etk_Bool *)return_value) = result;
}

/* etk_marshaller_INT_POINTER */
void etk_marshaller_INT_POINTER(Etk_Callback callback, Etk_Object *object, void *data, void *return_value, va_list arguments)
{
   typedef Etk_Bool (*Etk_Callback_INT_POINTER)(Etk_Object *object, int arg1, void *arg2, void *data);
   Etk_Callback_INT_POINTER callback_INT_POINTER;
   int arg1;
   void *arg2;
   Etk_Bool result;

   arg1 = va_arg(arguments, int);
   arg2 = va_arg(arguments, void *);

   callback_INT_POINTER = (Etk_Callback_INT_POINTER)callback;
   result = callback_INT_POINTER(object, arg1, arg2, data);

   *((Etk_Bool *)return_value) = result;
}
