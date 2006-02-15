#ifndef EWL_CALLBACK_H
#define EWL_CALLBACK_H

/**
 * @defgroup Ewl_Callback Ewl_Callback: The Callback Mechanisms
 * Defines methods for creating and modifying callbacks on widgets
 *
 * @{
 */

/**
 * The callbacks used internally for tracking event actions.
 */
typedef struct Ewl_Callback Ewl_Callback;

/**
 * @def EWL_CALLBACK(callback)
 * Typecasts a pointer to an Ewl_Callback pointer.
 */
#define EWL_CALLBACK(callback) ((Ewl_Callback *) callback)

/**
 * A shortcut for declaring functions that take a callback funciton pointer.
 */
typedef void    (*Ewl_Callback_Function) (Ewl_Widget * widget, void *ev_data,
					  void *user_data);

/**
 * @def EWL_CALLBACK_FUNCTION(cb_func)
 * Typecasts a pointer to an Ewl_Callback_Function pointer.
 */
#define EWL_CALLBACK_FUNCTION(cb_func) ((Ewl_Callback_Function) cb_func)

struct Ewl_Callback
{
	Ewl_Callback_Function func; /**< Function executed */
	void           *user_data; /**< user specified data to pass to func */
	int             references; /**< Reference counting */
	int             id; /**< id number of this callback */
};

/**
 * @def EWL_CALLBACK_NOTIFY_MASK
 * The value to binary AND with the callback pointer to check the notifiers.
 */
#define EWL_CALLBACK_NOTIFY_MASK (0x3)

/**
 * @def EWL_CALLBACK_LIST(w, t)
 * Retrives the callback list from a widget for a certain event type.
 */
#define EWL_CALLBACK_LIST(w, t) (w->callbacks[t].list)

/**
 * @def EWL_CALLBACK_FLAGS(w, t)
 * Retrives the callback flags from a widget for a certain event type.
 */
#define EWL_CALLBACK_FLAGS(w, t) (w->callbacks[t].mask)

/**
 * @def EWL_CALLBACK_LEN(w, t)
 * Retrives the length from a widget for a certain event type.
 */
#define EWL_CALLBACK_LEN(w, t) (w->callbacks[t].len)

/**
 * @def EWL_CALLBACK_POS(w, t)
 * Retrives the current callback position from a widget for an event type.
 */
#define EWL_CALLBACK_POS(w, t) w->callbacks[t].index

/**
 * @def EWL_CALLBACK_GET(w, t, i)
 * Retrives the callback struct at the given position
 */
#define EWL_CALLBACK_GET(w, t, i) \
	((w->callbacks[t].mask & EWL_CALLBACK_TYPE_DIRECT) ? w->callbacks[t].list : (w->callbacks[t].list ? w->callbacks[t].list[i] : NULL))

/**
 * @def EWL_CALLBACK_FLAG_INTERCEPT(w, t)
 * Sets the callback intercept flag from a widget for a certain event type.
 */
#define EWL_CALLBACK_FLAG_INTERCEPT(w, t) \
		w->callbacks[t].mask |= EWL_CALLBACK_NOTIFY_INTERCEPT

/**
 * @def EWL_CALLBACK_FLAG_NOINTERCEPT(w, t)
 * Clears the callback intercept flag from a widget for a certain event type.
 */
#define EWL_CALLBACK_FLAG_NOINTERCEPT(w, t) \
		w->callbacks[t].mask = w->callbacks[t].mask & ~EWL_CALLBACK_NOTIFY_INTERCEPT

/**
 * @def EWL_CALLBACK_FLAG_NOTIFY(w, t)
 * Sets the callback notify flag from a widget for a certain event type.
 */
#define EWL_CALLBACK_FLAG_NOTIFY(w, t) \
		w->callbacks[t].mask |= EWL_CALLBACK_NOTIFY_NOTIFY

/**
 * @def EWL_CALLBACK_SET_DIRECT(w, t)
 * Sets the callback direct flag for a centain event type
 */
#define EWL_CALLBACK_SET_DIRECT(w, t) \
		w->callbacks[t].mask |= EWL_CALLBACK_TYPE_DIRECT

/**
 * @def EWL_CALLBACK_SET_NODIRECT(w, t)
 * Clears the callback direct flag from a widget for a certain event type
 */
#define EWL_CALLBACK_SET_NODIRECT(w, t) \
		w->callbacks[t].mask &= ~EWL_CALLBACK_TYPE_DIRECT


int             ewl_callbacks_init(void);
void            ewl_callbacks_shutdown(void);
int             ewl_callback_append(Ewl_Widget * widget, Ewl_Callback_Type type,
				    Ewl_Callback_Function func,
				    void *user_data);
int             ewl_callback_prepend(Ewl_Widget * widget,
				     Ewl_Callback_Type type,
				     Ewl_Callback_Function func,
				     void *user_data);
int             ewl_callback_insert_after(Ewl_Widget * w, Ewl_Callback_Type t,
					  Ewl_Callback_Function f,
					  void *user_data,
					  Ewl_Callback_Function after,
					  void *after_data);
void            ewl_callback_clear(Ewl_Widget * widget);
void            ewl_callback_call(Ewl_Widget * widget, Ewl_Callback_Type type);
void            ewl_callback_call_with_event_data(Ewl_Widget * widget,
						  Ewl_Callback_Type type,
						  void *event_data);
void            ewl_callback_del_type(Ewl_Widget * w, Ewl_Callback_Type t);
void            ewl_callback_del(Ewl_Widget * w, Ewl_Callback_Type t,
				 Ewl_Callback_Function f);
void            ewl_callback_del_with_data(Ewl_Widget * w, Ewl_Callback_Type t,
				 Ewl_Callback_Function f, void *data);

/**
 * @}
 */

#endif
