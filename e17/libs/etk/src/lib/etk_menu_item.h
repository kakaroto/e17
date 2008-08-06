/* ETK - The Enlightened ToolKit
 * Copyright (C) 2006-2008 Simon Treny, Hisham Mardam-Bey, Vincent Torri, Viktor Kojouharov
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library. 
 * If not, see <http://www.gnu.org/licenses/>.
 */

/** @file etk_menu_item.h */
#ifndef _ETK_MENU_ITEM_H_
#define _ETK_MENU_ITEM_H_

#include "etk_container.h"
#include "etk_stock.h"
#include "etk_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup Etk_Menu_Item Etk_Menu_Item
 * @brief The Etk_Menu_Item widget is an item that can be packed in a menu shell (Etk_Menu or Etk_Menu_Bar)
 * @{
 */

/** Gets the type of a menu item */
#define ETK_MENU_ITEM_TYPE       (etk_menu_item_type_get())
/** Casts the object to an Etk_Menu_Item */
#define ETK_MENU_ITEM(obj)       (ETK_OBJECT_CAST((obj), ETK_MENU_ITEM_TYPE, Etk_Menu_Item))
/** Checks if the object is an Etk_Menu_Item */
#define ETK_IS_MENU_ITEM(obj)    (ETK_OBJECT_CHECK_TYPE((obj), ETK_MENU_ITEM_TYPE))

/** Gets the type of a menu item separator */
#define ETK_MENU_ITEM_SEPARATOR_TYPE       (etk_menu_item_separator_type_get())
/** Casts the object to an Etk_Menu_Item_Separator */
#define ETK_MENU_ITEM_SEPARATOR(obj)       (ETK_OBJECT_CAST((obj), ETK_MENU_ITEM_SEPARATOR_TYPE, Etk_Menu_Item_Separator))
/** Checks if the object is an Etk_Menu_Item_Separator */
#define ETK_IS_MENU_ITEM_SEPARATOR(obj)    (ETK_OBJECT_CHECK_TYPE((obj), ETK_MENU_ITEM_SEPARATOR_TYPE))

/** Gets the type of a menu item image */
#define ETK_MENU_ITEM_IMAGE_TYPE       (etk_menu_item_image_type_get())
/** Casts the object to an Etk_Menu_Item_Image */
#define ETK_MENU_ITEM_IMAGE(obj)       (ETK_OBJECT_CAST((obj), ETK_MENU_ITEM_IMAGE_TYPE, Etk_Menu_Item_Image))
/** Checks if the object is an Etk_Menu_Item_Image */
#define ETK_IS_MENU_ITEM_IMAGE(obj)    (ETK_OBJECT_CHECK_TYPE((obj), ETK_MENU_ITEM_IMAGE_TYPE))

/** Gets the type of a menu item check */
#define ETK_MENU_ITEM_CHECK_TYPE       (etk_menu_item_check_type_get())
/** Casts the object to an Etk_Menu_Item_Check */
#define ETK_MENU_ITEM_CHECK(obj)       (ETK_OBJECT_CAST((obj), ETK_MENU_ITEM_CHECK_TYPE, Etk_Menu_Item_Check))
/** Checks if the object is an Etk_Menu_Item_Check */
#define ETK_IS_MENU_ITEM_CHECK(obj)    (ETK_OBJECT_CHECK_TYPE((obj), ETK_MENU_ITEM_CHECK_TYPE))

/** Gets the type of a menu item radio */
#define ETK_MENU_ITEM_RADIO_TYPE       (etk_menu_item_radio_type_get())
/** Casts the object to an Etk_Menu_Item_Radio */
#define ETK_MENU_ITEM_RADIO(obj)       (ETK_OBJECT_CAST((obj), ETK_MENU_ITEM_RADIO_TYPE, Etk_Menu_Item_Radio))
/** Radios if the object is an Etk_Menu_Item_Radio */
#define ETK_IS_MENU_ITEM_RADIO(obj)    (ETK_OBJECT_CHECK_TYPE((obj), ETK_MENU_ITEM_RADIO_TYPE))

extern int ETK_MENU_ITEM_SELECTED_SIGNAL;
extern int ETK_MENU_ITEM_UNSELECTED_SIGNAL;
extern int ETK_MENU_ITEM_ACTIVATED_SIGNAL;

extern int ETK_MENU_ITEM_CHECK_TOGGLED_SIGNAL;

/**
 * @brief @widget An item that can be packed in a menu shell
 * @structinfo
 */
struct Etk_Menu_Item
{
   /* private: */
   /* Inherit from Etk_Widget */
   Etk_Widget widget;

   Etk_Bool (*selected_handler)(Etk_Menu_Item *menu_item);
   Etk_Bool (*unselected_handler)(Etk_Menu_Item *menu_item);
   Etk_Bool (*activated_handler)(Etk_Menu_Item *menu_item);

   Etk_Menu_Shell *parent_shell;
   Etk_Menu *submenu;

   char *label;
   Etk_Widget *left_widget;

   Etk_Bool is_selected:1;
};

/**
 * @brief @widget A menu-item used to separate two other items (looks like an horizontal bar in a menu)
 * @structinfo
 */
struct Etk_Menu_Item_Separator
{
   /* private: */
   /* Inherit from Etk_Menu_Item */
   Etk_Menu_Item menu_item;
};

/**
 * @brief @widget A menu-item with an image at the left of the label
 * @structinfo
 */
struct Etk_Menu_Item_Image
{
   /* private: */
   /* Inherit from Etk_Menu_Item */
   Etk_Menu_Item menu_item;
};

/**
 * @brief @widget A menu-item with a checkbox at the left of the label
 * @structinfo
 */
struct Etk_Menu_Item_Check
{
   /* private: */
   /* Inherit from Etk_Menu_Item */
   Etk_Menu_Item menu_item;

   Etk_Bool (*toggled_handler)(Etk_Menu_Item_Check *check_item);
   void (*active_set)(Etk_Menu_Item_Check *check_item, Etk_Bool active);
   Etk_Bool active:1;
};

/**
 * @brief @widget A menu-item with a radiobox at the left of the label
 * @structinfo
 */
struct Etk_Menu_Item_Radio
{
   /* private: */
   /* Inherit from Etk_Menu_Item_Check */
   Etk_Menu_Item_Check check_item;

   Evas_List **group;
   Etk_Bool can_uncheck:1;
};


/* Etk_Menu_Item */
Etk_Type   *etk_menu_item_type_get(void);
Etk_Widget *etk_menu_item_new(void);
Etk_Widget *etk_menu_item_new_with_label(const char *label);
Etk_Widget *etk_menu_item_new_from_stock(Etk_Stock_Id stock_id);

void        etk_menu_item_label_set(Etk_Menu_Item *menu_item, const char *label);
const char *etk_menu_item_label_get(Etk_Menu_Item *menu_item);
void        etk_menu_item_set_from_stock(Etk_Menu_Item *menu_item, Etk_Stock_Id stock_id);
void        etk_menu_item_submenu_set(Etk_Menu_Item *menu_item, Etk_Menu *submenu);
Etk_Menu   *etk_menu_item_submenu_get(Etk_Menu_Item *menu_item);

void        etk_menu_item_select(Etk_Menu_Item *menu_item);
void        etk_menu_item_unselect(Etk_Menu_Item *menu_item);
void        etk_menu_item_activate(Etk_Menu_Item *menu_item);

/* Etk_Menu_Item_Separator */
Etk_Type   *etk_menu_item_separator_type_get(void);
Etk_Widget *etk_menu_item_separator_new(void);

/* Etk_Menu_Item_Image */
Etk_Type   *etk_menu_item_image_type_get(void);
Etk_Widget *etk_menu_item_image_new(void);
Etk_Widget *etk_menu_item_image_new_with_label(const char *label);
Etk_Widget *etk_menu_item_image_new_from_stock(Etk_Stock_Id stock_id);
void        etk_menu_item_image_set(Etk_Menu_Item_Image *image_item, Etk_Image *image);

/* Etk_Menu_Item_Check */
Etk_Type   *etk_menu_item_check_type_get(void);
Etk_Widget *etk_menu_item_check_new(void);
Etk_Widget *etk_menu_item_check_new_with_label(const char *label);
void        etk_menu_item_check_active_set(Etk_Menu_Item_Check *check_item, Etk_Bool active);
Etk_Bool    etk_menu_item_check_active_get(Etk_Menu_Item_Check *check_item);

/* Etk_Menu_Item_Radio */
Etk_Type   *etk_menu_item_radio_type_get(void);
Etk_Widget *etk_menu_item_radio_new(Evas_List **group);
Etk_Widget *etk_menu_item_radio_new_from_widget(Etk_Menu_Item_Radio *radio_item);
Etk_Widget *etk_menu_item_radio_new_with_label(const char *label, Evas_List **group);
Etk_Widget *etk_menu_item_radio_new_with_label_from_widget(const char *label, Etk_Menu_Item_Radio *radio_item);
void        etk_menu_item_radio_group_set(Etk_Menu_Item_Radio *radio_item, Evas_List **group);
Evas_List **etk_menu_item_radio_group_get(Etk_Menu_Item_Radio *radio_item);

/** @} */

#ifdef __cplusplus
}
#endif

#endif
