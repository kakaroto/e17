#ifndef __EWL_CHECKBUTTON_H__
#define __EWL_CHECKBUTTON_H__

/**
 * @file ewl_checkbutton.h
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

Ewl_Widget     *ewl_checkbutton_new(void);
int             ewl_checkbutton_init(Ewl_CheckButton *cb);

void            ewl_checkbutton_checked_set(Ewl_CheckButton *cb, int checked);
int             ewl_checkbutton_is_checked(Ewl_CheckButton *cb);

void            ewl_checkbutton_label_position_set(Ewl_CheckButton *cb,
						   Ewl_Position p);

/*
 * Internally used callbacks, override at your own risk
 */
void ewl_checkbutton_clicked_cb(Ewl_Widget *w, void *ev_data,
					   void *user_data);

/**
 * @}
 */

#endif				/* __EWL_CHECKBUTTON_H__ */
