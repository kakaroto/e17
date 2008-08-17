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

/** @file etk_menu_shell.h */
#ifndef _ETK_MENU_SHELL_H_
#define _ETK_MENU_SHELL_H_

#include <Evas.h>

#include "etk_types.h"
#include "etk_widget.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup Etk_Menu_Shell Etk_Menu_Shell
 * @brief Etk_Menu_Shell is the base class used by Etk_Menu and Etk_Menu_Bar
 * @{
 */

/** Gets the type of a menu shell */
#define ETK_MENU_SHELL_TYPE       (etk_menu_shell_type_get())
/** Casts the object to an Etk_Menu_Shell */
#define ETK_MENU_SHELL(obj)       (ETK_OBJECT_CAST((obj), ETK_MENU_SHELL_TYPE, Etk_Menu_Shell))
/** Check if the object is an Etk_Menu_Shell */
#define ETK_IS_MENU_SHELL(obj)    (ETK_OBJECT_CHECK_TYPE((obj), ETK_MENU_SHELL_TYPE))

extern int ETK_MENU_SHELL_ITEM_ADDED_SIGNAL;
extern int ETK_MENU_SHELL_ITEM_REMOVED_SIGNAL;


/**
 * @brief @widget A base class used by Etk_Menu and Etk_Menu_Bar
 * @structinfo
 */
struct Etk_Menu_Shell
{
   /* private: */
   /* Inherit from Etk_Widget */
   Etk_Widget widget;

   Evas_List *items;
};


Etk_Type  *etk_menu_shell_type_get(void);

void       etk_menu_shell_prepend(Etk_Menu_Shell *menu_shell, Etk_Menu_Item *item);
void       etk_menu_shell_append(Etk_Menu_Shell *menu_shell, Etk_Menu_Item *item);
void       etk_menu_shell_prepend_relative(Etk_Menu_Shell *menu_shell, Etk_Menu_Item *item, Etk_Menu_Item *relative);
void       etk_menu_shell_append_relative(Etk_Menu_Shell *menu_shell, Etk_Menu_Item *item, Etk_Menu_Item *relative);
void       etk_menu_shell_insert(Etk_Menu_Shell *menu_shell, Etk_Menu_Item *item, int position);
void       etk_menu_shell_remove(Etk_Menu_Shell *menu_shell, Etk_Menu_Item *item);

Evas_List *etk_menu_shell_items_get(Etk_Menu_Shell *menu_shell);

/** @} */

#ifdef __cplusplus
}
#endif

#endif
