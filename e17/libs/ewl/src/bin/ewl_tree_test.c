#include "ewl_test.h"

#define ROWS 50
#define COLS 4
#define NEST 3

static Ewl_Widget *tree_button;

static void
__destroy_tree_test_window(Ewl_Widget * w, void *ev_data __UNUSED__,
				 void *user_data __UNUSED__)
{
	ewl_widget_destroy(w);
	ewl_callback_append(tree_button, EWL_CALLBACK_CLICKED,
			    __create_tree_test_window, NULL);
}

static void
__get_rows(Ewl_Widget *w __UNUSED__, void *ev_data __UNUSED__,
					void *user_data)
{
	Ecore_List *selected;
	Ewl_Tree *tree;
	
	tree = user_data;
	selected = ewl_tree_selected_get(tree);
	printf("Selected %d rows\n", ecore_list_nodes(selected));
}

void
__create_tree_test_window(Ewl_Widget * w, void *ev_data __UNUSED__,
				void *user_data __UNUSED__)
{
	int row, col;
	char buf[PATH_MAX];
	char *headers[COLS];
	char *entries[COLS];
	Ewl_Widget *tree_win;
	Ewl_Widget *tree;
	Ewl_Widget *prow = NULL;
	Ewl_Widget *box;
	Ewl_Widget *hbox;
	Ewl_Widget *button;

	tree_button = w;

	tree_win = ewl_window_new();
	ewl_object_fill_policy_set(EWL_OBJECT(tree_win), EWL_FLAG_FILL_ALL);
	ewl_window_title_set(EWL_WINDOW(tree_win), "Tree Test");
	ewl_window_name_set(EWL_WINDOW(tree_win), "EWL Test Application");
	ewl_window_class_set(EWL_WINDOW(tree_win), "EFL Test Application");
	/* ewl_object_set_maximum_size(EWL_OBJECT(tree_win), 400, 400); */
	ewl_object_size_request(EWL_OBJECT(tree_win), 400, 200);

	if (w) {
		ewl_callback_del(w, EWL_CALLBACK_CLICKED,
		                	 __create_tree_test_window);
		ewl_callback_append(tree_win, EWL_CALLBACK_DELETE_WINDOW,
			    __destroy_tree_test_window, NULL);
	} else
		ewl_callback_append(tree_win, EWL_CALLBACK_DELETE_WINDOW,
						__close_main_window, NULL);
	ewl_widget_show(tree_win);

	box = ewl_vbox_new();
	ewl_container_child_append(EWL_CONTAINER(tree_win), box);
	ewl_box_homogeneous_set(EWL_BOX(box), FALSE);
	ewl_object_fill_policy_set(EWL_OBJECT(box), EWL_FLAG_FILL_SHRINK);
	ewl_widget_show(box);

	hbox = ewl_hbox_new();
	ewl_container_child_append(EWL_CONTAINER(box), hbox);
	ewl_object_fill_policy_set(EWL_OBJECT(hbox), EWL_FLAG_FILL_SHRINK |
						    EWL_FLAG_FILL_HFILL);
	ewl_widget_show(hbox);

	/*
	 * Create the tree first to allow for attaching to button callbacks
	 */
	tree = ewl_tree_new(COLS);

	/*
	 * Create buttons for retrieving tree info
	 */
	button = ewl_button_new();
	ewl_button_label_set(EWL_BUTTON(button), "Number of selected rows");
	ewl_container_child_append(EWL_CONTAINER(hbox), button);
	ewl_object_fill_policy_set(EWL_OBJECT(button), EWL_FLAG_FILL_HFILL);
	ewl_callback_append(button, EWL_CALLBACK_CLICKED, __get_rows, tree);
	ewl_widget_show(button);

	button = ewl_button_new();
	ewl_button_label_set(EWL_BUTTON(button), "Clear selection");
	ewl_container_child_append(EWL_CONTAINER(hbox), button);
	ewl_object_fill_policy_set(EWL_OBJECT(button), EWL_FLAG_FILL_HFILL);
	ewl_callback_append(button, EWL_CALLBACK_CLICKED, __get_rows, tree);
	ewl_widget_show(button);

	for (col = 0; col < COLS; col++) {
		snprintf(buf, PATH_MAX, "Column %d", col);
		headers[col] = strdup(buf);
	}

	ewl_container_child_append(EWL_CONTAINER(box), tree);
	ewl_tree_headers_set(EWL_TREE(tree), headers);
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
			prow = ewl_tree_text_row_add(EWL_TREE(tree), NULL,
					entries);
		else
			prow = ewl_tree_text_row_add(EWL_TREE(tree),
					EWL_ROW(prow), entries);

		for (col = 0; col < COLS; col++) {
			IF_FREE(entries[col]);
		}
	}

	for (col = 0; col < COLS; col++) {
		IF_FREE(headers[col]);
	}

	printf("Tree setup complete!!!\n");
}

