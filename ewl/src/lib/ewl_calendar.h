/* vim: set sw=8 ts=8 sts=8 expandtab: */
#ifndef EWL_CALENDAR_H
#define EWL_CALENDAR_H

#include "ewl_box.h"

/**
 * @addtogroup Ewl_Calendar Ewl_Calendar: The EWL Calendar Widget
 * @brief Defines the Ewl_Calendar class
 *
 * @remarks Inherits from Ewl_Box.
 * @if HAVE_IMAGES
 * @image html Ewl_Calendar_inheritance.png
 * @endif
 *
 * @{
 */

/**
 * @themekey /calendar/file
 * @themekey /calendar/group
 */

/**
 * @def EWL_CALENDAR_TYPE
 * The type name for the Ewl_Calendar widget
 */
#define EWL_CALENDAR_TYPE "calendar"

/**
 * @def EWL_CALENDAR_IS(w)
 * Returns TRUE if the widget is an Ewl_Calendar, FALSE otherwise
 */
#define EWL_CALENDAR_IS(w) (ewl_widget_type_is(EWL_WIDGET(w), EWL_CALENDAR_TYPE))

/**
 * The Ewl_Calendar structure
 */
typedef struct Ewl_Calendar Ewl_Calendar;

/**
 * @def EWL_CALENDAR(calendar)
 * Typecast a pointer to an Ewl_Calendar pointer
 */
#define EWL_CALENDAR(calendar) ((Ewl_Calendar *) calendar)

/**
 * @brief Inherits from an Ewl_Box to provide a calendar widget
 *
 * Layout is either free-form, or auto-arranged to a grid.
 */
struct Ewl_Calendar
{
        Ewl_Box box;                     /**< Inherit from Ewl_Box */

        int cur_day;                     /**< The current day selected */
        int cur_month;                   /**< The current month selected */
        int cur_year;                    /**< The current year selected */
        Ewl_Widget *grid;                /**< The layout grid */
        Ewl_Widget *month_label;         /**< The month label */
};

Ewl_Widget      *ewl_calendar_new(void);
int              ewl_calendar_init(Ewl_Calendar *calendar);
char            *ewl_calendar_ascii_time_get(Ewl_Calendar *cal);
int              ewl_calendar_day_get(Ewl_Calendar *c);
int              ewl_calendar_month_get(Ewl_Calendar *c);
int              ewl_calendar_year_get(Ewl_Calendar *c);

/**
 * @}
 */

#endif

