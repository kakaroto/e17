#ifndef _ECLAIR_MENU_H_
#define _ECLAIR_MENU_H_

#include <Evas.h>
#include <Ecore.h>
#include <Ecore_Evas.h>
#include "eclair_types.h"

struct _Eclair_Menu
{
   Eclair_Menu_Item *parent;
   Eina_List *items;
   Eclair_Engine engine;
   Ecore_Evas *window;
   Ecore_X_Window x_window;
   Evas *evas;
   Evas_Object *edje_object;
};

enum _Eclair_Menu_Item_Type
{
   ECLAIR_MENU_ITEM_NORMAL_ITEM,
   ECLAIR_MENU_ITEM_ICON_ITEM,
   ECLAIR_MENU_ITEM_CHECK_ITEM,
   ECLAIR_MENU_ITEM_SEPARATOR_ITEM
};

struct _Eclair_Menu_Item
{
   Eclair_Menu *parent;
   Eclair_Menu *child;
   char *label;
   Evas_Object *edje_object;
   Evas_Object *icon_object;
   int height;
   Eclair_Menu_Item_Type type;
   void (*item_cb)(Eclair_Menu_Item *item, void *data);
   void *cb_data;
   int is_active : 1;
};

Eclair_Menu *eclair_menu_new(Eclair_Engine menu_engine);
void eclair_menu_free(Eclair_Menu *menu);
int eclair_menu_append_item(Eclair_Menu *menu, Eclair_Menu_Item *item);
void eclair_menu_attach_to_item(Eclair_Menu *menu, Eclair_Menu_Item *item);
void eclair_menu_popup(Eclair_Menu *menu);
void eclair_menu_popup_at_xy(Eclair_Menu *menu, int x, int y);
void eclair_menu_pop_down(Eclair_Menu *menu);

Eclair_Menu_Item *eclair_menu_add_item(Eclair_Menu *menu, const char *label);
Eclair_Menu_Item *eclair_menu_add_item_with_icon(Eclair_Menu *menu, const char *label, const char *icon_path);
Eclair_Menu_Item *eclair_menu_add_seperator(Eclair_Menu *menu);

Eclair_Menu_Item *eclair_menu_item_new(const char *label, Eclair_Menu *parent, Eclair_Menu_Item_Type, const char *icon_path);
void eclair_menu_item_free(Eclair_Menu_Item *menu_item);
void eclair_menu_item_label_set(Eclair_Menu_Item *item, const char *label);
void eclair_menu_item_activate(Eclair_Menu_Item *item);
void eclair_menu_item_deactivate(Eclair_Menu_Item *item);
void eclair_menu_item_callback_set(Eclair_Menu_Item *item, void (*item_cb)(Eclair_Menu_Item *item, void *data), void *data);

#endif
