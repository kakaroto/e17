#include <Ewl.h>

static Ewl_Widget *table_button = NULL;

void __create_table_test_window(Ewl_Widget * w, void *ev_data,
				void *user_data);

void
__destroy_table_test_window(Ewl_Widget * w, void *ev_data, void *user_data)
{
	ewl_widget_destroy_recursive(w);

	ewl_callback_append(table_button, EWL_CALLBACK_CLICKED,
			    __create_table_test_window, NULL);

	return;
	ev_data = NULL;
	user_data = NULL;
}

void
__create_table_test_window(Ewl_Widget * w, void *ev_data, void *user_data)
{
	Ewl_Widget *table_win;
	Ewl_Widget *table;
	Ewl_Widget *separator[2];
	Ewl_Widget *button[2];
	Ewl_Widget *check_button[2];
	Ewl_Widget *radio_button[2];

	ewl_callback_del(w, EWL_CALLBACK_CLICKED, __create_table_test_window);

	table_button = w;

	table_win = ewl_window_new();
	ewl_window_resize(table_win, 145, 230);
	ewl_window_set_min_size(table_win, 145, 230);
	ewl_callback_append(table_win, EWL_CALLBACK_DELETE_WINDOW,
			    __destroy_table_test_window, NULL);
	ewl_widget_show(table_win);

	table = ewl_table_new(8, 1);
	ewl_container_append_child(EWL_CONTAINER(table_win), table);
	ewl_widget_show(table);

	button[0] = ewl_button_new("With Label");
	ewl_table_attach(EWL_TABLE(table), button[0], 1, 1, 1, 1);
	ewl_object_set_alignment(EWL_OBJECT(button[0]), EWL_ALIGNMENT_LEFT);
	ewl_object_set_custom_size(EWL_OBJECT(button[0]), 100, 17);
	ewl_widget_show(button[0]);

	button[1] = ewl_button_new(NULL);
	ewl_table_attach(EWL_TABLE(table), button[1], 1, 2, 1, 2);
	ewl_object_set_alignment(EWL_OBJECT(button[1]), EWL_ALIGNMENT_RIGHT);
	ewl_object_set_custom_size(EWL_OBJECT(button[1]), 100, 17);
	ewl_widget_show(button[1]);

	separator[0] = ewl_vseparator_new();
	ewl_table_attach(EWL_TABLE(table), separator[0], 1, 3, 1, 3);
	ewl_object_set_padding(EWL_OBJECT(separator[0]), 2, 2, 5, 5);
	ewl_widget_show(separator[0]);

	check_button[0] = ewl_checkbutton_new("With Label");
	ewl_table_attach(EWL_TABLE(table), check_button[0], 1, 4, 1, 4);
	ewl_object_set_alignment(EWL_OBJECT(check_button[0]),
				 EWL_ALIGNMENT_LEFT);
	ewl_widget_show(check_button[0]);

	check_button[1] = ewl_checkbutton_new(NULL);
	ewl_table_attach(EWL_TABLE(table), check_button[1], 1, 5, 1, 5);
	ewl_object_set_alignment(EWL_OBJECT(check_button[1]),
				 EWL_ALIGNMENT_LEFT);
	ewl_widget_show(check_button[1]);

	separator[1] = ewl_vseparator_new();
	ewl_table_attach(EWL_TABLE(table), separator[1], 1, 6, 1, 6);
	ewl_object_set_padding(EWL_OBJECT(separator[1]), 2, 2, 5, 5);
	ewl_widget_show(separator[1]);


	radio_button[0] = ewl_radiobutton_new("With Label");
	ewl_table_attach(EWL_TABLE(table), radio_button[0], 1, 7, 1, 7);
	ewl_object_set_alignment(EWL_OBJECT(radio_button[0]),
				 EWL_ALIGNMENT_LEFT);
	ewl_widget_show(radio_button[0]);

	radio_button[1] = ewl_radiobutton_new(NULL);
	ewl_radiobutton_set_chain(radio_button[1], radio_button[0]);
	ewl_table_attach(EWL_TABLE(table), radio_button[1], 1, 8, 1, 8);
	ewl_object_set_alignment(EWL_OBJECT(radio_button[1]),
				 EWL_ALIGNMENT_LEFT);
	ewl_widget_show(radio_button[1]);

	return;
	w = NULL;
	ev_data = NULL;
	user_data = NULL;
}
