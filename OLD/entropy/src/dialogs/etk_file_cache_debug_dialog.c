#include "entropy.h"
#include <Etk.h>

static int _etk_file_cache_dialog_running = 0;
void etk_file_cache_dialog_refresh(Etk_Widget* tree);

static int _entropy_etk_file_cache_dialog_listener_compare_cb
(Etk_Tree *tree, Etk_Tree_Row *row1, Etk_Tree_Row *row2, Etk_Tree_Col *col, void *data)
{
	int val1, val2;

   etk_tree_row_fields_get(row1, col, &val1, NULL);
   etk_tree_row_fields_get(row2, col, &val2, NULL);

   if (val1 < val2)
      return 1;
   else if (val1 > val2)
      return -1;
   else
      return 0;
	
}

static Etk_Bool
_etk_file_cache_debug_dialog_delete_cb (Etk_Object * object, void *data)
{
	_etk_file_cache_dialog_running = 0;
	etk_object_destroy(object);

	return ETK_TRUE;
}

void _etk_file_cache_dialog_refresh_cb(Etk_Object* object, void* data)
{
	etk_file_cache_dialog_refresh(ETK_WIDGET(data));
}

void etk_file_cache_dialog_refresh(Etk_Widget* tree)
{
	Etk_Tree_Col* col1;
	Etk_Tree_Col* col2;

	char* key;
	char buffer[PATH_MAX];
	Ecore_List* keys;	
	
	etk_tree_clear(ETK_TREE(tree));
	
	/*Populate the tree*/
	col1 = etk_tree_nth_col_get(ETK_TREE(tree), 0);
	col2 = etk_tree_nth_col_get(ETK_TREE(tree), 1);

	etk_tree_freeze(ETK_TREE(tree));
	
	keys = entropy_core_file_cache_keys_retrieve();
	while ( (key = ecore_list_first_remove(keys))) {
		  entropy_file_listener* listen = entropy_core_file_cache_retrieve(key);
		  
		  if (listen) {
			  snprintf(buffer, PATH_MAX, "%s/%s", listen->file->path, listen->file->filename);
		
			  etk_tree_row_append(ETK_TREE(tree), NULL,
			  col1, listen->count, 
			  col2,   buffer,
			  NULL);
		  }
	
	}
	ecore_list_destroy(keys);

	etk_tree_thaw(ETK_TREE(tree));
}

void etk_file_cache_dialog_create()
{
	Etk_Widget* window = NULL;
	Etk_Widget* tree = NULL;
	Etk_Tree_Col* tree_col;
	Etk_Widget* button;
	Etk_Widget* vbox;


	if (_etk_file_cache_dialog_running)
		return;

	_etk_file_cache_dialog_running = 1;

	window = etk_window_new();

	etk_window_title_set(ETK_WINDOW(window), "File Cache");
	etk_window_wmclass_set(ETK_WINDOW(window), "entropyfilecache", "entropyfilecache");

	etk_widget_size_request_set(ETK_WIDGET(window), 450, 500);

	vbox = etk_vbox_new(ETK_FALSE,0);
	etk_container_add(ETK_CONTAINER(window), vbox);


	tree = etk_tree_new();
	etk_box_append(ETK_BOX(vbox), tree, ETK_BOX_START, ETK_BOX_EXPAND_FILL, 0);
	
	etk_tree_mode_set(ETK_TREE(tree), ETK_TREE_MODE_LIST);

	tree_col = etk_tree_col_new(ETK_TREE(tree), _("Listeners"), 125,0.0);
	etk_tree_col_model_add(tree_col, etk_tree_model_text_new());

  // Not yet implemented in Tree2
	//etk_tree_col_sort_func_set(tree_col, _entropy_etk_file_cache_dialog_listener_compare_cb, NULL);

	tree_col = etk_tree_col_new(ETK_TREE(tree), _("Filename"), 150, 0.0);
	etk_tree_col_model_add(tree_col, etk_tree_model_text_new());
        
	etk_tree_col_expand_set(tree_col, ETK_TRUE);

	etk_tree_build(ETK_TREE(tree));

	
	etk_file_cache_dialog_refresh(tree);

	button = etk_button_new_with_label("Refresh");
	etk_signal_connect("pressed", ETK_OBJECT(button), ETK_CALLBACK(_etk_file_cache_dialog_refresh_cb), tree);

	
	etk_box_append(ETK_BOX(vbox), button, ETK_BOX_END, ETK_BOX_NONE, 0);


	  etk_signal_connect ("delete_event", ETK_OBJECT (window),
		      ETK_CALLBACK (_etk_file_cache_debug_dialog_delete_cb), window);
	
	etk_widget_show_all(window);
}

