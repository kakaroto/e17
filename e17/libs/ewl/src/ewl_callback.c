#include <Ewl.h>

int
ewl_callback_append(Ewl_Widget * widget, Ewl_Callback_Type type,
			   void * func, void * func_data)
{
	Ewl_Callback * callback = NULL;

	CHECK_PARAM_POINTER_RETURN("widget", widget, -1);

	callback = NEW(Ewl_Callback, 1);

	memset(callback, 0, sizeof(Ewl_Callback));

	callback->widget = widget;
	callback->func = func;
	callback->func_data = func_data;
	callback->type = type;

	if (!widget->callbacks[type])	
		widget->callbacks[type] = ewd_list_new();

	ewd_list_append(widget->callbacks[type], callback);

	return widget->callbacks[type]->index;
}

int
ewl_callback_prepend(Ewl_Widget * widget, Ewl_Callback_Type type,
				void * func, void * func_data)
{
	Ewl_Callback * callback = NULL;

	CHECK_PARAM_POINTER_RETURN("widget", widget, -1);

	callback = NEW(Ewl_Callback, 1);;
	callback = memset(callback, 0, sizeof(Ewl_Callback));

	callback->widget = widget;
	callback->func = func;
	callback->func_data = func_data;
	callback->type = type;

	if (!widget->callbacks[type])
		widget->callbacks[type] = ewd_list_new();

	ewd_list_prepend(widget->callbacks[type], callback);

	return widget->callbacks[type]->index;
}

void
ewl_callback_del(Ewl_Widget * widget, Ewl_Callback_Type type,
					int callback_number)
{
	CHECK_PARAM_POINTER("widget", widget);

	if (!widget->callbacks[type] ||
		ewd_list_is_empty(widget->callbacks[type]) ||
		callback_number < widget->callbacks[type]->nodes)
		return;

	ewd_list_goto_index(widget->callbacks[type], callback_number);
	ewd_list_remove(widget->callbacks[type]);
}

void ewl_callback_del_all(Ewl_Widget * widget);

void
ewl_callback_call(Ewl_Widget * widget, Ewl_Callback_Type type)
{
	Ewl_Callback * callback = NULL;

	CHECK_PARAM_POINTER("widget", widget);

	if (!widget->callbacks[type] || ewd_list_is_empty(widget->callbacks[type]))
		return;

	ewd_list_goto_first(widget->callbacks[type]);

	while ((callback = ewd_list_next(widget->callbacks[type])) != NULL) {
			callback->func(widget, callback->func_data);
	}
}

void ewl_callback_call_with_data(Ewl_Widget * widget,
							   Ewl_Callback_Type type, void * func_data)
{
	Ewl_Callback * callback = NULL;

	CHECK_PARAM_POINTER("widget", widget);

	if (!widget->callbacks[type] || ewd_list_is_empty(widget->callbacks[type]))
		return;

	ewd_list_goto_first(widget->callbacks[type]);

	while ((callback = ewd_list_next(widget->callbacks[type])) != NULL) {
		callback->func(widget, func_data);
	}
}
