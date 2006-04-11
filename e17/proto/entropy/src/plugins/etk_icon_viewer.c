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


/*------------- boilerplate -----*/
typedef struct gui_file gui_file;
struct gui_file
{
  entropy_generic_file *file;
  entropy_gui_component_instance *instance;
  Etk_Iconbox_Icon *icon;
};

gui_file* _gui_file_new(entropy_generic_file* file, 
		entropy_gui_component_instance* instance, Etk_Iconbox_Icon* icon)
{
	gui_file* gfile =  entropy_malloc(sizeof(gui_file));
	gfile->file = file;
	gfile->instance = instance;
	gfile->icon = icon;

	return gfile;
}

void
gui_file_destroy (gui_file * file)
{
  entropy_free (file);

}

Ecore_List* 
gui_object_destroy_and_free (entropy_gui_component_instance * comp,
			     Ecore_Hash * gui_hash)
{

  Ecore_List *list;
  Ecore_List *file_remove_ref_list;
  entropy_generic_file *obj;
  gui_file *freeobj;
  entropy_etk_iconbox_viewer *view = comp->data;


  file_remove_ref_list = ecore_list_new();
  
  /*Temporarily stop callbacks, we don't want to clobber an in-op process */
  entropy_notify_lock_loop (comp->core->notify);

  list = ecore_hash_keys (gui_hash);

  ecore_list_goto_first (list);
  while ((obj = ecore_list_next (list))) {


    freeobj = ecore_hash_get (gui_hash, obj);
    if (freeobj) {
      /*De-Associate this icon with this file in the core, so DND works */
      entropy_core_object_file_disassociate (freeobj->icon);

      gui_file_destroy (freeobj);
    }

    /*Tell the core we no longer need this file - it might free it now */
    ecore_list_append(file_remove_ref_list, obj->md5);
  }
  ecore_hash_destroy (gui_hash);
  view->gui_hash = ecore_hash_new(ecore_direct_hash, ecore_direct_compare);
  ecore_list_destroy (list);


  /*ecore_list_goto_first(view->files);
  while ((row = ecore_list_remove_first(view->files))) {
	  ecore_hash_remove(etk_list_viewer_row_hash, row);
  }*/

  entropy_notify_unlock_loop (comp->core->notify);


  return file_remove_ref_list;

}
/*----- End boilerplate -----*/


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

void _etk_entropy_click_cb(Etk_Object *object, void *event_info, void *data)
{
  entropy_gui_component_instance *instance;	
  entropy_etk_iconbox_viewer *viewer;
  entropy_generic_file* file;
  entropy_gui_event* gui_event;
  Etk_Iconbox_Icon* icon;
  Etk_Event_Mouse_Up_Down *event;

  instance = data;
  viewer = instance->data;
  event = event_info;

   if (!(icon = etk_iconbox_icon_get_at_xy(ETK_ICONBOX(viewer->iconbox), 
   	event->canvas.x, event->canvas.y, ETK_FALSE, ETK_TRUE, ETK_TRUE)))
      return;

  file = etk_iconbox_icon_data_get(icon);
  printf("File is %p\n", file);

  if (file) {
  	gui_event = entropy_malloc (sizeof (entropy_gui_event));
	  gui_event->event_type =
		entropy_core_gui_event_get (ENTROPY_GUI_EVENT_ACTION_FILE);
	  gui_event->data = file;
	  entropy_core_layout_notify_event (instance, gui_event, ENTROPY_EVENT_GLOBAL); 
  }
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
	      char* ref;
	      Ecore_List* remove_ref;
	      entropy_generic_file *event_file =
		((entropy_file_request *) eevent->data)->file;

	      viewer->current_folder = event_file;

	      remove_ref = gui_object_destroy_and_free(comp, viewer->gui_hash);

	      etk_iconbox_clear(ETK_ICONBOX(viewer->iconbox));
	      etk_iconbox_freeze(ETK_ICONBOX(viewer->iconbox));
	      ecore_list_goto_first (el);
		while ((file = ecore_list_next (el))) {
		      icon_viewer_add_row (comp, file);
		}

		while ( (ref = ecore_list_remove_first(remove_ref)))  {
			entropy_core_file_cache_remove_reference (ref);
		}
		ecore_list_destroy(remove_ref);
	      etk_iconbox_thaw(ETK_ICONBOX(viewer->iconbox));


	      }
	      break;

           case ENTROPY_NOTIFY_THUMBNAIL_REQUEST:{

   	   /*Only bother if we have a thumbnail, and a component */
	      if (el && comp) {
		gui_file *obj;
		entropy_thumbnail *thumb = (entropy_thumbnail *) el;
		entropy_etk_iconbox_viewer *view = comp->data;
	
		obj = ecore_hash_get (view->gui_hash, thumb->parent);

		if (obj) {
		  etk_iconbox_icon_file_set(obj->icon, thumb->thumbnail_filename, NULL);
		} else {
		  /*printf ("ERR: Couldn't find a hash reference for this file!\n");*/
		}

	      }
	    }				//End case
	    break;					  
  }
}

void
icon_viewer_add_row (entropy_gui_component_instance * instance,
			  entropy_generic_file * file)
{
	entropy_etk_iconbox_viewer* viewer;
	Etk_Iconbox_Icon* icon;
	
	viewer = instance->data;
	
	if (!file->thumbnail) {
		icon = etk_iconbox_append(ETK_ICONBOX(viewer->iconbox), PACKAGE_DATA_DIR "/icons/default.png", NULL, file->filename);
		etk_iconbox_icon_data_set(icon, file);
		ecore_hash_set(viewer->gui_hash, file, _gui_file_new(file,instance,icon));
		entropy_plugin_thumbnail_request(instance, file, (void*)gui_event_callback);
	} else {
		icon = etk_iconbox_append(ETK_ICONBOX(viewer->iconbox), file->thumbnail->thumbnail_filename, NULL, file->filename);		
		ecore_hash_set(viewer->gui_hash, file, _gui_file_new(file,instance,icon));
		etk_iconbox_icon_data_set(icon, file);		
	}

	entropy_core_file_cache_add_reference (file->md5);
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

  viewer->gui_hash = ecore_hash_new(ecore_direct_hash,ecore_direct_compare);

  viewer->iconbox = etk_iconbox_new();
  etk_signal_connect("mouse_up", ETK_OBJECT(viewer->iconbox), ETK_CALLBACK(_etk_entropy_click_cb), instance);
  
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
