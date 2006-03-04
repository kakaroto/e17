#include "entropy.h"
#include <Etk.h>

void etk_mime_dialog_create()
{
	Etk_Widget* window = NULL;
	Etk_Widget* tree = NULL;
	Etk_Tree_Col* tree_col;

	window = etk_window_new();

	etk_window_title_set(ETK_WINDOW(window), "Program Associations");
	etk_window_wmclass_set(ETK_WINDOW(window), "mimedialog", "mimedialog");

	etk_widget_size_request_set(ETK_WIDGET(window), 450, 500);


	tree = etk_tree_new();
	etk_tree_mode_set(ETK_TREE(tree), ETK_TREE_MODE_LIST);
	tree_col = etk_tree_col_new(ETK_TREE(tree), _("File Type"), 
		  etk_tree_model_text_new(ETK_TREE(tree)), 55);
        /*etk_tree_col_expand_set(tree_col, ETK_TRUE);*/

	tree_col = etk_tree_col_new(ETK_TREE(tree), _("Executable"), 
		  etk_tree_model_text_new(ETK_TREE(tree)), 100);
        etk_tree_col_expand_set(tree_col, ETK_TRUE);

	etk_tree_build(ETK_TREE(tree));

	etk_container_add(ETK_CONTAINER(window), tree);


	etk_widget_show_all(window);
}

