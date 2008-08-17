/* vim: set sw=8 ts=8 sts=8 expandtab: */
#ifndef EWL_OVERLAY_H
#define EWL_OVERLAY_H

/**
 * @addtogroup Ewl_Overlay Ewl_Overlay: A Container for Displaying on a canvas
 * Defines the Ewl_Overlay class to provide EWL with the ability to work with an
 * evas.
 *
 * @remarks Inherits from Ewl_Container.
 * @if HAVE_IMAGES
 * @image html Ewl_Overlay_inheritance.png
 * @endif
 *
 * @{
 */

/**
 * @themekey /overlay/file
 * @themekey /overlay/group
 */

/**
 * @def EWL_OVERLAY_TYPE
 * The type name for the Ewl_Overlay widget
 */
#define EWL_OVERLAY_TYPE "overlay"

/**
 * @def EWL_OVERLAY_IS(w)
 * Returns TRUE if the widget is an Ewl_Overlay, FALSE otherwise
 */
#define EWL_OVERLAY_IS(w) (ewl_widget_type_is(EWL_WIDGET(w), EWL_OVERLAY_TYPE))

/**
 * The overlay structure is mostly a container for holding widgets and a
 * wrapper evas smart object.
 */
typedef struct Ewl_Overlay Ewl_Overlay;

/**
 * @def EWL_OVERLAY(widget)
 * @brief Typecast a pointer to an Ewl_Overlay pointer.
 */
#define EWL_OVERLAY(widget) ((Ewl_Overlay *) widget)

/**
 * @brief The class inheriting from Ewl_Container that acts as a top level
 * widget for interacting with the evas.
 */
struct Ewl_Overlay
{
        Ewl_Container   container; /**< Inherits from the Ewl_Container class */
};

Ewl_Widget      *ewl_overlay_new(void);
int              ewl_overlay_init(Ewl_Overlay *win);

/*
 * Internally used callbacks, override at your own risk.
 */
void ewl_overlay_cb_configure(Ewl_Widget *w, void *ev_data, void *user_data);

void ewl_overlay_cb_child_show(Ewl_Container *emb, Ewl_Widget *child);
void ewl_overlay_cb_child_resize(Ewl_Container *c, Ewl_Widget *w, int size,
                               Ewl_Orientation o);

/**
 * @}
 */

#endif
