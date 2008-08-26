#include "entropy.h"
#include <Etk.h>
#include "entropy_etk_context_menu.h"
#include "etk_directory_add_dialog.h"
#include "etk_properties_dialog.h"
#include "etk_file_rename_dialog.h"

Etk_Widget* _entropy_etk_context_menu = NULL;
Etk_Widget* _entropy_etk_context_menu_open_with = NULL;
Etk_Widget* _entropy_etk_context_menu_open_with_item = NULL;

Etk_Widget* _entropy_etk_context_menu_groups = NULL;
Etk_Widget* _entropy_etk_context_menu_groups_item = NULL;

Etk_Widget* _entropy_etk_context_menu_groups_add_to = NULL;
Etk_Widget* _entropy_etk_context_menu_groups_add_to_item = NULL;


Etk_Widget* _entropy_etk_context_menu_groups_remove_from = NULL;
Etk_Widget* _entropy_etk_context_menu_groups_remove_from_item = NULL;

Etk_Widget* _entropy_etk_context_menu_rename_menu_item = NULL;
Etk_Widget* _entropy_etk_context_menu_view_menu_item = NULL;
Etk_Widget* _entropy_etk_context_menu_restore_trash_menu_item = NULL;

entropy_generic_file* _entropy_etk_context_menu_current_folder = NULL;
entropy_generic_file* _entropy_etk_context_menu_current_file = NULL;
Ecore_List* _entropy_etk_context_menu_selected_files = NULL;

entropy_gui_component_instance* _entropy_etk_context_menu_current_instance = NULL;

static int _entropy_etk_context_menu_mode = 0;


typedef enum _Etk_Menu_Item_Type
{
   ETK_MENU_ITEM_NORMAL,
   ETK_MENU_ITEM_SEPARATOR
} Etk_Menu_Item_Type;


static Etk_Widget *_entropy_etk_menu_item_new(Etk_Menu_Item_Type item_type, const char *label,
   Etk_Stock_Id stock_id, Etk_Menu_Shell *menu_shell, Etk_Widget *statusbar)
{
   Etk_Widget *menu_item = NULL;
   
   switch (item_type)
   {
      case ETK_MENU_ITEM_NORMAL:
         menu_item = etk_menu_item_image_new_with_label(label);
         break;
      case ETK_MENU_ITEM_SEPARATOR:
         menu_item = etk_menu_item_separator_new();
         break;
      default:
         return NULL;
   }
   if (stock_id != ETK_STOCK_NO_STOCK)
   {
      Etk_Widget *image;
      
      image = etk_image_new_from_stock(stock_id, ETK_STOCK_SMALL);
      etk_menu_item_image_set(ETK_MENU_ITEM_IMAGE(menu_item), ETK_IMAGE(image));
   }
   etk_menu_shell_append(menu_shell, ETK_MENU_ITEM(menu_item));
   
   /*etk_signal_connect("selected", ETK_OBJECT(menu_item), ETK_CALLBACK(_etk_test_menu_item_selected_cb), statusbar);
   etk_signal_connect("deselected", ETK_OBJECT(menu_item), ETK_CALLBACK(_etk_test_menu_item_deselected_cb), statusbar);*/
   
   return menu_item;
}


static void _entropy_etk_context_menu_properties_cb(Etk_Object *object, void *data)
{
	entropy_event_extended_stat_expect(_entropy_etk_context_menu_current_file, 
			_entropy_etk_context_menu_current_instance);

	
	if (_entropy_etk_context_menu_current_file) {
		entropy_event_stat_request(_entropy_etk_context_menu_current_file, 
			_entropy_etk_context_menu_current_instance);
	}
}


void _entropy_etk_context_menu_directory_add_cb(Etk_Object *object, void *data)
{
	if (_entropy_etk_context_menu_current_file) {
		if (!strcmp(_entropy_etk_context_menu_current_file->mime_type, "file/folder"))
			_entropy_etk_context_menu_current_folder = _entropy_etk_context_menu_current_file;
		else
			_entropy_etk_context_menu_current_folder = 
				entropy_core_parent_folder_file_get(_entropy_etk_context_menu_current_file);
	
		if (_entropy_etk_context_menu_current_folder) {
			etk_directory_add_dialog_create(_entropy_etk_context_menu_current_folder);
		} else {
			printf("Current folder is NULL at context menu\n");
		}
	}
}

static void
_open_with_item_cb(Etk_Object *obj, void *data)
{
	int i;
	entropy_gui_event* gui_event;
	
	i = (int)etk_object_data_get(obj, "INDEX");

	if (_entropy_etk_context_menu_current_file) {
		gui_event = entropy_malloc (sizeof (entropy_gui_event));
		gui_event->event_type =
			entropy_core_gui_event_get (ENTROPY_GUI_EVENT_ACTION_FILE);
		gui_event->data = _entropy_etk_context_menu_current_file;
		gui_event->key = i;
		entropy_core_layout_notify_event (_entropy_etk_context_menu_current_instance, 
			gui_event, ENTROPY_EVENT_GLOBAL);
	}
}

static void 
_open_folder_window_cb(Etk_Object* obj, void* data)
{
	entropy_gui_event* gui_event;
	
	if (_entropy_etk_context_menu_current_file) {
		printf("New folder handler...\n");
		
		gui_event = entropy_malloc (sizeof (entropy_gui_event));
		gui_event->event_type =
			entropy_core_gui_event_get (ENTROPY_GUI_EVENT_ACTION_FILE);
		gui_event->data = _entropy_etk_context_menu_current_file;
		gui_event->hints |= ENTROPY_GUI_EVENT_HINT_WINDOW_NEW;
		entropy_core_layout_notify_event (_entropy_etk_context_menu_current_instance,
			gui_event, ENTROPY_EVENT_GLOBAL);
	}
	
}

static void
_entropy_etk_context_menu_file_delete_cb(Etk_Object *object, void *data)
{
	if (_entropy_etk_context_menu_mode ==0) {
		entropy_plugin_filesystem_file_remove(_entropy_etk_context_menu_current_file,
			_entropy_etk_context_menu_current_instance);
	} else {
		entropy_generic_file* file;

		ecore_list_first_goto(_entropy_etk_context_menu_selected_files);
		while ( (file = ecore_list_next(_entropy_etk_context_menu_selected_files))) {
			entropy_plugin_filesystem_file_remove(file,
				_entropy_etk_context_menu_current_instance);			
		}		
	}
}

static void
_entropy_etk_context_menu_file_rename_cb(Etk_Object *object, void *data)
{
	etk_file_rename_dialog_create(_entropy_etk_context_menu_current_file);
}

static void
_entropy_etk_context_menu_file_view_cb(Etk_Object *obj, void *data)
{
	entropy_generic_file* file;

	file = _entropy_etk_context_menu_current_file;

	strcpy(file->mime_type, "text/plain");
	int i;
	entropy_gui_event* gui_event;
	
	i = (int)etk_object_data_get(obj, "INDEX");

	if (file) {
		gui_event = entropy_malloc (sizeof (entropy_gui_event));
		gui_event->event_type =
			entropy_core_gui_event_get (ENTROPY_GUI_EVENT_ACTION_FILE);
		gui_event->data = file;
		gui_event->key = i;
		entropy_core_layout_notify_event (_entropy_etk_context_menu_current_instance, 
			gui_event, ENTROPY_EVENT_GLOBAL);
	}
}

static void
_entropy_etk_context_menu_group_file_paste_cb(Etk_Object *object, void *data)
{
	Entropy_Selection_Type stype;
	Ecore_List* files = entropy_core_selected_files_get();

	stype = entropy_core_selection_type_get();
	if (_entropy_etk_context_menu_current_file) {
		if (!strcmp(_entropy_etk_context_menu_current_file->mime_type, "file/folder"))
			_entropy_etk_context_menu_current_folder = _entropy_etk_context_menu_current_file;
		else
			_entropy_etk_context_menu_current_folder = 
				entropy_core_parent_folder_file_get(_entropy_etk_context_menu_current_file);
	
		if (_entropy_etk_context_menu_current_folder) {
			char* f_uri = 	_entropy_etk_context_menu_current_folder->uri;
			if (f_uri) {
				if (stype == ENTROPY_SELECTION_COPY) {
					entropy_plugin_filesystem_file_copy_multi(files, f_uri, 
						_entropy_etk_context_menu_current_instance);
				} else if (stype == ENTROPY_SELECTION_CUT) {
					entropy_plugin_filesystem_file_move_multi(files, f_uri, 
						_entropy_etk_context_menu_current_instance);					
				} else {
					printf("Unsupported copy type at context menu paste\n");
				}
			}
		} else {
			printf("Current folder is NULL at context menu\n");
		}
	}
	
}

static void
_entropy_etk_context_menu_group_file_copy_cb(Etk_Object *object, void *data)
{
	entropy_core_selected_files_clear();

	entropy_core_selection_type_set(ENTROPY_SELECTION_COPY);
	if (_entropy_etk_context_menu_mode == 0) {
		entropy_core_selected_file_add(_entropy_etk_context_menu_current_file);
	} else {
		entropy_generic_file* file;

		ecore_list_first_goto(_entropy_etk_context_menu_selected_files);
		while ( (file = ecore_list_next(_entropy_etk_context_menu_selected_files))) {
			entropy_core_selected_file_add(file);
		}
		
	}
}

/*No code reuse here - we may want to use special behaviour*/
static void
_entropy_etk_context_menu_group_file_cut_cb(Etk_Object *object, void *data)
{
	entropy_core_selected_files_clear();

	entropy_core_selection_type_set(ENTROPY_SELECTION_CUT);
	if (_entropy_etk_context_menu_mode == 0) {
		entropy_core_selected_file_add(_entropy_etk_context_menu_current_file);
	} else {
		entropy_generic_file* file;

		ecore_list_first_goto(_entropy_etk_context_menu_selected_files);
		while ( (file = ecore_list_next(_entropy_etk_context_menu_selected_files))) {
			entropy_core_selected_file_add(file);
		}
		
	}
}

static void
_entropy_etk_context_menu_trash_copy_cb(Etk_Object *object, void *data)
{
	Ecore_List* files = ecore_list_new();

	if (_entropy_etk_context_menu_mode == 0) {
		ecore_list_append(files,_entropy_etk_context_menu_current_file);
	} else {
		entropy_generic_file* file;

		ecore_list_first_goto(_entropy_etk_context_menu_selected_files);
		while ( (file = ecore_list_next(_entropy_etk_context_menu_selected_files))) {
			ecore_list_append(files,file);
		}
		
	}

	if (ecore_list_count(files) > 0) {
		entropy_plugin_filesystem_file_move_multi(files, "trash:///", 
			_entropy_etk_context_menu_current_instance);	
	}

	

	ecore_list_destroy(files);
}

static void
_entropy_etk_context_menu_trash_restore_cb(Etk_Object *object, void *data)
{
	Ecore_List* files;

	files = ecore_list_new();
	ecore_list_append(files, _entropy_etk_context_menu_current_file);
	
	entropy_plugin_filesystem_file_trash_restore(
		files, _entropy_etk_context_menu_current_instance);

	ecore_list_destroy(files);
}

static void
_entropy_etk_context_menu_group_file_add_remove_cb(Etk_Object *object, void *data)
{
	const char* label;

	int mode = (int)data;

	label = etk_menu_item_label_get(ETK_MENU_ITEM(object));
	printf("Add to group '%s'\n", label);

	/*Check for multi, or single, selection*/
	if (_entropy_etk_context_menu_mode == 0) {
		if (!mode) 
			entropy_plugin_filesystem_file_group_add(_entropy_etk_context_menu_current_file, (char*)label);
		else
			entropy_plugin_filesystem_file_group_remove(_entropy_etk_context_menu_current_file, (char*)label);
	} else {
		entropy_generic_file* file;

		ecore_list_first_goto(_entropy_etk_context_menu_selected_files);
		while ( (file = ecore_list_next(_entropy_etk_context_menu_selected_files))) {
			if (!mode) 
				entropy_plugin_filesystem_file_group_add(file, (char*)label);
			else
				entropy_plugin_filesystem_file_group_remove(file, (char*)label);
		}
	}
}

void
entropy_etk_context_menu_metadata_groups_populate()
{
	Etk_Widget* w;
	Ecore_List* l;
	char* str;

	if (!_entropy_etk_context_menu) 
		entropy_etk_context_menu_init();

	/*Empty groups menus*/
	if (ETK_MENU_ITEM(_entropy_etk_context_menu_groups_add_to_item)->submenu) {
	   etk_menu_item_submenu_set(ETK_MENU_ITEM(_entropy_etk_context_menu_groups_add_to_item), NULL);
	   etk_object_destroy(ETK_OBJECT(_entropy_etk_context_menu_groups_add_to));
	   _entropy_etk_context_menu_groups_add_to = NULL;
        }

       _entropy_etk_context_menu_groups_add_to = etk_menu_new();
       etk_menu_item_submenu_set(ETK_MENU_ITEM(_entropy_etk_context_menu_groups_add_to_item), 
             ETK_MENU(_entropy_etk_context_menu_groups_add_to)); 
       /*---*/

	/*Empty groups menus*/
        if (ETK_MENU_ITEM(_entropy_etk_context_menu_groups_remove_from_item)->submenu) {
           etk_menu_item_submenu_set(ETK_MENU_ITEM(_entropy_etk_context_menu_groups_remove_from_item), NULL);
           etk_object_destroy(ETK_OBJECT(_entropy_etk_context_menu_groups_remove_from));
           _entropy_etk_context_menu_groups_remove_from = NULL;
        }

       _entropy_etk_context_menu_groups_remove_from = etk_menu_new();
       etk_menu_item_submenu_set(ETK_MENU_ITEM(_entropy_etk_context_menu_groups_remove_from_item),
             ETK_MENU(_entropy_etk_context_menu_groups_remove_from));
       /*---*/
       

       l = entropy_plugin_filesystem_metadata_groups_retrieve();
       ecore_list_first_goto(l);
       while ( (str = ecore_list_next(l))) {
	       printf("Adding %s\n", str);

	       w = _entropy_etk_menu_item_new(ETK_MENU_ITEM_NORMAL, str, 
		    ETK_STOCK_EDIT_COPY, ETK_MENU_SHELL(_entropy_etk_context_menu_groups_add_to),NULL);
	      etk_signal_connect("activated", ETK_OBJECT(w), ETK_CALLBACK(_entropy_etk_context_menu_group_file_add_remove_cb), (int*)0);

	       w = _entropy_etk_menu_item_new(ETK_MENU_ITEM_NORMAL, str, 
		    ETK_STOCK_EDIT_COPY, ETK_MENU_SHELL(_entropy_etk_context_menu_groups_remove_from),NULL);
		etk_signal_connect("activated", ETK_OBJECT(w), ETK_CALLBACK(_entropy_etk_context_menu_group_file_add_remove_cb), (int*)1);


       }

}


static void
_entropy_etk_context_menu_popup_cb(Etk_Object *object, void *data)
{
	Entropy_Config_Mime_Binding* binding;
	Entropy_Config_Mime_Binding_Action* action;
	Evas_List* l;
	entropy_generic_file* file;

	file = _entropy_etk_context_menu_current_file;

	if (file && strlen(file->mime_type)) {
		
		   binding = entropy_config_mime_binding_for_type_get(file->mime_type);

		   if (ETK_MENU_ITEM(_entropy_etk_context_menu_open_with_item)->submenu) {
			   etk_menu_item_submenu_set(ETK_MENU_ITEM(_entropy_etk_context_menu_open_with_item), NULL);
			   etk_object_destroy(ETK_OBJECT(_entropy_etk_context_menu_open_with));
			   _entropy_etk_context_menu_open_with = NULL;
		   }

		   /*If it's a folder, add an 'Open in new layout..' entry*/
		   if (!strcmp(file->mime_type, "file/folder")) {
			   Etk_Widget* w;
			   
			   _entropy_etk_context_menu_open_with = etk_menu_new();
			   etk_menu_item_submenu_set(ETK_MENU_ITEM(_entropy_etk_context_menu_open_with_item), 
					  ETK_MENU(_entropy_etk_context_menu_open_with)); 

			   w = _entropy_etk_menu_item_new(ETK_MENU_ITEM_NORMAL, "Open in new window", 
				  ETK_STOCK_EDIT_COPY, ETK_MENU_SHELL(_entropy_etk_context_menu_open_with),NULL);
			   etk_signal_connect("activated", ETK_OBJECT(w), ETK_CALLBACK(_open_folder_window_cb), NULL);
		   }

	
		   if (binding) {
			   Etk_Widget* w; 
			   int i=0;
		   
			   if (!_entropy_etk_context_menu_open_with) {
				   _entropy_etk_context_menu_open_with = etk_menu_new();
				   etk_menu_item_submenu_set(ETK_MENU_ITEM(_entropy_etk_context_menu_open_with_item), 
						  ETK_MENU(_entropy_etk_context_menu_open_with)); 
			   }

			   for (l = binding->actions; l; ) {
				   action = l->data;

				   w = _entropy_etk_menu_item_new(ETK_MENU_ITEM_NORMAL, _(action->app_description),
					ETK_STOCK_EDIT_COPY, ETK_MENU_SHELL(_entropy_etk_context_menu_open_with),NULL);
				   etk_object_data_set(ETK_OBJECT(w), "INDEX", (int*)i);

				   etk_signal_connect("activated", ETK_OBJECT(w), ETK_CALLBACK(_open_with_item_cb), NULL);
				   
				   l = l->next;
				   i++;
			   }
		  }
	}
	
}


void entropy_etk_context_menu_init()
{
	Etk_Widget* menu_item;
	Etk_Widget* menu;
	Etk_Widget* new_menu;
   
	
	
   	if (!_entropy_etk_context_menu) {
		menu = etk_menu_new();
		_entropy_etk_context_menu = menu;
		
		etk_signal_connect("popped-up", ETK_OBJECT(menu), ETK_CALLBACK(_entropy_etk_context_menu_popup_cb), NULL);

		_entropy_etk_context_menu_open_with_item =  
		   _entropy_etk_menu_item_new(ETK_MENU_ITEM_NORMAL,
		  _("Open With"), ETK_STOCK_APPLICATION_X_EXECUTABLE, ETK_MENU_SHELL(menu),NULL);
		
		_entropy_etk_context_menu_open_with = etk_menu_new();
		etk_menu_item_submenu_set(ETK_MENU_ITEM(_entropy_etk_context_menu_open_with_item),
		ETK_MENU(_entropy_etk_context_menu_open_with)); 

		_entropy_etk_context_menu_groups_item =  
		   _entropy_etk_menu_item_new(ETK_MENU_ITEM_NORMAL, _("Groups"), 
		ETK_STOCK_EMBLEM_IMPORTANT, ETK_MENU_SHELL(menu),NULL);

		_entropy_etk_context_menu_groups = etk_menu_new();
		etk_menu_item_submenu_set(ETK_MENU_ITEM(_entropy_etk_context_menu_groups_item), 
		ETK_MENU(_entropy_etk_context_menu_groups)); 

		_entropy_etk_context_menu_groups_add_to_item =  
		   _entropy_etk_menu_item_new(ETK_MENU_ITEM_NORMAL, _("Add to.."), ETK_STOCK_DIALOG_APPLY, 
				   ETK_MENU_SHELL(_entropy_etk_context_menu_groups),NULL);
		
		_entropy_etk_context_menu_groups_add_to = etk_menu_new();
		etk_menu_item_submenu_set(ETK_MENU_ITEM(_entropy_etk_context_menu_groups_add_to_item), 
				ETK_MENU(_entropy_etk_context_menu_groups_add_to)); 

                _entropy_etk_context_menu_groups_remove_from_item =
                   _entropy_etk_menu_item_new(ETK_MENU_ITEM_NORMAL, _("Remove from.."), ETK_STOCK_DIALOG_CLOSE, 
                                   ETK_MENU_SHELL(_entropy_etk_context_menu_groups),NULL);
                _entropy_etk_context_menu_groups_remove_from = etk_menu_new();
                etk_menu_item_submenu_set(ETK_MENU_ITEM(_entropy_etk_context_menu_groups_remove_from_item),
                                ETK_MENU(_entropy_etk_context_menu_groups_remove_from));


		menu_item =
		   _entropy_etk_menu_item_new(ETK_MENU_ITEM_NORMAL, _("Copy (Ctrl-c)"),
		ETK_STOCK_EDIT_COPY, ETK_MENU_SHELL(menu),NULL);
		
		etk_signal_connect("activated", ETK_OBJECT(menu_item),
		ETK_CALLBACK(_entropy_etk_context_menu_group_file_copy_cb),NULL);
		
		menu_item =
		_entropy_etk_menu_item_new(ETK_MENU_ITEM_NORMAL, _("Cut (Ctrl-x)"), 
		ETK_STOCK_EDIT_CUT, ETK_MENU_SHELL(menu),NULL);
		etk_signal_connect("activated", ETK_OBJECT(menu_item),
		ETK_CALLBACK(_entropy_etk_context_menu_group_file_cut_cb),NULL);

		
		menu_item =
		   _entropy_etk_menu_item_new(ETK_MENU_ITEM_NORMAL, _("Paste (Ctrl-v)"), 
		ETK_STOCK_EDIT_PASTE, ETK_MENU_SHELL(menu),NULL);
		etk_signal_connect("activated", ETK_OBJECT(menu_item), 
		ETK_CALLBACK(_entropy_etk_context_menu_group_file_paste_cb),NULL);

		/*TEST: TRASH*/
		menu_item = _entropy_etk_menu_item_new(ETK_MENU_ITEM_NORMAL, _("Send to Trash"), ETK_STOCK_USER_TRASH_FULL, ETK_MENU_SHELL(menu),NULL);
		etk_signal_connect("activated", ETK_OBJECT(menu_item), 
		ETK_CALLBACK(_entropy_etk_context_menu_trash_copy_cb), NULL);
		
		menu_item = _entropy_etk_menu_item_new(ETK_MENU_ITEM_NORMAL, _("Delete (Del)"),
		ETK_STOCK_EDIT_DELETE, ETK_MENU_SHELL(menu),NULL);
		etk_signal_connect("activated", ETK_OBJECT(menu_item), 
		ETK_CALLBACK(_entropy_etk_context_menu_file_delete_cb), NULL);
		
		_entropy_etk_context_menu_rename_menu_item = 
			_entropy_etk_menu_item_new(ETK_MENU_ITEM_NORMAL, _("Rename"), 
		ETK_STOCK_EDIT_FIND_REPLACE, ETK_MENU_SHELL(menu),NULL);
		
		etk_signal_connect("activated", ETK_OBJECT(_entropy_etk_context_menu_rename_menu_item), 
				ETK_CALLBACK(_entropy_etk_context_menu_file_rename_cb), NULL);

		_entropy_etk_context_menu_view_menu_item = 
			_entropy_etk_menu_item_new(ETK_MENU_ITEM_NORMAL, _("View/Edit file"), 
		ETK_STOCK_ACCESSORIES_TEXT_EDITOR, ETK_MENU_SHELL(menu),NULL);

		etk_signal_connect("activated", ETK_OBJECT(_entropy_etk_context_menu_view_menu_item), 
				ETK_CALLBACK(_entropy_etk_context_menu_file_view_cb), NULL);

		menu_item = _entropy_etk_menu_item_new(ETK_MENU_ITEM_NORMAL, 
		_("Properties"), ETK_STOCK_DOCUMENT_PROPERTIES, ETK_MENU_SHELL(menu),NULL);
		etk_signal_connect("activated", ETK_OBJECT(menu_item), ETK_CALLBACK(_entropy_etk_context_menu_properties_cb), NULL);

		menu_item =  _entropy_etk_menu_item_new(ETK_MENU_ITEM_NORMAL, _("New"), 
		ETK_STOCK_DOCUMENT_NEW, ETK_MENU_SHELL(menu),NULL);
		new_menu = etk_menu_new();
		etk_menu_item_submenu_set(ETK_MENU_ITEM(menu_item), ETK_MENU(new_menu));

		menu_item =  _entropy_etk_menu_item_new(ETK_MENU_ITEM_NORMAL, 
		_("Folder.."), ETK_STOCK_FOLDER_NEW, ETK_MENU_SHELL(new_menu),NULL);
		etk_signal_connect("activated", ETK_OBJECT(menu_item), ETK_CALLBACK(_entropy_etk_context_menu_directory_add_cb), NULL);

		
		/*Trash restore*/
		_entropy_etk_context_menu_restore_trash_menu_item = 
		    _entropy_etk_menu_item_new(ETK_MENU_ITEM_NORMAL, _("Restore from Trash"), 
		   ETK_STOCK_PLACES_USER_TRASH , ETK_MENU_SHELL(menu),NULL);
		
		etk_signal_connect("activated", ETK_OBJECT(_entropy_etk_context_menu_restore_trash_menu_item), 
				ETK_CALLBACK(_entropy_etk_context_menu_trash_restore_cb), NULL);


	}

	entropy_etk_context_menu_metadata_groups_populate();

}

void entropy_etk_context_menu_popup(entropy_gui_component_instance* instance, entropy_generic_file* current_file)
{
        _entropy_etk_context_menu_current_file = current_file;
	_entropy_etk_context_menu_current_instance = instance;

	entropy_generic_file* file;
	file = _entropy_etk_context_menu_current_file;
	
	if (!_entropy_etk_context_menu) 
		entropy_etk_context_menu_init();

	/*Show*/
	etk_widget_show_all(_entropy_etk_context_menu_open_with_item);
	etk_widget_show_all(_entropy_etk_context_menu_rename_menu_item);
	if (file && (strcmp(file->mime_type, "file/folder")))
		etk_widget_show_all(_entropy_etk_context_menu_view_menu_item);
	else
		etk_widget_hide(_entropy_etk_context_menu_view_menu_item);

	if (!strcmp(current_file->uri_base, "trash"))
  	    etk_widget_show_all(_entropy_etk_context_menu_restore_trash_menu_item);
	else
	    etk_widget_hide(_entropy_etk_context_menu_restore_trash_menu_item);

	/*Mode = single*/
	_entropy_etk_context_menu_mode = 0;	

	etk_menu_popup(ETK_MENU(_entropy_etk_context_menu));
}

void entropy_etk_context_menu_popup_multi(entropy_gui_component_instance* instance, Ecore_List* current_files)
{
	_entropy_etk_context_menu_current_instance = instance;
	entropy_generic_file* file;


	/*Hide*/
	etk_widget_hide(_entropy_etk_context_menu_open_with_item);
	etk_widget_hide(_entropy_etk_context_menu_rename_menu_item);
	etk_widget_hide(_entropy_etk_context_menu_view_menu_item);
	etk_widget_hide(_entropy_etk_context_menu_restore_trash_menu_item);

	if (_entropy_etk_context_menu_selected_files)
		ecore_list_destroy(_entropy_etk_context_menu_selected_files);
	_entropy_etk_context_menu_selected_files = NULL;

	_entropy_etk_context_menu_selected_files = ecore_list_new();
	ecore_list_first_goto(current_files);
	while ( (file = ecore_list_next(current_files))) {
		ecore_list_append(_entropy_etk_context_menu_selected_files, file);
	}
	
	if (!_entropy_etk_context_menu) 
		entropy_etk_context_menu_init();

	/*Mode = multi*/
	_entropy_etk_context_menu_mode = 1;

	etk_menu_popup(ETK_MENU(_entropy_etk_context_menu));
}
