#include "entropy.h"
#include <Etk.h>

static int _etk_mime_dialog_displayed = 0;
static int _mime_current_binding = -1;
static Etk_Widget* mime_dialog_window = NULL;
static Etk_Widget* mime_dialog_add_edit_window = NULL;
static Etk_Widget* mime_dialog_add_edit_application_window = NULL;

static Entropy_Config_Mime_Binding_Action* edit_action = NULL;

static	Etk_Widget* etk_mime_mime_entry;
static	Etk_Widget* etk_mime_desc_entry;

static	Etk_Widget* etk_mime_app_name_entry;
static	Etk_Widget* etk_mime_app_exe_entry;
static	Etk_Widget* etk_mime_app_args_entry;


static Etk_Widget* _etk_mime_dialog_main_tree = NULL;
static Etk_Widget* _etk_mime_dialog_sub_tree = NULL;


void _entropy_etk_mime_dialog_add_cb(Etk_Object* w, void* user_data);
static Etk_Bool _etk_window_deleted_cb (Etk_Object * object, void *data);
void etk_mime_dialog_add_edit_create();
void etk_mime_dialog_create();
void etk_mime_dialog_tree_populate();
void etk_mime_dialog_application_create(char* mime, Entropy_Config_Mime_Binding_Action*);
void _entropy_etk_mime_dialog_app_add_edit_final_cb(Etk_Object* w, void* user_data);
void etk_mime_dialog_populate_nth_binding_apps(int record);

/*Various callbacks*/
static Etk_Bool
_etk_window_deleted_cb (Etk_Object * object, void *data)
{
	_etk_mime_dialog_displayed = 0;
	etk_object_destroy(ETK_OBJECT(mime_dialog_window));

	/*Save the config*/
	entropy_config_eet_config_save();

	return ETK_TRUE;
}

static Etk_Bool
_etk_window_deleted_add_cb (Etk_Object * object, void *data)
{
	etk_object_destroy(ETK_OBJECT(mime_dialog_add_edit_window));
	mime_dialog_add_edit_window = NULL;

	return ETK_TRUE;
}

static Etk_Bool
_etk_window_deleted_add_app_cb (Etk_Object * object, void *data)
{
	etk_object_destroy(ETK_OBJECT(mime_dialog_add_edit_application_window));
	mime_dialog_add_edit_application_window = NULL;

	return ETK_TRUE;
}

void _entropy_etk_mime_dialog_add_edit_cancel_cb(Etk_Object* w, void* user_data)
{
	if (mime_dialog_add_edit_window) {
		etk_object_destroy(ETK_OBJECT(mime_dialog_add_edit_window));
		mime_dialog_add_edit_window = NULL;
	}
}

void _entropy_etk_mime_dialog_app_add_edit_cancel_cb(Etk_Object* w, void* user_data)
{
	if (mime_dialog_add_edit_application_window) {
		etk_object_destroy(ETK_OBJECT(mime_dialog_add_edit_application_window));
		mime_dialog_add_edit_application_window = NULL;
		
	}
}


void _entropy_etk_mime_dialog_add_edit_final_cb(Etk_Object* w, void* user_data)
{
	const char *type_text = etk_entry_text_get(ETK_ENTRY(etk_mime_mime_entry));
	const char *desc_text = etk_entry_text_get(ETK_ENTRY(etk_mime_desc_entry));

	entropy_core_mime_action_add((char*)type_text, (char*)desc_text);

	etk_object_destroy(ETK_OBJECT(mime_dialog_add_edit_window));
	mime_dialog_add_edit_window= NULL;

	etk_mime_dialog_tree_populate();
}


void _entropy_etk_mime_dialog_app_add_edit_final_cb(Etk_Object* w, void* user_data)
{
	const char *name = etk_entry_text_get(ETK_ENTRY(etk_mime_app_name_entry));
	const char *exe = etk_entry_text_get(ETK_ENTRY(etk_mime_app_exe_entry));
	const char *args = etk_entry_text_get(ETK_ENTRY(etk_mime_app_args_entry)); 
	const char *mime = user_data;

	if (!edit_action) {
		if (mime && args && exe && name)
			entropy_core_mime_application_add((char*)mime, (char*)name, (char*)exe, (char*)args);
	} else {
		if (edit_action->app_description) free(edit_action->app_description);
		if (name) edit_action->app_description = strdup(name);
		if (edit_action->executable) free(edit_action->executable);
		if (exe) edit_action->executable = strdup(exe);
		if (edit_action->args) free(edit_action->args);
		if (args) edit_action->args = strdup(args);
	}

	etk_mime_dialog_populate_nth_binding_apps(_mime_current_binding);

	etk_object_destroy(ETK_OBJECT(mime_dialog_add_edit_application_window));
	mime_dialog_add_edit_application_window = NULL;

}


void _entropy_etk_mime_dialog_edit_cb(Etk_Object* w, void* user_data)
{
	Etk_Tree_Row* row = etk_tree_selected_row_get(ETK_TREE(_etk_mime_dialog_main_tree));
	char* mime = NULL;
	char* program = NULL;
	Etk_Tree_Col* col1;
	Etk_Tree_Col* col2;

	if (row) {
		col1 = etk_tree_nth_col_get(ETK_TREE(_etk_mime_dialog_main_tree), 0);
		col2 = etk_tree_nth_col_get(ETK_TREE(_etk_mime_dialog_main_tree), 1);

		etk_tree_row_fields_get(row, col1, &mime, col2, &program,NULL);
	
		etk_mime_dialog_add_edit_create(mime,program);
	}
}

void _entropy_etk_mime_dialog_remove_cb(Etk_Object* w, void* user_data)
{
	Etk_Tree_Row* row = etk_tree_selected_row_get(ETK_TREE(_etk_mime_dialog_main_tree));
	char* mime = NULL;
	Etk_Tree_Col* col1;
	Etk_Tree_Col* col2;

	if (row) {
		col1 = etk_tree_nth_col_get(ETK_TREE(_etk_mime_dialog_main_tree), 0);
		col2 = etk_tree_nth_col_get(ETK_TREE(_etk_mime_dialog_main_tree), 1);

		etk_tree_row_fields_get(row, col2, &mime, NULL);

		printf("Del mime is '%s'..\n", mime);
	
		entropy_core_mime_action_remove(mime);
		etk_mime_dialog_tree_populate();
	}
}

void _entropy_etk_mime_dialog_remove_app_cb(Etk_Object* w, void* user_data)
{
	Etk_Tree_Row* row = etk_tree_selected_row_get(ETK_TREE(_etk_mime_dialog_main_tree));
	Etk_Tree_Row* app_row = etk_tree_selected_row_get(ETK_TREE(_etk_mime_dialog_sub_tree));
	char* mime = NULL;
	Entropy_Config_Mime_Binding_Action* action;
	Etk_Tree_Col* col1;
	Etk_Tree_Col* col2;

	if (row && app_row) {
		col1 = etk_tree_nth_col_get(ETK_TREE(_etk_mime_dialog_main_tree), 0);
		col2 = etk_tree_nth_col_get(ETK_TREE(_etk_mime_dialog_main_tree), 1);

		action = etk_tree_row_data_get(app_row);
		etk_tree_row_fields_get(row, col2, &mime, NULL);

		entropy_core_mime_action_remove_app(mime, action);

		etk_mime_dialog_populate_nth_binding_apps((int)etk_tree_row_data_get(row));
	}
}


void _entropy_etk_mime_dialog_add_cb(Etk_Object* w, void* user_data)
{
	etk_mime_dialog_add_edit_create(NULL,NULL);
}

void _entropy_etk_mime_dialog_application_add_cb(Etk_Object* w, void* user_data)
{
	Etk_Tree_Col* col2;
	Etk_Tree_Row* row;
	char* mime_type = NULL;
	Entropy_Config_Mime_Binding_Action* action = NULL;


	col2 = etk_tree_nth_col_get(ETK_TREE(_etk_mime_dialog_main_tree), 1);
	row = etk_tree_selected_row_get(ETK_TREE(_etk_mime_dialog_main_tree));

	if (row) {
		int isedit = (int)user_data;

		if (isedit) {
			Etk_Tree_Row* app_row = etk_tree_selected_row_get(ETK_TREE(_etk_mime_dialog_sub_tree));
			edit_action = etk_tree_row_data_get(app_row);
			action = edit_action;
		} else {
			edit_action = NULL;
		}
		
		etk_tree_row_fields_get(row, col2, &mime_type, NULL);
		if (mime_type) {
			if ( (edit_action == NULL && isedit == 0) || (edit_action && isedit == 1)) 
				etk_mime_dialog_application_create(mime_type,action);	
		}
	}
}


/*--------------------------*/

void etk_mime_dialog_populate_nth_binding_apps(int record)
{
	Entropy_Config_Mime_Binding* binding;
	Evas_List* l;
	Entropy_Config_Mime_Binding_Action* action;
	Etk_Tree_Col* col1;
	Etk_Tree_Col* col2;
	Etk_Tree_Col* col3;
	Etk_Widget* tree = _etk_mime_dialog_sub_tree;
	Etk_Tree_Row* row;

	/*Do we have a valid record?*/
	if (record < 0) return;


	
	binding = evas_list_nth(entropy_core_get_core()->config->Loaded_Config->mime_bindings, record);

	etk_tree_freeze(ETK_TREE(tree));
	etk_tree_clear(ETK_TREE(tree));

	/*Populate the tree*/
	col1 = etk_tree_nth_col_get(ETK_TREE(tree), 0);
	col2 = etk_tree_nth_col_get(ETK_TREE(tree), 1);
	col3 = etk_tree_nth_col_get(ETK_TREE(tree), 2);


	for (l = binding->actions; l; ) {
		action = l->data;

		row = etk_tree_row_append(ETK_TREE(tree), NULL,
		  col1, action->app_description, 
		  col2,  action->executable,
		  col3, action->args,
		  NULL);
		etk_tree_row_data_set(row, action);
		
		l = l->next;
	}

	etk_tree_thaw(ETK_TREE(tree));

}


static void _etk_mime_dialog_main_row_clicked(Etk_Object *object, Etk_Tree_Row *row, Etk_Event_Mouse_Down *event, void *data)
{
	int record;

	record = (int)etk_tree_row_data_get(row);
	_mime_current_binding = record;

	etk_mime_dialog_populate_nth_binding_apps(record);
}



void etk_mime_dialog_tree_populate()
{
	Etk_Widget* tree = _etk_mime_dialog_main_tree;
	Etk_Tree_Col* col1;
	Etk_Tree_Col* col2;
	Entropy_Config_Mime_Binding* binding;
	//Entropy_Config_Mime_Binding_Action* action;
	Evas_List* l;
	int i =0;
	Etk_Tree_Row* row;

	
	etk_tree_freeze(ETK_TREE(tree));
	etk_tree_clear(ETK_TREE(tree));

	/*Populate the tree*/
	col1 = etk_tree_nth_col_get(ETK_TREE(tree), 0);
	col2 = etk_tree_nth_col_get(ETK_TREE(tree), 1);


	for (l = entropy_core_get_core()->config->Loaded_Config->mime_bindings; l; ) {
		binding = l->data;

		row = etk_tree_row_append(ETK_TREE(tree), NULL,
		  col1, binding->desc, 
		  col2,  binding->mime_type,
		  NULL);
		etk_tree_row_data_set(row, (int*)i);
		
		l = l->next;
		i++;
	}

	etk_tree_thaw(ETK_TREE(tree));
}


void etk_mime_dialog_add_edit_create(char* mime, char* program) {
	Etk_Widget* table;
	Etk_Widget* label;
	Etk_Widget* button;

	if (mime_dialog_add_edit_window)
		return;

	mime_dialog_add_edit_window = etk_window_new();
	etk_window_title_set(ETK_WINDOW(mime_dialog_add_edit_window), "MIME Type");
	etk_window_wmclass_set(ETK_WINDOW(mime_dialog_add_edit_window), "mimedialog", "mimedialog");
	etk_widget_size_request_set(ETK_WIDGET(mime_dialog_add_edit_window), 250, 80);

	etk_signal_connect ("delete-event", ETK_OBJECT (mime_dialog_add_edit_window),
	      ETK_CALLBACK (_etk_window_deleted_add_cb), NULL);

	table = etk_table_new(2,3,ETK_FALSE);
	
	label = etk_label_new("MIME-Type");
	etk_table_attach(ETK_TABLE(table), label,0,0,0,0,
				0,0,
				ETK_TABLE_HFILL | ETK_TABLE_VFILL | ETK_TABLE_HEXPAND);

	etk_mime_mime_entry = etk_entry_new();
	if (mime) {
		printf("Setting mime to '%s'...\n", mime);
		etk_entry_text_set(ETK_ENTRY(etk_mime_mime_entry), mime);
	}
	etk_table_attach(ETK_TABLE(table), etk_mime_mime_entry,1,1,0,0,
				0,0,
				ETK_TABLE_HFILL | ETK_TABLE_VFILL | ETK_TABLE_HEXPAND);

	label = etk_label_new("Description");
	etk_table_attach(ETK_TABLE(table), label,0,0,1,1,
				0,0,
				ETK_TABLE_HFILL | ETK_TABLE_VFILL | ETK_TABLE_HEXPAND);

	etk_mime_desc_entry = etk_entry_new();
	if (program) etk_entry_text_set(ETK_ENTRY(etk_mime_desc_entry), program);	
	etk_table_attach(ETK_TABLE(table), etk_mime_desc_entry,1,1,1,1,
				0,0,
				ETK_TABLE_HFILL | ETK_TABLE_VFILL | ETK_TABLE_HEXPAND);


	button = etk_button_new_with_label("OK");
	etk_table_attach(ETK_TABLE(table), button,0,0,2,2,
				0,0,
				ETK_TABLE_HFILL | ETK_TABLE_VFILL | ETK_TABLE_HEXPAND);
	etk_signal_connect("pressed", ETK_OBJECT(button), ETK_CALLBACK(_entropy_etk_mime_dialog_add_edit_final_cb), 
				NULL );

	button = etk_button_new_with_label("Cancel");
	etk_table_attach(ETK_TABLE(table), button,1,1,2,2,
				0,0,
				ETK_TABLE_HFILL | ETK_TABLE_VFILL | ETK_TABLE_HEXPAND);
	etk_signal_connect("pressed", ETK_OBJECT(button), ETK_CALLBACK(_entropy_etk_mime_dialog_add_edit_cancel_cb), 
				NULL );

	etk_container_add(ETK_CONTAINER(mime_dialog_add_edit_window), table);



	etk_widget_show_all(mime_dialog_add_edit_window);

        if (mime) {
                printf("Setting mime to '%s'...\n", mime);
                etk_entry_text_set(ETK_ENTRY(etk_mime_mime_entry), mime);
        }
        if (program) etk_entry_text_set(ETK_ENTRY(etk_mime_desc_entry), program);

}



void etk_mime_dialog_application_create(char* mime, Entropy_Config_Mime_Binding_Action* action) {
	Etk_Widget* table;
	Etk_Widget* label;
	Etk_Widget* button;
	Etk_Widget* window;

	if (mime_dialog_add_edit_application_window)
		return;

	mime_dialog_add_edit_application_window = etk_window_new();
	window = mime_dialog_add_edit_application_window;
	
	etk_window_title_set(ETK_WINDOW(window), "Program Associations");
	etk_window_wmclass_set(ETK_WINDOW(window), "mimedialog", "mimedialog");
	etk_widget_size_request_set(ETK_WIDGET(window), 250, 80);

	etk_signal_connect ("delete-event", ETK_OBJECT (window),
	      ETK_CALLBACK (_etk_window_deleted_add_app_cb), NULL);

	table = etk_table_new(2,4,ETK_FALSE);
	
	label = etk_label_new("Program Name (Description)");
	etk_table_attach(ETK_TABLE(table), label,0,0,0,0,
				0,0,
				ETK_TABLE_HFILL | ETK_TABLE_VFILL | ETK_TABLE_HEXPAND);

	etk_mime_app_name_entry = etk_entry_new();
	etk_table_attach(ETK_TABLE(table), etk_mime_app_name_entry,1,1,0,0,
				0,0,
				ETK_TABLE_HFILL | ETK_TABLE_VFILL | ETK_TABLE_HEXPAND);

	label = etk_label_new("Executable");
	etk_table_attach(ETK_TABLE(table), label,0,0,1,1,
				0,0,
				ETK_TABLE_HFILL | ETK_TABLE_VFILL | ETK_TABLE_HEXPAND);

	etk_mime_app_exe_entry = etk_entry_new();
	etk_table_attach(ETK_TABLE(table), etk_mime_app_exe_entry,1,1,1,1,
				0,0,
				ETK_TABLE_HFILL | ETK_TABLE_VFILL | ETK_TABLE_HEXPAND);


	label = etk_label_new("Arguments");
	etk_table_attach(ETK_TABLE(table), label,0,0,2,2,
				0,0,
				ETK_TABLE_HFILL | ETK_TABLE_VFILL | ETK_TABLE_HEXPAND);

	etk_mime_app_args_entry = etk_entry_new();
	etk_table_attach(ETK_TABLE(table), etk_mime_app_args_entry,1,1,2,2,
				0,0,
				ETK_TABLE_HFILL | ETK_TABLE_VFILL | ETK_TABLE_HEXPAND);



	button = etk_button_new_with_label("OK");
	etk_table_attach(ETK_TABLE(table), button,0,0,3,3,
				0,0,
				ETK_TABLE_HFILL | ETK_TABLE_VFILL | ETK_TABLE_HEXPAND);
	etk_signal_connect("pressed", ETK_OBJECT(button), 
			ETK_CALLBACK(_entropy_etk_mime_dialog_app_add_edit_final_cb), mime );

	button = etk_button_new_with_label("Cancel");
	etk_table_attach(ETK_TABLE(table), button,1,1,3,3,
				0,0,
				ETK_TABLE_HFILL | ETK_TABLE_VFILL | ETK_TABLE_HEXPAND);
	etk_signal_connect("pressed", ETK_OBJECT(button), 
			ETK_CALLBACK(_entropy_etk_mime_dialog_app_add_edit_cancel_cb), NULL );

	etk_container_add(ETK_CONTAINER(window), table);

	etk_widget_show_all(window);

	if (action) {
		etk_entry_text_set(ETK_ENTRY(etk_mime_app_name_entry), action->app_description);
		etk_entry_text_set(ETK_ENTRY(etk_mime_app_exe_entry), action->executable);
		etk_entry_text_set(ETK_ENTRY(etk_mime_app_args_entry), action->args);
	}

}

void etk_mime_dialog_create()
{
	Etk_Widget* tree;
	Etk_Widget* vbox = NULL;
	Etk_Widget* hbox = NULL;
	Etk_Tree_Col* tree_col;
	Etk_Widget* button;

	if (_etk_mime_dialog_displayed == 1)
		return;

	_etk_mime_dialog_displayed = 1;

	mime_dialog_window = etk_window_new();

	etk_window_title_set(ETK_WINDOW(mime_dialog_window), "Program Associations");
	etk_window_wmclass_set(ETK_WINDOW(mime_dialog_window), "mimedialog", "mimedialog");

	etk_signal_connect ("delete-event", ETK_OBJECT (mime_dialog_window),
	      ETK_CALLBACK (_etk_window_deleted_cb), NULL);

	etk_widget_size_request_set(ETK_WIDGET(mime_dialog_window), 450, 500);

	vbox = etk_vbox_new(ETK_FALSE,0);
	etk_container_add(ETK_CONTAINER(mime_dialog_window), vbox);

	hbox = etk_hbox_new(ETK_FALSE,0);
	etk_box_append(ETK_BOX(vbox), hbox, ETK_BOX_START, ETK_BOX_EXPAND_FILL, 0);


	/*Build the main tree*/
	tree = etk_tree_new();
	_etk_mime_dialog_main_tree = tree;
	
	etk_tree_mode_set(ETK_TREE(tree), ETK_TREE_MODE_LIST);
	
	tree_col = etk_tree_col_new(ETK_TREE(tree), _("Description"), 125, 0.0);
	etk_tree_col_model_add(tree_col, etk_tree_model_text_new());

	tree_col = etk_tree_col_new(ETK_TREE(tree), _("MIME Type"), 150,0.0);
	etk_tree_col_model_add(tree_col, etk_tree_model_text_new());
        
	etk_tree_col_expand_set(tree_col, ETK_TRUE);

	etk_tree_build(ETK_TREE(tree));
	etk_box_append(ETK_BOX(hbox), tree, ETK_BOX_START, ETK_BOX_EXPAND_FILL, 0);

	etk_signal_connect("row-clicked", ETK_OBJECT( tree  ), 
		  ETK_CALLBACK(_etk_mime_dialog_main_row_clicked), NULL);

	etk_mime_dialog_tree_populate();
	/*--------------------------*/


	hbox = etk_hbox_new(ETK_FALSE,0);
	etk_box_append(ETK_BOX(vbox), hbox, ETK_BOX_START, ETK_BOX_NONE, 0);

	button = etk_button_new_with_label("Add New Type");
	etk_box_append(ETK_BOX(hbox), button, ETK_BOX_START, ETK_BOX_NONE, 0);
	etk_signal_connect("pressed", ETK_OBJECT(button), ETK_CALLBACK(_entropy_etk_mime_dialog_add_cb), NULL);

	button = etk_button_new_with_label("Remove Selected Type");
	etk_box_append(ETK_BOX(hbox), button, ETK_BOX_START, ETK_BOX_NONE, 0);
	etk_signal_connect("pressed", ETK_OBJECT(button), ETK_CALLBACK(_entropy_etk_mime_dialog_remove_cb), NULL);


	/*button = etk_button_new_with_label("Add New..");
	etk_box_append(ETK_BOX(hbox), button, ETK_BOX_START, ETK_BOX_NONE, 0);*/
	


	/*Build the sub-tree*/
	tree = etk_tree_new();
	_etk_mime_dialog_sub_tree = tree;
	
	etk_tree_mode_set(ETK_TREE(tree), ETK_TREE_MODE_LIST);
	tree_col = etk_tree_col_new(ETK_TREE(tree), _("Description"), 125, 0.0);
	etk_tree_col_model_add(tree_col, etk_tree_model_text_new());

	tree_col = etk_tree_col_new(ETK_TREE(tree), _("Executable"),  150, 0.0);
	etk_tree_col_model_add(tree_col, etk_tree_model_text_new());

	tree_col = etk_tree_col_new(ETK_TREE(tree), _("Arguments"), 125,0.0);
	etk_tree_col_model_add(tree_col, etk_tree_model_text_new());

	
	etk_tree_build(ETK_TREE(tree));
	etk_box_append(ETK_BOX(vbox), tree, ETK_BOX_START, ETK_BOX_EXPAND_FILL, 0);

	/*Bottom button row*/
	hbox = etk_hbox_new(ETK_FALSE,0);
	etk_box_append(ETK_BOX(vbox), hbox, ETK_BOX_START, ETK_BOX_NONE, 0);

	button = etk_button_new_with_label("Add");
	etk_box_append(ETK_BOX(hbox), button, ETK_BOX_START, ETK_BOX_NONE, 0);
	etk_signal_connect("pressed", ETK_OBJECT(button), ETK_CALLBACK(_entropy_etk_mime_dialog_application_add_cb), NULL);
	//
	button = etk_button_new_with_label("Edit");
	etk_box_append(ETK_BOX(hbox), button, ETK_BOX_START, ETK_BOX_NONE, 0);
	etk_signal_connect("pressed", ETK_OBJECT(button), ETK_CALLBACK(_entropy_etk_mime_dialog_application_add_cb), (int*)1);

	button = etk_button_new_with_label("Remove");
	etk_box_append(ETK_BOX(hbox), button, ETK_BOX_START, ETK_BOX_NONE, 0);
	etk_signal_connect("pressed", ETK_OBJECT(button), ETK_CALLBACK(_entropy_etk_mime_dialog_remove_app_cb), NULL);
	

	etk_widget_show_all(mime_dialog_window);
}

