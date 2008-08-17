/* vim: set sw=8 ts=8 sts=8 expandtab: */
#ifndef EWL_FLOATER_H
#define EWL_FLOATER_H

#include "ewl_box.h"

/**
 * @addtogroup Ewl_Floater Ewl_Floater: A Floating Container
 * @brief Defines a widget for layering above other widgets in EWL's drawing
 * area, with the ability to follow the movement of another widget.
 *
 * @remarks Inherits from Ewl_Box.
 * @if HAVE_IMAGES
 * @image html Ewl_Floater_inheritance.png
 * @endif
 *
 * @{
 */

/**
 * @themekey /floater/file
 * @themekey /floater/group
 */

/**
 * @def EWL_FLOATER_TYPE
 * The type name for the Ewl_Floater widget
 */
#define EWL_FLOATER_TYPE "floater"

/**
 * @def EWL_FLOATER_IS(w)
 * Returns TRUE if the widget is an Ewl_Floater, FALSE otherwise
 */
#define EWL_FLOATER_IS(w) (ewl_widget_type_is(EWL_WIDGET(w), EWL_FLOATER_TYPE))

/**
 * The floater is a container for widgets that need to be drawn on top
 * of multiple widgets (ie. dropdownbox)
 */
typedef struct Ewl_Floater Ewl_Floater;

/**
 * @def EWL_FLOATER(floater)
 * Typecasts a pointer to an Ewl_Floater pointer.
 */
#define EWL_FLOATER(floater) ((Ewl_Floater *) floater)

/**
 * @brief Inherits from Ewl_Box for layout purposes. Provides a means to layer above
 * other widgets and to follow the movement of another widget.
 */
struct Ewl_Floater
{
        Ewl_Box box; /**< Inherit from Ewl_Box */
        Ewl_Widget *follows; /**< The widget to position relative to */
        int x; /**< The x offset from follows x positon */
        int y; /**< The y offset from follows y positon */
};


Ewl_Widget      *ewl_floater_new(void);
int              ewl_floater_init(Ewl_Floater *f);
void             ewl_floater_follow_set(Ewl_Floater *f, Ewl_Widget *p);
Ewl_Widget      *ewl_floater_follow_get(Ewl_Floater *f);
void             ewl_floater_position_set(Ewl_Floater *parent, int x, int y);

/*
 * Internally used callbacks, override at your own risk.
 */
void ewl_floater_cb_destroy(Ewl_Widget *w, void *ev_data, void *user_data);
void ewl_floater_cb_follow_configure(Ewl_Widget *w, void *ev_data,
                                                void *user_data);
void ewl_floater_cb_follow_destroy(Ewl_Widget *w, void *ev_data,
                                                   void *user_data);

/**
 * @}
 */

#endif
