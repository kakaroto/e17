#ifndef CALLBACK_EDITOR_H
#define CALLBACK_EDITOR_H

typedef struct Ewler_Callback_Editor Ewler_Callback_Editor;

struct Ewler_Callback_Editor {
	Ewl_Box box;
	Ewl_Widget *tree;
	Ewl_Widget *cb_combo;
	Ewl_Widget *handler_entry;
};

Ewl_Widget *ewler_callback_editor_new(void);
void ewler_callback_editor_init(Ewler_Callback_Editor *ce);

#endif
