#ifndef E_MOD_MAIN_H
#define E_MOD_MAIN_H

#ifdef ENABLE_NLS
# include <libintl.h>
# define D_(string) dgettext(PACKAGE, string)
#else
# define bindtextdomain(domain,dir)
# define bind_textdomain_codeset(domain,codeset)
# define D_(string) (string)
#endif

typedef struct _Config      Config;
typedef struct _Config_Item Config_Item;
typedef struct _Itask      Itask;
typedef struct _Itask_Item Itask_Item;

struct _Config
{
  /* just config state */
  E_Module        *module;
  Eina_List       *instances;
  E_Menu          *menu;
  Eina_List       *items;
};

struct _Config_Item
{
  const char *id;
  int show_label;
  int show_desk;
  int icon_label;
  int skip_dialogs;
  int skip_always_below_windows;
  int swap_on_focus;
  int iconify_focused;
  int ibox_style;
  int max_items;
  int always_group;
  int menu_all_window;
  int hide_menu_button;

  Itask *it;
  E_Config_Dialog *cfd;
};

struct _Itask
{
  E_Gadcon_Client *gcc;
  Config_Item	  *ci;
  Eina_List       *handlers;
  Evas_Object     *o_box;
  Evas_Object     *o_button;
  Eina_List       *items;
  E_Zone          *zone;
  E_Popup         *item_label_popup;
  Itask_Item      *menubutton;
  Ecore_Idle_Enterer *idler;
  
  int item_width;
  int item_height;
  int itask_width;
  int itask_height;
  int module_width;
  int module_height;

  int horizontal;
};


struct _Itask_Item
{
  Itask        *itask;
  Evas_Object *o_holder;
  Evas_Object *o_icon;
  E_Border    *border;
  const char  *class;
  double 	    last_time; // last time this item was activated
  Eina_List   *items;  // better name  needed
  Eina_List   *on_list;  // the list, if the item is on the items list of another item
  int in_bar;
  char *label;
  struct {
    unsigned char  start : 1;
    unsigned char  dnd : 1;
    int            x, y;
    int            dx, dy;
  } drag;

  double time_press;
};



EAPI extern E_Module_Api e_modapi;

EAPI void *e_modapi_init     (E_Module *m);
EAPI int   e_modapi_shutdown (E_Module *m);
EAPI int   e_modapi_save     (E_Module *m);
EAPI int   e_modapi_about    (E_Module *m);

void itask_update(Itask *it);
void itask_reload(Itask *it);

void itask_items_init(Config *itask_config);
void itask_items_shutdown();

void  itask_item_new(Itask *it, E_Border *bd);
void  itask_icon_signal_emit(Itask_Item *ic, char *sig, char *src);
void  itask_item_set_icon(Itask_Item *ic);
void  itask_item_set_label(Itask_Item *ic);
void  itask_item_free(Itask_Item *ic);
int   itask_item_add_check(Itask *it, E_Border *bd);
Itask_Item *itask_item_find(Itask *it, E_Border *bd);

void itask_menu_init(Itask *it);
void itask_menu_button(Itask *it);
void itask_menu_remove(Itask *it);
E_Menu *itask_menu_items_menu(Eina_List *items);

void _itask_config_update(void);
void _config_itask_module(Config_Item *ci);
extern Config *itask_config;
extern char *itask_theme_path;
#endif
