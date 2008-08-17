#include "entropy.h"
#include "entropy_gui.h"
#include <dlfcn.h>
#include <strings.h>
#include <limits.h>
#include <time.h>
#include <Etk.h>
#include <Emotion.h>
#include "etk_directory_add_dialog.h"
#include "etk_properties_dialog.h"
#include "entropy_etk_context_menu.h"

Entropy_Plugin* entropy_plugin_init (entropy_core * core);

entropy_gui_component_instance * 
entropy_plugin_gui_instance_new (entropy_core * core, entropy_gui_component_instance * 
		layout, void *data);

static Evas_Object* emo = NULL;
static int play=0;
int
entropy_plugin_type_get ();
int
entropy_plugin_sub_type_get ();
char *
entropy_plugin_identify ();
char*
entropy_plugin_toolkit_get() ;


typedef struct {
	Etk_Widget* window;
	Etk_Widget* canvas;

	Etk_Widget* filename;
	Etk_Widget* size;
	Etk_Widget* type;
} etk_hover_plugin;


/*------------- boilerplate -----*/
int
entropy_plugin_type_get ()
{
  return ENTROPY_PLUGIN_GUI_COMPONENT;
}

int
entropy_plugin_sub_type_get ()
{
  return ENTROPY_PLUGIN_GUI_COMPONENT_HOVER_PROVIDER;
}

char *
entropy_plugin_identify ()
{
  return (char *) "hoverviewer";
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
  etk_hover_plugin* viewer = comp->data;
	
  switch (eevent->event_type) {
	  case ENTROPY_NOTIFY_HOVER: {
		char buf[PATH_MAX];
		entropy_generic_file* file = (entropy_generic_file*)el;

		if (play) return;
		play=1;

		etk_window_move(ETK_WINDOW(viewer->window), eevent->key+10, eevent->hints+10);
		etk_widget_show_all(ETK_WIDGET(viewer->window));
		snprintf(buf,sizeof(buf),"%s/%s", file->path,file->filename);

		if (strstr(file->uri_base, "file") == file->uri_base) {
			if (strstr(file->mime_type, "video/")) {
				emo = emotion_object_add( evas_object_evas_get(((Etk_Canvas*)viewer->canvas)->clip));
				emotion_object_init(emo, "emotion_decoder_xine.so");
				emotion_object_file_set(emo, buf);
				emotion_object_play_set(emo, 1);
				evas_object_move(emo, 0, 0);
				evas_object_resize(emo, 100, 80);
				emotion_object_smooth_scale_set(emo, 1);
				evas_object_show(emo);
				etk_canvas_object_add(ETK_CANVAS(viewer->canvas), emo);
			} else if (strstr(file->mime_type, "image/")) {
				emo = evas_object_image_add(evas_object_evas_get(((Etk_Canvas*)viewer->canvas)->clip));
				evas_object_image_file_set(emo, buf, NULL);
				evas_object_show(emo);
				etk_canvas_object_add(ETK_CANVAS(viewer->canvas), emo);
			}
		} 

		etk_entry_text_set(ETK_ENTRY(viewer->filename), buf);
		if (file->retrieved_stat) {
			snprintf(buf,sizeof(buf),"%lld KB", file->properties.st_size / 1024);
			etk_entry_text_set(ETK_ENTRY(viewer->size), buf);
		}
		etk_entry_text_set(ETK_ENTRY(viewer->type), file->mime_type);
		

		break;
	  }
          break;

	  case ENTROPY_NOTIFY_DEHOVER: {
	  	entropy_generic_file* file = (entropy_generic_file*)el;
		
		
		printf("Dehover for: %s/%s\n", file->path, file->filename);

		if (emo) {
			etk_canvas_object_remove(ETK_CANVAS(viewer->canvas), emo);
			evas_object_del(emo);
			emo= NULL;
		}
		play = 0;
		etk_widget_hide_all(ETK_WIDGET(viewer->window));
	  }
	  break;
  }
}

entropy_gui_component_instance *
entropy_plugin_gui_instance_new (entropy_core * core,
		     entropy_gui_component_instance * layout, void *data)
{	
  entropy_gui_component_instance *instance;	
  etk_hover_plugin* viewer;
  Etk_Widget *vbox, *table;
  Etk_Widget* label;

  viewer = calloc(1, sizeof(etk_hover_plugin));
  instance = entropy_gui_component_instance_new ();
  instance->data = viewer;
  instance->layout_parent = layout;

  viewer->window = etk_widget_new(ETK_WINDOW_TYPE, "theme-group", "tooltip", "decorated", ETK_FALSE, "skip-taskbar", ETK_TRUE, "skip-pager", ETK_TRUE, NULL);
  viewer->canvas = etk_canvas_new();

  vbox = etk_vbox_new(ETK_FALSE,0);
  etk_container_add(ETK_CONTAINER(viewer->window), vbox);
 

  etk_box_append(ETK_BOX(vbox), viewer->canvas, ETK_BOX_START, ETK_BOX_EXPAND_FILL, 0);

  table = etk_table_new(2, 3, ETK_FALSE);
  etk_box_append(ETK_BOX(vbox), table, ETK_BOX_START, ETK_BOX_NONE, 0);

  label = etk_label_new("Filename");
  viewer->filename = etk_entry_new();
  etk_table_attach_default(ETK_TABLE(table),
                            label,
                            0, 0, 0, 0);
   etk_table_attach_default(ETK_TABLE(table),
                            viewer->filename,
                            1, 1, 0, 0);

  

  label = etk_label_new("Size");
  viewer->size = etk_entry_new();
  etk_table_attach_default(ETK_TABLE(table),
                            label,
                            0, 0, 1, 1);
   etk_table_attach_default(ETK_TABLE(table),
                            viewer->size,
                            1, 1, 1, 1);

  label = etk_label_new("Type");
  viewer->type = etk_entry_new();
  etk_table_attach_default(ETK_TABLE(table),
                            label,
                            0, 0, 2, 2);
   etk_table_attach_default(ETK_TABLE(table),
                            viewer->type,
                            1, 1, 2, 2);
   
  etk_window_resize(ETK_WINDOW(viewer->window), 200,150);
  etk_window_stacking_set(ETK_WINDOW(viewer->window), ETK_WINDOW_ABOVE);
  

  entropy_core_component_event_register (instance,
					 entropy_core_gui_event_get
					 (ENTROPY_GUI_EVENT_HOVER));

  entropy_core_component_event_register (instance,
					 entropy_core_gui_event_get
					 (ENTROPY_GUI_EVENT_DEHOVER));


  return instance;
}
