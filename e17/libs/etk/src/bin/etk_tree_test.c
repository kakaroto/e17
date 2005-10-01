#include "etk_test.h"
#include "../../config.h"

static Etk_Bool _etk_test_tree_window_deleted_cb(void *data)
{
   etk_widget_hide(ETK_WIDGET(data));
   return 1;
}

void etk_test_tree_window_create(void *data)
{
   static Etk_Widget *win = NULL;
   Etk_Widget *tree;
   Etk_Tree_Col *col1, *col2, *col3, *col4;

   if (win)
	{
		etk_widget_show(ETK_WIDGET(win));
		return;
	}

   win = etk_window_new();
   etk_window_title_set(ETK_WINDOW(win), "Etk Tree Test");

   etk_signal_connect("delete_event", ETK_OBJECT(win), ETK_CALLBACK(_etk_test_tree_window_deleted_cb), win);	
	
   tree = etk_tree_new();
   col1 = etk_tree_col_new(ETK_TREE(tree), "Column 1", ETK_TREE_COL_TEXT);
   col2 = etk_tree_col_new(ETK_TREE(tree), "Column 2", ETK_TREE_COL_INT);
   col3 = etk_tree_col_new(ETK_TREE(tree), "Column 3", ETK_TREE_COL_TEXT);
   col4 = etk_tree_col_new(ETK_TREE(tree), "Column 4", ETK_TREE_COL_DOUBLE);
   etk_tree_build(ETK_TREE(tree));

   etk_tree_append(ETK_TREE(tree), col1, "Row1", col2, 1, col3, "Test1", col4, 1.0, NULL);
   etk_tree_append(ETK_TREE(tree), col1, "Row2", col2, 2, col3, "Test2", col4, 2.0, NULL);
   etk_tree_append(ETK_TREE(tree), col1, "Row3", col2, 3, col3, "Test3", col4, 3.0, NULL);
   etk_tree_append(ETK_TREE(tree), col1, "Row1", col2, 1, col3, "Test4", col4, 1.0, NULL);
   etk_tree_append(ETK_TREE(tree), col1, "Row2", col2, 2, col3, "Test5", col4, 2.0, NULL);
   etk_tree_append(ETK_TREE(tree), col1, "Row3", col2, 3, col3, "Test6", col4, 3.0, NULL);
   etk_tree_append(ETK_TREE(tree), col1, "Row1", col2, 1, col3, "Test7", col4, 1.0, NULL);
   etk_tree_append(ETK_TREE(tree), col1, "Row2", col2, 2, col3, "Test8", col4, 2.0, NULL);
   etk_tree_append(ETK_TREE(tree), col1, "Row3", col2, 3, col3, "Test9", col4, 3.0, NULL);
   etk_tree_append(ETK_TREE(tree), col1, "Row1", col2, 1, col3, "Test10", col4, 1.0, NULL);
   etk_tree_append(ETK_TREE(tree), col1, "Row2", col2, 2, col3, "Test11", col4, 2.0, NULL);
   etk_tree_append(ETK_TREE(tree), col1, "Row3", col2, 3, col3, "Test12", col4, 3.0, NULL);
   etk_tree_append(ETK_TREE(tree), col1, "Row1", col2, 1, col3, "Test13", col4, 1.0, NULL);
   etk_tree_append(ETK_TREE(tree), col1, "Row2", col2, 2, col3, "Test14", col4, 2.0, NULL);
   etk_tree_append(ETK_TREE(tree), col1, "Row3", col2, 3, col3, "Test15", col4, 3.0, NULL);

   etk_container_add(ETK_CONTAINER(win), tree);
   
   etk_widget_show_all(win);
}
