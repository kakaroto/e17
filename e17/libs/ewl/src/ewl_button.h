
/*
 * The button class is a basic button with a label. This class is inherited by
 * the check button and radio button classes.
 */

#ifndef __EWL_BUTTON_H__
#define __EWL_BUTTON_H__

typedef struct _ewl_button Ewl_Button;

#define EWL_BUTTON(button) ((Ewl_Button *) button)

struct _ewl_button {
	Ewl_Box box;

	char *label;
	Ewl_Widget *label_object;
};

Ewl_Widget *ewl_button_new(char *l);
void ewl_button_set_label(Ewl_Button * b, char *l);

#endif				/* __EWL_BUTTON_H__ */
