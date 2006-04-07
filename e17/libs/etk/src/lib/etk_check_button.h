/** @file etk_check_button.h */
#ifndef _ETK_CHECK_BUTTON_H_
#define _ETK_CHECK_BUTTON_H_

#include "etk_toggle_button.h"
#include "etk_types.h"

/**
 * @defgroup Etk_Check_Button Etk_Check_Button
 * @{
 */

/** @brief Gets the type of a check button */
#define ETK_CHECK_BUTTON_TYPE       (etk_check_button_type_get())
/** @brief Casts the object to an Etk_Check_Button */
#define ETK_CHECK_BUTTON(obj)       (ETK_OBJECT_CAST((obj), ETK_CHECK_BUTTON_TYPE, Etk_Check_Button))
/** @brief Checks if the object is an Etk_Check_Button */
#define ETK_IS_CHECK_BUTTON(obj)    (ETK_OBJECT_CHECK_TYPE((obj), ETK_CHECK_BUTTON_TYPE))

/**
 * @struct Etk_Check_Button
 * @brief An Etk_Check_Button is a button that can be checked and unckecked
 */
struct _Etk_Check_Button
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
