#include "ewl_test.h"

#define ROWS 10
#define COLS 4
#define NEST 3

static Ewl_Widget *tree_button;

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
	char buf[PATH_MAX];
	char *headers[COLS];
	char *entries[COLS];
	Ewl_Widget *tree_win;
	Ewl_Widget *tree;
	Ewl_Widget *prow = NULL;

	ewl_callback_del(w, EWL_CALLBACK_CLICKED,
			 __create_tree_test_window);

	tree_button = w;

	tree_win = ewl_window_new();
	ewl_window_set_title(EWL_WINDOW(tree_win), "Tree Test");
	ewl_window_set_name(EWL_WINDOW(tree_win), "EWL Test Application");
	ewl_window_set_class(EWL_WINDOW(tree_win), "EFL Test Application");
	ewl_object_set_minimum_size(EWL_OBJECT(tree_win), 400, 400);
	ewl_callback_append(tree_win, EWL_CALLBACK_DELETE_WINDOW,
			    __destroy_tree_test_window, NULL);
	ewl_widget_show(tree_win);

	for (col = 0; col < COLS; col++) {
		snprintf(buf, PATH_MAX, "Column %d", col);
		headers[col] = strdup(buf);
	}

	tree = ewl_tree_new(COLS);
	ewl_container_append_child(EWL_CONTAINER(tree_win), tree);
	ewl_tree_set_headers(EWL_TREE(tree), headers);
	ewl_widget_show(tree);

	memset(entries, 0, COLS * sizeof(char *));

	for (row = 0; row < ROWS; row++) {

		/*
		 * Build the array of entries.
		 */
		for (col = 0; col < COLS; col++) {
			snprintf(buf, 1024, "Row %d Col %d", row, col);
			IF_FREE(entries[col]);
			entries[col] = strdup(buf);
		}

		if (row % NEST == 0)
			prow = ewl_tree_add_text_row(EWL_TREE(tree), NULL,
					entries);
		else
			prow = ewl_tree_add_text_row(EWL_TREE(tree),
					EWL_ROW(prow), entries);
		/* ewl_tree_set_row_expanded(EWL_ROW(prow), TRUE); */

		for (col = 0; col < COLS; col++) {
			IF_FREE(entries[col]);
		}
	}

	for (col = 0; col < COLS; col++) {
		IF_FREE(headers[col]);
	}

	printf("Tree setup complete!!!\n");

	return;
	ev_data = NULL;
	user_data = NULL;
}
