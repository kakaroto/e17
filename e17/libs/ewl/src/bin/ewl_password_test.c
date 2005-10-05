#include "ewl_test.h"

static Ewl_Widget *password_button;
static Ewl_Widget *password[2];

static void
__destroy_password_test_window(Ewl_Widget * w, void *ev_data __UNUSED__,
						void *user_data __UNUSED__)
{
	ewl_widget_destroy(w);
	ewl_callback_append(password_button, EWL_CALLBACK_CLICKED,
			    __create_password_test_window, NULL);
}

static void
__fetch_password_text(Ewl_Widget * w __UNUSED__, void *ev_data __UNUSED__,
					void *user_data __UNUSED__)
{
	char *s;

	s = ewl_password_text_get(EWL_PASSWORD(password[0]));
	printf("First password covers: %s\n", s);
	FREE(s);

	s = ewl_password_text_get(EWL_PASSWORD(password[1]));
	printf("Second password covers: %s\n", s);
	FREE(s);
}

static void
__set_password_text(Ewl_Widget * w __UNUSED__, void *ev_data __UNUSED__,
					void *user_data __UNUSED__)
{
	ewl_password_text_set(EWL_PASSWORD(password[0]), "Play with me ?");
	ewl_password_text_set(EWL_PASSWORD(password[1]), "E W L ! ! !");
}

void
__create_password_test_window(Ewl_Widget * w, void *ev_data __UNUSED__,
						void *user_data __UNUSED__)
{
	Ewl_Widget     *password_win;
	Ewl_Widget     *password_box;
	Ewl_Widget     *button_hbox;
	Ewl_Widget     *button[2];

	password_button = w;

	password_win = ewl_window_new();
	ewl_window_title_set(EWL_WINDOW(password_win), "Password Entry Test");
	ewl_window_name_set(EWL_WINDOW(password_win), "EWL Test Application");
	ewl_window_class_set(EWL_WINDOW(password_win), "EFL Test Application");

	if (w) {
		ewl_callback_del(w, EWL_CALLBACK_CLICKED, 
					__create_password_test_window);
		ewl_callback_append(password_win, EWL_CALLBACK_DELETE_WINDOW,
				    __destroy_password_test_window, NULL);
	} else
		ewl_callback_append(password_win, EWL_CALLBACK_DELETE_WINDOW,
					__close_main_window, NULL);
	ewl_widget_show(password_win);

	/*
	 * Create the main box for holding the widgets
	 */
	password_box = ewl_vbox_new();
	ewl_container_child_append(EWL_CONTAINER(password_win), password_box);
	ewl_box_spacing_set(EWL_BOX(password_box), 10);
	ewl_widget_show(password_box);

	password[0] = ewl_password_new();
	ewl_password_text_set(EWL_PASSWORD(password[0]), "Play with me ?");
	ewl_object_padding_set(EWL_OBJECT(password[0]), 5, 5, 5, 0);
	ewl_container_child_append(EWL_CONTAINER(password_box), password[0]);
	ewl_callback_append(password[0], EWL_CALLBACK_VALUE_CHANGED,
			    __fetch_password_text, NULL);
	ewl_widget_show(password[0]);

	password[1] = ewl_password_new();
	ewl_password_text_set(EWL_PASSWORD(password[1]), "E W L ! ! !");
	ewl_object_padding_set(EWL_OBJECT(password[1]), 5, 5, 0, 0);
	ewl_container_child_append(EWL_CONTAINER(password_box), password[1]);
	ewl_callback_append(password[1], EWL_CALLBACK_VALUE_CHANGED,
			    __fetch_password_text, NULL);
	ewl_widget_show(password[1]);

	button_hbox = ewl_hbox_new();
	ewl_object_alignment_set(EWL_OBJECT(button_hbox), EWL_FLAG_ALIGN_CENTER);
	ewl_container_child_append(EWL_CONTAINER(password_box), button_hbox);
	ewl_box_spacing_set(EWL_BOX(button_hbox), 5);
	ewl_widget_show(button_hbox);

	button[0] = ewl_button_new();
	ewl_button_label_set(EWL_BUTTON(button[0]), "Fetch text");
	ewl_container_child_append(EWL_CONTAINER(button_hbox), button[0]);
	ewl_callback_append(button[0], EWL_CALLBACK_CLICKED,
			    __fetch_password_text, NULL);
	ewl_widget_show(button[0]);

	button[1] = ewl_button_new();
	ewl_button_label_set(EWL_BUTTON(button[1]), "Set Text");
	ewl_container_child_append(EWL_CONTAINER(button_hbox), button[1]);
	ewl_callback_append(button[1], EWL_CALLBACK_CLICKED,
			    __set_password_text, NULL);
	ewl_widget_show(button[1]);
}

