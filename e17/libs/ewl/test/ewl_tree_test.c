#include <Ewl.h>

#define ROWS 100
#define COLS 4
#define NEST 3

static Ewl_Widget *tree_button;

void __create_tree_test_window(Ewl_Widget * w, void *ev_data,
				     void *user_data);


void
__destroy_tree_test_window(Ewl_Widget * w, void *ev_data,
				 void *user_data)
{
	ewl_widget_destroy(w);

	ewl_callback_append(tree_button, EWL_CALLBACK_CLICKED,
			    __create_tree_test_window, NULL);

	return;
	ev_data = NULL;
	user_data = NULL;
}

void
__create_tree_test_window(Ewl_Widget * w, void *ev_data,
				void *user_data)
{
	int row, col;
	Ewl_Widget *tree_win;
	Ewl_Widget *tree;
	Ewl_Widget *entries[COLS];
	Ewl_Widget *prow = NULL;

	ewl_callback_del(w, EWL_CALLBACK_CLICKED,
			 __create_tree_test_window);

	tree_button = w;

	tree_win = ewl_window_new();
	ewl_object_set_maximum_size(EWL_OBJECT(tree_win), 200, 200);
	ewl_callback_append(tree_win, EWL_CALLBACK_DELETE_WINDOW,
			    __destroy_tree_test_window, NULL);
	ewl_widget_show(tree_win);

	tree = ewl_tree_new(4);
	ewl_container_append_child(EWL_CONTAINER(tree_win), tree);
	ewl_widget_show(tree);

	for (row = 0; row < ROWS; row++) {
		char buf[1024];

		/*
		 * Build the array of entries.
		 */
		for (col = 0; col < COLS; col++) {
			snprintf(buf, 1024, "Row %d Col %d", row, col);
			entries[col] = ewl_entry_new(buf);
			ewl_widget_show(entries[col]);
		}

		if (row % NEST == 0)
			prow = ewl_tree_add_row(EWL_TREE(tree), NULL, entries);
		else
			prow = ewl_tree_add_row(EWL_TREE(tree), EWL_ROW(prow),
					entries);
		/* ewl_tree_set_row_expanded(EWL_ROW(prow), TRUE); */
	}

	printf("Tree setup complete!!!\n");

	return;
	ev_data = NULL;
	user_data = NULL;
}
