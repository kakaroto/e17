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

/** @file etk_check_button.c */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "etk_check_button.h"

#include <stdlib.h>

/**
 * @addtogroup Etk_Check_Button
 * @{
 */

/**************************
 *
 * Implementation
 *
 **************************/

/**
 * @internal
 * @brief Gets the type of an Etk_Check_Button
 * @return Returns the type of an Etk_Check_Button
 */
Etk_Type *etk_check_button_type_get(void)
{
   static Etk_Type *check_button_type = NULL;

   if (!check_button_type)
   {
      check_button_type = etk_type_new("Etk_Check_Button",
         ETK_TOGGLE_BUTTON_TYPE, sizeof(Etk_Check_Button), NULL, NULL, NULL);
   }

   return check_button_type;
}

/**
 * @brief Creates a new check button
 * @return Returns the new check button widget
 */
Etk_Widget *etk_check_button_new(void)
{
   return etk_widget_new(ETK_CHECK_BUTTON_TYPE, "theme-group", "check_button",
         "focusable", ETK_TRUE, "xalign", 0.0, NULL);
}

/**
 * @brief Creates a new check button with a label
 * @param label the label
 * @return Returns the new check button widget
 */
Etk_Widget *etk_check_button_new_with_label(const char *label)
{
   return etk_widget_new(ETK_CHECK_BUTTON_TYPE, "theme-group", "check_button",
         "label", label, "focusable", ETK_TRUE, "xalign", 0.0, NULL);
}

/** @} */

/**************************
 *
 * Documentation
 *
 **************************/

/**
 * @addtogroup Etk_Check_Button
 *
 * @image html widgets/check_button.png
 *
 * Since Etk_Check_Button inherits from Etk_Toggle_Button, you can use the @a etk_toggle_button_*() functions to
 * change the state of the check-button. For example, to know if the button is checked, you should call
 * etk_toggle_button_active_get(). @n
 * You can also be notified when the state of the check button is changed with the @b "toggled" signal of
 * Etk_Toggle_Button.
 *
 * \par Object Hierarchy:
 * - Etk_Object
 *   - Etk_Widget
 *     - Etk_Container
 *       - Etk_Bin
 *         - Etk_Button
 *           - Etk_Toggle_Button
 *             - Etk_Check_Button
 */
