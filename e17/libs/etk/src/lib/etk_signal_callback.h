/** @file etk_signal_callback.h */
#ifndef _ETK_SIGNAL_CALLBACK_H_
#define _ETK_SIGNAL_CALLBACK_H_

#include <stdarg.h>
#include "etk_types.h"

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
   Etk_Signal *signal;
   Etk_Callback callback;
   Etk_Bool swapped;
   Etk_Bool blocked;
   void *data;
};


Etk_Signal_Callback *etk_signal_callback_new(Etk_Signal *signal, Etk_Callback callback, void *data, Etk_Bool swapped);
void                 etk_signal_callback_del(Etk_Signal_Callback *signal_callback);

void     etk_signal_callback_call(Etk_Signal_Callback *callback, Etk_Object *object, void *return_value, ...);
void     etk_signal_callback_call_valist(Etk_Signal_Callback *callback, Etk_Object *object, void *return_value, va_list args);
void     etk_signal_callback_block(Etk_Signal_Callback *callback);
void     etk_signal_callback_unblock(Etk_Signal_Callback *callback);
Etk_Bool etk_signal_callback_is_blocked(Etk_Signal_Callback *callback);


/** @} */

#endif
