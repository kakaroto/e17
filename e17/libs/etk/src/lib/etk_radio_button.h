/** @file etk_radio_button.h */
#ifndef _ETK_RADIO_BUTTON_H_
#define _ETK_RADIO_BUTTON_H_

#include <Evas.h>
#include "etk_check_button.h"
#include "etk_types.h"

/**
 * @defgroup Etk_Radio_Button Etk_Radio_Button
 * @{
 */

/** @brief Gets the type of a radio button */
#define ETK_RADIO_BUTTON_TYPE       (etk_radio_button_type_get())
/** @brief Casts the object to an Etk_Radio_Button */
#define ETK_RADIO_BUTTON(obj)       (ETK_OBJECT_CAST((obj), ETK_RADIO_BUTTON_TYPE, Etk_Radio_Button))
/** @brief Checks if the object is an Etk_Radio_Button */
#define ETK_IS_RADIO_BUTTON(obj)    (ETK_OBJECT_RADIO_TYPE((obj), ETK_RADIO_BUTTON_TYPE))

/**
 * @struct Etk_Radio_Button
 * @brief A radio button is a toggle button that belongs to a group in which only one button can be toggled
 */
struct _Etk_Radio_Button
{
   /* private: */
   /* Inherit from Etk_Check_Button */
   Etk_Check_Button check_button;

   Evas_List **group;
   Etk_Bool can_uncheck;
};

Etk_Type *etk_radio_button_type_get();
Etk_Widget *etk_radio_button_new(Evas_List **group);
Etk_Widget *etk_radio_button_new_from_widget(Etk_Radio_Button *radio_button);
Etk_Widget *etk_radio_button_new_with_label(const char *label, Evas_List **group);
Etk_Widget *etk_radio_button_new_with_label_from_widget(const char *label, Etk_Radio_Button *radio_button);

void etk_radio_button_group_set(Etk_Radio_Button *radio_button, Evas_List **group);
Evas_List **etk_radio_button_group_get(Etk_Radio_Button *radio_button);

/** @} */

#endif
