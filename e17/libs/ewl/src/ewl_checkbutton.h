
/*
 * The button class is a basic button with a label. This class is inherited by
 * the check button and radio button classes.
 */

#ifndef __EWL_CHECKBUTTON_H__
#define __EWL_CHECKBUTTON_H__

typedef struct _ewl_checkbutton Ewl_CheckButton;

#define EWL_CHECKBUTTON(button) ((Ewl_CheckButton *) button)

struct _ewl_checkbutton {

	/*
	 * Inherit the basic button properties
	 */
	Ewl_Button      button;

	/*
	 * Label positition determines packing order of the label and the
	 * check
	 */
	Ewl_Position    label_position;

	/*
	 * Public references to the check and label widgets.
	 */
	Ewl_Widget     *check;
};

Ewl_Widget     *ewl_checkbutton_new(char *l);
void            ewl_checkbutton_init(Ewl_CheckButton * cb, char *label);

#define ewl_checkbutton_set_checked(cb, c) \
	ewl_check_set_checked(EWL_CHECK(cb->check), c)
#define ewl_checkbutton_is_checked(cb) \
	ewl_check_is_checked(EWL_CHECK(cb->check))
void            ewl_checkbutton_set_label_position(Ewl_Widget * w,
						   Ewl_Position p);

#endif				/* __EWL_CHECKBUTTON_H__ */
