#include <Ewl.h>

static Ewl_Widget * entry_button;

void __create_entry_test_window(Ewl_Widget * w, void * ev_data, void * user_data);


void
__destroy_entry_test_window(Ewl_Widget * w, void * ev_data, void * user_data)
{
	ewl_widget_destroy(w);

	ewl_callback_append(entry_button, EWL_CALLBACK_CLICKED,
				__create_entry_test_window, NULL);

	return;
	ev_data = NULL;
	user_data = NULL;
}

void
__create_entry_test_window(Ewl_Widget * w, void * ev_data, void * user_data)
{
	Ewl_Widget * entry_win;
	Ewl_Widget * main_vbox;
	Ewl_Widget * entry[2];
	Ewl_Widget * button_hbox[2];
	Ewl_Widget * button[2][2];

	ewl_callback_del(w, EWL_CALLBACK_CLICKED, __create_entry_test_window);

	entry_button = w;

	entry_win = ewl_window_new();
	ewl_callback_append(entry_win, EWL_CALLBACK_DELETE_WINDOW,
			__destroy_entry_test_window, NULL);
	ewl_widget_show(entry_win);

	main_vbox = ewl_vbox_new();
	ewl_container_append_child(EWL_CONTAINER(entry_win), main_vbox);
	ewl_box_set_spacing(main_vbox, 10);
	ewl_widget_show(main_vbox);

	entry[0] = ewl_entry_new();
	ewl_entry_set_text(entry[0], "Test!! =)");
	ewl_container_append_child(EWL_CONTAINER(main_vbox), entry[0]);
	ewl_widget_show(entry[0]);

	return;
	ev_data = NULL;
	user_data = NULL;
}
