/** @file etk_tool_button.c */
#include "etk_tool_button.h"
#include <stdlib.h>
#include <string.h>

#include "etk_button.h"


/**
 * @addtogroup Etk_Tool_Button
 * @{
 */

static void _etk_tool_button_constructor(Etk_Tool_Button *tool_button);

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
         ETK_CONSTRUCTOR(_etk_tool_button_constructor), NULL);
   }

   return tool_button_type;
}

/**
 * @brief Creates a new tool button
 * @return Returns the new tool button widget
 */
Etk_Widget *etk_tool_button_new()
{
   return etk_widget_new(ETK_TOOL_BUTTON_TYPE, "theme_group", "toolbar/button",
      "tool", ETK_TRUE, NULL);
}

/**
 * @brief Creates a new tool button with a label
 * @return Returns the new tool button widget
 */
Etk_Widget *etk_tool_button_new_with_label(const char *label)
{
   return etk_widget_new(ETK_TOOL_BUTTON_TYPE, "theme_group", "toolbar/button",
      "label", label, "tool", ETK_TRUE, NULL);
}

/**
 * @brief Creates a new tool button with a label and an icon defined by a stock id
 * @param stock_id the stock id corresponding to a label and an icon
 * @return Returns the new tool button widget
 * @see Etk_Stock
 */
Etk_Widget *etk_tool_button_new_from_stock(Etk_Stock_Id stock_id)
{
   Etk_Widget *tool_button;
   
   tool_button = etk_tool_button_new();
   etk_button_set_from_stock(ETK_BUTTON(tool_button), stock_id);

   return tool_button;
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
