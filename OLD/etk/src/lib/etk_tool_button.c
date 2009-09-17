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

/** @file etk_tool_button.c */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "etk_tool_button.h"

#include <stdlib.h>
#include <string.h>

/**
 * @addtogroup Etk_Tool_Button
 * @{
 */

static void _etk_tool_button_constructor(Etk_Tool_Button *tool_button);
static void _etk_tool_toggle_button_constructor(Etk_Tool_Toggle_Button *tool_toggle_button);

/**************************
 *
 * Implementation
 *
 **************************/

/**
 * @brief Gets the type of an Etk_Tool_Button
 * @return Returns the type of an Etk_Tool_Button
 */
Etk_Type *etk_tool_button_type_get()
{
   static Etk_Type *tool_button_type = NULL;

   if (!tool_button_type)
   {
      tool_button_type = etk_type_new("Etk_Tool_Button", ETK_BUTTON_TYPE, sizeof(Etk_Tool_Button),
         ETK_CONSTRUCTOR(_etk_tool_button_constructor), NULL, NULL);
   }

   return tool_button_type;
}

/**
 * @brief Creates a new tool button
 * @return Returns the new tool button widget
 */
Etk_Widget *etk_tool_button_new()
{
   return etk_widget_new(ETK_TOOL_BUTTON_TYPE, "theme-group", "tool_button",
      "style", ETK_BUTTON_BOTH_VERT, NULL);
}

/**
 * @brief Creates a new tool button with a label
 * @return Returns the new tool button widget
 */
Etk_Widget *etk_tool_button_new_with_label(const char *label)
{
   return etk_widget_new(ETK_TOOL_BUTTON_TYPE, "theme-group", "tool_button",
      "label", label, "style", ETK_BUTTON_BOTH_VERT, NULL);
}

/**
 * @brief Creates a new tool toggle button with a label and an icon defined by a stock id
 * @param stock_id the stock id corresponding to a label and an icon
 * @return Returns the new tool toggle button widget
 * @see Etk_Stock
 */
Etk_Widget *etk_tool_button_new_from_stock(Etk_Stock_Id stock_id)
{
   Etk_Widget *tool_button;

   tool_button = etk_tool_button_new();
   etk_button_set_from_stock(ETK_BUTTON(tool_button), stock_id);

   return tool_button;
}

/**
 * @brief Gets the type of an Etk_Tool_Toggle_Button
 * @return Returns the type of an Etk_Tool_Toggle_Button
 */
Etk_Type *etk_tool_toggle_button_type_get()
{
   static Etk_Type *tool_toggle_button_type = NULL;

   if (!tool_toggle_button_type)
   {
      tool_toggle_button_type = etk_type_new("Etk_Tool_Toggle_Button", ETK_TOGGLE_BUTTON_TYPE, sizeof(Etk_Tool_Toggle_Button),
         ETK_CONSTRUCTOR(_etk_tool_toggle_button_constructor), NULL, NULL);
   }

   return tool_toggle_button_type;
}

/**
 * @brief Creates a new tool toggle button
 * @return Returns the new tool toggle button widget
 */
Etk_Widget *etk_tool_toggle_button_new()
{
   return etk_widget_new(ETK_TOOL_TOGGLE_BUTTON_TYPE, "theme-group", "tool_toggle_button",
      "style", ETK_BUTTON_BOTH_VERT, NULL);
}

/**
 * @brief Creates a new tool toggle button with a label
 * @return Returns the new tool toggle button widget
 */
Etk_Widget *etk_tool_toggle_button_new_with_label(const char *label)
{
   return etk_widget_new(ETK_TOOL_TOGGLE_BUTTON_TYPE, "theme-group", "tool_toggle_button",
      "label", label, "style", ETK_BUTTON_BOTH_VERT, NULL);
}

/**
 * @brief Creates a new tool toggle button with a label and an icon defined by a stock id
 * @param stock_id the stock id corresponding to a label and an icon
 * @return Returns the new tool toggle button widget
 * @see Etk_Stock
 */
Etk_Widget *etk_tool_toggle_button_new_from_stock(Etk_Stock_Id stock_id)
{
   Etk_Widget *tool_toggle_button;

   tool_toggle_button = etk_tool_toggle_button_new();
   etk_button_set_from_stock(ETK_BUTTON(tool_toggle_button), stock_id);

   return tool_toggle_button;
}

/**************************
 *
 * Etk specific functions
 *
 **************************/

/* Initializes the tool button */
static void _etk_tool_button_constructor(Etk_Tool_Button *tool_button)
{
   if (!tool_button)
      return;
}

/* Initializes the tool toggle button */
static void _etk_tool_toggle_button_constructor(Etk_Tool_Toggle_Button *tool_toggle_button)
{
   if (!tool_toggle_button)
      return;
}

/** @} */

/**************************
 *
 * Documentation
 *
 **************************/

/**
 * @addtogroup Etk_Tool_Button
 *
 * @image html widgets/tool_button.png
 *
 * \par Object Hierarchy:
 * - Etk_Object
 *   - Etk_Widget
 *     - Etk_Container
 *       - Etk_Bin
 *         - Etk_Button
 *           - Etk_Tool_Button
 */

/**
 * @addtogroup Etk_Tool_Toggle_Button
 *
 * @image html widgets/tool_toggle_button.png
 *
 * \par Object Hierarchy:
 * - Etk_Object
 *   - Etk_Widget
 *     - Etk_Container
 *       - Etk_Bin
 *         - Etk_Button
 *           - Etk_Toggle_Button
 *             - Etk_Tool_Toggle_Button
 */
