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
