#include "ewl_test.h"

static Ewl_Widget *tooltip_button;

static void
__destroy_tooltip_test_window(Ewl_Widget * w, void *ev_data __UNUSED__,
						void *user_data __UNUSED__)
{
	ewl_widget_destroy(w);
	ewl_callback_append(tooltip_button, EWL_CALLBACK_CLICKED,
			__create_tooltip_test_window, NULL);
}

void
__create_tooltip_test_window(Ewl_Widget * w, void *ev_data __UNUSED__,
					void *user_data __UNUSED__)
{
	Ewl_Widget     *tooltip_win;
	Ewl_Widget     *tooltip_vbox;
	Ewl_Widget     *button;

	tooltip_button = w;

	tooltip_win = ewl_window_new();
	ewl_window_title_set(EWL_WINDOW(tooltip_win), "Tooltip Test");
	ewl_window_name_set(EWL_WINDOW(tooltip_win), "EWL Test Application");
	ewl_window_class_set(EWL_WINDOW(tooltip_win), "EFL Test Application");
	ewl_object_size_request(EWL_OBJECT(tooltip_win), 200, 100);

	if (w) {
		ewl_callback_del(w, EWL_CALLBACK_CLICKED, 
					__create_tooltip_test_window);
		ewl_callback_append(tooltip_win, EWL_CALLBACK_DELETE_WINDOW,
				__destroy_tooltip_test_window, NULL);
	} else
		ewl_callback_append(tooltip_win, EWL_CALLBACK_DELETE_WINDOW,
					__close_main_window, NULL);
	ewl_widget_show(tooltip_win);
	
	tooltip_vbox = ewl_vbox_new();
	ewl_container_child_append(EWL_CONTAINER(tooltip_win), tooltip_vbox);
	ewl_box_spacing_set(EWL_BOX(tooltip_vbox), 0);
	ewl_widget_show(tooltip_vbox);

	button = ewl_button_new();
	ewl_button_label_set(EWL_BUTTON(button), "Hoover on this button");
	ewl_container_child_append(EWL_CONTAINER (tooltip_vbox), button);
	ewl_object_fill_policy_set(EWL_OBJECT(button), EWL_FLAG_FILL_SHRINK);
	ewl_attach_tooltip_text_set(button, "and see.");
	ewl_widget_show (button);
}

