#include "ewl_test.h"

static Ewl_Widget *entry_button;
static Ewl_Widget *entry[3];

static void
__destroy_entry_test_window(Ewl_Widget * w, void *ev_data __UNUSED__,
					void *user_data __UNUSED__)
{
	ewl_widget_destroy(w);
	ewl_callback_append(entry_button, EWL_CALLBACK_CLICKED,
				__create_entry_test_window, NULL);
}

static void
__fetch_entry_text(Ewl_Widget *w __UNUSED__, void *ev_data __UNUSED__, 
				void *user_data __UNUSED__)
{
	char *s;

	s = ewl_text_text_get(EWL_TEXT(entry[0]));
	if (s) {
		printf("First entry covers: %s\n", s);
		FREE(s);
	}

	s = ewl_text_text_get(EWL_TEXT(entry[1]));
	if (s) {
		printf("Second entry covers: %s\n", s);
		FREE(s);
	}

	s = ewl_text_text_get(EWL_TEXT(entry[2]));
	if (s) {
		printf("Third entry covers: %s\n", s);
		FREE(s);
	}
}

static void
__set_entry_text(Ewl_Widget *w __UNUSED__, void *ev_data __UNUSED__, 
					void *user_data __UNUSED__)
{
	ewl_text_color_set(EWL_TEXT(entry[0]), 0, 0, 0, 255);
	ewl_text_color_set(EWL_TEXT(entry[1]), 0, 0, 0, 255);
	ewl_text_color_set(EWL_TEXT(entry[2]), 0, 0, 0, 255);

	ewl_text_text_set(EWL_TEXT(entry[0]), "Play with me ?");
	ewl_text_text_set(EWL_TEXT(entry[1]), "E W L ! ! !");

	ewl_text_clear(EWL_TEXT(entry[2]));
}

void
__create_entry_test_window(Ewl_Widget *w, void *ev_data __UNUSED__, 
						void *user_data __UNUSED__)
{
	Ewl_Widget *entry_win;
	Ewl_Widget *entry_box;
	Ewl_Widget *button_hbox, *o;
	Ewl_Widget *button[2];

	entry_button = w;

	entry_win = ewl_window_new();
	ewl_window_title_set(EWL_WINDOW(entry_win), "Entry Test");
	ewl_window_name_set(EWL_WINDOW(entry_win), "EWL Test Application");
	ewl_window_class_set(EWL_WINDOW(entry_win), "EFL Test Application");

	if (w) {
		ewl_callback_del(w, EWL_CALLBACK_CLICKED, 
					__create_entry_test_window);
		ewl_callback_append(entry_win, EWL_CALLBACK_DELETE_WINDOW,
					__destroy_entry_test_window, NULL);
	} else
		ewl_callback_append(entry_win, EWL_CALLBACK_DELETE_WINDOW,
					__close_main_window, NULL);
	ewl_widget_show(entry_win);

	/*
	 * Create the main box for holding the widgets
	 */
	entry_box = ewl_vbox_new();
	ewl_container_child_append(EWL_CONTAINER(entry_win), entry_box);
	ewl_box_spacing_set(EWL_BOX(entry_box), 10);
	ewl_widget_show(entry_box);

	entry[0] = ewl_entry_new();
	ewl_text_text_set(EWL_TEXT(entry[0]), "Play with me ?");
	ewl_entry_multiline_set(EWL_ENTRY(entry[0]), 1);
	ewl_text_color_set(EWL_TEXT(entry[0]), 255, 0, 0, 255);
	ewl_object_padding_set(EWL_OBJECT(entry[0]), 5, 5, 5, 0);
	ewl_container_child_append(EWL_CONTAINER(entry_box), entry[0]);
	ewl_widget_show(entry[0]);

	entry[1] = ewl_entry_new();
	ewl_text_text_set(EWL_TEXT(entry[1]), "E W L ! ! !");
	ewl_text_color_set(EWL_TEXT(entry[1]), 255, 0, 0, 255);
	ewl_object_padding_set(EWL_OBJECT(entry[1]), 5, 5, 0, 0);
	ewl_container_child_append(EWL_CONTAINER(entry_box), entry[1]);
	ewl_widget_show(entry[1]);

	entry[2] = ewl_entry_new();
	ewl_object_padding_set(EWL_OBJECT(entry[2]), 5, 5, 0, 0);
	ewl_container_child_append(EWL_CONTAINER(entry_box), entry[2]);
	ewl_widget_show(entry[2]);

	o = ewl_entry_new();
	ewl_text_text_set(EWL_TEXT(o), "disabled");
	ewl_object_padding_set(EWL_OBJECT(o), 5, 5, 0, 0);
	ewl_container_child_append(EWL_CONTAINER(entry_box), o);
	ewl_widget_disable(o);
	ewl_widget_show(o);

	button_hbox = ewl_hbox_new();
	ewl_object_alignment_set(EWL_OBJECT(button_hbox), EWL_FLAG_ALIGN_CENTER);
	ewl_container_child_append(EWL_CONTAINER(entry_box), button_hbox);
	ewl_box_spacing_set(EWL_BOX(button_hbox), 5);
	ewl_widget_show(button_hbox);

	button[0] = ewl_button_new();
	ewl_button_label_set(EWL_BUTTON(button[0]), "Fetch text");
	ewl_container_child_append(EWL_CONTAINER(button_hbox), button[0]);
	ewl_callback_append(button[0], EWL_CALLBACK_CLICKED,
				__fetch_entry_text, NULL);
	ewl_widget_show(button[0]);

	button[1] = ewl_button_new();
	ewl_button_label_set(EWL_BUTTON(button[1]), "Set Text");
	ewl_container_child_append(EWL_CONTAINER(button_hbox), button[1]);
	ewl_callback_append(button[1], EWL_CALLBACK_CLICKED,
				__set_entry_text, NULL);
	ewl_widget_show(button[1]);

	o = ewl_label_new();
	ewl_label_text_set(EWL_LABEL(o), "Text insert \"\"");
	ewl_container_child_append(EWL_CONTAINER(button_hbox), o);
	ewl_widget_show(o);

	o = ewl_text_new();
	ewl_text_text_set(EWL_TEXT(o), "");
	ewl_container_child_append(EWL_CONTAINER(button_hbox), o);
	ewl_widget_show(o);
}


