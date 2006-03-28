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

enum _Etk_Menu_Item_Check_Signal_Id
{
   ETK_MENU_ITEM_CHECK_TOGGLED_SIGNAL,
   ETK_MENU_ITEM_CHECK_NUM_SIGNALS
};

enum _Etk_Menu_Item_Property_Id
{
   ETK_MENU_ITEM_LABEL_PROPERTY,
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
static void _etk_menu_item_realize_cb(Etk_Object *object, void *data);
static void _etk_menu_item_left_widget_realize_cb(Etk_Object *object, void *data);
static void _etk_menu_item_check_left_widget_realize_cb(Etk_Object *object, void *data);
static void _etk_menu_item_check_activated_cb(Etk_Object *object, void *data);
static void _etk_menu_item_selected_handler(Etk_Menu_Item *menu_item);
static void _etk_menu_item_deselected_handler(Etk_Menu_Item *menu_item);
static void _etk_menu_item_activated_handler(Etk_Menu_Item *menu_item);
static void _etk_menu_item_check_toggled_handler(Etk_Menu_Item_Check *check_item);
static void _etk_menu_item_check_active_set(Etk_Menu_Item_Check *check_item, Etk_Bool active);
static void _etk_menu_item_radio_active_set(Etk_Menu_Item_Check *check_item, Etk_Bool active);

static Etk_Signal *_etk_menu_item_signals[ETK_MENU_ITEM_NUM_SIGNALS];
static Etk_Signal *_etk_menu_item_check_signals[ETK_MENU_ITEM_CHECK_NUM_SIGNALS];

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
      
      etk_type_property_add(menu_item_type, "label", ETK_MENU_ITEM_LABEL_PROPERTY, ETK_PROPERTY_STRING, ETK_PROPERTY_READABLE_WRITABLE, etk_property_value_string(NULL));
      
      menu_item_type->property_set = _etk_menu_item_property_set;
      menu_item_type->property_get = _etk_menu_item_property_get;
   }

   return menu_item_type;
}

/**
 * @brief Creates a new menu item
 * @return Returns the new menu item widget
 * @note Unlike the other widgets, you don't need to call etk_widget_shown, the widget is automatically shown at its creation
 */
Etk_Widget *etk_menu_item_new()
{
   return etk_widget_new(ETK_MENU_ITEM_TYPE, "theme_group", "menu_item", "visible", ETK_TRUE, NULL);
}

/**
 * @brief Creates a new menu item with a label
 * @param label the label to set to the new menu item
 * @return Returns the new menu item widget
 * @note Unlike the other widgets, You don't need to call etk_widget_shown, the widget is automatically shown at its creation
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
 * @brief Sets the submenu of the menu item: the submenu will be popped up when the menu item is activated
 * @param menu_item a menu item
 * @param submenu the submenu to set
 */
void etk_menu_item_submenu_set(Etk_Menu_Item *menu_item, Etk_Menu *submenu)
{
   if (!menu_item)
      return;
   
   if (menu_item->submenu)
      ETK_MENU_SHELL(menu_item->submenu)->parent = NULL;
   menu_item->submenu = submenu;
   if (menu_item->submenu)
      ETK_MENU_SHELL(menu_item->submenu)->parent = menu_item;
   
   if (menu_item->submenu)
      etk_widget_theme_object_signal_emit(ETK_WIDGET(menu_item), "arrow_show");
   else if (!menu_item->submenu)
      etk_widget_theme_object_signal_emit(ETK_WIDGET(menu_item), "arrow_hide");
   
   etk_widget_size_recalc_queue(ETK_WIDGET(menu_item));
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

/**************************
 * Menu Item Separator
 **************************/

/**
 * @brief Gets the type of an Etk_Menu_Item_Separator
 * @return Returns the type on an Etk_Menu_Item_Separator
 */
Etk_Type *etk_menu_item_separator_type_get()
{
   static Etk_Type *menu_item_separator_type = NULL;

   if (!menu_item_separator_type)
      menu_item_separator_type = etk_type_new("Etk_Menu_Item_Separator", ETK_MENU_ITEM_TYPE, sizeof(Etk_Menu_Item_Separator), NULL, NULL);

   return menu_item_separator_type;
}

/**
 * @brief Creates a new menu separator
 * @return Returns the new menu separator widget
 * @note Unlike the other widgets, You don't need to call etk_widget_shown, the widget is automatically shown at its creation
 */
Etk_Widget *etk_menu_item_separator_new()
{
   return etk_widget_new(ETK_MENU_ITEM_SEPARATOR_TYPE, "theme_group", "menu_separator", "visible", ETK_TRUE, NULL);
}

/**************************
 * Menu Item Image
 **************************/

/**
 * @brief Gets the type of an Etk_Menu_Item_Image
 * @return Returns the type on an Etk_Menu_Item_Image
 */
Etk_Type *etk_menu_item_image_type_get()
{
   static Etk_Type *menu_item_image_type = NULL;

   if (!menu_item_image_type)
   {
      menu_item_image_type = etk_type_new("Etk_Menu_Item_Image", ETK_MENU_ITEM_TYPE, sizeof(Etk_Menu_Item_Image), NULL, NULL);
      
      etk_type_property_add(menu_item_image_type, "image", ETK_MENU_ITEM_IMAGE_PROPERTY, ETK_PROPERTY_POINTER, ETK_PROPERTY_READABLE_WRITABLE, etk_property_value_pointer(NULL));
      
      menu_item_image_type->property_set = _etk_menu_item_image_property_set;
      menu_item_image_type->property_get = _etk_menu_item_image_property_get;
   }

   return menu_item_image_type;
}

/**
 * @brief Creates a new menu item with an image
 * @return Returns the new menu item with an image
 * @note Unlike the other widgets, You don't need to call etk_widget_shown, the widget is automatically shown at its creation
 */
Etk_Widget *etk_menu_item_image_new()
{
   return etk_widget_new(ETK_MENU_ITEM_IMAGE_TYPE, "theme_group", "menu_item", "visible", ETK_TRUE, NULL);
}

/**
 * @brief Creates a new menu item with an image
 * @param label the label to set to the new menu item
 * @return Returns the new menu item with an image
 * @note Unlike the other widgets, You don't need to call etk_widget_shown, the widget is automatically shown at its creation
 */
Etk_Widget *etk_menu_item_image_new_with_label(const char *label)
{
   return etk_widget_new(ETK_MENU_ITEM_IMAGE_TYPE, "theme_group", "menu_item", "label", label, "visible", ETK_TRUE, NULL);
}

/**
 * @brief Sets the image of the menu item. It will be displayed at the left of the label
 * @param image_item a image_item
 * @param image the image to set (NULL to unset the image)
 */
void etk_menu_item_image_set(Etk_Menu_Item_Image *image_item, Etk_Image *image)
{
   Etk_Menu_Item *menu_item;
   Etk_Widget *image_widget;
   
   if (!(menu_item = ETK_MENU_ITEM(image_item)))
      return;
   
   if (menu_item->left_widget)
   {
      etk_widget_parent_set(ETK_WIDGET(menu_item->left_widget), NULL);
      menu_item->left_widget = NULL;
   }
   
   if ((image_widget = ETK_WIDGET(image)))
   {
      if (image_widget->parent && ETK_IS_CONTAINER(image_widget->parent))
         etk_container_remove(ETK_CONTAINER(image_widget->parent), image_widget);

      /* TODO: disconnect */
      menu_item->left_widget = image_widget;
      etk_signal_connect("realize", ETK_OBJECT(menu_item->left_widget), ETK_CALLBACK(_etk_menu_item_left_widget_realize_cb), menu_item);
      etk_widget_parent_set(menu_item->left_widget, ETK_WIDGET(menu_item));
      etk_widget_pass_mouse_events_set(menu_item->left_widget, ETK_TRUE);
      
      etk_widget_theme_object_signal_emit(ETK_WIDGET(menu_item), "left_widget_show");
   }
   else
      etk_widget_theme_object_signal_emit(ETK_WIDGET(menu_item), "left_widget_hide");
   
   etk_widget_size_recalc_queue(ETK_WIDGET(menu_item));
}

/**************************
 * Menu Item Check
 **************************/

/**
 * @brief Gets the type of an Etk_Menu_Item_Check
 * @return Returns the type on an Etk_Menu_Item_Check
 */
Etk_Type *etk_menu_item_check_type_get()
{
   static Etk_Type *menu_item_check_type = NULL;

   if (!menu_item_check_type)
   {
      menu_item_check_type = etk_type_new("Etk_Menu_Item_Check", ETK_MENU_ITEM_TYPE, sizeof(Etk_Menu_Item_Check), ETK_CONSTRUCTOR(_etk_menu_item_check_constructor), NULL);
      
      _etk_menu_item_check_signals[ETK_MENU_ITEM_CHECK_TOGGLED_SIGNAL] = etk_signal_new("toggled", menu_item_check_type, ETK_MEMBER_OFFSET(Etk_Menu_Item_Check, toggled), etk_marshaller_VOID__VOID, NULL, NULL);
      
      etk_type_property_add(menu_item_check_type, "active", ETK_MENU_ITEM_ACTIVE_PROPERTY, ETK_PROPERTY_BOOL, ETK_PROPERTY_READABLE_WRITABLE, etk_property_value_bool(ETK_FALSE));
      
      menu_item_check_type->property_set = _etk_menu_item_check_property_set;
      menu_item_check_type->property_get = _etk_menu_item_check_property_get;
   }

   return menu_item_check_type;
}

/**
 * @brief Creates a new menu item with a checkbox
 * @return Returns the new menu item with a checkbox
 * @note Unlike the other widgets, You don't need to call etk_widget_shown, the widget is automatically shown at its creation
 */
Etk_Widget *etk_menu_item_check_new()
{
   return etk_widget_new(ETK_MENU_ITEM_CHECK_TYPE, "theme_group", "menu_item", "visible", ETK_TRUE, NULL);
}

/**
 * @brief Creates a new menu item with a checkbox
 * @param label the label to set to the new menu item
 * @return Returns the new menu item with a checkbox
 * @note Unlike the other widgets, You don't need to call etk_widget_shown, the widget is automatically shown at its creation
 */
Etk_Widget *etk_menu_item_check_new_with_label(const char *label)
{
   return etk_widget_new(ETK_MENU_ITEM_CHECK_TYPE, "theme_group", "menu_item", "label", label, "visible", ETK_TRUE, NULL);
}

/**
 * @brief Sets whether the check menu item is active (i.e. checked)
 * @param check_item a check menu item
 * @param active if @a active == ETK_TRUE, the check menu item will be active
 */
void etk_menu_item_check_active_set(Etk_Menu_Item_Check *check_item, Etk_Bool active)
{
   if (!check_item || !check_item->active_set)
      return;
   check_item->active_set(check_item, active);
}

/**
 * @brief Gets whether the check menu item is active
 * @param check_item a check menu item
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
 * @brief Gets the type of an Etk_Menu_Item_Radio
 * @return Returns the type on an Etk_Menu_Item_Radio
 */
Etk_Type *etk_menu_item_radio_type_get()
{
   static Etk_Type *menu_item_radio_type = NULL;

   if (!menu_item_radio_type)
   {
      menu_item_radio_type = etk_type_new("Etk_Menu_Item_Radio", ETK_MENU_ITEM_CHECK_TYPE, sizeof(Etk_Menu_Item_Radio),
         ETK_CONSTRUCTOR(_etk_menu_item_radio_constructor), ETK_DESTRUCTOR(_etk_menu_item_radio_destructor));

      etk_type_property_add(menu_item_radio_type, "group", ETK_MENU_ITEM_GROUP_PROPERTY, ETK_PROPERTY_POINTER,
         ETK_PROPERTY_READABLE_WRITABLE,  etk_property_value_pointer(NULL));
   
      menu_item_radio_type->property_set = _etk_menu_item_radio_property_set;
      menu_item_radio_type->property_get = _etk_menu_item_radio_property_get;
   }

   return menu_item_radio_type;
}

/**
 * @brief Creates a new radio menu item
 * @param group the group which the radio menu item will be added to (NULL if the radio menu item should create its own group)
 * @return Returns the new radio menu item widget
 * @note Unlike the other widgets, You don't need to call etk_widget_shown, the widget is automatically shown at its creation
 */
Etk_Widget *etk_menu_item_radio_new(Evas_List **group)
{
   return etk_widget_new(ETK_MENU_ITEM_RADIO_TYPE, "theme_group", "menu_item", "visible", ETK_TRUE, "group", group, NULL);
}

/**
 * @brief Creates a new radio menu item and uses the group of another radio menu item
 * @param radio_item the radio menu item whose group will be used for the new radio menu item
 * @return Returns the new radio menu item widget
 * @note Unlike the other widgets, You don't need to call etk_widget_shown, the widget is automatically shown at its creation
 */
Etk_Widget *etk_menu_item_radio_new_from_widget(Etk_Menu_Item_Radio *radio_item)
{
   return etk_widget_new(ETK_MENU_ITEM_RADIO_TYPE, "theme_group", "menu_item", "visible", ETK_TRUE,
      "group", etk_menu_item_radio_group_get(radio_item), NULL);
}

/**
 * @brief Creates a new radio menu item with a label
 * @param label the label
 * @param group the group which the radio menu item will be added to (NULL if the radio menu item should create its own group)
 * @return Returns the new radio menu item widget
 * @note Unlike the other widgets, You don't need to call etk_widget_shown, the widget is automatically shown at its creation
 */
Etk_Widget *etk_menu_item_radio_new_with_label(const char *label, Evas_List **group)
{
   return etk_widget_new(ETK_MENU_ITEM_RADIO_TYPE, "theme_group", "menu_item", "visible", ETK_TRUE, "group", group, "label", label, NULL);
}

/**
 * @brief Creates a new radio menu item with a label and uses the group of another radio menu item
 * @param label the label
 * @param radio_item the radio menu item whose group will be used for the new radio menu item
 * @return Returns the new radio menu item widget
 * @note Unlike the other widgets, You don't need to call etk_widget_shown, the widget is automatically shown at its creation
 */
Etk_Widget *etk_menu_item_radio_new_with_label_from_widget(const char *label, Etk_Menu_Item_Radio *radio_item)
{
   return etk_widget_new(ETK_MENU_ITEM_RADIO_TYPE, "theme_group", "menu_item", "visible", ETK_TRUE,
      "group", etk_menu_item_radio_group_get(radio_item), "label", label, NULL);
}

/**
 * @brief Sets the group of the radio menu item
 * @param radio_item a radio menu item
 * @param group the group to use
 */
void etk_menu_item_radio_group_set(Etk_Menu_Item_Radio *radio_item, Evas_List **group)
{
   Etk_Menu_Item_Check *check_item;
   Etk_Bool active;

   if (!(check_item = ETK_MENU_ITEM_CHECK(radio_item)) || (group && (radio_item->group == group)))
      return;

   if (radio_item->group)
   {
      *radio_item->group = evas_list_remove(*radio_item->group, radio_item);
      if (!(*radio_item->group))
      {
         free(radio_item->group);
         radio_item->group = NULL;
      }
   }

   if (!group)
   {
      group = malloc(sizeof(Evas_List *));
      *group = NULL;
      active = ETK_TRUE;
   }
   else
      active = ETK_FALSE;

   *group = evas_list_append(*group, radio_item);
   radio_item->group = group;
   etk_object_notify(ETK_OBJECT(radio_item), "group");

   radio_item->can_uncheck = ETK_TRUE;
   etk_menu_item_check_active_set(check_item, active);
}

/**
 * @brief Gets the group of the radio menu item
 * @param radio_item a radio menu item
 * @return Returns the group of the radio menu item
 */
Evas_List **etk_menu_item_radio_group_get(Etk_Menu_Item_Radio *radio_item)
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

/* Menu_Item: Initializes the members of the new menu item */
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
   menu_item->left_widget = NULL;
   menu_item->is_selected = ETK_FALSE;
   
   etk_signal_connect("realize", ETK_OBJECT(menu_item), ETK_CALLBACK(_etk_menu_item_realize_cb), NULL);
}

/* Menu_Item: Destroys the menu item */
static void _etk_menu_item_destructor(Etk_Menu_Item *menu_item)
{
   if (!menu_item)
      return;
   free(menu_item->label);
}

/* Menu_Item_Ckeck: Initializes the members of the new menu item with a checkbox */
static void _etk_menu_item_check_constructor(Etk_Menu_Item_Check *check_item)
{
   Etk_Menu_Item *menu_item;
   
   if (!(menu_item = ETK_MENU_ITEM(check_item)))
      return;
   
   check_item->active = ETK_FALSE;
   check_item->toggled = _etk_menu_item_check_toggled_handler;
   check_item->active_set = _etk_menu_item_check_active_set;
   
   menu_item->left_widget = etk_widget_new(ETK_WIDGET_TYPE, "theme_group", ETK_IS_MENU_ITEM_RADIO(check_item) ? "menu_radio" : "menu_check",
      "pass_mouse_events", ETK_TRUE, "visible", ETK_TRUE, NULL);
   etk_signal_connect("realize", ETK_OBJECT(menu_item->left_widget), ETK_CALLBACK(_etk_menu_item_left_widget_realize_cb), menu_item);
   etk_signal_connect("realize", ETK_OBJECT(menu_item->left_widget), ETK_CALLBACK(_etk_menu_item_check_left_widget_realize_cb), menu_item);
   etk_widget_parent_set(menu_item->left_widget, ETK_WIDGET(menu_item));
   
   etk_signal_connect("activated", ETK_OBJECT(menu_item), ETK_CALLBACK(_etk_menu_item_check_activated_cb), menu_item);
}

/* Menu_Item_Radio: Initializes the members of the new menu item with a radiobox */
static void _etk_menu_item_radio_constructor(Etk_Menu_Item_Radio *radio_item)
{
   if (!radio_item)
      return;
   
   radio_item->group = NULL;
   radio_item->can_uncheck = ETK_FALSE;
   ETK_MENU_ITEM_CHECK(radio_item)->active_set = _etk_menu_item_radio_active_set;
}

/* Menu_Item_Radio: Destroys the menu item with a radiobox */
static void _etk_menu_item_radio_destructor(Etk_Menu_Item_Radio *radio_item)
{
   if (!radio_item || !radio_item->group)
      return;

   *radio_item->group = evas_list_remove(*radio_item->group, radio_item);
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
         etk_menu_item_image_set(image_item, etk_property_value_pointer_get(value));
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
         etk_property_value_pointer_set(value, menu_item->left_widget);
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
static void _etk_menu_item_realize_cb(Etk_Object *object, void *data)
{
   Etk_Menu_Item *menu_item;
   
   if (!(menu_item = ETK_MENU_ITEM(object)))
      return;
   
   etk_widget_theme_object_part_text_set(ETK_WIDGET(menu_item), "label", menu_item->label ? menu_item->label : "");
   if (menu_item->left_widget && menu_item->left_widget->realized)
      etk_widget_swallow_widget(ETK_WIDGET(menu_item), "left_widget_swallow", menu_item->left_widget);
}

/* Called when the left widget of the menu item is realized */
static void _etk_menu_item_left_widget_realize_cb(Etk_Object *object, void *data)
{
   Etk_Menu_Item *menu_item;

   if (!(menu_item = ETK_MENU_ITEM(data)) || !menu_item->left_widget)
      return;

   if (ETK_WIDGET(menu_item)->realized)
      etk_widget_swallow_widget(ETK_WIDGET(menu_item), "left_widget_swallow", menu_item->left_widget);
}

/* Called when the left widget of the check item is realized */
static void _etk_menu_item_check_left_widget_realize_cb(Etk_Object *object, void *data)
{
   Etk_Menu_Item *menu_item;

   if (!(menu_item = ETK_MENU_ITEM(data)) || !menu_item->left_widget)
      return;
   etk_widget_theme_object_signal_emit(menu_item->left_widget, ETK_MENU_ITEM_CHECK(menu_item)->active ? "check" : "uncheck");
}

/* Called when the check item is activated */
static void _etk_menu_item_check_activated_cb(Etk_Object *object, void *data)
{
   Etk_Menu_Item_Check *check_item;
   
   if (!(check_item = ETK_MENU_ITEM_CHECK(object)))
      return;
   etk_menu_item_check_active_set(check_item, !check_item->active);
}

/* Default handler for the "selected" signal */
static void _etk_menu_item_selected_handler(Etk_Menu_Item *menu_item)
{
   if (!menu_item)
      return;
   etk_widget_theme_object_signal_emit(ETK_WIDGET(menu_item), "select");
   etk_widget_theme_object_signal_emit(ETK_WIDGET(menu_item->left_widget), "select");
}

/* Default handler for the "deselected" signal */
static void _etk_menu_item_deselected_handler(Etk_Menu_Item *menu_item)
{
   if (!menu_item)
      return;
   etk_widget_theme_object_signal_emit(ETK_WIDGET(menu_item), "deselect");
   etk_widget_theme_object_signal_emit(ETK_WIDGET(menu_item->left_widget), "deselect");
}

/* Default handler for the "activated" signal */
static void _etk_menu_item_activated_handler(Etk_Menu_Item *menu_item)
{
   if (!menu_item)
      return;
   etk_widget_theme_object_signal_emit(ETK_WIDGET(menu_item), "activate");
   etk_widget_theme_object_signal_emit(ETK_WIDGET(menu_item->left_widget), "activate");
}

/* Default handler for the "toggled" signal */
static void _etk_menu_item_check_toggled_handler(Etk_Menu_Item_Check *check_item)
{
   if (check_item && ETK_MENU_ITEM(check_item)->left_widget)
      etk_widget_theme_object_signal_emit(ETK_MENU_ITEM(check_item)->left_widget, check_item->active ? "check" : "uncheck");
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
   etk_object_notify(ETK_OBJECT(check_item), "active");
   etk_signal_emit(_etk_menu_item_check_signals[ETK_MENU_ITEM_CHECK_TOGGLED_SIGNAL], ETK_OBJECT(check_item), NULL);
}

/* Behavior of the "active_set" function for a radio menu item */
static void _etk_menu_item_radio_active_set(Etk_Menu_Item_Check *check_item, Etk_Bool active)
{
   Etk_Menu_Item_Radio *radio_item;
   Etk_Menu_Item_Check *ci;
   Evas_List *l;

   if (!(radio_item = ETK_MENU_ITEM_RADIO(check_item)) || check_item->active == active)
      return;

   if (!check_item->active || (check_item->active && radio_item->can_uncheck))
   {
      check_item->active = active;
      etk_object_notify(ETK_OBJECT(check_item), "active");
      etk_signal_emit(_etk_menu_item_check_signals[ETK_MENU_ITEM_CHECK_TOGGLED_SIGNAL], ETK_OBJECT(check_item), NULL);
   
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
