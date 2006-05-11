#ifndef EWL_ENTRY_H
#define EWL_ENTRY_H

/**
 * @addtogroup Ewl_Entry Ewl_Entry: A text entry widget
 * This widget provides all text entry needs, from single to multiline
 *
 * @{
 */

/**
 * @def EWL_ENTRY_TYPE
 * The type name for the Ewl_Entry widget
 */
#define EWL_ENTRY_TYPE "entry"

/**
 * @def EWL_ENTRY_CURSOR_TYPE
 * The type name for the Ewl_Entry_Cursor widget
 */
#define EWL_ENTRY_CURSOR_TYPE "cursor"

/**
 * @def EWL_ENTRY(entry)
 * Typecast a pointer to an Ewl_Entry
 */
#define EWL_ENTRY(entry) ((Ewl_Entry *) entry)

/**
 * The Ewl_Entry 
 */
typedef struct Ewl_Entry Ewl_Entry;

/**
 * Inherits from Ewl_Text and extends to allow editing of the text
 */
struct Ewl_Entry
{
	Ewl_Text	 text;			/**< The Ewl_Text parent */
	Ewl_Widget	*cursor;		/**< The cursor */

	unsigned int	 multiline;		/**< Is the entry multi-line */
	unsigned int	 editable;		/**< Is the entry editable */
	unsigned int	 in_select_mode;	/**< Are we in select mode */
};

Ewl_Widget	*ewl_entry_new(void);
int 		 ewl_entry_init(Ewl_Entry *e);	

void		 ewl_entry_multiline_set(Ewl_Entry *e, unsigned int multiline);
unsigned int	 ewl_entry_multiline_get(Ewl_Entry *e);

void		 ewl_entry_editable_set(Ewl_Entry *e, unsigned int editable);
unsigned int	 ewl_entry_editable_get(Ewl_Entry *e);

unsigned int	 ewl_entry_selection_clear(Ewl_Entry *e);

/*
 * Internal stuff
 */
void ewl_entry_cb_configure(Ewl_Widget *w, void *ev, void *data);
void ewl_entry_cb_key_down(Ewl_Widget *w, void *ev, void *data);
void ewl_entry_cb_mouse_down(Ewl_Widget *w, void *ev, void *data);
void ewl_entry_cb_mouse_up(Ewl_Widget *w, void *ev, void *data);
void ewl_entry_cb_mouse_move(Ewl_Widget *w, void *ev, void *data);
void ewl_entry_cb_focus_in(Ewl_Widget *w, void *ev, void *data);
void ewl_entry_cb_focus_out(Ewl_Widget *w, void *ev, void *data);
void ewl_entry_cb_disable(Ewl_Widget *w, void *ev, void *data);
void ewl_entry_cb_enable(Ewl_Widget *w, void *ev, void *data);

void ewl_entry_cursor_move_left(Ewl_Entry *e);
void ewl_entry_cursor_move_right(Ewl_Entry *e);
void ewl_entry_cursor_move_up(Ewl_Entry *e);
void ewl_entry_cursor_move_down(Ewl_Entry *e);
void ewl_entry_delete_left(Ewl_Entry *e);
void ewl_entry_delete_right(Ewl_Entry *e);

/*
 * cursor stuff
 */

/**
 * @def EWL_ENTRY_CURSOR(cursor)
 * Typecast a pointer to an Ewl_Entry_Cursor pointer
 */
#define EWL_ENTRY_CURSOR(cursor) ((Ewl_Entry_Cursor *) cursor)

/**
 * The Ewl_Entry_Cursor widget
 */
typedef struct Ewl_Entry_Cursor Ewl_Entry_Cursor;

/**
 * Inherit from Ewl_Widget and extend for the Entry cursor 
 */
struct Ewl_Entry_Cursor
{
	Ewl_Widget	 widget;	/**< Inherit from Ewl_Widget */
	Ewl_Entry	*parent;	/**< The parent Ewl_Text */
};

Ewl_Widget 	*ewl_entry_cursor_new(Ewl_Entry *parent);
int		 ewl_entry_cursor_init(Ewl_Entry_Cursor *c, Ewl_Entry *parent);
void		 ewl_entry_cursor_position_set(Ewl_Entry_Cursor *c, unsigned int pos);
unsigned int	 ewl_entry_cursor_position_get(Ewl_Entry_Cursor *c);

/**
 * @}
 */

#endif

