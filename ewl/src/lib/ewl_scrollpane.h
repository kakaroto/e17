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
 * The scrollpane can be scolled with a drag of the mouse
 */
typedef struct Ewl_Scrollpane_Scroll_Info_Base Ewl_Scrollpane_Scroll_Info_Base;

/**
 * @def EWL_SCROLLPANE_SCROLL_INFO_BASE(scroll_info)
 * Typecasts a pointer to an Ewl_Scrollpane_Scroll_Info_Base pointer.
 */
#define EWL_SCROLLPANE_SCROLL_INFO_BASE(scroll_info) ((Ewl_Scrollpane_Scroll_Info_Base *) scroll_info)

/**
 * @brief Enables a scrollpane to be scrolled with kinetic behaviour
 */
struct Ewl_Scrollpane_Scroll_Info_Base
{
        unsigned char clicked:1;        /**< If the mouse is currently clicked
                                                or not */
        unsigned char active:1;         /**< If the pane is currently moving */
        int fps;                        /**< Number of recalculations per
                                                second */
        double vmax;                    /**< Maximum speed in pixels */
        double vmin;                    /**< Minimum speed in pixels */
        double dampen;                  /**< Frictional variable */
        void *extra;                    /**< Additional information */
};

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
        Ewl_Scrollpane_Scroll_Info_Base *kinfo;         /**< Kinetic scrolling 
                                                             info */
        Ewl_Kinetic_Scroll type;        /**< If the scrollpane is to use 
                                                kinetic scrolling */
};

Ewl_Widget              *ewl_scrollpane_new(void);
int                      ewl_scrollpane_init(Ewl_Scrollpane *s);
void                     ewl_scrollpane_kinetic_scrolling_set(Ewl_Scrollpane *s,
                                                Ewl_Kinetic_Scroll type);
Ewl_Kinetic_Scroll       ewl_scrollpane_kinetic_scrolling_get(Ewl_Scrollpane *s);
void                     ewl_scrollpane_kinetic_max_velocity_set(Ewl_Scrollpane *s,
                                                double v);
double                   ewl_scrollpane_kinetic_max_velocity_get(Ewl_Scrollpane *s);
void                     ewl_scrollpane_kinetic_min_velocity_set(Ewl_Scrollpane *s,
                                                double v);
double                   ewl_scrollpane_kinetic_min_velocity_get(Ewl_Scrollpane *s);
void                     ewl_scrollpane_kinetic_dampen_set(Ewl_Scrollpane *s,
                                                double d);
double                   ewl_scrollpane_kinetic_dampen_get(Ewl_Scrollpane *s);
void                     ewl_scrollpane_kinetic_fps_set(Ewl_Scrollpane *s,
                                                int fps); 
int                      ewl_scrollpane_kinetic_fps_get(Ewl_Scrollpane *s);

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
