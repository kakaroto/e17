
/*
 * The button class is a basic button with a label. This class is inherited by
 * the check button and radio button classes.
 */

#ifndef __EWL_RADIOBUTTON_H__
#define __EWL_RADIOBUTTON_H__

typedef struct _ewl_radiobutton Ewl_RadioButton;
#define EWL_RADIOBUTTON(button) ((Ewl_RadioButton *) button)

struct _ewl_radiobutton {
	Ewl_CheckButton button;
	Ewd_List *chain;
};

Ewl_Widget *ewl_radiobutton_new(const char *l);

/* 1 for checked and 0 for not checked. */
void ewl_radiobutton_set_checked(Ewl_Widget * w, int c);
void ewl_radiobutton_set_chain(Ewl_Widget * w, Ewd_List * chain);

#endif				/* __EWL_RADIOBUTTON_H__ */
