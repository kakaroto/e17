#include <Ewl.h>

Ewl_Widget     *tmp_win;

static Ewl_Widget *spinner_button;
static Ewl_Widget *spinner[4];

void            __create_spinner_test_window(Ewl_Widget * w, void *ev_data,
					     void *user_data);
void
                __spinner_main_window_configure(Ewl_Widget * w, void *ev_data,
						void *user_data);

void
__destroy_spinner_test_window(Ewl_Widget * w, void *ev_data, void *user_data)
{
	ewl_widget_destroy(w);

	ewl_callback_append(spinner_button, EWL_CALLBACK_CLICKED,
			    __create_spinner_test_window, NULL);

	ewl_widget_destroy(tmp_win);

	return;
	ev_data = NULL;
	user_data = NULL;
}

void
__spinner_value_changed(Ewl_Widget * w, void *ev_data, void *user_data)
{
	int             xx, yy, ww, hh;

	xx = (int) (ewl_spinner_get_value(EWL_SPINNER(spinner[0])));
	yy = (int) (ewl_spinner_get_value(EWL_SPINNER(spinner[1])));
	ww = (int) (ewl_spinner_get_value(EWL_SPINNER(spinner[2])));
	hh = (int) (ewl_spinner_get_value(EWL_SPINNER(spinner[3])));

	ewl_callback_del(tmp_win, EWL_CALLBACK_CONFIGURE,
			 __spinner_main_window_configure);

	ewl_window_move(EWL_WINDOW(tmp_win), xx, yy);
	ewl_window_resize(EWL_WINDOW(tmp_win), ww, hh);

	ewl_callback_append(tmp_win, EWL_CALLBACK_CONFIGURE,
			    __spinner_main_window_configure, NULL);

	return;
	w = NULL;
	ev_data = NULL;
	user_data = NULL;
}

void
__spinner_main_window_configure(Ewl_Widget * w, void *ev_data, void *user_data)
{
	int             xx, yy, ww, hh;

	ewl_window_get_geometry(EWL_WINDOW(tmp_win), &xx, &yy, &ww, &hh);

	ewl_callback_del(spinner[0], EWL_CALLBACK_VALUE_CHANGED,
			 __spinner_value_changed);
	ewl_callback_del(spinner[1], EWL_CALLBACK_VALUE_CHANGED,
			 __spinner_value_changed);
	ewl_callback_del(spinner[2], EWL_CALLBACK_VALUE_CHANGED,
			 __spinner_value_changed);
	ewl_callback_del(spinner[3], EWL_CALLBACK_VALUE_CHANGED,
			 __spinner_value_changed);

	ewl_spinner_set_value(EWL_SPINNER(spinner[0]), (double) (xx));
	ewl_spinner_set_value(EWL_SPINNER(spinner[1]), (double) (yy));
	ewl_spinner_set_value(EWL_SPINNER(spinner[2]), (double) (ww));
	ewl_spinner_set_value(EWL_SPINNER(spinner[3]), (double) (hh));

	ewl_callback_append(spinner[0], EWL_CALLBACK_VALUE_CHANGED,
			    __spinner_value_changed, NULL);
	ewl_callback_append(spinner[1], EWL_CALLBACK_VALUE_CHANGED,
			    __spinner_value_changed, NULL);
	ewl_callback_append(spinner[2], EWL_CALLBACK_VALUE_CHANGED,
			    __spinner_value_changed, NULL);
	ewl_callback_append(spinner[3], EWL_CALLBACK_VALUE_CHANGED,
			    __spinner_value_changed, NULL);

	return;
	w = NULL;
	ev_data = NULL;
	user_data = NULL;
}

void
__create_spinner_test_window(Ewl_Widget * w, void *ev_data, void *user_data)
{
	Ewl_Widget     *spinner_win;
	Ewl_Widget     *spinner_box;
	Ewl_Widget     *text[4];
	Ewl_Widget     *separator[3];
	int             xx, yy, ww, hh, mw, mh;

	ewl_callback_del(w, EWL_CALLBACK_CLICKED, __create_spinner_test_window);

	spinner_button = w;

	spinner_win = ewl_window_new();
	ewl_window_set_min_size(EWL_WINDOW(spinner_win), 151, 316);
	ewl_callback_append(spinner_win, EWL_CALLBACK_DELETE_WINDOW,
			    __destroy_spinner_test_window, NULL);
	ewl_widget_show(spinner_win);

	/*
	 * Create the main box for holding the widgets
	 */
	spinner_box = ewl_vbox_new();
	ewl_container_append_child(EWL_CONTAINER(spinner_win), spinner_box);
	ewl_box_set_spacing(EWL_BOX(spinner_box), 10);
	ewl_widget_show(spinner_box);

	tmp_win = ewl_window_new();
	ewl_callback_append(tmp_win, EWL_CALLBACK_CONFIGURE,
			    __spinner_main_window_configure, NULL);
	ewl_widget_show(tmp_win);
	ewl_window_set_borderless(EWL_WINDOW(tmp_win));

	ewl_object_get_current_geometry(EWL_OBJECT(tmp_win), &xx, &yy, &ww,
					&hh);
	ewl_object_get_minimum_size(EWL_OBJECT(tmp_win), &mw, &mh);

	text[0] = ewl_text_new("X");
	ewl_container_append_child(EWL_CONTAINER(spinner_box), text[0]);
	ewl_widget_show(text[0]);

	spinner[0] = ewl_spinner_new();
	ewl_spinner_set_digits(EWL_SPINNER(spinner[0]), 0);
	ewl_spinner_set_min_val(EWL_SPINNER(spinner[0]), 0);
	ewl_spinner_set_max_val(EWL_SPINNER(spinner[0]), 1280);
	ewl_spinner_set_value(EWL_SPINNER(spinner[0]), (double) (xx));
	ewl_spinner_set_step(EWL_SPINNER(spinner[0]), 1.0);
	ewl_container_append_child(EWL_CONTAINER(spinner_box), spinner[0]);
	ewl_callback_append(spinner[0], EWL_CALLBACK_VALUE_CHANGED,
			    __spinner_value_changed, NULL);
	ewl_widget_show(spinner[0]);

	separator[0] = ewl_hseparator_new();
	ewl_container_append_child(EWL_CONTAINER(spinner_box), separator[0]);
	ewl_widget_show(separator[0]);

	text[1] = ewl_text_new("Y");
	ewl_container_append_child(EWL_CONTAINER(spinner_box), text[1]);
	ewl_widget_show(text[1]);

	spinner[1] = ewl_spinner_new();
	ewl_spinner_set_digits(EWL_SPINNER(spinner[1]), 0);
	ewl_spinner_set_min_val(EWL_SPINNER(spinner[1]), 0);
	ewl_spinner_set_max_val(EWL_SPINNER(spinner[1]), 1024);
	ewl_spinner_set_value(EWL_SPINNER(spinner[1]), (double) (yy));
	ewl_spinner_set_step(EWL_SPINNER(spinner[1]), 1.0);
	ewl_container_append_child(EWL_CONTAINER(spinner_box), spinner[1]);
	ewl_callback_append(spinner[1], EWL_CALLBACK_VALUE_CHANGED,
			    __spinner_value_changed, NULL);
	ewl_widget_show(spinner[1]);

	separator[1] = ewl_hseparator_new();
	ewl_container_append_child(EWL_CONTAINER(spinner_box), separator[1]);
	ewl_widget_show(separator[1]);

	text[2] = ewl_text_new("W");
	ewl_container_append_child(EWL_CONTAINER(spinner_box), text[2]);
	ewl_widget_show(text[2]);

	spinner[2] = ewl_spinner_new();
	ewl_spinner_set_digits(EWL_SPINNER(spinner[2]), 0);
	ewl_spinner_set_min_val(EWL_SPINNER(spinner[2]), (double) (mw));
	ewl_spinner_set_max_val(EWL_SPINNER(spinner[2]), 1280);
	ewl_spinner_set_value(EWL_SPINNER(spinner[2]), (double) (ww));
	ewl_spinner_set_step(EWL_SPINNER(spinner[2]), 1.0);
	ewl_container_append_child(EWL_CONTAINER(spinner_box), spinner[2]);
	ewl_callback_append(spinner[2], EWL_CALLBACK_VALUE_CHANGED,
			    __spinner_value_changed, NULL);
	ewl_widget_show(spinner[2]);

	separator[2] = ewl_hseparator_new();
	ewl_container_append_child(EWL_CONTAINER(spinner_box), separator[2]);
	ewl_widget_show(separator[2]);

	text[3] = ewl_text_new("H");
	ewl_container_append_child(EWL_CONTAINER(spinner_box), text[3]);
	ewl_widget_show(text[3]);

	spinner[3] = ewl_spinner_new();
	ewl_spinner_set_digits(EWL_SPINNER(spinner[3]), 0);
	ewl_spinner_set_min_val(EWL_SPINNER(spinner[3]), (double) (mh));
	ewl_spinner_set_max_val(EWL_SPINNER(spinner[3]), 1024);
	ewl_spinner_set_value(EWL_SPINNER(spinner[3]), (double) (hh));
	ewl_spinner_set_step(EWL_SPINNER(spinner[3]), 1.0);
	ewl_container_append_child(EWL_CONTAINER(spinner_box), spinner[3]);
	ewl_callback_append(spinner[3], EWL_CALLBACK_VALUE_CHANGED,
			    __spinner_value_changed, NULL);
	ewl_widget_show(spinner[3]);

	return;
	ev_data = NULL;
	user_data = NULL;
}
