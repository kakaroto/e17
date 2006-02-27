#include <Etk.h>
#include "entropy.h"
#include "entropy_gui.h"
#include "etk_location_add_dialog.h"
#include <dlfcn.h>
#include <Ecore.h>
#include <stdlib.h>


//This is here mostly as a demo of the "widget library independence" of entropy, but is on hold
//for the moment until ETK supports more widgets that we need

typedef struct entropy_layout_gui entropy_layout_gui;
struct entropy_layout_gui
{
  entropy_gui_component_instance *iconbox_viewer;
  entropy_gui_component_instance *structure_viewer;
  Etk_Widget *tree;
  Etk_Widget *paned;
  Etk_Widget *statusbar_box;
  Etk_Widget *statusbars[3];

  Ecore_Hash* toplevel_row_entries;

  Etk_Widget* popup;

  Ecore_Hash* config_hash;
};

typedef enum _Etk_Menu_Item_Type
{
   ETK_MENU_ITEM_NORMAL,
   ETK_MENU_ITEM_SEPARATOR
} Etk_Menu_Item_Type;

void layout_etk_simple_add_header(entropy_gui_component_instance* instance, char* name, char* uri);

static Etk_Bool
_etk_window_deleted_cb (Etk_Object * object, void *data)
{
  entropy_core *core = (entropy_core *) data;

  entropy_core_destroy (core);
  etk_main_quit ();
  exit (0);
}

void
entropy_plugin_destroy (entropy_gui_component_instance * comp)
{
  printf ("Destroying layout_etk...\n");


}

static void _etk_layout_row_clicked(Etk_Object *object, 
		Etk_Tree_Row *row, Etk_Event_Mouse_Up_Down *event, void *data)
{
	entropy_gui_component_instance* instance = data;
	entropy_layout_gui* gui = instance->data;	
	
	printf("Button: %d\n", event->button);

	if (event->button == 3) {
		etk_tree_row_select(row);
		etk_menu_popup(ETK_MENU(gui->popup));
	}
	
}

static Etk_Widget *_entropy_etk_menu_item_new(Etk_Menu_Item_Type item_type, const char *label,
   Etk_Stock_Id stock_id, Etk_Menu_Shell *menu_shell, Etk_Widget *statusbar)
{
   Etk_Widget *menu_item = NULL;
   
   switch (item_type)
   {
      case ETK_MENU_ITEM_NORMAL:
         menu_item = etk_menu_item_new_with_label(label);
         break;
      case ETK_MENU_ITEM_SEPARATOR:
         menu_item = etk_menu_separator_new();
         break;
      default:
         return NULL;
   }
   if (stock_id != ETK_STOCK_NO_STOCK)
   {
      Etk_Widget *image;
      
      image = etk_image_new_from_stock(stock_id, ETK_STOCK_SMALL);
      etk_menu_item_image_set(ETK_MENU_ITEM(menu_item), ETK_IMAGE(image));
   }
   etk_menu_shell_append(menu_shell, ETK_MENU_ITEM(menu_item));
   
   /*etk_signal_connect("selected", ETK_OBJECT(menu_item), ETK_CALLBACK(_etk_test_menu_item_selected_cb), statusbar);
   etk_signal_connect("deselected", ETK_OBJECT(menu_item), ETK_CALLBACK(_etk_test_menu_item_deselected_cb), statusbar);*/
   
   return menu_item;
}

void
entropy_plugin_init (entropy_core * core)
{
  /*Init etk */
  etk_init ();
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


void _location_add_cb(Etk_Object *obj, void *data)
{
	printf("Add location\n");
	etk_location_add_dialog_create((entropy_gui_component_instance*)data, layout_etk_simple_add_header);
}

void layout_etk_simple_add_header(entropy_gui_component_instance* instance, char* name, char* uri)
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



  col = etk_tree_nth_col_get(ETK_TREE(gui->tree), 0);
	
  etk_tree_freeze(ETK_TREE(gui->tree));
  row = etk_tree_append(ETK_TREE(gui->tree), col, 
			  etk_theme_icon_theme_get(), "places/start-here_16", _(name), NULL);
  etk_tree_thaw(ETK_TREE(gui->tree));
  
  
  structure = entropy_plugins_type_get_first(ENTROPY_PLUGIN_GUI_COMPONENT,ENTROPY_PLUGIN_GUI_COMPONENT_STRUCTURE_VIEW);
   structure_plugin_init =
      dlsym (structure->dl_ref, "entropy_plugin_init");

   file = entropy_core_parse_uri (uri);
   /*We shouldn't really assume it's a folder - but it bootstraps us for
    * now- FIXME*/
   strcpy(file->mime_type, "file/folder");

  if (!strlen (file->mime_type)) {
	    entropy_mime_file_identify (instance->core->mime_plugins, file);
  }
   
   instance = (*structure_plugin_init)(instance->core, instance, row,file);
   instance->plugin = structure;
}

entropy_gui_component_instance *
entropy_plugin_layout_create (entropy_core * core)
{
  Etk_Widget *window;
  entropy_layout_gui *gui;
  entropy_gui_component_instance *layout;
  entropy_gui_component_instance* instance;

  void *(*local_plugin_init) (entropy_core * core,
				  entropy_gui_component_instance *,
				  void *data);
  
  entropy_plugin *local;
  Etk_Tree_Col* col;
  Etk_Widget* vbox;
  Etk_Widget* menubar;
  Etk_Widget* menu_item;
  Etk_Widget* menu;


  Ecore_List* config_hash_keys;
  char *tmp,*key;

  /*Entropy related init */
  layout = entropy_malloc (sizeof (entropy_gui_component_instance));	/*Create a component instance */
  gui = entropy_malloc (sizeof (entropy_layout_gui));
  layout->data = gui;
  layout->core = core;

  /*Register this layout container with the core, so our children can get events */
  entropy_core_layout_register (core, layout);


  /*Etk related init */
  window = etk_window_new ();
  gui->paned = etk_hpaned_new();

  etk_signal_connect ("delete_event", ETK_OBJECT (window),
		      ETK_CALLBACK (_etk_window_deleted_cb), core);

  etk_window_title_set(ETK_WINDOW(window), "Entropy");
  etk_window_wmclass_set(ETK_WINDOW(window), "entropy", "Entropy");

  vbox = etk_vbox_new(ETK_FALSE,0);
  etk_container_add(ETK_CONTAINER(window), vbox);

  /*Menu setup*/
  menubar = etk_menu_bar_new();
  menu_item = _entropy_etk_menu_item_new(ETK_MENU_ITEM_NORMAL, _("File"), ETK_STOCK_NO_STOCK, ETK_MENU_SHELL(menubar), NULL);
  menu = etk_menu_new();
  etk_menu_item_submenu_set(ETK_MENU_ITEM(menu_item), ETK_MENU(menu));
  _entropy_etk_menu_item_new(ETK_MENU_ITEM_NORMAL, _("Exit"), ETK_STOCK_SYSTEM_SHUTDOWN, ETK_MENU_SHELL(menu), NULL);
  
  menu_item = _entropy_etk_menu_item_new(ETK_MENU_ITEM_NORMAL, _("Edit"), ETK_STOCK_NO_STOCK, ETK_MENU_SHELL(menubar), NULL);
  menu = etk_menu_new();
  etk_menu_item_submenu_set(ETK_MENU_ITEM(menu_item), ETK_MENU(menu));
  _entropy_etk_menu_item_new(ETK_MENU_ITEM_NORMAL, _("Cut"), ETK_STOCK_EDIT_CUT, ETK_MENU_SHELL(menu), NULL);
  _entropy_etk_menu_item_new(ETK_MENU_ITEM_NORMAL, _("Copy"), ETK_STOCK_EDIT_COPY, ETK_MENU_SHELL(menu), NULL);
  _entropy_etk_menu_item_new(ETK_MENU_ITEM_NORMAL, _("Paste"), ETK_STOCK_EDIT_PASTE, ETK_MENU_SHELL(menu), NULL);
  
  
  menu_item = _entropy_etk_menu_item_new(ETK_MENU_ITEM_NORMAL, _("Tools"), ETK_STOCK_NO_STOCK, ETK_MENU_SHELL(menubar), NULL);
  menu = etk_menu_new();
  etk_menu_item_submenu_set(ETK_MENU_ITEM(menu_item), ETK_MENU(menu));
  
  menu_item = _entropy_etk_menu_item_new(ETK_MENU_ITEM_NORMAL, _("Add Location"), ETK_STOCK_ADDRESS_BOOK_NEW, ETK_MENU_SHELL(menu), NULL);
  etk_signal_connect("activated", ETK_OBJECT(menu_item), ETK_CALLBACK(_location_add_cb), layout);
  
  _entropy_etk_menu_item_new(ETK_MENU_ITEM_NORMAL, _("Program Associations.."), ETK_STOCK_SYSTEM_SHUTDOWN, ETK_MENU_SHELL(menu), NULL);

  
  menu_item = _entropy_etk_menu_item_new(ETK_MENU_ITEM_NORMAL, _("View"), ETK_STOCK_NO_STOCK, ETK_MENU_SHELL(menubar), NULL);
  menu = etk_menu_new();
  etk_menu_item_submenu_set(ETK_MENU_ITEM(menu_item), ETK_MENU(menu));
  _entropy_etk_menu_item_new(ETK_MENU_ITEM_NORMAL, _("Tree View"), ETK_STOCK_SYSTEM_SHUTDOWN, ETK_MENU_SHELL(menu), NULL);
  _entropy_etk_menu_item_new(ETK_MENU_ITEM_NORMAL, _("List View"), ETK_STOCK_SYSTEM_SHUTDOWN, ETK_MENU_SHELL(menu), NULL);
  _entropy_etk_menu_item_new(ETK_MENU_ITEM_NORMAL, _("Icon View"), ETK_STOCK_SYSTEM_SHUTDOWN, ETK_MENU_SHELL(menu), NULL);

  
  menu_item = _entropy_etk_menu_item_new(ETK_MENU_ITEM_NORMAL, _("Debug"), ETK_STOCK_NO_STOCK, ETK_MENU_SHELL(menubar), NULL); 
  menu = etk_menu_new();
  etk_menu_item_submenu_set(ETK_MENU_ITEM(menu_item), ETK_MENU(menu));
  _entropy_etk_menu_item_new(ETK_MENU_ITEM_NORMAL, _("File Cache"), ETK_STOCK_SYSTEM_SHUTDOWN, ETK_MENU_SHELL(menu), NULL);
  
  menu_item = _entropy_etk_menu_item_new(ETK_MENU_ITEM_NORMAL, _("Help"), ETK_STOCK_NO_STOCK, ETK_MENU_SHELL(menubar), NULL);
  menu = etk_menu_new();
  etk_menu_item_submenu_set(ETK_MENU_ITEM(menu_item), ETK_MENU(menu));
  _entropy_etk_menu_item_new(ETK_MENU_ITEM_NORMAL, _("About.."), ETK_STOCK_SYSTEM_SHUTDOWN, ETK_MENU_SHELL(menu), NULL);


  etk_box_pack_start(ETK_BOX(vbox), menubar, ETK_FALSE, ETK_FALSE, 0);
  etk_box_pack_start(ETK_BOX(vbox), gui->paned, TRUE, TRUE, 0);

  //etk_widget_size_request_set(ETK_WIDGET(window), 800,600);

  /*Tree init*/
  gui->tree = etk_tree_new();
  etk_paned_add1(ETK_PANED(gui->paned), gui->tree, ETK_FALSE);
  etk_tree_mode_set(ETK_TREE(gui->tree), ETK_TREE_MODE_TREE);
  col = etk_tree_col_new(ETK_TREE(gui->tree), _("Folders"), 
		  etk_tree_model_icon_text_new(ETK_TREE(gui->tree), ETK_TREE_FROM_EDJE), 60);
  
  etk_tree_col_expand_set(col, ETK_TRUE);
  etk_tree_build(ETK_TREE(gui->tree));

  etk_widget_size_request_set(gui->tree, 180, 600);

  /*Popup init*/
   gui->popup = etk_menu_new();
   etk_signal_connect("row_clicked", ETK_OBJECT( gui->tree  ),
          ETK_CALLBACK(_etk_layout_row_clicked), layout);

   _entropy_etk_menu_item_new(ETK_MENU_ITEM_NORMAL, _("Delete this entry"), ETK_STOCK_DOCUMENT_OPEN, ETK_MENU_SHELL(gui->popup),NULL);

   /*Make the toplevel row hash*/
   gui->toplevel_row_entries = ecore_hash_new(ecore_direct_hash, ecore_direct_compare);


  /*Config load*/
  if (!(tmp = entropy_config_str_get ("layout_ewl_simple", "structure_bar"))) {
    entropy_config_standard_structures_create ();
    tmp = entropy_config_str_get ("layout_ewl_simple", "structure_bar");
  }

  printf ("Config for layout is: '%s' (%d)\n", tmp, strlen (tmp));
  
  gui->config_hash = entropy_config_standard_structures_parse (layout, tmp);
  config_hash_keys = ecore_hash_keys(gui->config_hash);
  while ( (key = ecore_list_remove_first(config_hash_keys))) {
	  char* uri = ecore_hash_get(gui->config_hash, key);
	  layout_etk_simple_add_header (layout, key, uri);
	  
	  free(key);
	  free(uri);
  }
  ecore_list_destroy(config_hash_keys);
  
  entropy_free (tmp);
  

  /*Initialise the list view*/
  local = entropy_plugins_type_get_first(ENTROPY_PLUGIN_GUI_COMPONENT,ENTROPY_PLUGIN_GUI_COMPONENT_LOCAL_VIEW);
  local_plugin_init =
      dlsym (local->dl_ref, "entropy_plugin_init");   
  instance = (*local_plugin_init)(core, layout,NULL);
  instance->plugin = local;

  etk_paned_add2(ETK_PANED(gui->paned), instance->gui_object, ETK_TRUE);

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

  return layout;
}
