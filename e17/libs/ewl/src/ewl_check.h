#ifndef _EWL_CHECK_H_
#define _EWL_CHECK_H_

/**
 * @defgroup Ewl_Check The Simple Check for a Checkbutton
 * Defines a widget with single purpose of providing a button with a checked
 * state, without any extra decorating.
 */

/**
 * @themekey /check/file
 * @themekey /check/group
 */

/**
 * A single purpose extension of Ewl_Widget to provide a checked state.
 */
typedef struct Ewl_Check Ewl_Check;

/**
 * @def EWL_CHECK(c)
 * Typecasts a pointer to an Ewl_Check pointer.
 *
 * @{
 */
#define EWL_CHECK(c) ((Ewl_Check *)c)

/**
 * @struct Ewl_Check
 * Inherits from Ewl_Check and extends it to provide a checked state.
 */
struct Ewl_Check
{
	Ewl_Widget      w; /**< Inherit from Ewl_Widget */
	int             checked; /**< Indicates if this is checked */
};

Ewl_Widget     *ewl_check_new();
void            ewl_check_init(Ewl_Check * c);
int             ewl_check_is_checked(Ewl_Check * c);
void            ewl_check_set_checked(Ewl_Check * c, int checked);

#endif
