/** @file etk_menu.c */
#include "etk_menu.h"
#include <stdlib.h>
#include "etk_popup_window.h"
#include "etk_menu_item.h"
#include "etk_utils.h"
#include "etk_signal.h"
#include "etk_signal_callback.h"

/**
 * @addtogroup Etk_Menu
* @{
 */

enum _Etk_Menu_Signal_Id
{
   ETK_MENU_POPPED_DOWN_SIGNAL,
   ETK_MENU_POPPED_UP_SIGNAL,
   ETK_MENU_NUM_SIGNALS
};

static void _etk_menu_constructor(Etk_Menu *menu);
static void _etk_menu_destructor(Etk_Menu *menu);
static void _etk_menu_size_request(Etk_Widget *widget, Etk_Size *size_requisition);
static void _etk_menu_size_allocate(Etk_Widget *widget, Etk_Geometry geometry);
static void _etk_menu_window_popped_up_cb(Etk_Object *object, void *data);
static void _etk_menu_window_popped_down_cb(Etk_Object *object, void *data);
static void _etk_menu_window_key_down_cb(Etk_Object *object, void *event_info, void *data);
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
 * @brief Gets the type of an Etk_Menu
 * @return Returns the type on an Etk_Menu
 */
Etk_Type *etk_menu_type_get()
{
   static Etk_Type *menu_type = NULL;

   if (!menu_type)
   {
      menu_type = etk_type_new("Etk_Menu", ETK_MENU_SHELL_TYPE, sizeof(Etk_Menu), ETK_CONSTRUCTOR(_etk_menu_constructor), ETK_DESTRUCTOR(_etk_menu_destructor));
      
      _etk_menu_signals[ETK_MENU_POPPED_UP_SIGNAL] = etk_signal_new("popped_up", menu_type, -1, etk_marshaller_VOID__VOID, NULL, NULL);
      _etk_menu_signals[ETK_MENU_POPPED_DOWN_SIGNAL] = etk_signal_new("popped_down", menu_type, -1, etk_marshaller_VOID__VOID, NULL, NULL);
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
 * @brief Popups the menu at the position (x, y)
 * @param menu a menu
 * @param x the x component of the location where to popup the menu
 * @param y the y component of the location where to popup the menu
 */
void etk_menu_popup_at_xy(Etk_Menu *menu, int x, int y)
{
   if (!menu)
      return;
   etk_popup_window_popup_at_xy(menu->window, x, y);
}

/**
 * @brief Popups the menu at the mouse pointer position
 * @param menu a menu
 */
void etk_menu_popup(Etk_Menu *menu)
{
   if (!menu)
      return;
   etk_popup_window_popup(menu->window);
}

/**
 * @brief Pops down the menu and its children
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

/* Initializes the members */
static void _etk_menu_constructor(Etk_Menu *menu)
{
   if (!menu)
      return;
      
   menu->window = ETK_POPUP_WINDOW(etk_widget_new(ETK_POPUP_WINDOW_TYPE, NULL));
   etk_signal_connect("popped_up", ETK_OBJECT(menu->window), ETK_CALLBACK(_etk_menu_window_popped_up_cb), menu);
   etk_signal_connect("popped_down", ETK_OBJECT(menu->window), ETK_CALLBACK(_etk_menu_window_popped_down_cb), menu);
   etk_signal_connect("key_down", ETK_OBJECT(menu->window), ETK_CALLBACK(_etk_menu_window_key_down_cb), menu);
   
   etk_container_add(ETK_CONTAINER(menu->window), ETK_WIDGET(menu));
   ETK_WIDGET(menu)->size_request = _etk_menu_size_request;
   ETK_WIDGET(menu)->size_allocate = _etk_menu_size_allocate;
   
   etk_signal_connect("item_added", ETK_OBJECT(menu), ETK_CALLBACK(_etk_menu_item_added_cb), NULL);
   etk_signal_connect("item_removed", ETK_OBJECT(menu), ETK_CALLBACK(_etk_menu_item_removed_cb), NULL);
}

/* Destroys the menu */
static void _etk_menu_destructor(Etk_Menu *menu)
{
   if (!menu)
      return;
   etk_container_remove(ETK_CONTAINER(menu->window), ETK_WIDGET(menu));
   etk_object_destroy(ETK_OBJECT(menu->window));
}

/* Calculates the ideal size of the menu */
static void _etk_menu_size_request(Etk_Widget *widget, Etk_Size *size_requisition)
{
   Evas_List *l;
   Etk_Menu_Shell *menu_shell;
   
   if (!(menu_shell = ETK_MENU_SHELL(widget)) || !size_requisition)
      return;
   
   size_requisition->w = 0;
   size_requisition->h = 0;
   for (l = menu_shell->items; l; l = l->next)
   {
      Etk_Size child_requisition;
      
      etk_widget_size_request(ETK_WIDGET(l->data), &child_requisition);
      size_requisition->w = ETK_MAX(size_requisition->w, child_requisition.w);
      size_requisition->h += child_requisition.h;
   }
   /* TODO: Fixme: size request */
   size_requisition->w += 15;
}

/* Resizes the menu to the size allocation */
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
         etk_widget_theme_object_signal_emit(ETK_WIDGET(item), item->submenu ? "arrow_show" : "arrow_spacer");
      else
         etk_widget_theme_object_signal_emit(ETK_WIDGET(item), "arrow_hide");
      
      etk_widget_theme_object_signal_emit(ETK_WIDGET(item), items_have_left_widget ? "left_widget_show" : "left_widget_hide");
   }
   
   y_offset = geometry.y;
   child_geometry.x = geometry.x;
   for (l = menu_shell->items; l; l = l->next)
   {
      Etk_Size child_requisition;
      
      item = ETK_MENU_ITEM(l->data);
      etk_widget_size_request(ETK_WIDGET(item), &child_requisition);
      child_geometry.y = y_offset;
      child_geometry.w = geometry.w;
      child_geometry.h = child_requisition.h;
      
      etk_widget_size_allocate(ETK_WIDGET(item), child_geometry);
      y_offset += child_requisition.h;
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
   Etk_Widget *menu_widget;
   
   if (!(menu_widget = ETK_WIDGET(data)))
      return;
   
   etk_widget_show(menu_widget);
   etk_signal_emit(_etk_menu_signals[ETK_MENU_POPPED_UP_SIGNAL], ETK_OBJECT(menu_widget), NULL);
}

/* Called when the menu window is popped down */
static void _etk_menu_window_popped_down_cb(Etk_Object *object, void *data)
{
   Evas_List *l;
   Etk_Menu_Shell *menu_shell;
   
   if (!(menu_shell = ETK_MENU_SHELL(data)))
      return;
   
   for (l = menu_shell->items; l; l = l->next)
      etk_menu_item_deselect(ETK_MENU_ITEM(l->data));
   
   etk_signal_emit(_etk_menu_signals[ETK_MENU_POPPED_DOWN_SIGNAL], ETK_OBJECT(menu_shell), NULL);
   if (menu_shell->parent)
      etk_signal_emit_by_name("submenu_popped_down", ETK_OBJECT(menu_shell->parent), NULL);
}

/* Called when a key is pressed on the menu window */
static void _etk_menu_window_key_down_cb(Etk_Object *object, void *event_info, void *data)
{
   Etk_Menu_Shell *menu_shell;
   Evas_Event_Key_Down *event;
   Evas_List *l;
   
   if (!(menu_shell = ETK_MENU_SHELL(data)) || !(event = event_info))
      return;
   
   /*if (strcmp(event->keyname, "Down") == 0)
   {
      if (!menu_shell->items)
         return;
      if (!combobox->selected_item || !(l = evas_list_find_list(combobox->items, combobox->selected_item)) || !l->next)
         _etk_combobox_selected_item_set(combobox, ETK_COMBOBOX_ITEM(combobox->items->data));
      else
         _etk_combobox_selected_item_set(combobox, ETK_COMBOBOX_ITEM(l->next->data));
   }
   else if (strcmp(event->keyname, "Up") == 0)
   {
      if (!combobox->items)
         return;
      if (!combobox->selected_item || !(l = evas_list_find_list(combobox->items, combobox->selected_item)) || !l->prev)
         _etk_combobox_selected_item_set(combobox, ETK_COMBOBOX_ITEM(evas_list_last(combobox->items)->data));
      else
         _etk_combobox_selected_item_set(combobox, ETK_COMBOBOX_ITEM(l->prev->data));
   }
   else if (strcmp(event->keyname, "Return") == 0 || strcmp(event->keyname, "space") == 0 || strcmp(event->keyname, "KP_Enter") == 0)
   {
      if (combobox->selected_item)
         etk_combobox_active_item_set(combobox, combobox->selected_item);
   }
   else if (strcmp(event->keyname, "Escape") == 0)
      etk_popup_window_popdown(combobox->window);*/
}

/* Called when an item is added to the menu */
static void _etk_menu_item_added_cb(Etk_Object *object, void *item, void *data)
{
   Etk_Object *item_object;
   
   if (!(item_object = ETK_OBJECT(item)))
      return;
   
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
   
   if (!(item = ETK_MENU_ITEM(object)) || !(menu = ETK_MENU(item->parent)))
      return;

   /* First, we deactivate all the items that are on the same menu than the item */
   for (l = ETK_MENU_SHELL(menu)->items; l; l = l->next)
   {
      if (ETK_MENU_ITEM(l->data) == item)
         continue;
      etk_menu_item_deselect(ETK_MENU_ITEM(l->data));
   }
   
   /* Then we popup the child menu */
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
