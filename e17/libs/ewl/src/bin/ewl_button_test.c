#include "ewl_test.h"

static Ewl_Widget *button_button = NULL;

static void
__delete_button_test_window(Ewl_Widget *w, void *ev_data __UNUSED__, 
					void *user_data __UNUSED__)
{
	ewl_widget_destroy(w);

	ewl_callback_append(button_button, EWL_CALLBACK_CLICKED,
			    __create_button_test_window, NULL);
}

void
__create_button_test_window(Ewl_Widget *w, void *ev_data __UNUSED__, 
						void *user_data __UNUSED__)
{
	Ewl_Widget *button_win;
	Ewl_Widget *button_box;
	Ewl_Widget *separator[2];
	Ewl_Widget *button[2];
	Ewl_Widget *check_button[3];
	Ewl_Widget *radio_button[2];
	Ewl_Widget *label;

	button_button = w;

	button_win = ewl_window_new();
	ewl_window_title_set(EWL_WINDOW(button_win), "Button Type Test");
	ewl_window_name_set(EWL_WINDOW(button_win), "EWL Test Application");
	ewl_window_class_set(EWL_WINDOW(button_win), "EFL Test Application");

	if (w) {
		ewl_callback_del(w, EWL_CALLBACK_CLICKED,
					__create_button_test_window);
		ewl_callback_append(button_win, EWL_CALLBACK_DELETE_WINDOW,
				    __delete_button_test_window, NULL);
	} else 
		ewl_callback_append(button_win, EWL_CALLBACK_DELETE_WINDOW,
					__close_main_window, NULL);
	ewl_widget_show(button_win);

	/*
	 * Create the main box for holding the button widgets
	 */
	button_box = ewl_vbox_new();
	ewl_container_child_append(EWL_CONTAINER(button_win), button_box);
	ewl_box_spacing_set(EWL_BOX(button_box), 10);
	ewl_widget_show(button_box);

	/*
	 * Create a button to be displayed witha label.
	 */
	button[0] = ewl_button_new();
	ewl_button_label_set(EWL_BUTTON(button[0]), "With Label");
	ewl_container_child_append(EWL_CONTAINER(button_box), button[0]);
	ewl_object_alignment_set(EWL_OBJECT(button[0]), EWL_FLAG_ALIGN_LEFT);
	ewl_widget_show(button[0]);

	/*
	 * Create a button that does not contain a label
	 */
	button[1] = ewl_button_new();
	ewl_container_child_append(EWL_CONTAINER(button_box), button[1]);
	ewl_object_alignment_set(EWL_OBJECT(button[1]), EWL_FLAG_ALIGN_LEFT);
	ewl_widget_show(button[1]);

	/*
	 * Add a separator between the classic buttons and the check buttons.
	 */
	separator[0] = ewl_hseparator_new();
	ewl_container_child_append(EWL_CONTAINER(button_box), separator[0]);
	ewl_widget_show(separator[0]);

	/*
	 * Create a check button with a label.
	 */
	check_button[0]  = ewl_checkbutton_new();
	ewl_button_label_set(EWL_BUTTON(check_button[0] ), "With Label");
	ewl_container_child_append(EWL_CONTAINER(button_box), check_button[0]);
	ewl_object_alignment_set(EWL_OBJECT(check_button[0]),
				 EWL_FLAG_ALIGN_LEFT);
	ewl_widget_show(check_button[0]);

	/*
	 * Create a check button with a label and checked.
	 */
	check_button[1]  = ewl_checkbutton_new();
	ewl_button_label_set(EWL_BUTTON(check_button[1] ), "With Label and checked");
	ewl_checkbutton_checked_set(EWL_CHECKBUTTON(check_button[1]), TRUE);
	ewl_container_child_append(EWL_CONTAINER(button_box), check_button[1]);
	ewl_object_alignment_set(EWL_OBJECT(check_button[1]),
				 EWL_FLAG_ALIGN_LEFT);
	ewl_widget_show(check_button[1]);

	/*
	 * Create a check button w/o a label.
	 */
	check_button[2]  = ewl_checkbutton_new();
	ewl_container_child_append(EWL_CONTAINER(button_box), check_button[2]);
	ewl_object_alignment_set(EWL_OBJECT(check_button[2]),
				 EWL_FLAG_ALIGN_LEFT);
	ewl_widget_show(check_button[2]);

	/*
	 * Add a separator between the check buttons and the radio buttons
	 */
	separator[1] = ewl_hseparator_new();
	ewl_container_child_append(EWL_CONTAINER(button_box), separator[1]);
	ewl_widget_show(separator[1]);

	/*
	 * Add a radio button with
	 */
	radio_button[0]  = ewl_radiobutton_new();
	ewl_button_label_set(EWL_BUTTON(radio_button[0] ), "With Label");
	ewl_radiobutton_checked_set(radio_button[0], TRUE);
	ewl_container_child_append(EWL_CONTAINER(button_box), radio_button[0]);
	ewl_object_alignment_set(EWL_OBJECT(radio_button[0]),
				 EWL_FLAG_ALIGN_LEFT);
	ewl_widget_show(radio_button[0]);

	radio_button[1]  = ewl_radiobutton_new();
	ewl_radiobutton_chain_set(EWL_RADIOBUTTON(radio_button[1]), 
	                          EWL_RADIOBUTTON(radio_button[0]));
	ewl_container_child_append(EWL_CONTAINER(button_box), radio_button[1]);
	ewl_object_alignment_set(EWL_OBJECT(radio_button[1]),
				 EWL_FLAG_ALIGN_LEFT);
	ewl_widget_show(radio_button[1]);

	label = ewl_label_new();
	ewl_label_text_set(EWL_LABEL(label), "A label");
	ewl_container_child_append(EWL_CONTAINER(button_box), label);
	ewl_widget_show(label);
}

