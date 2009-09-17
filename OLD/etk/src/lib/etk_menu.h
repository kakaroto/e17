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

/** @file etk_menu.h */
#ifndef _ETK_MENU_H_
#define _ETK_MENU_H_

#include "etk_menu_shell.h"
#include "etk_popup_window.h"
#include "etk_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/* TODO/FIXME list:
 * - Keyboard navigation!
 * - Sometimes an item is selected when the menu is popped up. Probably a bug in Etk_Popup_Window's feeding code
 */

/**
 * @defgroup Etk_Menu Etk_Menu
 * @brief The Etk_Menu widget is a menu shell implemented in a popup window
 * @{
 */

/** Gets the type of a menu */
#define ETK_MENU_TYPE       (etk_menu_type_get())
/** Casts the object to an Etk_Menu */
#define ETK_MENU(obj)       (ETK_OBJECT_CAST((obj), ETK_MENU_TYPE, Etk_Menu))
/** Check if the object is an Etk_Menu */
#define ETK_IS_MENU(obj)    (ETK_OBJECT_CHECK_TYPE((obj), ETK_MENU_TYPE))

extern int ETK_MENU_POPPED_DOWN_SIGNAL;
extern int ETK_MENU_POPPED_UP_SIGNAL;

/**
 * @brief @widget A menu shell implemented in a popup window
 * @structinfo
 */
struct Etk_Menu
{
   /* private: */
   /* Inherit from Etk_Menu_Shell */
   Etk_Menu_Shell menu_shell;

   Etk_Popup_Window *window;
   Etk_Menu_Item *parent_item;
};


Etk_Type      *etk_menu_type_get(void);
Etk_Widget    *etk_menu_new(void);
Etk_Menu_Item *etk_menu_parent_item_get(Etk_Menu *menu);

void           etk_menu_popup(Etk_Menu *menu);
void           etk_menu_popup_in_direction(Etk_Menu *menu, Etk_Popup_Direction direction);
void           etk_menu_popup_at_xy(Etk_Menu *menu, int x, int y);
void           etk_menu_popup_at_xy_in_direction(Etk_Menu *menu, int x, int y, Etk_Popup_Direction direction);
void           etk_menu_popdown(Etk_Menu *menu);

/** @} */

#ifdef __cplusplus
}
#endif

#endif
