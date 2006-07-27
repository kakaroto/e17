#include <Etk.h>
#include "entropy.h"
#include "entropy_gui.h"
#include "etk_location_add_dialog.h"
#include "etk_progress_dialog.h"
#include "etk_properties_dialog.h"
#include "etk_user_interaction_dialog.h"
#include "etk_mime_dialog.h"
#include "etk_file_cache_dialog.h"
#include "entropy_etk_context_menu.h"
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
  Ecore_Hash* progress_hash; /*Track progress events->dialogs*/

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

void entropy_layout_etk_simple_local_view_set(entropy_gui_component_instance* instance,
		entropy_gui_component_instance* local);



void layout_etk_simple_quit(entropy_core* core)
{
  entropy_core_destroy (core);
  etk_main_quit ();
}

static Etk_Bool
_etk_window_deleted_cb (Etk_Object * object, void *data)
{
  entropy_gui_component_instance* instance = data;
	
  /*Decrement window reference counter*/
  _etk_layout_window_count--;

  /*TODO - destroy this layout object, and deregister for events*/

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
			while ( (ref = ecore_list_remove_first(row_refs))) {
				etk_tree_row_del(ref->row);
				IF_FREE(ref);
			}
			ecore_list_destroy(row_refs);
			ecore_hash_remove(_etk_layout_structure_plugin_reference, structure);
		}
		entropy_config_standard_structure_remove(structure);
	}
}

static void _etk_layout_row_clicked(Etk_Object *object, 
		Etk_Tree_Row *row, Etk_Event_Mouse_Up_Down *event, void *data)
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

Entropy_Plugin*
entropy_plugin_init (entropy_core * core)
{
  Entropy_Plugin_Gui* plugin;
  Entropy_Plugin* base;
	
  /*Init etk */
  etk_init ("ecore_evas_software_x11");

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

void etk_layout_simple_exit_cb(Etk_Object* obj, void* data)
{
	entropy_gui_component_instance* instance = data;

	layout_etk_simple_quit(instance->core);
}


void etk_mime_dialog_cb(Etk_Object* obj, void* data)
{
	etk_mime_dialog_create();
}

void entropy_etk_layout_trackback_cb(Etk_Object* obj, void* data)
{
	entropy_gui_component_instance* instance = data;
	entropy_layout_gui* gui = instance->data;

	if (!etk_widget_is_visible(gui->trackback->gui_object)) {
		etk_widget_show_all(gui->trackback->gui_object);
	} else {
		etk_widget_hide(gui->trackback->gui_object);
	}
}

void entropy_etk_layout_tree_cb(Etk_Object* obj, void* data)
{
	entropy_gui_component_instance* instance = data;
	entropy_layout_gui* gui = instance->data;

	if (!etk_widget_is_visible(gui->tree)) {
		etk_widget_show_all(gui->tree);
		etk_paned_position_set(ETK_PANED(gui->paned), ENTROPY_ETK_WINDOW_PANE_DEFAULT_X);
	} else {
		etk_widget_hide(gui->tree);
		etk_paned_position_set(ETK_PANED(gui->paned), 0);
	}
}

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
	Evas_List* children;
	entropy_layout_gui* gui = instance->data;
	Etk_Widget* widget;
	entropy_generic_file* file;

	/*FIXME - disable plugins - this should be cleaner*/
	gui->iconbox_viewer->active = 0;
	gui->list_viewer->active = 0;

	for (children = etk_container_children_get(ETK_CONTAINER(gui->localshell)); children; ) {
		widget = children->data;
		etk_container_remove(ETK_CONTAINER(gui->localshell), widget);
			
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
		etk_box_pack_start(ETK_BOX(gui->localshell), local->gui_object, ETK_TRUE,ETK_TRUE,0);
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


void
_entropy_etk_layout_key_down_cb(Etk_Object *object, void *event, void *data)
{
   Etk_Event_Key_Up_Down *ev;
   entropy_gui_component_instance* instance = data;
   entropy_layout_gui* gui = instance->data;

   ev = event;

   if(evas_key_modifier_is_set(ev->modifiers, "Control"))
   {
	   if (!strcmp(ev->key, "q")) {
		   layout_etk_simple_quit(instance->core);
	   }
   } else if (evas_key_modifier_is_set(ev->modifiers, "Alt")) {
 	   if (!strcmp(ev->key, "i")) {
		entropy_layout_etk_simple_local_view_set(instance, gui->iconbox_viewer);
	   }
 	   if (!strcmp(ev->key, "l")) {
		entropy_layout_etk_simple_local_view_set(instance, gui->list_viewer);
	   }

  
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
			   
	
  etk_tree_freeze(ETK_TREE(gui->tree));
  row = etk_tree_append(ETK_TREE(gui->tree), col, 
			  icon_string, structure_obj->name, NULL);
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

	     case ENTROPY_NOTIFY_METADATA_GROUPS: {
		/*entropy_etk_context_menu_metadata_groups_populate((Evas_List*)eevent->return_struct);*/
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
  entropy_gui_component_instance* instance=  NULL;

  void *(*local_plugin_init) (entropy_core * core,
				  entropy_gui_component_instance *,
				  void *data);
  entropy_plugin *local;

  entropy_gui_component_instance*
	  (*metadata_plugin_init) (entropy_core * core,
				  entropy_gui_component_instance *,
				  void* parent_visual,
				  void *data);

  
  entropy_plugin *meta;
  entropy_plugin *trackback;
  entropy_gui_component_instance* meta_instance;
	  
  Etk_Tree_Col* col;
  Etk_Widget* vbox;
  Etk_Widget* menubar;
  Etk_Widget* menu_item;
  Etk_Widget* menu;

  Evas_List* structures;
  Entropy_Config_Structure* structure;


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


  /*Etk related init */
  window = etk_window_new ();
  layout->gui_object = window;
  etk_signal_connect("key_down", ETK_OBJECT(window), ETK_CALLBACK(_entropy_etk_layout_key_down_cb), layout);
  
  gui->paned = etk_hpaned_new();
  

  etk_signal_connect ("delete_event", ETK_OBJECT (window),
		      ETK_CALLBACK (_etk_window_deleted_cb), layout);

  etk_window_title_set(ETK_WINDOW(window), "Entropy");
  etk_window_wmclass_set(ETK_WINDOW(window), "entropy", "Entropy");

  vbox = etk_vbox_new(ETK_FALSE,0);
  etk_container_add(ETK_CONTAINER(window), vbox);

  /*Tree init*/
  gui->tree = etk_tree_new();
  etk_paned_child1_set(ETK_PANED(gui->paned), gui->tree, ETK_FALSE);
  etk_tree_mode_set(ETK_TREE(gui->tree), ETK_TREE_MODE_TREE);
  col = etk_tree_col_new(ETK_TREE(gui->tree), _("Folders"), 
		  etk_tree_model_icon_text_new(ETK_TREE(gui->tree), ETK_TREE_FROM_FILE), 60);
  
  etk_tree_col_expand_set(col, ETK_TRUE);
  etk_tree_build(ETK_TREE(gui->tree));

  etk_widget_size_request_set(ETK_WIDGET(gui->tree), ENTROPY_ETK_WINDOW_PANE_DEFAULT_X, 50);

  /*LocalShell Init*/
  gui->localshell = etk_vbox_new(ETK_TRUE,0);
  etk_paned_child2_set(ETK_PANED(gui->paned), gui->localshell, ETK_TRUE);

  /*Popup init*/
   gui->popup = etk_menu_new();
   etk_signal_connect("row_clicked", ETK_OBJECT( gui->tree  ),
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
	  instance = (*local_plugin_init)(core, layout,NULL);
	  instance->plugin = local;
	  gui->list_viewer = instance;
	  etk_box_pack_start(ETK_BOX(gui->localshell), instance->gui_object, ETK_TRUE,ETK_TRUE,0);
   }

   /*Initialise the icon viewer*/
  /*Initialise the list view*/
  local = entropy_plugin_gui_get_by_name_toolkit(ENTROPY_TOOLKIT_ETK, "iconviewer");
  if (local) {
	  local_plugin_init =
	      dlsym (local->dl_ref, "entropy_plugin_gui_instance_new");   
	  gui->iconbox_viewer = (*local_plugin_init)(core, layout,NULL);
	  gui->iconbox_viewer->plugin = local;
	  gui->iconbox_viewer->active=0;
	  //etk_box_pack_start(ETK_BOX(gui->localshell), instance->gui_object, ETK_TRUE,ETK_TRUE,0);
   }



  /*Initialise the metadata plugin*/
  meta = entropy_plugins_type_get_first(ENTROPY_PLUGIN_GUI_COMPONENT, ENTROPY_PLUGIN_GUI_COMPONENT_INFO_PROVIDER);
  if (meta) {
	  metadata_plugin_init = 
	  dlsym(meta->dl_ref, "entropy_plugin_gui_instance_new");
	  meta_instance = (*metadata_plugin_init)(core,layout,layout->gui_object,NULL);
	  meta_instance->plugin = meta;
  }


  /*Initialise the trackback*/
  trackback = entropy_plugin_gui_get_by_name_toolkit(ENTROPY_TOOLKIT_ETK, "trackback");
  if (trackback) {
	  local_plugin_init =
	      dlsym (trackback->dl_ref, "entropy_plugin_gui_instance_new");   
	  gui->trackback = (*local_plugin_init)(core, layout,NULL);
	  gui->trackback->plugin = trackback;
	  gui->trackback->active=1;
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
  _entropy_etk_menu_item_new(ETK_MENU_ITEM_NORMAL, _("Cut"), ETK_STOCK_EDIT_CUT, ETK_MENU_SHELL(menu), NULL);
  _entropy_etk_menu_item_new(ETK_MENU_ITEM_NORMAL, _("Copy"), ETK_STOCK_EDIT_COPY, ETK_MENU_SHELL(menu), NULL);
  _entropy_etk_menu_item_new(ETK_MENU_ITEM_NORMAL, _("Paste"), ETK_STOCK_EDIT_PASTE, ETK_MENU_SHELL(menu), NULL);
  
  
  /*Tools menu*/
  menu_item = _entropy_etk_menu_item_new(ETK_MENU_ITEM_NORMAL, _("Tools"), ETK_STOCK_NO_STOCK, ETK_MENU_SHELL(menubar), NULL);
  menu = etk_menu_new();
  etk_menu_item_submenu_set(ETK_MENU_ITEM(menu_item), ETK_MENU(menu));
  
  menu_item = _entropy_etk_menu_item_new(ETK_MENU_ITEM_NORMAL, _("Add Location"), ETK_STOCK_ADDRESS_BOOK_NEW, ETK_MENU_SHELL(menu), NULL);
  etk_signal_connect("activated", ETK_OBJECT(menu_item), ETK_CALLBACK(_location_add_cb), layout);
  
  menu_item = _entropy_etk_menu_item_new(ETK_MENU_ITEM_NORMAL, _("Program Associations.."), 
		  ETK_STOCK_EMBLEM_SYMBOLIC_LINK, ETK_MENU_SHELL(menu), NULL);
  etk_signal_connect("activated", ETK_OBJECT(menu_item), ETK_CALLBACK(etk_mime_dialog_cb), layout);
  
  /*View menu*/
  menu_item = _entropy_etk_menu_item_new(ETK_MENU_ITEM_NORMAL, _("View"), ETK_STOCK_NO_STOCK, ETK_MENU_SHELL(menubar), NULL);
  menu = etk_menu_new();
  etk_menu_item_submenu_set(ETK_MENU_ITEM(menu_item), ETK_MENU(menu));
  
  menu_item = _entropy_etk_menu_check_item_new(_("Tree View"), ETK_MENU_SHELL(menu));
  etk_menu_item_check_active_set(ETK_MENU_ITEM_CHECK(menu_item),1 );
  etk_signal_connect("activated", ETK_OBJECT(menu_item), ETK_CALLBACK(entropy_etk_layout_tree_cb), layout);

  menu_item = _entropy_etk_menu_check_item_new(_("Trackback view"), ETK_MENU_SHELL(menu));
  etk_menu_item_check_active_set(ETK_MENU_ITEM_CHECK(menu_item),1 );
  etk_signal_connect("activated", ETK_OBJECT(menu_item), ETK_CALLBACK(entropy_etk_layout_trackback_cb), layout);


  _entropy_etk_menu_item_new(ETK_MENU_ITEM_SEPARATOR, NULL, 
		  ETK_STOCK_NO_STOCK, ETK_MENU_SHELL(menu), NULL);

  menu_item = _entropy_etk_radio_item_new(_("List View (Alt-l)"), NULL, ETK_MENU_SHELL(menu));
  etk_object_data_set(ETK_OBJECT(menu_item), "VISUAL", gui->list_viewer);
  etk_signal_connect("activated", ETK_OBJECT(menu_item), ETK_CALLBACK(etk_local_viewer_cb), layout);

  
  menu_item = _entropy_etk_radio_item_new(_("Icon View (Alt-i)"), menu_item, ETK_MENU_SHELL(menu));
  etk_object_data_set(ETK_OBJECT(menu_item), "VISUAL", gui->iconbox_viewer);
  etk_signal_connect("activated", ETK_OBJECT(menu_item), ETK_CALLBACK(etk_local_viewer_cb), layout);


  /*Debug menu*/
  menu_item = _entropy_etk_menu_item_new(ETK_MENU_ITEM_NORMAL, _("Debug"), ETK_STOCK_NO_STOCK, ETK_MENU_SHELL(menubar), NULL); 
  menu = etk_menu_new();
  etk_menu_item_submenu_set(ETK_MENU_ITEM(menu_item), ETK_MENU(menu));
  menu_item = _entropy_etk_menu_item_new(ETK_MENU_ITEM_NORMAL, _("File Cache"), ETK_STOCK_PLACES_FOLDER_SAVED_SEARCH, ETK_MENU_SHELL(menu), NULL);
  etk_signal_connect("activated", ETK_OBJECT(menu_item), ETK_CALLBACK(etk_file_cache_dialog_cb), layout);
  
  /*Help menu*/
  menu_item = _entropy_etk_menu_item_new(ETK_MENU_ITEM_NORMAL, _("Help"), ETK_STOCK_NO_STOCK, ETK_MENU_SHELL(menubar), NULL);
  menu = etk_menu_new();
  etk_menu_item_submenu_set(ETK_MENU_ITEM(menu_item), ETK_MENU(menu));
  _entropy_etk_menu_item_new(ETK_MENU_ITEM_NORMAL, _("About.."), ETK_STOCK_HELP_BROWSER, ETK_MENU_SHELL(menu), NULL);


  etk_box_pack_start(ETK_BOX(vbox), menubar, ETK_FALSE, ETK_FALSE, 0);

  if (trackback) {
	  etk_box_pack_start(ETK_BOX(vbox), gui->trackback->gui_object, ETK_FALSE,ETK_FALSE,0);
  }
  
  etk_box_pack_start(ETK_BOX(vbox), gui->paned, TRUE, TRUE, 0);
  /*---------------------------*/

  gui->statusbar_box = etk_hbox_new(ETK_TRUE, 0);
  etk_box_pack_start(ETK_BOX(vbox), gui->statusbar_box, ETK_FALSE, ETK_FALSE, 0);
  
  gui->statusbars[0] = etk_statusbar_new();
  etk_statusbar_has_resize_grip_set(ETK_STATUSBAR(gui->statusbars[0]), ETK_FALSE);
  etk_box_pack_start(ETK_BOX(gui->statusbar_box), gui->statusbars[0], ETK_TRUE, ETK_TRUE, 0);
   
  gui->statusbars[1] = etk_statusbar_new();
  etk_statusbar_has_resize_grip_set(ETK_STATUSBAR(gui->statusbars[1]), ETK_FALSE);
  etk_box_pack_start(ETK_BOX(gui->statusbar_box), gui->statusbars[1], ETK_TRUE, ETK_TRUE, 0);
   
  gui->statusbars[2] = etk_statusbar_new();
  etk_statusbar_has_resize_grip_set(ETK_STATUSBAR(gui->statusbars[2]), ETK_TRUE);
  etk_box_pack_start(ETK_BOX(gui->statusbar_box), gui->statusbars[2], ETK_TRUE, ETK_TRUE, 0);
     
  etk_widget_show_all (window);

  /*Increment the window counter*/
  _etk_layout_window_count++;

  etk_paned_position_set(ETK_PANED(gui->paned), ENTROPY_ETK_WINDOW_PANE_DEFAULT_X);
  etk_window_resize(ETK_WINDOW(window), ENTROPY_ETK_WINDOW_WIDTH, ENTROPY_ETK_WINDOW_HEIGHT);

  return layout;
}
