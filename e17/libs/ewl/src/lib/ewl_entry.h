#ifndef __EWL_ENTRY_H__
#define __EWL_ENTRY_H__

/**
 * @file ewl_entry.h
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
	Ewl_Container container;      /**< Inherit from Ewl_Container */

	char         *text;           /**< The initial text in the entry */
	int           length;         /**< Length of the text displayed */
	Evas_Object  *textobj;        /**< Object does the actual layout work */
	Ecore_DList  *ops;            /**< Series of operations to apply */
	Ecore_DList  *applied;        /**< Applied set of operations */

	Ewl_Widget   *cursor;         /**< Cursor widget */

	int           offset;         /**< Starting position of scrolling */
	unsigned int  editable;       /**< Flag indicating user can edit text */
	Ecore_Timer  *timer;          /**< Time until next text scrolling */
	double        start_time;     /**< Time timer started */
	int           in_select_mode; /**< keyboard cursor movements select? */
	int           multiline;      /**< Deal with multiple lines of text? */
	int           wrap;           /**< Enable wrapping of the text */
};

Ewl_Widget     *ewl_entry_new(char *text);
Ewl_Widget     *ewl_entry_multiline_new(char *text);
int             ewl_entry_init(Ewl_Entry * e, char *text);
void            ewl_entry_text_set(Ewl_Entry * e, char *t);
char           *ewl_entry_text_get(Ewl_Entry * e);
void            ewl_entry_editable_set(Ewl_Entry *e, unsigned int edit);
void            ewl_entry_multiline_set(Ewl_Entry * e, int m);
int             ewl_entry_multiline_get(Ewl_Entry * e);

void            ewl_entry_text_prepend(Ewl_Entry * e, char *text);
void            ewl_entry_text_append(Ewl_Entry * e, char *text);
void            ewl_entry_text_insert(Ewl_Entry * e, char *text, int index);
void		ewl_entry_text_at_cursor_insert(Ewl_Entry * e, char *text);

int             ewl_entry_length_get(Ewl_Entry *e);

void            ewl_entry_font_set(Ewl_Entry *e, char *font, int size);
char           *ewl_entry_font_get(Ewl_Entry *e);
int             ewl_entry_font_size_get(Ewl_Entry *e);

void            ewl_entry_style_set(Ewl_Entry *e, char *style);
char           *ewl_entry_style_get(Ewl_Entry *e);

void            ewl_entry_color_set(Ewl_Entry *e, int r, int g, int b, int a);
void            ewl_entry_color_get(Ewl_Entry *e, int *r, int *g, int *b,
				    int *a);

void            ewl_entry_align_set(Ewl_Entry *e, unsigned int align);
unsigned int    ewl_entry_align_get(Ewl_Entry *e);

void            ewl_entry_wrap_set(Ewl_Entry *e, int wrap);
int             ewl_entry_wrap_get(Ewl_Entry *e);

void            ewl_entry_index_select(Ewl_Entry *e, int si, int ei);
void            ewl_entry_coord_select(Ewl_Entry *e, int sx, int sy, int ex,
				       int ey);

void            ewl_entry_index_geometry_map(Ewl_Entry *e, int index, int *xx,
					     int *yy, int *ww, int *hh);
int             ewl_entry_coord_index_map(Ewl_Entry *e, int x, int y);
int             ewl_entry_coord_geometry_map(Ewl_Entry *e, int x, int y,
					     int *xx, int *yy,
					     int *ww, int *hh);

/*
 * Internal stuff, use at your own risk
 */
void            ewl_entry_cursor_left_move(Ewl_Entry * e);
void            ewl_entry_cursor_right_move(Ewl_Entry * e);
void            ewl_entry_cursor_down_move(Ewl_Entry * e);
void            ewl_entry_cursor_up_move(Ewl_Entry * e);
void            ewl_entry_cursor_home_move(Ewl_Entry * e);
void            ewl_entry_cursor_end_move(Ewl_Entry * e);
void            ewl_entry_left_delete(Ewl_Entry * e);
void            ewl_entry_right_delete(Ewl_Entry * e);
void            ewl_entry_word_begin_delete(Ewl_Entry * e);

/**
 * @enum  Ewl_Entry_Op_Type
 * Provides a series of operations that can be performed on the entry.
 */
typedef enum
{
	EWL_ENTRY_OP_TYPE_NONE,
	EWL_ENTRY_OP_TYPE_COLOR_SET,
	EWL_ENTRY_OP_TYPE_FONT_SET,
	EWL_ENTRY_OP_TYPE_STYLE_SET,
	EWL_ENTRY_OP_TYPE_ALIGN_SET,
	EWL_ENTRY_OP_TYPE_SELECT,
	EWL_ENTRY_OP_TYPE_TEXT_SET,
	EWL_ENTRY_OP_TYPE_TEXT_PREPEND,
	EWL_ENTRY_OP_TYPE_TEXT_APPEND,
	EWL_ENTRY_OP_TYPE_TEXT_INSERT,
	EWL_ENTRY_OP_TYPE_TEXT_DELETE
} Ewl_Entry_Op_Type;

typedef struct Ewl_Entry_Op Ewl_Entry_Op;
struct Ewl_Entry_Op
{
	Ewl_Entry_Op_Type type;
	void (*apply)(Ewl_Entry *e, Ewl_Entry_Op *op);
	void (*free)(void *);
	int position;
};

#define EWL_ENTRY_OP_COLOR(op) ((Ewl_Entry_Op_Color *) op)
typedef struct Ewl_Entry_Op_Color Ewl_Entry_Op_Color;
struct Ewl_Entry_Op_Color
{
	Ewl_Entry_Op op;
	int r, g, b, a;
};

#define EWL_ENTRY_OP_FONT(op) ((Ewl_Entry_Op_Font *) op)
typedef struct Ewl_Entry_Op_Font Ewl_Entry_Op_Font;
struct Ewl_Entry_Op_Font
{
	Ewl_Entry_Op op;
	char *font;
	int size;
};

#define EWL_ENTRY_OP_STYLE(op) ((Ewl_Entry_Op_Style *) op)
typedef struct Ewl_Entry_Op_Style Ewl_Entry_Op_Style;
struct Ewl_Entry_Op_Style
{
	Ewl_Entry_Op op;
	char *style;
};

#define EWL_ENTRY_OP_ALIGN(op) ((Ewl_Entry_Op_Align *) op)
typedef struct Ewl_Entry_Op_Align Ewl_Entry_Op_Align;
struct Ewl_Entry_Op_Align
{
	Ewl_Entry_Op op;
	unsigned int align;
};

#define EWL_ENTRY_OP_SELECT(op) ((Ewl_Entry_Op_Select *) op)
typedef struct Ewl_Entry_Op_Select Ewl_Entry_Op_Select;
struct Ewl_Entry_Op_Select
{
	Ewl_Entry_Op op;
	Evas_Coord sx, sy, ex, ey;
	int si, ei;
	char *match;
	int index;
};

#define EWL_ENTRY_OP_TEXT(op) ((Ewl_Entry_Op_Text *) op)
typedef struct Ewl_Entry_Op_Text Ewl_Entry_Op_Text;
struct Ewl_Entry_Op_Text
{
	Ewl_Entry_Op op;
	char *text;
	int len;
};

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

Ewl_Widget 	*ewl_entry_cursor_new(void);
void 		 ewl_entry_cursor_init(Ewl_Entry_Cursor *c);
void 		 ewl_entry_cursor_position_set(Ewl_Entry_Cursor *c, int pos);
int 		 ewl_entry_cursor_position_get(Ewl_Entry_Cursor *c);

/* 
 * the selection stuff
 */
typedef struct Ewl_Entry_Selection Ewl_Entry_Selection;

#define EWL_ENTRY_SELECTION(selection) ((Ewl_Entry_Selection *) selection)

struct Ewl_Entry_Selection
{
	Ewl_Widget      widget;
	void           *select;
	unsigned int    start;
	unsigned int    end;
};

Ewl_Widget 	*ewl_entry_selection_new(void);
void 		 ewl_entry_selection_init(Ewl_Entry_Selection *s);
void 		 ewl_entry_selection_start_position_set(Ewl_Entry_Selection *s,
							unsigned int start);
int 		ewl_entry_selection_start_position_get(Ewl_Entry_Selection *s);
void 		ewl_entry_selection_end_position_set(Ewl_Entry_Selection *s,
							unsigned int end);
int 		ewl_entry_selection_end_position_get(Ewl_Entry_Selection *s);
void 		ewl_entry_selection_select_to(Ewl_Entry_Selection *s, unsigned int pos);

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

void ewl_entry_realize_cb(Ewl_Widget * w, void *ev_data, void *user_data);
void ewl_entry_unrealize_cb(Ewl_Widget * w, void *ev_data, void *user_data);
void ewl_entry_show_cb(Ewl_Widget *w, void *ev_data, void *user_data);
void ewl_entry_hide_cb(Ewl_Widget *w, void *ev_data, void *user_data);
void ewl_entry_destroy_cb(Ewl_Widget * w, void *ev_data, void *user_data);
void ewl_entry_reparent_cb(Ewl_Widget * w, void *ev_data, void *user_data);

/**
 * @}
 */

#endif				/* __EWL_ENTRY_H__ */
