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

/** @file etk_menu_bar.h */
#ifndef _ETK_MENU_BAR_H_
#define _ETK_MENU_BAR_H_

#include "etk_menu_shell.h"
#include "etk_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/* TODO/FIXME list:
 * - Improve default theme
 * - Add keyboard navigation
 */

/**
 * @defgroup Etk_Menu_Bar Etk_Menu_Bar
 * @brief A menu bar is a horizontal bar that can contain several menu items
 * @{
 */

/** Gets the type of a menu bar */
#define ETK_MENU_BAR_TYPE       (etk_menu_bar_type_get())
/** Casts the object to an Etk_Menu_Bar */
#define ETK_MENU_BAR(obj)       (ETK_OBJECT_CAST((obj), ETK_MENU_BAR_TYPE, Etk_Menu_Bar))
/** Check if the object is an Etk_Menu_Bar */
#define ETK_IS_MENU_BAR(obj)    (ETK_OBJECT_CHECK_TYPE((obj), ETK_MENU_BAR_TYPE))


/**
 * @brief @widget The structure of a menu bar
 * @structinfo
 */
struct Etk_Menu_Bar
{
   /* private: */
   /* Inherit from Etk_Menu_Shell */
   Etk_Menu_Shell menu_shell;

   Etk_Bool move_callback:1;
};


Etk_Type   *etk_menu_bar_type_get(void);
Etk_Widget *etk_menu_bar_new(void);

/** @} */

#ifdef __cplusplus
}
#endif

#endif
