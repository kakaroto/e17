#include <Ewl.h>

static Ewl_Widget *entry_button;
static Ewl_Widget *entry[2];

void            __create_entry_test_window(Ewl_Widget * w, void *ev_data,
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
	char           *s;

	s = ewl_entry_get_text(EWL_ENTRY(entry[0]));
	printf("First entry covers: %s\n", s);
	FREE(s);

	s = ewl_entry_get_text(EWL_ENTRY(entry[1]));
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
	ewl_entry_set_text(EWL_ENTRY(entry[0]), "Play with me ?");
	ewl_entry_set_text(EWL_ENTRY(entry[1]), "E W L ! ! !");

	return;
	w = NULL;
	ev_data = NULL;
	user_data = NULL;
}

void
__create_entry_test_window(Ewl_Widget * w, void *ev_data, void *user_data)
{
	Ewl_Widget     *entry_win;
	Ewl_Widget     *entry_box;
	Ewl_Widget     *button_hbox;
	Ewl_Widget     *button[2];

	ewl_callback_del(w, EWL_CALLBACK_CLICKED, __create_entry_test_window);

	entry_button = w;

	entry_win = ewl_window_new();
	ewl_window_set_auto_size(EWL_WINDOW(entry_win), TRUE);
	ewl_callback_append(entry_win, EWL_CALLBACK_DELETE_WINDOW,
			    __destroy_entry_test_window, NULL);
	ewl_widget_show(entry_win);

	/*
	 * Create the main box for holding the widgets
	 */
	entry_box = ewl_vbox_new();
	ewl_container_append_child(EWL_CONTAINER(entry_win), entry_box);
	ewl_box_set_spacing(EWL_BOX(entry_box), 10);
	ewl_widget_show(entry_box);

	entry[0] = ewl_entry_new("Play with me ?");
	ewl_object_set_padding(EWL_OBJECT(entry[0]), 5, 5, 5, 0);
	ewl_container_append_child(EWL_CONTAINER(entry_box), entry[0]);
	ewl_widget_show(entry[0]);

	entry[1] = ewl_entry_new("E W L ! ! !");
	ewl_object_set_padding(EWL_OBJECT(entry[1]), 5, 5, 0, 0);
	ewl_container_append_child(EWL_CONTAINER(entry_box), entry[1]);
	ewl_widget_show(entry[1]);

	button_hbox = ewl_hbox_new();
	ewl_object_set_alignment(EWL_OBJECT(button_hbox), EWL_ALIGNMENT_CENTER);
	ewl_container_append_child(EWL_CONTAINER(entry_box), button_hbox);
	ewl_box_set_spacing(EWL_BOX(button_hbox), 5);
	ewl_widget_show(button_hbox);

	button[0] = ewl_button_new("Fetch text");
	ewl_container_append_child(EWL_CONTAINER(button_hbox), button[0]);
	ewl_callback_append(button[0], EWL_CALLBACK_CLICKED,
			    __fetch_entry_text, NULL);
	ewl_widget_show(button[0]);

	button[1] = ewl_button_new("Set Text");
	ewl_container_append_child(EWL_CONTAINER(button_hbox), button[1]);
	ewl_callback_append(button[1], EWL_CALLBACK_CLICKED,
			    __set_entry_text, NULL);
	ewl_widget_show(button[1]);

	return;
	ev_data = NULL;
	user_data = NULL;
}
