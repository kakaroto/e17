#ifndef __EWL_CALLBACK_H__
#define __EWL_CALLBACK_H__

/**
 * @defgroup Ewl_Callback Callback: The Callback Mechanisms
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
 * @def EWL_CALLBACK_FUNCTION(cb_func)
 * Typecasts a pointer to an Ewl_Callback_Function pointer.
 */
#define EWL_CALLBACK_FUNCTION(cb_func) ((Ewl_Callback_Function) cb_func)

/**
 * A shortcut for declaring functions that take a callback funciton pointer.
 */
typedef void    (*Ewl_Callback_Function) (Ewl_Widget * widget, void *ev_data,
					  void *user_data);
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
 * @def EWL_CALLBACK_LIST_POINTER(w, t)
 * Retrives the callback list from a widget for a certain event type.
 */
#define EWL_CALLBACK_LIST_POINTER(w, t) \
		(void *)((unsigned int)(w->callbacks[t]) & \
					~EWL_CALLBACK_NOTIFY_MASK)

/**
 * @def EWL_CALLBACK_FLAGS(w, t)
 * Retrives the callback flags from a widget for a certain event type.
 */
#define EWL_CALLBACK_FLAGS(w, t) \
		((unsigned int)(w->callbacks[t]) & \
					EWL_CALLBACK_NOTIFY_MASK)
/**
 * @def EWL_CALLBACK_FLAG_INTERCEPT(w, t)
 * Sets the callback intercept flag from a widget for a certain event type.
 */
#define EWL_CALLBACK_FLAG_INTERCEPT(w, t) \
		((unsigned int)w->callbacks[t] = \
			 (unsigned int)EWL_CALLBACK_LIST_POINTER(w, t) | \
			 EWL_CALLBACK_NOTIFY_INTERCEPT)

/**
 * @def EWL_CALLBACK_FLAG_NOTIFY(w, t)
 * Sets the callback notify flag from a widget for a certain event type.
 */
#define EWL_CALLBACK_FLAG_NOTIFY(w, t) \
		((unsigned int)w->callbacks[t] = \
			 (unsigned int)EWL_CALLBACK_LIST_POINTER(w, t) | \
			 EWL_CALLBACK_NOTIFY_NOTIFY)

/**
 * @def EWL_CALLBACK_LIST_ASSIGN(w, t, l)
 * Sets the callback list for a widget for a certain event type.
 */
#define EWL_CALLBACK_LIST_ASSIGN(w, t, l) \
		(unsigned int)w->callbacks[t] = (unsigned int)l | \
			((unsigned int)w->callbacks[t] & \
			 EWL_CALLBACK_NOTIFY_MASK)

void            ewl_callbacks_init(void);
void            ewl_callbacks_deinit(void);
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

#endif				/* __EWL_CALLBACK_H__ */
