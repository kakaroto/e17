#include <Ewl.h>

static unsigned int __ewl_callback_hash(void *key);
static int      __ewl_callback_compare(void *key1, void *key2);
static Ewl_Callback *__ewl_callback_register(Ewl_Callback * cb);
static void     __ewl_callback_unregister(Ewl_Callback * cb);

static int      callback_id = 0;
static Ewd_Hash *cb_registration = NULL;

/**
 * ewl_callbacks_init - setup internal registration variables for callbacks
 *
 * Returns no value. Sets up some important variables for tracking callbacks
 * that allow shared callbacks.
 *
 * W/o shared callbacks ewl_test with all windows open has a top line of:
 * 21279 ningerso  19   0 22972  22M  9412 R     6.0  8.0   0:40 ewl_test
 * With shared callbacks ewl_test with all windows open has a top line of:
 * 15901 ningerso  10   0 20120  19M  9148 S     0.0  7.0   0:34 ewl_test
 * 
 * So using shared callbacks saves us over 2 MB of memory in this case.
 */
void
ewl_callbacks_init()
{
	cb_registration = ewd_hash_new(__ewl_callback_hash,
				       __ewl_callback_compare);
}

/**
 * ewl_callbacks_deinit - destroy internal registration variables for callbacks
 *
 * Returns no value. Destroys some important variables for tracking callbacks
 * that allow shared callbacks.
 */
void
ewl_callbacks_deinit()
{
	ewd_hash_destroy(cb_registration);
}

/*
 * __ewl_callback_register - register a callback to check for duplicates
 * @cb: the callback to register
 *
 * Returns a pointer to the callback that should be used instead of the passed
 * @cb on success, NULL on failure. The returned callback may in fact be @cb,
 * but this can not be counted on. The callback @cb will be freed if this is
 * not the case.
 */
static Ewl_Callback *
__ewl_callback_register(Ewl_Callback * cb)
{
	Ewl_Callback   *found;

	DENTER_FUNCTION(DLEVEL_STABLE);

	DCHECK_PARAM_PTR_RET("cb", cb, NULL);

	found = ewd_hash_get(cb_registration, cb);
	if (!found) {
		cb->id = ++callback_id;
		ewd_hash_set(cb_registration, cb, cb);
		found = cb;
	} else
		FREE(cb);

	found->references++;

	DRETURN_PTR(found, DLEVEL_STABLE);
}

/*
 * __ewl_callback_unregister - unreference a callback and free if appropriate
 * @cb: the callback to unregister
 *
 * Returns no value. Checks to see if @cb has nay remaining references, if not
 * it is removed from the registration system and freed.
 */
static void
__ewl_callback_unregister(Ewl_Callback * cb)
{
	DENTER_FUNCTION(DLEVEL_STABLE);

	DCHECK_PARAM_PTR("cb", cb);

	cb->references--;
	if (cb->references < 1) {
		ewd_hash_remove(cb_registration, cb);
		FREE(cb);
	}

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

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
ewl_callback_append(Ewl_Widget * w, Ewl_Callback_Type t,
		    Ewl_Callback_Function f, void *user_data)
{
	Ewl_Callback   *cb;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("w", w, -1);
	DCHECK_PARAM_PTR_RET("f", f, -1);

	cb = NEW(Ewl_Callback, 1);
	if (!cb)
		DRETURN_INT(0, DLEVEL_STABLE);

	ZERO(cb, Ewl_Callback, 1);
	cb->func = f;
	cb->user_data = user_data;

	cb = __ewl_callback_register(cb);

	if (!w->callbacks[t])
		w->callbacks[t] = ewd_list_new();

	ewd_list_append(w->callbacks[t], cb);

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
ewl_callback_prepend(Ewl_Widget * w, Ewl_Callback_Type t,
		     Ewl_Callback_Function f, void *user_data)
{
	Ewl_Callback   *cb;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("w", w, -1);
	DCHECK_PARAM_PTR_RET("f", f, -1);

	cb = NEW(Ewl_Callback, 1);
	if (!cb)
		DRETURN_INT(0, DLEVEL_STABLE);

	ZERO(cb, Ewl_Callback, 1);
	cb->func = f;
	cb->user_data = user_data;

	cb = __ewl_callback_register(cb);

	if (!w->callbacks[t])
		w->callbacks[t] = ewd_list_new();

	ewd_list_prepend(w->callbacks[t], cb);

	DRETURN_INT(cb->id, DLEVEL_STABLE);
}

/**
 * ewl_callback_insert_after - add a callback after a previous callback in list
 * @w: the widget to insert the callback
 * @t: the type of the callback that is being attached
 * @f: the function to attach as a callback
 * @user_data: the data to be passed to the callback function
 *
 * Returns 0 on failure, the id of the new callback on success. Same
 * functionality as ewl_callback_append, but the callback is placed after the
 * specified callback on the callback chain.
 */
int
ewl_callback_insert_after(Ewl_Widget * w, Ewl_Callback_Type t,
			  Ewl_Callback_Function f, void *user_data,
			  Ewl_Callback_Function after, void *after_data)
{
	Ewl_Callback   *cb;
	Ewl_Callback   *search;

	DENTER_FUNCTION(DLEVEL_STABLE);

	DCHECK_PARAM_PTR_RET("w", w, 0);
	DCHECK_PARAM_PTR_RET("f", f, 0);

	cb = NEW(Ewl_Callback, 1);
	if (!cb)
		DRETURN_INT(0, DLEVEL_STABLE);

	ZERO(cb, Ewl_Callback, 1);
	cb->func = f;
	cb->user_data = user_data;

	cb = __ewl_callback_register(cb);

	if (!w->callbacks[t])
		w->callbacks[t] = ewd_list_new();

	/*
	 * Step 1 position past the callback we want to insert after.
	 */
	ewd_list_goto_first(w->callbacks[t]);
	while ((search = ewd_list_next(w->callbacks[t])) &&
	       (search->func != f || search->user_data != after_data));

	ewd_list_insert(w->callbacks[t], cb);

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
	Ewl_Callback   *cb = NULL;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);

	if (!w->callbacks[t] || ewd_list_is_empty(w->callbacks[t]))
		DRETURN(DLEVEL_STABLE);

	/*
	 * Loop through all the callbacks for this widget's event and execute
	 * them.
	 */
	ewd_list_goto_first(w->callbacks[t]);
	while (w->callbacks[t] && (cb = ewd_list_next(w->callbacks[t]))) {
		if (cb->func)
			cb->func(w, NULL, cb->user_data);
	}

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * ewl_callback_call_with_event_data - execute callbacks with event data
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
	Ewl_Callback   *cb;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);

	if (!w->callbacks[t] || ewd_list_is_empty(w->callbacks[t]))
		DRETURN(DLEVEL_STABLE);

	/*
	 * Loop through and execute each of the callbacks of a certain type for
	 * the specified widget.
	 */
	ewd_list_goto_first(w->callbacks[t]);
	while (w->callbacks[t] && (cb = ewd_list_next(w->callbacks[t]))) {
		if (cb->func)
			cb->func(w, ev_data, cb->user_data);
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
	Ewl_Callback   *rm;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);

	if (!w->callbacks[t])
		DRETURN(DLEVEL_STABLE);

	while ((rm = ewd_list_remove_first(w->callbacks[t])))
		__ewl_callback_unregister(rm);

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
	Ewl_Callback   *cb;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);

	if (!w->callbacks[t] ||
	    ewd_list_is_empty(w->callbacks[t]) || cb_id > callback_id)
		DRETURN(DLEVEL_STABLE);

	while ((cb = ewd_list_next(w->callbacks[t])) != NULL)
		if (cb->id == cb_id) {
			ewd_list_remove(w->callbacks[t]);
			__ewl_callback_unregister(cb);
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
	int             i;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);

	for (i = 0; i < EWL_CALLBACK_MAX; i++) {
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
ewl_callback_del(Ewl_Widget * w, Ewl_Callback_Type t, Ewl_Callback_Function f)
{
	Ewl_Callback   *cb;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);

	if (!w->callbacks[t] || ewd_list_is_empty(w->callbacks[t]))
		DRETURN(DLEVEL_STABLE);

	ewd_list_goto_first(w->callbacks[t]);

	while ((cb = ewd_list_current(w->callbacks[t])) != NULL) {
		if (cb->func == f) {
			ewd_list_remove(w->callbacks[t]);
			__ewl_callback_unregister(cb);
			break;
		}

		ewd_list_next(w->callbacks[t]);
	}

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/*
 * Hashes the value of a callback based on it's type, function, and user data.
 */
static unsigned int
__ewl_callback_hash(void *key)
{
	Ewl_Callback   *cb = EWL_CALLBACK(key);

	DENTER_FUNCTION(DLEVEL_STABLE);

	DCHECK_PARAM_PTR_RET("key", key, 0);

	DRETURN_INT((unsigned int) (cb->func) ^
		    (unsigned int) (cb->user_data), DLEVEL_STABLE);
}

/*
 * Simple comparison of callbacks, always returns -1 unless there is an exact
 * match, in which case it returns 0.
 */
static int
__ewl_callback_compare(void *key1, void *key2)
{
	Ewl_Callback   *cb1 = EWL_CALLBACK(key1);
	Ewl_Callback   *cb2 = EWL_CALLBACK(key2);

	DENTER_FUNCTION(DLEVEL_STABLE);

	DCHECK_PARAM_PTR_RET("key1", key1, -1)
		DCHECK_PARAM_PTR_RET("key2", key2, -1)
		if (cb1->func == cb2->func && cb1->user_data == cb2->user_data)
		DRETURN_INT(0, DLEVEL_STABLE);

	DRETURN_INT(-1, DLEVEL_STABLE);
}
