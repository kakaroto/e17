#include <Ewl.h>

static Ewl_Widget *entry_button;
static Ewl_Widget *entry[2];

void __create_entry_test_window(Ewl_Widget * w, void *ev_data,
				void *user_data);

void
__destroy_entry_test_window(Ewl_Widget * w, void *ev_data, void *user_data)
{
	ewl_widget_destroy(w);

	ewl_callback_append(entry_button, EWL_CALLBACK_CLICKED,
			    __create_entry_test_window, NULL);

	return;
	ev_data = NULL;
	user_data = NULL;
}

void
__fetch_entry_text(Ewl_Widget * w, void *ev_data, void *user_data)
{
	char *s;

	s = ewl_entry_get_text(entry[0]);
	printf("First entry covers: %s\n", s);
	FREE(s);

	s = ewl_entry_get_text(entry[1]);
	printf("Second entry covers: %s\n", s);
	FREE(s);

	return;
	w = NULL;
	ev_data = NULL;
	user_data = NULL;

}

void
__set_entry_text(Ewl_Widget * w, void *ev_data, void *user_data)
{
	ewl_entry_set_text(entry[0], "Play with me ?");
	ewl_entry_set_text(entry[1], "E W L ! ! !");

	return;
	w = NULL;
	ev_data = NULL;
	user_data = NULL;
}

void
__create_entry_test_window(Ewl_Widget * w, void *ev_data, void *user_data)
{
	Ewl_Widget *entry_win;
	Ewl_Widget *main_vbox;
	Ewl_Widget *button_hbox;
	Ewl_Widget *button[2];

	ewl_callback_del(w, EWL_CALLBACK_CLICKED, __create_entry_test_window);

	entry_button = w;

	entry_win = ewl_window_new();
	ewl_window_resize(entry_win, 371, 96);
	ewl_window_set_min_size(entry_win, 371, 96);
	ewl_callback_append(entry_win, EWL_CALLBACK_DELETE_WINDOW,
			    __destroy_entry_test_window, NULL);
	ewl_widget_show(entry_win);

	main_vbox = ewl_vbox_new();
	ewl_container_append_child(EWL_CONTAINER(entry_win), main_vbox);
	ewl_box_set_spacing(main_vbox, 10);
	ewl_widget_show(main_vbox);

	entry[0] = ewl_entry_new();
	ewl_entry_set_text(entry[0], "Play with me ?");
	ewl_container_append_child(EWL_CONTAINER(main_vbox), entry[0]);
	ewl_widget_show(entry[0]);

	entry[1] = ewl_entry_new();
	ewl_entry_set_text(entry[1], "E W L ! ! !");
	ewl_container_append_child(EWL_CONTAINER(main_vbox), entry[1]);
	ewl_widget_show(entry[1]);

	button_hbox = ewl_hbox_new();
	ewl_object_set_custom_size(EWL_OBJECT(button_hbox), 206, 23);
	ewl_object_set_alignment(EWL_OBJECT(button_hbox),
				 EWL_ALIGNMENT_CENTER);
	ewl_container_append_child(EWL_CONTAINER(main_vbox), button_hbox);
	ewl_widget_show(button_hbox);

	button[0] = ewl_button_new("Fetch text");
	ewl_object_set_custom_size(EWL_OBJECT(button[0]), 100, 17);
	ewl_container_append_child(EWL_CONTAINER(button_hbox), button[0]);
	ewl_callback_append(button[0], EWL_CALLBACK_CLICKED,
			    __fetch_entry_text, NULL);
	ewl_widget_show(button[0]);

	button[1] = ewl_button_new("Set Text");
	ewl_object_set_custom_size(EWL_OBJECT(button[1]), 100, 17);
	ewl_container_append_child(EWL_CONTAINER(button_hbox), button[1]);
	ewl_callback_append(button[1], EWL_CALLBACK_CLICKED,
			    __set_entry_text, NULL);
	ewl_widget_show(button[1]);

	return;
	ev_data = NULL;
	user_data = NULL;
}
