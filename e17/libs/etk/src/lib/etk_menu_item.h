/** @file etk_menu_item.h */
#ifndef _ETK_MENU_ITEM_H_
#define _ETK_MENU_ITEM_H_

#include "etk_container.h"
#include "etk_types.h"

/**
 * @defgroup Etk_Menu_Item Etk_Menu_Item
 * @{
 */

/** @brief Gets the type of a menu item */
#define ETK_MENU_ITEM_TYPE       (etk_menu_item_type_get())
/** @brief Casts the object to an Etk_Menu_Item */
#define ETK_MENU_ITEM(obj)       (ETK_OBJECT_CAST((obj), ETK_MENU_ITEM_TYPE, Etk_Menu_Item))
/** @brief Checks if the object is an Etk_Menu_Item */
#define ETK_IS_MENU_ITEM(obj)    (ETK_OBJECT_CHECK_TYPE((obj), ETK_MENU_ITEM_TYPE))

/** @brief Gets the type of a menu separator */
#define ETK_MENU_SEPARATOR_TYPE       (etk_menu_separator_type_get())
/** @brief Casts the object to an Etk_Menu_Sepatator */
#define ETK_MENU_SEPARATOR(obj)       (ETK_OBJECT_CAST((obj), ETK_MENU_SEPARATOR_TYPE, Etk_Menu_Separator))
/** @brief Checks if the object is an Etk_Menu_Separator */
#define ETK_IS_MENU_SEPARATOR(obj)    (ETK_OBJECT_CHECK_TYPE((obj), ETK_MENU_SEPARATOR_TYPE))

/**
 * @struct Etk_Menu_Item
 * @brief An item of a menu or of a menu bar
 */
struct _Etk_Menu_Item
{
   /* private: */
   /* Inherit from Etk_Widget */
   Etk_Widget widget;
   
   void (*selected)(Etk_Menu_Item *menu_item);
   void (*deselected)(Etk_Menu_Item *menu_item);
   void (*activated)(Etk_Menu_Item *menu_item);
   
   Etk_Menu_Shell *parent;
   Etk_Menu *submenu;
   
   char *label;
   Etk_Image *image;
   /* "right_widget" can be the submenu arrow, the checkbox or the radiobox widget */
   Etk_Widget *right_widget;
   
   Etk_Bool is_selected;
   Etk_Bool show_image;
   Etk_Bool show_right_swallow;
   Etk_Bool right_widget_is_arrow;
};

/**
 * @struct Etk_Menu_Separator
 * @brief An horizontal separator for the menus
 */
struct _Etk_Menu_Separator
{
   /* private: */
   /* Inherit from Etk_Menu_Item */
   Etk_Menu_Item menu_item;
};

Etk_Type *etk_menu_item_type_get();
Etk_Widget *etk_menu_item_new();
Etk_Widget *etk_menu_item_new_with_label(const char *label);

void etk_menu_item_label_set(Etk_Menu_Item *menu_item, const char *label);
const char *etk_menu_item_label_get(Etk_Menu_Item *menu_item);

void etk_menu_item_select(Etk_Menu_Item *menu_item);
void etk_menu_item_deselect(Etk_Menu_Item *menu_item);
void etk_menu_item_activate(Etk_Menu_Item *menu_item);

void etk_menu_item_submenu_set(Etk_Menu_Item *menu_item, Etk_Menu *submenu);
void etk_menu_item_image_set(Etk_Menu_Item *menu_item, Etk_Image *image);

void etk_menu_item_image_show(Etk_Menu_Item *menu_item, Etk_Bool show);
void etk_menu_item_right_swallow_show(Etk_Menu_Item *menu_item, Etk_Bool show);

Etk_Type *etk_menu_separator_type_get();
Etk_Widget *etk_menu_separator_new();

/** @} */

#endif
