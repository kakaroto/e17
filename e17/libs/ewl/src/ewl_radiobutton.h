
#ifndef __EWL_RADIOBUTTON_H__
#define __EWL_RADIOBUTTON_H__

typedef struct _ewl_radiobutton Ewl_RadioButton;

#define EWL_RADIOBUTTON(button) ((Ewl_RadioButton *) button)

struct _ewl_radiobutton {
	Ewl_CheckButton button;
	Ewd_List *chain;
};

Ewl_Widget *ewl_radiobutton_new(char *l);

void ewl_radiobutton_set_chain(Ewl_Widget * w, Ewl_Widget * c);

#endif				/* __EWL_RADIOBUTTON_H__ */
