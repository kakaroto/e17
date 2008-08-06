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

/** @file etk_popup_window.h */
#ifndef _ETK_POPUP_WINDOW_H_
#define _ETK_POPUP_WINDOW_H_

#include <Evas.h>

#include "etk_types.h"
#include "etk_window.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup Etk_Popup_Window Etk_Popup_Window
 * @brief The Etk_Popup_Window widget is a borderless window that can pops up/down. It is used by menus and comboboxes
 * @{
 */

/** Gets the type of a popup window */
#define ETK_POPUP_WINDOW_TYPE       (etk_popup_window_type_get())
/** Casts the object to an Etk_Popup_Window */
#define ETK_POPUP_WINDOW(obj)       (ETK_OBJECT_CAST((obj), ETK_POPUP_WINDOW_TYPE, Etk_Popup_Window))
/** Check if the object is an Etk_Popup_Window */
#define ETK_IS_POPUP_WINDOW(obj)    (ETK_OBJECT_CHECK_TYPE((obj), ETK_POPUP_WINDOW_TYPE))

extern int ETK_POPUP_WINDOW_POPPED_DOWN_SIGNAL;
extern int ETK_POPUP_WINDOW_POPPED_UP_SIGNAL;

/** @brief The directions to which the popup window is popped up */
typedef enum
{
   ETK_POPUP_BELOW_RIGHT,       /**< The window is popped up on the right, below the given position (default) */
   ETK_POPUP_BELOW_LEFT,        /**< The window is popped up on the left, below the given position */
   ETK_POPUP_ABOVE_RIGHT,       /**< The window is popped up on the right, above the given position */
   ETK_POPUP_ABOVE_LEFT         /**< The window is popped up on the left, above the given position */
} Etk_Popup_Direction;


/**
 * @brief @widget A borderless window that can be popped up/down
 * @structinfo
 */
struct Etk_Popup_Window
{
   /* private: */
   /* Inherit from Etk_Window */
   Etk_Window window;

   Etk_Popup_Window *parent;
   Evas_List *children;
   Etk_Popup_Window *popped_child;
   Etk_Bool popped_up:1;
};


Etk_Type         *etk_popup_window_type_get(void);

void              etk_popup_window_parent_set(Etk_Popup_Window *popup_window, Etk_Popup_Window *parent);
Etk_Popup_Window *etk_popup_window_parent_get(Etk_Popup_Window *popup_window);
void              etk_popup_window_focused_window_set(Etk_Popup_Window *popup_window);
Etk_Popup_Window *etk_popup_window_focused_window_get(void);

void              etk_popup_window_popup(Etk_Popup_Window *popup_window);
void              etk_popup_window_popup_in_direction(Etk_Popup_Window *popup_window, Etk_Popup_Direction direction);
void              etk_popup_window_popup_at_xy(Etk_Popup_Window *popup_window, int x, int y);
void              etk_popup_window_popup_at_xy_in_direction(Etk_Popup_Window *popup_window, int x, int y, Etk_Popup_Direction direction);
void              etk_popup_window_popdown(Etk_Popup_Window *popup_window);
void              etk_popup_window_popdown_all(void);
Etk_Bool          etk_popup_window_is_popped_up(Etk_Popup_Window *popup_window);

/** @} */

#ifdef __cplusplus
}
#endif

#endif
