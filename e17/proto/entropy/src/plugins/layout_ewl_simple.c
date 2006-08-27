#include <Ewl.h>
#include "entropy.h"
#include "entropy_gui.h"
//#include "ewl_location_add_dialog.h"
#include "ewl_progress_dialog.h"
#include "ewl_properties_dialog.h"
#include "ewl_user_interaction_dialog.h"
#include "ewl_mime_dialog.h"
//#include "ewl_file_cache_dialog.h"
//#include "entropy_ewl_context_menu.h"
#include <dlfcn.h>
#include <Ecore.h>
#include <stdlib.h>

/* TODO:
 *
 * 1. Add a shareable context menu
 * 2. Add a menubar/status bar
 * 3. Allow switching between icon/list view
 * 4. Modify ewl_structure_viewer.c to return a tree row - and append to 
 *    tree in layout_ewl_simple.c
 * 5. Add an ewl_trackback plugin
 * 6. Add an ewl_metadata plugin
 *
 */

#define ENTROPY_EWL_WINDOW_WIDTH 640
#define ENTROPY_EWL_WINDOW_HEIGHT 480
#define ENTROPY_EWL_WINDOW_PANE_DEFAULT_X 150

static int _ewl_layout_window_count = 0;

static Ecore_Hash *_ewl_layout_structure_plugin_reference;
typedef struct entropy_layout_gui entropy_layout_gui;
struct entropy_layout_gui
{
  entropy_gui_component_instance *iconbox_viewer;
  entropy_gui_component_instance *list_viewer;
  entropy_gui_component_instance *structure_viewer;
  entropy_gui_component_instance *trackback;

  Ewl_Widget *tree;
  Ewl_Widget *paned;
  Ewl_Widget *statusbar_box;
  Ewl_Widget *statusbars[3];
  Ewl_Widget *delete_row;
  Ewl_Widget* popup;
  Ewl_Widget* localshell;
  Ecore_Hash* progress_hash;
  Ecore_Hash* properties_request_hash;

};

typedef enum _Ewl_Menu_Item_Type
{
   EWL_MENU_ITEM_NORMAL,
   EWL_MENU_ITEM_SEPARATOR
} Ewl_Menu_Item_Type;

void layout_ewl_simple_add_header(entropy_gui_component_instance *instance, Entropy_Config_Structure *structure);
void entropy_plugin_layout_main ();
char* entropy_plugin_toolkit_get();
entropy_gui_component_instance *entropy_plugin_layout_create (entropy_core *core);
void entropy_ewl_layout_trackback_cb(Ewl_Widget* w, void *event, void* data);
void entropy_layout_ewl_simple_local_view_set(entropy_gui_component_instance *instance,
                entropy_gui_component_instance *local);

void layout_ewl_simple_quit(entropy_core *core)
{
 entropy_core_destroy(core);
 ewl_main_quit();
}

static void _ewl_window_delete_cb(Ewl_Widget *w, void *event, void *data)
{
 entropy_gui_component_instance *instance = data;

 _ewl_layout_window_count--;

 ewl_widget_destroy(EWL_WIDGET(instance->gui_object));

 if (_ewl_layout_window_count == 0)
 {
  layout_ewl_simple_quit(instance->core);
 }
}

static Ewl_Widget *
_entropy_ewl_menu_item_new(Ewl_Menu_Item_Type item_type, const char *label,
			   char *stock, Ewl_Widget *menu, Ewl_Widget *statusbar)
{
 Ewl_Widget *menu_item = NULL;

 if (!menu) return NULL;
 
 switch (item_type)
 {
  case EWL_MENU_ITEM_NORMAL:
   menu_item = ewl_menu_item_new();
   if (stock != NULL) ewl_button_image_set(EWL_BUTTON(menu_item), stock, NULL);
   ewl_button_label_set(EWL_BUTTON(menu_item), label);
   break;
  case EWL_MENU_ITEM_SEPARATOR:
   menu_item = ewl_hseparator_new();
   break;
  default:
   return NULL;
 }
 ewl_container_child_append(EWL_CONTAINER(menu), menu_item);
 return menu_item;
}

static Ewl_Widget *
_entropy_ewl_menu_check_item_new(const char *label, Ewl_Widget *menu)
{
 Ewl_Widget *menu_item;
 
 if (!menu) return NULL;

 menu_item = ewl_checkbutton_new();
 ewl_button_label_set(EWL_BUTTON(menu_item), label);
 ewl_container_child_append(EWL_CONTAINER(menu), menu_item);

 return menu_item;
}

static Ewl_Widget *
_entropy_ewl_menu_radio_item_new(const char *label, Ewl_Widget *group_radio, 
				 Ewl_Widget *menu)
{
 Ewl_Widget *menu_item;

 if (!menu) return NULL;

 menu_item = ewl_radiobutton_new();
 ewl_button_label_set(EWL_BUTTON(menu_item), label);
 ewl_radiobutton_chain_set(EWL_RADIOBUTTON(group_radio), EWL_RADIOBUTTON(menu_item));
 ewl_container_child_append(EWL_CONTAINER(menu), menu_item);

 return menu_item;
}
	
Entropy_Plugin *
entropy_plugin_init(entropy_core *core)
{
 Entropy_Plugin_Gui *plugin;
 Entropy_Plugin *base;

 ewl_init(0, NULL);
 
 plugin = entropy_malloc(sizeof(Entropy_Plugin_Gui));
 base = ENTROPY_PLUGIN(plugin);
 
 base->functions.entropy_plugin_init = &entropy_plugin_init;
 plugin->gui_functions.layout_main = &entropy_plugin_layout_main;
 plugin->gui_functions.layout_create = &entropy_plugin_layout_create;
 plugin->gui_functions.toolkit_get = &entropy_plugin_toolkit_get;

 return base;
}

char *
entropy_plugin_identify()
{
 return (char *)"ewl";
}

int
entropy_plugin_type_get()
{
 return ENTROPY_PLUGIN_GUI_LAYOUT;
}

char *
entropy_plugin_toolkit_get()
{
 return ENTROPY_TOOLKIT_EWL;
}

void
entropy_plugin_layout_main()
{
 printf("Init Ewl...\n");
 ewl_main();
}

void
entropy_plugin_destroy (entropy_gui_component_instance * comp)
{
  if (!comp) {
	printf("*** No plugin to destroy at layout_ewl_simple.c\n");
  	return;
  }

  /*TODO: put config save code, etc - here*/
}

void
ewl_layout_simple_exit_cb(Ewl_Widget *w, void *event, void *data)
{
 entropy_gui_component_instance *instance = data;
 layout_ewl_simple_quit(instance->core);
}

void
gui_event_callback (entropy_notify_event * eevent, void *requestor,
		    void *el, entropy_gui_component_instance * comp)
{
	printf("STUB: layout_ewl_simple gui_event_callback\n");
}

entropy_gui_component_instance *
entropy_plugin_layout_create (entropy_core * core)
{
 Ewl_Widget *win;
 entropy_layout_gui *gui;
 entropy_gui_component_instance *layout;
 entropy_gui_component_instance *instance = NULL;

 void *(*local_plugin_init) (entropy_core *core,
				entropy_gui_component_instance *,
				void *data);

 entropy_plugin *local;

 entropy_gui_component_instance *(*metadata_plugin_init)
				 (entropy_core *core,
				  entropy_gui_component_instance *,
				  void *parent_visual,
				  void *data);


 entropy_gui_component_instance *(*structure_plugin_instance_new)
				 (entropy_core *core,
				  entropy_gui_component_instance *,
				  void *parent_visual,
				  void *data);

 entropy_plugin *meta;
 entropy_plugin *trackback;
 entropy_plugin *structure_p;
 entropy_gui_component_instance *meta_instance;

 Ewl_Widget *tree;
 Ewl_Widget *vbox;
 Ewl_Widget *menubar;
 Ewl_Widget *menu;
 Ewl_Widget *menu_item;
 char *header[1];

 header[0] = "Folders";
 header[1] = NULL;
 
 Ecore_List *structures;
 Entropy_Config_Structure *structure;

 layout = (entropy_gui_component_instance *)entropy_gui_component_instance_layout_new();
 gui = entropy_malloc(sizeof(entropy_layout_gui));
 layout->data = gui;
 layout->core = core;
 gui->progress_hash = ecore_hash_new(ecore_direct_hash, ecore_direct_compare);
 gui->properties_request_hash = ecore_hash_new(ecore_direct_hash, ecore_direct_compare);

 entropy_plugin_filesystem_metadata_groups_get(layout);

 entropy_core_layout_register(core, layout);

 entropy_core_component_event_register (layout,
                                        entropy_core_gui_event_get
                                        (ENTROPY_GUI_EVENT_FILE_PROGRESS));

 entropy_core_component_event_register (layout,
                                        entropy_core_gui_event_get
                                        (ENTROPY_GUI_EVENT_USER_INTERACTION_YES_NO_ABORT));

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

 win = ewl_window_new();
 layout->gui_object = win;
 ewl_window_title_set(EWL_WINDOW(win), "Entropy");
 ewl_window_name_set(EWL_WINDOW(win), "Entropy");
 ewl_object_size_request(EWL_OBJECT(win), ENTROPY_EWL_WINDOW_WIDTH, 
					  ENTROPY_EWL_WINDOW_HEIGHT);
 ewl_callback_append(win, EWL_CALLBACK_DELETE_WINDOW, _ewl_window_delete_cb, layout);

 vbox = ewl_vbox_new();
 ewl_container_child_append(EWL_CONTAINER(win), vbox);
 ewl_object_fill_policy_set(EWL_OBJECT(vbox), EWL_FLAG_FILL_ALL);
 
 menubar = ewl_menubar_new();
 ewl_container_child_append(EWL_CONTAINER(vbox), menubar);
 ewl_object_fill_policy_set(EWL_OBJECT(menubar), EWL_FLAG_FILL_HFILL);
 ewl_widget_show(menubar);
 
 menu = ewl_menu_new();
 ewl_button_label_set(EWL_BUTTON(menu), "File");
 ewl_container_child_append(EWL_CONTAINER(menubar), menu);
 ewl_object_fill_policy_set(EWL_OBJECT(menu), EWL_FLAG_FILL_NONE);
 ewl_widget_show(menu);

 menu_item = _entropy_ewl_menu_item_new(EWL_MENU_ITEM_NORMAL, "Exit", NULL, menu, NULL);
 ewl_callback_append(menu_item, EWL_CALLBACK_CLICKED, ewl_layout_simple_exit_cb, layout);
 ewl_widget_show(menu_item);

 menu = ewl_menu_new();
 ewl_button_label_set(EWL_BUTTON(menu), "View");
 ewl_container_child_append(EWL_CONTAINER(menubar), menu);
 ewl_object_fill_policy_set(EWL_OBJECT(menu), EWL_FLAG_FILL_NONE);
 ewl_widget_show(menu);

 menu_item = _entropy_ewl_menu_check_item_new("Default", menu);
 ewl_widget_show(menu_item);
 
 gui->paned = ewl_hpaned_new();
 ewl_container_child_append(EWL_CONTAINER(vbox), gui->paned);

 gui->tree = ewl_tree_new(1);
 ewl_container_child_append(EWL_CONTAINER(gui->paned), gui->tree);
 ewl_object_size_request(EWL_OBJECT(gui->tree), ENTROPY_EWL_WINDOW_PANE_DEFAULT_X, 50);
 ewl_tree_headers_visible_set(EWL_TREE(gui->tree), TRUE);
 ewl_tree_headers_set(EWL_TREE(gui->tree), header);
 
 gui->localshell = ewl_vbox_new();
 ewl_container_child_append(EWL_CONTAINER(gui->paned), gui->localshell);
 
 gui->popup = ewl_menu_new();
 menu_item = ewl_menu_item_new();
 menu_item = ewl_menu_item_new();

 local = entropy_plugin_gui_get_by_name_toolkit(ENTROPY_TOOLKIT_EWL, "listviewer");

 if (local)
 {
  local_plugin_init = dlsym(local->dl_ref, "entropy_plugin_gui_instance_new");
  instance = (*local_plugin_init)(core, layout, NULL);
  instance->plugin = local;
  gui->list_viewer = instance;

  /*FIXME: this needs to be menu selectable*/
  ewl_container_child_append(EWL_CONTAINER(gui->localshell), gui->list_viewer->gui_object);
  printf("Found listviewer..\n");
 }

 local = entropy_plugin_gui_get_by_name_toolkit(ENTROPY_TOOLKIT_EWL, "iconviewer");
 if (local) 
 {
  local_plugin_init = dlsym(local->dl_ref, "entropy_plugin_gui_instance_new");
  gui->iconbox_viewer = (*local_plugin_init)(core, layout, NULL);
  gui->iconbox_viewer->plugin = local;
  gui->iconbox_viewer->active=0;
  printf("Found listviewer..\n");
 }

 meta = entropy_plugins_type_get_first(ENTROPY_PLUGIN_GUI_COMPONENT, 
 		                       ENTROPY_PLUGIN_GUI_COMPONENT_INFO_PROVIDER);
 if (meta)
 {
  metadata_plugin_init = dlsym(meta->dl_ref, "entropy_plugin_gui_instance_new");
  meta_instance = (*metadata_plugin_init)(core, layout, layout->gui_object, NULL);
  meta_instance->plugin = meta;
 }

 trackback = entropy_plugin_gui_get_by_name_toolkit(ENTROPY_TOOLKIT_EWL, "trackback");

 if (trackback)
 {
  local_plugin_init = dlsym(trackback->dl_ref, "entropy_plugin_gui_instance_new");
  gui->trackback = (*local_plugin_init)(core, layout, NULL);
  gui->trackback->plugin = trackback;
  gui->trackback->active=1;
 }

 /*Add an example row to the tree*/
 structure_p = entropy_plugin_gui_get_by_name_toolkit(ENTROPY_TOOLKIT_EWL, "structureviewer");
 if (structure_p) {
   Entropy_Generic_File* file = entropy_generic_file_new();
   snprintf(file->uri_base,sizeof(file->uri_base),"file");
   snprintf(file->filename,sizeof(file->filename),"/");
   file->filetype = FILE_FOLDER;
   strcpy(file->mime_type, "file/folder");

	 
   printf("Found structure plugin");
   structure_plugin_instance_new = dlsym(structure_p->dl_ref, "entropy_plugin_gui_instance_new");
   instance = (*structure_plugin_instance_new)(core, layout, gui->tree, file); 
   instance->plugin = structure_p;
   
 }
 

 ewl_widget_show(win);
 ewl_widget_show(vbox);
 ewl_widget_show(gui->paned);
 ewl_widget_show(gui->tree);
 ewl_widget_show(gui->localshell);

 _ewl_layout_window_count++;

 return layout;
}
