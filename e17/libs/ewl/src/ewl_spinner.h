#ifndef __EWL_SPINNER_H__
#define __EWL_SPINNER_H__

/**
 * @defgroup Ewl_Spinner Spinner: A Numerical Value Entry
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
 * Inherits from Ewl_Container and adds an entry box that can only contain
 * numerical values as well as buttons for manipulating that value.
 */
struct Ewl_Spinner
{
	Ewl_Container   container; /**< Inherit from Ewl_Container */
	double          min_val; /**< Minimum numerical value displayed */
	double          max_val; /**< Maximum numerical value displayed */
	double          value; /**< Current value displayed */
	double          step; /**< Amount to add or subtract at a time */
	int             digits; /**< Number of digits displayed after decimal */
	Ewl_Widget     *entry; /**< The Ewl_Entry displaying value */
	Ewl_Widget     *button_increase; /**< Ewl_Button to add value */
	Ewl_Widget     *button_decrease; /**< Ewl_Button to subtract value */
};

Ewl_Widget     *ewl_spinner_new(void);
void            ewl_spinner_init(Ewl_Spinner * s);
void            ewl_spinner_set_value(Ewl_Spinner * s, double value);
double          ewl_spinner_get_value(Ewl_Spinner * s);
void            ewl_spinner_set_digits(Ewl_Spinner * s, int digits);
void            ewl_spinner_set_min_val(Ewl_Spinner * s, double val);
void            ewl_spinner_set_max_val(Ewl_Spinner * s, double val);
void            ewl_spinner_set_step(Ewl_Spinner * s, double step);

/*
 * Internally used callbacks, override at your own risk.
 */
void ewl_spinner_child_add_cb(Ewl_Container *c, Ewl_Widget *w);
void ewl_spinner_child_resize_cb(Ewl_Container *c, Ewl_Widget *w,
				 int size, Ewl_Orientation o);
void ewl_spinner_realize_cb(Ewl_Widget * widget, void *ev_data,
			    void *user_data);
void ewl_spinner_configure_cb(Ewl_Widget * widget, void *ev_data,
			      void *user_data);
void ewl_spinner_key_down_cb(Ewl_Widget * widget, void *ev_data,
			     void *user_data);
void ewl_spinner_deselect_cb(Ewl_Widget * w, void *ev_data,
			     void *user_data);

void ewl_spinner_increase_value_cb(Ewl_Widget * widget, void *ev_data,
				   void *user_data);
void ewl_spinner_decrease_value_cb(Ewl_Widget * widget, void *ev_data,
				   void *user_data);

/**
 * @}
 */

#endif				/* __EWL_SPINNER_H__ */
