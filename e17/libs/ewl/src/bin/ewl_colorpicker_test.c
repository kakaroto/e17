#include "ewl_test.h"

static Ewl_Widget *colorpicker_button = NULL;

void
__destroy_colorpicker_test_window(Ewl_Widget * w, void *ev_data, void *user_data)
{
	ewl_widget_destroy(w);
	ewl_callback_append(colorpicker_button, EWL_CALLBACK_CLICKED,
			    __create_colorpicker_test_window, NULL);

	return;
	w = NULL;
	ev_data = NULL;
	user_data = NULL;
}

void
__color_value_changed(Ewl_Widget * w, void *ev_data, void *user_data)
{
	char           *text = ev_data;

	printf("value changed to %s\n", text);

	return;
	w = NULL;
	ev_data = NULL;
	user_data = NULL;
}

void
__create_colorpicker_test_window(Ewl_Widget * w, void *ev_data, void *user_data)
{
	Ewl_Widget     *colorpicker_win;
	Ewl_Widget     *colorpicker_box;
	Ewl_Widget     *colorpicker1;


	colorpicker_button = w;

	colorpicker_win = ewl_window_new();
	ewl_window_title_set(EWL_WINDOW(colorpicker_win), "Color Picker Test");
	ewl_window_name_set(EWL_WINDOW(colorpicker_win), "EWL Test Application");
	ewl_window_class_set(EWL_WINDOW(colorpicker_win), "EFL Test Application");

	if (w) {
		ewl_callback_del(w, EWL_CALLBACK_CLICKED, 
				__create_colorpicker_test_window);
		ewl_callback_append(colorpicker_win, EWL_CALLBACK_DELETE_WINDOW,
				    __destroy_colorpicker_test_window, NULL);
	} else
		ewl_callback_append(colorpicker_win, EWL_CALLBACK_DELETE_WINDOW,
				__close_main_window, NULL);
	ewl_widget_show(colorpicker_win);

	/*
	 * Create the main box for holding the widgets
	 */
	colorpicker_box = ewl_vbox_new();
	ewl_object_fill_policy_set(EWL_OBJECT(colorpicker_box),
				   EWL_FLAG_FILL_FILL);
	ewl_container_child_append(EWL_CONTAINER(colorpicker_win),
				   colorpicker_box);
	ewl_widget_show(colorpicker_box);

	/*
	 * Create the menu
	 */
	colorpicker1 = ewl_colorpicker_new();
	ewl_container_child_append(EWL_CONTAINER(colorpicker_box),
				   colorpicker1);
	ewl_callback_append(colorpicker1, EWL_CALLBACK_VALUE_CHANGED,
			    __color_value_changed, NULL);
	ewl_widget_show(colorpicker1);

	return;
	w = NULL;
	ev_data = NULL;
	user_data = NULL;
}
