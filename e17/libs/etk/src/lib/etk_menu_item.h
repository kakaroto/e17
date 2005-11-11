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

/* TODO: doc */
struct _Etk_Menu_Item
{
   /* private: */
   /* Inherit from Etk_Container */
   Etk_Container container;
   
   void (*select)(Etk_Menu_Item *menu_item);
   void (*deselect)(Etk_Menu_Item *menu_item);
   void (*activate)(Etk_Menu_Item *menu_item);
   
   Etk_Menu *child, *parent;
   char *label;
   Etk_Image *image;
   Etk_Bool selected;
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

/** @} */

#endif
