#include "ewl_test.h"

Ewl_Widget *tooltip_button;

void __destroy_tooltip_test_window(Ewl_Widget * w, void *ev_data, void *user_data)
{
	ewl_widget_destroy(w);
	ewl_callback_append(tooltip_button, EWL_CALLBACK_CLICKED,
			__create_tooltip_test_window, NULL);

	return;
	ev_data = NULL;
	user_data = NULL;
}

void __create_tooltip_test_window(Ewl_Widget * w, void *ev_data, void *user_data)
{
	Ewl_Widget     *tooltip_win;
	Ewl_Widget     *tooltip_vbox;
	Ewl_Widget     *button;


	ewl_callback_del(w, EWL_CALLBACK_CLICKED, __create_tooltip_test_window);

	tooltip_button = w;

	tooltip_win = ewl_window_new();
	ewl_object_request_size(EWL_OBJECT(tooltip_win), 200, 100);
	ewl_callback_append(tooltip_win, EWL_CALLBACK_DELETE_WINDOW,
			__destroy_tooltip_test_window, NULL);
	ewl_widget_show(tooltip_win);
	
	tooltip_vbox = ewl_vbox_new();
	ewl_container_append_child(EWL_CONTAINER(tooltip_win), tooltip_vbox);
	ewl_box_set_spacing(EWL_BOX(tooltip_vbox), 0);
	ewl_widget_show(tooltip_vbox);

	button = ewl_button_new ("Hoover on this button");
	ewl_container_append_child(EWL_CONTAINER (tooltip_vbox), button);
	ewl_object_set_fill_policy(EWL_OBJECT(button), EWL_FLAG_FILL_SHRINK);
	ewl_widget_show (button);

	return;
	w = NULL;
	ev_data = NULL;
	user_data = NULL;
}
