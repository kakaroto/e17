#ifndef __EWL_ENTRY_H__
#define __EWL_ENTRY_H__

#define EWL_ENTRY(entry) ((Ewl_Entry *) entry)

typedef struct Ewl_Entry Ewl_Entry;
struct Ewl_Entry
{
	Ewl_Text	 text;
	Ewl_Widget	*cursor;

	unsigned int	 multiline;
	unsigned int	 editable;
	unsigned int	 in_select_mode;
};

Ewl_Widget	*ewl_entry_new();
int 		 ewl_entry_init(Ewl_Entry *e);	

void		 ewl_entry_multiline_set(Ewl_Entry *e, unsigned int multiline);
unsigned int	 ewl_entry_multiline_get(Ewl_Entry *e);

void		 ewl_entry_editable_set(Ewl_Entry *e, unsigned int editable);
unsigned int	 ewl_entry_editable_get(Ewl_Entry *e);

/*
 * Internal stuff
 */
void ewl_entry_cb_configure(Ewl_Widget *w, void *ev, void *data);
void ewl_entry_cb_key_down(Ewl_Widget *w, void *ev, void *data);
void ewl_entry_cb_mouse_down(Ewl_Widget *w, void *ev, void *data);
void ewl_entry_cb_mouse_up(Ewl_Widget *w, void *ev, void *data);
void ewl_entry_cb_mouse_move(Ewl_Widget *w, void *ev, void *data);
void ewl_entry_cb_selected(Ewl_Widget *w, void *ev, void *data);
void ewl_entry_cb_deselected(Ewl_Widget *w, void *ev, void *data);

void ewl_entry_cursor_move_left(Ewl_Entry *e);
void ewl_entry_cursor_move_right(Ewl_Entry *e);
void ewl_entry_cursor_move_up(Ewl_Entry *e);
void ewl_entry_cursor_move_down(Ewl_Entry *e);
void ewl_entry_delete_left(Ewl_Entry *e);
void ewl_entry_delete_right(Ewl_Entry *e);

/*
 * cursor stuff
 */
#define EWL_ENTRY_CURSOR(cursor) ((Ewl_Entry_Cursor *) cursor)

typedef struct Ewl_Entry_Cursor Ewl_Entry_Cursor;
struct Ewl_Entry_Cursor
{
	Ewl_Widget	 widget;
	Ewl_Entry	*parent;
};

Ewl_Widget 	*ewl_entry_cursor_new(Ewl_Entry *parent);
int		 ewl_entry_cursor_init(Ewl_Entry_Cursor *c, Ewl_Entry *parent);
void		 ewl_entry_cursor_position_set(Ewl_Entry_Cursor *c, unsigned int pos);
unsigned int	 ewl_entry_cursor_position_get(Ewl_Entry_Cursor *c);

#endif /* __EWL_ENTRY_H__ */

