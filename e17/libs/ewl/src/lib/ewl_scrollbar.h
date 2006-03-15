#ifndef EWL_SCROLLBAR_H
#define EWL_SCROLLBAR_H

/**
 * @addtogroup Ewl_Scrollbar Ewl_Scrollbar: A Simple Scrollbar Widget
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
 * @def EWL_SCROLLBAR_TYPE
 * The type name for the Ewl_Scrollbar widget
 */
#define EWL_SCROLLBAR_TYPE "scrollbar"

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
 * Inherits from Ewl_Box to layout an Ewl_Seeker and two Ewl_Buttons to
 * provide scrollbar functionality.
 */
struct Ewl_Scrollbar
{
	Ewl_Box         box;               /**< Inherit from Ewl_Box */

	Ewl_Widget     *seeker;            /**< The internal Ewl_Seeker */
	Ewl_Widget     *decrement;         /**< The internal decrement button */
	Ewl_Widget     *increment;         /**< The internal increment button */
	unsigned int    buttons_alignment; /**< The ordering of buttons */

	double          fill_percentage;   /**< Ratio of size for draggable */
	double          start_time;        /**< Time scrolling began */
	Ecore_Timer    *timer;             /**< Repeating timer for scrolling */
	signed char     direction;         /**< Direction for the scrollbar */
	signed char     invert;            /**< Invert the scrolling direction */
};

Ewl_Widget     *ewl_scrollbar_new(void);
Ewl_Widget     *ewl_hscrollbar_new(void);
Ewl_Widget     *ewl_vscrollbar_new(void);
int             ewl_scrollbar_init(Ewl_Scrollbar *s);

void            ewl_scrollbar_orientation_set(Ewl_Scrollbar *s,
					      Ewl_Orientation orientation);
Ewl_Orientation ewl_scrollbar_orientation_get(Ewl_Scrollbar *s);
char            ewl_scrollbar_inverse_scroll_get(Ewl_Scrollbar *s);
void            ewl_scrollbar_inverse_scroll_set(Ewl_Scrollbar *s, char v);
double          ewl_scrollbar_value_get(Ewl_Scrollbar *s);
void            ewl_scrollbar_value_set(Ewl_Scrollbar *s, double v);

double          ewl_scrollbar_step_get(Ewl_Scrollbar *s);
void            ewl_scrollbar_step_set(Ewl_Scrollbar *s, double v);

/*
 * Internally used callbacks, override at your own risk.
 */
void ewl_scrollbar_scroll_start_cb(Ewl_Widget *w, void *ev_data,
					  void *user_data);
void ewl_scrollbar_scroll_stop_cb(Ewl_Widget *w, void *ev_data,
					 void *user_data);

/**
 * @}
 */

#endif
