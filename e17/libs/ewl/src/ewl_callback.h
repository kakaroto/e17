
/*\
|*|
|*| Functions to add/remove/call callbacks.
|*| each callback you append will be given a own specific number
|*| which you should keep if you want to remove the callback later on.
|*|
\*/

#ifndef __EWL_CALLBACK_H__
#define __EWL_CALLBACK_H__

typedef struct _ewl_callback Ewl_Callback;
#define EWL_CALLBACK(callback) ((Ewl_Callback *) callback)

typedef void (*Ewl_Cb_Func) (Ewl_Widget * widget, void *event_data,
			     void *user_data);
struct _ewl_callback
{
	Ewl_Widget *widget;
	Ewl_Callback_Type type;
	Ewl_Cb_Func func;
	void *event_data;
	void *user_data;
	int id;
};

int ewl_callback_append(Ewl_Widget * widget, Ewl_Callback_Type type,
			Ewl_Cb_Func func, void *user_data);
int ewl_callback_prepend(Ewl_Widget * widget, Ewl_Callback_Type type,
			 Ewl_Cb_Func func, void *user_data);
void ewl_callback_del(Ewl_Widget * widget, Ewl_Callback_Type type, int cb_id);
void ewl_callback_clear(Ewl_Widget * widget);
void ewl_callback_call(Ewl_Widget * widget, Ewl_Callback_Type type);
void ewl_callback_call_with_event_data(Ewl_Widget * widget,
				       Ewl_Callback_Type type,
				       void *event_data);
void ewl_callback_set_user_data(Ewl_Widget * w, Ewl_Callback_Type type,
				Ewl_Cb_Func func, void *user_data);
void ewl_callback_del_type(Ewl_Widget * w, Ewl_Callback_Type t);

#endif
