
#ifndef __EWL_RADIOBUTTON_H__
#define __EWL_RADIOBUTTON_H__

typedef struct _ewl_radiobutton Ewl_RadioButton;

#define EWL_RADIOBUTTON(button) ((Ewl_RadioButton *) button)

struct _ewl_radiobutton {
	Ewl_CheckButton button;
	Ewd_List       *chain;
};

Ewl_Widget     *ewl_radiobutton_new(char *l);

#define ewl_radiobutton_set_checked(r, c) \
	ewl_checkbutton_set_checked(EWL_CHECKBUTTON(r), c)
#define ewl_radiobutton_is_checked(r) \
	ewl_checkbutton_is_checked(EWL_CHECKBUTTON(r))
void            ewl_radiobutton_set_chain(Ewl_Widget * w, Ewl_Widget * c);

#endif				/* __EWL_RADIOBUTTON_H__ */
