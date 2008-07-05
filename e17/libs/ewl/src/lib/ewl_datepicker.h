/* vim: set sw=8 ts=8 sts=8 expandtab: */
#ifndef EWL_DATEPICKER_H
#define EWL_DATEPICKER_H

#include "ewl_text.h"

/**
 * @addtogroup Ewl_Datepicker Ewl_Datepicker: The EWL Datepicker widget
 * @brief Defines the Ewl_Datepicker class,
 *
 * @remarks Inherits from Ewl_Text.
 * @if HAVE_IMAGES
 * @image html Ewl_Datepicker_inheritance.png
 * @endif
 *
 * @{
 */

/**
 * @themekey /datepicker/file
 * @themekey /datepicker/group
 */

/**
 * @def EWL_DATEPICKER_TYPE
 * The type name for the Ewl_Datepicker widget
 */
#define EWL_DATEPICKER_TYPE "datepicker"

/**
 * @def EWL_DATEPICKER_IS(w)
 * Returns TRUE if the widget is an Ewl_Datepicker, FALSE otherwise
 */
#define EWL_DATEPICKER_IS(w) (ewl_widget_type_is(EWL_WIDGET(w), EWL_DATEPICKER_TYPE))

/**
 * The Ewl_Datepicker
 */
typedef struct Ewl_Datepicker Ewl_Datepicker;

/**
 * @def EWL_DATEPICKER(datepicker)
 * Typecast a pointer to an Ewl_Datepicker pointer
 */
#define EWL_DATEPICKER(datepicker) ((Ewl_Datepicker *) datepicker)

/**
 * @brief Inherits from Ewl_Text and extends to provide a date picker
 */
struct Ewl_Datepicker
{
        Ewl_Text  text;                 /**< Inherit from Ewl_Text */

        Ewl_Widget *calendar;                /**< The calendar */
        Ewl_Widget *calendar_window;        /**< The window */

};

Ewl_Widget      *ewl_datepicker_new(void);
int              ewl_datepicker_init(Ewl_Datepicker* datepicker);

/*
 * Internally used callbacks, override at your risk
 */
void ewl_datepicker_cb_destroy(Ewl_Widget *w, void *ev, void *user_data);
void ewl_datepicker_cb_value_changed(Ewl_Widget *w, void *ev, void *user_data);
void ewl_datepicker_cb_window_mouse_down(Ewl_Widget *w, void *ev,
                                                        void *user_data);

/**
 * @}
 */

#endif
