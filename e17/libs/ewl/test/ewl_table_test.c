#include <Ewl.h>

static Ewl_Widget *table_button = NULL;

void __create_table_test_window(Ewl_Widget * w, void *ev_data,
				void *user_data);

void
__destroy_table_test_window(Ewl_Widget * w, void *ev_data, void *user_data)
{
	ewl_widget_destroy_recursive(w);

	ewl_callback_append(table_button, EWL_CALLBACK_CLICKED,
			    __create_table_test_window, NULL);

	return;
	ev_data = NULL;
	user_data = NULL;
}

void
__create_table_test_window(Ewl_Widget * w, void *ev_data, void *user_data)
{
	Ewl_Widget *table_win;
	Ewl_Widget *table;
	Ewl_Widget *button[5];
	int r, c, i = 0;

	ewl_callback_del(w, EWL_CALLBACK_CLICKED, __create_table_test_window);

	table_button = w;

	table_win = ewl_window_new();
	ewl_window_resize(table_win, 145, 230);
	ewl_window_set_min_size(table_win, 145, 230);
	ewl_callback_append(table_win, EWL_CALLBACK_DELETE_WINDOW,
			    __destroy_table_test_window, NULL);
	ewl_widget_show(table_win);

	table = ewl_table_new(2, 3);
	ewl_container_append_child(EWL_CONTAINER(table_win), table);
	ewl_widget_show(table);

	for (r = 1; r < 3; r++)
	  {
		  for (c = 1; c < 3; c++)
		    {
			    printf("Attaching child @ row %i, col %i\n", r,
				   c);
			    button[i] = ewl_button_new("E W L");
			    ewl_table_attach(EWL_TABLE(table), button[i], c,
					     c, r, r);
			    ewl_object_set_fill_policy(EWL_OBJECT(button[i]),
						       EWL_FILL_POLICY_NORMAL);
			    ewl_object_set_custom_size(EWL_OBJECT(button[i]),
						       100, 17);
			    ewl_widget_show(button[i]);
		    }
		  ++i;
	  }

	button[i] = ewl_button_new("E W L");
	ewl_table_attach(EWL_TABLE(table), button[i], 1, 2, 3, 3);
	ewl_object_set_fill_policy(EWL_OBJECT(button[i]),
				   EWL_FILL_POLICY_FILL);
	ewl_object_set_custom_height(EWL_OBJECT(button[i]), 17);
	ewl_widget_show(button[i]);

	return;
	w = NULL;
	ev_data = NULL;
	user_data = NULL;
}
