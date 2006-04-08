#include "entropy.h"
#include "entropy_gui.h"
#include <dlfcn.h>
#include <strings.h>
#include <limits.h>
#include <time.h>
#include <Etk.h>
#include "etk_directory_add_dialog.h"
#include "etk_properties_dialog.h"

typedef struct entropy_etk_iconbox_viewer entropy_etk_iconbox_viewer;
struct entropy_etk_iconbox_viewer
{
  Etk_Widget *iconbox;
  Etk_Widget* parent_visual; 

  Ecore_Hash* etk_list_viewer_row_hash;
  Ecore_Hash* gui_hash;

  Ecore_List *gui_events;
  Ecore_List *files;		/*The entropy_generic_file references we copy. */

  Etk_Widget* popup;
  Etk_Widget* open_with_menu;
  Etk_Widget* open_with_menuitem;

  /*Current folder - TODO - move to core - per layout API*/
  entropy_generic_file* current_folder;

  /*A file we're waiting on for passback properties*/
  Ecore_Hash* properties_request_hash;  
};


Entropy_Plugin* entropy_plugin_init (entropy_core * core);

entropy_gui_component_instance * 
entropy_plugin_gui_instance_new (entropy_core * core, entropy_gui_component_instance * 
		layout, void *data);

void
icon_viewer_add_row (entropy_gui_component_instance * instance,
			  entropy_generic_file * file);


int
entropy_plugin_type_get ();
int
entropy_plugin_sub_type_get ();
char *
entropy_plugin_identify ();
char*
entropy_plugin_toolkit_get() ;

int
entropy_plugin_type_get ()
{
  return ENTROPY_PLUGIN_GUI_COMPONENT;
}

int
entropy_plugin_sub_type_get ()
{
  return ENTROPY_PLUGIN_GUI_COMPONENT_LOCAL_VIEW;
}

char *
entropy_plugin_identify ()
{
  return (char *) "iconviewer";
}

char*
entropy_plugin_toolkit_get() 
{
	return ENTROPY_TOOLKIT_ETK;
}


Entropy_Plugin*
entropy_plugin_init (entropy_core * core)
{
  Entropy_Plugin_Gui* plugin;
  Entropy_Plugin* base;
	
  plugin = entropy_malloc(sizeof(Entropy_Plugin_Gui));
  base = ENTROPY_PLUGIN(plugin);
  
  return base;
}

void
gui_event_callback (entropy_notify_event * eevent, void *requestor,
		    void *el, entropy_gui_component_instance * comp)
{
  entropy_etk_iconbox_viewer *viewer =
    (entropy_etk_iconbox_viewer *) comp->data;

  switch (eevent->event_type) {
  	  case ENTROPY_NOTIFY_FILELIST_REQUEST_EXTERNAL:
	  case ENTROPY_NOTIFY_FILELIST_REQUEST:{
	      entropy_generic_file *file;
	      Ecore_List* remove_ref;
	      entropy_generic_file *event_file =
		((entropy_file_request *) eevent->data)->file;

	      viewer->current_folder = event_file;

	      //remove_ref = gui_object_destroy_and_free(comp, viewer->gui_hash);

	      etk_iconbox_clear(ETK_ICONBOX(viewer->iconbox));
	      etk_iconbox_freeze(ETK_ICONBOX(viewer->iconbox));
	      ecore_list_goto_first (el);
		while ((file = ecore_list_next (el))) {
		      icon_viewer_add_row (comp, file);
		}

		/*while ( (ref = ecore_list_remove_first(remove_ref)))  {
			entropy_core_file_cache_remove_reference (ref);
		}
		ecore_list_destroy(remove_ref);*/
	      etk_iconbox_thaw(ETK_ICONBOX(viewer->iconbox));


	      }
	      break;
  }
}

void
icon_viewer_add_row (entropy_gui_component_instance * instance,
			  entropy_generic_file * file)
{
	entropy_etk_iconbox_viewer* viewer;
	
	viewer = instance->data;
	
	if (!file->thumbnail) {
		etk_iconbox_append(ETK_ICONBOX(viewer->iconbox), etk_theme_icon_theme_get(), "places/folder_48", file->filename);
		//etk_iconbox_append(ETK_ICONBOX(viewer->iconbox), PACKAGE_DATA_DIR "/icons/default.png", NULL, file->filename);
	} else {
		etk_iconbox_append(ETK_ICONBOX(viewer->iconbox), etk_theme_icon_theme_get(), "places/folder_48", file->filename);
		//etk_iconbox_append(ETK_ICONBOX(viewer->iconbox), file->thumbnail->thumbnail_filename, NULL, file->filename);		
	}
}



entropy_gui_component_instance *
entropy_plugin_gui_instance_new (entropy_core * core,
		     entropy_gui_component_instance * layout, void *data)
{	
  entropy_gui_component_instance *instance;	
  entropy_etk_iconbox_viewer *viewer;
  char  **dnd_types;
  int dnd_types_num=0;
  Etk_Widget* new_menu;
  Etk_Widget* menu_item;

    
  instance = entropy_gui_component_instance_new ();
  viewer = entropy_malloc (sizeof (entropy_etk_iconbox_viewer));

  viewer->iconbox = etk_iconbox_new();
  instance->gui_object = viewer->iconbox;
  instance->core = core;
  instance->data = viewer;
  instance->layout_parent = layout;

  /*Register out interest in receiving folder notifications */
  entropy_core_component_event_register (instance,
					 entropy_core_gui_event_get
					 (ENTROPY_GUI_EVENT_FOLDER_CHANGE_CONTENTS));
  entropy_core_component_event_register (instance,
					 entropy_core_gui_event_get
					 (ENTROPY_GUI_EVENT_FOLDER_CHANGE_CONTENTS_EXTERNAL));

  /*Register our interest in receiving file mod/create/delete notifications */
  entropy_core_component_event_register (instance,
					 entropy_core_gui_event_get
					 (ENTROPY_GUI_EVENT_FILE_CHANGE));
  entropy_core_component_event_register (instance,
					 entropy_core_gui_event_get
					 (ENTROPY_GUI_EVENT_FILE_CREATE));
  entropy_core_component_event_register (instance,
					 entropy_core_gui_event_get
					 (ENTROPY_GUI_EVENT_FILE_REMOVE));
  entropy_core_component_event_register (instance,
					 entropy_core_gui_event_get
					 (ENTROPY_GUI_EVENT_FILE_REMOVE_DIRECTORY));

  /*Register interest in getting stat events */
  entropy_core_component_event_register (instance,
					 entropy_core_gui_event_get
					 (ENTROPY_GUI_EVENT_FILE_STAT));
  entropy_core_component_event_register (instance,
					 entropy_core_gui_event_get
					 (ENTROPY_GUI_EVENT_FILE_STAT_AVAILABLE));

  /*We want to know about thumbnail available events */
  entropy_core_component_event_register (instance,
					 entropy_core_gui_event_get
					 (ENTROPY_GUI_EVENT_THUMBNAIL_AVAILABLE));


  etk_widget_show_all(viewer->iconbox);

  return instance;
}
