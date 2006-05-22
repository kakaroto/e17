#include "Ewl_Test.h"
#include "ewl_test_private.h"
#include <stdio.h>
#include <limits.h>
#include <string.h>
#include <stdlib.h>

#define ROWS 50
#define COLS 4
#define NEST 3

static int create_test(Ewl_Container *box);
static void cb_get_rows(Ewl_Widget *w, void *ev, void *data);

void 
test_info(Ewl_Test *test)
{
	test->name = "Tree";
	test->tip = "Defines a widget for laying out other\n"
			"widgets in a tree or list like manner.";
	test->filename = "ewl_tree.c";
	test->func = create_test;
	test->type = EWL_TEST_TYPE_CONTAINER;
}

static int
create_test(Ewl_Container *box)
{
	int row, col;
	char buf[PATH_MAX];
	char *headers[COLS], *entries[COLS];
	Ewl_Widget *tree, *prow = NULL;
	Ewl_Widget *hbox, *button;

	hbox = ewl_hbox_new();
	ewl_container_child_append(EWL_CONTAINER(box), hbox);
	ewl_object_fill_policy_set(EWL_OBJECT(hbox), EWL_FLAG_FILL_SHRINK |
						    EWL_FLAG_FILL_HFILL);
	ewl_widget_show(hbox);

	tree = ewl_tree_new(COLS);

	button = ewl_button_new();
	ewl_button_label_set(EWL_BUTTON(button), "Number of selected rows");
	ewl_container_child_append(EWL_CONTAINER(hbox), button);
	ewl_object_fill_policy_set(EWL_OBJECT(button), EWL_FLAG_FILL_HFILL);
	ewl_callback_append(button, EWL_CALLBACK_CLICKED, cb_get_rows, tree);
	ewl_widget_show(button);

	button = ewl_button_new();
	ewl_button_label_set(EWL_BUTTON(button), "Clear selection");
	ewl_container_child_append(EWL_CONTAINER(hbox), button);
	ewl_object_fill_policy_set(EWL_OBJECT(button), EWL_FLAG_FILL_HFILL);
	ewl_callback_append(button, EWL_CALLBACK_CLICKED, cb_get_rows, tree);
	ewl_widget_show(button);

	for (col = 0; col < COLS; col++) 
	{
		snprintf(buf, PATH_MAX, "Column %d", col);
		headers[col] = strdup(buf);
	}

	ewl_container_child_append(EWL_CONTAINER(box), tree);
	ewl_tree_headers_set(EWL_TREE(tree), headers);
	ewl_widget_show(tree);

	memset(entries, 0, COLS * sizeof(char *));

	for (row = 0; row < ROWS; row++) 
	{

		for (col = 0; col < COLS; col++) 
		{
			snprintf(buf, 1024, "Row %d Col %d", row, col);
			entries[col] = strdup(buf);
		}

		if (row % NEST == 0)
			prow = ewl_tree_text_row_add(EWL_TREE(tree), NULL,
					entries);
		else
			prow = ewl_tree_text_row_add(EWL_TREE(tree),
					EWL_ROW(prow), entries);

		for (col = 0; col < COLS; col++) 
		{
			if (entries[col])
				free(entries[col]);
			entries[col] = NULL;
		}
	}

	for (col = 0; col < COLS; col++) 
	{
		if (headers[col])
			free(headers[col]);
	}

	return 1;
}

static void
cb_get_rows(Ewl_Widget *w __UNUSED__, void *ev __UNUSED__, void *data)
{
	Ecore_List *selected;
	Ewl_Tree *tree;
	
	tree = data;
	selected = ewl_tree_selected_get(tree);
	printf("Selected %d rows\n", ecore_list_nodes(selected));
}

