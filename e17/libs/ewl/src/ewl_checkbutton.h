
/*
 * The button class is a basic button with a label. This class is inherited by
 * the check button and radio button classes.
 */

#ifndef __EWL_CHECKBUTTON_H
#define __EWL_CHECKBUTTON_H

typedef struct _ewl_checkbutton Ewl_CheckButton;

#define EWL_CHECKBUTTON(button) ((Ewl_CheckButton *) button)

struct _ewl_checkbutton
{
	Ewl_Button button;
	unsigned int checked;
};

Ewl_Widget *ewl_checkbutton_new(char *l);

/* 1 for checked and 0 for not checked. */
void ewl_checkbutton_set_checked(Ewl_Widget * w, int c);
int ewl_checkbutton_is_checked(Ewl_Widget * w);

#endif /* __EWL_CHECKBUTTON_H */
