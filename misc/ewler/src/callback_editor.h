#ifndef CALLBACK_EDITOR_H
#define CALLBACK_EDITOR_H

typedef struct Ewler_Callback_Editor Ewler_Callback_Editor;

#define EWLER_CALLBACK_EDITOR(ce) ((Ewler_Callback_Editor *) (ce))

struct Ewler_Callback_Editor {
	Ewl_Box box;
	Ewl_Widget *target;

	Ewl_Widget *tree;
	Ewl_Widget *cb_combo;
	Ewl_Widget *handler_entry;

	Ewl_Widget *add_button;
	Ewl_Widget *modify_button;
	Ewl_Widget *delete_button;
	Ewl_Widget *clear_button;

	Ewler_Callback *active;
};

void ewler_callback_editor_target_set( Ewler_Callback_Editor *ce,
																			 Ewl_Widget *w );
Ewl_Widget *ewler_callback_editor_new(void);
void ewler_callback_editor_init(Ewler_Callback_Editor *ce);
int ewler_callback_value(char *callback);
char *ewler_callback_string(int callback);

#endif
