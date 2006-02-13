/** @file etk_marshallers.h */
#ifndef _ETK_MARSHALLERS_H_
#define _ETK_MARSHALLERS_H_

#include <stdarg.h>
#include "etk_types.h"

/** @brief Casts the function pointer to an Etk_Signal_Marshaller */
#define ETK_MARSHALLER(marshaller)    ((Etk_Signal_Marshaller)(marshaller))

/* TODO: marshaller generator */

void etk_marshaller_VOID__VOID(Etk_Signal_Callback_Function callback, Etk_Object *object, void *data, void *return_value, va_list arguments);
void etk_marshaller_VOID__INT(Etk_Signal_Callback_Function callback, Etk_Object *object, void *data, void *return_value, va_list arguments);
void etk_marshaller_VOID__DOUBLE(Etk_Signal_Callback_Function callback, Etk_Object *object, void *data, void *return_value, va_list arguments);
void etk_marshaller_VOID__POINTER(Etk_Signal_Callback_Function callback, Etk_Object *object, void *data, void *return_value, va_list arguments);
void etk_marshaller_VOID__POINTER_POINTER(Etk_Signal_Callback_Function callback, Etk_Object *object, void *data, void *return_value, va_list arguments);
void etk_marshaller_VOID__INT_POINTER(Etk_Signal_Callback_Function callback, Etk_Object *object, void *data, void *return_value, va_list arguments);
void etk_marshaller_BOOL__VOID(Etk_Signal_Callback_Function callback, Etk_Object *object, void *data, void *return_value, va_list arguments);
void etk_marshaller_BOOL__DOUBLE(Etk_Signal_Callback_Function callback, Etk_Object *object, void *data, void *return_value, va_list arguments);
void etk_marshaller_BOOL__POINTER_POINTER(Etk_Signal_Callback_Function callback, Etk_Object *object, void *data, void *return_value, va_list arguments);

#endif
