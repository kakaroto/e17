/* vim: set sw=8 ts=8 sts=8 expandtab: */
#ifndef EWL_SEEKER_H
#define EWL_SEEKER_H

#include "ewl_range.h"

/**
 * @addtogroup Ewl_Seeker Ewl_Seeker: A Value Selector from a Range
 * Defines an Ewl_Widget with a draggable button enclosed, used to select a
 * value from a range.
 *
 * @remarks Inherits from Ewl_Range.
 * @if HAVE_IMAGES
 * @image html Ewl_Seeker_inheritance.png
 * @endif
 *
 * @{
 */

/**
 * @themekey /hseeker/file
 * @themekey /hseeker/group
 * @themekey /vseeker/file
 * @themekey /vseeker/group
 */

/**
 * @def EWL_SEEKER_TYPE
 * The type name for the Ewl_Seeker widget
 */
#define EWL_SEEKER_TYPE "seeker"

/**
 * @def EWL_SEEKER_IS(w)
 * Returns TRUE if the widget is an Ewl_Seeker, FALSE otherwise
 */
#define EWL_SEEKER_IS(w) (ewl_widget_type_is(EWL_WIDGET(w), EWL_SEEKER_TYPE))

/**
 * The Ewl_Seeker provides a means to select a value from a range using a
 * draggable button.
 */
typedef struct Ewl_Seeker Ewl_Seeker;

/**
 * @def EWL_SEEKER(seeker)
 * Typecasts a pointer to an Ewl_Seeker pointer.
 */
#define EWL_SEEKER(seeker) ((Ewl_Seeker *) seeker)

/**
 * @brief Inherits from Ewl_Range and extends to provide a seeker widget
 */
struct Ewl_Seeker
{
        Ewl_Range range; /**< Inherit from Ewl_Range */
        Ewl_Orientation orientation; /**< Indicates layout direction */
        Ewl_Widget *button; /**< Draggable widget for selecting value */
        int dragstart; /**< The coordinate where the drag starts */
        int autohide; /**< Indicator to hide when not scrollable */
        Ecore_Timer *timer; /**< Timer for scroll repeating */
        double start_time; /**< Time the timer was started */
};

Ewl_Widget      *ewl_seeker_new(void);
Ewl_Widget      *ewl_hseeker_new(void);
Ewl_Widget      *ewl_vseeker_new(void);
int              ewl_seeker_init(Ewl_Seeker *s);

void             ewl_seeker_orientation_set(Ewl_Seeker *s, Ewl_Orientation o);
Ewl_Orientation  ewl_seeker_orientation_get(Ewl_Seeker *s);

void             ewl_seeker_autohide_set(Ewl_Seeker *s, int v);
int              ewl_seeker_autohide_get(Ewl_Seeker *s);

/*
 * Internally used callbacks, override at your own risk.
 */
void ewl_seeker_cb_configure(Ewl_Widget *w, void *ev_data, void *user_data);
void ewl_seeker_cb_button_mouse_down(Ewl_Widget *w, void *ev_data,
                                                void *user_data);
void ewl_seeker_cb_button_mouse_up(Ewl_Widget *w, void *ev_data,
                                              void *user_data);
void ewl_seeker_cb_mouse_move(Ewl_Widget *w, void *ev_data, void *user_data);
void ewl_seeker_cb_mouse_down(Ewl_Widget *w, void *ev_data, void *user_data);
void ewl_seeker_cb_mouse_up(Ewl_Widget *w, void *ev_data, void *user_data);
void ewl_seeker_cb_key_down(Ewl_Widget *w, void *ev_data, void *user_data);
void ewl_seeker_cb_child_show(Ewl_Container *p, Ewl_Widget * w);

/**
 * @}
 */

#endif
