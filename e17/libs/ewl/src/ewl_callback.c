#include <Ewl.h>

static int callback_id = 0;

int
ewl_callback_append(Ewl_Widget * widget, Ewl_Callback_Type type,
		    Ewl_Cb_Func func, void *user_data)
{
	Ewl_Callback *callback = NULL;

	DENTER_FUNCTION;

	CHECK_PARAM_POINTER_RETURN("widget", widget, -1);

	callback = NEW(Ewl_Callback, 1);
	memset(callback, 0, sizeof(Ewl_Callback));

	callback->widget = widget;
	callback->func = func;
	callback->user_data = user_data;
	callback->type = type;

	if (!widget->callbacks[type])
		widget->callbacks[type] = ewd_list_new();

	ewd_list_append(widget->callbacks[type], callback);

	callback->id = ++callback_id;

	DRETURN_INT(callback->id);
}

int
ewl_callback_prepend(Ewl_Widget * widget, Ewl_Callback_Type type,
		     Ewl_Cb_Func func, void *user_data)
{
	Ewl_Callback *callback = NULL;

	DENTER_FUNCTION;

	CHECK_PARAM_POINTER_RETURN("widget", widget, -1);

	callback = NEW(Ewl_Callback, 1);;
	callback = memset(callback, 0, sizeof(Ewl_Callback));

	callback->widget = widget;
	callback->func = func;
	callback->user_data = user_data;
	callback->type = type;

	if (!widget->callbacks[type])
		widget->callbacks[type] = ewd_list_new();

	ewd_list_prepend(widget->callbacks[type], callback);

	callback->id = ++callback_id;

	DRETURN_INT(callback->id);
}

void
ewl_callback_del(Ewl_Widget * widget, Ewl_Callback_Type type, int cb_id)
{
	Ewl_Callback *cb;

	DENTER_FUNCTION;

	CHECK_PARAM_POINTER("widget", widget);

	if (!widget->callbacks[type] ||
	    ewd_list_is_empty(widget->callbacks[type]) || cb_id > callback_id)
		DRETURN;

	while ((cb = ewd_list_next(widget->callbacks[type])) != NULL)
		if (cb->id == cb_id)
		  {
			  ewd_list_remove(widget->callbacks[type]);
			  FREE(cb);
			  break;
		  }

	DLEAVE_FUNCTION;
}

void
ewl_callback_del_all(Ewl_Widget * w)
{
	int i;

	DENTER_FUNCTION;
	DCHECK_PARAM_PTR("w", w);

	for (i = 0; i < EWL_CALLBACK_MAX; i++)
	  {
		  if (w->callbacks[i])
			  ewd_list_destroy(w->callbacks[i]);
	  }


	DLEAVE_FUNCTION;
}

void
ewl_callback_call(Ewl_Widget * widget, Ewl_Callback_Type type)
{
	Ewl_Callback *callback = NULL;

	DENTER_FUNCTION;

	CHECK_PARAM_POINTER("widget", widget);

	if (!widget->callbacks[type] ||
	    ewd_list_is_empty(widget->callbacks[type]))
		DRETURN;

	ewd_list_goto_first(widget->callbacks[type]);

	while ((callback = ewd_list_next(widget->callbacks[type])) != NULL)
		callback->func(widget, callback->event_data,
			       callback->user_data);

	DLEAVE_FUNCTION;
}

void
ewl_callback_call_with_event_data(Ewl_Widget * widget, Ewl_Callback_Type type,
				  void *event_data)
{
	Ewl_Callback *callback;

	DCHECK_PARAM_PTR("widget", widget);

	if (!widget->callbacks[type] ||
	    ewd_list_is_empty(widget->callbacks[type]))
		DRETURN;

	ewd_list_goto_first(widget->callbacks[type]);

	while ((callback = ewd_list_next(widget->callbacks[type])) != NULL)
		callback->func(widget, event_data, callback->user_data);

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
