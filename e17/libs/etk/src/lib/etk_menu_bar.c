/** @file etk_menu_bar.c */
#include "etk_menu_bar.h"
#include <stdlib.h>
#include <Ecore.h>
#include <Ecore_X.h>
#include "etk_menu.h"
#include "etk_menu_item.h"
#include "etk_toplevel_widget.h"
#include "etk_utils.h"
#include "etk_signal.h"
#include "etk_signal_callback.h"

/**
 * @addtogroup Etk_Menu_Bar
 * @{
 */

static void _etk_menu_bar_constructor(Etk_Menu_Bar *menu_bar);
static void _etk_menu_bar_size_request(Etk_Widget *widget, Etk_Size *size_requisition);
static void _etk_menu_bar_size_allocate(Etk_Widget *widget, Etk_Geometry geometry);
static void _etk_menu_bar_item_added_cb(Etk_Object *object, void *item, void *data);
static void _etk_menu_bar_item_removed_cb(Etk_Object *object, void *item, void *data);
static int _etk_menu_bar_mouse_move_cb(void *data, int type, void *event);
static void _etk_menu_bar_item_selected_cb(Etk_Object *object, void *data);
static void _etk_menu_bar_item_deselected_cb(Etk_Object *object, void *data);
static void _etk_menu_bar_item_mouse_up_cb(Etk_Object *object, void *event, void *data);
static void _etk_menu_bar_item_submenu_popped_down_cb(Etk_Object *object, void *event, void *data);

static Ecore_Event_Handler *_etk_menu_bar_mouse_move_handler = NULL;

/**************************
 *
 * Implementation
 *
 **************************/

/**
 * @brief Gets the type of an Etk_Menu_Bar
 * @return Returns the type on an Etk_Menu_Bar
 */
Etk_Type *etk_menu_bar_type_get()
{
   static Etk_Type *menu_bar_type = NULL;

   if (!menu_bar_type)
      menu_bar_type = etk_type_new("Etk_Menu_Bar", ETK_MENU_SHELL_TYPE, sizeof(Etk_Menu_Bar), ETK_CONSTRUCTOR(_etk_menu_bar_constructor), NULL);

   return menu_bar_type;
}

/**
 * @brief Creates a new menu_bar
 * @return Returns the new menu_bar widget
 */
Etk_Widget *etk_menu_bar_new()
{
   return etk_widget_new(ETK_MENU_BAR_TYPE, "theme_group", "menu_bar", NULL);
}

/**************************
 *
 * Etk specific functions
 *
 **************************/

/* Initializes the members */
static void _etk_menu_bar_constructor(Etk_Menu_Bar *menu_bar)
{
   if (!menu_bar)
      return;
   
   menu_bar->item_selected = ETK_FALSE;
   ETK_WIDGET(menu_bar)->size_request = _etk_menu_bar_size_request;
   ETK_WIDGET(menu_bar)->size_allocate = _etk_menu_bar_size_allocate;
   
   etk_signal_connect("item_added", ETK_OBJECT(menu_bar), ETK_CALLBACK(_etk_menu_bar_item_added_cb), NULL);
   etk_signal_connect("item_removed", ETK_OBJECT(menu_bar), ETK_CALLBACK(_etk_menu_bar_item_removed_cb), NULL);
}

/* Calculates the ideal size of the menu_bar */
static void _etk_menu_bar_size_request(Etk_Widget *widget, Etk_Size *size_requisition)
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
      size_requisition->w += child_requisition.w;
      size_requisition->h = ETK_MAX(size_requisition->h, child_requisition.h);
   }
}

/* Resizes the menu_bar to the size allocation */
static void _etk_menu_bar_size_allocate(Etk_Widget *widget, Etk_Geometry geometry)
{
   Etk_Geometry child_geometry;
   Etk_Menu_Shell *menu_shell;
   Evas_List *l;
   int x_offset;
   
   if (!(menu_shell = ETK_MENU_SHELL(widget)))
      return;
   
   x_offset = geometry.x;
   child_geometry.y = geometry.y;
   for (l = menu_shell->items; l; l = l->next)
   {
      Etk_Size child_requisition;
      
      etk_widget_size_request(ETK_WIDGET(l->data), &child_requisition);
      child_geometry.x = x_offset;
      child_geometry.w = child_requisition.w;
      child_geometry.h = geometry.h;
      
      etk_widget_size_allocate(ETK_WIDGET(l->data), child_geometry);
      x_offset += child_requisition.w;
   }
}

/**************************
 *
 * Callbacks and handlers
 *
 **************************/

/* Called when an item is added to the menu_bar */
static void _etk_menu_bar_item_added_cb(Etk_Object *object, void *item, void *data)
{
   Etk_Object *item_object;
   
   if (!(item_object = ETK_OBJECT(item)))
      return;
   
   etk_signal_connect("mouse_up", item_object, ETK_CALLBACK(_etk_menu_bar_item_mouse_up_cb), NULL);
   etk_signal_connect("selected", item_object, ETK_CALLBACK(_etk_menu_bar_item_selected_cb), NULL);
   etk_signal_connect("deselected", item_object, ETK_CALLBACK(_etk_menu_bar_item_deselected_cb), NULL);
   etk_signal_connect("submenu_popped_down", item_object, ETK_CALLBACK(_etk_menu_bar_item_submenu_popped_down_cb), NULL);
}

/* Called when an item is removed from the menu_bar */
static void _etk_menu_bar_item_removed_cb(Etk_Object *object, void *item, void *data)
{
   Etk_Object *item_object;
   
   if (!(item_object = ETK_OBJECT(item)))
      return;
   
   etk_signal_disconnect("mouse_up", item_object, ETK_CALLBACK(_etk_menu_bar_item_mouse_up_cb));
   etk_signal_disconnect("selected", item_object, ETK_CALLBACK(_etk_menu_bar_item_selected_cb));
   etk_signal_disconnect("deselected", item_object, ETK_CALLBACK(_etk_menu_bar_item_deselected_cb));
   etk_signal_disconnect("submenu_popped_down", item_object, ETK_CALLBACK(_etk_menu_bar_item_submenu_popped_down_cb));
}

/*
 * Called when the user moves the mouse above the menu input window of the current popped menu
 * It popups other menus if the mouse is above an item of the menu bar
 */
static int _etk_menu_bar_mouse_move_cb(void *data, int type, void *event)
{
   Etk_Menu_Bar *menu_bar;
   Ecore_X_Event_Mouse_Move *mouse_event;
   Etk_Toplevel_Widget *toplevel;
   int tx, ty;
   Etk_Geometry item_geometry;
   Evas_List *l;
   Etk_Widget *item;
   
   if (!(menu_bar = ETK_MENU_BAR(data)) || !(toplevel = etk_widget_toplevel_parent_get(ETK_WIDGET(menu_bar))) || !(mouse_event = event))
      return 1;
   
   /* If the mouse pointer is above a menu item, we select it */
   etk_toplevel_widget_geometry_get(toplevel, &tx, &ty, NULL, NULL);
   for (l = ETK_MENU_SHELL(menu_bar)->items; l; l = l->next)
   {
      item = ETK_WIDGET(l->data);
      etk_widget_geometry_get(item, &item_geometry.x, &item_geometry.y, &item_geometry.w, &item_geometry.h);
      
      if ((mouse_event->x - tx) >= item_geometry.x && (mouse_event->x - tx) <= item_geometry.x + item_geometry.w &&
         (mouse_event->y - ty) >= item_geometry.y && (mouse_event->y - ty) <= item_geometry.y + item_geometry.h)
      {
         etk_menu_item_select(ETK_MENU_ITEM(item));
         break;
      }
   }
   
   return 1;
}

/* Called when the item is selected */
static void _etk_menu_bar_item_selected_cb(Etk_Object *object, void *data)
{
   Etk_Toplevel_Widget *toplevel;
   Etk_Menu_Item *item;
   Etk_Menu_Bar *menu_bar;
   Evas_List *l;
   
   if (!(item = ETK_MENU_ITEM(object)) || !(menu_bar = ETK_MENU_BAR(item->parent)))
      return;

   /* First, we deselect all the items that are on the same menu_bar than the item */
   for (l = ETK_MENU_SHELL(menu_bar)->items; l; l = l->next)
   {
      if (ETK_MENU_ITEM(l->data) == item)
         continue;
      etk_menu_item_deselect(ETK_MENU_ITEM(l->data));
   }
   
   /* Then we popup the child menu */
   if (item->submenu && (toplevel = etk_widget_toplevel_parent_get(ETK_WIDGET(menu_bar))))
   {
      int ix, iy, ih, tx, ty;
      
      etk_widget_geometry_get(ETK_WIDGET(item), &ix, &iy, NULL, &ih);
      etk_toplevel_widget_geometry_get(toplevel, &tx, &ty, NULL, NULL);
      etk_menu_popup_at_xy(item->submenu, tx + ix, ty + iy + ih);
   }
   
   if (!_etk_menu_bar_mouse_move_handler)
      _etk_menu_bar_mouse_move_handler = ecore_event_handler_add(ECORE_X_EVENT_MOUSE_MOVE, _etk_menu_bar_mouse_move_cb, menu_bar);
   menu_bar->item_selected = ETK_TRUE;
}

/* Called when the item is deselected */
static void _etk_menu_bar_item_deselected_cb(Etk_Object *object, void *data)
{
   Etk_Menu_Item *item;
   Etk_Menu_Bar *menu_bar;
   
   if (!(item = ETK_MENU_ITEM(object)) || !(menu_bar = ETK_MENU_BAR(item->parent)))
      return;
   
   if (item->submenu)
      etk_menu_popdown(item->submenu);
   
   if (_etk_menu_bar_mouse_move_handler)
   {
      ecore_event_handler_del(_etk_menu_bar_mouse_move_handler);
      _etk_menu_bar_mouse_move_handler = NULL;
   }
   menu_bar->item_selected = ETK_FALSE;
}

/* Called when the user has clicked on the item */
static void _etk_menu_bar_item_mouse_up_cb(Etk_Object *object, void *event, void *data)
{   
   etk_menu_item_select(ETK_MENU_ITEM(object));
}

/* Called when the submenu of an item is popped down */
static void _etk_menu_bar_item_submenu_popped_down_cb(Etk_Object *object, void *event, void *data)
{
   Etk_Menu_Item *item;
   
   if (!(item = ETK_MENU_ITEM(object)))
      return;
   etk_menu_item_deselect(item);
}

/** @} */
