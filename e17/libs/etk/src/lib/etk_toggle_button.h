/** @file etk_toggle_button.h */
#ifndef _ETK_TOGGLE_BUTTON_H_
#define _ETK_TOGGLE_BUTTON_H_

#include "etk_button.h"
#include "etk_types.h"

/**
 * @defgroup Etk_Toggle_Button Etk_Toggle_Button
 * @{
 */

/** @brief Gets the type of a toggle button */
#define ETK_TOGGLE_BUTTON_TYPE       (etk_toggle_button_type_get())
/** @brief Casts the object to an Etk_Toggle_Button */
#define ETK_TOGGLE_BUTTON(obj)       (ETK_OBJECT_CAST((obj), ETK_TOGGLE_BUTTON_TYPE, Etk_Toggle_Button))
/** @brief Checks if the object is an Etk_Toggle_Button */
#define ETK_IS_TOGGLE_BUTTON(obj)    (ETK_OBJECT_CHECK_TYPE((obj), ETK_TOGGLE_BUTTON_TYPE))

/**
 * @struct Etk_Toggle_Button
 * @brief A toggle button is a button that can be activated/deactivated. @n
 * It's the base class of check and radio buttons
 */
struct _Etk_Toggle_Button
{
   /* private: */
   /* Inherit from Etk_Button */
   Etk_Button button;
   
   void (*active_set)(Etk_Toggle_Button *toggle_button, Etk_Bool active);
   void (*toggled)(Etk_Toggle_Button *toggle_button);
   Etk_Bool active;
};

Etk_Type *etk_toggle_button_type_get();
Etk_Widget *etk_toggle_button_new();
Etk_Widget *etk_toggle_button_new_with_label(const char *label);

void etk_toggle_button_toggle(Etk_Toggle_Button *toggle_button);
void etk_toggle_button_active_set(Etk_Toggle_Button *toggle_button, Etk_Bool active);
Etk_Bool etk_toggle_button_active_get(Etk_Toggle_Button *toggle_button);

/** @} */

#endif
