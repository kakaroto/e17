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

/** @brief Gets the type of a menu item separator */
#define ETK_MENU_ITEM_SEPARATOR_TYPE       (etk_menu_item_separator_type_get())
/** @brief Casts the object to an Etk_Menu_Item_Separator */
#define ETK_MENU_ITEM_SEPARATOR(obj)       (ETK_OBJECT_CAST((obj), ETK_MENU_ITEM_SEPARATOR_TYPE, Etk_Menu_Item_Separator))
/** @brief Checks if the object is an Etk_Menu_Item_Separator */
#define ETK_IS_MENU_ITEM_SEPARATOR(obj)    (ETK_OBJECT_CHECK_TYPE((obj), ETK_MENU_ITEM_SEPARATOR_TYPE))

/** @brief Gets the type of a menu item image */
#define ETK_MENU_ITEM_IMAGE_TYPE       (etk_menu_item_image_type_get())
/** @brief Casts the object to an Etk_Menu_Item_Image */
#define ETK_MENU_ITEM_IMAGE(obj)       (ETK_OBJECT_CAST((obj), ETK_MENU_ITEM_IMAGE_TYPE, Etk_Menu_Item_Image))
/** @brief Checks if the object is an Etk_Menu_Item_Image */
#define ETK_IS_MENU_ITEM_IMAGE(obj)    (ETK_OBJECT_CHECK_TYPE((obj), ETK_MENU_ITEM_IMAGE_TYPE))

/** @brief Gets the type of a menu item check */
#define ETK_MENU_ITEM_CHECK_TYPE       (etk_menu_item_check_type_get())
/** @brief Casts the object to an Etk_Menu_Item_Check */
#define ETK_MENU_ITEM_CHECK(obj)       (ETK_OBJECT_CAST((obj), ETK_MENU_ITEM_CHECK_TYPE, Etk_Menu_Item_Check))
/** @brief Checks if the object is an Etk_Menu_Item_Check */
#define ETK_IS_MENU_ITEM_CHECK(obj)    (ETK_OBJECT_CHECK_TYPE((obj), ETK_MENU_ITEM_CHECK_TYPE))

/** @brief Gets the type of a menu item radio */
#define ETK_MENU_ITEM_RADIO_TYPE       (etk_menu_item_radio_type_get())
/** @brief Casts the object to an Etk_Menu_Item_Radio */
#define ETK_MENU_ITEM_RADIO(obj)       (ETK_OBJECT_CAST((obj), ETK_MENU_ITEM_RADIO_TYPE, Etk_Menu_Item_Radio))
/** @brief Radios if the object is an Etk_Menu_Item_Radio */
#define ETK_IS_MENU_ITEM_RADIO(obj)    (ETK_OBJECT_CHECK_TYPE((obj), ETK_MENU_ITEM_RADIO_TYPE))

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
   Etk_Widget *left_widget;
   
   Etk_Bool is_selected;
};

/**
 * @struct Etk_Menu_Item_Separator
 * @brief An horizontal separator for the menus
 */
struct _Etk_Menu_Item_Separator
{
   /* private: */
   /* Inherit from Etk_Menu_Item */
   Etk_Menu_Item menu_item;
};

/**
 * @struct Etk_Menu_Item_Image
 * @brief A menu item with an image
 */
struct _Etk_Menu_Item_Image
{
   /* private: */
   /* Inherit from Etk_Menu_Item */
   Etk_Menu_Item menu_item;
};

/**
 * @struct Etk_Menu_Item_Check
 * @brief A menu item with a checkbox
 */
struct _Etk_Menu_Item_Check
{
   /* private: */
   /* Inherit from Etk_Menu_Item */
   Etk_Menu_Item menu_item;
   
   void (*toggled)(Etk_Menu_Item_Check *check_item);
   void (*active_set)(Etk_Menu_Item_Check *check_item, Etk_Bool active);
   Etk_Bool active;
};

/**
 * @struct Etk_Menu_Item_Check
 * @brief A menu item with a radiobox
 */
struct _Etk_Menu_Item_Radio
{
   /* private: */
   /* Inherit from Etk_Menu_Item_Check */
   Etk_Menu_Item_Check check_item;
   
   Evas_List **group;
   Etk_Bool can_uncheck;
};

/* Etk_Menu_Item */
Etk_Type *etk_menu_item_type_get();
Etk_Widget *etk_menu_item_new();
Etk_Widget *etk_menu_item_new_with_label(const char *label);

void etk_menu_item_label_set(Etk_Menu_Item *menu_item, const char *label);
const char *etk_menu_item_label_get(Etk_Menu_Item *menu_item);
void etk_menu_item_submenu_set(Etk_Menu_Item *menu_item, Etk_Menu *submenu);

void etk_menu_item_select(Etk_Menu_Item *menu_item);
void etk_menu_item_deselect(Etk_Menu_Item *menu_item);
void etk_menu_item_activate(Etk_Menu_Item *menu_item);

/* Etk_Menu_Item_Separator */
Etk_Type *etk_menu_item_separator_type_get();
Etk_Widget *etk_menu_item_separator_new();

/* Etk_Menu_Item_Image */
Etk_Type *etk_menu_item_image_type_get();
Etk_Widget *etk_menu_item_image_new();
Etk_Widget *etk_menu_item_image_new_with_label(const char *label);
void etk_menu_item_image_set(Etk_Menu_Item_Image *image_item, Etk_Image *image);

/* Etk_Menu_Item_Check */
Etk_Type *etk_menu_item_check_type_get();
Etk_Widget *etk_menu_item_check_new();
Etk_Widget *etk_menu_item_check_new_with_label(const char *label);
void etk_menu_item_check_active_set(Etk_Menu_Item_Check *check_item, Etk_Bool active);
Etk_Bool etk_menu_item_check_active_get(Etk_Menu_Item_Check *check_item);

/* Etk_Menu_Item_Radio */
Etk_Type *etk_menu_item_radio_type_get();
Etk_Widget *etk_menu_item_radio_new(Evas_List **group);
Etk_Widget *etk_menu_item_radio_new_from_widget(Etk_Menu_Item_Radio *radio_item);
Etk_Widget *etk_menu_item_radio_new_with_label(const char *label, Evas_List **group);
Etk_Widget *etk_menu_item_radio_new_with_label_from_widget(const char *label, Etk_Menu_Item_Radio *radio_item);
void etk_menu_item_radio_group_set(Etk_Menu_Item_Radio *radio_item, Evas_List **group);
Evas_List **etk_menu_item_radio_group_get(Etk_Menu_Item_Radio *radio_item);

/** @} */

#endif
