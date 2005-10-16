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
   Etk_Tree_Col *col1, *col2, *col3;
   Etk_Tree_Row *row;
   Etk_Widget *table;
   Etk_Widget *label;
   int i;

   if (win)
	{
		etk_widget_show(ETK_WIDGET(win));
		return;
	}

   win = etk_window_new();
   etk_window_title_set(ETK_WINDOW(win), _("Etk Tree Test"));
   etk_signal_connect("delete_event", ETK_OBJECT(win), ETK_CALLBACK(_etk_test_tree_window_deleted_cb), win);	
	
   table = etk_table_new(2, 2, FALSE);
   etk_container_add(ETK_CONTAINER(win), table);

   label = etk_label_new(_("<h1>Tree:</h1>"));
   etk_table_attach(ETK_TABLE(table), label, 0, 0, 0, 0, 10, 0, ETK_FILL_POLICY_HFILL);

   label = etk_label_new(_("<h1>List:</h1>"));
   etk_table_attach(ETK_TABLE(table), label, 1, 1, 0, 0, 10, 0, ETK_FILL_POLICY_HFILL);

   /* The tree: */
   tree = etk_tree_new();
   etk_widget_size_request_set(tree, 300, 400);
   etk_table_attach_defaults(ETK_TABLE(table), tree, 0, 0, 1, 1);

   etk_tree_mode_set(ETK_TREE(tree), ETK_TREE_MODE_TREE);
   col1 = etk_tree_col_new(ETK_TREE(tree), _("Column 1"), ETK_TREE_COL_ICON_TEXT);
   col2 = etk_tree_col_new(ETK_TREE(tree), _("Column 2"), ETK_TREE_COL_INT);
   col3 = etk_tree_col_new(ETK_TREE(tree), _("Column 3"), ETK_TREE_COL_IMAGE);
   etk_tree_build(ETK_TREE(tree));

   etk_tree_freeze(ETK_TREE(tree));
   for (i = 0; i < 1000; i++)
   {
      row = etk_tree_append(ETK_TREE(tree), col1, PACKAGE_DATA_DIR "/images/open.png", _("Row1"),
         col2, 1, col3, PACKAGE_DATA_DIR "/images/1star.png", NULL);
      row = etk_tree_append_to_row(row, col1, PACKAGE_DATA_DIR "/images/open.png", _("Row2"),
         col2, 2, col3, PACKAGE_DATA_DIR "/images/2stars.png", NULL);
      etk_tree_append_to_row(row, col1, PACKAGE_DATA_DIR "/images/open.png", _("Row3"),
         col2, 3, col3, PACKAGE_DATA_DIR "/images/3stars.png", NULL);
   }
   etk_tree_thaw(ETK_TREE(tree));


   /* The list: */
   tree = etk_tree_new();
   etk_widget_size_request_set(tree, 300, 400);
   etk_table_attach_defaults(ETK_TABLE(table), tree, 1, 1, 1, 1);

   etk_tree_multiple_select_set(ETK_TREE(tree), TRUE);
   col1 = etk_tree_col_new(ETK_TREE(tree), _("Column 1"), ETK_TREE_COL_TEXT);
   col2 = etk_tree_col_new(ETK_TREE(tree), _("Column 2"), ETK_TREE_COL_INT);
   col3 = etk_tree_col_new(ETK_TREE(tree), _("Column 3"), ETK_TREE_COL_IMAGE);
   etk_tree_build(ETK_TREE(tree));

   etk_tree_freeze(ETK_TREE(tree));
   for (i = 0; i < 300; i++)
   {
      etk_tree_append(ETK_TREE(tree), col1, _("Row1"), col2, 1, col3, PACKAGE_DATA_DIR "/images/1star.png", NULL);
      etk_tree_append(ETK_TREE(tree), col1, _("Row2"), col2, 2, col3, PACKAGE_DATA_DIR "/images/2stars.png", NULL);
      etk_tree_append(ETK_TREE(tree), col1, _("Row3"), col2, 3, col3, PACKAGE_DATA_DIR "/images/3stars.png", NULL);
      etk_tree_append(ETK_TREE(tree), col1, _("Row4"), col2, 1, col3, PACKAGE_DATA_DIR "/images/3stars.png", NULL);
      etk_tree_append(ETK_TREE(tree), col1, _("Row5"), col2, 2, col3, PACKAGE_DATA_DIR "/images/2stars.png", NULL);
      etk_tree_append(ETK_TREE(tree), col1, _("Row6"), col2, 3, col3, PACKAGE_DATA_DIR "/images/1star.png", NULL);
      etk_tree_append(ETK_TREE(tree), col1, _("Row7"), col2, 1, col3, PACKAGE_DATA_DIR "/images/1star.png", NULL);
      etk_tree_append(ETK_TREE(tree), col1, _("Row8"), col2, 2, col3, PACKAGE_DATA_DIR "/images/2stars.png", NULL);
      etk_tree_append(ETK_TREE(tree), col1, _("Row9"), col2, 3, col3, PACKAGE_DATA_DIR "/images/3stars.png", NULL);
      etk_tree_append(ETK_TREE(tree), col1, _("Row10"), col2, 1, col3, PACKAGE_DATA_DIR "/images/3stars.png", NULL);
      etk_tree_append(ETK_TREE(tree), col1, _("Row11"), col2, 2, col3, PACKAGE_DATA_DIR "/images/2stars.png", NULL);
      etk_tree_append(ETK_TREE(tree), col1, _("Row12"), col2, 3, col3, PACKAGE_DATA_DIR "/images/1star.png", NULL);
   }
   etk_tree_thaw(ETK_TREE(tree));
   
   etk_widget_show_all(win);
}
