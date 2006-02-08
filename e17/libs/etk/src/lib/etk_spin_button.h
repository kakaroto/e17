/** @file etk_spin_button.h */
#ifndef _ETK_SPIN_BUTTON_H_
#define _ETK_SPIN_BUTTON_H_

#include "etk_entry.h"
#include "etk_types.h"

/**
 * @defgroup Etk_Spin_Button Etk_Spin_Button
 * @{
 */

/** @brief Gets the type of a spin button */
#define ETK_SPIN_BUTTON_TYPE       (etk_spin_button_type_get())
/** @brief Casts the object to an Etk_Spin_Button */
#define ETK_SPIN_BUTTON(obj)       (ETK_OBJECT_CAST((obj), ETK_SPIN_BUTTON_TYPE, Etk_Spin_Button))
/** @brief Checks if the object is an Etk_Spin_Button */
#define ETK_IS_SPIN_BUTTON(obj)    (ETK_OBJECT_CHECK_TYPE((obj), ETK_SPIN_BUTTON_TYPE))

struct _Etk_Spin_Button
{
   /* private: */
   /* Inherit from Etk_Entry */
   Etk_Entry entry;
   
   Etk_Widget *range;
};

Etk_Type *etk_spin_button_type_get();
Etk_Widget *etk_spin_button_new(double min, double max, double step);

/** @} */

#endif
