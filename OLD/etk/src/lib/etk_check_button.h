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

/** @file etk_check_button.h */
#ifndef _ETK_CHECK_BUTTON_H_
#define _ETK_CHECK_BUTTON_H_

#include "etk_toggle_button.h"
#include "etk_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup Etk_Check_Button Etk_Check_Button
 * @brief The Etk_Check_Button widget is a toggle button made up of a checkbox and a label
 * @{
 */

/** Gets the type of a check button */
#define ETK_CHECK_BUTTON_TYPE       (etk_check_button_type_get())
/** Casts the object to an Etk_Check_Button */
#define ETK_CHECK_BUTTON(obj)       (ETK_OBJECT_CAST((obj), ETK_CHECK_BUTTON_TYPE, Etk_Check_Button))
/** Checks if the object is an Etk_Check_Button */
#define ETK_IS_CHECK_BUTTON(obj)    (ETK_OBJECT_CHECK_TYPE((obj), ETK_CHECK_BUTTON_TYPE))


/**
 * @brief @widget A toggle button made up of a checkbox and a label
 * @structinfo
 */
struct Etk_Check_Button
{
   /* private: */
   /* Inherit from Etk_Toggle_Button */
   Etk_Toggle_Button toggle_button;
};


Etk_Type   *etk_check_button_type_get(void);
Etk_Widget *etk_check_button_new(void);
Etk_Widget *etk_check_button_new_with_label(const char *label);

/** @} */

#ifdef __cplusplus
}
#endif

#endif
