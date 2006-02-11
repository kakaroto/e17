/** @file etk_menu_item.c */
#include "etk_menu_item.h"
#include <stdlib.h>
#include <string.h>
#include "etk_menu_shell.h"
#include "etk_signal.h"
#include "etk_signal_callback.h"
#include "etk_utils.h"

/**
 * @addtogroup Etk_Menu_Item
 * @{
 */

enum _Etk_Menu_Item_Signal_Id
{
   ETK_MENU_ITEM_SELECTED_SIGNAL,
   ETK_MENU_ITEM_DESELECTED_SIGNAL,
   ETK_MENU_ITEM_ACTIVATED_SIGNAL,
   ETK_MENU_ITEM_SUBMENU_POPPED_DOWN_SIGNAL,
   ETK_MENU_ITEM_NUM_SIGNALS
};

enum _Etk_Menu_Item_Property_Id
{
   ETK_MENU_ITEM_LABEL_PROPERTY
};

static void _etk_menu_item_constructor(Etk_Menu_Item *menu_item);
static void _etk_menu_item_destructor(Etk_Menu_Item *menu_item);
static void _etk_menu_item_property_set(Etk_Object *object, int property_id, Etk_Property_Value *value);
static void _etk_menu_item_property_get(Etk_Object *object, int property_id, Etk_Property_Value *value);
static void _etk_menu_item_realize_cb(Etk_Object *object, void *data);
static void _etk_menu_item_image_realize_cb(Etk_Object *object, void *data);
static void _etk_menu_item_right_widget_realize_cb(Etk_Object *object, void *data);
static void _etk_menu_item_selected_handler(Etk_Menu_Item *menu_item);
static void _etk_menu_item_deselected_handler(Etk_Menu_Item *menu_item);
static void _etk_menu_item_activated_handler(Etk_Menu_Item *menu_item);

static Etk_Signal *_etk_menu_item_signals[ETK_MENU_ITEM_NUM_SIGNALS];

/**************************
 *
 * Implementation
 *
 **************************/

/**************************
 * Menu Item
 **************************/

/**
 * @brief Gets the type of an Etk_Menu_Item
 * @return Returns the type on an Etk_Menu_Item
 */
Etk_Type *etk_menu_item_type_get()
{
   static Etk_Type *menu_item_type = NULL;

   if (!menu_item_type)
   {
      menu_item_type = etk_type_new("Etk_Menu_Item", ETK_WIDGET_TYPE, sizeof(Etk_Menu_Item), ETK_CONSTRUCTOR(_etk_menu_item_constructor), ETK_DESTRUCTOR(_etk_menu_item_destructor));

      _etk_menu_item_signals[ETK_MENU_ITEM_SELECTED_SIGNAL] = etk_signal_new("selected", menu_item_type, ETK_MEMBER_OFFSET(Etk_Menu_Item, selected), etk_marshaller_VOID__VOID, NULL, NULL);
      _etk_menu_item_signals[ETK_MENU_ITEM_DESELECTED_SIGNAL] = etk_signal_new("deselected", menu_item_type, ETK_MEMBER_OFFSET(Etk_Menu_Item, deselected), etk_marshaller_VOID__VOID, NULL, NULL);
      _etk_menu_item_signals[ETK_MENU_ITEM_ACTIVATED_SIGNAL] = etk_signal_new("activated", menu_item_type, ETK_MEMBER_OFFSET(Etk_Menu_Item, activated), etk_marshaller_VOID__VOID, NULL, NULL);
      _etk_menu_item_signals[ETK_MENU_ITEM_SUBMENU_POPPED_DOWN_SIGNAL] = etk_signal_new("submenu_popped_down", menu_item_type, -1, etk_marshaller_VOID__VOID, NULL, NULL);
      
      etk_type_property_add(menu_item_type, "label", ETK_MENU_ITEM_LABEL_PROPERTY, ETK_PROPERTY_STRING, ETK_PROPERTY_READABLE_WRITABLE,  etk_property_value_string(NULL));
      
      menu_item_type->property_set = _etk_menu_item_property_set;
      menu_item_type->property_get = _etk_menu_item_property_get;
   }

   return menu_item_type;
}

/**
 * @brief Creates a new menu item
 * @return Returns the new menu item widget
 */
Etk_Widget *etk_menu_item_new()
{
   return etk_widget_new(ETK_MENU_ITEM_TYPE, "theme_group", "menu_item", "visible", ETK_TRUE, NULL);
}

/**
 * @brief Creates a new menu item with a label
 * @param label the label to set to the new menu item
 * @return Returns the new menu item widget
 */
Etk_Widget *etk_menu_item_new_with_label(const char *label)
{
   return etk_widget_new(ETK_MENU_ITEM_TYPE, "theme_group", "menu_item", "visible", ETK_TRUE, "label", label, NULL);
}

/**
 * @brief Sets the label of the menu item
 * @param menu_item a menu item
 * @param label the label to set
 */
void etk_menu_item_label_set(Etk_Menu_Item *menu_item, const char *label)
{
   if (!menu_item || menu_item->label == label)
      return;
   
   free(menu_item->label);
   if (label)
   {
      menu_item->label = strdup(label);
      etk_widget_theme_object_part_text_set(ETK_WIDGET(menu_item), "label", label);
   }
   else
   {
      menu_item->label = NULL;
      etk_widget_theme_object_part_text_set(ETK_WIDGET(menu_item), "label", "");
   }
   
   etk_object_notify(ETK_OBJECT(menu_item), "label");
}

/**
 * @brief Gets the label of the menu item
 * @param menu_item a menu item
 * @return Returns the label of the menu item
 */
const char *etk_menu_item_label_get(Etk_Menu_Item *menu_item)
{
   if (!menu_item)
      return NULL;
   return menu_item->label;
}

/**
 * @brief Selects the menu item
 * @param menu_item a menu_item
 */
void etk_menu_item_select(Etk_Menu_Item *menu_item)
{
   if (!menu_item || menu_item->is_selected)
      return;
   
   menu_item->is_selected = ETK_TRUE;
   etk_signal_emit(_etk_menu_item_signals[ETK_MENU_ITEM_SELECTED_SIGNAL], ETK_OBJECT(menu_item), NULL);
}

/**
 * @brief Deselects the menu item
 * @param menu_item a menu_item
 */
void etk_menu_item_deselect(Etk_Menu_Item *menu_item)
{
   if (!menu_item || !menu_item->is_selected)
      return;
   
   menu_item->is_selected = ETK_FALSE;
   etk_signal_emit(_etk_menu_item_signals[ETK_MENU_ITEM_DESELECTED_SIGNAL], ETK_OBJECT(menu_item), NULL);
}

/**
 * @brief Activates the menu item
 * @param menu_item a menu_item
 */
void etk_menu_item_activate(Etk_Menu_Item *menu_item)
{
   if (!menu_item)
      return;
   etk_signal_emit(_etk_menu_item_signals[ETK_MENU_ITEM_ACTIVATED_SIGNAL], ETK_OBJECT(menu_item), NULL);
}

/**
 * @brief Sets the submenu of the menu item: the submenu will be popped up when the menu item is activated
 * @param menu_item a menu item
 * @param submenu the submenu to set
 */
void etk_menu_item_submenu_set(Etk_Menu_Item *menu_item, Etk_Menu *submenu)
{
   if (!menu_item)
      return;
   menu_item->submenu = submenu;
   if (menu_item->submenu)
      ETK_MENU_SHELL(menu_item->submenu)->parent = menu_item;
   
   if (menu_item->submenu && !menu_item->right_widget)
   {
      menu_item->right_widget = etk_widget_new(ETK_WIDGET_TYPE, "theme_group", "menu_arrow", NULL);
      /* TODO: disconnect */
      etk_signal_connect("realize", ETK_OBJECT(menu_item->right_widget), ETK_CALLBACK(_etk_menu_item_right_widget_realize_cb), menu_item);
      etk_widget_parent_set(menu_item->right_widget, ETK_WIDGET(menu_item));
      etk_widget_pass_events_set(menu_item->right_widget, ETK_TRUE);
      menu_item->right_widget_is_arrow = ETK_TRUE;
   }
   else if (!menu_item->submenu && menu_item->right_widget && menu_item->right_widget_is_arrow)
   {
      etk_object_destroy(ETK_OBJECT(menu_item->right_widget));
      menu_item->right_widget = NULL;
      menu_item->right_widget_is_arrow = ETK_FALSE;
   }
   
   etk_menu_item_right_swallow_show(menu_item, menu_item->submenu != NULL);
   if (menu_item->parent)
      etk_menu_shell_update(menu_item->parent);
}

/**
 * @brief Sets the image of the menu item. It will be displayed at the left of the label
 * @param menu_item a menu_item
 * @param image the image to set (NULL to unset the image)
 */
void etk_menu_item_image_set(Etk_Menu_Item *menu_item, Etk_Image *image)
{
   Etk_Widget *image_widget;
   
   if (!menu_item)
      return;
   
   if (menu_item->image)
   {
      etk_widget_parent_set(ETK_WIDGET(menu_item->image), NULL);
      menu_item->image = NULL;
   }
   
   if ((image_widget = ETK_WIDGET(image)))
   {
      if (image_widget->parent && ETK_IS_CONTAINER(image_widget->parent))
         etk_container_remove(ETK_CONTAINER(image_widget->parent), image_widget);

      /* TODO: disconnect */
      etk_signal_connect("realize", ETK_OBJECT(image_widget), ETK_CALLBACK(_etk_menu_item_image_realize_cb), menu_item);
      etk_widget_parent_set(image_widget, ETK_WIDGET(menu_item));
      etk_widget_pass_events_set(image_widget, ETK_TRUE);
      menu_item->image = image;
   }
   
   etk_menu_item_right_swallow_show(menu_item, menu_item->image != NULL);
   if (menu_item->parent)
      etk_menu_shell_update(menu_item->parent);
}

/**
 * @brief Show the image swallow area of the menu item. It's a function used by etk_menu, do not use it manually!
 * @param menu_item a menu item
 * @param show ETK_TRUE whether the image swallow area should be show or not
 */
void etk_menu_item_image_show(Etk_Menu_Item *menu_item, Etk_Bool show)
{
   if (!menu_item || menu_item->show_image == show)
      return;
   
   if (show)
      etk_widget_theme_object_signal_emit(ETK_WIDGET(menu_item), "image_show");
   else
      etk_widget_theme_object_signal_emit(ETK_WIDGET(menu_item), "image_hide");
   menu_item->show_image = show;
}

/**
 * @brief Show the right swallow area of the menu item. It's a function used by Etk_Menu, do not use it manually!
 * @param menu_item a menu item
 * @param show ETK_TRUE whether the right swallow area should be show or not
 */
void etk_menu_item_right_swallow_show(Etk_Menu_Item *menu_item, Etk_Bool show)
{
   if (!menu_item || menu_item->show_right_swallow == show)
      return;
   
   if (show)
      etk_widget_theme_object_signal_emit(ETK_WIDGET(menu_item), "right_swallow_show");
   else
      etk_widget_theme_object_signal_emit(ETK_WIDGET(menu_item), "right_swallow_hide");
   menu_item->show_right_swallow = show;
}

/**************************
 * Menu Separator
 **************************/

/**
 * @brief Gets the type of an Etk_Menu_Separator
 * @return Returns the type on an Etk_Menu_Separator
 */
Etk_Type *etk_menu_separator_type_get()
{
   static Etk_Type *menu_separator_type = NULL;

   if (!menu_separator_type)
      menu_separator_type = etk_type_new("Etk_Menu_Separator", ETK_MENU_ITEM_TYPE, sizeof(Etk_Menu_Separator), NULL, NULL);

   return menu_separator_type;
}

/**
 * @brief Creates a new menu separator
 * @return Returns the new menu separator widget
 * @note Unlike the other widgets, You don't need to call etk_widget_shown, the widget is automaticall shown at its creation
 */
Etk_Widget *etk_menu_separator_new()
{
   return etk_widget_new(ETK_MENU_SEPARATOR_TYPE, "theme_group", "menu_separator", "visible", ETK_TRUE, NULL);
}

/**************************
 *
 * Etk specific functions
 *
 **************************/

/* Initializes the members */
static void _etk_menu_item_constructor(Etk_Menu_Item *menu_item)
{
   if (!menu_item)
      return;

   menu_item->selected = _etk_menu_item_selected_handler;
   menu_item->deselected = _etk_menu_item_deselected_handler;
   menu_item->activated = _etk_menu_item_activated_handler;
   
   menu_item->submenu = NULL;
   menu_item->parent = NULL;
   menu_item->label = NULL;
   menu_item->image = NULL;
   menu_item->right_widget = NULL;
   
   menu_item->is_selected = ETK_FALSE;
   menu_item->show_image = ETK_FALSE;
   menu_item->show_right_swallow = ETK_FALSE;
   
   etk_signal_connect("realize", ETK_OBJECT(menu_item), ETK_CALLBACK(_etk_menu_item_realize_cb), NULL);
}

/* Destroys the menu item */
static void _etk_menu_item_destructor(Etk_Menu_Item *menu_item)
{
   if (!menu_item)
      return;
   free(menu_item->label);
}

/* Sets the property whose id is "property_id" to the value "value" */
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
      default:
         break;
   }
}

/* Gets the value of the property whose id is "property_id" */
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
static void _etk_menu_item_realize_cb(Etk_Object *object, void *data)
{
   Etk_Menu_Item *menu_item;
   
   if (!(menu_item = ETK_MENU_ITEM(object)))
      return;
   
   if (menu_item->label)
      etk_widget_theme_object_part_text_set(ETK_WIDGET(menu_item), "label", menu_item->label);
   else
      etk_widget_theme_object_part_text_set(ETK_WIDGET(menu_item), "label", "");
   
   if (menu_item->image && ETK_WIDGET(menu_item->image)->realized)
      etk_widget_swallow_widget(ETK_WIDGET(menu_item), "image_swallow", ETK_WIDGET(menu_item->image));
   if (menu_item->right_widget && ETK_WIDGET(menu_item->right_widget)->realized)
      etk_widget_swallow_widget(ETK_WIDGET(menu_item), "right_swallow", ETK_WIDGET(menu_item->right_widget));
   
   if (menu_item->show_image)
      etk_widget_theme_object_signal_emit(ETK_WIDGET(menu_item), "image_show");
   if (menu_item->show_right_swallow)
      etk_widget_theme_object_signal_emit(ETK_WIDGET(menu_item), "right_swallow_show");
}

/* Called when the image of the menu is realized */
static void _etk_menu_item_image_realize_cb(Etk_Object *object, void *data)
{
   Etk_Menu_Item *menu_item;

   if (!(menu_item = ETK_MENU_ITEM(data)) || !menu_item->image)
      return;

   if (ETK_WIDGET(menu_item)->realized)
      etk_widget_swallow_widget(ETK_WIDGET(menu_item), "image_swallow", ETK_WIDGET(menu_item->image));
}

/* Called when the right widget of the menu is realized */
static void _etk_menu_item_right_widget_realize_cb(Etk_Object *object, void *data)
{
   Etk_Menu_Item *menu_item;

   if (!(menu_item = ETK_MENU_ITEM(data)) || !menu_item->right_widget)
      return;

   if (ETK_WIDGET(menu_item)->realized)
      etk_widget_swallow_widget(ETK_WIDGET(menu_item), "right_swallow", ETK_WIDGET(menu_item->right_widget));
}

/* Default handler for the "selected" signal */
static void _etk_menu_item_selected_handler(Etk_Menu_Item *menu_item)
{
   if (!menu_item)
      return;
   etk_widget_theme_object_signal_emit(ETK_WIDGET(menu_item), "select");
   etk_widget_theme_object_signal_emit(ETK_WIDGET(menu_item->image), "select");
   etk_widget_theme_object_signal_emit(ETK_WIDGET(menu_item->right_widget), "select");
}

/* Default handler for the "deselected" signal */
static void _etk_menu_item_deselected_handler(Etk_Menu_Item *menu_item)
{
   if (!menu_item)
      return;
   etk_widget_theme_object_signal_emit(ETK_WIDGET(menu_item), "deselect");
   etk_widget_theme_object_signal_emit(ETK_WIDGET(menu_item->image), "deselect");
   etk_widget_theme_object_signal_emit(ETK_WIDGET(menu_item->right_widget), "deselect");
}

/* Default handler for the "activated" signal */
static void _etk_menu_item_activated_handler(Etk_Menu_Item *menu_item)
{
   if (!menu_item)
      return;
   etk_widget_theme_object_signal_emit(ETK_WIDGET(menu_item), "activate");
   etk_widget_theme_object_signal_emit(ETK_WIDGET(menu_item->image), "activate");
   etk_widget_theme_object_signal_emit(ETK_WIDGET(menu_item->right_widget), "activate");
}

/** @} */
