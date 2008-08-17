/* vim: set sw=8 ts=8 sts=8 expandtab: */
#ifndef EWL_CHECKBUTTON_H
#define EWL_CHECKBUTTON_H

#include "ewl_button.h"
#include "ewl_view.h"

/**
 * @addtogroup Ewl_Checkbutton Ewl_Checkbutton: An Ewl_Checkbutton with Label
 * @brief Defines an Ewl_Checkbutton that inherits from Ewl_Widget and
 * provides an Ewl_Check that changes value on each click.
 *
 * @remarks Inherits from Ewl_Button.
 * @if HAVE_IMAGES
 * @image html Ewl_Checkbutton_inheritance.png
 * @endif
 *
 * @{
 */

/**
 * @themekey /checkbutton/file
 * @themekey /checkbutton/group
 */

/**
 * @def EWL_CHECKBUTTON_TYPE
 * The type name for the Ewl_Checkbutton widget
 */
#define EWL_CHECKBUTTON_TYPE "checkbutton"

/**
 * @def EWL_CHECKBUTTON_IS(w)
 * Returns TRUE if the widget is an Ewl_Checkbutton, FALSE otherwise
 */
#define EWL_CHECKBUTTON_IS(w) (ewl_widget_type_is(EWL_WIDGET(w), EWL_CHECKBUTTON_TYPE))

/**
 * The button class is a basic button with a label. This class is inherited by
 * the check button and radio button classes.
 */
typedef struct Ewl_Checkbutton Ewl_Checkbutton;

/**
 * @def EWL_CHECKBUTTON(button)
 * Typecasts a pointer to an Ewl_Checkbutton pointer.
 */
#define EWL_CHECKBUTTON(button) ((Ewl_Checkbutton *) button)

/**
 * @brief Inherits from Ewl_Button and expands to provide a stateful check button.
 */
struct Ewl_Checkbutton
{
        Ewl_Button button;              /**< Inherit the basic button properties */
        Ewl_Position label_position;    /**< Order of label and check */
        Ewl_Widget *check;              /**< Check widget represented */
};

Ewl_Widget       *ewl_checkbutton_new(void);
int               ewl_checkbutton_init(Ewl_Checkbutton *cb);

void              ewl_checkbutton_checked_set(Ewl_Checkbutton *cb, int checked);
int               ewl_checkbutton_is_checked(Ewl_Checkbutton *cb);

void              ewl_checkbutton_label_position_set(Ewl_Checkbutton *cb,
                                                           Ewl_Position p);
Ewl_View         *ewl_checkbutton_view_get(void);

/*
 * Internally used callbacks, override at your own risk
 */
void ewl_checkbutton_cb_clicked(Ewl_Widget *w, void *ev_data, void *user_data);

/**
 * @}
 */

#endif
