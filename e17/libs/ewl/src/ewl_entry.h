
/*\
|*|
|*| The entry widget is used to get input from a user.
|*|
\*/



#ifndef __EWL_ENTRY_H__
#define __EWL_ENTRY_H__

typedef struct _ewl_entry Ewl_Entry;
#define EWL_ENTRY(entry) ((Ewl_Entry *) entry)

struct _ewl_entry {
	Ewl_Container container;
	Ebits_Object cursor;
	int cursor_pos;
	Ewl_Widget *selection;
	Ewl_Widget *text;
	char *font;
	int font_size;
	Window paste_win;
};

Ewl_Widget *ewl_entry_new();
void ewl_entry_set_text(Ewl_Widget * w, const char *t);
char *ewl_entry_get_text(Ewl_Widget * w);
void ewl_entry_set_cursor_pos(Ewl_Widget * w, int p);
int ewl_entry_get_cursor_pos(Ewl_Widget * w);

#endif				/* __EWL_ENTRY_H__ */
