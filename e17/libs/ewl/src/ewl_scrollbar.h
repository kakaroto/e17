#ifndef __EWL_SCROLLBAR_H__
#define __EWL_SCROLLBAR_H__

/**
 * @defgroup Ewl_Scrollbar A Simple Scrollbar Widget
 * Defines an Ewl_Scrollbar for using when scrolling values are needed.
 *
 * @{
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
	Ewl_Alignment   buttons_alignment; /**< The ordering of buttons */

	double          fill_percentage; /**< The ratio of size for draggable */
	Ewl_ScrollBar_Flags flag; /**< Flags to indicate part visibility */
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
void            ewl_scrollbar_init(Ewl_Scrollbar * s,
				   Ewl_Orientation orientation);

double          ewl_scrollbar_get_value(Ewl_Scrollbar * s);
void            ewl_scrollbar_set_value(Ewl_Scrollbar * s, double v);

double          ewl_scrollbar_get_step(Ewl_Scrollbar *s);
void            ewl_scrollbar_set_step(Ewl_Scrollbar * s, double v);

void            ewl_scrollbar_set_flag(Ewl_Scrollbar * s,
				       Ewl_ScrollBar_Flags f);
Ewl_ScrollBar_Flags ewl_scrollbar_get_flag(Ewl_Scrollbar * s);

/**
 * @}
 */

#endif				/* __EWL_SCROLLBAR_H__ */
