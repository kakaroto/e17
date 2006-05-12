/** @file etk_check_button.h */
#ifndef _ETK_CHECK_BUTTON_H_
#define _ETK_CHECK_BUTTON_H_

#include "etk_toggle_button.h"
#include "etk_types.h"

/**
 * @defgroup Etk_Check_Button Etk_Check_Button
 * @brief The Etk_Check_Button widget is a toggle button made up of a checkbox, which can be checked or unchecked, and a label
 * @{
 */

/** Gets the type of a check button */
#define ETK_CHECK_BUTTON_TYPE       (etk_check_button_type_get())
/** Casts the object to an Etk_Check_Button */
#define ETK_CHECK_BUTTON(obj)       (ETK_OBJECT_CAST((obj), ETK_CHECK_BUTTON_TYPE, Etk_Check_Button))
/** Checks if the object is an Etk_Check_Button */
#define ETK_IS_CHECK_BUTTON(obj)    (ETK_OBJECT_CHECK_TYPE((obj), ETK_CHECK_BUTTON_TYPE))

/**
 * @brief @widget The structure of a check button
 * @structinfo
 */
struct Etk_Check_Button
{
   /* private: */
   /* Inherit from Etk_Toggle_Button */
   Etk_Toggle_Button toggle_button;
};

Etk_Type *etk_check_button_type_get();
Etk_Widget *etk_check_button_new();
Etk_Widget *etk_check_button_new_with_label(const char *label);

/** @} */

#endif
