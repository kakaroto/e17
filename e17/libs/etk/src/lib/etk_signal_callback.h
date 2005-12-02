/** @file etk_signal_callback.h */
#ifndef _ETK_SIGNAL_CALLBACK_H_
#define _ETK_SIGNAL_CALLBACK_H_

#include <stdarg.h>
#include "etk_types.h"

/**
 * @defgroup Etk_Signal_Callback Etk_Signal_Callback
 * @{
 */

/** @brief Casts the function pointer to an Etk_Signal_Callback_Function */
#define ETK_CALLBACK(callback)           ((Etk_Signal_Callback_Function)(callback))
/** @brief Casts the function pointer to an Etk_Signal_Swapped_Callback_Function */
#define ETK_SWAPPED_CALLBACK(callback)   ((Etk_Signal_Swapped_Callback_Function)(callback))

/**
 * @struct Etk_Signal_Callback
 * @brief An Etk_Signal_Callback is a function called when the associated signal is emmited. @n
 * If it swapped, it is called with @a data as the only argument. @n
 * Otherwise, its arguments are (Etk_Object *object, ..., void *data) where ... are optionnal arguments depending on the signal
 */
struct _Etk_Signal_Callback
{
   /* private: */
   Etk_Signal *signal;
   Etk_Signal_Callback_Function callback;
   void *data;
   Etk_Bool swapped;
};

Etk_Signal_Callback *etk_signal_callback_new(Etk_Signal *signal, Etk_Signal_Callback_Function callback, void *data, Etk_Bool swapped);
void etk_signal_callback_del(Etk_Signal_Callback *signal_callback);
void etk_signal_callback_call(Etk_Signal_Callback *callback, Etk_Object *object, void *return_value, ...);
void etk_signal_callback_call_valist(Etk_Signal_Callback *callback, Etk_Object *object, void *return_value, va_list args);

/** @} */

#endif
