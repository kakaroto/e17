#ifndef __EWL_CALLBACK_H__
#define __EWL_CALLBACK_H__

/**
 * @defgroup Ewl_Callback The Callback Mechanisms
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
	Ewl_Callback_Function func; /**< Function to be executed when the event occurs. */

	/*
	 * The user specified data to pass to func when executed.
	 */
	void           *user_data;

	/*
	 * Reference counting to determine when this should be freed.
	 */
	int             references;

	/*
	 * The id of this callback which can be used for identifying it later.
	 */
	int             id;
};

#define EWL_CALLBACK_NOTIFY_MASK (0x3)
#define EWL_CALLBACK_LIST_POINTER(w, t) \
		(void *)((unsigned int)(w->callbacks[t]) & \
					~EWL_CALLBACK_NOTIFY_MASK)
#define EWL_CALLBACK_FLAGS(w, t) \
		((unsigned int)(w->callbacks[t]) & \
					EWL_CALLBACK_NOTIFY_MASK)
#define EWL_CALLBACK_FLAG_INTERCEPT(w, t) \
		((unsigned int)w->callbacks[t] = \
			 (unsigned int)EWL_CALLBACK_LIST_POINTER(w, t) | \
			 EWL_CALLBACK_NOTIFY_INTERCEPT)
#define EWL_CALLBACK_FLAG_NOTIFY(w, t) \
		((unsigned int)w->callbacks[t] = \
			 (unsigned int)EWL_CALLBACK_LIST_POINTER(w, t) | \
			 EWL_CALLBACK_NOTIFY_NOTIFY)
#define EWL_CALLBACK_LIST_ASSIGN(w, t, l) \
		(unsigned int)w->callbacks[t] = (unsigned int)l | \
			((unsigned int)w->callbacks[t] & \
			 EWL_CALLBACK_NOTIFY_MASK)

void            ewl_callbacks_init();
void            ewl_callbacks_deinit();
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

/**
 * @}
 */

#endif				/* __EWL_CALLBACK_H__ */
