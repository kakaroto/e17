#include <Etk.h>
#include "entropy.h"
#include "entropy_gui.h"
#include "etk_location_add_dialog.h"
#include "etk_progress_dialog.h"
#include "etk_properties_dialog.h"
#include "etk_user_interaction_dialog.h"
#include "entropy_etk_options_dialog.h"
#include "etk_mime_dialog.h"
#include "etk_file_cache_dialog.h"
#include "entropy_etk_context_menu.h"
#include "entropy_etk_options_dialog.h"
#include "etk_auth_request_dialog.h"
#include "etk_efolder_dialog.h"
#include "etk_about_dialog.h"
#include <dlfcn.h>
#include <Ecore.h>
#include <stdlib.h>

#define ENTROPY_ETK_WINDOW_WIDTH 640
#define ENTROPY_ETK_WINDOW_HEIGHT 480
#define ENTROPY_ETK_WINDOW_PANE_DEFAULT_X 150


//This is here mostly as a demo of the "widget library independence" of entropy, but is on hold
//for the moment until ETK supports more widgets that we need
//

static int _etk_layout_window_count = 0;
static Ecore_Hash* _etk_layout_row_reference;
static Ecore_Hash* _etk_layout_structure_plugin_reference;
static int _etk_layout_global_init = 0;

typedef struct _layout_etk_row_structure_plugin _layout_etk_row_structure_plugin;
struct _layout_etk_row_structure_plugin {
	Etk_Tree_Row* row;
	entropy_plugin* structure_plugin;
}; 

typedef struct entropy_layout_gui entropy_layout_gui;
struct entropy_layout_gui
{
  entropy_gui_component_instance *iconbox_viewer;
  entropy_gui_component_instance *list_viewer;
  entropy_gui_component_instance *structure_viewer;
  entropy_gui_component_instance *trackback;
  
  Etk_Widget *tree;
  Etk_Widget *paned;
  Etk_Widget *statusbar_box;
  Etk_Widget *statusbars[3];
  Etk_Tree_Row* delete_row; /*The row pending deletion, if any*/

  Etk_Widget* popup;
  Etk_Widget* localshell;
  Etk_Widget* trackback_shell;
  Etk_Widget* tree_shell;
  Ecore_Hash* progress_hash; /*Track progress events->dialogs*/

  Etk_Widget* tree_view_menu;

  Ecore_Hash* properties_request_hash; 

};

typedef enum _Etk_Menu_Item_Type
{
   ETK_MENU_ITEM_NORMAL,
   ETK_MENU_ITEM_SEPARATOR
} Etk_Menu_Item_Type;

void layout_etk_simple_add_header(entropy_gui_component_instance* instance, Entropy_Config_Structure* structure);
void entropy_plugin_layout_main ();
char* entropy_plugin_toolkit_get();
entropy_gui_component_instance* entropy_plugin_layout_create (entropy_core * core);
void entropy_etk_layout_trackback_cb(Etk_Object* obj, void* data);
void entropy_etk_layout_trackback_show(entropy_layout_gui* gui, int visible);
void entropy_etk_layout_tree_show(entropy_layout_gui* gui, int visible);
void entropy_layout_etk_simple_local_view_set(entropy_gui_component_instance* instance,
		entropy_gui_component_instance* local);

static void _entropy_layout_etk_cut_cb(Etk_Object* object, void* data);
static void _entropy_layout_etk_copy_cb(Etk_Object* object, void* data);
static void _entropy_layout_etk_paste_cb(Etk_Object* object, void* data);



void layout_etk_simple_quit(entropy_core* core)
{
  entropy_core_destroy (core);
  etk_main_quit ();
}

static Etk_Bool
_etk_window_deleted_cb (Etk_Object * object, void *data)
{
  entropy_gui_component_instance* instance = data;
	
  entropy_core_component_event_deregister (instance,
					 entropy_core_gui_event_get
					 (ENTROPY_GUI_EVENT_FILE_PROGRESS));

  entropy_core_component_event_deregister (instance,
					 entropy_core_gui_event_get
					 (ENTROPY_GUI_EVENT_USER_INTERACTION_YES_NO_ABORT));

  entropy_core_component_event_deregister (instance,
					 entropy_core_gui_event_get
					 (ENTROPY_GUI_EVENT_EXTENDED_STAT));

  entropy_core_component_event_deregister (instance,
					 entropy_core_gui_event_get
					 (ENTROPY_GUI_EVENT_FILE_STAT));
  entropy_core_component_event_deregister (instance,
					 entropy_core_gui_event_get
					 (ENTROPY_GUI_EVENT_FILE_STAT_AVAILABLE));

  entropy_core_component_event_deregister (instance,
					 entropy_core_gui_event_get
					 (ENTROPY_GUI_EVENT_METADATA_GROUPS));

  entropy_core_component_event_deregister (instance,
					 entropy_core_gui_event_get
					 (ENTROPY_GUI_EVENT_PASTE_REQUEST));

  entropy_core_component_event_deregister (instance,
					 entropy_core_gui_event_get
					 (ENTROPY_GUI_EVENT_AUTH_REQUEST));

  entropy_core_component_event_deregister (instance,
					 entropy_core_gui_event_get
					 (ENTROPY_GUI_EVENT_META_ALL_REQUEST));

  /*Deregister this layout*/
  entropy_core_layout_deregister(instance->core, instance);


  /*TODO: Destroy plugin children, and layout*/

  /*Decrement window reference counter*/
  _etk_layout_window_count--;
  
  etk_object_destroy(ETK_OBJECT(instance->gui_object));
  
  if (_etk_layout_window_count == 0) {
	  layout_etk_simple_quit(instance->core);
  }

  return ETK_TRUE;
}

void
entropy_plugin_destroy (entropy_gui_component_instance * comp)
{
  printf ("Destroying layout_etk...\n");


}


/*Function to make standard menu items*/
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

/*Function to make checkbox menu items*/
static Etk_Widget *_entropy_etk_menu_check_item_new(const char *label, Etk_Menu_Shell *menu_shell)
{
   Etk_Widget *menu_item;
   
   if (!menu_shell)
      return NULL;
   
   menu_item = etk_menu_item_check_new_with_label(label);
   etk_menu_shell_append(menu_shell, ETK_MENU_ITEM(menu_item));
   
   return menu_item;
}

/*Function to make radio items*/
static Etk_Widget *_entropy_etk_radio_item_new(const char *label, Etk_Menu_Item_Radio *group_item, Etk_Menu_Shell *menu_shell)
{
   Etk_Widget *menu_item;
   
   if (!menu_shell)
      return NULL;
   
   menu_item = etk_menu_item_radio_new_with_label_from_widget(label, group_item);
   etk_menu_shell_append(menu_shell, ETK_MENU_ITEM(menu_item));
   
   return menu_item;
}



/*-----*/
/*Cut/Copy/Paste Callbacks*/
static void _entropy_layout_etk_cut_cb(Etk_Object* object, void* data)
{
	printf("Cut request at layout..\n");
	entropy_event_cut_request(data);
}

static void _entropy_layout_etk_copy_cb(Etk_Object* object, void* data)
{
	printf("Copy request at layout..\n");
	entropy_event_copy_request(data);
}

static void _entropy_layout_etk_paste_cb(Etk_Object* object, void* data)
{
	printf("Paste request at layout..\n");
	entropy_event_paste_request(data);
}

/*-----*/
/*Location related functions*/
static void _etk_layout_location_delete_confirm_cb(Etk_Object * object, void *data)
{
	entropy_gui_component_instance* instance = data;
	entropy_layout_gui* gui = instance->data;	
	Etk_Tree_Row* row = gui->delete_row;
	Entropy_Config_Structure* structure;
	Ecore_List* row_refs = NULL;
        _layout_etk_row_structure_plugin* ref;

	if (row) {
		structure = ecore_hash_get(_etk_layout_row_reference, row);
		row_refs = ecore_hash_get(_etk_layout_structure_plugin_reference, structure);
		if (row_refs) {	
			while ( (ref = ecore_list_first_remove(row_refs))) {
				etk_tree_row_delete(ref->row);
				IF_FREE(ref);
			}
			ecore_list_destroy(row_refs);
			ecore_hash_remove(_etk_layout_structure_plugin_reference, structure);
		}
		entropy_config_standard_structure_remove(structure);
	}
}

static void _etk_layout_row_clicked(Etk_Object *object, 
		Etk_Tree_Row *row, Etk_Event_Mouse_Down *event, void *data)
{
	entropy_gui_component_instance* instance = data;
	entropy_layout_gui* gui = instance->data;	
	Entropy_Config_Structure* structure;

	structure = ecore_hash_get(_etk_layout_row_reference, row);

	if (event->button == 3 && structure) {
		gui->delete_row = row;
		
		etk_tree_row_select(row);
		etk_menu_popup(ETK_MENU(gui->popup));
	}
	
}

void layout_etk_simple_add_header(entropy_gui_component_instance* instance, Entropy_Config_Structure* structure_obj)
{
  void *(*structure_plugin_init) (entropy_core * core,
				  entropy_gui_component_instance *,
				  void* parent_visual,
				  void *data);

  entropy_plugin *structure;
  entropy_generic_file* file;
  Etk_Tree_Row* row;
  Etk_Tree_Col* col;
  entropy_layout_gui* gui = instance->data;
  char* icon_string = NULL;
  Ecore_List* layouts;
  _layout_etk_row_structure_plugin* struct_ref = NULL;

  col = etk_tree_nth_col_get(ETK_TREE(gui->tree), 0);

  /*Parse the file from the URI*/
   file = entropy_core_parse_uri (structure_obj->uri);

   printf("Object for %s/%s is %p....\n", file->path, file->filename, file);
   

   /*This will be moved to a central function. TODO*/
   if (!strcmp(file->uri_base, "file"))
	   icon_string = PACKAGE_DATA_DIR "/icons/local-system.png";
  else if (!strcmp(file->uri_base, "smb"))
	  icon_string = PACKAGE_DATA_DIR "/icons/samba-system.png";
   else if (!strcmp(file->uri_base,"sftp"))
	  icon_string = PACKAGE_DATA_DIR "/icons/sftp-system.png"; 
   else if (!strcmp(file->uri_base,"vfolder"))
	  icon_string = PACKAGE_DATA_DIR "/icons/vfolder-system.png"; 
			   
	
  etk_tree_freeze(ETK_TREE(gui->tree));
  row = etk_tree_row_append(ETK_TREE(gui->tree), NULL, col,
			  icon_string, NULL, structure_obj->name, NULL);
  etk_tree_thaw(ETK_TREE(gui->tree));
  
  
  structure = entropy_plugins_type_get_first(ENTROPY_PLUGIN_GUI_COMPONENT,ENTROPY_PLUGIN_GUI_COMPONENT_STRUCTURE_VIEW);
   structure_plugin_init =
      dlsym (structure->dl_ref, "entropy_plugin_gui_instance_new");

   /*We shouldn't really assume it's a folder - but it bootstraps us for
    * now- FIXME*/
   strcpy(file->mime_type, "file/folder");
   file->filetype = FILE_FOLDER;

  if (!strlen (file->mime_type)) {
	    entropy_mime_file_identify (file);
  }
   
   instance = (*structure_plugin_init)(instance->core, instance, row,file);
   instance->plugin = structure;

   /*Add to tracker*/
  ecore_hash_set(_etk_layout_row_reference, row, structure_obj);

  printf ("LOADED: %s/%s\n", file->path, file->filename);
  
  /*Add to layout/plugin tracker - this is to destroy if the user removes a location*/
  if (! (layouts = ecore_hash_get(_etk_layout_structure_plugin_reference, structure_obj))) {
	  layouts = ecore_list_new();
	  ecore_hash_set(_etk_layout_structure_plugin_reference, structure_obj, layouts);
  }

  struct_ref = entropy_malloc(sizeof(_layout_etk_row_structure_plugin));
  struct_ref->row = row;
  struct_ref->structure_plugin = structure;

  ecore_list_append(layouts, struct_ref);

}
/*----------*/


void etk_layout_simple_exit_cb(Etk_Object* obj, void* data)
{
	entropy_gui_component_instance* instance = data;

	layout_etk_simple_quit(instance->core);
}

void entropy_etk_about_dialog_cb(Etk_Object* cb, void* data)
{
	entropy_etk_about_dialog_show();
}


void etk_mime_dialog_cb(Etk_Object* obj, void* data)
{
	etk_mime_dialog_create();
}

void entropy_etk_options_dialog_cb(Etk_Object* obj, void* data)
{
	entropy_etk_options_dialog_show();
}

/*---*/
/*Trackback plugin related functions*/
void entropy_etk_layout_trackback_cb(Etk_Object* obj, void* data)
{
	entropy_gui_component_instance* instance = data;
	entropy_layout_gui* gui = instance->data;

	if (etk_container_is_child(ETK_CONTAINER(gui->trackback_shell), gui->trackback->gui_object) == ETK_FALSE) {
		entropy_etk_layout_trackback_show(gui,1);
	} else {
		entropy_etk_layout_trackback_show(gui,0);
	}
}

void entropy_etk_layout_trackback_show(entropy_layout_gui* gui, int visible)
{
	if (visible) {
		etk_box_append(ETK_BOX(gui->trackback_shell), gui->trackback->gui_object, ETK_BOX_START, ETK_BOX_NONE,0);
		etk_widget_show_all(ETK_WIDGET(gui->trackback->gui_object));	
	} else {
		etk_container_remove(ETK_WIDGET(gui->trackback->gui_object));
	}
}
/*--------*/

/*------*/
/*Tree related functions*/
void entropy_etk_layout_tree_cb(Etk_Object* obj, void* data)
{
	entropy_gui_component_instance* instance = data;
	entropy_layout_gui* gui = instance->data;

	if (!etk_widget_is_visible(gui->tree)) {
		entropy_etk_layout_tree_show(gui,1);
	} else {
		entropy_etk_layout_tree_show(gui,0);
	}
}

void entropy_etk_layout_tree_show(entropy_layout_gui* gui, int visible)
{
	if (visible) {
		etk_box_append(ETK_BOX(gui->tree_shell), gui->tree, ETK_BOX_START, ETK_BOX_EXPAND_FILL, 0);
		etk_widget_show_all(gui->tree);
		etk_paned_position_set(ETK_PANED(gui->paned), ENTROPY_ETK_WINDOW_PANE_DEFAULT_X);
	} else {
		etk_widget_hide(gui->tree);
		etk_container_remove(ETK_WIDGET(gui->tree));
		etk_paned_position_set(ETK_PANED(gui->paned), 0);
	}
}
/*----------*/

void etk_local_viewer_cb(Etk_Object* obj, void* data)
{
	entropy_gui_component_instance* local;
	entropy_gui_component_instance* instance = data;
	
	/*Get the local viewer they want..*/
	local = etk_object_data_get(ETK_OBJECT(obj), "VISUAL");

	if (local) {
		entropy_layout_etk_simple_local_view_set(instance, local);
	} else {
		printf("Local is null!\n");
	}
}

void entropy_layout_etk_simple_local_view_set(entropy_gui_component_instance* instance,
		entropy_gui_component_instance* local)
{
	Eina_List* children;
	entropy_layout_gui* gui = instance->data;
	Etk_Widget* widget;
	entropy_generic_file* file;

	/*FIXME - disable plugins - this should be cleaner*/
	gui->iconbox_viewer->active = 0;
	gui->list_viewer->active = 0;

	for (children = etk_container_children_get(ETK_CONTAINER(gui->localshell)); children; ) {
		widget = children->data;
		etk_container_remove(ETK_WIDGET(widget));
			
		children = children->next;
	}

	if (local->gui_object) {
		local->active = 1;
		
		/*Update the visual current_folder*/
		if ( (file = ((entropy_gui_component_instance_layout*)instance)->current_folder) ) {
			entropy_event_action_file(file, instance);
		} else {
			printf("No current folder!\n");
		}
		etk_box_append(ETK_BOX(gui->localshell), local->gui_object, ETK_BOX_START, ETK_BOX_EXPAND_FILL, 0);
	} else {
		printf("Selected instance has no GUI_OBJECT\n");
	}	
}


void etk_file_cache_dialog_cb(Etk_Object *obj, void *data)
{
	etk_file_cache_dialog_create();
}

void _location_add_cb(Etk_Object *obj, void *data)
{
	printf("Add location\n");
	etk_location_add_dialog_create((entropy_gui_component_instance*)data, layout_etk_simple_add_header);
}

void _entropy_etk_efolder_dialog_show_cb(Etk_Object *obj, void *data)
{
	entropy_gui_component_instance* layout = (entropy_gui_component_instance*)data;
	entropy_plugin_filesystem_metadata_all_get(layout);
}


void
_entropy_etk_layout_key_down_cb(Etk_Object *object, void *event, void *data)
{
   Etk_Event_Key_Down *ev;
   entropy_gui_component_instance* instance = data;
   entropy_layout_gui* gui = instance->data;

   ev = event;

   if ((ev->modifiers & ETK_MODIFIER_CTRL))
   {
	   if (!strcmp(ev->key, "q")) {
		   layout_etk_simple_quit(instance->core);
	   } else if (!strcmp(ev->key, "c")) {
		 _entropy_layout_etk_copy_cb(NULL, instance); 
	   } else if (!strcmp(ev->key, "x")) {
		   _entropy_layout_etk_cut_cb(NULL, instance);
	   } else if (!strcmp(ev->key, "v")) {
		_entropy_layout_etk_paste_cb(NULL, instance);
	   }
   } else if ((ev->modifiers & ETK_MODIFIER_ALT)) {
 	   if (!strcmp(ev->key, "i")) {
		entropy_layout_etk_simple_local_view_set(instance, gui->iconbox_viewer);
	   }
 	   if (!strcmp(ev->key, "l")) {
		entropy_layout_etk_simple_local_view_set(instance, gui->list_viewer);
	   }

  
   }
}

/*Config related functions*/
void
_entropy_layout_etk_simple_config_cb(char* option, void* data)
{
	entropy_layout_gui* gui;

	gui = data;
	
	printf("Layout notified of change to '%s'\n", option);
	if (!strcmp(option, "general.trackback")) {
		if (entropy_config_misc_is_set("general.trackback"))
			entropy_etk_layout_trackback_show(gui, 1);
		else
			entropy_etk_layout_trackback_show(gui, 0);
	} else if (!strcmp(option, "general.treeviewer")) {
		if (entropy_config_misc_is_set("general.treeviewer"))
			entropy_etk_layout_tree_show(gui,1);
		else
			entropy_etk_layout_tree_show(gui,0);
	}
}
/*----*/


/*----*/
/*Interal functions*/

Entropy_Plugin*
entropy_plugin_init (entropy_core * core)
{
  Entropy_Plugin_Gui* plugin;
  Entropy_Plugin* base;
	
  /*Init etk */
  etk_init (NULL, NULL);

  plugin = entropy_malloc(sizeof(Entropy_Plugin_Gui));
  base = ENTROPY_PLUGIN(plugin);
  
  base->functions.entropy_plugin_init = &entropy_plugin_init;
  plugin->gui_functions.layout_main = &entropy_plugin_layout_main;
  plugin->gui_functions.layout_create = &entropy_plugin_layout_create;
  plugin->gui_functions.toolkit_get= &entropy_plugin_toolkit_get;

  return base;
}

char *
entropy_plugin_identify ()
{
  return (char *) "etk";
}

int
entropy_plugin_type_get ()
{
  return ENTROPY_PLUGIN_GUI_LAYOUT;
}

char*
entropy_plugin_toolkit_get() 
{
	return ENTROPY_TOOLKIT_ETK;
}

void
entropy_plugin_layout_main ()
{

  printf("Init ETK main...\n");
  etk_main ();
}


void
gui_event_callback (entropy_notify_event * eevent, void *requestor,
		    void *el, entropy_gui_component_instance * comp)
{
    entropy_layout_gui* view = (entropy_layout_gui*)comp->data;

    switch (eevent->event_type) {
	    case ENTROPY_NOTIFY_FILE_PROGRESS:{
		entropy_file_progress_window* window;
		entropy_file_progress *progress = el;
		

		if (! (window = ecore_hash_get(view->progress_hash, (long*)progress->identifier))) {
			window = entropy_etk_progress_window_create();
			entropy_etk_progress_dialog_show(window);
			entropy_etk_progress_dialog_set_file_from_to(window, progress->file_from, progress->file_to);
			entropy_etk_progress_dialog_set_progress_pct(window, &progress->progress);

			ecore_hash_set(view->progress_hash, (long*)progress->identifier, window);
		} else {
			entropy_etk_progress_dialog_set_file_from_to(window, progress->file_from, progress->file_to);
			entropy_etk_progress_dialog_set_progress_pct(window, &progress->progress);
		}

		if (progress->type == TYPE_END) {
			window = ecore_hash_get(view->progress_hash, (long*)progress->identifier);
			if (window) {
				ecore_hash_remove(view->progress_hash, (long*)progress->identifier);
				entropy_etk_progress_dialog_destroy(window);
			}
		}

	     }
	     break;

	     case ENTROPY_NOTIFY_USER_INTERACTION_YES_NO_ABORT: {
		/*FIXME: We need to handle different types here*/
		entropy_etk_user_interaction_dialog_new((entropy_file_operation*)el);
	     }
	     break;

	     case ENTROPY_NOTIFY_EXTENDED_STAT: {
		 printf("**** Extended stat at layout\n");
		 ecore_hash_set(view->properties_request_hash, (entropy_generic_file*)el, (int*)1);
	     };
	     break;

	     case ENTROPY_NOTIFY_FILE_STAT_AVAILABLE:{
		entropy_file_stat *file_stat = (entropy_file_stat *) eevent->return_struct;							     
		
		if (ecore_hash_get(view->properties_request_hash, file_stat->file)) {
			ecore_hash_remove(view->properties_request_hash, file_stat->file);	
			
			/*Lauch a properties window*/
			etk_properties_dialog_new(file_stat->file);
		}
	     }
	     break;

	     case ENTROPY_NOTIFY_PASTE_REQUEST: {
		printf("Paste request..\n");
							
		Entropy_Selection_Type stype = entropy_core_selection_type_get();
		entropy_generic_file* cfolder = 
			((entropy_gui_component_instance_layout*)comp)->current_folder; 
		Ecore_List* files = entropy_core_selected_files_get();
		
		if (cfolder) {
			char* f_uri = 	cfolder->uri;
			if (f_uri) {
				if (stype == ENTROPY_SELECTION_COPY) {
					printf("Copy type..: %d:%s\n", ecore_list_count(files), f_uri);
					entropy_plugin_filesystem_file_copy_multi(files, f_uri, 
						comp);
				} else if (stype == ENTROPY_SELECTION_CUT) {
					printf("Cut type..:%d:%s\n", ecore_list_count(files), f_uri);
					entropy_plugin_filesystem_file_move_multi(files, f_uri, 
						comp);					
				} else {
					printf("Unsupported copy type at context menu paste\n");
				}
			}
		} else {
			printf("Current folder is NULL at layout paste\n");
		}
	     }
	     break;

	     case ENTROPY_NOTIFY_AUTH_REQUEST: {
		  printf("Requested auth for: %s\n",(char*)el); 
		  etk_auth_request_dialog_create(strdup((char*)el));
	     }
	     break;

	     case ENTROPY_NOTIFY_METADATA_ALL: {
			entropy_etk_efolder_dialog_show((Eina_List*)el);
	     }
	     break;

     }
}

entropy_gui_component_instance *
entropy_plugin_layout_create (entropy_core * core)
{
  Etk_Widget *window;
  entropy_layout_gui *gui;
  entropy_gui_component_instance *layout;

  void *(*local_plugin_init) (entropy_core * core,
				  entropy_gui_component_instance *,
				  void *data);
  entropy_plugin *local;

  entropy_plugin *meta;
  entropy_plugin *hover;
  entropy_plugin *trackback;
  entropy_gui_component_instance* meta_instance;
  entropy_gui_component_instance* hover_instance;
	  
  Etk_Tree_Col* col;
  Etk_Widget* vbox;
  Etk_Widget* menubar;
  Etk_Widget* menu_item;
  Etk_Widget* menu;

  Eina_List* structures;
  Entropy_Config_Structure* structure;
  int local_viewer_selected = 0;


  /*Entropy related init */
  layout = (entropy_gui_component_instance*)entropy_gui_component_instance_layout_new(); /*Create a component instance */
  gui = entropy_malloc (sizeof (entropy_layout_gui));
  layout->data = gui;
  layout->core = core;
  gui->progress_hash = ecore_hash_new(ecore_direct_hash, ecore_direct_compare);
  gui->properties_request_hash = ecore_hash_new(ecore_direct_hash, ecore_direct_compare);

  /*Global init for all layouts*/
  if (!_etk_layout_global_init) {
	  /*Request metadata groups from evfs*/
	entropy_plugin_filesystem_metadata_groups_get(layout);
	  
	  _etk_layout_row_reference = ecore_hash_new(ecore_direct_hash, ecore_direct_compare);
	  _etk_layout_structure_plugin_reference = ecore_hash_new(ecore_direct_hash, ecore_direct_compare);
	  
	  _etk_layout_global_init = 1;
  }


  /*Register this layout container with the core, so our children can get events */
  entropy_core_layout_register (core, layout);


  /*Register this instance (the layout itself), to receive events that can be safely handled
   * by the layout (and reduce the clutter in the child plugins)
   * i.e. PROGRESS events, Stat for properties, Overwrite yes/no/etc events, etc*/

  /*Handle progress events*/
  entropy_core_component_event_register (layout,
					 entropy_core_gui_event_get
					 (ENTROPY_GUI_EVENT_FILE_PROGRESS));

    /*We want to know if the backend needs feedback */
  entropy_core_component_event_register (layout,
					 entropy_core_gui_event_get
					 (ENTROPY_GUI_EVENT_USER_INTERACTION_YES_NO_ABORT));

    /*We want to know if a stat is an 'extended stat' - e.g. a properties dialog etc */
  entropy_core_component_event_register (layout,
					 entropy_core_gui_event_get
					 (ENTROPY_GUI_EVENT_EXTENDED_STAT));

  entropy_core_component_event_register (layout,
					 entropy_core_gui_event_get
					 (ENTROPY_GUI_EVENT_FILE_STAT));
  entropy_core_component_event_register (layout,
					 entropy_core_gui_event_get
					 (ENTROPY_GUI_EVENT_FILE_STAT_AVAILABLE));

  entropy_core_component_event_register (layout,
					 entropy_core_gui_event_get
					 (ENTROPY_GUI_EVENT_METADATA_GROUPS));

  entropy_core_component_event_register (layout,
					 entropy_core_gui_event_get
					 (ENTROPY_GUI_EVENT_PASTE_REQUEST));

  entropy_core_component_event_register (layout,
					 entropy_core_gui_event_get
					 (ENTROPY_GUI_EVENT_AUTH_REQUEST));

  entropy_core_component_event_register (layout,
					 entropy_core_gui_event_get
					 (ENTROPY_GUI_EVENT_META_ALL_REQUEST));


  /*Etk related init */
  window = etk_window_new ();
  layout->gui_object = window;
  etk_signal_connect("key-down", ETK_OBJECT(window), ETK_CALLBACK(_entropy_etk_layout_key_down_cb), layout);
  
  gui->paned = etk_hpaned_new();
  

  etk_signal_connect ("delete-event", ETK_OBJECT (window),
		      ETK_CALLBACK (_etk_window_deleted_cb), layout);

  etk_window_title_set(ETK_WINDOW(window), "Entropy");
  etk_window_wmclass_set(ETK_WINDOW(window), "entropy", "Entropy");

  vbox = etk_vbox_new(ETK_FALSE,0);
  etk_container_add(ETK_CONTAINER(window), vbox);

  /*Tree init*/
  gui->tree = etk_tree_new();
  gui->tree_shell = etk_vbox_new(ETK_FALSE,0);
  etk_box_append(ETK_BOX(gui->tree_shell), gui->tree, ETK_BOX_START, ETK_BOX_EXPAND_FILL, 0); 

  etk_paned_child1_set(ETK_PANED(gui->paned), gui->tree_shell, ETK_FALSE);
  etk_tree_mode_set(ETK_TREE(gui->tree), ETK_TREE_MODE_TREE);
  col = etk_tree_col_new(ETK_TREE(gui->tree), _("Folders"), 60,0.0);
  etk_tree_col_model_add(col, etk_tree_model_image_new());
  etk_tree_col_model_add(col, etk_tree_model_text_new());   
  
  etk_tree_col_expand_set(col, ETK_TRUE);
  etk_tree_build(ETK_TREE(gui->tree));
  etk_widget_size_request_set(ETK_WIDGET(gui->tree), ENTROPY_ETK_WINDOW_PANE_DEFAULT_X, 50);

  /*Register to receive events related to the treeview config*/
  entropy_config_misc_callback_register("general.treeviewer", _entropy_layout_etk_simple_config_cb, gui);

  /*If we're configured not to show tree on start, don't show*/
  if (!entropy_config_misc_is_set("general.treeviewer")) {
	  entropy_etk_layout_tree_show(gui,0);
  } else {
	etk_paned_position_set(ETK_PANED(gui->paned), ENTROPY_ETK_WINDOW_PANE_DEFAULT_X);
  }

  /*LocalShell Init*/
  gui->localshell = etk_vbox_new(ETK_TRUE,0);
  etk_paned_child2_set(ETK_PANED(gui->paned), gui->localshell, ETK_TRUE);

  /*Trackback container init*/
  gui->trackback_shell = etk_vbox_new(ETK_TRUE,0);

  /*Popup init*/
   gui->popup = etk_menu_new();
   etk_signal_connect("row-clicked", ETK_OBJECT( gui->tree  ),
          ETK_CALLBACK(_etk_layout_row_clicked), layout);

   menu_item = _entropy_etk_menu_item_new(ETK_MENU_ITEM_NORMAL, _("Delete this location"),
		   ETK_STOCK_DOCUMENT_OPEN, ETK_MENU_SHELL(gui->popup),NULL);
   etk_signal_connect("activated", ETK_OBJECT(menu_item), ETK_CALLBACK(_etk_layout_location_delete_confirm_cb), layout);
   menu_item = _entropy_etk_menu_item_new(ETK_MENU_ITEM_NORMAL, _("Add a new location"),
		   ETK_STOCK_DOCUMENT_OPEN, ETK_MENU_SHELL(gui->popup),NULL);
   

  for (structures = entropy_config_standard_structures_parse (layout, NULL); structures; ) {
	  structure = structures->data;
	  layout_etk_simple_add_header (layout,structure);
	  structures = structures->next;
  }

  /*Initialise the list view*/
  local = entropy_plugin_gui_get_by_name_toolkit(ENTROPY_TOOLKIT_ETK, "listviewer");
  if (local) {
	  local_plugin_init =
	      dlsym (local->dl_ref, "entropy_plugin_gui_instance_new");   
	  gui->list_viewer = (*local_plugin_init)(core, layout,NULL);
	  gui->list_viewer->plugin = local;
	  
	  if (entropy_config_misc_is_set("general.listviewer")) {
		  gui->list_viewer->active=1;
		  local_viewer_selected = 1;
		  etk_widget_show(gui->list_viewer->gui_object);
		  etk_box_append(ETK_BOX(gui->localshell), gui->list_viewer->gui_object, ETK_BOX_START, ETK_BOX_EXPAND_FILL, 0);
	  } else {
		   gui->list_viewer->active=0;
		   etk_widget_show(gui->list_viewer->gui_object);
	  }
   }

   /*Initialise the icon viewer*/
  /*Initialise the list view*/
  local = entropy_plugin_gui_get_by_name_toolkit(ENTROPY_TOOLKIT_ETK, "iconviewer");
  if (local) {
	  local_plugin_init =
	      dlsym (local->dl_ref, "entropy_plugin_gui_instance_new");   
	  gui->iconbox_viewer = (*local_plugin_init)(core, layout,NULL);
	  gui->iconbox_viewer->plugin = local;

	  if (entropy_config_misc_is_set("general.iconviewer")) {
		  gui->iconbox_viewer->active=1;
		  local_viewer_selected = 1;
		  etk_widget_show(gui->iconbox_viewer->gui_object);
		  etk_box_append(ETK_BOX(gui->localshell), gui->iconbox_viewer->gui_object, ETK_BOX_START, ETK_BOX_EXPAND_FILL, 0);
	  } else {
	  	gui->iconbox_viewer->active=0;
		etk_widget_show(gui->iconbox_viewer->gui_object);
	  }
   }

  /*Initialise the metadata plugin*/
  meta = entropy_plugins_type_get_first(ENTROPY_PLUGIN_GUI_COMPONENT, ENTROPY_PLUGIN_GUI_COMPONENT_INFO_PROVIDER);
  if (meta) {
	  local_plugin_init = 
	  dlsym(meta->dl_ref, "entropy_plugin_gui_instance_new");
	  meta_instance = (*local_plugin_init)(core,layout,NULL);
	  meta_instance->plugin = meta;
  }

  /*Initialise the hover viewer*/
  hover = entropy_plugins_type_get_first(ENTROPY_PLUGIN_GUI_COMPONENT, ENTROPY_PLUGIN_GUI_COMPONENT_HOVER_PROVIDER);
  if (hover) {
	  local_plugin_init = 
	  dlsym(hover->dl_ref, "entropy_plugin_gui_instance_new");
	  hover_instance = (*local_plugin_init)(core,layout,NULL);
	  hover_instance->plugin = hover;
  }


  /*Initialise the trackback*/
  trackback = entropy_plugin_gui_get_by_name_toolkit(ENTROPY_TOOLKIT_ETK, "trackback");
  if (trackback) {
	  local_plugin_init =
	      dlsym (trackback->dl_ref, "entropy_plugin_gui_instance_new");   
	  gui->trackback = (*local_plugin_init)(core, layout,NULL);
	  gui->trackback->plugin = trackback;
	  gui->trackback->active=1;

	  if (entropy_config_misc_is_set("general.trackback")) {
		  etk_box_append(ETK_BOX(gui->trackback_shell), gui->trackback->gui_object, ETK_BOX_START, ETK_BOX_NONE, 0);
	  }
	  entropy_config_misc_callback_register("general.trackback", _entropy_layout_etk_simple_config_cb, gui);
  }



  /*Menu setup*/
  menubar = etk_menu_bar_new();

  /*File menu*/
  menu_item = _entropy_etk_menu_item_new(ETK_MENU_ITEM_NORMAL, _("File"), ETK_STOCK_NO_STOCK, ETK_MENU_SHELL(menubar), NULL);
  menu = etk_menu_new();
  etk_menu_item_submenu_set(ETK_MENU_ITEM(menu_item), ETK_MENU(menu));
  menu_item = _entropy_etk_menu_item_new(ETK_MENU_ITEM_NORMAL, _("Exit"), ETK_STOCK_SYSTEM_SHUTDOWN, ETK_MENU_SHELL(menu), NULL);
  etk_signal_connect("activated", ETK_OBJECT(menu_item), ETK_CALLBACK(etk_layout_simple_exit_cb), layout);
  
  /*Edit menu*/
  menu_item = _entropy_etk_menu_item_new(ETK_MENU_ITEM_NORMAL, _("Edit"), ETK_STOCK_NO_STOCK, ETK_MENU_SHELL(menubar), NULL);
  menu = etk_menu_new();
  etk_menu_item_submenu_set(ETK_MENU_ITEM(menu_item), ETK_MENU(menu));
  
  menu_item = _entropy_etk_menu_item_new(ETK_MENU_ITEM_NORMAL, _("Cut"), ETK_STOCK_EDIT_CUT, ETK_MENU_SHELL(menu), NULL);
  etk_signal_connect("activated", ETK_OBJECT(menu_item), ETK_CALLBACK(_entropy_layout_etk_cut_cb), layout);
  
  menu_item = _entropy_etk_menu_item_new(ETK_MENU_ITEM_NORMAL, _("Copy"), ETK_STOCK_EDIT_COPY, ETK_MENU_SHELL(menu), NULL);
  etk_signal_connect("activated", ETK_OBJECT(menu_item), ETK_CALLBACK(_entropy_layout_etk_copy_cb), layout);
  
  menu_item = _entropy_etk_menu_item_new(ETK_MENU_ITEM_NORMAL, _("Paste"), ETK_STOCK_EDIT_PASTE, ETK_MENU_SHELL(menu), NULL);
  etk_signal_connect("activated", ETK_OBJECT(menu_item), ETK_CALLBACK(_entropy_layout_etk_paste_cb), layout);
  
  
  /*Tools menu*/
  menu_item = _entropy_etk_menu_item_new(ETK_MENU_ITEM_NORMAL, _("Tools"), ETK_STOCK_NO_STOCK, ETK_MENU_SHELL(menubar), NULL);
  menu = etk_menu_new();
  etk_menu_item_submenu_set(ETK_MENU_ITEM(menu_item), ETK_MENU(menu));
  
  menu_item = _entropy_etk_menu_item_new(ETK_MENU_ITEM_NORMAL, _("Add Location"), ETK_STOCK_ADDRESS_BOOK_NEW, ETK_MENU_SHELL(menu), NULL);
  etk_signal_connect("activated", ETK_OBJECT(menu_item), ETK_CALLBACK(_location_add_cb), layout);
 
  menu_item = _entropy_etk_menu_item_new(ETK_MENU_ITEM_NORMAL, _("eFolder Wizard.."), ETK_STOCK_ADDRESS_BOOK_NEW, ETK_MENU_SHELL(menu), NULL);
  etk_signal_connect("activated", ETK_OBJECT(menu_item), ETK_CALLBACK(_entropy_etk_efolder_dialog_show_cb), layout);
  
  menu_item = _entropy_etk_menu_item_new(ETK_MENU_ITEM_NORMAL, _("Program Associations.."), 
		  ETK_STOCK_EMBLEM_SYMBOLIC_LINK, ETK_MENU_SHELL(menu), NULL);
  etk_signal_connect("activated", ETK_OBJECT(menu_item), ETK_CALLBACK(etk_mime_dialog_cb), layout);

  menu_item = _entropy_etk_menu_item_new(ETK_MENU_ITEM_NORMAL, _("Options.."), 
		  ETK_STOCK_EMBLEM_SYMBOLIC_LINK, ETK_MENU_SHELL(menu), NULL);
  etk_signal_connect("activated", ETK_OBJECT(menu_item), ETK_CALLBACK(entropy_etk_options_dialog_cb), layout);
  
  /*View menu*/
  menu_item = _entropy_etk_menu_item_new(ETK_MENU_ITEM_NORMAL, _("View"), ETK_STOCK_NO_STOCK, ETK_MENU_SHELL(menubar), NULL);
  menu = etk_menu_new();
  etk_menu_item_submenu_set(ETK_MENU_ITEM(menu_item), ETK_MENU(menu));
  
  gui->tree_view_menu = _entropy_etk_menu_check_item_new(_("Tree View"), ETK_MENU_SHELL(menu));
  if (entropy_config_misc_is_set("general.treeviewer")) {
	etk_menu_item_check_active_set(ETK_MENU_ITEM_CHECK(gui->tree_view_menu),ETK_TRUE );
  }
  etk_signal_connect("activated", ETK_OBJECT(gui->tree_view_menu), ETK_CALLBACK(entropy_etk_layout_tree_cb), layout);

  menu_item = _entropy_etk_menu_check_item_new(_("Trackback view"), ETK_MENU_SHELL(menu));
  if (entropy_config_misc_is_set("general.trackback")) {
	etk_menu_item_check_active_set(ETK_MENU_ITEM_CHECK(menu_item), ETK_TRUE);
  }
  etk_signal_connect("activated", ETK_OBJECT(menu_item), ETK_CALLBACK(entropy_etk_layout_trackback_cb), layout);


  _entropy_etk_menu_item_new(ETK_MENU_ITEM_SEPARATOR, NULL, 
		  ETK_STOCK_NO_STOCK, ETK_MENU_SHELL(menu), NULL);

  menu_item = _entropy_etk_radio_item_new(_("List View (Alt-l)"), NULL, ETK_MENU_SHELL(menu));
  etk_object_data_set(ETK_OBJECT(menu_item), "VISUAL", gui->list_viewer);
  etk_signal_connect("activated", ETK_OBJECT(menu_item), ETK_CALLBACK(etk_local_viewer_cb), layout);
  if (entropy_config_misc_is_set("general.listviewer")) {
	  etk_menu_item_check_active_set(ETK_MENU_ITEM_CHECK(menu_item), ETK_TRUE);
  }

  
  menu_item = _entropy_etk_radio_item_new(_("Icon View (Alt-i)"), ETK_MENU_ITEM_RADIO(menu_item), ETK_MENU_SHELL(menu));
  etk_object_data_set(ETK_OBJECT(menu_item), "VISUAL", gui->iconbox_viewer);
  etk_signal_connect("activated", ETK_OBJECT(menu_item), ETK_CALLBACK(etk_local_viewer_cb), layout);
  if (entropy_config_misc_is_set("general.iconviewer")) {
	  etk_menu_item_check_active_set(ETK_MENU_ITEM_CHECK(menu_item), ETK_TRUE);
  }


  /*Debug menu*/
  /*menu_item = _entropy_etk_menu_item_new(ETK_MENU_ITEM_NORMAL, _("Debug"), ETK_STOCK_NO_STOCK, ETK_MENU_SHELL(menubar), NULL); 
  menu = etk_menu_new();
  etk_menu_item_submenu_set(ETK_MENU_ITEM(menu_item), ETK_MENU(menu));
  menu_item = _entropy_etk_menu_item_new(ETK_MENU_ITEM_NORMAL, _("File Cache"), ETK_STOCK_PLACES_FOLDER_SAVED_SEARCH, ETK_MENU_SHELL(menu), NULL);
  etk_signal_connect("activated", ETK_OBJECT(menu_item), ETK_CALLBACK(etk_file_cache_dialog_cb), layout);*/
  
  /*Help menu*/
  menu_item = _entropy_etk_menu_item_new(ETK_MENU_ITEM_NORMAL, _("Help"), ETK_STOCK_NO_STOCK, ETK_MENU_SHELL(menubar), NULL);
  menu = etk_menu_new();
  etk_menu_item_submenu_set(ETK_MENU_ITEM(menu_item), ETK_MENU(menu));
  menu_item = _entropy_etk_menu_item_new(ETK_MENU_ITEM_NORMAL, _("About.."), ETK_STOCK_HELP_BROWSER, ETK_MENU_SHELL(menu), NULL);
  etk_signal_connect("activated", ETK_OBJECT(menu_item), ETK_CALLBACK(entropy_etk_about_dialog_cb), layout);

  etk_box_append(ETK_BOX(vbox), menubar, ETK_BOX_START, ETK_BOX_NONE, 0);
  etk_box_append(ETK_BOX(vbox), gui->trackback_shell, ETK_BOX_START, ETK_BOX_NONE, 0);
  etk_box_append(ETK_BOX(vbox), gui->paned, ETK_BOX_START, ETK_BOX_EXPAND_FILL, 0);
  /*---------------------------*/

  gui->statusbar_box = etk_hbox_new(ETK_TRUE, 0);
  etk_box_append(ETK_BOX(vbox), gui->statusbar_box, ETK_BOX_START, ETK_BOX_NONE, 0);
  
  gui->statusbars[0] = etk_statusbar_new();
  etk_statusbar_has_resize_grip_set(ETK_STATUSBAR(gui->statusbars[0]), ETK_FALSE);
  etk_box_append(ETK_BOX(gui->statusbar_box), gui->statusbars[0], ETK_BOX_START, ETK_BOX_EXPAND_FILL, 0);
   
  gui->statusbars[1] = etk_statusbar_new();
  etk_statusbar_has_resize_grip_set(ETK_STATUSBAR(gui->statusbars[1]), ETK_FALSE);
  etk_box_append(ETK_BOX(gui->statusbar_box), gui->statusbars[1], ETK_BOX_START, ETK_BOX_EXPAND_FILL, 0);
   
  gui->statusbars[2] = etk_statusbar_new();
  etk_statusbar_has_resize_grip_set(ETK_STATUSBAR(gui->statusbars[2]), ETK_TRUE);
  etk_box_append(ETK_BOX(gui->statusbar_box), gui->statusbars[2], ETK_BOX_START, ETK_BOX_EXPAND_FILL, 0);
     
  etk_widget_show_all (window);

  /*Increment the window counter*/
  _etk_layout_window_count++;

  etk_window_resize(ETK_WINDOW(window), ENTROPY_ETK_WINDOW_WIDTH, ENTROPY_ETK_WINDOW_HEIGHT);

  return layout;
}
