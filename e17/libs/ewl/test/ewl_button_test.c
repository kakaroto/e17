#include <Ewl.h>

extern int button_win_realized;

void
__destroy_button_test_window(Ewl_Widget * w, void * ev_data, void * user_data)
{
	ewl_widget_destroy_recursive(w);

	button_win_realized = 0;

	return;
	ev_data = NULL;
	user_data = NULL;
}

void
__create_button_test_window(Ewl_Widget * w, void * ev_data, void * user_data)
{
	Ewl_Widget * button_win;
	Ewl_Widget * main_vbox;
	Ewl_Widget * button[2];
	Ewl_Widget * check_button[2];
	Ewl_Widget * radio_button[2];

	if (button_win_realized)
		return;

	button_win_realized = 0;

	button_win = ewl_window_new();
	ewl_window_resize(button_win, 92, 167);
	ewl_callback_append(button_win, EWL_CALLBACK_DELETE_WINDOW,
		__destroy_button_test_window, NULL);
	ewl_widget_show(button_win);

	main_vbox = ewl_vbox_new();
	ewl_container_append_child(EWL_CONTAINER(button_win), main_vbox);
	ewl_box_set_spacing(main_vbox, 10);
	ewl_widget_show(main_vbox);

	button[0] = ewl_button_new("With Label");
	ewl_container_append_child(EWL_CONTAINER(main_vbox), button[0]);
	ewl_object_set_alignment(EWL_OBJECT(button[0]), EWL_ALIGNMENT_LEFT);
	ewl_widget_show(button[0]);

	button[1] = ewl_button_new(NULL);
	ewl_container_append_child(EWL_CONTAINER(main_vbox), button[1]);
	ewl_object_set_alignment(EWL_OBJECT(button[1]), EWL_ALIGNMENT_LEFT);
	ewl_widget_show(button[1]);

	check_button[0] = ewl_checkbutton_new("With Label");
	ewl_container_append_child(EWL_CONTAINER(main_vbox), check_button[0]);
	ewl_object_set_alignment(EWL_OBJECT(check_button[0]), EWL_ALIGNMENT_LEFT);
	ewl_widget_show(check_button[0]);

	check_button[1] = ewl_checkbutton_new(NULL);
	ewl_container_append_child(EWL_CONTAINER(main_vbox), check_button[1]);
	ewl_object_set_alignment(EWL_OBJECT(check_button[1]), EWL_ALIGNMENT_LEFT);
	ewl_widget_show(check_button[1]);

	radio_button[0] = ewl_radiobutton_new("With Label");
	ewl_container_append_child(EWL_CONTAINER(main_vbox), radio_button[0]);
	ewl_object_set_alignment(EWL_OBJECT(radio_button[0]), EWL_ALIGNMENT_LEFT);
	ewl_widget_show(radio_button[0]);

	radio_button[1] = ewl_radiobutton_new(NULL);
	ewl_container_append_child(EWL_CONTAINER(main_vbox), radio_button[1]);
	ewl_object_set_alignment(EWL_OBJECT(radio_button[1]), EWL_ALIGNMENT_LEFT);
	ewl_widget_show(radio_button[1]);

	return;
	w = NULL;
	ev_data = NULL;
	user_data = NULL;
}
