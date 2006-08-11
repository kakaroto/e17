#ifndef EWL_SPINNER_H
#define EWL_SPINNER_H

/**
 * @addtogroup Ewl_Spinner Ewl_Spinner: A Numerical Value Entry
 * Provides a field for entering numerical values, along with buttons to
 * increment and decrement the value.
 *
 * @{
 */

/**
 * @themekey /spinner/file
 * @themekey /spinner/group
 */

/**
 * @def EWL_SPINNER_TYPE
 * The type name for the Ewl_Spinner widget
 */
#define EWL_SPINNER_TYPE "spinner"

/**
 * A combination of entry and increment/decrement buttons for adjusting
 * numerical values.
 */
typedef struct Ewl_Spinner Ewl_Spinner;

/**
 * @def EWL_SPINNER(spinner)
 * Typecasts a pointer to an Ewl_Spinner pointer.
 */
#define EWL_SPINNER(spinner) ((Ewl_Spinner *) spinner)

/**
 * Inherits from Ewl_Range and adds an entry box that can only contain
 * numerical values as well as buttons for manipulating that value.
 */
struct Ewl_Spinner
{
	Ewl_Range   	range; /**< Inherit from Ewl_Range */
	int             digits; /**< Number of digits displayed after decimal */
	Ewl_Widget     *entry; /**< The Ewl_Entry displaying value */
	Ewl_Widget     *vbox;  /**< Ewl_Box to hold the buttons */
	Ewl_Widget     *increment; /**< Ewl_Button to add value */
	Ewl_Widget     *decrement; /**< Ewl_Button to subtract value */
	double          start_time; /**< Time the spinner was pressed */
	int             direction; /**< Indicate increasing/decreasing value */
	Ecore_Timer    *timer; /**< Timer for tracking mouse button held down */
};

Ewl_Widget     *ewl_spinner_new(void);
int             ewl_spinner_init(Ewl_Spinner *s);
void            ewl_spinner_digits_set(Ewl_Spinner *s, unsigned char digits);

/*
 * Internally used callbacks, override at your own risk.
 */
void ewl_spinner_realize_cb(Ewl_Widget *widget, void *ev_data,
						void *user_data);
void ewl_spinner_value_changed_cb(Ewl_Widget *widget, void *ev_data,
						void *user_data);
void ewl_spinner_configure_cb(Ewl_Widget *widget, void *ev_data,
						void *user_data);
void ewl_spinner_key_down_cb(Ewl_Widget *widget, void *ev_data,
						void *user_data);
void ewl_spinner_focus_out_cb(Ewl_Widget *w, void *ev_data,
						void *user_data);
void ewl_spinner_wheel_cb(Ewl_Widget *w, void *ev_data, 
						void *user_data);

void ewl_spinner_increase_value_cb(Ewl_Widget *widget, void *ev_data,
						void *user_data);
void ewl_spinner_decrease_value_cb(Ewl_Widget *widget, void *ev_data,
						void *user_data);
void ewl_spinner_value_stop_cb(Ewl_Widget *w, void *ev_data, void *user_data);
void ewl_spinner_destroy_cb(Ewl_Widget *w, void *ev_data, void *user_data);
void ewl_spinner_child_show_cb(Ewl_Container *c, Ewl_Widget *w);
void ewl_spinner_child_resize_cb(Ewl_Container *c, Ewl_Widget *w, int size,
					Ewl_Orientation o);
/**
 * @}
 */

#endif

