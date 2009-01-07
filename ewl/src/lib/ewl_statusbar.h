/* vim: set sw=8 ts=8 sts=8 expandtab: */
#ifndef EWL_STATUSBAR_H
#define EWL_STATUSBAR_H

#include "ewl_box.h"

/**
 * @addtogroup Ewl_Statusbar Ewl_Statusbar: A status bar widget
 * Defines a class for displaying status bars
 *
 * @remarks Inherits from Ewl_Box.
 * @if HAVE_IMAGES
 * @image html Ewl_Statusbar_inheritance.png
 * @endif
 *
 * @{
 */

/**
 * @themekey /statusbar/file
 * @themekey /statusbar/group
 */

/**
 * @def EWL_STATUSBAR_TYPE
 * The type name for the Ewl_Statusbar widget
 */
#define EWL_STATUSBAR_TYPE "statusbar"

/**
 * @def EWL_STATUSBAR_IS(w)
 * Returns TRUE if the widget is an Ewl_Statusbar, FALSE otherwise
 */
#define EWL_STATUSBAR_IS(w) (ewl_widget_type_is(EWL_WIDGET(w), EWL_STATUSBAR_TYPE))

/**
 * The Ewl_Statusbar
 */
typedef struct Ewl_Statusbar Ewl_Statusbar;

/**
 * @def EWL_STATUSBAR(statusbar)
 * Typecasts a pointer to a Ewl_Statusbar pointer
 */
#define EWL_STATUSBAR(statusbar) ((Ewl_Statusbar *) statusbar)

/**
 * @brief Inherits from the Ewl_Box class and extends it to provide for
 * a simple status bar
 */
struct Ewl_Statusbar
{
        Ewl_Box                         outer_box;        /**< Inherit from Ewl_Box */
        Ewl_Widget                *left;                /**< The left container **/
        Ewl_Widget                *status;        /**< The status container */
        Ewl_Widget                *right;                /**< The right container */
        Ecore_List                *stack;                /**< The stack of status' */
        Ewl_Widget                *current;        /**< The currently displayed status */
};

Ewl_Widget      *ewl_statusbar_new(void);
int              ewl_statusbar_init(Ewl_Statusbar *sb);

void             ewl_statusbar_left_hide(Ewl_Statusbar *sb);
void             ewl_statusbar_left_show(Ewl_Statusbar *sb);
void             ewl_statusbar_right_hide(Ewl_Statusbar *sb);
void             ewl_statusbar_right_show(Ewl_Statusbar *sb);

void             ewl_statusbar_active_set(Ewl_Statusbar *sb, Ewl_Position pos);
void             ewl_statusbar_left_append(Ewl_Statusbar *sb, Ewl_Widget *w);
void             ewl_statusbar_left_prepend(Ewl_Statusbar *sb, Ewl_Widget *w);
void             ewl_statusbar_right_append(Ewl_Statusbar *sb, Ewl_Widget *w);
void             ewl_statusbar_right_prepend(Ewl_Statusbar *sb, Ewl_Widget *w);

void             ewl_statusbar_push(Ewl_Statusbar *sb, const char *txt);
void             ewl_statusbar_pop(Ewl_Statusbar *sb);

/*
 * Internal functions
 */
void ewl_statusbar_cb_destroy(Ewl_Widget *w, void *ev, void *data);

/**
 * @}
 */

#endif

