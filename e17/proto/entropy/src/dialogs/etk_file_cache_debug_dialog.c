#include "entropy.h"
#include <Etk.h>

void etk_mime_dialog_create()
{
	Etk_Widget* window = NULL;
	Etk_Widget* tree = NULL;
	Etk_Tree_Col* tree_col;

	Etk_Tree_Col* col1;
	Etk_Tree_Col* col2;


	char* key;
	entropy_mime_action* action;
	Ecore_List* keys;

	window = etk_window_new();

	etk_window_title_set(ETK_WINDOW(window), "Program Associations");
	etk_window_wmclass_set(ETK_WINDOW(window), "mimedialog", "mimedialog");

	etk_widget_size_request_set(ETK_WIDGET(window), 450, 500);


	tree = etk_tree_new();
	etk_tree_mode_set(ETK_TREE(tree), ETK_TREE_MODE_LIST);
	tree_col = etk_tree_col_new(ETK_TREE(tree), _("File Type"), 
		  etk_tree_model_text_new(ETK_TREE(tree)), 125);

	tree_col = etk_tree_col_new(ETK_TREE(tree), _("Executable"), 
		  etk_tree_model_text_new(ETK_TREE(tree)), 150);
        etk_tree_col_expand_set(tree_col, ETK_TRUE);

	etk_tree_build(ETK_TREE(tree));

	etk_container_add(ETK_CONTAINER(window), tree);

	/*Populate the tree*/
	col1 = etk_tree_nth_col_get(ETK_TREE(tree), 0);
	col2 = etk_tree_nth_col_get(ETK_TREE(tree), 1);
	
	keys = ecore_hash_keys(entropy_core_get_core()->mime_action_hint);
	while ((key = ecore_list_remove_first(keys))) {
	     etk_tree_append(ETK_TREE(tree), 
		  col1, key, 
		  col2,  ((entropy_mime_action*)ecore_hash_get(entropy_core_get_core()->mime_action_hint, key))->executable,
		  NULL);
	}


	etk_widget_show_all(window);
}

