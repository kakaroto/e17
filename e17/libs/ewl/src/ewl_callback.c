#include <Ewl.h>

static int callback_id = 0;

/**
 * ewl_callback_append - append a callback of the specified type
 * @w: the widget to attach the callback
 * @t: the type of the callback that is being attached
 * @f: the function to attach as a callback
 * @user_data: the data to be passed to the callback function
 *
 * Returns 0 on failure, the id of the new callback on success. Allocates a
 * new callback for the specified widget that calls @f with @user_data as the
 * data parameter when event @t occurs. This event is placed at the end of the
 * callback chain.
 */
int
ewl_callback_append(Ewl_Widget * w, Ewl_Callback_Type t, Ewl_Cb_Func f,
		    void *user_data)
{
	Ewl_Callback *cb;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("w", w, -1);
	DCHECK_PARAM_PTR_RET("f", f, -1);

	cb = NEW(Ewl_Callback, 1);
	if (!cb)
		DRETURN_INT(0, DLEVEL_STABLE);

	ZERO(cb, Ewl_Callback, 1);
	cb->widget = w;
	cb->func = f;
	cb->user_data = user_data;
	cb->type = t;

	if (!w->callbacks[t])
		w->callbacks[t] = ewd_list_new();

	ewd_list_append(w->callbacks[t], cb);

	cb->id = ++callback_id;

	DRETURN_INT(cb->id, DLEVEL_STABLE);
}

/**
 * ewl_callback_prepend - prepend a callback of the specified type
 * @w: the widget to attach the callback
 * @t: the type of the callback that is being attached
 * @f: the function to attach as a callback
 * @user_data: the data to be passed to the callback function
 *
 * Returns 0 on failure, the id of the new callback on success. Same
 * functionality as ewl_callback_append, but the callback is placed at the
 * beginning of the callback chain.
 */
int
ewl_callback_prepend(Ewl_Widget * w, Ewl_Callback_Type t, Ewl_Cb_Func f,
		     void *user_data)
{
	Ewl_Callback *cb;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("w", w, -1);
	DCHECK_PARAM_PTR_RET("f", f, -1);

	cb = NEW(Ewl_Callback, 1);
	if (!cb)
		DRETURN_INT(0, DLEVEL_STABLE);

	ZERO(cb, Ewl_Callback, 1);
	cb->widget = w;
	cb->func = f;
	cb->user_data = user_data;
	cb->type = t;

	if (!w->callbacks[t])
		w->callbacks[t] = ewd_list_new();

	ewd_list_prepend(w->callbacks[t], cb);

	cb->id = ++callback_id;

	DRETURN_INT(cb->id, DLEVEL_STABLE);
}

/**
 * ewl_callback_call - execute callbacks of specified types for the widget
 * @w: the widget to execute the callbacks
 * @t: the type of the callbacks to be executed
 *
 * Returns no value. Executes the callback chain for the specified widget @w,
 * with event @t.
 */
void
ewl_callback_call(Ewl_Widget * w, Ewl_Callback_Type t)
{
	Ewd_List *cb_list = NULL;
	Ewl_Callback *cb = NULL;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);

	cb_list = w->callbacks[t];

	if (!cb_list || ewd_list_is_empty(cb_list))
		DRETURN(DLEVEL_STABLE);

	ewd_list_goto_first(cb_list);

	while (w && (cb_list = w->callbacks[t])
	       && (cb = ewd_list_next(cb_list)))
	  {
		  if (cb->func)
			  cb->func(w, cb->event_data, cb->user_data);
	  }

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * ewl_callback_call - execute callbacks of specified types with event data
 * @w: the widget to execute the callbacks
 * @t: the type of the callbacks to be executed
 * @ev_data: the event data to pass to the callbacks
 *
 * Returns no value. Similar to ewl_callback_call, but the event data is
 * substituted by @ev_data.
 */
void
ewl_callback_call_with_event_data(Ewl_Widget * w, Ewl_Callback_Type t,
				  void *ev_data)
{
	Ewl_Callback *cb;
	Ewd_List *cb_list;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);

	cb_list = w->callbacks[t];
	if (!cb_list || ewd_list_is_empty(cb_list))
		DRETURN(DLEVEL_STABLE);

	ewd_list_goto_first(cb_list);

	while ((cb_list = w->callbacks[t]) && (cb = ewd_list_next(cb_list)))
	  {
		  if (cb->func)
			  cb->func(w, ev_data, cb->user_data);
	  }

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * ewl_callback_set_user_data - set the user data for the specified callback
 * @w: the widget to search for the callback
 * @type: the type of the callback to be changed
 * @func: the function whose callback data will be changed
 * @user_data: the new data to pass into the callback
 *
 * Returns no value. Change the user data for the specified callback to
 * @user_data.
 */
void
ewl_callback_set_user_data(Ewl_Widget * w, Ewl_Callback_Type type,
			   Ewl_Cb_Func func, void *user_data)
{
	Ewl_Callback *cb;

	DENTER_FUNCTION(DLEVEL_STABLE);

	if (!w->callbacks[type] || ewd_list_is_empty(w->callbacks[type]))
		DRETURN(DLEVEL_STABLE);

	ewd_list_goto_first(w->callbacks[type]);

	while ((cb = ewd_list_next(w->callbacks[type])) != NULL)
	  {
		  if (cb->func == func)
		    {
			    cb->user_data = user_data;
			    break;
		    }
	  }

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * ewl_callback_del_type - delete all callbacks of the specified type
 * @w: the widget to delete the callbacks
 * @t: the type of the callbacks to be deleted
 *
 * Returns no value. Delete all callbacks of type @t from widget @w.
 */
void
ewl_callback_del_type(Ewl_Widget * w, Ewl_Callback_Type t)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);

	if (!w->callbacks[t])
		DRETURN(DLEVEL_STABLE);

	ewd_list_clear(w->callbacks[t]);
	ewd_list_destroy(w->callbacks[t]);
	w->callbacks[t] = NULL;

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * ewl_callback_del_cb_id - delete the specified callback id from the widget
 * @w: the widget to delete the id
 * @t: the type of event the callback is attached to
 * @cb_id: the id of the callback to delete
 *
 * Returns no value. Delete the specified callback id from the widget @w.
 */
void
ewl_callback_del_cb_id(Ewl_Widget * w, Ewl_Callback_Type t, int cb_id)
{
	Ewl_Callback *cb;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);

	if (!w->callbacks[t] ||
	    ewd_list_is_empty(w->callbacks[t]) || cb_id > callback_id)
		DRETURN(DLEVEL_STABLE);

	while ((cb = ewd_list_next(w->callbacks[t])) != NULL)
		if (cb->id == cb_id)
		  {
			  ewd_list_remove(w->callbacks[t]);
			  FREE(cb);
			  break;
		  }

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}


/**
 * ewl_callback_clear - remove all callbacks from the specified widget
 * @w: the widget to remove the callbacks
 *
 * Returns no value. Removes and frees all callbacks associated with widget
 * @w.
 */
void
ewl_callback_clear(Ewl_Widget * w)
{
	int i;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);

	for (i = 0; i < EWL_CALLBACK_MAX; i++)
	  {
		  if (w->callbacks[i])
			  ewl_callback_del_type(w, i);
	  }


	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * ewl_callback_del - delete the specified callback function from the widget
 * @w: the widget to delete the callback
 * @t: the type of event associated with the callback
 * @f: the function called by the callback
 *
 * Returns no value. Delete and frees the callback that calls function @f when
 * event @t occurs to widget @w.
 */
void
ewl_callback_del(Ewl_Widget * w, Ewl_Callback_Type t, Ewl_Cb_Func f)
{
	Ewl_Callback *cb;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);

	if (!w->callbacks[t] || ewd_list_is_empty(w->callbacks[t]))
		DRETURN(DLEVEL_STABLE);

	ewd_list_goto_first(w->callbacks[t]);

	while ((cb = ewd_list_current(w->callbacks[t])) != NULL)
	  {
		  if (cb->func == f)
		    {
			    ewd_list_remove(w->callbacks[t]);
			    break;
		    }
		  ewd_list_next(w->callbacks[t]);
	  }

	IF_FREE(cb);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}
