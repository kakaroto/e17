#ifndef __EWL_CALENDAR_H__
#define __EWL_CALENDAR_H__



/**
 * @file ewl_calendar.h
 * @defgroup Ewl_Calendar Calendar: The EWL Calendar Widget
 * @brief Defines the Ewl_Calendar class  
 * Defined the Ewl_Calendar class, allowing selection of a day in time
 *
 */

/** 
 * @themekey /calendar/file
 * @themekey /calendar/group
 */


typedef struct Ewl_Calendar Ewl_Calendar;

/**
 * @def EWL_CALENDAR(calendar)
 * Typecast a pointer to an Ewl_Calendar pointer
 */
#define EWL_CALENDAR(calendar) ((Ewl_Calendar *) calendar)

/** 
 * @struct Ewl_Calendar
 * Inherits from an Ewl_Container to provide layout facilities for child widgets placed inside
 * Layout is either free-form, or auto-arranged to a grid.
 */
struct Ewl_Calendar
{
	Ewl_Box	box; /**< Inherit from Ewl_Container */

	int cur_day;
	int cur_month;
	int cur_year;
	Ewl_Widget* grid;
	Ewl_Widget* month_label;

};


Ewl_Widget	*ewl_calendar_new(void);
int		ewl_calendar_init(Ewl_Calendar* calendar);
void ewl_calendar_grid_setup(Ewl_Calendar* cal);
void ewl_calendar_ascii_time_get(Ewl_Calendar* cal, char* str);
int ewl_calendar_day_get(Ewl_Calendar*);
int ewl_calendar_month_get(Ewl_Calendar*);
int ewl_calendar_year_get(Ewl_Calendar*);


#endif
