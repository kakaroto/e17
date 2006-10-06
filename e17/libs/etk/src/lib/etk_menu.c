/** @file etk_menu.c */
#include "etk_menu.h"
#include <stdlib.h>
#include "etk_menu_item.h"
#include "etk_signal.h"
#include "etk_signal_callback.h"
#include "etk_utils.h"

/**
 * @addtogroup Etk_Menu
 * @{
 */

enum Etk_Menu_Signal_Id
{
   ETK_MENU_POPPED_DOWN_SIGNAL,
   ETK_MENU_POPPED_UP_SIGNAL,
   ETK_MENU_NUM_SIGNALS
};

enum Etk_Menu_Property_Id
{
   ETK_MENU_PARENT_ITEM_PROPERTY
};

static void _etk_menu_constructor(Etk_Menu *menu);
static void _etk_menu_destructor(Etk_Menu *menu);
static void _etk_menu_property_set(Etk_Object *object, int property_id, Etk_Property_Value *value);
static void _etk_menu_property_get(Etk_Object *object, int property_id, Etk_Property_Value *value);
static void _etk_menu_size_request(Etk_Widget *widget, Etk_Size *size);
static void _etk_menu_size_allocate(Etk_Widget *widget, Etk_Geometry geometry);
static void _etk_menu_window_popped_up_cb(Etk_Object *object, void *data);
static void _etk_menu_window_popped_down_cb(Etk_Object *object, void *data);
static void _etk_menu_window_key_down_cb(Etk_Object *object, void *event_info, void *data);
static void _etk_menu_parent_item_changed_cb(Etk_Object *object, const char *property_name, void *data);
static void _etk_menu_item_added_cb(Etk_Object *object, void *item, void *data);
static void _etk_menu_item_removed_cb(Etk_Object *object, void *item, void *data);
static void _etk_menu_item_enter_cb(Etk_Object *object, void *data);
static void _etk_menu_item_leave_cb(Etk_Object *object, void *data);
static void _etk_menu_item_mouse_up_cb(Etk_Object *object, void *event, void *data);
static void _etk_menu_item_selected_cb(Etk_Object *object, void *data);
static void _etk_menu_item_deselected_cb(Etk_Object *object, void *data);
static void _etk_menu_item_activated_cb(Etk_Object *object, void *data);

static Etk_Signal *_etk_menu_signals[ETK_MENU_NUM_SIGNALS];


/**************************
 *
 * Implementation
 *
 **************************/

/**
 * @internal
 * @brief Gets the type of an Etk_Menu
 * @return Returns the type of an Etk_Menu
 */
Etk_Type *etk_menu_type_get()
{
   static Etk_Type *menu_type = NULL;

   if (!menu_type)
   {
      menu_type = etk_type_new("Etk_Menu", ETK_MENU_SHELL_TYPE, sizeof(Etk_Menu),
         ETK_CONSTRUCTOR(_etk_menu_constructor), ETK_DESTRUCTOR(_etk_menu_destructor));
      
      _etk_menu_signals[ETK_MENU_POPPED_UP_SIGNAL] = etk_signal_new("popped_up",
         menu_type, -1, etk_marshaller_VOID__VOID, NULL, NULL);
      _etk_menu_signals[ETK_MENU_POPPED_DOWN_SIGNAL] = etk_signal_new("popped_down",
         menu_type, -1, etk_marshaller_VOID__VOID, NULL, NULL);
      
      etk_type_property_add(menu_type, "parent_item", ETK_MENU_PARENT_ITEM_PROPERTY,
         ETK_PROPERTY_POINTER, ETK_PROPERTY_READABLE_WRITABLE, etk_property_value_pointer(NULL));
      
      menu_type->property_set = _etk_menu_property_set;
      menu_type->property_get = _etk_menu_property_get;
   }

   return menu_type;
}

/**
 * @brief Creates a new menu
 * @return Returns the new menu widget
 */
Etk_Widget *etk_menu_new()
{
   return etk_widget_new(ETK_MENU_TYPE, "theme_group", "menu", NULL);
}

/**
 * @brief Gets the menu-item which the menu is attached to
 * @param menu a menu
 * @return Returns the menu-item which the menu is attached to
 * @see etk_menu_item_submenu_set()
 */
Etk_Menu_Item *etk_menu_parent_item_get(Etk_Menu *menu)
{
   if (!menu)
      return NULL;
   return menu->parent_item;
}

/**
 * @brief Pops up the menu at the mouse position
 * @param menu a menu
 */
void etk_menu_popup(Etk_Menu *menu)
{
   if (!menu)
      return;
   etk_popup_window_popup(menu->window);
}

/**
 * @brief Pops up the menu at the mouse position, in the given direction
 * @param menu a menu
 * @param direction the direction to which the menu should be popped up
 */
void etk_menu_popup_in_direction(Etk_Menu *menu, Etk_Popup_Direction direction)
{
   if (!menu)
      return;
   etk_popup_window_popup_in_direction(menu->window, direction);
}

/**
 * @brief Pops up the menu at the position (x, y)
 * @param menu a menu
 * @param x the x component of the position where to popup the menu
 * @param y the y component of the position where to popup the menu
 */
void etk_menu_popup_at_xy(Etk_Menu *menu, int x, int y)
{
   if (!menu)
      return;
   etk_popup_window_popup_at_xy(menu->window, x, y);
}

/**
 * @brief Pops up the menu at the position (x, y), in the given direction
 * @param menu a menu
 * @param x the x component of the position where to popup the menu
 * @param y the y component of the position where to popup the menu
 * @param direction the direction to which the menu should be popped up
 */
void etk_menu_popup_at_xy_in_direction(Etk_Menu *menu, int x, int y, Etk_Popup_Direction direction)
{
   if (!menu)
      return;
   etk_popup_window_popup_at_xy_in_direction(menu->window, x, y, direction);
}

/**
 * @brief Pops down the menu and all its submenus (menus attached to its items)
 * @param menu a menu
 */
void etk_menu_popdown(Etk_Menu *menu)
{
   if (!menu)
      return;
   etk_popup_window_popdown(menu->window);
}

/**************************
 *
 * Etk specific functions
 *
 **************************/

/* Initializes the menu */
static void _etk_menu_constructor(Etk_Menu *menu)
{
   if (!menu)
      return;
   
   menu->parent_item = NULL;
   menu->window = ETK_POPUP_WINDOW(etk_widget_new(ETK_POPUP_WINDOW_TYPE, NULL));
   etk_signal_connect("popped_up", ETK_OBJECT(menu->window), ETK_CALLBACK(_etk_menu_window_popped_up_cb), menu);
   etk_signal_connect("popped_down", ETK_OBJECT(menu->window), ETK_CALLBACK(_etk_menu_window_popped_down_cb), menu);
   etk_signal_connect("key_down", ETK_OBJECT(menu->window), ETK_CALLBACK(_etk_menu_window_key_down_cb), menu);
   
   etk_container_add(ETK_CONTAINER(menu->window), ETK_WIDGET(menu));
   ETK_WIDGET(menu)->size_request = _etk_menu_size_request;
   ETK_WIDGET(menu)->size_allocate = _etk_menu_size_allocate;
   
   /* We make sure the menu widget is always visible */
   etk_signal_connect_swapped("hide", ETK_OBJECT(menu), ETK_CALLBACK(etk_widget_show), menu);
   etk_widget_show(ETK_WIDGET(menu));
   
   etk_signal_connect("item_added", ETK_OBJECT(menu), ETK_CALLBACK(_etk_menu_item_added_cb), NULL);
   etk_signal_connect("item_removed", ETK_OBJECT(menu), ETK_CALLBACK(_etk_menu_item_removed_cb), NULL);
   etk_object_notification_callback_add(ETK_OBJECT(menu), "parent_item", _etk_menu_parent_item_changed_cb, NULL);
}

/* Destroys the menu */
static void _etk_menu_destructor(Etk_Menu *menu)
{
   if (!menu)
      return;
   
   etk_container_remove(ETK_CONTAINER(menu->window), ETK_WIDGET(menu));
   etk_object_destroy(ETK_OBJECT(menu->window));
}

/* Sets the property whose id is "property_id" to the value "value" */
static void _etk_menu_property_set(Etk_Object *object, int property_id, Etk_Property_Value *value)
{
   Etk_Menu *menu;

   if (!(menu = ETK_MENU(object)) || !value)
      return;

   switch (property_id)
   {
      case ETK_MENU_PARENT_ITEM_PROPERTY:
         etk_menu_item_submenu_set(ETK_MENU_ITEM(etk_property_value_pointer_get(value)), menu);
         break;
      default:
         break;
   }
}

/* Gets the value of the property whose id is "property_id" */
static void _etk_menu_property_get(Etk_Object *object, int property_id, Etk_Property_Value *value)
{
   Etk_Menu *menu;

   if (!(menu = ETK_MENU(object)) || !value)
      return;
   
   switch (property_id)
   {
      case ETK_MENU_PARENT_ITEM_PROPERTY:
         etk_property_value_pointer_set(value, menu->parent_item);
         break;
      default:
         break;
   }
}

/* Calculates the ideal size of the menu */
static void _etk_menu_size_request(Etk_Widget *widget, Etk_Size *size)
{
   Evas_List *l;
   Etk_Menu_Shell *menu_shell;
   
   if (!(menu_shell = ETK_MENU_SHELL(widget)) || !size)
      return;
   
   size->w = 0;
   size->h = 0;
   for (l = menu_shell->items; l; l = l->next)
   {
      Etk_Size child_size;
      
      etk_widget_size_request(ETK_WIDGET(l->data), &child_size);
      size->w = ETK_MAX(size->w, child_size.w);
      size->h += child_size.h;
   }
}

/* Resizes the menu to the allocated size */
static void _etk_menu_size_allocate(Etk_Widget *widget, Etk_Geometry geometry)
{
   Etk_Geometry child_geometry;
   Etk_Menu_Shell *menu_shell;
   Evas_List *l;
   Etk_Menu_Item *item;
   int y_offset;
   Etk_Bool items_have_left_widget = ETK_FALSE;
   Etk_Bool items_have_submenu = ETK_FALSE;
   
   if (!(menu_shell = ETK_MENU_SHELL(widget)))
      return;
   
   for (l = menu_shell->items; l; l = l->next)
   {
      item = ETK_MENU_ITEM(l->data);
      if (item->left_widget)
         items_have_left_widget = ETK_TRUE;
      if (item->submenu)
         items_have_submenu = ETK_TRUE;
   }
   
   for (l = menu_shell->items; l; l = l->next)
   {
      item = ETK_MENU_ITEM(l->data);
   
      if (items_have_submenu)
      {
         if (item->submenu)
            etk_widget_theme_signal_emit(ETK_WIDGET(item), "etk,action,show,arrow", ETK_TRUE);
         else
            etk_widget_theme_signal_emit(ETK_WIDGET(item), "etk,action,show,arrow_space", ETK_TRUE);
      }
      else
         etk_widget_theme_signal_emit(ETK_WIDGET(item), "etk,action,hide,arrow", ETK_TRUE);
      
      if (items_have_left_widget)
         etk_widget_theme_signal_emit(ETK_WIDGET(item), "etk,action,show,left_widget", ETK_TRUE);
      else
         etk_widget_theme_signal_emit(ETK_WIDGET(item), "etk,action,hide,left_widget", ETK_TRUE);
   }
   
   y_offset = geometry.y;
   child_geometry.x = geometry.x;
   for (l = menu_shell->items; l; l = l->next)
   {
      Etk_Size child_size;
      
      item = ETK_MENU_ITEM(l->data);
      etk_widget_size_request(ETK_WIDGET(item), &child_size);
      child_geometry.y = y_offset;
      child_geometry.w = geometry.w;
      child_geometry.h = child_size.h;
      
      etk_widget_size_allocate(ETK_WIDGET(item), child_geometry);
      y_offset += child_geometry.h;
   }
}

/**************************
 *
 * Callbacks and handlers
 *
 **************************/

/* Called when the menu window is popped up */
static void _etk_menu_window_popped_up_cb(Etk_Object *object, void *data)
{
   Etk_Menu *menu;
   
   if (!(menu = ETK_MENU(data)))
      return;
   etk_signal_emit(_etk_menu_signals[ETK_MENU_POPPED_UP_SIGNAL], ETK_OBJECT(menu), NULL);
}

/* Called when the menu window is popped down */
static void _etk_menu_window_popped_down_cb(Etk_Object *object, void *data)
{
   Evas_List *l;
   Etk_Menu *menu;
   
   if (!(menu = ETK_MENU(data)))
      return;
   
   for (l = ETK_MENU_SHELL(menu)->items; l; l = l->next)
      etk_menu_item_deselect(ETK_MENU_ITEM(l->data));
   
   etk_signal_emit(_etk_menu_signals[ETK_MENU_POPPED_DOWN_SIGNAL], ETK_OBJECT(menu), NULL);
}

/* Called when a key is pressed on the menu window */
static void _etk_menu_window_key_down_cb(Etk_Object *object, void *event_info, void *data)
{
   Etk_Menu_Shell *menu_shell;
   Evas_Event_Key_Down *event;
   
   if (!(menu_shell = ETK_MENU_SHELL(data)) || !(event = event_info))
      return;
   
   /* TODO: keyboard navigation */
}

/* Called when the menu is attached to a menu-item */
static void _etk_menu_parent_item_changed_cb(Etk_Object *object, const char *property_name, void *data)
{
   Etk_Menu *menu;
   Etk_Menu_Item *item;
   
   if (!(menu = ETK_MENU(object)))
      return;
   
   if ((item = menu->parent_item) && item->parent_shell && ETK_IS_MENU(item->parent_shell))
      etk_popup_window_parent_set(menu->window, ETK_MENU(item->parent_shell)->window);
   else
      etk_popup_window_parent_set(menu->window, NULL);
   
   //TODO: What if the parent item is removed from its parent-menu?
}

/* Called when an item is added to the menu */
static void _etk_menu_item_added_cb(Etk_Object *object, void *item, void *data)
{
   Etk_Widget *menu_widget;
   Etk_Object *item_object;
   
   if (!(menu_widget = ETK_WIDGET(object)) || !(item_object = ETK_OBJECT(item)))
      return;
   
   etk_widget_theme_parent_set(ETK_WIDGET(item_object), menu_widget);
   etk_signal_connect("enter", item_object, ETK_CALLBACK(_etk_menu_item_enter_cb), NULL);
   etk_signal_connect("leave", item_object, ETK_CALLBACK(_etk_menu_item_leave_cb), NULL);
   etk_signal_connect("mouse_up", item_object, ETK_CALLBACK(_etk_menu_item_mouse_up_cb), NULL);
   etk_signal_connect("selected", item_object, ETK_CALLBACK(_etk_menu_item_selected_cb), NULL);
   etk_signal_connect("deselected", item_object, ETK_CALLBACK(_etk_menu_item_deselected_cb), NULL);
   etk_signal_connect("activated", item_object, ETK_CALLBACK(_etk_menu_item_activated_cb), NULL);
}

/* Called when an item is removed from the menu */
static void _etk_menu_item_removed_cb(Etk_Object *object, void *item, void *data)
{
   Etk_Object *item_object;
   
   if (!(item_object = ETK_OBJECT(item)))
      return;
   
   etk_widget_theme_parent_set(ETK_WIDGET(item_object), NULL);
   etk_signal_disconnect("enter", item_object, ETK_CALLBACK(_etk_menu_item_enter_cb));
   etk_signal_disconnect("leave", item_object, ETK_CALLBACK(_etk_menu_item_leave_cb));
   etk_signal_disconnect("mouse_up", item_object, ETK_CALLBACK(_etk_menu_item_mouse_up_cb));
   etk_signal_disconnect("selected", item_object, ETK_CALLBACK(_etk_menu_item_selected_cb));
   etk_signal_disconnect("deselected", item_object, ETK_CALLBACK(_etk_menu_item_deselected_cb));
   etk_signal_disconnect("activated", item_object, ETK_CALLBACK(_etk_menu_item_activated_cb));
}

/* Called when the mouse pointer enters the item */ 
static void _etk_menu_item_enter_cb(Etk_Object *object, void *data)
{
   etk_menu_item_select(ETK_MENU_ITEM(object));
}

/* Called when the mouse pointer leaves the item */
static void _etk_menu_item_leave_cb(Etk_Object *object, void *data)
{
   Etk_Menu_Item *item;
   
   if (!(item = ETK_MENU_ITEM(object)) || item->submenu)
      return;
   etk_menu_item_deselect(item);
}

/* Called when the user has released the item */
static void _etk_menu_item_mouse_up_cb(Etk_Object *object, void *event, void *data)
{
   Etk_Menu_Item *item;
   
   if (!(item = ETK_MENU_ITEM(object)) || item->submenu)
      return;
   etk_menu_item_activate(item);
}

/* Called when the item is selected */
static void _etk_menu_item_selected_cb(Etk_Object *object, void *data)
{
   Etk_Menu_Item *item;
   Etk_Menu *menu;
   Evas_List *l;
   
   if (!(item = ETK_MENU_ITEM(object)) || !(menu = ETK_MENU(item->parent_shell)))
      return;

   /* First, we deactivate all the items that are on the same menu than the item */
   for (l = ETK_MENU_SHELL(menu)->items; l; l = l->next)
   {
      if (ETK_MENU_ITEM(l->data) == item)
         continue;
      etk_menu_item_deselect(ETK_MENU_ITEM(l->data));
   }
   
   /* Then we pop up the child menu */
   if (item->submenu)
   {
      int mx, my, mw, item_y;
      
      item_y = ETK_WIDGET(item)->geometry.y;
      etk_window_geometry_get(ETK_WINDOW(menu->window), &mx, &my, &mw, NULL);
      etk_menu_popup_at_xy(item->submenu, mx + mw, my + item_y);
   }
}

/* Called when the item is deselected */
static void _etk_menu_item_deselected_cb(Etk_Object *object, void *data)
{
   Etk_Menu_Item *item;
   
   if (!(item = ETK_MENU_ITEM(object)))
      return;
   
   if (item->submenu)
      etk_menu_popdown(item->submenu);
}

/* Called when the item is activated */
static void _etk_menu_item_activated_cb(Etk_Object *object, void *data)
{
   etk_popup_window_popdown_all();
}

/** @} */

/**************************
 *
 * Documentation
 *
 **************************/

/**
 * @addtogroup Etk_Menu
 *
 * @image html widgets/menu.png
 * The items of the menu are packed vertically. @n
 * To add or remove items, you have to use the functions provided by the Etk_Menu_Shell:
 * etk_menu_shell_append(), etk_menu_shell_remove()... @n
 *
 * A menu is usually popped up by clicking on an item of a menu bar, or by activating an item of another menu. @n
 * You can also pop up a menu at the mouse position with etk_menu_popup() or at a specific position with
 * etk_menu_popup_at_xy(). @n
 * The menu can then be popped down with etk_menu_popdown(). @n
 * Never use etk_widget_show() and etk_widget_hide() on a menu to make it pop up/down. @n
 *
 * 
 * \par Object Hierarchy:
 * - Etk_Object
 *   - Etk_Widget
 *     - Etk_Menu_Shell
 *       - Etk_Menu
 *
 * \par Signals:
 * @signal_name "popped_up": Emitted when the the menu has been popped up
 * @signal_cb void callback(Etk_Menu *menu, void *data)
 * @signal_arg menu: the menu that has been popped up
 * @signal_data
 * \par
 * @signal_name "popped_down": Emitted when the the menu has been popped down
 * @signal_cb void callback(Etk_Menu *menu, void *data)
 * @signal_arg menu: the menu that has been popped down
 * @signal_data
 *
 * \par Properties:
 * @prop_name "parent_item": The menu-item which the menu is attached to
 * @prop_type Pointer (Etk_Menu_Item *)
 * @prop_rw
 * @prop_val NULL
 */
