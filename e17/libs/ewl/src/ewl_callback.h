
#ifndef __EWL_CALLBACK_H__
#define __EWL_CALLBACK_H__

typedef struct _ewl_callback Ewl_Callback;

struct _ewl_callback {
	Ewl_Widget              * widget;
	void					(* func) (Ewl_Widget * widget,
									  Ewl_Callback * cb);
	void					* func_data;
	void					* user_data;
	Ewl_Callback_Type		type;
	int						id;
};

int ewl_callback_append(Ewl_Widget * widget, Ewl_Callback_Type type,
						void (* func) (Ewl_Widget * widget, Ewl_Callback * cb),
						void * func_data);

void ewl_callback_del(Ewl_Widget * widget, Ewl_Callback_Type type,
					  int cb_id);
void ewl_callback_del_all(Ewl_Widget * widget);
void ewl_callback_call(Ewl_Widget * widget, Ewl_Callback_Type type);
void ewl_callback_call_with_data(Ewl_Widget * widget,
							   Ewl_Callback_Type type, void * func_data);

#endif
