/** @file etk_menu_item.c */
#include "etk_menu_item.h"
#include <stdlib.h>
#include <string.h>
#include "etk_signal.h"
#include "etk_signal_callback.h"
#include "etk_utils.h"

/**
 * @addtogroup Etk_Menu_Item
 * @{
 */

enum _Etk_Menu_Item_Signal_Id
{
   ETK_MENU_ITEM_SELECT_SIGNAL,
   ETK_MENU_ITEM_DESELECT_SIGNAL,
   ETK_MENU_ITEM_ACTIVATE_SIGNAL,
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
static void _etk_menu_item_select_handler(Etk_Menu_Item *menu_item);
static void _etk_menu_item_deselect_handler(Etk_Menu_Item *menu_item);

static Etk_Signal *_etk_menu_item_signals[ETK_MENU_ITEM_NUM_SIGNALS];

/**************************
 *
 * Implementation
 *
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
      menu_item_type = etk_type_new("Etk_Menu_Item", ETK_CONTAINER_TYPE, sizeof(Etk_Menu_Item), ETK_CONSTRUCTOR(_etk_menu_item_constructor), ETK_DESTRUCTOR(_etk_menu_item_destructor));

      _etk_menu_item_signals[ETK_MENU_ITEM_SELECT_SIGNAL] = etk_signal_new("select", menu_item_type, ETK_MEMBER_OFFSET(Etk_Menu_Item, select), etk_marshaller_VOID__VOID, NULL, NULL);
      _etk_menu_item_signals[ETK_MENU_ITEM_DESELECT_SIGNAL] = etk_signal_new("deselect", menu_item_type, ETK_MEMBER_OFFSET(Etk_Menu_Item, deselect), etk_marshaller_VOID__VOID, NULL, NULL);
      _etk_menu_item_signals[ETK_MENU_ITEM_ACTIVATE_SIGNAL] = etk_signal_new("activate", menu_item_type, ETK_MEMBER_OFFSET(Etk_Menu_Item, activate), etk_marshaller_VOID__VOID, NULL, NULL);
      
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
   return etk_widget_new(ETK_MENU_ITEM_TYPE, "theme_group", "menu_item", NULL);
}

/**
 * @brief Creates a new menu item with a label
 * @param label the label to set to the new menu item
 * @return Returns the new menu item widget
 */
Etk_Widget *etk_menu_item_new_with_label(const char *label)
{
   return etk_widget_new(ETK_MENU_ITEM_TYPE, "theme_group", "menu_item", "label", label, NULL);
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
 * @brief Emits the "select" signal which will select the menu item
 * @param menu_item a menu_item
 */
void etk_menu_item_select(Etk_Menu_Item *menu_item)
{
   if (!menu_item)
      return;
   etk_signal_emit(_etk_menu_item_signals[ETK_MENU_ITEM_SELECT_SIGNAL], ETK_OBJECT(menu_item), NULL);
}

/**
 * @brief Emits the "deselect" signal which will deselect the menu item
 * @param menu_item a menu_item
 */
void etk_menu_item_deselect(Etk_Menu_Item *menu_item)
{
   if (!menu_item)
      return;
   etk_signal_emit(_etk_menu_item_signals[ETK_MENU_ITEM_DESELECT_SIGNAL], ETK_OBJECT(menu_item), NULL);
}

/**
 * @brief Emits the "activate" signal which will activate the menu item
 * @param menu_item a menu_item
 */
void etk_menu_item_activate(Etk_Menu_Item *menu_item)
{
   if (!menu_item)
      return;
   etk_signal_emit(_etk_menu_item_signals[ETK_MENU_ITEM_ACTIVATE_SIGNAL], ETK_OBJECT(menu_item), NULL);
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
   menu_item->child = submenu;
}

/* TODO */
void etk_menu_item_image_set(Etk_Menu_Item *menu_item, Etk_Image *image)
{
   Etk_Widget *image_widget;
   
   if (!menu_item)
      return;
   
   if (menu_item->image)
   {
      etk_widget_parent_set(ETK_WIDGET(menu_item->image), NULL);
      menu_item->image = NULL;
      etk_widget_size_recalc_queue(ETK_WIDGET(menu_item));
   }
   
   if ((image_widget = ETK_WIDGET(image)))
   {
      if (image_widget->parent)
         etk_container_remove(image_widget->parent, image_widget);

      /* TODO: disconnect */
      etk_signal_connect_after("realize", ETK_OBJECT(image_widget), ETK_CALLBACK(_etk_menu_item_image_realize_cb), menu_item);
      etk_widget_parent_set(image_widget, ETK_CONTAINER(menu_item));
      etk_widget_theme_object_signal_emit(ETK_WIDGET(menu_item), "");
      menu_item->image = image;
   }
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

   menu_item->select = _etk_menu_item_select_handler;
   menu_item->deselect = _etk_menu_item_deselect_handler;
   menu_item->activate = NULL;
   
   menu_item->child = NULL;
   menu_item->parent = NULL;
   menu_item->label = NULL;
   menu_item->image = NULL;
   menu_item->selected = FALSE;
   
   etk_signal_connect_after("realize", ETK_OBJECT(menu_item), ETK_CALLBACK(_etk_menu_item_realize_cb), NULL);
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
}

/* Called when the image of the menu is realized */
static void _etk_menu_item_image_realize_cb(Etk_Object *object, void *data)
{
   Etk_Menu_Item *menu_item;

   if (!(menu_item = ETK_MENU_ITEM(data)) || !menu_item->image || ETK_OBJECT(menu_item->image) != object)
      return;

   if (ETK_WIDGET(menu_item)->realized && ETK_WIDGET(menu_item->image)->realized)
      etk_widget_swallow_widget(ETK_WIDGET(menu_item), "image_swallow", ETK_WIDGET(menu_item->image));
}

/* Default handler for the "select" signal */
static void _etk_menu_item_select_handler(Etk_Menu_Item *menu_item)
{
   if (!menu_item || menu_item->selected)
      return;
   
   menu_item->selected = TRUE;
   etk_widget_theme_object_signal_emit(ETK_WIDGET(menu_item), "select");
}

/* Default handler for the "deselect" signal */
static void _etk_menu_item_deselect_handler(Etk_Menu_Item *menu_item)
{
   if (!menu_item || !menu_item->selected)
      return;
   
   etk_widget_theme_object_signal_emit(ETK_WIDGET(menu_item), "deselect");
   menu_item->selected = FALSE;
}

/** @} */
