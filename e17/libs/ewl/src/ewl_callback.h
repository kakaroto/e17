
#ifndef __EWL_CALLBACK_H__
#define __EWL_CALLBACK_H__

typedef struct _ewl_callback Ewl_Callback;

#define EWL_CALLBACK(callback) ((Ewl_Callback *) callback)
#define EWL_CALLBACK_FUNCTION(cb_func) ((Ewl_Callback_Function) cb_func)

typedef void (*Ewl_Callback_Function) (Ewl_Widget * widget, void *ev_data,
				       void *user_data);
struct _ewl_callback
{
	Ewl_Callback_Function func;
	void *user_data;
	int references;
	int id;
};

void ewl_callbacks_init();
int ewl_callback_append(Ewl_Widget * widget, Ewl_Callback_Type type,
			Ewl_Callback_Function func, void *user_data);
int ewl_callback_prepend(Ewl_Widget * widget, Ewl_Callback_Type type,
			 Ewl_Callback_Function func, void *user_data);
void ewl_callback_clear(Ewl_Widget * widget);
void ewl_callback_call(Ewl_Widget * widget, Ewl_Callback_Type type);
void ewl_callback_call_with_event_data(Ewl_Widget * widget,
				       Ewl_Callback_Type type,
				       void *event_data);
void ewl_callback_del_type(Ewl_Widget * w, Ewl_Callback_Type t);
void ewl_callback_del(Ewl_Widget * w, Ewl_Callback_Type t,
		      Ewl_Callback_Function f);

#endif /* __EWL_CALLBACK_H__ */
