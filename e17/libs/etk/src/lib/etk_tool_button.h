/** @file etk_tool_button.h */
#ifndef _ETK_TOOL_BUTTON_H_
#define _ETK_TOOL_BUTTON_H_

#include "etk_button.h"
#include "etk_toggle_button.h"
#include "etk_stock.h"
#include "etk_types.h"

/**
 * @defgroup Etk_Tool_Button Etk_Tool_Button
 * @brief The Etk_Tool_Button widget is similar to an Etk_Button but it has a specific toolbar friendly theme
 * @{
 */

/** Gets the type of a tool button */
#define ETK_TOOL_BUTTON_TYPE       (etk_tool_button_type_get())
/** Casts the object to an Etk_Tool_Button */
#define ETK_TOOL_BUTTON(obj)       (ETK_OBJECT_CAST((obj), ETK_TOOL_BUTTON_TYPE, Etk_Tool_Button))
/** Checks if the object is an Etk_Tool_Button */
#define ETK_IS_TOOL_BUTTON(obj)    (ETK_OBJECT_CHECK_TYPE((obj), ETK_TOOL_BUTTON_TYPE))

/**
 * @brief @widget The structure of a tool button
 * @structinfo
 */
struct _Etk_Tool_Button
{
   /* private: */
   /* Inherit from Etk_Button */
   Etk_Button button;
};

Etk_Type *etk_tool_button_type_get();
Etk_Widget *etk_tool_button_new();
Etk_Widget *etk_tool_button_new_with_label(const char *label);
Etk_Widget *etk_tool_button_new_from_stock(Etk_Stock_Id stock_id);

/** @} */

/**
 * @defgroup Etk_Tool_Toggle_Button Etk_Tool_Toggle_Button
 * @brief The Etk_Tool_Toggle_Button widget is similar to an Etk_Toggle_Button but it has a specific toolbar friendly theme
 * @{
 */

/** Gets the type of a tool toggle button */
#define ETK_TOOL_TOGGLE_BUTTON_TYPE       (etk_tool_toggle_button_type_get())
/** Casts the object to an Etk_Tool_Toggle_Button */
#define ETK_TOOL_TOGGLE_BUTTON(obj)       (ETK_OBJECT_CAST((obj), ETK_TOOL_TOGGLE_BUTTON_TYPE, Etk_Tool_Toggle_Button))
/** Checks if the object is an Etk_Tool_Toggle_Button */
#define ETK_IS_TOOL_TOGGLE_BUTTON(obj)    (ETK_OBJECT_CHECK_TYPE((obj), ETK_TOOL_TOGGLE_BUTTON_TYPE))

/**
 * @brief @widget The structure of a tool toggle button
 * @structinfo
 */
struct _Etk_Tool_Toggle_Button
{
   /* private: */
   /* Inherit from Etk_Toggle_Button */
   Etk_Toggle_Button button;
};

Etk_Type *etk_tool_toggle_button_type_get();
Etk_Widget *etk_tool_toggle_button_new();
Etk_Widget *etk_tool_toggle_button_new_with_label(const char *label);
Etk_Widget *etk_tool_toggle_button_new_from_stock(Etk_Stock_Id stock_id);

/** @} */

#endif
