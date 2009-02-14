/* vim: set sw=8 ts=8 sts=8 expandtab: */
#ifndef EWL_SCROLLPANE_H
#define EWL_SCROLLPANE_H

#include "ewl_scrollport.h"

/**
 * @addtogroup Ewl_Scrollpane Ewl_Scrollpane: A Scrollable Viewing Area
 * Provides a scrollable area for viewing large sets of widgets in a smaller
 * viewable region.
 *
 * @remarks Inherits from Ewl_Container.
 * @if HAVE_IMAGES
 * @image html Ewl_Scrollpane_inheritance.png
 * @endif
 *
 * @{
 */

/**
 * @themekey /scrollpane/file
 * @themekey /scrollpane/group
 */

/**
 * @def EWL_SCROLLPANE_TYPE
 * The type name for the Ewl_Scrollpane widget
 */
#define EWL_SCROLLPANE_TYPE "scrollpane"

/**
 * @def EWL_SCROLLPANE_IS(w)
 * Returns TRUE if the widget is an Ewl_Scrollpane, FALSE otherwise
 */
#define EWL_SCROLLPANE_IS(w) (ewl_widget_type_is(EWL_WIDGET(w), EWL_SCROLLPANE_TYPE))


/**
 * The scrollpane provides a way to pan around large collections of images.
 */
typedef struct Ewl_Scrollpane Ewl_Scrollpane;

/**
 * @def EWL_SCROLLPANE(scrollpane)
 * Typecasts a pointer to an Ewl_Scrollpane pointer.
 */
#define EWL_SCROLLPANE(scrollpane) ((Ewl_Scrollpane *) scrollpane)

/**
 * @brief Inherits from Ewl_Container and extends it to enclose a single widget in
 * vertical and horizontal scrollbars for panning around displaying different
 * regions of the enclosed widget.
 */
struct Ewl_Scrollpane
{
        Ewl_Scrollport scrollport;        /**< Inherit from Ewl_Scrollport */

        Ewl_Widget *box;                /**< Lays out enclosed widget */
};

Ewl_Widget              *ewl_scrollpane_new(void);
int                      ewl_scrollpane_init(Ewl_Scrollpane *s);

/*
 * Internally used callbacks, override at your own risk.
 */
void ewl_scrollpane_cb_configure(Ewl_Widget *w, void *ev_data,
                                                void *user_data);
void ewl_scrollpane_cb_focus_jump(Ewl_Widget *w, void *ev_data,
                                                 void *user_data);

void ewl_scrollpane_cb_container_show(Ewl_Container *p, Ewl_Widget *c);
void ewl_scrollpane_cb_container_resize(Ewl_Container *p, Ewl_Widget *c,
                                                int size, Ewl_Orientation o);

/**
 * @}
 */

#endif
