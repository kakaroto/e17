#ifndef __EWL_ENTRY_H__
#define __EWL_ENTRY_H__

/**
 * @defgroup Ewl_Entry Entry: A Single Line Text Entry Widget
 * @brief Defines the Ewl_Entry class to allow for single line editable text.
 *
 * @{
 */

/**
 * @themekey /entry/file
 * @themekey /entry/group
 */

/**
 * Inherits from the Ewl_Widget and provides single line editable text.
 */
typedef struct Ewl_Entry Ewl_Entry;

/**
 * @def EWL_ENTRY(entry)
 * Typecasts a pointer to an Ewl_Entry pointer.
 */
#define EWL_ENTRY(entry) ((Ewl_Entry *) entry)

/**
 * @struct Ewl_Entry
 * Inherits from the Ewl_Container and extends it to provide text and a cursor
 * for editing the contents of the text.
 */
struct Ewl_Entry
{
	Ewl_Container   container; /**< Inherit from Ewl_Container */

	Ewl_Widget     *text; /**< Provide Ewl_Text for text display */
	Ewl_Widget     *cursor; /**< Provide Ewl_Entry_Cursor for cursor display */
	Ewl_Widget     *selection; /**< Provied Ewl_Entry_Selection for selection display */

	int             offset; /**< Starting position of cursor in text */
	int             editable; /**< Flag to indicate if user can edit text */
	Ecore_Timer    *timer; /**< Time until scrolling text on select */
	double          start_time; /**< Time timer started */
	int             in_select_mode; /**< Do keyboard cursor movements select? */
	int             multiline; /**< Do we deal with multiple lines of text? */
};

Ewl_Widget     *ewl_entry_new(char *text);
Ewl_Widget     *ewl_entry_multiline_new(char *text);
int             ewl_entry_init(Ewl_Entry * e, char *text);
void            ewl_entry_text_set(Ewl_Entry * e, char *t);
char           *ewl_entry_text_get(Ewl_Entry * e);
void            ewl_entry_editable_set(Ewl_Entry *e, unsigned int edit);
void            ewl_entry_multiline_set(Ewl_Entry * e, int m);
int             ewl_entry_multiline_get(Ewl_Entry * e);

void ewl_entry_cursor_left_move(Ewl_Entry * e);
void ewl_entry_cursor_right_move(Ewl_Entry * e);
void ewl_entry_cursor_down_move(Ewl_Entry * e);
void ewl_entry_cursor_up_move(Ewl_Entry * e);
void ewl_entry_cursor_home_move(Ewl_Entry * e);
void ewl_entry_cursor_end_move(Ewl_Entry * e);
void ewl_entry_text_insert(Ewl_Entry * e, char *s);
void ewl_entry_left_delete(Ewl_Entry * e);
void ewl_entry_right_delete(Ewl_Entry * e);
void ewl_entry_word_begin_delete(Ewl_Entry * e);

/*
 * Internal API stuff
 */

/* 
 * the cursor stuff 
 */
typedef struct Ewl_Entry_Cursor Ewl_Entry_Cursor;

#define EWL_ENTRY_CURSOR(cursor) ((Ewl_Entry_Cursor *) cursor)

struct Ewl_Entry_Cursor
{
	Ewl_Widget      widget;
	int position;
};

Ewl_Widget *ewl_entry_cursor_new(void);
void ewl_entry_cursor_init(Ewl_Entry_Cursor *c);
void ewl_entry_cursor_position_set(Ewl_Entry_Cursor *c, int pos);
int ewl_entry_cursor_position_get(Ewl_Entry_Cursor *c);

/* 
 * the selection stuff
 */
typedef struct Ewl_Entry_Selection Ewl_Entry_Selection;

#define EWL_ENTRY_SELECTION(selection) ((Ewl_Entry_Selection *) selection)

struct Ewl_Entry_Selection
{
	Ewl_Widget      widget;
	unsigned int start;
	unsigned int    end;
};

Ewl_Widget *ewl_entry_selection_new(void);
void ewl_entry_selection_init(Ewl_Entry_Selection *s);
void ewl_entry_selection_start_position_set(Ewl_Entry_Selection *s, unsigned int start);
int ewl_entry_selection_start_position_get(Ewl_Entry_Selection *s);
void ewl_entry_selection_end_position_set(Ewl_Entry_Selection *s, unsigned int end);
int ewl_entry_selection_end_position_get(Ewl_Entry_Selection *s);
void ewl_entry_selection_select_to(Ewl_Entry_Selection *s, unsigned int pos);

/*
 * Internally used callbacks, override at your own risk.
 */

void ewl_entry_configure_cb(Ewl_Widget * w, void *ev_data, void *user_data);
void ewl_entry_configure_text_cb(Ewl_Widget * w, void *ev_data,
				 void *user_data);
void ewl_entry_key_down_cb(Ewl_Widget * w, void *ev_data, void *user_data);
void ewl_entry_mouse_down_cb(Ewl_Widget * w, void *ev_data, void *user_data);
void ewl_entry_mouse_up_cb(Ewl_Widget * w, void *ev_data, void *user_data);
void ewl_entry_mouse_move_cb(Ewl_Widget * w, void *ev_data, void *user_data);
void ewl_entry_mouse_double_click_cb(Ewl_Widget * w, void *ev_data,
				     void *user_data);
void ewl_entry_select_cb(Ewl_Widget * w, void *ev_data, void *user_data);
void ewl_entry_deselect_cb(Ewl_Widget * w, void *ev_data, void *user_data);

void ewl_entry_update_selected_region_cb(Ewl_Widget * w, void *user_data,
					 void *ev_data);
void ewl_entry_child_show_cb(Ewl_Container * c, Ewl_Widget * w);
void ewl_entry_child_resize_cb(Ewl_Container * entry, Ewl_Widget * text,
			       int size, Ewl_Orientation o);

/**
 * @}
 */

#endif				/* __EWL_ENTRY_H__ */
