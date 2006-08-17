/** @file etk_marshallers.h */
#ifndef _ETK_MARSHALLERS_H_
#define _ETK_MARSHALLERS_H_

#include <stdarg.h>
#include "etk_types.h"

void etk_marshaller_VOID__VOID(Etk_Callback callback, Etk_Object *object, void *data, void *return_value, va_list arguments);
void etk_marshaller_VOID__INT(Etk_Callback callback, Etk_Object *object, void *data, void *return_value, va_list arguments);
void etk_marshaller_VOID__DOUBLE(Etk_Callback callback, Etk_Object *object, void *data, void *return_value, va_list arguments);
void etk_marshaller_VOID__POINTER(Etk_Callback callback, Etk_Object *object, void *data, void *return_value, va_list arguments);
void etk_marshaller_VOID__POINTER_POINTER(Etk_Callback callback, Etk_Object *object, void *data, void *return_value, va_list arguments);
void etk_marshaller_VOID__INT_POINTER(Etk_Callback callback, Etk_Object *object, void *data, void *return_value, va_list arguments);
void etk_marshaller_BOOL__VOID(Etk_Callback callback, Etk_Object *object, void *data, void *return_value, va_list arguments);
void etk_marshaller_BOOL__DOUBLE(Etk_Callback callback, Etk_Object *object, void *data, void *return_value, va_list arguments);
void etk_marshaller_BOOL__POINTER_POINTER(Etk_Callback callback, Etk_Object *object, void *data, void *return_value, va_list arguments);

#endif
