#include "ewl_test.h"

Ewl_Widget     *tmp_win;

static Ewl_Widget *spinner_button;
static Ewl_Widget *spinner[4];

static void __spinner_window_configure(Ewl_Widget * w, void *ev_data,
				     void *user_data);

static void
__destroy_spinner_test_window(Ewl_Widget * w, void *ev_data __UNUSED__,
					void *user_data __UNUSED__)
{
	ewl_widget_destroy(w);
	ewl_callback_append(spinner_button, EWL_CALLBACK_CLICKED,
			    __create_spinner_test_window, NULL);
	ewl_widget_destroy(tmp_win);
}

static void
__spinner_value_changed(Ewl_Widget * w __UNUSED__, void *ev_data __UNUSED__,
					void *user_data __UNUSED__)
{
	int             xx, yy, ww, hh;

	xx = (int) (ewl_spinner_value_get(EWL_SPINNER(spinner[0])));
	yy = (int) (ewl_spinner_value_get(EWL_SPINNER(spinner[1])));
	ww = (int) (ewl_spinner_value_get(EWL_SPINNER(spinner[2])));
	hh = (int) (ewl_spinner_value_get(EWL_SPINNER(spinner[3])));

	ewl_callback_del(tmp_win, EWL_CALLBACK_CONFIGURE,
			 __spinner_window_configure);

	ewl_window_move(EWL_WINDOW(tmp_win), xx, yy);
	ewl_object_size_request(EWL_OBJECT(tmp_win), ww, hh);

	ewl_callback_append(tmp_win, EWL_CALLBACK_CONFIGURE,
			    __spinner_window_configure, NULL);
}

static void
__spinner_window_configure(Ewl_Widget * w __UNUSED__, void *ev_data __UNUSED__,
						void *user_data __UNUSED__)
{
	int xx, yy, ww, hh;

	ewl_window_position_get(EWL_WINDOW(tmp_win), &xx, &yy);
	ewl_object_current_size_get(EWL_OBJECT(tmp_win), &ww, &hh);

	ewl_spinner_value_set(EWL_SPINNER(spinner[0]), (double) (xx));
	ewl_spinner_value_set(EWL_SPINNER(spinner[1]), (double) (yy));
	ewl_spinner_value_set(EWL_SPINNER(spinner[2]), (double) (ww));
	ewl_spinner_value_set(EWL_SPINNER(spinner[3]), (double) (hh));
}

void
__create_spinner_test_window(Ewl_Widget * w, void *ev_data __UNUSED__,
					void *user_data __UNUSED__)
{
	Ewl_Widget     *spinner_win;
	Ewl_Widget     *spinner_box;
	Ewl_Widget     *spinner_row;
	Ewl_Widget     *text[4];
	Ewl_Widget     *separator[3];
	int             xx, yy, ww, hh, mw, mh;

	spinner_button = w;

	spinner_win = ewl_window_new();
	ewl_window_title_set(EWL_WINDOW(spinner_win), "STest");
	ewl_window_name_set(EWL_WINDOW(spinner_win), "EWL Test Application");
	ewl_window_class_set(EWL_WINDOW(spinner_win), "EFL Test Application");

	if (w) {
		ewl_callback_del(w, EWL_CALLBACK_CLICKED, 
				__create_spinner_test_window);
		ewl_callback_append(spinner_win, EWL_CALLBACK_DELETE_WINDOW,
				    __destroy_spinner_test_window, NULL);
	} else
		ewl_callback_append(spinner_win, EWL_CALLBACK_DELETE_WINDOW,
					__close_main_window, NULL);
	ewl_widget_show(spinner_win);

	/*
	 * Create the main box for holding the widgets
	 */
	spinner_box = ewl_vbox_new();
	ewl_container_child_append(EWL_CONTAINER(spinner_win), spinner_box);
	ewl_box_spacing_set(EWL_BOX(spinner_box), 10);
	ewl_widget_show(spinner_box);

	tmp_win = ewl_window_new();
	ewl_callback_append(tmp_win, EWL_CALLBACK_CONFIGURE,
			    __spinner_window_configure, NULL);
	ewl_widget_show(tmp_win);
	ewl_window_borderless_set(EWL_WINDOW(tmp_win));

	/*
	 * Grab the current coordinates of the window we will manipulate.
	 */
	ewl_object_current_geometry_get(EWL_OBJECT(tmp_win), &xx, &yy, &ww,
					&hh);
	ewl_object_minimum_size_get(EWL_OBJECT(tmp_win), &mw, &mh);

	spinner_row = ewl_hbox_new();
	ewl_container_child_append(EWL_CONTAINER(spinner_box), spinner_row);
	ewl_widget_show(spinner_row);

	text[0] = ewl_text_new();
	ewl_text_text_set(EWL_TEXT(text[0]), "X");
	ewl_object_fill_policy_set(EWL_OBJECT(text[0]), EWL_FLAG_FILL_NONE);
	ewl_object_alignment_set(EWL_OBJECT(text[0]), EWL_FLAG_ALIGN_CENTER);
	ewl_container_child_append(EWL_CONTAINER(spinner_row), text[0]);
	ewl_widget_show(text[0]);

	spinner[0] = ewl_spinner_new();
	ewl_spinner_digits_set(EWL_SPINNER(spinner[0]), 0);
	ewl_spinner_min_val_set(EWL_SPINNER(spinner[0]), 0);
	ewl_spinner_max_val_set(EWL_SPINNER(spinner[0]), 1280);
	ewl_spinner_value_set(EWL_SPINNER(spinner[0]), (double) (xx));
	ewl_spinner_step_set(EWL_SPINNER(spinner[0]), 1.0);
	ewl_container_child_append(EWL_CONTAINER(spinner_row), spinner[0]);
	ewl_callback_append(spinner[0], EWL_CALLBACK_VALUE_CHANGED,
			    __spinner_value_changed, NULL);
	ewl_widget_show(spinner[0]);

	separator[0] = ewl_hseparator_new();
	ewl_container_child_append(EWL_CONTAINER(spinner_box), separator[0]);
	ewl_widget_show(separator[0]);

	spinner_row = ewl_hbox_new();
	ewl_container_child_append(EWL_CONTAINER(spinner_box), spinner_row);
	ewl_widget_show(spinner_row);

	text[1] = ewl_text_new();
	ewl_text_text_set(EWL_TEXT(text[1]), "Y");
	ewl_object_fill_policy_set(EWL_OBJECT(text[1]), EWL_FLAG_FILL_NONE);
	ewl_object_alignment_set(EWL_OBJECT(text[1]), EWL_FLAG_ALIGN_CENTER);
	ewl_container_child_append(EWL_CONTAINER(spinner_row), text[1]);
	ewl_widget_show(text[1]);

	spinner[1] = ewl_spinner_new();
	ewl_spinner_digits_set(EWL_SPINNER(spinner[1]), 0);
	ewl_spinner_min_val_set(EWL_SPINNER(spinner[1]), 0);
	ewl_spinner_max_val_set(EWL_SPINNER(spinner[1]), 1024);
	ewl_spinner_value_set(EWL_SPINNER(spinner[1]), (double) (yy));
	ewl_spinner_step_set(EWL_SPINNER(spinner[1]), 1.0);
	ewl_container_child_append(EWL_CONTAINER(spinner_row), spinner[1]);
	ewl_callback_append(spinner[1], EWL_CALLBACK_VALUE_CHANGED,
			    __spinner_value_changed, NULL);
	ewl_widget_show(spinner[1]);

	separator[1] = ewl_hseparator_new();
	ewl_container_child_append(EWL_CONTAINER(spinner_box), separator[1]);
	ewl_widget_show(separator[1]);

	spinner_row = ewl_hbox_new();
	ewl_container_child_append(EWL_CONTAINER(spinner_box), spinner_row);
	ewl_widget_show(spinner_row);

	text[2] = ewl_text_new();
	ewl_text_text_set(EWL_TEXT(text[2]), "W");
	ewl_object_fill_policy_set(EWL_OBJECT(text[2]), EWL_FLAG_FILL_NONE);
	ewl_object_alignment_set(EWL_OBJECT(text[2]), EWL_FLAG_ALIGN_CENTER);
	ewl_container_child_append(EWL_CONTAINER(spinner_row), text[2]);
	ewl_widget_show(text[2]);

	spinner[2] = ewl_spinner_new();
	ewl_spinner_digits_set(EWL_SPINNER(spinner[2]), 0);
	ewl_spinner_min_val_set(EWL_SPINNER(spinner[2]), (double) (mw));
	ewl_spinner_max_val_set(EWL_SPINNER(spinner[2]), 1280);
	ewl_spinner_value_set(EWL_SPINNER(spinner[2]), (double) (ww));
	ewl_spinner_step_set(EWL_SPINNER(spinner[2]), 1.0);
	ewl_container_child_append(EWL_CONTAINER(spinner_row), spinner[2]);
	ewl_callback_append(spinner[2], EWL_CALLBACK_VALUE_CHANGED,
			    __spinner_value_changed, NULL);
	ewl_widget_show(spinner[2]);

	separator[2] = ewl_hseparator_new();
	ewl_container_child_append(EWL_CONTAINER(spinner_box), separator[2]);
	ewl_widget_show(separator[2]);

	spinner_row = ewl_hbox_new();
	ewl_container_child_append(EWL_CONTAINER(spinner_box), spinner_row);
	ewl_widget_show(spinner_row);

	text[3] = ewl_text_new();
	ewl_text_text_set(EWL_TEXT(text[3]), "H");
	ewl_object_fill_policy_set(EWL_OBJECT(text[3]), EWL_FLAG_FILL_NONE);
	ewl_object_alignment_set(EWL_OBJECT(text[3]), EWL_FLAG_ALIGN_CENTER);
	ewl_container_child_append(EWL_CONTAINER(spinner_row), text[3]);
	ewl_widget_show(text[3]);

	spinner[3] = ewl_spinner_new();
	ewl_spinner_digits_set(EWL_SPINNER(spinner[3]), 0);
	ewl_spinner_min_val_set(EWL_SPINNER(spinner[3]), (double) (mh));
	ewl_spinner_max_val_set(EWL_SPINNER(spinner[3]), 1024);
	ewl_spinner_value_set(EWL_SPINNER(spinner[3]), (double) (hh));
	ewl_spinner_step_set(EWL_SPINNER(spinner[3]), 1.0);
	ewl_container_child_append(EWL_CONTAINER(spinner_row), spinner[3]);
	ewl_callback_append(spinner[3], EWL_CALLBACK_VALUE_CHANGED,
			    __spinner_value_changed, NULL);
	ewl_widget_show(spinner[3]);
}

