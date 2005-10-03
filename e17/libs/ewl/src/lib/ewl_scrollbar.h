#ifndef __EWL_SCROLLBAR_H__
#define __EWL_SCROLLBAR_H__

/**
 * @file ewl_scrollbar.h
 * @defgroup Ewl_Scrollbar Scrollbar: A Simple Scrollbar Widget
 * Defines an Ewl_Scrollbar for using when scrolling values are needed.
 *
 * @{
 */

/**
 * @themekey /hscrollbar/file
 * @themekey /hscrollbar/group
 * @themekey /vscrollbar/file
 * @themekey /vscrollbar/group
 *
 * @themekey /hscrollbar/seeker/file
 * @themekey /hscrollbar/seeker/group
 * @themekey /vscrollbar/seeker/file
 * @themekey /vscrollbar/seeker/group
 *
 * @themekey /hscrollbar/seeker/button/file
 * @themekey /hscrollbar/seeker/button/group
 * @themekey /vscrollbar/seeker/button/file
 * @themekey /vscrollbar/seeker/button/group
 */

/**
 * Provides a seeker with increment and decrement buttons arranged in a
 * specified order.
 */
typedef struct Ewl_Scrollbar Ewl_Scrollbar;

/**
 * @def EWL_SCROLLBAR(scrollbar)
 * Typecasts a pointer to an Ewl_Scrollbar pointer.
 */
#define EWL_SCROLLBAR(scrollbar) ((Ewl_Scrollbar *) scrollbar)

/**
 * @struct Ewl_Scrollbar
 * Inherits from Ewl_Box to layout an Ewl_Seeker and two Ewl_Buttons to
 * provide scrollbar functionality.
 */
struct Ewl_Scrollbar
{
	Ewl_Box         box; /**< Inherit from Ewl_Box */

	Ewl_Widget     *seeker; /**< The internal Ewl_Seeker */
	Ewl_Widget     *button_decrement; /**< The internal decrement button */
	Ewl_Widget     *button_increment; /**< The internal increment button */
	unsigned int    buttons_alignment; /**< The ordering of buttons */

	double          fill_percentage; /**< The ratio of size for draggable */
	double          start_time; /**< Time scrolling began */
	Ecore_Timer    *timer; /**< Repeating timer for scrolling */
	signed char     direction;
};

/**
 * @def ewl_hscrollbar_new()
 * A shortcut for allocating a new horizontal scrollbar.
 */
#define ewl_hscrollbar_new() ewl_scrollbar_new(EWL_ORIENTATION_HORIZONTAL)

/**
 * @def ewl_vscrollbar_new()
 * A shortcut for allocating a new vertical scrollbar.
 */
#define ewl_vscrollbar_new() ewl_scrollbar_new(EWL_ORIENTATION_VERTICAL)

Ewl_Widget     *ewl_scrollbar_new(Ewl_Orientation orientation);
int             ewl_scrollbar_init(Ewl_Scrollbar * s,
				   Ewl_Orientation orientation);

double          ewl_scrollbar_value_get(Ewl_Scrollbar * s);
void            ewl_scrollbar_value_set(Ewl_Scrollbar * s, double v);

double          ewl_scrollbar_step_get(Ewl_Scrollbar *s);
void            ewl_scrollbar_step_set(Ewl_Scrollbar * s, double v);

/*
 * Internally used callbacks, override at your own risk.
 */
void        ewl_scrollbar_scroll_start_cb(Ewl_Widget * w, void *ev_data,
					  void *user_data);
void        ewl_scrollbar_scroll_stop_cb(Ewl_Widget * w, void *ev_data,
					 void *user_data);

/**
 * @}
 */

#endif				/* __EWL_SCROLLBAR_H__ */
