
#ifndef __EWL_CHECKBUTTON_H__
#define __EWL_CHECKBUTTON_H__

/**
 * @defgroup Ewl_CheckButton CheckButton: A CheckButton with Label
 * @brief Defines an Ewl_CheckButton that inherits from Ewl_Widget and
 * provides an Ewl_Check that changes value on each click.
 *
 * @{
 */

/**
 * @themekey /checkbutton/file
 * @themekey /checkbutton/group
 */

/**
 * The button class is a basic button with a label. This class is inherited by
 * the check button and radio button classes.
 */
typedef struct Ewl_CheckButton Ewl_CheckButton;

/**
 * @def EWL_CHECKBUTTON(button)
 * Typecasts a pointer to an Ewl_CheckButton pointer.
 */
#define EWL_CHECKBUTTON(button) ((Ewl_CheckButton *) button)

/**
 * @struct Ewl_CheckButton
 * Inherits from Ewl_Widget and expands to provide a stateful check button.
 */
struct Ewl_CheckButton
{
	Ewl_Button      button; /**< Inherit the basic button properties */

	
	Ewl_Position    label_position; /**< Order of label and check */

	Ewl_Widget     *check; /**< Check widget represented */
};

Ewl_Widget     *ewl_checkbutton_new(char *l);
void            ewl_checkbutton_init(Ewl_CheckButton * cb, char *label);

/**
 * @def ewl_checkbutton_set_checked(cb, c);
 * Shortcut for dereferencing the checkbutton to update the checked state of
 * the Ewl_Check.
 */
#define ewl_checkbutton_set_checked(cb, c) \
	ewl_check_set_checked(EWL_CHECK(cb->check), c)

/**
 * @def ewl_checkbutton_is_checked(cb);
 * Shortcut for dereferencing the checkbutton to test the checked state of
 * the Ewl_Check.
 */
#define ewl_checkbutton_is_checked(cb) \
	ewl_check_is_checked(EWL_CHECK(cb->check))

void            ewl_checkbutton_set_label_position(Ewl_Widget * w,
						   Ewl_Position p);

void            ewl_checkbutton_clicked_cb(Ewl_Widget * w, void *ev_data,
					   void *user_data);
void            ewl_check_clicked_cb(Ewl_Widget * w, void *ev_data,
				     void *user_data);

/**
 * @}
 */

#endif				/* __EWL_CHECKBUTTON_H__ */
