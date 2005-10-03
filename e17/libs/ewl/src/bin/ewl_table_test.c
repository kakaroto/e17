#include "ewl_test.h"

static Ewl_Widget *table_button = NULL;

static void
__destroy_table_test_window(Ewl_Widget * w, void *ev_data __UNUSED__,
					void *user_data __UNUSED__)
{
	ewl_widget_destroy(w);
	ewl_callback_append(table_button, EWL_CALLBACK_CLICKED,
			    __create_table_test_window, NULL);
}

void
__create_table_test_window(Ewl_Widget * w, void *ev_data __UNUSED__,
					void *user_data __UNUSED__)
{
	Ewl_Widget     *table_win;
	Ewl_Widget     *table_box;
	Ewl_Widget     *table;
	Ewl_Widget     *button[10];
	char           *headers[4];
	char           *one = "one";
	char           *two = "two";
	char           *three = "three";
	char           *four = "four";

	table_button = w;

	table_win = ewl_window_new();
	ewl_window_title_set(EWL_WINDOW(table_win), "Table Test");
	ewl_window_name_set(EWL_WINDOW(table_win), "EWL Test Application");
	ewl_window_class_set(EWL_WINDOW(table_win), "EFL Test Application");

	if (w) {
		ewl_callback_del(w, EWL_CALLBACK_CLICKED, 
					__create_table_test_window);
		ewl_callback_append(table_win, EWL_CALLBACK_DELETE_WINDOW,
				    __destroy_table_test_window, NULL);
	} else 
		ewl_callback_append(table_win, EWL_CALLBACK_DELETE_WINDOW,
					__close_main_window, NULL);
	ewl_widget_show(table_win);

	/*
	 * Create the main box for holding the widgets
	 */
	table_box = ewl_vbox_new();
	ewl_object_fill_policy_set(EWL_OBJECT(table_box), EWL_FLAG_FILL_FILL);
	ewl_container_child_append(EWL_CONTAINER(table_win), table_box);
	ewl_theme_data_str_set(table_box,
			       "/appearance/box/vertical/base/visible", "no");
	ewl_widget_show(table_box);


	headers[0] = one;
	headers[1] = two;
	headers[2] = three;
	headers[3] = four;

	table = ewl_table_new(4, 5, headers);
	ewl_container_child_append(EWL_CONTAINER(table_box), table);
	ewl_widget_show(table);

	

	button[0] = ewl_button_new();
	ewl_button_label_set(EWL_BUTTON(button[0]), "FIRST");
	ewl_table_add(EWL_TABLE(table), button[0], 3, 4, 2, 3);
	ewl_widget_show(button[0]);

/*
	button[1] = ewl_button_new();
	ewl_button_label_set(EWL_BUTTON(button[1]), NULL);
	ewl_table_attach(EWL_TABLE(table), button[1], 1, 1, 2, 2);
	ewl_object_set_custom_size(EWL_OBJECT(button[1]), 100, 20);
	ewl_object_fill_policy_set(EWL_OBJECT(button[1]),
				   EWL_FLAG_FILL_NORMAL);
	ewl_widget_realize(button[1]);

	button[2] = ewl_button_new();
	ewl_button_label_set(EWL_BUTTON(button[2]), NULL);
	ewl_table_attach(EWL_TABLE(table), button[2], 1, 1, 3, 3);
	ewl_object_set_custom_size(EWL_OBJECT(button[2]), 100, 20);
	ewl_object_fill_policy_set(EWL_OBJECT(button[2]),
				   EWL_FLAG_FILL_NORMAL);
	ewl_widget_show(button[2]);


	button[3] = ewl_button_new();
	ewl_button_label_set(EWL_BUTTON(button[3]), NULL);
	ewl_object_set_custom_height(EWL_OBJECT(button[3]), 20);
	ewl_table_attach(EWL_TABLE(table), button[3], 2, 2, 1, 1);
	ewl_widget_show(button[3]);

	button[4] = ewl_button_new();
	ewl_button_label_set(EWL_BUTTON(button[4]), NULL);
	ewl_object_set_custom_height(EWL_OBJECT(button[4]), 20);
	ewl_table_attach(EWL_TABLE(table), button[4], 2, 2, 2, 2);
	ewl_widget_show(button[4]);

	button[5] = ewl_button_new();
	ewl_button_label_set(EWL_BUTTON(button[5]), NULL);
	ewl_object_set_custom_height(EWL_OBJECT(button[5]), 20);
	ewl_table_attach(EWL_TABLE(table), button[5], 2, 2, 3, 3);
	ewl_widget_realize(button[5]);

	button[6] = ewl_button_new();
	ewl_button_label_set(EWL_BUTTON(button[6]), NULL);
	ewl_table_attach(EWL_TABLE(table), button[6], 2, 2, 4, 4);
	ewl_object_set_custom_size(EWL_OBJECT(button[6]), 100, 20);
	ewl_object_fill_policy_set(EWL_OBJECT(button[6]),
				   EWL_FLAG_FILL_NORMAL);
	ewl_widget_realize(button[6]);

	button[7] = ewl_button_new();
	ewl_button_label_set(EWL_BUTTON(button[7]), NULL);
	ewl_table_attach(EWL_TABLE(table), button[7], 3, 3, 4, 4);
	ewl_object_set_custom_size(EWL_OBJECT(button[7]), 100, 20);
	ewl_object_fill_policy_set(EWL_OBJECT(button[7]),
				   EWL_FLAG_FILL_NORMAL);
	ewl_widget_realize(button[7]);

	button[8] = ewl_button_new();
	ewl_button_label_set(EWL_BUTTON(button[8]), NULL);
	ewl_object_set_custom_height(EWL_OBJECT(button[8]), 20);
	ewl_table_attach(EWL_TABLE(table), button[8], 3, 3, 2, 2);
	ewl_widget_realize(button[8]);

	button[9] = ewl_button_new();
	ewl_button_label_set(EWL_BUTTON(button[9]), NULL);
	ewl_object_set_custom_height(EWL_OBJECT(button[9]), 20);
	ewl_table_attach(EWL_TABLE(table), button[9], 2, 3, 5, 5);
	ewl_widget_realize(button[9]);

	button[10] = ewl_button_new();
	ewl_button_label_set(EWL_BUTTON(button[10]), NULL);
	ewl_object_set_custom_size(EWL_OBJECT(button[10]), 100, 20);
	ewl_object_fill_policy_set(EWL_OBJECT(button[10]),
				   EWL_FLAG_FILL_NORMAL);
	ewl_table_attach(EWL_TABLE(table), button[10], 4, 4, 4, 4);
	ewl_widget_realize(button[10]);

	button[11] = ewl_button_new();
	ewl_button_label_set(EWL_BUTTON(button[11]), NULL);
	ewl_object_set_custom_size(EWL_OBJECT(button[11]), 100, 20);
	ewl_object_fill_policy_set(EWL_OBJECT(button[11]),
				   EWL_FLAG_FILL_NORMAL);

	ewl_table_attach(EWL_TABLE(table), button[11], 4, 4, 5, 5);
	ewl_widget_realize(button[11]);
*/

	ewl_widget_configure(table);
}

