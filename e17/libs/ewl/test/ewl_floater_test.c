#include <Ewl.h>

static Ewl_Widget *floater_button = NULL;

void __create_floater_test_window(Ewl_Widget * w, void *ev_data,
				 void *user_data);

void
__destroy_floater_test_window(Ewl_Widget * w, void *ev_data, void *user_data)
{
	ewl_widget_destroy_recursive(w);

	ewl_callback_append(floater_button, EWL_CALLBACK_CLICKED,
			    __create_floater_test_window, NULL);

	return;
	ev_data = NULL;
	user_data = NULL;
}

void
__create_floater_test_window(Ewl_Widget * w, void *ev_data, void *user_data)
{
	Ewl_Widget *floater;
	Ewl_Widget *floater_win;
	Ewl_Widget *separator;
	Ewl_Widget *button[2];
	Ewl_Widget *check_button[2];
	Ewl_Widget *radio_button[2];

	ewl_callback_del(w, EWL_CALLBACK_CLICKED,
			 __create_floater_test_window);

	floater_button = w;

	floater_win= ewl_window_new();
	ewl_window_resize(floater_win, 145, 230);
	ewl_window_set_min_size(floater_win, 145, 230);
	ewl_box_set_spacing(EWL_BOX(floater_win), 10);
	ewl_callback_append(floater_win, EWL_CALLBACK_DELETE_WINDOW,
			    __destroy_floater_test_window, NULL);
	ewl_widget_show(floater_win);

	radio_button[0] = ewl_radiobutton_new("With Label");
	ewl_container_append_child(EWL_CONTAINER(floater_win),
				   radio_button[0]);
	ewl_object_set_alignment(EWL_OBJECT(radio_button[0]),
				 EWL_ALIGNMENT_LEFT);
	ewl_widget_show(radio_button[0]);

	radio_button[1] = ewl_radiobutton_new(NULL);
	ewl_radiobutton_set_chain(radio_button[1], radio_button[0]);
	ewl_container_append_child(EWL_CONTAINER(floater_win),
				   radio_button[1]);
	ewl_object_set_alignment(EWL_OBJECT(radio_button[1]),
				 EWL_ALIGNMENT_LEFT);
	ewl_widget_show(radio_button[1]);

	floater = ewl_floater_new(radio_button[1]);
	ewl_floater_set_position(EWL_FLOATER(floater), 20, 20);
	ewl_window_floater_add(EWL_WINDOW(floater_win), EWL_FLOATER(floater));
	ewl_widget_show(floater);

	button[0] = ewl_button_new("With Label");
	ewl_container_append_child(EWL_CONTAINER(floater), button[0]);
	ewl_object_set_alignment(EWL_OBJECT(button[0]), EWL_ALIGNMENT_LEFT);
	ewl_object_set_custom_size(EWL_OBJECT(button[0]), 100, 17);
	ewl_widget_show(button[0]);

	button[1] = ewl_button_new(NULL);
	ewl_container_append_child(EWL_CONTAINER(floater), button[1]);
	ewl_object_set_alignment(EWL_OBJECT(button[1]), EWL_ALIGNMENT_LEFT);
	ewl_object_set_custom_size(EWL_OBJECT(button[1]), 100, 17);
	ewl_widget_show(button[1]);

	separator = ewl_vseparator_new();
	ewl_container_append_child(EWL_CONTAINER(floater), separator);
	ewl_widget_realize(separator);

	ewl_object_set_padding(EWL_OBJECT(separator), 2, 2, 5, 5);

	check_button[0] = ewl_checkbutton_new("With Label");
	ewl_container_append_child(EWL_CONTAINER(floater),
				   check_button[0]);
	ewl_object_set_alignment(EWL_OBJECT(check_button[0]),
				 EWL_ALIGNMENT_LEFT);
	ewl_widget_show(check_button[0]);

	check_button[1] = ewl_checkbutton_new(NULL);
	ewl_container_append_child(EWL_CONTAINER(floater),
				   check_button[1]);
	ewl_object_set_alignment(EWL_OBJECT(check_button[1]),
				 EWL_ALIGNMENT_LEFT);
	ewl_widget_show(check_button[1]);

	return;
	w = NULL;
	ev_data = NULL;
	user_data = NULL;
}
