#include "ewl_test.h"

static Ewl_Widget *floater_button = NULL;

static void
__destroy_floater_test_window(Ewl_Widget *w, void *ev_data __UNUSED__, 
					void *user_data __UNUSED__)
{
	ewl_widget_destroy(w);
	ewl_callback_append(floater_button, EWL_CALLBACK_CLICKED,
			    __create_floater_test_window, NULL);
}

void
__create_floater_test_window(Ewl_Widget * w, void *ev_data __UNUSED__,
						void *user_data __UNUSED__)
{
	Ewl_Widget     *floater;
	Ewl_Widget     *floater_win;
	Ewl_Widget     *floater_box;
	Ewl_Widget     *separator;
	Ewl_Widget     *button[2];
	Ewl_Widget     *check_button[2];
	Ewl_Widget     *radio_button[2];

	floater_button = w;

	floater_win = ewl_window_new();
	ewl_window_title_set(EWL_WINDOW(floater_win), "Floater Test");
	ewl_window_name_set(EWL_WINDOW(floater_win), "EWL Test Application");
	ewl_window_class_set(EWL_WINDOW(floater_win), "EFL Test Application");

	if (w) {
		ewl_callback_del(w, EWL_CALLBACK_CLICKED, 
				__create_floater_test_window);
		ewl_callback_append(floater_win, EWL_CALLBACK_DELETE_WINDOW,
					__destroy_floater_test_window, NULL);
	} else
		ewl_callback_append(floater_win, EWL_CALLBACK_DELETE_WINDOW,
						__close_main_window, NULL);
	ewl_widget_show(floater_win);

	floater_box = ewl_vbox_new();
	ewl_container_child_append(EWL_CONTAINER(floater_win), floater_box);
	ewl_widget_show(floater_box);

	radio_button[0] = ewl_radiobutton_new();
	ewl_button_label_set(EWL_BUTTON(radio_button[0]), "With Label");
	ewl_container_child_append(EWL_CONTAINER(floater_box), radio_button[0]);
	ewl_object_alignment_set(EWL_OBJECT(radio_button[0]),
				 EWL_FLAG_ALIGN_LEFT);
	ewl_widget_show(radio_button[0]);

	radio_button[1] = ewl_radiobutton_new();
	ewl_button_label_set(EWL_BUTTON(radio_button[1]), NULL);
	ewl_radiobutton_chain_set(EWL_RADIOBUTTON(radio_button[1]), 
				  EWL_RADIOBUTTON(radio_button[0]));
	ewl_container_child_append(EWL_CONTAINER(floater_box), radio_button[1]);
	ewl_object_alignment_set(EWL_OBJECT(radio_button[1]),
				 EWL_FLAG_ALIGN_LEFT);
	ewl_widget_show(radio_button[1]);

	floater = ewl_floater_new(radio_button[1]);
	ewl_container_child_append(EWL_CONTAINER(floater_box), floater);
	ewl_floater_position_set(EWL_FLOATER(floater), 20, 20);
	ewl_widget_show(floater);

	button[0] = ewl_button_new();
	ewl_button_label_set(EWL_BUTTON(button[0]), "With Label");
	ewl_container_child_append(EWL_CONTAINER(floater), button[0]);
	ewl_object_alignment_set(EWL_OBJECT(button[0]), EWL_FLAG_ALIGN_LEFT);
	ewl_object_custom_size_set(EWL_OBJECT(button[0]), 100, 17);
	ewl_widget_show(button[0]);

	button[1] = ewl_button_new();
	ewl_button_label_set(EWL_BUTTON(button[1]), NULL);
	ewl_container_child_append(EWL_CONTAINER(floater), button[1]);
	ewl_object_alignment_set(EWL_OBJECT(button[1]), EWL_FLAG_ALIGN_LEFT);
	ewl_object_custom_size_set(EWL_OBJECT(button[1]), 100, 17);
	ewl_widget_show(button[1]);

	separator = ewl_hseparator_new();
	ewl_container_child_append(EWL_CONTAINER(floater), separator);
	ewl_widget_show(separator);

	ewl_object_padding_set(EWL_OBJECT(separator), 2, 2, 5, 5);

	check_button[0] = ewl_checkbutton_new();
	ewl_button_label_set(EWL_BUTTON(check_button[0]), "With Label");
	ewl_container_child_append(EWL_CONTAINER(floater), check_button[0]);
	ewl_object_alignment_set(EWL_OBJECT(check_button[0]),
				 EWL_FLAG_ALIGN_LEFT);
	ewl_widget_show(check_button[0]);

	check_button[1] = ewl_checkbutton_new();
	ewl_button_label_set(EWL_BUTTON(check_button[1]), NULL);
	ewl_container_child_append(EWL_CONTAINER(floater), check_button[1]);
	ewl_object_alignment_set(EWL_OBJECT(check_button[1]),
				 EWL_FLAG_ALIGN_LEFT);
	ewl_widget_show(check_button[1]);
}

