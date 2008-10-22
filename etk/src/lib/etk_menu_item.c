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

/** @file etk_menu_item.c */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "etk_menu_item.h"

#include <stdlib.h>
#include <string.h>

#include "etk_image.h"
#include "etk_menu.h"
#include "etk_menu_shell.h"
#include "etk_signal.h"
#include "etk_signal_callback.h"
#include "etk_utils.h"

/**
 * @addtogroup Etk_Menu_Item
 * @{
 */

int ETK_MENU_ITEM_SELECTED_SIGNAL;
int ETK_MENU_ITEM_UNSELECTED_SIGNAL;
int ETK_MENU_ITEM_ACTIVATED_SIGNAL;

int ETK_MENU_ITEM_CHECK_TOGGLED_SIGNAL;

enum Etk_Menu_Item_Property_Id
{
   ETK_MENU_ITEM_LABEL_PROPERTY,
   ETK_MENU_ITEM_SUBMENU_PROPERTY,
   ETK_MENU_ITEM_IMAGE_PROPERTY,
   ETK_MENU_ITEM_ACTIVE_PROPERTY,
   ETK_MENU_ITEM_GROUP_PROPERTY
};

static void _etk_menu_item_constructor(Etk_Menu_Item *menu_item);
static void _etk_menu_item_destructor(Etk_Menu_Item *menu_item);
static void _etk_menu_item_check_constructor(Etk_Menu_Item_Check *check_item);
static void _etk_menu_item_radio_constructor(Etk_Menu_Item_Radio *radio_item);
static void _etk_menu_item_radio_destructor(Etk_Menu_Item_Radio *radio_item);
static void _etk_menu_item_property_set(Etk_Object *object, int property_id, Etk_Property_Value *value);
static void _etk_menu_item_property_get(Etk_Object *object, int property_id, Etk_Property_Value *value);
static void _etk_menu_item_image_property_set(Etk_Object *object, int property_id, Etk_Property_Value *value);
static void _etk_menu_item_image_property_get(Etk_Object *object, int property_id, Etk_Property_Value *value);
static void _etk_menu_item_check_property_set(Etk_Object *object, int property_id, Etk_Property_Value *value);
static void _etk_menu_item_check_property_get(Etk_Object *object, int property_id, Etk_Property_Value *value);
static void _etk_menu_item_radio_property_set(Etk_Object *object, int property_id, Etk_Property_Value *value);
static void _etk_menu_item_radio_property_get(Etk_Object *object, int property_id, Etk_Property_Value *value);
static Etk_Bool _etk_menu_item_realized_cb(Etk_Object *object, void *data);
static Etk_Bool _etk_menu_item_check_box_realized_cb(Etk_Object *object, void *data);
static Etk_Bool _etk_menu_item_check_activated_cb(Etk_Object *object, void *data);
static Etk_Bool _etk_menu_item_selected_handler(Etk_Menu_Item *menu_item);
static Etk_Bool _etk_menu_item_unselected_handler(Etk_Menu_Item *menu_item);
static Etk_Bool _etk_menu_item_activated_handler(Etk_Menu_Item *menu_item);
static Etk_Bool _etk_menu_item_check_toggled_handler(Etk_Menu_Item_Check *check_item);
static void _etk_menu_item_check_active_set(Etk_Menu_Item_Check *check_item, Etk_Bool active);
static void _etk_menu_item_radio_active_set(Etk_Menu_Item_Check *check_item, Etk_Bool active);

/**************************
 *
 * Implementation
 *
 **************************/

/**************************
 * Menu Item
 **************************/

/**
 * @internal
 * @brief Gets the type of an Etk_Menu_Item
 * @return Returns the type of an Etk_Menu_Item
 */
Etk_Type *etk_menu_item_type_get(void)
{
   static Etk_Type *menu_item_type = NULL;

   if (!menu_item_type)
   {
      const Etk_Signal_Description signals[] = {
         ETK_SIGNAL_DESC_HANDLER(ETK_MENU_ITEM_SELECTED_SIGNAL,
            "selected", Etk_Menu_Item, selected_handler,
            etk_marshaller_VOID),
         ETK_SIGNAL_DESC_HANDLER(ETK_MENU_ITEM_UNSELECTED_SIGNAL,
            "unselected", Etk_Menu_Item, unselected_handler,
            etk_marshaller_VOID),
         ETK_SIGNAL_DESC_HANDLER(ETK_MENU_ITEM_ACTIVATED_SIGNAL,
            "activated", Etk_Menu_Item, activated_handler,
            etk_marshaller_VOID),
         ETK_SIGNAL_DESCRIPTION_SENTINEL
      };

      menu_item_type = etk_type_new("Etk_Menu_Item", ETK_WIDGET_TYPE,
         sizeof(Etk_Menu_Item), ETK_CONSTRUCTOR(_etk_menu_item_constructor),
         ETK_DESTRUCTOR(_etk_menu_item_destructor), signals);

      etk_type_property_add(menu_item_type, "label", ETK_MENU_ITEM_LABEL_PROPERTY,
         ETK_PROPERTY_STRING, ETK_PROPERTY_READABLE_WRITABLE, etk_property_value_string(NULL));
      etk_type_property_add(menu_item_type, "submenu", ETK_MENU_ITEM_SUBMENU_PROPERTY,
         ETK_PROPERTY_OBJECT, ETK_PROPERTY_READABLE_WRITABLE, etk_property_value_object(NULL));

      menu_item_type->property_set = _etk_menu_item_property_set;
      menu_item_type->property_get = _etk_menu_item_property_get;
   }

   return menu_item_type;
}

/**
 * @brief Creates a new menu-item with an empty label
 * @return Returns the new menu-item widget
 * @note Unlike the other widgets, the menu-item will automatically shown at its creation
 */
Etk_Widget *etk_menu_item_new(void)
{
   return etk_widget_new(ETK_MENU_ITEM_TYPE, "theme-group", "menu_item", "visible", ETK_TRUE, NULL);
}

/**
 * @brief Creates a new menu-item and sets its label to @a label
 * @param label the label to set to the new menu item
 * @return Returns the new menu-item widget
 * @note Unlike the other widgets, the menu-item will automatically shown at its creation
 */
Etk_Widget *etk_menu_item_new_with_label(const char *label)
{
   return etk_widget_new(ETK_MENU_ITEM_TYPE, "theme-group", "menu_item", "visible", ETK_TRUE, "label", label, NULL);
}

/**
 * @brief Creates a new menu-item with a label defined by the stock-id
 * @param stock_id the stock-id corresponding to the label
 * @return Returns the new menu-item widget
 * @note Unlike the other widgets, the menu-item will automatically shown at its creation
 * @see Etk_Stock
 */
Etk_Widget *etk_menu_item_new_from_stock(Etk_Stock_Id stock_id)
{
   Etk_Widget *menu_item;

   menu_item = etk_menu_item_new();
   etk_menu_item_set_from_stock(ETK_MENU_ITEM(menu_item), stock_id);

   return menu_item;
}

/**
 * @brief Sets the label of the menu-item
 * @param menu_item a menu-item
 * @param label the label to set
 */
void etk_menu_item_label_set(Etk_Menu_Item *menu_item, const char *label)
{
   if (!menu_item || menu_item->label == label)
      return;

   free(menu_item->label);
   menu_item->label = label ? strdup(label) : NULL;
   etk_widget_theme_part_text_set(ETK_WIDGET(menu_item), "etk.text.label", label ? label : "");

   etk_object_notify(ETK_OBJECT(menu_item), "label");
}

/**
 * @brief Gets the label of the menu-item
 * @param menu_item a menu-item
 * @return Returns the label of the menu-item
 */
const char *etk_menu_item_label_get(Etk_Menu_Item *menu_item)
{
   if (!menu_item)
      return NULL;
   return menu_item->label;
}

/**
 * @brief Sets the label of the menu-item from a stock-id. If the menu-item is an Etk_Menu_Item_Image,
 * the image is also changed depending on the stock-id
 * @param menu_item a menu-item
 * @param stock_id the stock-id corresponding to the label and the image you want to use
 * @see Etk_Stock
 */
void etk_menu_item_set_from_stock(Etk_Menu_Item *menu_item, Etk_Stock_Id stock_id)
{
   const char *label;

   if (!menu_item)
      return;

   label = etk_stock_label_get(stock_id);
   etk_menu_item_label_set(menu_item, label);

   if (ETK_IS_MENU_ITEM_IMAGE(menu_item))
   {
      Etk_Widget *image;

      image = etk_image_new_from_stock(stock_id, ETK_STOCK_SMALL);
      etk_menu_item_image_set(ETK_MENU_ITEM_IMAGE(menu_item), ETK_IMAGE(image));
      etk_widget_internal_set(image, ETK_TRUE);
      etk_widget_show(image);
   }
}

/**
 * @brief Sets the submenu of the menu-item: the submenu will be popped up when the menu-item is selected
 * @param menu_item a menu-item
 * @param submenu the submenu to attach to the menu-item
 */
void etk_menu_item_submenu_set(Etk_Menu_Item *menu_item, Etk_Menu *submenu)
{
   if (!menu_item || menu_item->submenu == submenu)
      return;

   if (menu_item->submenu)
   {
      menu_item->submenu->parent_item = NULL;
      etk_object_notify(ETK_OBJECT(menu_item->submenu), "parent-item");
   }
   menu_item->submenu = submenu;
   if (menu_item->submenu)
   {
      menu_item->submenu->parent_item = menu_item;
      etk_object_notify(ETK_OBJECT(menu_item->submenu), "parent-item");
   }

   if (menu_item->submenu)
      etk_widget_theme_signal_emit(ETK_WIDGET(menu_item), "etk,action,show,arrow", ETK_TRUE);
   else
      etk_widget_theme_signal_emit(ETK_WIDGET(menu_item), "etk,action,hide,arrow", ETK_TRUE);

   etk_widget_size_recalc_queue(ETK_WIDGET(menu_item));
   etk_object_notify(ETK_OBJECT(menu_item), "submenu");
}

/**
 * @brief Gets the submenu attached to the menu-item
 * @param menu_item a menu-item
 * @return Returns the submenu attached to the menu-item
 */
Etk_Menu *etk_menu_item_submenu_get(Etk_Menu_Item *menu_item)
{
   if (!menu_item)
      return NULL;
   return menu_item->submenu;
}

/**
 * @brief Selects the menu-item
 * @param menu_item a menu-item
 */
void etk_menu_item_select(Etk_Menu_Item *menu_item)
{
   if (!menu_item || menu_item->is_selected)
      return;

   menu_item->is_selected = ETK_TRUE;
   etk_signal_emit(ETK_MENU_ITEM_SELECTED_SIGNAL, ETK_OBJECT(menu_item));
}

/**
 * @brief Unselects the menu-item
 * @param menu_item a menu-item
 */
void etk_menu_item_unselect(Etk_Menu_Item *menu_item)
{
   if (!menu_item || !menu_item->is_selected)
      return;

   menu_item->is_selected = ETK_FALSE;
   etk_signal_emit(ETK_MENU_ITEM_UNSELECTED_SIGNAL, ETK_OBJECT(menu_item));
}

/**
 * @brief Activates the menu-item
 * @param menu_item a menu-item
 */
void etk_menu_item_activate(Etk_Menu_Item *menu_item)
{
   if (!menu_item)
      return;
   etk_signal_emit(ETK_MENU_ITEM_ACTIVATED_SIGNAL, ETK_OBJECT(menu_item));
}

/**************************
 * Menu Item Separator
 **************************/

/**
 * @internal
 * @brief Gets the type of an Etk_Menu_Item_Separator
 * @return Returns the type of an Etk_Menu_Item_Separator
 */
Etk_Type *etk_menu_item_separator_type_get(void)
{
   static Etk_Type *menu_item_separator_type = NULL;

   if (!menu_item_separator_type)
   {
      menu_item_separator_type = etk_type_new("Etk_Menu_Item_Separator",
         ETK_MENU_ITEM_TYPE, sizeof(Etk_Menu_Item_Separator),
         NULL, NULL, NULL);
   }

   return menu_item_separator_type;
}

/**
 * @brief Creates a new separator menu-item
 * @return Returns the new menu-item widget
 * @note Unlike the other widgets, the menu-item will automatically shown at its creation
 */
Etk_Widget *etk_menu_item_separator_new(void)
{
   return etk_widget_new(ETK_MENU_ITEM_SEPARATOR_TYPE, "theme-group", "separator", "visible", ETK_TRUE, NULL);
}

/**************************
 * Menu Item Image
 **************************/

/**
 * @internal
 * @brief Gets the type of an Etk_Menu_Item_Image
 * @return Returns the type of an Etk_Menu_Item_Image
 */
Etk_Type *etk_menu_item_image_type_get(void)
{
   static Etk_Type *menu_item_image_type = NULL;

   if (!menu_item_image_type)
   {
      menu_item_image_type = etk_type_new("Etk_Menu_Item_Image",
         ETK_MENU_ITEM_TYPE, sizeof(Etk_Menu_Item_Image),
         NULL, NULL, NULL);

      etk_type_property_add(menu_item_image_type, "image", ETK_MENU_ITEM_IMAGE_PROPERTY,
         ETK_PROPERTY_OBJECT, ETK_PROPERTY_READABLE_WRITABLE, etk_property_value_object(NULL));

      menu_item_image_type->property_set = _etk_menu_item_image_property_set;
      menu_item_image_type->property_get = _etk_menu_item_image_property_get;
   }

   return menu_item_image_type;
}

/**
 * @brief Creates a new image menu-item with an empty label
 * @return Returns the new menu-item widget
 * @note Unlike the other widgets, the menu-item will automatically shown at its creation
 */
Etk_Widget *etk_menu_item_image_new(void)
{
   return etk_widget_new(ETK_MENU_ITEM_IMAGE_TYPE, "theme-group", "menu_item", "visible", ETK_TRUE, NULL);
}

/**
 * @brief Creates a new image menu-item and sets its label to @a label
 * @param label the label to set to the new menu-item
 * @return Returns the new menu-item widget
 * @note Unlike the other widgets, the menu-item will automatically shown at its creation
 */
Etk_Widget *etk_menu_item_image_new_with_label(const char *label)
{
   return etk_widget_new(ETK_MENU_ITEM_IMAGE_TYPE, "theme-group", "menu_item",
      "label", label, "visible", ETK_TRUE, NULL);
}

/**
 * @brief Creates a new image menu-item with the label and the image defined by the stock-id
 * @param stock_id the stock-id corresponding to the label and the image
 * @return Returns the new menu-item widget
 * @note Unlike the other widgets, the menu-item will automatically shown at its creation
 * @see Etk_Stock
 */
Etk_Widget *etk_menu_item_image_new_from_stock(Etk_Stock_Id stock_id)
{
   Etk_Widget *menu_item;

   menu_item = etk_menu_item_image_new();
   etk_menu_item_set_from_stock(ETK_MENU_ITEM(menu_item), stock_id);

   return menu_item;
}

/**
 * @brief Sets the image of the menu-item. The image will be displayed on the left of the label
 * @param image_item an image menu-item
 * @param image the image to set (NULL to remove the image)
 */
void etk_menu_item_image_set(Etk_Menu_Item_Image *image_item, Etk_Image *image)
{
   Etk_Menu_Item *menu_item;
   Etk_Widget *image_widget;

   if (!(menu_item = ETK_MENU_ITEM(image_item)))
      return;

   if (menu_item->left_widget)
   {
      etk_widget_parent_set(menu_item->left_widget, NULL);
      menu_item->left_widget = NULL;
   }

   if ((image_widget = ETK_WIDGET(image)))
   {
      menu_item->left_widget = image_widget;
      etk_widget_parent_set(menu_item->left_widget, ETK_WIDGET(menu_item));
      etk_widget_pass_mouse_events_set(menu_item->left_widget, ETK_TRUE);

      etk_widget_swallow_widget(ETK_WIDGET(menu_item), "etk.swallow.left_widget", image_widget);
      etk_widget_theme_signal_emit(ETK_WIDGET(menu_item), "etk,action,show,left_widget", ETK_TRUE);
   }
   else
      etk_widget_theme_signal_emit(ETK_WIDGET(menu_item), "etk,action,hide,left_widget", ETK_TRUE);

   etk_widget_size_recalc_queue(ETK_WIDGET(menu_item));
}

/**************************
 * Menu Item Check
 **************************/

/**
 * @internal
 * @brief Gets the type of an Etk_Menu_Item_Check
 * @return Returns the type of an Etk_Menu_Item_Check
 */
Etk_Type *etk_menu_item_check_type_get(void)
{
   static Etk_Type *menu_item_check_type = NULL;

   if (!menu_item_check_type)
   {
      const Etk_Signal_Description signals[] = {
         ETK_SIGNAL_DESC_HANDLER(ETK_MENU_ITEM_CHECK_TOGGLED_SIGNAL,
            "toggled", Etk_Menu_Item_Check, toggled_handler,
            etk_marshaller_VOID),
         ETK_SIGNAL_DESCRIPTION_SENTINEL
      };

      menu_item_check_type = etk_type_new("Etk_Menu_Item_Check",
         ETK_MENU_ITEM_TYPE, sizeof(Etk_Menu_Item_Check),
         ETK_CONSTRUCTOR(_etk_menu_item_check_constructor), NULL, signals);

      etk_type_property_add(menu_item_check_type, "active", ETK_MENU_ITEM_ACTIVE_PROPERTY,
         ETK_PROPERTY_BOOL, ETK_PROPERTY_READABLE_WRITABLE, etk_property_value_bool(ETK_FALSE));

      menu_item_check_type->property_set = _etk_menu_item_check_property_set;
      menu_item_check_type->property_get = _etk_menu_item_check_property_get;
   }

   return menu_item_check_type;
}

/**
 * @brief Creates a new check menu-item with an empty label
 * @return Returns the new menu-item widget
 * @note Unlike the other widgets, the menu-item will automatically shown at its creation
 */
Etk_Widget *etk_menu_item_check_new(void)
{
   return etk_widget_new(ETK_MENU_ITEM_CHECK_TYPE, "theme-group", "menu_item", "visible", ETK_TRUE, NULL);
}

/**
 * @brief Creates a new check menu-item and sets the label to @a label
 * @param label the label to set to the new menu-item
 * @return Returns the new menu-item widget
 * @note Unlike the other widgets, the menu-item will automatically shown at its creation
 */
Etk_Widget *etk_menu_item_check_new_with_label(const char *label)
{
   return etk_widget_new(ETK_MENU_ITEM_CHECK_TYPE, "theme-group", "menu_item",
      "label", label, "visible", ETK_TRUE, NULL);
}

/**
 * @brief Sets whether or not the check menu item is active (i.e. checked)
 * @param check_item a check menu-item
 * @param active if @a active == ETK_TRUE, the check menu-item will be active
 */
void etk_menu_item_check_active_set(Etk_Menu_Item_Check *check_item, Etk_Bool active)
{
   if (!check_item || !check_item->active_set)
      return;
   check_item->active_set(check_item, active);
}

/**
 * @brief Gets whether the check menu-item is active
 * @param check_item a check menu-item
 * @return Returns ETK_TRUE if @a check_item is active, ETK_FALSE otherwise
 */
Etk_Bool etk_menu_item_check_active_get(Etk_Menu_Item_Check *check_item)
{
   if (!check_item)
      return ETK_FALSE;
   return check_item->active;
}

/**************************
 * Menu Item Radio
 **************************/

/**
 * @internal
 * @brief Gets the type of an Etk_Menu_Item_Radio
 * @return Returns the type of an Etk_Menu_Item_Radio
 */
Etk_Type *etk_menu_item_radio_type_get(void)
{
   static Etk_Type *menu_item_radio_type = NULL;

   if (!menu_item_radio_type)
   {
      menu_item_radio_type = etk_type_new("Etk_Menu_Item_Radio", ETK_MENU_ITEM_CHECK_TYPE, sizeof(Etk_Menu_Item_Radio),
         ETK_CONSTRUCTOR(_etk_menu_item_radio_constructor), ETK_DESTRUCTOR(_etk_menu_item_radio_destructor), NULL);

      etk_type_property_add(menu_item_radio_type, "group", ETK_MENU_ITEM_GROUP_PROPERTY,
         ETK_PROPERTY_POINTER, ETK_PROPERTY_READABLE_WRITABLE,  etk_property_value_pointer(NULL));

      menu_item_radio_type->property_set = _etk_menu_item_radio_property_set;
      menu_item_radio_type->property_get = _etk_menu_item_radio_property_get;
   }

   return menu_item_radio_type;
}

/**
 * @brief Creates a new radio menu-item with an empty label
 * @param group the group which the radio menu-item will be added to
 * (NULL if the radio menu-item should create its own group)
 * @return Returns the new menu-item widget
 * @note Unlike the other widgets, the menu-item will automatically shown at its creation
 */
Etk_Widget *etk_menu_item_radio_new(Eina_List **group)
{
   return etk_widget_new(ETK_MENU_ITEM_RADIO_TYPE, "theme-group", "menu_item",
      "visible", ETK_TRUE, "group", group, NULL);
}

/**
 * @brief Creates a new radio menu-item with an empty label and adds it to the group of another radio menu-item
 * @param radio_item the radio menu-item whose group will be used for the new radio menu-item
 * @return Returns the new menu-item widget
 * @note Unlike the other widgets, the menu-item will automatically shown at its creation
 */
Etk_Widget *etk_menu_item_radio_new_from_widget(Etk_Menu_Item_Radio *radio_item)
{
   return etk_widget_new(ETK_MENU_ITEM_RADIO_TYPE, "theme-group", "menu_item", "visible", ETK_TRUE,
      "group", etk_menu_item_radio_group_get(radio_item), NULL);
}

/**
 * @brief Creates a new radio menu-item and sets its label to @a label
 * @param label the label to set to the new menu-item
 * @param group the group which the radio menu-item will be added to
 * (NULL if the radio menu-item should create its own group)
 * @return Returns the new menu-item widget
 * @note Unlike the other widgets, the menu-item will automatically shown at its creation
 */
Etk_Widget *etk_menu_item_radio_new_with_label(const char *label, Eina_List **group)
{
   return etk_widget_new(ETK_MENU_ITEM_RADIO_TYPE, "theme-group", "menu_item",
      "visible", ETK_TRUE, "group", group, "label", label, NULL);
}

/**
 * @brief Creates a new radio menu-item and sets its label to @a label.
 * It is then added to the group of @a radio_item
 * @param label the label to set to the new menu-item
 * @param radio_item the radio menu-item whose group will be used for the new radio menu-item
 * @return Returns the new menu-item widget
 * @note Unlike the other widgets, the menu-item will automatically shown at its creation
 */
Etk_Widget *etk_menu_item_radio_new_with_label_from_widget(const char *label, Etk_Menu_Item_Radio *radio_item)
{
   return etk_widget_new(ETK_MENU_ITEM_RADIO_TYPE, "theme-group", "menu_item", "visible", ETK_TRUE,
      "group", etk_menu_item_radio_group_get(radio_item), "label", label, NULL);
}

/**
 * @brief Sets the group of the radio menu-item
 * @param radio_item a radio menu-item
 * @param group the group to set
 */
void etk_menu_item_radio_group_set(Etk_Menu_Item_Radio *radio_item, Eina_List **group)
{
   Etk_Menu_Item_Check *check_item;
   Etk_Bool active;

   if (!(check_item = ETK_MENU_ITEM_CHECK(radio_item)) || (group && (radio_item->group == group)))
      return;

   if (radio_item->group)
   {
      *radio_item->group = eina_list_remove(*radio_item->group, radio_item);
      if (!(*radio_item->group))
      {
         free(radio_item->group);
         radio_item->group = NULL;
      }
   }

   if (!group)
   {
      group = malloc(sizeof(Eina_List *));
      *group = NULL;
      active = ETK_TRUE;
   }
   else
      active = ETK_FALSE;

   *group = eina_list_append(*group, radio_item);
   radio_item->group = group;
   etk_object_notify(ETK_OBJECT(radio_item), "group");

   radio_item->can_uncheck = ETK_TRUE;
   etk_menu_item_check_active_set(check_item, active);
}

/**
 * @brief Gets the group of the radio menu-item
 * @param radio_item a radio menu-item
 * @return Returns the group of the radio menu-item
 */
Eina_List **etk_menu_item_radio_group_get(Etk_Menu_Item_Radio *radio_item)
{
   if (!radio_item)
      return NULL;
   return radio_item->group;
}

/**************************
 *
 * Etk specific functions
 *
 **************************/

/* Menu_Item: Initializes the menu item */
static void _etk_menu_item_constructor(Etk_Menu_Item *menu_item)
{
   if (!menu_item)
      return;

   menu_item->selected_handler = _etk_menu_item_selected_handler;
   menu_item->unselected_handler = _etk_menu_item_unselected_handler;
   menu_item->activated_handler = _etk_menu_item_activated_handler;

   menu_item->submenu = NULL;
   menu_item->parent_shell = NULL;
   menu_item->label = NULL;
   menu_item->left_widget = NULL;
   menu_item->is_selected = ETK_FALSE;

   etk_signal_connect_by_code(ETK_WIDGET_REALIZED_SIGNAL, ETK_OBJECT(menu_item), ETK_CALLBACK(_etk_menu_item_realized_cb), NULL);
}

/* Menu_Item: Destroys the menu item */
static void _etk_menu_item_destructor(Etk_Menu_Item *menu_item)
{
   if (!menu_item)
      return;
   free(menu_item->label);
}

/* Menu_Item_Ckeck: Initializes the check menu item */
static void _etk_menu_item_check_constructor(Etk_Menu_Item_Check *check_item)
{
   Etk_Menu_Item *menu_item;

   if (!(menu_item = ETK_MENU_ITEM(check_item)))
      return;

   check_item->active = ETK_FALSE;
   check_item->toggled_handler = _etk_menu_item_check_toggled_handler;
   check_item->active_set = _etk_menu_item_check_active_set;

   menu_item->left_widget = etk_widget_new(ETK_WIDGET_TYPE,
      "theme-group", ETK_IS_MENU_ITEM_RADIO(check_item) ? "radiobox" : "checkbox",
      "theme-parent", check_item, "pass-mouse-events", ETK_TRUE, "visible", ETK_TRUE, NULL);
   etk_widget_parent_set(menu_item->left_widget, ETK_WIDGET(menu_item));
   etk_widget_swallow_widget(ETK_WIDGET(menu_item), "etk.swallow.left_widget", menu_item->left_widget);

   etk_widget_theme_signal_emit(ETK_WIDGET(menu_item), "etk,action,show,left_widget", ETK_TRUE);
   if (check_item->active)
      etk_widget_theme_signal_emit(menu_item->left_widget, "etk,state,on", ETK_FALSE);
   else
      etk_widget_theme_signal_emit(menu_item->left_widget, "etk,state,off", ETK_FALSE);

   etk_signal_connect_by_code(ETK_WIDGET_REALIZED_SIGNAL, ETK_OBJECT(menu_item->left_widget),
      ETK_CALLBACK(_etk_menu_item_check_box_realized_cb), menu_item);
   etk_signal_connect_by_code(ETK_MENU_ITEM_ACTIVATED_SIGNAL, ETK_OBJECT(menu_item),
      ETK_CALLBACK(_etk_menu_item_check_activated_cb), menu_item);
}

/* Menu_Item_Radio: Initializes the radio menu item */
static void _etk_menu_item_radio_constructor(Etk_Menu_Item_Radio *radio_item)
{
   if (!radio_item)
      return;

   radio_item->group = NULL;
   radio_item->can_uncheck = ETK_FALSE;
   ETK_MENU_ITEM_CHECK(radio_item)->active_set = _etk_menu_item_radio_active_set;
}

/* Menu_Item_Radio: Destroys the radio menu item */
static void _etk_menu_item_radio_destructor(Etk_Menu_Item_Radio *radio_item)
{
   if (!radio_item || !radio_item->group)
      return;

   *radio_item->group = eina_list_remove(*radio_item->group, radio_item);
   if (!(*radio_item->group))
      free(radio_item->group);
   else if (ETK_MENU_ITEM_CHECK(radio_item)->active)
      etk_menu_item_check_active_set(ETK_MENU_ITEM_CHECK((*radio_item->group)->data), ETK_TRUE);
}

/* Menu_Item: Sets the property whose id is "property_id" to the value "value" */
static void _etk_menu_item_property_set(Etk_Object *object, int property_id, Etk_Property_Value *value)
{
   Etk_Menu_Item *menu_item;

   if (!(menu_item = ETK_MENU_ITEM(object)) || !value)
      return;

   switch (property_id)
   {
      case ETK_MENU_ITEM_LABEL_PROPERTY:
         etk_menu_item_label_set(menu_item, etk_property_value_string_get(value));
         break;
      case ETK_MENU_ITEM_SUBMENU_PROPERTY:
         etk_menu_item_submenu_set(menu_item, ETK_MENU(etk_property_value_object_get(value)));
         break;
      default:
         break;
   }
}

/* Menu_Item: Gets the value of the property whose id is "property_id" */
static void _etk_menu_item_property_get(Etk_Object *object, int property_id, Etk_Property_Value *value)
{
   Etk_Menu_Item *menu_item;

   if (!(menu_item = ETK_MENU_ITEM(object)) || !value)
      return;

   switch (property_id)
   {
      case ETK_MENU_ITEM_LABEL_PROPERTY:
         etk_property_value_string_set(value, menu_item->label);
         break;
      case ETK_MENU_ITEM_SUBMENU_PROPERTY:
         etk_property_value_object_set(value, ETK_OBJECT(menu_item->submenu));
         break;
      default:
         break;
   }
}

/* Menu_Item_Image: Sets the property whose id is "property_id" to the value "value" */
static void _etk_menu_item_image_property_set(Etk_Object *object, int property_id, Etk_Property_Value *value)
{
   Etk_Menu_Item_Image *image_item;

   if (!(image_item = ETK_MENU_ITEM_IMAGE(object)) || !value)
      return;

   switch (property_id)
   {
      case ETK_MENU_ITEM_IMAGE_PROPERTY:
         etk_menu_item_image_set(image_item, ETK_IMAGE(etk_property_value_object_get(value)));
         break;
      default:
         break;
   }
}

/* Menu_Item_Image: Gets the value of the property whose id is "property_id" */
static void _etk_menu_item_image_property_get(Etk_Object *object, int property_id, Etk_Property_Value *value)
{
   Etk_Menu_Item *menu_item;

   if (!(menu_item = ETK_MENU_ITEM(object)) || !value)
      return;

   switch (property_id)
   {
      case ETK_MENU_ITEM_IMAGE_PROPERTY:
         etk_property_value_object_set(value, ETK_OBJECT(menu_item->left_widget));
         break;
      default:
         break;
   }
}

/* Menu_Item_Check: Sets the property whose id is "property_id" to the value "value" */
static void _etk_menu_item_check_property_set(Etk_Object *object, int property_id, Etk_Property_Value *value)
{
   Etk_Menu_Item_Check *check_item;

   if (!(check_item = ETK_MENU_ITEM_CHECK(object)) || !value)
      return;

   switch (property_id)
   {
      case ETK_MENU_ITEM_ACTIVE_PROPERTY:
         etk_menu_item_check_active_set(check_item, etk_property_value_bool_get(value));
         break;
      default:
         break;
   }
}

/* Menu_Item_Check: Gets the value of the property whose id is "property_id" */
static void _etk_menu_item_check_property_get(Etk_Object *object, int property_id, Etk_Property_Value *value)
{
   Etk_Menu_Item_Check *check_item;

   if (!(check_item = ETK_MENU_ITEM_CHECK(object)) || !value)
      return;

   switch (property_id)
   {
      case ETK_MENU_ITEM_ACTIVE_PROPERTY:
         etk_property_value_bool_set(value, check_item->active);
         break;
      default:
         break;
   }
}

/* Menu_Item_Radio: Sets the property whose id is "property_id" to the value "value" */
static void _etk_menu_item_radio_property_set(Etk_Object *object, int property_id, Etk_Property_Value *value)
{
   Etk_Menu_Item_Radio *radio_item;

   if (!(radio_item = ETK_MENU_ITEM_RADIO(object)) || !value)
      return;

   switch (property_id)
   {
      case ETK_MENU_ITEM_GROUP_PROPERTY:
         etk_menu_item_radio_group_set(radio_item, etk_property_value_pointer_get(value));
         break;
      default:
         break;
   }
}

/* Menu_Item_Radio: Gets the value of the property whose id is "property_id" */
static void _etk_menu_item_radio_property_get(Etk_Object *object, int property_id, Etk_Property_Value *value)
{
   Etk_Menu_Item_Radio *radio_item;

   if (!(radio_item = ETK_MENU_ITEM_RADIO(object)) || !value)
      return;

   switch (property_id)
   {
      case ETK_MENU_ITEM_GROUP_PROPERTY:
         etk_property_value_pointer_set(value, radio_item->group);
         break;
      default:
         break;
   }
}

/**************************
 *
 * Callbacks and handlers
 *
 **************************/

/* Called when the menu item is realized */
static Etk_Bool _etk_menu_item_realized_cb(Etk_Object *object, void *data)
{
   Etk_Menu_Item *menu_item;

   if (!(menu_item = ETK_MENU_ITEM(object)))
      return ETK_TRUE;

   etk_widget_theme_part_text_set(ETK_WIDGET(menu_item), "etk.text.label", menu_item->label ? menu_item->label : "");
   if (menu_item->left_widget)
      etk_widget_swallow_widget(ETK_WIDGET(menu_item), "etk.swallow.left_widget", menu_item->left_widget);
   /* TODO: emit "show left_widget", "show arrow", ... ?? */

   return ETK_TRUE;
}

/* Called when the checkbox of the check item is realized */
static Etk_Bool _etk_menu_item_check_box_realized_cb(Etk_Object *object, void *data)
{
   Etk_Menu_Item *menu_item;

   if (!(menu_item = ETK_MENU_ITEM(data)) || !menu_item->left_widget)
      return ETK_TRUE;

   if (ETK_MENU_ITEM_CHECK(menu_item)->active)
      etk_widget_theme_signal_emit(menu_item->left_widget, "etk,state,on", ETK_FALSE);
   else
      etk_widget_theme_signal_emit(menu_item->left_widget, "etk,state,off", ETK_FALSE);

   return ETK_TRUE;
}

/* Called when the check item is activated */
static Etk_Bool _etk_menu_item_check_activated_cb(Etk_Object *object, void *data)
{
   Etk_Menu_Item_Check *check_item;

   if (!(check_item = ETK_MENU_ITEM_CHECK(object)))
      return ETK_TRUE;
   etk_menu_item_check_active_set(check_item, !check_item->active);
   return ETK_TRUE;
}

/* Default handler for the "selected" signal */
static Etk_Bool _etk_menu_item_selected_handler(Etk_Menu_Item *menu_item)
{
   if (!menu_item)
      return ETK_TRUE;
   etk_widget_theme_signal_emit(ETK_WIDGET(menu_item), "etk,state,selected", ETK_FALSE);
   etk_widget_theme_signal_emit(ETK_WIDGET(menu_item->left_widget), "etk,state,selected", ETK_FALSE);
   return ETK_TRUE;
}

/* Default handler for the "unselected" signal */
static Etk_Bool _etk_menu_item_unselected_handler(Etk_Menu_Item *menu_item)
{
   if (!menu_item)
      return ETK_TRUE;
   etk_widget_theme_signal_emit(ETK_WIDGET(menu_item), "etk,state,unselected", ETK_FALSE);
   etk_widget_theme_signal_emit(ETK_WIDGET(menu_item->left_widget), "etk,state,unselected", ETK_FALSE);
   return ETK_TRUE;
}

/* Default handler for the "activated" signal */
static Etk_Bool _etk_menu_item_activated_handler(Etk_Menu_Item *menu_item)
{
   if (!menu_item)
      return ETK_TRUE;

   /* TODO: rename signal */
   etk_widget_theme_signal_emit(ETK_WIDGET(menu_item), "etk,state,activate", ETK_FALSE);
   etk_widget_theme_signal_emit(ETK_WIDGET(menu_item->left_widget), "etk,state,activate", ETK_FALSE);
   return ETK_TRUE;
}

/* Default handler for the "toggled" signal */
static Etk_Bool _etk_menu_item_check_toggled_handler(Etk_Menu_Item_Check *check_item)
{
   if (check_item && ETK_MENU_ITEM(check_item)->left_widget)
   {
      if (check_item->active)
         etk_widget_theme_signal_emit(ETK_MENU_ITEM(check_item)->left_widget, "etk,state,on", ETK_FALSE);
      else
         etk_widget_theme_signal_emit(ETK_MENU_ITEM(check_item)->left_widget, "etk,state,off", ETK_FALSE);
   }
   return ETK_TRUE;
}

/**************************
 *
 * Private functions
 *
 **************************/

/* Behavior of the "active_set" function for a check menu item */
static void _etk_menu_item_check_active_set(Etk_Menu_Item_Check *check_item, Etk_Bool active)
{
   if (!check_item || check_item->active == active)
      return;

   check_item->active = active;
   etk_signal_emit(ETK_MENU_ITEM_CHECK_TOGGLED_SIGNAL, ETK_OBJECT(check_item));
   etk_object_notify(ETK_OBJECT(check_item), "active");
}

/* Behavior of the "active_set" function for a radio menu item */
static void _etk_menu_item_radio_active_set(Etk_Menu_Item_Check *check_item, Etk_Bool active)
{
   Etk_Menu_Item_Radio *radio_item;
   Etk_Menu_Item_Check *ci;
   Eina_List *l;

   if (!(radio_item = ETK_MENU_ITEM_RADIO(check_item)) || check_item->active == active)
      return;

   if (!check_item->active || (check_item->active && radio_item->can_uncheck))
   {
      check_item->active = active;
      etk_signal_emit(ETK_MENU_ITEM_CHECK_TOGGLED_SIGNAL, ETK_OBJECT(check_item));
      etk_object_notify(ETK_OBJECT(check_item), "active");

      if (check_item->active)
      {
         /* Uncheck the previously checked item of the group */
         for (l = *radio_item->group; l; l = l->next)
         {
            ci = ETK_MENU_ITEM_CHECK(l->data);
            if (ci != check_item && ci->active)
            {
               ETK_MENU_ITEM_RADIO(ci)->can_uncheck = ETK_TRUE;
               etk_menu_item_check_active_set(ci, ETK_FALSE);
            }
         }
      }
      radio_item->can_uncheck = ETK_FALSE;
   }
}

/** @} */

/**************************
 *
 * Documentation
 *
 **************************/

/**
 * @addtogroup Etk_Menu_Item
 *
 * @image html widgets/menu_item.png
 * Several widgets inherits from Etk_Menu_Item:
 *   - Etk_Menu_Item_Separator: a menu item without label which used to separate two menu items
 *   - Etk_Menu_Item_Image: a menu item with a label and an icon on the left of the label
 *   - Etk_Menu_Item_Check: a menu item with a label and a checkbox on the left of the label
 *   - Etk_Menu_Item_Radio: a menu item with a label and a radio on the left of the label
 * @n @n
 * A menu can also be attached to the menu item with etk_menu_item_submenu_set(), and it will be popped up when
 * the menu item is selected.
 *
 * \par Object Hierarchy:
 * - Etk_Object
 *   - Etk_Widget
 *     - Etk_Menu_Item
 *       - Etk_Menu_Item_Separator
 *       - Etk_Menu_Item_Image
 *       - Etk_Menu_Item_Check
 *         - Etk_Menu_Item_Radio
 *
 * \par Signals:
 * @signal_name "selected": Emitted when the menu item is selected
 * @signal_cb Etk_Bool callback(Etk_Menu_Item *menu_item, void *data)
 * @signal_arg menu_item: the menu item which has been selected
 * @signal_data
 * \par
 * @signal_name "unselected": Emitted when the menu item is unselected
 * @signal_cb Etk_Bool callback(Etk_Menu_Item *menu_item, void *data)
 * @signal_arg menu_item: the menu item which has been unselected
 * @signal_data
 * \par
 * @signal_name "activated": Emitted when the menu item is activated (mainly when it has been clicked)
 * @signal_cb Etk_Bool callback(Etk_Menu_Item *menu_item, void *data)
 * @signal_arg menu_item: the menu item which has been activated
 * @signal_data
 * \par
 * @signal_name "toggled" (only for Etk_Menu_Item_Check and Etk_Menu_Item_Radio):
 * Emitted when the menu item (check or radio) has been toggled
 * @signal_cb Etk_Bool callback(Etk_Menu_Item_Check *check_item, void *data)
 * @signal_arg menu_item: the menu item which has been toggled
 * @signal_data
 *
 * \par Properties:
 * @prop_name "label": The text of the label of the menu item
 * @prop_type String (char *)
 * @prop_rw
 * @prop_val NULL
 * \par
 * @prop_name "submenu": The submenu attached to the menu item
 * @prop_type Pointer (Etk_Menu *)
 * @prop_rw
 * @prop_val NULL
 * \par
 * @prop_name "image" (only for Etk_Menu_Item_Image): The image widget of the menu item
 * @prop_type Pointer (Etk_Image *)
 * @prop_rw
 * @prop_val NULL
 * \par
 * @prop_name "active" (only for Etk_Menu_Item_Check and Etk_Menu_Item_Radio):
 * Whether or not the menu item is active (i.e. toggled)
 * @prop_type Boolean
 * @prop_rw
 * @prop_val ETK_FALSE
 * \par
 * @prop_name "group" (only for Etk_Menu_Item_Radio): The group of menu items which the menu item belongs to
 * @prop_type Pointer (Eina_List *)
 * @prop_rw
 * @prop_val NULL
 */
