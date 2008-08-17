#include "entropy.h"
#include "entropy_gui.h"
#include "entropy_config.h"
#include <unistd.h>
#include <limits.h>
#include <Etk.h>

#define _INCREMENT 8
#define _MAX_SIZE 250


struct _etk_meta_plugin {
	Etk_Widget* window;
	Etk_Widget* list;
	Etk_Widget* parent_window;
	Ecore_Timer* timer;
	int flip;
	int pos;
};
typedef struct _etk_meta_plugin _etk_meta_plugin;


int
entropy_plugin_type_get ()
{
  return ENTROPY_PLUGIN_GUI_COMPONENT;
}

int
entropy_plugin_sub_type_get()
{
	return ENTROPY_PLUGIN_GUI_COMPONENT_INFO_PROVIDER;
}

char *
entropy_plugin_identify ()
{
  return (char *) "ETK slide metadata plugin";
}

char*
entropy_plugin_toolkit_get() 
{
	return ENTROPY_TOOLKIT_ETK;
}

int _etk_metadata_timer(void* data) {
	entropy_gui_component_instance* instance = (entropy_gui_component_instance*)data;
	_etk_meta_plugin* view = instance->data;
	
	int x,y,w,h;
	int newsize = 0;
	etk_window_geometry_get(ETK_WINDOW(view->window),&x,&y,&w,&h);

	newsize = w + (view->pos);
		etk_window_resize(ETK_WINDOW(view->window), newsize, h);

	if (view->flip) {
		view->flip = 0;
		view->timer = NULL;
		ecore_timer_add(0.02, _etk_metadata_timer, instance);
		
		return 0;
	}

	if (view->pos > 0) {
		if (w<_MAX_SIZE)
			return 1;
		else {
			view->pos = -_INCREMENT;
			view->flip = 1;

			view->timer = NULL;
			
			ecore_timer_add(5, _etk_metadata_timer, instance);
			return 0;
		}
	} else {
		if (w+view->pos > 0)
			return 1;
		else {
			etk_widget_hide_all(view->window);
			view->timer = NULL;
			return 0;
		}
	}

}


void
gui_event_callback (entropy_notify_event * eevent, void *requestor, void *obj,
		    entropy_gui_component_instance * comp)
{
	switch (eevent->event_type) {
		case ENTROPY_NOTIFY_FILE_METADATA_AVAILABLE: {
			_etk_meta_plugin* view = comp->data;								     
			Ecore_List* list = (Ecore_List*)obj;
			Entropy_Metadata_Object* object;
			Etk_Tree_Col *col1, *col2;
			int w,h,x,y;

			col1 = etk_tree_nth_col_get(ETK_TREE(view->list), 0);
			col2 = etk_tree_nth_col_get(ETK_TREE(view->list), 1);

			etk_tree_clear(ETK_TREE(view->list));
			ecore_list_first_goto(list);
			while ( (object = ecore_list_next(list))) {
				etk_tree_row_append(ETK_TREE(view->list), NULL,
						col1, object->key,
						col2, object->value,
						NULL);
				
			}

			view->pos = _INCREMENT;

			/*Set this windows size to be that of the parent*/
			etk_window_geometry_get(ETK_WINDOW(view->parent_window),&x,&y,&w,&h);
			etk_window_move(ETK_WINDOW(view->window), x+w, y);
			etk_widget_size_request_set(ETK_WIDGET(view->window), 2, h);

			etk_widget_show_all(view->window);

			if (view->timer) {
				ecore_timer_del(view->timer);
				view->timer = NULL;
			}

			view->timer = ecore_timer_add(0.02, _etk_metadata_timer, comp);
			printf("Received an ecore_list...\n");
		}
		break;
	}
}


entropy_plugin*
entropy_plugin_init(entropy_core* core)
{
	entropy_plugin* plugin;


	plugin = entropy_malloc(sizeof(Entropy_Plugin_Gui));


	return plugin;
}

entropy_gui_component_instance *
entropy_plugin_gui_instance_new (entropy_core * core,
		     entropy_gui_component_instance * layout, void *data)
{
  Etk_Tree_Col *col_tree;
  _etk_meta_plugin* view = entropy_malloc(sizeof(_etk_meta_plugin));
  entropy_gui_component_instance *instance = entropy_gui_component_instance_new ();
  instance->layout_parent = layout;
  instance->core = core;

  Etk_Tree_Col* tree_col;

  entropy_core_component_event_register (instance,
					 entropy_core_gui_event_get
					 (ENTROPY_GUI_EVENT_FILE_METADATA_AVAILABLE));

  instance->data = view;
  view->parent_window = (Etk_Widget*)layout->gui_object;
  view->window = etk_window_new();
  etk_window_decorated_set(ETK_WINDOW(view->window), ETK_FALSE);
  
  view->list = etk_tree_new();
  etk_tree_mode_set(ETK_TREE(view->list), ETK_TREE_MODE_LIST);
  
  col_tree = etk_tree_col_new(ETK_TREE(view->list), _("Keyword"), 80, 0.0);
	etk_tree_col_model_add(col_tree, etk_tree_model_text_new());
  col_tree = etk_tree_col_new(ETK_TREE(view->list), _("Value"), 200, 0.0);
	etk_tree_col_model_add(col_tree, etk_tree_model_text_new());
  
	etk_tree_build(ETK_TREE(view->list));
  etk_container_add(ETK_CONTAINER(view->window), view->list);
  


  return instance;
}
