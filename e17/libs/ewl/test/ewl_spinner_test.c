#include <Ewl.h>

Ewl_Widget *tmp_win;

static Ewl_Widget *spinner_button;
static Ewl_Widget *spinner[4];

void __create_spinner_test_window(Ewl_Widget * w, void *ev_data,
				  void *user_data);
void
__spinner_main_window_configure(Ewl_Widget * w, void *ev_data,
				void *user_data);

void
__destroy_spinner_test_window(Ewl_Widget * w, void *ev_data, void *user_data)
{
	ewl_widget_destroy_recursive(w);

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
	int xx, yy, ww, hh;

	xx = (int) (ewl_spinner_get_value(spinner[0]));
	yy = (int) (ewl_spinner_get_value(spinner[1]));
	ww = (int) (ewl_spinner_get_value(spinner[2]));
	hh = (int) (ewl_spinner_get_value(spinner[3]));

	ewl_callback_del(tmp_win, EWL_CALLBACK_CONFIGURE,
			 __spinner_main_window_configure);

	ewl_window_move(tmp_win, xx, yy);
	ewl_window_resize(tmp_win, ww, hh);

	ewl_callback_append(tmp_win, EWL_CALLBACK_CONFIGURE,
			    __spinner_main_window_configure, NULL);

	return;
	w = NULL;
	ev_data = NULL;
	user_data = NULL;
}

void
__spinner_main_window_configure(Ewl_Widget * w, void *ev_data,
				void *user_data)
{
	int xx, yy, ww, hh;

	ewl_object_get_current_geometry(EWL_OBJECT(tmp_win), &xx, &yy, &ww,
					&hh);

	ewl_callback_del(spinner[0], EWL_CALLBACK_VALUE_CHANGED,
			 __spinner_value_changed);
	ewl_callback_del(spinner[1], EWL_CALLBACK_VALUE_CHANGED,
			 __spinner_value_changed);
	ewl_callback_del(spinner[2], EWL_CALLBACK_VALUE_CHANGED,
			 __spinner_value_changed);
	ewl_callback_del(spinner[3], EWL_CALLBACK_VALUE_CHANGED,
			 __spinner_value_changed);

	ewl_spinner_set_value(spinner[0], (double) (xx));
	ewl_spinner_set_value(spinner[1], (double) (yy));
	ewl_spinner_set_value(spinner[2], (double) (ww));
	ewl_spinner_set_value(spinner[3], (double) (hh));

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
	Ewl_Widget *spinner_win;
	Ewl_Widget *main_vbox;
	Ewl_Widget *text[4];
	Ewl_Widget *separator[3];
	int xx, yy, ww, hh, mw, mh;

	ewl_callback_del(w, EWL_CALLBACK_CLICKED,
			 __create_spinner_test_window);

	spinner_button = w;

	spinner_win = ewl_window_new();
	ewl_window_set_min_size(spinner_win, 151, 316);
	ewl_callback_append(spinner_win, EWL_CALLBACK_DELETE_WINDOW,
			    __destroy_spinner_test_window, NULL);
	ewl_widget_show(spinner_win);

	tmp_win = ewl_window_new();
	ewl_callback_append(tmp_win, EWL_CALLBACK_CONFIGURE,
			    __spinner_main_window_configure, NULL);
	ewl_widget_show(tmp_win);
	ewl_window_set_borderless(tmp_win);

	main_vbox = ewl_vbox_new();
	ewl_container_append_child(EWL_CONTAINER(spinner_win), main_vbox);
	ewl_box_set_spacing(main_vbox, 10);
	ewl_widget_show(main_vbox);

	ewl_object_get_current_geometry(EWL_OBJECT(tmp_win), &xx, &yy, &ww,
					&hh);
	ewl_object_get_minimum_size(EWL_OBJECT(tmp_win), &mw, &mh);

	text[0] = ewl_text_new();
	ewl_text_set_text(text[0], "X");
	ewl_container_append_child(EWL_CONTAINER(main_vbox), text[0]);
	ewl_widget_show(text[0]);

	spinner[0] = ewl_spinner_new();
	ewl_spinner_set_digits(spinner[0], 0);
	ewl_spinner_set_min_val(spinner[0], 0);
	ewl_spinner_set_max_val(spinner[0], 1280);
	ewl_spinner_set_value(spinner[0], (double) (xx));
	ewl_spinner_set_step(spinner[0], 1.0);
	ewl_container_append_child(EWL_CONTAINER(main_vbox), spinner[0]);
	ewl_callback_append(spinner[0], EWL_CALLBACK_VALUE_CHANGED,
			    __spinner_value_changed, NULL);
	ewl_widget_show(spinner[0]);

	separator[0] = ewl_vseparator_new();
	ewl_container_append_child(EWL_CONTAINER(main_vbox), separator[0]);
	ewl_widget_show(separator[0]);

	text[1] = ewl_text_new();
	ewl_text_set_text(text[1], "Y");
	ewl_container_append_child(EWL_CONTAINER(main_vbox), text[1]);
	ewl_widget_show(text[1]);

	spinner[1] = ewl_spinner_new();
	ewl_spinner_set_digits(spinner[1], 0);
	ewl_spinner_set_min_val(spinner[1], 0);
	ewl_spinner_set_max_val(spinner[1], 1024);
	ewl_spinner_set_value(spinner[1], (double) (yy));
	ewl_spinner_set_step(spinner[1], 1.0);
	ewl_container_append_child(EWL_CONTAINER(main_vbox), spinner[1]);
	ewl_callback_append(spinner[1], EWL_CALLBACK_VALUE_CHANGED,
			    __spinner_value_changed, NULL);
	ewl_widget_show(spinner[1]);

	separator[1] = ewl_vseparator_new();
	ewl_container_append_child(EWL_CONTAINER(main_vbox), separator[1]);
	ewl_widget_show(separator[1]);

	text[2] = ewl_text_new();
	ewl_text_set_text(text[2], "W");
	ewl_container_append_child(EWL_CONTAINER(main_vbox), text[2]);
	ewl_widget_show(text[2]);

	spinner[2] = ewl_spinner_new();
	ewl_spinner_set_digits(spinner[2], 0);
	ewl_spinner_set_min_val(spinner[2], (double) (mw));
	ewl_spinner_set_max_val(spinner[2], 1280);
	ewl_spinner_set_value(spinner[2], (double) (ww));
	ewl_spinner_set_step(spinner[2], 1.0);
	ewl_container_append_child(EWL_CONTAINER(main_vbox), spinner[2]);
	ewl_callback_append(spinner[2], EWL_CALLBACK_VALUE_CHANGED,
			    __spinner_value_changed, NULL);
	ewl_widget_show(spinner[2]);

	separator[2] = ewl_vseparator_new();
	ewl_container_append_child(EWL_CONTAINER(main_vbox), separator[2]);
	ewl_widget_show(separator[2]);

	text[3] = ewl_text_new();
	ewl_text_set_text(text[3], "H");
	ewl_container_append_child(EWL_CONTAINER(main_vbox), text[3]);
	ewl_widget_show(text[3]);

	spinner[3] = ewl_spinner_new();
	ewl_spinner_set_digits(spinner[3], 0);
	ewl_spinner_set_min_val(spinner[3], (double) (mh));
	ewl_spinner_set_max_val(spinner[3], 1024);
	ewl_spinner_set_value(spinner[3], (double) (hh));
	ewl_spinner_set_step(spinner[3], 1.0);
	ewl_container_append_child(EWL_CONTAINER(main_vbox), spinner[3]);
	ewl_callback_append(spinner[3], EWL_CALLBACK_VALUE_CHANGED,
			    __spinner_value_changed, NULL);
	ewl_widget_show(spinner[3]);

	return;
	ev_data = NULL;
	user_data = NULL;
}
