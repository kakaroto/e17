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

/** @file etk_menu_bar.c */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "etk_menu_bar.h"

#include <stdlib.h>

#include "etk_event.h"
#include "etk_menu.h"
#include "etk_menu_item.h"
#include "etk_signal.h"
#include "etk_signal_callback.h"
#include "etk_toplevel.h"
#include "etk_utils.h"

/**
 * @addtogroup Etk_Menu_Bar
 * @{
 */

static void _etk_menu_bar_constructor(Etk_Menu_Bar *menu_bar);
static void _etk_menu_bar_size_request(Etk_Widget *widget, Etk_Size *size);
static void _etk_menu_bar_size_allocate(Etk_Widget *widget, Etk_Geometry geometry);
static Etk_Bool _etk_menu_bar_item_added_cb(Etk_Object *object, void *item, void *data);
static Etk_Bool _etk_menu_bar_item_removed_cb(Etk_Object *object, void *item, void *data);
static Etk_Bool _etk_menu_bar_item_selected_cb(Etk_Object *object, void *data);
static Etk_Bool _etk_menu_bar_item_unselected_cb(Etk_Object *object, void *data);
static Etk_Bool _etk_menu_bar_item_mouse_up_cb(Etk_Object *object, void *event, void *data);
static void _etk_menu_bar_mouse_move_cb(Etk_Event_Global event, void *data);
static Etk_Bool _etk_menu_bar_menu_popped_down_cb(Etk_Object *object, void *event, void *data);

/**************************
 *
 * Implementation
 *
 **************************/

/**
 * @internal
 * @brief Gets the type of an Etk_Menu_Bar
 * @return Returns the type of an Etk_Menu_Bar
 */
Etk_Type *etk_menu_bar_type_get(void)
{
   static Etk_Type *menu_bar_type = NULL;

   if (!menu_bar_type)
   {
      menu_bar_type = etk_type_new("Etk_Menu_Bar", ETK_MENU_SHELL_TYPE,
         sizeof(Etk_Menu_Bar),
         ETK_CONSTRUCTOR(_etk_menu_bar_constructor), NULL, NULL);
   }

   return menu_bar_type;
}

/**
 * @brief Creates a new menu bar
 * @return Returns the new menu bar widget
 */
Etk_Widget *etk_menu_bar_new(void)
{
   return etk_widget_new(ETK_MENU_BAR_TYPE, "theme-group", "menu_bar", NULL);
}

/**************************
 *
 * Etk specific functions
 *
 **************************/

/* Initializes the menu bar */
static void _etk_menu_bar_constructor(Etk_Menu_Bar *menu_bar)
{
   if (!menu_bar)
      return;

   menu_bar->move_callback = ETK_FALSE;
   ETK_WIDGET(menu_bar)->size_request = _etk_menu_bar_size_request;
   ETK_WIDGET(menu_bar)->size_allocate = _etk_menu_bar_size_allocate;

   etk_signal_connect_by_code(ETK_MENU_SHELL_ITEM_ADDED_SIGNAL, ETK_OBJECT(menu_bar), ETK_CALLBACK(_etk_menu_bar_item_added_cb), NULL);
   etk_signal_connect_by_code(ETK_MENU_SHELL_ITEM_REMOVED_SIGNAL, ETK_OBJECT(menu_bar), ETK_CALLBACK(_etk_menu_bar_item_removed_cb), NULL);
}

/* Calculates the ideal size of the menu bar */
static void _etk_menu_bar_size_request(Etk_Widget *widget, Etk_Size *size)
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
      size->w += child_size.w;
      size->h = ETK_MAX(size->h, child_size.h);
   }
}

/* Resizes the menu bar to the allocated size */
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
      Etk_Size child_size;

      etk_widget_size_request(ETK_WIDGET(l->data), &child_size);
      child_geometry.x = x_offset;
      child_geometry.w = child_size.w;
      child_geometry.h = geometry.h;

      etk_widget_size_allocate(ETK_WIDGET(l->data), child_geometry);
      x_offset += child_geometry.w;
   }
}

/**************************
 *
 * Callbacks and handlers
 *
 **************************/

/* Called when an item is added to the menu_bar */
static Etk_Bool _etk_menu_bar_item_added_cb(Etk_Object *object, void *item, void *data)
{
   Etk_Widget *menu_bar_widget;
   Etk_Object *item_object;

   if (!(menu_bar_widget = ETK_WIDGET(object)) || !(item_object = ETK_OBJECT(item)))
      return ETK_TRUE;

   etk_widget_theme_parent_set(ETK_WIDGET(item_object), menu_bar_widget);
   etk_signal_connect_by_code(ETK_WIDGET_MOUSE_UP_SIGNAL, item_object, ETK_CALLBACK(_etk_menu_bar_item_mouse_up_cb), NULL);
   etk_signal_connect_by_code(ETK_MENU_ITEM_SELECTED_SIGNAL, item_object, ETK_CALLBACK(_etk_menu_bar_item_selected_cb), NULL);
   etk_signal_connect_by_code(ETK_MENU_ITEM_UNSELECTED_SIGNAL, item_object, ETK_CALLBACK(_etk_menu_bar_item_unselected_cb), NULL);

   return ETK_TRUE;
}

/* Called when an item is removed from the menu_bar */
static Etk_Bool _etk_menu_bar_item_removed_cb(Etk_Object *object, void *item, void *data)
{
   Etk_Object *item_object;

   if (!(item_object = ETK_OBJECT(item)))
      return ETK_TRUE;

   etk_widget_theme_parent_set(ETK_WIDGET(item_object), NULL);
   etk_signal_disconnect_by_code(ETK_WIDGET_MOUSE_UP_SIGNAL, item_object, ETK_CALLBACK(_etk_menu_bar_item_mouse_up_cb), NULL);
   etk_signal_disconnect_by_code(ETK_MENU_ITEM_SELECTED_SIGNAL, item_object, ETK_CALLBACK(_etk_menu_bar_item_selected_cb), NULL);
   etk_signal_disconnect_by_code(ETK_MENU_ITEM_UNSELECTED_SIGNAL, item_object, ETK_CALLBACK(_etk_menu_bar_item_unselected_cb), NULL);

   return ETK_TRUE;
}

/* Called when the item is selected */
static Etk_Bool _etk_menu_bar_item_selected_cb(Etk_Object *object, void *data)
{
   Etk_Toplevel *toplevel;
   Etk_Menu_Item *item;
   Etk_Menu_Bar *menu_bar;
   Evas_List *l;

   if (!(item = ETK_MENU_ITEM(object)) || !(menu_bar = ETK_MENU_BAR(item->parent_shell)))
      return ETK_TRUE;

   /* First, we unselect all the items that belong to the same menu bar */
   for (l = ETK_MENU_SHELL(menu_bar)->items; l; l = l->next)
   {
      if (ETK_MENU_ITEM(l->data) == item)
         continue;
      etk_menu_item_unselect(ETK_MENU_ITEM(l->data));
   }

   /* Then we popup the child menu */
   if (item->submenu && (toplevel = etk_widget_toplevel_parent_get(ETK_WIDGET(menu_bar))))
   {
      int ix, iy, ih, ex, ey, sx, sy;

      etk_widget_geometry_get(ETK_WIDGET(item), &ix, &iy, NULL, &ih);
      etk_toplevel_evas_position_get(toplevel, &ex, &ey);
      etk_toplevel_screen_position_get(toplevel, &sx, &sy);
      etk_menu_popup_at_xy(item->submenu, sx + (ix - ex), sy + (iy - ey) + ih);
      etk_signal_connect_by_code(ETK_MENU_POPPED_DOWN_SIGNAL, ETK_OBJECT(item->submenu), ETK_CALLBACK(_etk_menu_bar_menu_popped_down_cb), NULL);
   }

   if (!menu_bar->move_callback)
   {
      etk_event_global_callback_add(ETK_EVENT_MOUSE_MOVE, _etk_menu_bar_mouse_move_cb, menu_bar);
      menu_bar->move_callback = ETK_TRUE;
   }

   return ETK_TRUE;
}

/* Called when the item is unselected */
static Etk_Bool _etk_menu_bar_item_unselected_cb(Etk_Object *object, void *data)
{
   Etk_Menu_Item *item;
   Etk_Menu_Bar *menu_bar;

   if (!(item = ETK_MENU_ITEM(object)) || !(menu_bar = ETK_MENU_BAR(item->parent_shell)))
      return ETK_TRUE;

   if (item->submenu)
      etk_menu_popdown(item->submenu);

   if (menu_bar->move_callback)
   {
      etk_event_global_callback_del(ETK_EVENT_MOUSE_MOVE, _etk_menu_bar_mouse_move_cb);
      menu_bar->move_callback = ETK_FALSE;
   }

   return ETK_TRUE;
}

/* Called when the user has clicked on the item */
static Etk_Bool _etk_menu_bar_item_mouse_up_cb(Etk_Object *object, void *event, void *data)
{
   etk_menu_item_select(ETK_MENU_ITEM(object));
   return ETK_TRUE;
}

/* Called when the user moves the mouse above the screen when a menu is popped up.
 * It popups other menus if the mouse is above another item of the menu bar */
static void _etk_menu_bar_mouse_move_cb(Etk_Event_Global event, void *data)
{
   Etk_Menu_Bar *menu_bar;
   Etk_Toplevel *toplevel;
   int tx, ty;
   Etk_Geometry item_geometry;
   Evas_List *l;
   Etk_Widget *item;

   if (!(menu_bar = ETK_MENU_BAR(data)) || !(toplevel = etk_widget_toplevel_parent_get(ETK_WIDGET(menu_bar))))
      return;

   /* If the mouse pointer is above a menu item, we select it */
   etk_toplevel_screen_position_get(toplevel, &tx, &ty);
   for (l = ETK_MENU_SHELL(menu_bar)->items; l; l = l->next)
   {
      item = ETK_WIDGET(l->data);
      etk_widget_geometry_get(item, &item_geometry.x, &item_geometry.y, &item_geometry.w, &item_geometry.h);

      if (ETK_INSIDE(event.mouse_move.pos.x - tx, event.mouse_move.pos.y - ty,
         item_geometry.x, item_geometry.y, item_geometry.w, item_geometry.h))
      {
         etk_menu_item_select(ETK_MENU_ITEM(item));
         break;
      }
   }
}

/* Called when a submenu of the menu-bar is popped down */
static Etk_Bool _etk_menu_bar_menu_popped_down_cb(Etk_Object *object, void *event, void *data)
{
   Etk_Menu *menu;

   if (!(menu = ETK_MENU(object)))
      return ETK_TRUE;

   etk_menu_item_unselect(etk_menu_parent_item_get(menu));
   etk_signal_disconnect_by_code(ETK_MENU_POPPED_DOWN_SIGNAL, ETK_OBJECT(menu), ETK_CALLBACK(_etk_menu_bar_menu_popped_down_cb), NULL);

   return ETK_TRUE;
}

/** @} */

/**************************
 *
 * Documentation
 *
 **************************/

/**
 * @addtogroup Etk_Menu_Bar
 *
 * @image html widgets/menu_bar.png
 * The menu bar is a menu shell whose items are packed horizontally.
 * A menu bar is usually packed at the top of a window. @n
 * To add or remove items, you have to use the functions provided by the Etk_Menu_Shell:
 * etk_menu_shell_append(), etk_menu_shell_remove(), ...
 *
 * \par Object Hierarchy:
 * - Etk_Object
 *   - Etk_Widget
 *     - Etk_Menu_Shell
 *       - Etk_Menu_Bar
 */
