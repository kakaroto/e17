#ifndef __EWL_SPINNER_H__
#define __EWL_SPINNER_H__

/**
 * @defgroup Ewl_Spinner A Numerical Value Entry
 * Provides a field for entering numerical values, along with buttons to
 * increment and decrement the value.
 *
 * @{
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

Ewl_Widget     *ewl_spinner_new();
void            ewl_spinner_init(Ewl_Spinner * s);
void            ewl_spinner_set_value(Ewl_Spinner * s, double value);
double          ewl_spinner_get_value(Ewl_Spinner * s);
void            ewl_spinner_set_digits(Ewl_Spinner * s, int digits);
void            ewl_spinner_set_min_val(Ewl_Spinner * s, double val);
void            ewl_spinner_set_max_val(Ewl_Spinner * s, double val);
void            ewl_spinner_set_step(Ewl_Spinner * s, double step);

/**
 * @}
 */

#endif				/* __EWL_SPINNER_H__ */
