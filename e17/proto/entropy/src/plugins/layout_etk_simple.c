#include <Etk.h>
#include "entropy.h"
#include "entropy_gui.h"
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
};

typedef enum _Etk_Menu_Item_Type
{
   ETK_MENU_ITEM_NORMAL,
   ETK_MENU_ITEM_SEPARATOR
} Etk_Menu_Item_Type;

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


/*------------------------------------------------*/
/*Config handlers*/


Ecore_Hash *
layout_ewl_simple_parse_config (entropy_gui_component_instance * instance,
				char *config)
{
  Ecore_Hash *ret = ecore_hash_new (ecore_str_hash, ecore_str_compare);
  char *last;

  if (!strstr (config, "|")) {
    char *name;
    char *uri;

    printf ("Simple case - only one object...\n");

    name = strtok (config, ";");
    uri = strtok (NULL, ";");

    printf ("Name/uri is %s %s\n", name, uri);

    /*Cut the obj up by semicolon; */

  }
  else {
    Ecore_List *objects = ecore_list_new ();
    char *object;
    char *name;
    char *uri;

    printf ("Complex case, multiple objects...\n");

    object = strtok (config, "|");
    ecore_list_append (objects, strdup (object));
    while ((object = strtok (NULL, "|"))) {
      ecore_list_append (objects, strdup (object));
    }

    ecore_list_goto_first (objects);
    while ((object = ecore_list_next (objects))) {
      name = strtok (object, ";");
      uri = strtok (NULL, ";");

      printf ("Name/uri is %s %s\n", name, uri);

      free (object);
    }
    ecore_list_destroy (objects);



  }

  return ret;
}

/*------------------------------------------------*/


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

entropy_gui_component_instance *
entropy_plugin_layout_create (entropy_core * core)
{
  Etk_Widget *window;
  entropy_layout_gui *gui;
  entropy_gui_component_instance *layout;
  entropy_gui_component_instance* instance;
  Etk_Widget* test;
  

  void *(*entropy_plugin_init) (entropy_core * core,
				entropy_gui_component_instance *);
  void *(*structure_plugin_init) (entropy_core * core,
				  entropy_gui_component_instance *,
				  void* parent_visual,
				  void *data);

  void *(*local_plugin_init) (entropy_core * core,
				  entropy_gui_component_instance *,
				  void *data);
  
  entropy_plugin *plugin;
  entropy_plugin *structure;
  entropy_plugin *local;
  entropy_generic_file* file;
  Etk_Tree_Col* col;
  Etk_Tree_Row* row;
  Etk_Widget* vbox;
  Etk_Widget* menubar;
  Etk_Widget* menu_item;
  Etk_Widget* menu;
  char* home = strdup(getenv("HOME"));
  char* pos;


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
  _entropy_etk_menu_item_new(ETK_MENU_ITEM_NORMAL, _("Add Location"), ETK_STOCK_ADDRESS_BOOK_NEW, ETK_MENU_SHELL(menu), NULL);
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
  etk_box_pack_end(ETK_BOX(vbox), gui->paned, TRUE, TRUE, 0);

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

  /*Add test row */
  etk_tree_freeze(ETK_TREE(gui->tree));
  row = etk_tree_append(ETK_TREE(gui->tree), col, 
			  etk_theme_icon_theme_get(), "places/start-here_16", _("Home Directory"), NULL);
  etk_tree_thaw(ETK_TREE(gui->tree));
  
  
  structure = entropy_plugins_type_get_first(ENTROPY_PLUGIN_GUI_COMPONENT,ENTROPY_PLUGIN_GUI_COMPONENT_STRUCTURE_VIEW);
   structure_plugin_init =
      dlsym (structure->dl_ref, "entropy_plugin_init");

   file = entropy_generic_file_new();

   pos = strrchr(home, '/');
   *pos = '\0';
   pos++;

   strcpy(file->path, home);
   strcpy(file->filename, pos);
   strcpy(file->uri_base, "posix");
   strcpy(file->mime_type, "file/folder");


   instance = (*structure_plugin_init)(core, layout, row,file);
   instance->plugin = structure;
   /*-----------------------*/

  /*Add test row */
  etk_tree_freeze(ETK_TREE(gui->tree));
  row = etk_tree_append(ETK_TREE(gui->tree), col, 
			  etk_theme_icon_theme_get(), "places/start-here_16", _("Filesystem Root"), NULL);
  etk_tree_thaw(ETK_TREE(gui->tree));
  
  
  structure = entropy_plugins_type_get_first(ENTROPY_PLUGIN_GUI_COMPONENT,ENTROPY_PLUGIN_GUI_COMPONENT_STRUCTURE_VIEW);
   structure_plugin_init =
      dlsym (structure->dl_ref, "entropy_plugin_init");

   file = entropy_generic_file_new();
   
   strcpy(file->path, "/");
   strcpy(file->filename, "/");
   strcpy(file->uri_base, "posix");
   strcpy(file->mime_type, "file/folder");

   instance = (*structure_plugin_init)(core, layout, row,file);
   instance->plugin = structure;
   /*-----------------------*/

  local = entropy_plugins_type_get_first(ENTROPY_PLUGIN_GUI_COMPONENT,ENTROPY_PLUGIN_GUI_COMPONENT_LOCAL_VIEW);
  local_plugin_init =
      dlsym (local->dl_ref, "entropy_plugin_init");   
  instance = (*local_plugin_init)(core, layout,file);
  instance->plugin = local;

  etk_paned_add2(ETK_PANED(gui->paned), instance->gui_object, ETK_TRUE);

  
  etk_widget_show_all (window);





  return layout;
}
