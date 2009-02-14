/* vim: set sw=8 ts=8 sts=8 expandtab: */
#ifndef EWL_SCROLLPORT_KINETIC_H
#define EWL_SCROLLPORT_KINETIC_H

#include "ewl_scrollport.h"

/**
 * @addtogroup Ewl_Scrollport Ewl_Scrollport_Kinetic: Functions to add kinetic
 * scrolling support to the scrollport
 *
 * @{
 */

/**
 * @def EWL_SCROLLPORT_SCROLL_INFO(scroll_info)
 * Typecasts a pointer to an Ewl_Scrollport_Kinetic_Info_Base pointer.
 */
#define EWL_SCROLLPORT_KINETIC_INFO(scroll_info) ((Ewl_Scrollport_Scroll_Info_Base *) scroll_info)

/**
 * @brief Enables a scrollport to be scrolled with kinetic behaviour
 */
struct Ewl_Scrollport_Kinetic_Info
{
        unsigned char clicked:1;        /**< If the mouse is currently clicked
                                                or not */
        unsigned char active:1;         /**< If the port is currently moving */
        int fps;                        /**< Number of recalculations per
                                                second */
        double vmax;                    /**< Maximum speed in pixels */
        double vmin;                    /**< Minimum speed in pixels */
        double dampen;                  /**< Frictional variable */
        void *extra;                    /**< Additional information */
};

void                     ewl_scrollport_kinetic_scrolling_set(Ewl_Scrollport *s,
                                                Ewl_Kinetic_Scroll type);
Ewl_Kinetic_Scroll       ewl_scrollport_kinetic_scrolling_get(Ewl_Scrollport *s);
void                     ewl_scrollport_kinetic_max_velocity_set(Ewl_Scrollport *s,
                                                double v);
double                   ewl_scrollport_kinetic_max_velocity_get(Ewl_Scrollport *s);
void                     ewl_scrollport_kinetic_min_velocity_set(Ewl_Scrollport *s,
                                                double v);
double                   ewl_scrollport_kinetic_min_velocity_get(Ewl_Scrollport *s);
void                     ewl_scrollport_kinetic_dampen_set(Ewl_Scrollport *s,
                                                double d);
double                   ewl_scrollport_kinetic_dampen_get(Ewl_Scrollport *s);
void                     ewl_scrollport_kinetic_fps_set(Ewl_Scrollport *s,
                                                int fps); 
int                      ewl_scrollport_kinetic_fps_get(Ewl_Scrollport *s);

/**
 * @}
 */

#endif
