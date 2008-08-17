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

/** @file etk_toggle_button.h */
#ifndef _ETK_TOGGLE_BUTTON_H_
#define _ETK_TOGGLE_BUTTON_H_

#include "etk_button.h"
#include "etk_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup Etk_Toggle_Button Etk_Toggle_Button
 * @brief An Etk_Toggle_Button is a button that can be activated and deactivated by clicking on it.
 * It is the base class of check and radio buttons.
 * @{
 */

/** @brief Gets the type of a toggle button */
#define ETK_TOGGLE_BUTTON_TYPE       (etk_toggle_button_type_get())
/** @brief Casts the object to an Etk_Toggle_Button */
#define ETK_TOGGLE_BUTTON(obj)       (ETK_OBJECT_CAST((obj), ETK_TOGGLE_BUTTON_TYPE, Etk_Toggle_Button))
/** @brief Checks if the object is an Etk_Toggle_Button */
#define ETK_IS_TOGGLE_BUTTON(obj)    (ETK_OBJECT_CHECK_TYPE((obj), ETK_TOGGLE_BUTTON_TYPE))

extern int ETK_TOGGLE_BUTTON_TOGGLED_SIGNAL;

/**
 * @brief @widget A button that can be activated/deactivated
 * @structinfo
 */
struct Etk_Toggle_Button
{
   /* private: */
   /* Inherit from Etk_Button */
   Etk_Button button;

   void (*active_set)(Etk_Toggle_Button *toggle_button, Etk_Bool active);
   Etk_Bool (*toggled_handler)(Etk_Toggle_Button *toggle_button);
   Etk_Bool active:1;
};


Etk_Type   *etk_toggle_button_type_get(void);
Etk_Widget *etk_toggle_button_new(void);
Etk_Widget *etk_toggle_button_new_with_label(const char *label);

Etk_Bool    etk_toggle_button_toggle(Etk_Toggle_Button *toggle_button);
void        etk_toggle_button_active_set(Etk_Toggle_Button *toggle_button, Etk_Bool active);
Etk_Bool    etk_toggle_button_active_get(Etk_Toggle_Button *toggle_button);

/** @} */

#ifdef __cplusplus
}
#endif

#endif
