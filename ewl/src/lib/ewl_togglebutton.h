/* vim: set sw=8 ts=8 sts=8 expandtab: */
#ifndef EWL_TOGGLEBUTTON_H
#define EWL_TOGGLEBUTTON_H

#include "ewl_button.h"

/**
 * @addtogroup Ewl_Togglebutton Ewl_Togglebutton: An Ewl_Togglebutton with Label
 * @brief Defines an Ewl_Togglebutton that inherits from Ewl_Button and
 * provides a button that serves as a switch so it has a pressed and a 
 * realeased state.
 *
 * @remarks Inherits from Ewl_Button.
 * @if HAVE_IMAGES
 * @image html Ewl_Togglebutton_inheritance.png
 * @endif
 *
 * @{
 */

/**
 * @themekey /togglebutton/file
 * @themekey /togglebutton/group
 */

/**
 * @def EWL_TOGGLEBUTTON_TYPE
 * The type name for the Ewl_Togglebutton widget
 */
#define EWL_TOGGLEBUTTON_TYPE "togglebutton"

/**
 * @def EWL_TOGGLEBUTTON_IS(w)
 * Returns TRUE if the widget is an Ewl_Togglebutton, FALSE otherwise
 */
#define EWL_TOGGLEBUTTON_IS(w) (ewl_widget_type_is(EWL_WIDGET(w), EWL_TOGGLEBUTTON_TYPE))

/**
 * The toggle button class inherits from Ewl_Button
 */
typedef struct Ewl_Togglebutton Ewl_Togglebutton;

/**
 * @def EWL_TOGGLEBUTTON(button)
 * Typecasts a pointer to an Ewl_Togglebutton pointer.
 */
#define EWL_TOGGLEBUTTON(button) ((Ewl_Togglebutton *) button)

/**
 * @brief Inherits from Ewl_Button and expands to provide a stateful toggle button.
 */
struct Ewl_Togglebutton
{
        Ewl_Button button;       /**< Inherit the basic button properties */
        unsigned char checked:1; /**< Wether the button is pressed or not */
};

Ewl_Widget      *ewl_togglebutton_new(void);
int              ewl_togglebutton_init(Ewl_Togglebutton *tb);

void             ewl_togglebutton_checked_set(Ewl_Togglebutton *tb,
                                                        unsigned int checked);
unsigned int     ewl_togglebutton_checked_get(Ewl_Togglebutton *tb);

/*
 * Internally used callbacks, override at your own risk
 */
void ewl_togglebutton_cb_clicked(Ewl_Widget *w, void *ev_data, void *user_data);

/**
 * @}
 */

#endif
