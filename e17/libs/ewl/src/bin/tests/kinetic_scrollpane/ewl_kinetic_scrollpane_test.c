/* vim: set sw=8 ts=8 sts=8 noexpandtab: */
#include "Ewl_Test.h"
#include "ewl_test_private.h"
#include "ewl_tree.h"
#include "ewl_label.h"

#include <stdio.h>
#include <string.h>

static int create_test(Ewl_Container *box);

void
test_info(Ewl_Test *test)
{
	test->name = "Kinetic Scrollpane";
	test->tip = "A kinetic scrollpane";
	test->filename = __FILE__;
	test->func = create_test;
	test->type = EWL_TEST_TYPE_CONTAINER;
}

static int
create_test(Ewl_Container *box)
{
	Ewl_Widget *tree;
	Ewl_Model *model;
	Ewl_View *view;
	Ecore_List *data;
	int i;

	data = ecore_list_new();
	for (i = 0; i < 500; i++)
	{
		char text[15];

		snprintf(text, sizeof(text), "Row: %d", i);
		ecore_list_append(data, strdup(text));
	}


	model = ewl_model_ecore_list_get();
	view = ewl_label_view_get();

	tree = ewl_tree_new();
	ewl_tree_headers_visible_set(EWL_TREE(tree), FALSE);
	ewl_tree_fixed_rows_set(EWL_TREE(tree), TRUE);
	ewl_tree_kinetic_scrolling_set(EWL_TREE(tree), EWL_KINETIC_SCROLL_NORMAL);
	ewl_tree_kinetic_fps_set(EWL_TREE(tree), 30);
	ewl_tree_column_count_set(EWL_TREE(tree), 1);
	ewl_mvc_model_set(EWL_MVC(tree), model);
	ewl_mvc_view_set(EWL_MVC(tree), view);
	ewl_mvc_data_set(EWL_MVC(tree), data);
	ewl_container_child_append(box, tree);
	ewl_widget_show(tree);

	return 1;
}
