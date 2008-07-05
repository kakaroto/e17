/* vim: set sw=8 ts=8 sts=8 expandtab: */
#ifndef EWL_SPINNER_H
#define EWL_SPINNER_H

#include "ewl_range.h"

/**
 * @addtogroup Ewl_Spinner Ewl_Spinner: A Numerical Value Entry
 * Provides a field for entering numerical values, along with buttons to
 * increment and decrement the value.
 *
 * @remarks Inherits from Ewl_Range.
 * @if HAVE_IMAGES
 * @image html Ewl_Spinner_inheritance.png
 * @endif
 *
 * @{
 */

/**
 * @themekey /spinner/file
 * @themekey /spinner/group
 */

/**
 * @def EWL_SPINNER_TYPE
 * The type name for the Ewl_Spinner widget
 */
#define EWL_SPINNER_TYPE "spinner"

/**
 * @def EWL_SPINNER_IS(w)
 * Returns TRUE if the widget is an Ewl_Spinner, FALSE otherwise
 */
#define EWL_SPINNER_IS(w) (ewl_widget_type_is(EWL_WIDGET(w), EWL_SPINNER_TYPE))

/**
 * A combination of entry and increment/decrement buttons for adjusting
 * numerical values.
 */
typedef struct Ewl_Spinner Ewl_Spinner;

/**
 * @def EWL_SPINNER(spinner)
 * Typecasts a pointer to an Ewl_Spinner pointer.
 */
#define EWL_SPINNER(spinner) ((Ewl_Spinner *) spinner)

/**
 * @brief Inherits from Ewl_Range and extends to provide a spinner widget
 */
struct Ewl_Spinner
{
        Ewl_Range range;         /**< Inherit from Ewl_Range */
        unsigned char digits;         /**< Number of digits displayed after decimal */
        Ewl_Widget *entry;         /**< The Ewl_Entry displaying value */
        Ewl_Widget *vbox;          /**< Ewl_Box to hold the buttons */
        Ewl_Widget *increment;         /**< Ewl_Button to add value */
        Ewl_Widget *decrement;         /**< Ewl_Button to subtract value */
        double start_time;         /**< Time the spinner was pressed */
        double last_value;         /**< The last value while spinning */
        int direction;                 /**< Indicate increasing/decreasing value */
        Ecore_Timer *timer;         /**< Timer for tracking mouse button held down */
};

Ewl_Widget      *ewl_spinner_new(void);
int              ewl_spinner_init(Ewl_Spinner *s);

void             ewl_spinner_digits_set(Ewl_Spinner *s, unsigned char digits);
unsigned char    ewl_spinner_digits_get(Ewl_Spinner *s);

/*
 * Internally used callbacks, override at your own risk.
 */
void ewl_spinner_cb_realize(Ewl_Widget *widget, void *ev_data,
                                                void *user_data);
void ewl_spinner_cb_value_changed(Ewl_Widget *widget, void *ev_data,
                                                void *user_data);
void ewl_spinner_cb_configure(Ewl_Widget *widget, void *ev_data,
                                                void *user_data);
void ewl_spinner_cb_key_down(Ewl_Widget *widget, void *ev_data,
                                                void *user_data);
void ewl_spinner_cb_focus_out(Ewl_Widget *w, void *ev_data,
                                                void *user_data);
void ewl_spinner_cb_wheel(Ewl_Widget *w, void *ev_data,
                                                void *user_data);

void ewl_spinner_cb_increase_value(Ewl_Widget *widget, void *ev_data,
                                                void *user_data);
void ewl_spinner_cb_decrease_value(Ewl_Widget *widget, void *ev_data,
                                                void *user_data);
void ewl_spinner_cb_value_stop(Ewl_Widget *w, void *ev_data, void *user_data);
void ewl_spinner_cb_destroy(Ewl_Widget *w, void *ev_data, void *user_data);
void ewl_spinner_cb_child_show(Ewl_Container *c, Ewl_Widget *w);
void ewl_spinner_cb_child_resize(Ewl_Container *c, Ewl_Widget *w, int size,
                                        Ewl_Orientation o);
/**
 * @}
 */

#endif

