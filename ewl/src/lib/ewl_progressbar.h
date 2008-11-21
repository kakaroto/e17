/* vim: set sw=8 ts=8 sts=8 expandtab: */
#ifndef EWL_PROGRESSBAR_H
#define EWL_PROGRESSBAR_H

#include "ewl_range.h"

/**
 * @addtogroup Ewl_Progressbar Ewl_Progressbar: A progressbar from a value
 * A progress bar widget
 *
 * @remarks Inherits from Ewl_Range.
 * @if HAVE_IMAGES
 * @image html Ewl_Progressbar_inheritance.png
 * @endif
 *
 * @{
 */

/**
 * @themekey /progressbar/file
 * @themekey /progressbar/group
 * @themekey /bar/file
 * @themekey /bar/group
 */

/**
 * @def EWL_PROGRESSBAR_TYPE
 * The type name for the Ewl_Progressbar widget
 */
#define EWL_PROGRESSBAR_TYPE "progressbar"

/**
 * @def EWL_PROGRESSBAR_IS(w)
 * Returns TRUE if the widget is an Ewl_Progressbar, FALSE otherwise
 */
#define EWL_PROGRESSBAR_IS(w) (ewl_widget_type_is(EWL_WIDGET(w), EWL_PROGRESSBAR_TYPE))

/**
 * The Ewl_Progressbar provides a statusbar from a given value
 */
typedef struct Ewl_Progressbar Ewl_Progressbar;

/**
 * @def EWL_PROGRESSBAR(seeker)
 * Typecasts a pointer to an Ewl_Progressbar pointer.
 */
#define EWL_PROGRESSBAR(progressbar) ((Ewl_Progressbar *) progressbar)

/**
 * @brief Inherits from Ewl_Range and extends to provide a progressbar
 * widget
 */
struct Ewl_Progressbar
{
        Ewl_Range range;     /**< Inherit from Ewl_Range */
        Ewl_Widget *bar;       /**< The moving bar on top */
        Ewl_Widget *label;     /**< text label on the bar */
        int auto_label;         /**< flag if user is setting label or not */
};


Ewl_Widget      *ewl_progressbar_new(void);
int              ewl_progressbar_init(Ewl_Progressbar *p);

void             ewl_progressbar_label_set(Ewl_Progressbar *p,
                                        const char *label);
void             ewl_progressbar_custom_label_set(Ewl_Progressbar *p,
                                        const char *format_string);

void             ewl_progressbar_label_show(Ewl_Progressbar *p);
void             ewl_progressbar_label_hide(Ewl_Progressbar *p);

/*
 * Internally used callbacks, override at your own risk.
 */
void ewl_progressbar_cb_configure(Ewl_Widget *w, void *ev_data,
                                  void *user_data);
void ewl_progressbar_cb_value_changed(Ewl_Widget *w, void *ev_data,
                                  void *user_data);
void ewl_progressbar_cb_child_show(Ewl_Container *c, Ewl_Widget *w);
void ewl_progressbar_cb_child_resize(Ewl_Container *c, Ewl_Widget *w, int size,
                                     Ewl_Orientation o);

/**
 * @}
 */

#endif
