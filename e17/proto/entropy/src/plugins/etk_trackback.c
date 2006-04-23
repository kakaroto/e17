#include "entropy.h"
#include "entropy_gui.h"
#include <dlfcn.h>
#include <strings.h>
#include <limits.h>
#include <time.h>
#include <Etk.h>


typedef struct entropy_etk_trackback {
	Etk_Widget* label;
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
		}
	}
	
	
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
		snprintf(buf, PATH_MAX, "%s/%s", event_file->path, event_file->filename);
		etk_label_set(ETK_LABEL(trackback->label), buf);
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
  Etk_Widget* hbox;
  Etk_Widget* button;

  instance = entropy_gui_component_instance_new ();
  trackback = calloc(1, sizeof(entropy_etk_trackback));
  instance->data = trackback;

  hbox = etk_hbox_new(ETK_FALSE,0);
  button = etk_button_new_from_stock(ETK_STOCK_GO_UP);
  etk_signal_connect("pressed", ETK_OBJECT(button), ETK_CALLBACK(_entropy_etk_trackback_up_cb), instance);
  
  trackback->label = etk_label_new("");
  etk_box_pack_start(ETK_BOX(hbox), button, ETK_FALSE,ETK_FALSE,0);
  etk_box_pack_start(ETK_BOX(hbox), trackback->label, ETK_TRUE,ETK_TRUE,0);
  
  
  instance->gui_object = hbox;
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

