#include <Ewl.h>

static int callback_id = 0;

int
ewl_callback_append(Ewl_Widget * w, Ewl_Callback_Type t, Ewl_Cb_Func f,
		    void *user_data)
{
	Ewl_Callback *cb;

	DENTER_FUNCTION;
	DCHECK_PARAM_PTR_RET("w", w, -1);
	DCHECK_PARAM_PTR_RET("f", f, -1);

	cb = NEW(Ewl_Callback, 1);
	if (!cb)
		DRETURN_INT(0);

	memset(cb, 0, sizeof(Ewl_Callback));
	cb->widget = w;
	cb->func = f;
	cb->user_data = user_data;
	cb->type = t;

	if (!w->callbacks[t])
		w->callbacks[t] = ewd_list_new();

	ewd_list_append(w->callbacks[t], cb);

	cb->id = ++callback_id;

	DRETURN_INT(cb->id);
}

int
ewl_callback_prepend(Ewl_Widget * w, Ewl_Callback_Type t, Ewl_Cb_Func f,
		     void *user_data)
{
	Ewl_Callback *cb;

	DENTER_FUNCTION;
	DCHECK_PARAM_PTR_RET("w", w, -1);
	DCHECK_PARAM_PTR_RET("f", f, -1);

	cb = NEW(Ewl_Callback, 1);
	if (!cb)
		DRETURN_INT(0);

	memset(cb, 0, sizeof(Ewl_Callback));
	cb->widget = w;
	cb->func = f;
	cb->user_data = user_data;
	cb->type = t;

	if (!w->callbacks[t])
		w->callbacks[t] = ewd_list_new();

	ewd_list_prepend(w->callbacks[t], cb);

	cb->id = ++callback_id;

	DRETURN_INT(cb->id);
}

void
ewl_callback_call(Ewl_Widget * w, Ewl_Callback_Type t)
{
	Ewd_List *cb_list = NULL;
	Ewl_Callback *cb = NULL;

	DENTER_FUNCTION;
	DCHECK_PARAM_PTR("w", w);

	cb_list = w->callbacks[t];

	if (!cb_list || ewd_list_is_empty(cb_list))
		DRETURN;

	ewd_list_goto_first(cb_list);

	while (cb_list && (cb = ewd_list_next(cb_list)))
	  {
		  if (cb->func)
			  cb->func(w, cb->event_data, cb->user_data);
	  }

	DLEAVE_FUNCTION;
}

void
ewl_callback_call_with_event_data(Ewl_Widget * w, Ewl_Callback_Type t,
				  void *ev_data)
{
	Ewl_Callback *cb;
	Ewd_List *cb_list;

	DENTER_FUNCTION;
	DCHECK_PARAM_PTR("w", w);

	cb_list = w->callbacks[t];
	if (!cb_list || ewd_list_is_empty(cb_list))
		DRETURN;

	ewd_list_goto_first(cb_list);
	while (cb_list && (cb = ewd_list_next(cb_list)))
	  {
		  if (cb->func)
			  cb->func(w, ev_data, cb->user_data);
	  }

	DLEAVE_FUNCTION;
}

void
ewl_callback_set_user_data(Ewl_Widget * w, Ewl_Callback_Type type,
			   Ewl_Cb_Func func, void *user_data)
{
	Ewl_Callback *cb;

	DENTER_FUNCTION;

	if (!w->callbacks[type] || ewd_list_is_empty(w->callbacks[type]))
		DRETURN;

	ewd_list_goto_first(w->callbacks[type]);

	while ((cb = ewd_list_next(w->callbacks[type])) != NULL)
	  {
		  if (cb->func == func)
		    {
			    cb->user_data = user_data;
			    break;
		    }
	  }

	DLEAVE_FUNCTION;
}

void
ewl_callback_del_type(Ewl_Widget * w, Ewl_Callback_Type t)
{
	DENTER_FUNCTION;
	DCHECK_PARAM_PTR("w", w);

	if (!w->callbacks[t])
		DRETURN;

	ewd_list_clear(w->callbacks[t]);
	ewd_list_destroy(w->callbacks[t]);
	w->callbacks[t] = NULL;

	DLEAVE_FUNCTION;
}

void
ewl_callback_del_cb_id(Ewl_Widget * w, Ewl_Callback_Type t, int cb_id)
{
	Ewl_Callback *cb;

	DENTER_FUNCTION;
	DCHECK_PARAM_PTR("w", w);

	if (!w->callbacks[t] ||
	    ewd_list_is_empty(w->callbacks[t]) || cb_id > callback_id)
		DRETURN;

	while ((cb = ewd_list_next(w->callbacks[t])) != NULL)
		if (cb->id == cb_id)
		  {
			  ewd_list_remove(w->callbacks[t]);
			  FREE(cb);
			  break;
		  }

	DLEAVE_FUNCTION;
}

void
ewl_callback_clear(Ewl_Widget * w)
{
	int i;

	DENTER_FUNCTION;
	DCHECK_PARAM_PTR("w", w);

	for (i = 0; i < EWL_CALLBACK_MAX; i++)
	  {
		  if (w->callbacks[i])
			  ewl_callback_del_type(w, i);
	  }


	DLEAVE_FUNCTION;
}

void
ewl_callback_del(Ewl_Widget * w, Ewl_Callback_Type t, Ewl_Cb_Func f)
{
	Ewl_Callback *cb;

	DENTER_FUNCTION;
	DCHECK_PARAM_PTR("w", w);

	if (!w->callbacks[t] || ewd_list_is_empty(w->callbacks[t]))
		DRETURN;

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

	DLEAVE_FUNCTION;
}
