#include "entropy.h"
#include "entropy_gui.h"
#include <dlfcn.h>
#include <strings.h>
#include <limits.h>
#include <time.h>
#include <Etk.h>


typedef struct entropy_etk_trackback {
	Etk_Widget* label;
	entropy_generic_file* base_file;
	Etk_Widget* hbox;
	Ecore_List* levels;
} entropy_etk_trackback;


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
  return (char *) "trackback";
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


void _entropy_etk_trackback_up_cb(Etk_Object* w, void* user_data)
{
	entropy_gui_component_instance* comp = user_data;
	entropy_etk_trackback* trackback = comp->data;
	entropy_generic_file* parent_dir = NULL;

	parent_dir = entropy_layout_current_folder_get((entropy_gui_component_instance_layout*)comp->layout_parent);
	if (parent_dir) {
		parent_dir = entropy_core_parent_folder_file_get(parent_dir);
		if (parent_dir) {
			entropy_event_action_file(parent_dir, comp);
			trackback->base_file = parent_dir;
		}
	}

}

void _entropy_etk_trackback_combo_cb(Etk_Object *object, void *data)
{
   entropy_gui_component_instance* comp = NULL;
   entropy_generic_file* file;
   
   file = etk_combobox_item_data_get(ETK_COMBOBOX_ITEM(object));
   comp  = etk_object_data_get(ETK_OBJECT(ETK_COMBOBOX_ITEM(object)->combobox), "COMP");

   printf("Item activated, file: %p, %s/%s, comp: %p\n", file, file->path, file->filename, comp);
   if (file && comp) entropy_event_action_file(file, comp);
}

void
gui_event_callback (entropy_notify_event * eevent, void *requestor,
		    void *el, entropy_gui_component_instance * comp)
{
  entropy_etk_trackback* trackback = comp->data;
	
  switch (eevent->event_type) {
  	  case ENTROPY_NOTIFY_FILELIST_REQUEST_EXTERNAL:
	  case ENTROPY_NOTIFY_FILELIST_REQUEST:{
  	        entropy_generic_file *event_file =
		((entropy_file_request *) eevent->data)->file;
						       
		char buf[PATH_MAX];
		int do_combo_new = 0;

		if (!trackback->base_file) {
			snprintf(buf, PATH_MAX, "%s/%s", event_file->path, event_file->filename);
			etk_label_set(ETK_LABEL(trackback->label), buf);
			trackback->base_file = event_file;
			do_combo_new = 1;
		} else {
			/*If a base file already here, check if this new parent_dir 
			 * is a child of the current base_dir.  If so, continue on,
			 * if not, nuke all existing combos, and set as new base_dir*/
			int hit=0;
			entropy_generic_file* traverse_file = 
				entropy_core_parent_folder_file_get(event_file);

			ecore_list_first_goto(trackback->levels);
			while ( ecore_list_current(trackback->levels) ) {
				  entropy_generic_file* combo_file = 
				       etk_object_data_get(ETK_OBJECT(ecore_list_current(trackback->levels)),"FILE");
				
				  //printf ("^^^^^^^^^^^ %s/%s is %s/%s?\n", traverse_file->path, traverse_file->filename,
					//combo_file->path, combo_file->filename);

	                          if (combo_file == traverse_file) {
					  //printf (" >>>>> YES\n");
					  /*etk_object_destroy(ETK_OBJECT(ecore_list_current(trackback->levels)));
					  ecore_list_remove(trackback->levels);*/

				  
	                                  hit = 1;
	                                  goto done;
	                          } else {
					  //printf (" >>>>> NO\n");
					  etk_object_destroy(ETK_OBJECT(ecore_list_current(trackback->levels)));
					  ecore_list_remove(trackback->levels);
				  }
	                }
	
			done:
	
			/*Check if new dir is a direct descendent...*/
			if (hit == 1) {
				/*HACK - We access combobox internals here..*/
				ecore_list_first_goto(trackback->levels);
				Etk_Widget* last_combo;
				if (  (last_combo = ecore_list_current(trackback->levels))) {
					Etk_Combobox_Item* item = NULL;					
					for ( item = ETK_COMBOBOX(last_combo)->first_item; item; item=item->next) {

						if (etk_combobox_item_data_get(item) == event_file) {
							etk_combobox_active_item_set(ETK_COMBOBOX(last_combo), item);
						}
						
					}
					
				}
				
				do_combo_new = 1;			
			} else {
				snprintf(buf, PATH_MAX, "%s/%s", event_file->path, event_file->filename);
				etk_label_set(ETK_LABEL(trackback->label), buf);
				trackback->base_file = event_file;	
				do_combo_new = 1;
			}				
		}

		if (do_combo_new) {
			Etk_Widget* combo;
			entropy_generic_file* file;
			Ecore_List* fl = el;

			if (!trackback->levels) 
				trackback->levels = ecore_list_new();

			//printf (" *** Event file is %s/%s\n", event_file->path, event_file->filename);
			
			combo = etk_combobox_new_default();
			
			ecore_list_first_goto(fl);
			while ( (file = ecore_list_next(fl) )) {
				Etk_Combobox_Item* item;
				
				if (!strcmp(file->mime_type, "file/folder")) {
					snprintf(buf, PATH_MAX, "%s", file->filename);
					item = etk_combobox_item_append(ETK_COMBOBOX(combo), buf);
					etk_combobox_item_data_set(item, file);

					etk_signal_connect("mouse-click", ETK_OBJECT(item),
						ETK_CALLBACK(_entropy_etk_trackback_combo_cb), comp);

					//printf("      ** Added %s.%p.\n", file->filename, comp);
				}
			}
			etk_object_data_set(ETK_OBJECT(combo), "FILE", event_file);
			etk_object_data_set(ETK_OBJECT(combo), "COMP", comp);
			
			etk_widget_show_all(combo);
			etk_box_append(ETK_BOX(trackback->hbox), combo, ETK_BOX_START, ETK_BOX_NONE, 0);


			ecore_list_prepend(trackback->levels, combo);

		}
          }
	  break;
  }
}


entropy_gui_component_instance *
entropy_plugin_gui_instance_new (entropy_core * core,
		     entropy_gui_component_instance * layout, void *data)
{	
  entropy_gui_component_instance *instance;
  entropy_etk_trackback* trackback;
  Etk_Widget* button;

  instance = entropy_gui_component_instance_new ();
  trackback = calloc(1, sizeof(entropy_etk_trackback));
  trackback->levels = NULL;
  trackback->base_file = NULL;
  instance->data = trackback;

  trackback->hbox = etk_hbox_new(ETK_FALSE,0);
  button = etk_button_new_from_stock(ETK_STOCK_GO_UP);
  etk_signal_connect("pressed", ETK_OBJECT(button), ETK_CALLBACK(_entropy_etk_trackback_up_cb), instance);
  
  trackback->label = etk_label_new("");
  etk_box_append(ETK_BOX(trackback->hbox), button, ETK_BOX_START, ETK_BOX_NONE, 0);
  etk_box_append(ETK_BOX(trackback->hbox), trackback->label, ETK_BOX_START, ETK_BOX_NONE, 0);
  
  
  instance->gui_object = trackback->hbox;
  instance->core = core;
  instance->layout_parent = layout;

  /*Register out interest in receiving folder notifications */
  entropy_core_component_event_register (instance,
					 entropy_core_gui_event_get
					 (ENTROPY_GUI_EVENT_FOLDER_CHANGE_CONTENTS));
  entropy_core_component_event_register (instance,
					 entropy_core_gui_event_get
					 (ENTROPY_GUI_EVENT_FOLDER_CHANGE_CONTENTS_EXTERNAL));

  return instance;
}

