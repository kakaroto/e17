#include <Ewl.h>
#include "ewl_private.h"
#include "ewl_debug.h"
#include "ewl_macros.h"

typedef struct Ewl_Text_Fmt Ewl_Text_Fmt;
struct Ewl_Text_Fmt
{
	Ewl_Text_Context *tx;
	unsigned int char_len;
	unsigned int byte_len;
};

static Ewl_Text_Context *ewl_text_default_context = NULL;

/* how much do we extend the text by when we need more space? */
#define EWL_TEXT_EXTEND_VAL  4096

/* Make a static hash to look up the context's. They can be shared between
 * the different text blocks. Just need to ref count them so we know when
 * they can be destroyed
 */
static Ecore_Hash *context_hash = NULL;

static const char ewl_text_trailing_bytes[256] = {
	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
	2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2, 2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,
	3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3, 4,4,4,4,4,4,4,4,5,5,5,5,6,6,6,6
};

/* returns length of the next utf-8 sequence */
#define EWL_TEXT_CHAR_BYTE_LEN(s) \
	(ewl_text_trailing_bytes[(unsigned int)(unsigned char)((s)[0])])

static Ewl_Text_Fmt *ewl_text_fmt_get(Ewl_Text *t, unsigned int char_idx);
static void ewl_text_current_fmt_set(Ewl_Text *t, unsigned int context_mask, 
						Ewl_Text_Context *change);
static void ewl_text_fmt_apply(Ewl_Text *t, unsigned int context_mask,
						Ewl_Text_Context *change,
						unsigned int char_idx,
						unsigned int char_len);
static void ewl_text_fmt_walk(Ewl_Text *t);
static void ewl_text_cb_fmt_free(Ewl_Text_Fmt *fmt);

static void ewl_text_text_insert_private(Ewl_Text *t, const char *txt, 
				unsigned int char_idx, unsigned int *char_len, 
				unsigned int *byte_len);

static void ewl_text_context_cb_free(void *data);
static void ewl_text_context_print(Ewl_Text_Context *tx, const char *indent);
#if 0
static char *ewl_text_context_name_get(Ewl_Text_Context *tx, 
			unsigned int context_mask, Ewl_Text_Context *tx_change);
#endif
static void ewl_text_context_merge(Ewl_Text_Context *tx, unsigned int context_mask,
			Ewl_Text_Context *tx_change);
static Ewl_Text_Context *ewl_text_context_find(Ewl_Text_Context *tx,
			unsigned int context_mask, Ewl_Text_Context *tx_change);
static void ewl_text_context_format_string_create(Ewl_Text_Context *ctx);

static void ewl_text_display(Ewl_Text *t);
static void ewl_text_plaintext_parse(Evas_Object *tb, char *txt);

static char *ewl_text_color_string_get(int r, int g, int b, int a);

static Evas_Textblock_Cursor *ewl_text_textblock_cursor_position(Ewl_Text *t, 
							unsigned int char_idx);
static unsigned int ewl_text_textblock_cursor_to_index(Evas_Textblock_Cursor *cursor);

static void ewl_text_triggers_remove(Ewl_Text *t);
static void ewl_text_triggers_shift(Ewl_Text *t, unsigned int char_pos, 
					unsigned int char_len, unsigned int del);
static void ewl_text_trigger_position(Ewl_Text *t, Ewl_Text_Trigger *trig);

static void ewl_text_trigger_add(Ewl_Text *t, Ewl_Text_Trigger *trigger);
static void ewl_text_trigger_del(Ewl_Text *t, Ewl_Text_Trigger *trigger);
static void ewl_text_trigger_area_add(Ewl_Text *t, Ewl_Text_Trigger *cur, 
			Evas_Coord x, Evas_Coord y, Evas_Coord w, Evas_Coord h);

static void ewl_text_selection_select_to(Ewl_Text_Trigger *s, 
						unsigned int char_idx);

static void ewl_text_char_to_byte(Ewl_Text *t, unsigned int char_idx, 
						unsigned int char_len,
						unsigned int *byte_idx,
						unsigned int *byte_len);
static void ewl_text_byte_to_char(Ewl_Text *t, unsigned int byte_idx, 
						unsigned int byte_len,
						unsigned int *char_idx,
						unsigned int *char_len);
static char *ewl_text_text_next_char(const char *text, 
						unsigned int *idx);
static unsigned int ewl_text_context_hash_key(const void *ctx);
static int ewl_text_context_hash_cmp(const void *ctx1, const void *ctx2);

/**
 * @return Returns a new Ewl_Text widget on success, NULL on failure.
 * @brief Creates a new Ewl_Text widget
 */
Ewl_Widget *
ewl_text_new(void)
{
	Ewl_Widget *w;
	DENTER_FUNCTION(DLEVEL_STABLE);

	w = NEW(Ewl_Text, 1);
	if (!w)
		DRETURN_PTR(NULL, DLEVEL_STABLE);

	if (!ewl_text_init(EWL_TEXT(w)))
	{
		ewl_widget_destroy(w);
		DRETURN_PTR(NULL, DLEVEL_STABLE);
	}

	DRETURN_PTR(w, DLEVEL_STABLE);
}

/**
 * @param t: The Ewl_Text widget
 * @return Returns TRUE on successfully init or FALSE on failure
 * @brief Initializes an Ewl_Text widget to default values
 */
int
ewl_text_init(Ewl_Text *t)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("t", t, FALSE);

	if (!ewl_container_init(EWL_CONTAINER(t)))
		DRETURN_INT(FALSE, DLEVEL_STABLE);

	ewl_widget_appearance_set(EWL_WIDGET(t), EWL_TEXT_TYPE);
	ewl_widget_inherit(EWL_WIDGET(t), EWL_TEXT_TYPE);

	ewl_object_fill_policy_set(EWL_OBJECT(t), EWL_FLAG_FILL_HFILL 
						| EWL_FLAG_FILL_VFILL);

	t->formatting.nodes = ecore_dlist_new();
	if (!t->formatting.nodes) 
		DRETURN_INT(FALSE, DLEVEL_STABLE);
	ecore_dlist_set_free_cb(t->formatting.nodes, 
					ECORE_FREE_CB(ewl_text_cb_fmt_free));

	t->formatting.current.tx = ewl_text_context_default_create(t);
	ewl_text_context_acquire(t->formatting.current.tx);

	ewl_callback_append(EWL_WIDGET(t), EWL_CALLBACK_CONFIGURE, 
					ewl_text_cb_configure, NULL);
	ewl_callback_append(EWL_WIDGET(t), EWL_CALLBACK_REVEAL,
					ewl_text_cb_reveal, NULL);
	ewl_callback_append(EWL_WIDGET(t), EWL_CALLBACK_OBSCURE,
					ewl_text_cb_obscure, NULL);
	ewl_callback_append(EWL_WIDGET(t), EWL_CALLBACK_SHOW,
					ewl_text_cb_show, NULL);
	ewl_callback_append(EWL_WIDGET(t), EWL_CALLBACK_HIDE,
					ewl_text_cb_hide, NULL);
	ewl_callback_prepend(EWL_WIDGET(t), EWL_CALLBACK_DESTROY,
					ewl_text_cb_destroy, NULL);

	ewl_container_add_notify_set(EWL_CONTAINER(t), 
					ewl_text_cb_child_add);
	ewl_container_remove_notify_set(EWL_CONTAINER(t), 
					ewl_text_cb_child_del);

	t->dirty = TRUE;

	/* text consumes tabs by default */
//	ewl_widget_ignore_focus_change_set(EWL_WIDGET(t), TRUE);
	ewl_widget_focusable_set(EWL_WIDGET(t), FALSE);

	DRETURN_INT(TRUE, DLEVEL_STABLE);
}

/**
 * @param t: The Ewl_Text to get the length from
 * @return Returns the character length of the text in the widget @a t
 * @brief Retrieve the character length of the text
 */
unsigned int
ewl_text_length_get(Ewl_Text *t)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("t", t, 0);
	DCHECK_TYPE_RET("t", t, EWL_TEXT_TYPE, 0);

	DRETURN_INT(t->length.chars, DLEVEL_STABLE);
}

/**
 * @param t: The Ewl_Text to get the layout offsets from
 * @param x: A pointer to an integer to store the x offset of the text display
 * @param y: A pointer to an integer to store the y offset of the text display
 * @return Returns no value.
 * @brief Retrieve the current layout offsets of the text
 */
void
ewl_text_offsets_get(Ewl_Text *t, int *x, int *y)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("t", t);
	DCHECK_TYPE("t", t, EWL_TEXT_TYPE);

	if (x) *x = t->offset.x;
	if (y) *y = t->offset.y;

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param t: The Ewl_Text to set the layout offsets
 * @param x: The x amount to offset of the text display
 * @param y: The y amount to offset of the text display
 * @return Returns no value.
 * @brief Set the current layout offsets of the text
 */
void
ewl_text_offsets_set(Ewl_Text *t, int x, int y)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("t", t);
	DCHECK_TYPE("t", t, EWL_TEXT_TYPE);

	t->offset.x = x;
	t->offset.y = y;
	ewl_widget_configure(EWL_WIDGET(t));

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param t: The Ewl_Text to get the geometry from
 * @param char_idx: The character index to get the geometry for
 * @param x: Where to put the x value
 * @param y: Where to put the y value
 * @param w: Where to put the w value
 * @param h: Where to put the h value
 * @brief Map the given character index into a position in the text widget
 */
void
ewl_text_index_geometry_map(Ewl_Text *t, unsigned int char_idx, int *x, int *y, 
							int *w, int *h)
{
	Evas_Coord tx = 0, ty = 0, tw = 0, th = 0;
	Evas_Textblock_Cursor *cursor;
	int shifting = 0;
	unsigned int byte_idx = 0;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("t", t);
	DCHECK_TYPE("t", t, EWL_TEXT_TYPE);

	/* can't do this if we don't have an evas object */
	if ((!REALIZED(t)) || (!t->textblock) || (!t->text))
	{
		if (x) *x = 0;
		if (y) *y = 0;
		if (w) *w = 1;
		if (h) *h = ewl_theme_data_int_get(EWL_WIDGET(t), "font_size");

		DRETURN(DLEVEL_STABLE);
	}

	/* force a display of the text */
	if (t->dirty) ewl_text_display(t);

	if (char_idx >= t->length.chars)
	{
		char_idx --;
		shifting = 1;
	}

	ewl_text_char_to_byte(t, char_idx, 0, &byte_idx, NULL);
	cursor = ewl_text_textblock_cursor_position(t, byte_idx);
	evas_textblock_cursor_char_geometry_get(cursor, &tx, &ty, &tw, &th);
	evas_textblock_cursor_free(cursor);

	if (x) *x = (int)(tx + CURRENT_X(t));
	if (y) *y = (int)(ty + CURRENT_Y(t));
	if (w) *w = (int)tw;
	if (h) *h = (int)th;

	/* if we didn't count the last item, move us over to the other side
	 * of it */
	if (shifting) *x += *w;

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param t: The Ewl_Text to mapp the coords into
 * @param x: The x coord to map
 * @param y: The y coord to map
 * @return Returns the character index of the given coordinates
 * @brief Map the given coordinate into an index into the text widget
 */
unsigned int
ewl_text_coord_index_map(Ewl_Text *t, int x, int y)
{
	Evas_Textblock_Cursor *cursor;
	unsigned int byte_idx = 0, char_idx = 0, ctmp = 0;
	Evas_Coord tx, ty, cx = 0, cy, cw, ch;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("t", t, 0);
	DCHECK_TYPE_RET("t", t, EWL_TEXT_TYPE, 0);

	if ((!REALIZED(t)) || (!t->textblock) || (!t->text))
		DRETURN_INT(0, DLEVEL_STABLE);

	/* force a display of the text */
	if (t->dirty) ewl_text_display(t);

	tx = (Evas_Coord)(x - CURRENT_X(t));
	ty = (Evas_Coord)(y - CURRENT_Y(t));

	cursor = evas_object_textblock_cursor_new(t->textblock);

	/* see if we have the mouse over a char */
	if (!evas_textblock_cursor_char_coord_set(cursor, tx, ty))
	{
		int line;

		/* if not, see if the mouse is by a line */
		line = evas_textblock_cursor_line_coord_set(cursor, ty);
		if (line >= 0)
		{
			/* if so, get the line geometry and determine start
			 * or end of line */
			evas_textblock_cursor_line_geometry_get(cursor, 
							&cx, &cy, &cw, &ch);
			if (x < (cx + (cw / 2)))
				evas_textblock_cursor_line_first(cursor);
			else
			{
				const char *txt;
				evas_textblock_cursor_line_last(cursor);

				/* we want to be past the last char so we
				 * need to increment this by 1 to begin */
				txt = evas_textblock_cursor_node_format_get(cursor);

				/* Increment if we're on the last line */
				if (!txt || (strcmp(txt, "\n")))
					char_idx ++;
			}
		}
		else
		{
			evas_textblock_cursor_line_set(cursor, 0);
			evas_textblock_cursor_line_first(cursor);
		}
	}
	else 
	{
		evas_textblock_cursor_char_geometry_get(cursor,
						&cx, &cy, &cw, &ch);
		if (tx > (cx + ((cw + 1) >> 1)))
			 char_idx ++;
	}

	byte_idx = ewl_text_textblock_cursor_to_index(cursor);
	ewl_text_byte_to_char(t, byte_idx, 0, &ctmp, NULL);
	evas_textblock_cursor_free(cursor);

	char_idx += ctmp;

	DRETURN_INT(char_idx, DLEVEL_STABLE);
}

/**
 * @param t: The Ewl_Text to get the text from
 * @return Returns the text in the widget @a t or NULL if no text is set
 * @brief Retrieve the text from the text widget
 */
char *
ewl_text_text_get(Ewl_Text *t)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("t", t, NULL);
	DCHECK_TYPE_RET("t", t, EWL_TEXT_TYPE, NULL);

	DRETURN_PTR(((t->text) ? strdup(t->text) : NULL), DLEVEL_STABLE);
}

/**
 * @param t: The Ewl_Text to clear
 * @return Returns no value
 * @brief Clear the text widget
 */
void
ewl_text_clear(Ewl_Text *t)
{
	Ewl_Text_Fmt *fmt;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("t", t);
	DCHECK_TYPE("t", t, EWL_TEXT_TYPE);

	if (t->length.chars > 0)
	{
		ewl_text_cursor_position_set(t, 0);
		ewl_text_text_delete(t, t->length.chars);
	}
	t->dirty = TRUE;

	if (t->formatting.current.tx)
		ewl_text_context_release(t->formatting.current.tx);

	t->formatting.current.tx = ewl_text_context_default_create(t);
	t->formatting.current.char_idx = 0;
	t->formatting.current.byte_idx = 0;

	/* make sure this list is empty */
	while ((fmt = ecore_dlist_remove_first(t->formatting.nodes)))
	{
		if (fmt->tx) ewl_text_context_release(fmt->tx);
		FREE(fmt);
	}

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param t: The Ewl_Text to set the text into
 * @param text: The text to set into the widget
 * @return Returns no value
 * @brief Set the text in the text widget
 */
void
ewl_text_text_set(Ewl_Text *t, const char *text)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("t", t);
	DCHECK_TYPE("t", t, EWL_TEXT_TYPE);

	ewl_text_clear(t);
	ewl_text_text_append(t, text);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param t: The Ewl_Text to set the text into
 * @param text: The text to set into the widget
 * @return Returns no value
 * @brief Prepend the given text into the text widget
 */
void 
ewl_text_text_prepend(Ewl_Text *t, const char *text)
{
	Ewl_Text_Fmt *fmt;
	unsigned int char_len = 0, byte_len = 0;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("t", t);

	/* don't do anything if there is no text */
	if (!text) DRETURN(DLEVEL_STABLE);

	ewl_text_text_insert_private(t, text, 0, &char_len, &byte_len);
	if (t->formatting.current.tx)
	{
		fmt = NEW(Ewl_Text_Fmt, 1);
		fmt->tx = t->formatting.current.tx;
		ewl_text_context_acquire(fmt->tx);
		fmt->char_len = char_len;
		fmt->byte_len = byte_len;

		ecore_dlist_prepend(t->formatting.nodes, fmt);

		/* we free this here as the cursor_position_set may not
		 * actually remove it if the cursor dosen't move */
		ewl_text_context_release(t->formatting.current.tx);
		t->formatting.current.tx = NULL;
	}
	else
	{
		fmt = ecore_dlist_goto_first(t->formatting.nodes);
		fmt->char_len += char_len;
		fmt->byte_len += byte_len;
	}
	ewl_text_cursor_position_set(t, char_len);
	t->dirty = TRUE;

	if (text) ewl_text_triggers_shift(t, 0, char_len, FALSE);

	ewl_widget_configure(EWL_WIDGET(t));

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param t: The Ewl_Text to set the text into
 * @param text: The text to set into the widget
 * @return Returns no value
 * @brief Append the text into the text widget
 */
void
ewl_text_text_append(Ewl_Text *t, const char *text)
{
	Ewl_Text_Fmt *fmt;
	unsigned int char_len = 0, byte_len = 0;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("t", t);
	DCHECK_TYPE("t", t, EWL_TEXT_TYPE);

	/* don't do anything if there is no text */
	if (!text) DRETURN(DLEVEL_STABLE);

	ewl_text_text_insert_private(t, text, t->length.chars, &char_len, &byte_len);
	if (t->formatting.current.tx)
	{
		fmt = NEW(Ewl_Text_Fmt, 1);
		fmt->tx = t->formatting.current.tx;
		ewl_text_context_acquire(fmt->tx);
		fmt->char_len = char_len;
		fmt->byte_len = byte_len;

		ecore_dlist_append(t->formatting.nodes, fmt);

		/* we free this here as the cursor_position_set may not
		 * actually remove it if the cursor dosen't move */
		ewl_text_context_release(t->formatting.current.tx);
		t->formatting.current.tx = NULL;
	}
	else
	{
		fmt = ecore_dlist_goto_last(t->formatting.nodes);
		fmt->char_len += char_len;
		fmt->byte_len += byte_len;
	}
	ewl_text_cursor_position_set(t, t->length.chars);
	t->dirty = TRUE;

	ewl_widget_configure(EWL_WIDGET(t));

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param t: The Ewl_Text to set the text into
 * @param text: The text to set into the widget
 * @param char_idx: The index to insert the text at
 * @return Returns no value
 * @brief Insert the given text into the text widget
 */
void
ewl_text_text_insert(Ewl_Text *t, const char *text, unsigned int char_idx)
{
	Ewl_Text_Fmt *fmt;
	Ewl_Text_Context *tx;
	unsigned int char_len = 0, byte_len = 0;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("t", t);
	DCHECK_TYPE("t", t, EWL_TEXT_TYPE);

	/* don't do anything if there is no text */
	if (!text) DRETURN(DLEVEL_STABLE);

	/* Limit the index to be within safe boundaries */
	if (char_idx > t->length.chars + 1)
		char_idx = t->length.chars + 1;

	/* make sure we set the position _before_ inserting the text else
	 * it'll fuck up the cursor_position_set call when inserting into
	 * and empty node list */
	tx = t->formatting.current.tx;
	t->formatting.current.tx = NULL;
	ewl_text_cursor_position_set(t, char_idx);

	ewl_text_text_insert_private(t, text, char_idx, &char_len, &byte_len);
	
	fmt = ecore_dlist_current(t->formatting.nodes);
	if (tx)
	{	
		Ewl_Text_Fmt *new;

		new = NEW(Ewl_Text_Fmt, 1);
		new->tx = tx;
		ewl_text_context_acquire(new->tx);
		new->char_len = char_len;
		new->byte_len = byte_len;

		/* insert before the current node */
		if (t->formatting.current.char_idx == char_idx)
			ecore_dlist_insert(t->formatting.nodes, new);

		/* insert after the current node */
		else if ((t->formatting.current.char_idx + fmt->char_len) == char_idx)
		{
			Ewl_Text_Fmt *cur;

			ecore_dlist_next(t->formatting.nodes);
			cur = ecore_dlist_current(t->formatting.nodes);
			if (cur) ecore_dlist_insert(t->formatting.nodes, new);
			else ecore_dlist_append(t->formatting.nodes, new);

			/* move back to the previous mode so that our index
			 * is still correct */
			ecore_dlist_previous(t->formatting.nodes);
		}
		/* insert in the middle of the current node */
		else
		{
			Ewl_Text_Fmt *left;
			unsigned int blen = 0;

			left = NEW(Ewl_Text_Fmt, 1);
			left->tx = fmt->tx;
			ewl_text_context_acquire(left->tx);
			left->char_len = char_idx - t->formatting.current.char_idx;

			ewl_text_char_to_byte(t,
				t->formatting.current.char_idx,
				left->char_len, NULL, &blen);

			left->byte_len = blen;

			fmt->char_len -= left->char_len;
			fmt->byte_len -= left->byte_len;

			ecore_dlist_insert(t->formatting.nodes, new);
			ecore_dlist_insert(t->formatting.nodes, left);
		}
	}
	else
	{
		fmt->char_len += char_len;
		fmt->byte_len += byte_len;
	}

	if (tx) ewl_text_context_release(tx);
	ewl_text_cursor_position_set(t, char_idx + char_len);
	t->dirty = TRUE;

	if (text) ewl_text_triggers_shift(t, char_idx, char_len, FALSE);

	ewl_widget_configure(EWL_WIDGET(t));

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param t: The Ewl_Text to delete the text from
 * @param char_len: The length of text to delete
 * @return Returns no value
 * @brief This will delete the specified length of text from the current cursor
 * position
 */
void
ewl_text_text_delete(Ewl_Text *t, unsigned int char_len)
{
	Ewl_Text_Fmt *fmt;
	unsigned int byte_idx = 0, byte_len = 0;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("t", t);
	DCHECK_TYPE("t", t, EWL_TEXT_TYPE);

	if ((!t->text) || (char_len == 0) || (t->cursor_position >= t->length.chars))
		DRETURN(DLEVEL_STABLE);

	ewl_text_char_to_byte(t, t->cursor_position, char_len,
						&byte_idx, &byte_len);

	/* don't try to delete more then we have after the current cursor
	 * position */
	if ((t->length.chars - t->cursor_position) < char_len)
		char_len = t->length.chars - t->cursor_position;

	t->length.chars -= char_len;
	if (t->length.chars > 0)
	{
		t->length.bytes -= byte_len;
		memmove(t->text + byte_idx, 
				t->text + byte_idx + byte_len,
				t->length.bytes - byte_idx);

		t->text[t->length.bytes] = '\0';

		ewl_text_triggers_shift(t, t->cursor_position, char_len, TRUE);
	}
	else
	{
		IF_FREE(t->text);
		t->length.bytes = 0;
		t->length.chars = 0;
		t->total_size = 0;
		t->cursor_position = 0;
		ewl_text_triggers_remove(t);

		/* cleanup the selection */
		if (t->selection) 
			ewl_widget_destroy(EWL_WIDGET(t->selection));

		t->selection = NULL;
	}

	/* delete the formatting nodes */
	fmt = ecore_dlist_current(t->formatting.nodes);
	while (char_len > 0)
	{
		unsigned int left;

		/* how much of this node is to the left of the delete area */
		left = t->cursor_position - t->formatting.current.char_idx;

		/* does the current node have enough to handle the
		 * full delete */
		if ((fmt->char_len - left) > char_len)
		{
			fmt->char_len -= char_len;
			fmt->byte_len -= byte_len;

			break;
		}

		/* if there is something left in this node then correct the
		 * numbers */
		if (left > 0)
		{
			unsigned int blen = 0;

			char_len -= (fmt->char_len - left);

			fmt->char_len = left;
			ewl_text_char_to_byte(t,
				t->formatting.current.char_idx,
				fmt->char_len, NULL, &blen);
			fmt->byte_len = blen;

			/* remove node and update the current index to point
			 * to the correct index */
			fmt = ecore_dlist_next(t->formatting.nodes);
			t->formatting.current.char_idx += left;
			t->formatting.current.byte_idx += blen;
		}
		else
		{
			char_len -= fmt->char_len;
			ecore_dlist_remove(t->formatting.nodes);
			fmt = ecore_dlist_current(t->formatting.nodes);
		}
	}
	t->dirty = TRUE;

	fmt = ecore_dlist_current(t->formatting.nodes);
	if (!fmt)
	{
		fmt = ecore_dlist_goto_last(t->formatting.nodes);
		if (fmt) 
		{
			t->formatting.current.char_idx = 
				t->length.chars - fmt->char_len;
			t->formatting.current.byte_idx = 
				t->length.bytes - fmt->byte_len;
		}
		else 
		{
			t->formatting.current.char_idx = 0;
			t->formatting.current.byte_idx = 0;
		}
	}

	if (ecore_dlist_nodes(t->formatting.nodes) == 0)
		t->formatting.current.tx =
			ewl_text_context_default_create(t);

	if (t->cursor_position > t->length.chars)
		ewl_text_cursor_position_set(t, t->length.chars);

	ewl_widget_configure(EWL_WIDGET(t));

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param t: The text to set the selectable value of
 * @param selectable: The selectable value to set
 * @return Returns no value
 * @brief Set if the text is selectable
 */
void
ewl_text_selectable_set(Ewl_Text *t, unsigned int selectable)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("t", t);
	DCHECK_TYPE("t", t, EWL_TEXT_TYPE);

	if (t->selectable == selectable)
		DRETURN(DLEVEL_STABLE);

	t->selectable = selectable;

	if (t->selectable)
	{
		ewl_callback_append(EWL_WIDGET(t), EWL_CALLBACK_MOUSE_DOWN,
						ewl_text_cb_mouse_down, NULL);
		ewl_callback_append(EWL_WIDGET(t), EWL_CALLBACK_MOUSE_UP,
						ewl_text_cb_mouse_up, NULL);
	}
	else
	{
		ewl_callback_del(EWL_WIDGET(t), EWL_CALLBACK_MOUSE_DOWN,
						ewl_text_cb_mouse_down);
		ewl_callback_del(EWL_WIDGET(t), EWL_CALLBACK_MOUSE_UP,
						ewl_text_cb_mouse_up);
	}

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param t: The text to get the selectable value from
 * @return Returns the selectable value of the widget
 * @brief Get the selectable state of the text
 */
unsigned int
ewl_text_selectable_get(Ewl_Text *t)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("t", t, 0);
	DCHECK_TYPE_RET("t", t, EWL_TEXT_TYPE, 0);

	DRETURN_INT(t->selectable, DLEVEL_STABLE);
}

/**
 * @param t: The Ewl_Text widget to get the selection text from
 * @return Returns the selection text or NULL if none set
 * @brief Gets the current text of the selection
 */
char *
ewl_text_selection_text_get(Ewl_Text *t)
{
	char *ret = NULL;
	unsigned int byte_pos = 0;
	unsigned int byte_len = 0;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("t", t, NULL);
	DCHECK_TYPE_RET("t", t, EWL_TEXT_TYPE, NULL);

	if ((!t->selection) || (t->selection->char_len == 0))
		DRETURN_PTR(NULL, DLEVEL_STABLE);

	ewl_text_char_to_byte(t, t->selection->char_pos, t->selection->char_len,
							&byte_pos, &byte_len);

	ret = malloc(sizeof(char) * (byte_len + 1));
	if (!ret) 
		DRETURN_PTR(NULL, DLEVEL_STABLE);

	memcpy(ret, t->text + byte_pos, byte_len);
	ret[byte_len] = '\0';

	DRETURN_PTR(ret, DLEVEL_STABLE);
}

/**
 * @param t: The Ewl_Text to get the selection from
 * @return Returns the selection object of this text or NULL if no current
 * selection
 * @brief Get the current text selection
 */
Ewl_Text_Trigger *
ewl_text_selection_get(Ewl_Text *t)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("t", t, NULL);
	DCHECK_TYPE_RET("t", t, EWL_TEXT_TYPE, NULL);

	if (t->selection && ewl_text_trigger_length_get(t->selection) > 0)
		DRETURN_PTR(t->selection, DLEVEL_STABLE);

	DRETURN_PTR(NULL, DLEVEL_STABLE);
}

/**
 * @param t: The text to check if there is a selection
 * @return Returns TRUE if there is selected text, FALSE otherwise
 * @brief Check if anything is selected in the text widget
 */
unsigned int
ewl_text_has_selection(Ewl_Text *t)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("t", t, FALSE);
	DCHECK_TYPE_RET("t", t, EWL_TEXT_TYPE, FALSE);

	if (ewl_text_selection_get(t))
		DRETURN_INT(TRUE, DLEVEL_STABLE);

	DRETURN_INT(FALSE, DLEVEL_STABLE);
}

/**
 * @param t: The Ewl_Text widget to set the position into
 * @param char_pos: The position to set
 * @return Returns no value.
 * @brief Set the cursor position in the text widget
 */
void
ewl_text_cursor_position_set(Ewl_Text *t, unsigned int char_pos)
{
	Ewl_Text_Fmt *fmt;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("t", t);
	DCHECK_TYPE("t", t, EWL_TEXT_TYPE);

	/* make sure we aren't more then the next char past the 
	 * end of the text */
	if (char_pos > t->length.chars) char_pos = t->length.chars;

	/* it's the same position, do nothing */
	if (char_pos == t->cursor_position)
	{
		/* nothing to check if we have no text */
		if (t->length.chars > 0)
		{
			fmt = ecore_dlist_current(t->formatting.nodes);
			if (!fmt) DWARNING("Current format node is NULL.");
		}

		DRETURN(DLEVEL_STABLE);
	}

	/* clean the current context if it exists */
	if (t->formatting.current.tx)
	{
		ewl_text_context_release(t->formatting.current.tx);
		t->formatting.current.tx = NULL;
	}

	t->cursor_position = char_pos;

	/* short cut the start/end cases */
	if (char_pos == 0)
	{
		t->formatting.current.char_idx = 0;
		t->formatting.current.byte_idx = 0;

		ecore_dlist_goto_first(t->formatting.nodes);

		fmt = ecore_dlist_current(t->formatting.nodes);
		if (!fmt) DWARNING("Current format node is NULL.");

		DRETURN(DLEVEL_STABLE);
	}
	else if (char_pos >= t->length.chars)
	{
		fmt = ecore_dlist_goto_last(t->formatting.nodes);

		t->formatting.current.char_idx = 
			t->length.chars - fmt->char_len;;
		t->formatting.current.byte_idx = 
			t->length.bytes - fmt->byte_len;;

		fmt = ecore_dlist_current(t->formatting.nodes);
		if (!fmt) DWARNING("Current format node is NULL.");

		DRETURN(DLEVEL_STABLE);
	}

	/* XXX We might want to make this a bit smarter. Currrently it will
	 * always walk from the current position to the new position. But,
	 * we might want to do something like, if this seems to be closer ot
	 * the start of the list, start there. Same for end. 
	 * 
	 * Not sure if it would be faster not not ...
	 */

	/* position the formatting list at the correct node */
	if (t->formatting.current.char_idx < char_pos)
	{
		/* walk forward until we cover the given position */
		fmt = ecore_dlist_current(t->formatting.nodes);
		while ((fmt->char_len + t->formatting.current.char_idx) < char_pos)
		{
			t->formatting.current.char_idx += fmt->char_len;
			t->formatting.current.byte_idx += fmt->byte_len;

			ecore_dlist_next(t->formatting.nodes);
			fmt = ecore_dlist_current(t->formatting.nodes);
		}
	}
	else
	{
		/* walk back until we're less then the given position */
		while (t->formatting.current.char_idx > char_pos)
		{
			ecore_dlist_previous(t->formatting.nodes);
			fmt = ecore_dlist_current(t->formatting.nodes);
			if (!fmt) break;

			t->formatting.current.char_idx -= fmt->char_len;
			t->formatting.current.byte_idx -= fmt->byte_len;
		}
	}

	fmt = ecore_dlist_current(t->formatting.nodes);
	if (!fmt) DWARNING("Current format node is NULL.");

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param t: The Ewl_Text to get the cursor position from
 * @return Returns the current cursor position in the widget
 * @brief Retrieve the cursor position from the text widget
 */
unsigned int
ewl_text_cursor_position_get(Ewl_Text *t)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("t", t, 0);
	DCHECK_TYPE_RET("t", t, EWL_TEXT_TYPE, 0);

	DRETURN_INT(t->cursor_position, DLEVEL_STABLE);
}

/**
 * @param t: The Ewl_Text to get the cursor position one line up from
 * @return Returns the cursor position if we moved up one line
 * @brief Get the index if we were to move the cursor up one line
 */
unsigned int
ewl_text_cursor_position_line_up_get(Ewl_Text *t)
{
	Evas_Textblock_Cursor *cursor;
	unsigned int cur_char_idx = 0, byte_idx = 0;
	Evas_Coord cx, cw;
	Evas_Coord lx, ly, lw, lh;
	int line;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("t", t, t->cursor_position);
	DCHECK_TYPE_RET("t", t, EWL_TEXT_TYPE, t->cursor_position);

	cur_char_idx = ewl_text_cursor_position_get(t);
	ewl_text_char_to_byte(t, cur_char_idx, 0, &byte_idx, NULL);

	cursor = ewl_text_textblock_cursor_position(t, byte_idx);
	line = evas_textblock_cursor_char_geometry_get(cursor, &cx, NULL, 
								&cw, NULL);
	line --;

	if (evas_object_textblock_line_number_geometry_get(t->textblock, 
						line, &lx, &ly, &lw, &lh))
	{
		if (!evas_textblock_cursor_char_coord_set(cursor, cx + (cw / 2), ly))
		{
			if (evas_textblock_cursor_line_set(cursor, line))
			{
				if ((cx + (cw / 2)) >= (lx + lw))
					evas_textblock_cursor_line_last(cursor);
				else
					evas_textblock_cursor_line_first(cursor);
			}
		}

	}
	byte_idx = ewl_text_textblock_cursor_to_index(cursor);
	cur_char_idx = 0;
	ewl_text_byte_to_char(t, byte_idx, 0, &cur_char_idx, NULL);

	DRETURN_INT(cur_char_idx, DLEVEL_STABLE);
}

/**
 * @param t: The Ewl_Text to get the cursor position one line down from
 * @return Returns the cursor position if we moved down one line
 * @brief Get the index if we were to move the cursor down one line
 */
unsigned int
ewl_text_cursor_position_line_down_get(Ewl_Text *t)
{
	Evas_Textblock_Cursor *cursor;
	unsigned int cur_char_idx = 0, byte_idx = 0;
	Evas_Coord cx, cw;
	Evas_Coord lx, ly, lw, lh;
	int line;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("t", t, t->cursor_position);
	DCHECK_TYPE_RET("t", t, EWL_TEXT_TYPE, t->cursor_position);

	cur_char_idx = ewl_text_cursor_position_get(t);
	ewl_text_char_to_byte(t, cur_char_idx, 0, &byte_idx, NULL);

	cursor = ewl_text_textblock_cursor_position(t, byte_idx);
	line = evas_textblock_cursor_char_geometry_get(cursor, &cx, NULL, 
								&cw, NULL);
	line ++;

	if (evas_object_textblock_line_number_geometry_get(t->textblock, 
						line, &lx, &ly, &lw, &lh))
	{
		if (!evas_textblock_cursor_char_coord_set(cursor, cx + (cw / 2), ly))
		{
			if (evas_textblock_cursor_line_set(cursor, line))
			{
				if ((cx + (cw / 2)) >= (lx + lw))
					evas_textblock_cursor_line_last(cursor);
				else
					evas_textblock_cursor_line_first(cursor);
			}
		}

	}
	byte_idx = ewl_text_textblock_cursor_to_index(cursor);
	cur_char_idx = 0;
	ewl_text_byte_to_char(t, byte_idx, 0, &cur_char_idx, NULL);

	DRETURN_INT(cur_char_idx, DLEVEL_STABLE);
}

/**
 * @param t: The Ewl_Widget to set the font into
 * @param font: The font to set
 * @return Returns no value
 * @brief This will set the current font to be used when we insert more text
 */
void
ewl_text_font_set(Ewl_Text *t, const char *font)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("t", t);
	DCHECK_TYPE("t", t, EWL_TEXT_TYPE);

	ewl_text_font_source_set(t, NULL, font);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param t: The Ewl_Text to set the font too
 * @param font: The font to set
 * @param char_len: The distance to set the font over
 * @return Returns no value
 * @brief This will apply the specfied @a font from the current cursor position to
 * the length specified
 */
void
ewl_text_font_apply(Ewl_Text *t, const char *font, unsigned int char_len)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("t", t);
	DCHECK_TYPE("t", t, EWL_TEXT_TYPE);

	ewl_text_font_source_apply(t, NULL, font, char_len);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param t: The Ewl_Text to get the font from
 * @param char_idx: The index to get the font at
 * @return Returns no value
 * @brief This will retrive the font used at the specified index in the text
 */
char *
ewl_text_font_get(Ewl_Text *t, unsigned int char_idx)
{
	Ewl_Text_Fmt *fmt;
	char *font = NULL;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("t", t, NULL);
	DCHECK_TYPE_RET("t", t, EWL_TEXT_TYPE, NULL);

	fmt = ewl_text_fmt_get(t, char_idx);
	if (fmt && fmt->tx && fmt->tx->font)
		font = strdup(fmt->tx->font);

	DRETURN_PTR(font, DLEVEL_STABLE);
}

/**
 * @param t: The Ewl_Widget to set the font into
 * @param source: The font source to set
 * @param font: The font to set
 * @return Returns no value
 * @brief This will set the current font to be used when we insert more text
 */
void
ewl_text_font_source_set(Ewl_Text *t, const char *source, const char *font)
{
	Ewl_Text_Context *change;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("t", t);
	DCHECK_TYPE("t", t, EWL_TEXT_TYPE);

	change = ewl_text_context_new();
	if (source) change->font_source = strdup(source);

	/* null font will go back to the theme default */
	if (!font) change->font = ewl_theme_data_str_get(EWL_WIDGET(t), "font");
	else change->font = strdup(font);

	ewl_text_current_fmt_set(t, EWL_TEXT_CONTEXT_MASK_FONT, change);
	ewl_text_context_release(change);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param t: The Ewl_Text to set the font too
 * @param source: The font souce
 * @param font: The font to set
 * @param char_len: The distance to set the font over
 * @return Returns no value
 * @brief This will apply the specfied @a font from the current cursor position to
 * the length specified
 */
void
ewl_text_font_source_apply(Ewl_Text *t, const char *source, const char *font, 
							unsigned int char_len)
{
	Ewl_Text_Context *tx;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("t", t);
	DCHECK_TYPE("t", t, EWL_TEXT_TYPE);

	/* if length is 0 we have nothing to do */
	if (char_len == 0) 
		DRETURN(DLEVEL_STABLE);

	tx = ewl_text_context_new();

	if (source) tx->font_source = strdup(source);

	/* null font will go back to the theme default */
	if (!font) tx->font = ewl_theme_data_str_get(EWL_WIDGET(t), "font");
	else tx->font = strdup(font);

	ewl_text_fmt_apply(t, EWL_TEXT_CONTEXT_MASK_FONT, tx,
					t->cursor_position, char_len);
	ewl_text_context_release(tx);
	t->dirty = TRUE;

	ewl_widget_configure(EWL_WIDGET(t));

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param t: The Ewl_Text to get the font from
 * @param char_idx: The index to get the font at
 * @return Returns no value
 * @brief This will retrive the font source used at the specified index in the text
 */
char *
ewl_text_font_source_get(Ewl_Text *t, unsigned int char_idx)
{
	Ewl_Text_Fmt *fmt;
	char *source = NULL;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("t", t, NULL);
	DCHECK_TYPE_RET("t", t, EWL_TEXT_TYPE, NULL);

	fmt = ewl_text_fmt_get(t, char_idx);
	if (fmt && fmt->tx && fmt->tx->font_source)
		source = strdup(fmt->tx->font_source);

	DRETURN_PTR(source, DLEVEL_STABLE);
}

/**
 * @param t: The Ewl_Text to set the size too
 * @param size: The size to set the font too
 * @return Returns no value
 * @brief Set the font size to use when inserting new text
 */
void
ewl_text_font_size_set(Ewl_Text *t, unsigned int size)
{
	Ewl_Text_Context *change;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("t", t);
	DCHECK_TYPE("t", t, EWL_TEXT_TYPE);

	change = ewl_text_context_new();
	change->size = size;

	ewl_text_current_fmt_set(t, EWL_TEXT_CONTEXT_MASK_SIZE, change);
	ewl_text_context_release(change);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param t: The Ewl_Text to set the size into
 * @param size: The size to set
 * @param char_len: Length of block to get the new size
 * @return Returns no value
 * @brief This will apply the font size to the text from the current cursor
 * position for the given length
 */
void
ewl_text_font_size_apply(Ewl_Text *t, unsigned int size, unsigned int char_len)
{
	Ewl_Text_Context *tx;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("t", t);
	DCHECK_TYPE("t", t, EWL_TEXT_TYPE);

	if (char_len == 0)
		DRETURN(DLEVEL_STABLE);

	tx = ewl_text_context_new();
	tx->size = size;
	ewl_text_fmt_apply(t, EWL_TEXT_CONTEXT_MASK_SIZE, tx,
					t->cursor_position, char_len);
	ewl_text_context_release(tx);
	t->dirty = TRUE;

	ewl_widget_configure(EWL_WIDGET(t));

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param t: The Ewl_Text to get the size from
 * @param char_idx: The index you want to get the size for
 * @return Returns no value
 * @brief Retrieve the font size at the given index
 */
unsigned int
ewl_text_font_size_get(Ewl_Text *t, unsigned int char_idx)
{
	Ewl_Text_Fmt *fmt;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("t", t, 0);
	DCHECK_TYPE_RET("t", t, EWL_TEXT_TYPE, 0);

	fmt = ewl_text_fmt_get(t, char_idx);

	DRETURN_INT(((fmt && fmt->tx) ? fmt->tx->size : 0), DLEVEL_STABLE);
}

/**
 * @param t: The Ewl_Text to set the colour on
 * @param r: The red value
 * @param g: The green value
 * @param b: The blue value
 * @param a: The alpha value
 * @return Returns no value
 * @brief Set the text colour at the cursor
 */
void
ewl_text_color_set(Ewl_Text *t, unsigned int r, unsigned int g, 
				unsigned int b, unsigned int a)
{
	Ewl_Text_Context *change;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("t", t);
	DCHECK_TYPE("t", t, EWL_TEXT_TYPE);

	change = ewl_text_context_new();
	change->color.r = r;
	change->color.g = g;
	change->color.b = b;
	change->color.a = a;

	ewl_text_current_fmt_set(t, EWL_TEXT_CONTEXT_MASK_COLOR, change);
	ewl_text_context_release(change);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param t: The Ewl_Text to set the colour into
 * @param r: The red value to set
 * @param g: The green value to set
 * @param b: The blue value to set
 * @param a: The alpha value to set
 * @param char_len: The length of text to apply the colour over
 * @return Returns no value
 * @brief This will set the given colour from the current cursor position for the
 * specified length
 */
void 
ewl_text_color_apply(Ewl_Text *t, unsigned int r, unsigned int g,
				unsigned int b, unsigned int a, 
				unsigned int char_len)
{
	Ewl_Text_Context *tx;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("t", t);
	DCHECK_TYPE("t", t, EWL_TEXT_TYPE);

	if (char_len == 0)
		DRETURN(DLEVEL_STABLE);

	tx = ewl_text_context_new();
	tx->color.r = r;
	tx->color.g = g;
	tx->color.b = b;
	tx->color.a = a;

	ewl_text_fmt_apply(t, EWL_TEXT_CONTEXT_MASK_COLOR, tx,
					t->cursor_position, char_len);
	ewl_text_context_release(tx);
	t->dirty = TRUE;

	ewl_widget_configure(EWL_WIDGET(t));

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param t: The Ewl_Text to get the colour from
 * @param r: Where to put the red value
 * @param g: Where to put the green value
 * @param b: Where to put the blue value
 * @param a: Where to put the alpha value
 * @param char_idx: The index to get the colour from 
 * @return Returns no value
 * @brief Retrives the text colour at the given index
 */
void
ewl_text_color_get(Ewl_Text *t, unsigned int *r, unsigned int *g,
				unsigned int *b, unsigned int *a,
				unsigned int char_idx)
{
	Ewl_Text_Fmt *fmt;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("t", t);
	DCHECK_TYPE("t", t, EWL_TEXT_TYPE);

	fmt = ewl_text_fmt_get(t, char_idx);
	if (fmt && fmt->tx)
	{
		if (r) *r = fmt->tx->color.r;
		if (g) *g = fmt->tx->color.g;
		if (b) *b = fmt->tx->color.b;
		if (a) *a = fmt->tx->color.a;
	}

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param t: The Ewl_Text ot set the alignment into
 * @param align: The alignment to set
 * @return Returns no value
 * @brief Set the current alignment value of the text
 */
void
ewl_text_align_set(Ewl_Text *t, unsigned int align)
{
	Ewl_Text_Context *change;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("t", t);
	DCHECK_TYPE("t", t, EWL_TEXT_TYPE);

	change = ewl_text_context_new();
	change->align = align;

	ewl_text_current_fmt_set(t, EWL_TEXT_CONTEXT_MASK_ALIGN, change);
	ewl_text_context_release(change);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param t: The Ewl_Text to apply the alignment too
 * @param align: The alignment to apply
 * @param char_len: The length to apply the alignment for
 * @return Returns no value
 * @brief This will set the given alignment from the current cursor position for
 * the given length of text
 */
void
ewl_text_align_apply(Ewl_Text *t, unsigned int align, unsigned int char_len)
{
	Ewl_Text_Context *tx;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("t", t);
	DCHECK_TYPE("t", t, EWL_TEXT_TYPE);

	if (char_len == 0)
		DRETURN(DLEVEL_STABLE);

	tx = ewl_text_context_new();
	tx->align = align;
	ewl_text_fmt_apply(t, EWL_TEXT_CONTEXT_MASK_ALIGN, tx,
					t->cursor_position, char_len);
	ewl_text_context_release(tx);
	t->dirty = TRUE;

	ewl_widget_configure(EWL_WIDGET(t));

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param t: The Ewl_Text to get the alignment from
 * @param char_idx: The index to get the alignment from
 * @return Returns the current text alignment value
 * @brief Retrieves the alignment value from the given index
 */
unsigned int
ewl_text_align_get(Ewl_Text *t, unsigned int char_idx)
{
	Ewl_Text_Fmt *fmt;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("t", t, 0);
	DCHECK_TYPE_RET("t", t, EWL_TEXT_TYPE, 0);

	fmt = ewl_text_fmt_get(t, char_idx);

	DRETURN_INT(((fmt && fmt->tx) ? fmt->tx->align : 0), DLEVEL_STABLE);
}

/**
 * @param t: The Ewl_Text to set the style into
 * @param styles: The styles to set into the text
 * @return Returns no value
 * @brief Sets the given styles into the text at the cursor
 */
void
ewl_text_styles_set(Ewl_Text *t, unsigned int styles)
{
	Ewl_Text_Context *change;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("t", t);
	DCHECK_TYPE("t", t, EWL_TEXT_TYPE);

	change = ewl_text_context_new();
	change->styles = styles;

	ewl_text_current_fmt_set(t, EWL_TEXT_CONTEXT_MASK_STYLES, change);
	ewl_text_context_release(change);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param t: The Ewl_Text to apply the style too
 * @param styles: The styles to set into the text
 * @param char_len: The length of text to apply the style too
 * @return Returns no value
 * @brief This will set the given style from the current cursor position for the
 * given length of text
 */
void
ewl_text_styles_apply(Ewl_Text *t, unsigned int styles, unsigned int char_len)
{
	Ewl_Text_Context *tx;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("t", t);
	DCHECK_TYPE("t", t, EWL_TEXT_TYPE);

	if (char_len == 0)
		DRETURN(DLEVEL_STABLE);

	tx = ewl_text_context_new();
	tx->styles = styles;
	ewl_text_fmt_apply(t, EWL_TEXT_CONTEXT_MASK_STYLES, tx,
					t->cursor_position, char_len);
	ewl_text_context_release(tx);
	t->dirty = TRUE;

	ewl_widget_configure(EWL_WIDGET(t));

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param t: The text to add the style too
 * @param style: The style to add to the text
 * @param char_len: The lenght of text to add the style too
 * @return Returns no value
 * @brief This will add the given style to the text from the cursor up to length
 * characters
 */
void
ewl_text_style_add(Ewl_Text *t, Ewl_Text_Style style, unsigned int char_len)
{
	Ewl_Text_Fmt *fmt;
	unsigned int styles;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("t", t);
	DCHECK_TYPE("t", t, EWL_TEXT_TYPE);

	fmt = ewl_text_fmt_get(t, t->cursor_position);
	if (!fmt || !fmt->tx) DRETURN(DLEVEL_STABLE);

	styles = fmt->tx->styles;
	styles |= style;

	ewl_text_styles_apply(t, styles, char_len);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param t: The text to delete the style from
 * @param style: The style to delete from the text 
 * @param char_len: The lenght of text to delete the style from 
 * @return Returns no value
 * @brief This will delete the given style from the text starting at the cursor up 
 * to length characters
 */
void
ewl_text_style_del(Ewl_Text *t, Ewl_Text_Style style, unsigned int char_len)
{
	Ewl_Text_Fmt *fmt;
	unsigned int styles;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("t", t);
	DCHECK_TYPE("t", t, EWL_TEXT_TYPE);

	fmt = ewl_text_fmt_get(t, t->cursor_position);
	if (!fmt || !fmt->tx) DRETURN(DLEVEL_STABLE);

	styles = fmt->tx->styles;
	styles &= ~style;

	ewl_text_styles_apply(t, styles, char_len);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
 }

/**
 * @param t: The text to invert the style on
 * @param style: The style to invert in the text 
 * @param char_len: The lenght of text to invert the style on 
 * @return Returns no value
 * @brief This will invert the given style in the text starting at the cursor up 
 * to length characters
 */
void
ewl_text_style_invert(Ewl_Text *t, Ewl_Text_Style style, unsigned int char_len)
{
	Ewl_Text_Fmt *fmt;
	unsigned int styles;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("t", t);
	DCHECK_TYPE("t", t, EWL_TEXT_TYPE);

	fmt = ewl_text_fmt_get(t, t->cursor_position);
	if (!fmt || !fmt->tx) DRETURN(DLEVEL_STABLE);

	styles = fmt->tx->styles;
	styles ^= style;

	ewl_text_styles_apply(t, styles, char_len);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param t: The text to check for the style
 * @param style: The style to check for
 * @param char_idx: The index to check for the style
 * @return Returns no value
 * @brief Check if the given style is set at the given index in the text
 */
unsigned int
ewl_text_style_has(Ewl_Text *t, Ewl_Text_Style style, unsigned int char_idx)
{
	Ewl_Text_Fmt *fmt;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("t", t, FALSE);
	DCHECK_TYPE_RET("t", t, EWL_TEXT_TYPE, FALSE);

	fmt = ewl_text_fmt_get(t, char_idx);
	if (!fmt || !fmt->tx)
		DRETURN_INT(FALSE, DLEVEL_STABLE);

	DRETURN_INT((fmt->tx->styles & style), DLEVEL_STABLE);
}

/**
 * @param t: The Ewl_Text to get the style from
 * @param char_idx: The index to get the style from
 * @return Get the styles set at the given index in the text
 * @brief Retrives the styles in use at the given index
 */
unsigned int
ewl_text_styles_get(Ewl_Text *t, unsigned int char_idx)
{
	Ewl_Text_Fmt *fmt;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("t", t, 0);
	DCHECK_TYPE_RET("t", t, EWL_TEXT_TYPE, 0);

	fmt = ewl_text_fmt_get(t, char_idx);

	DRETURN_INT(((fmt && fmt->tx) ? fmt->tx->styles : 0), DLEVEL_STABLE);
}

/**
 * @param t: The Ewl_Text to set the wrap into
 * @param wrap: The wrap value to set
 * @return Returns no value
 * @brief Sets the wrap value of the text at the given index
 */
void
ewl_text_wrap_set(Ewl_Text *t, Ewl_Text_Wrap wrap)
{
	Ewl_Text_Context *change;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("t", t);
	DCHECK_TYPE("t", t, EWL_TEXT_TYPE);

	change = ewl_text_context_new();
	change->wrap = wrap;

	if (wrap == EWL_TEXT_WRAP_NONE)
		ewl_object_fill_policy_set(EWL_OBJECT(t), EWL_FLAG_FILL_HFILL 
								| EWL_FLAG_FILL_VFILL);
	else
		ewl_object_fill_policy_set(EWL_OBJECT(t), EWL_FLAG_FILL_HSHRINK 
								| EWL_FLAG_FILL_HFILL
								| EWL_FLAG_FILL_VFILL);

	ewl_text_current_fmt_set(t, EWL_TEXT_CONTEXT_MASK_WRAP, change);
	ewl_text_context_release(change);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param t: The Ewl_Text to apply the wrap value too
 * @param wrap: The wrap value to apply
 * @param char_len: The length of text to apply the wrap value over
 * @return Returns no value
 * @brief This will apply the given wrap value from the current cursor position for
 * the given length of text
 */
void
ewl_text_wrap_apply(Ewl_Text *t, Ewl_Text_Wrap wrap, unsigned int char_len)
{
	Ewl_Text_Context *tx;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("t", t);
	DCHECK_TYPE("t", t, EWL_TEXT_TYPE);

	if (char_len == 0)
		DRETURN(DLEVEL_STABLE);

	tx = ewl_text_context_new();
	tx->wrap = wrap;
	ewl_text_fmt_apply(t, EWL_TEXT_CONTEXT_MASK_WRAP, tx,
					t->cursor_position, char_len);
	ewl_text_context_release(tx);
	t->dirty = TRUE;

	ewl_widget_configure(EWL_WIDGET(t));

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param t: The Ewl_Text to get the wrap value for
 * @param char_idx: The index to get the wrap value from
 * @return Returns the wrap value of the text at the given index
 * @brief Retrives the text wrap value at the given index
 */
Ewl_Text_Wrap
ewl_text_wrap_get(Ewl_Text *t, unsigned int char_idx)
{
	Ewl_Text_Fmt *fmt;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("t", t, 0);
	DCHECK_TYPE_RET("t", t, EWL_TEXT_TYPE, 0);

	fmt = ewl_text_fmt_get(t, char_idx);

	DRETURN_INT(((fmt && fmt->tx) ? fmt->tx->wrap : 0), DLEVEL_STABLE);
}

/**
 * @param t: The Ewl_Text to set the text background colour of
 * @param r: The red value
 * @param g: The green value
 * @param b: The blue value
 * @param a: The alpha value
 * @return Returns no value
 * @brief Set the background colour at the cursor
 */
void
ewl_text_bg_color_set(Ewl_Text *t, unsigned int r, unsigned int g,
					unsigned int b, unsigned int a)
{
	Ewl_Text_Context *change;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("t", t);
	DCHECK_TYPE("t", t, EWL_TEXT_TYPE);

	change = ewl_text_context_new();
	change->style_colors.bg.r = r;
	change->style_colors.bg.g = g;
	change->style_colors.bg.b = b;
	change->style_colors.bg.a = a;

	ewl_text_current_fmt_set(t, EWL_TEXT_CONTEXT_MASK_BG_COLOR, change);
	ewl_text_context_release(change);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param t: The Ewl_Text to set the text background colour of
 * @param r: The red value
 * @param g: The green value
 * @param b: The blue value
 * @param a: The alpha value
 * @param char_len: The length of text to apply the bg colour over
 * @return Returns no value
 * @brief This will set the bg colour of the text from the current cursor position
 * to the given length.
 */
void
ewl_text_bg_color_apply(Ewl_Text *t, unsigned int r, unsigned int g,
					unsigned int b, unsigned int a,
					unsigned int char_len)
{
	Ewl_Text_Context *tx;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("t", t);
	DCHECK_TYPE("t", t, EWL_TEXT_TYPE);

	if (char_len == 0)
		DRETURN(DLEVEL_STABLE);

	tx = ewl_text_context_new();
	tx->style_colors.bg.r = r;
	tx->style_colors.bg.g = g;
	tx->style_colors.bg.b = b;
	tx->style_colors.bg.a = a;

	ewl_text_fmt_apply(t, EWL_TEXT_CONTEXT_MASK_BG_COLOR, tx,
					t->cursor_position, char_len);
	ewl_text_context_release(tx);
	t->dirty = TRUE;

	ewl_widget_configure(EWL_WIDGET(t));

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param t: The Ewl_Text to get the text background colour from
 * @param r: Where to put the red value
 * @param g: Where to put the green value
 * @param b: Where to put the blue value
 * @param a: Where to put the alpha value
 * @param char_idx: The index to get the colour from
 * @return Returns no value
 * @brief Get the text background colour at the given index
 */
void
ewl_text_bg_color_get(Ewl_Text *t, unsigned int *r, unsigned int *g,
					unsigned int *b, unsigned int *a,
					unsigned int char_idx)
{
	Ewl_Text_Fmt *fmt;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("t", t);
	DCHECK_TYPE("t", t, EWL_TEXT_TYPE);

	fmt = ewl_text_fmt_get(t, char_idx);
	if (fmt && fmt->tx)
	{
		if (r) *r = fmt->tx->style_colors.bg.r;
		if (g) *g = fmt->tx->style_colors.bg.g;
		if (b) *b = fmt->tx->style_colors.bg.b;
		if (a) *a = fmt->tx->style_colors.bg.a;
	}

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param t: The Ewl_Text to set the text glow colour of
 * @param r: The red value
 * @param g: The green value
 * @param b: The blue value
 * @param a: The alpha value
 * @return Returns no value
 * @brief Set the glow colour at the cursor
 */
void
ewl_text_glow_color_set(Ewl_Text *t, unsigned int r, unsigned int g,
					unsigned int b, unsigned int a)
{
	Ewl_Text_Context *change;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("t", t);
	DCHECK_TYPE("t", t, EWL_TEXT_TYPE);

	change = ewl_text_context_new();
	change->style_colors.glow.r = r;
	change->style_colors.glow.g = g;
	change->style_colors.glow.b = b;
	change->style_colors.glow.a = a;

	ewl_text_current_fmt_set(t, EWL_TEXT_CONTEXT_MASK_GLOW_COLOR, change);
	ewl_text_context_release(change);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param t: The Ewl_Text to set the text glow colour of
 * @param r: The red value
 * @param g: The green value
 * @param b: The blue value
 * @param a: The alpha value
 * @param char_len: The length of text to apply the glow colour over
 * @return Returns no value
 * @brief This will set the glow colour of the text from the current cursor position
 * to the given length.
 */
void
ewl_text_glow_color_apply(Ewl_Text *t, unsigned int r, unsigned int g,
					unsigned int b, unsigned int a,
					unsigned int char_len)
{
	Ewl_Text_Context *tx;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("t", t);
	DCHECK_TYPE("t", t, EWL_TEXT_TYPE);

	if (char_len == 0)
		DRETURN(DLEVEL_STABLE);

	tx = ewl_text_context_new();
	tx->style_colors.glow.r = r;
	tx->style_colors.glow.g = g;
	tx->style_colors.glow.b = b;
	tx->style_colors.glow.a = a;

	ewl_text_fmt_apply(t, EWL_TEXT_CONTEXT_MASK_GLOW_COLOR, tx,
						t->cursor_position, char_len);
	ewl_text_context_release(tx);
	t->dirty = TRUE;

	ewl_widget_configure(EWL_WIDGET(t));

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param t: The Ewl_Text to get the text glow colour from
 * @param r: Where to put the red value
 * @param g: Where to put the green value
 * @param b: Where to put the blue value
 * @param a: Where to put the alpha value
 * @param char_idx: The index to get the colour from
 * @return Returns no value
 * @brief Get the glow colour at the given index
 */
void
ewl_text_glow_color_get(Ewl_Text *t, unsigned int *r, unsigned int *g,
					unsigned int *b, unsigned int *a,
					unsigned int char_idx)
{
	Ewl_Text_Fmt *fmt;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("t", t);
	DCHECK_TYPE("t", t, EWL_TEXT_TYPE);

	fmt = ewl_text_fmt_get(t, char_idx);
	if (fmt && fmt->tx)
	{
		if (r) *r = fmt->tx->style_colors.glow.r;
		if (g) *g = fmt->tx->style_colors.glow.g;
		if (b) *b = fmt->tx->style_colors.glow.b;
		if (a) *a = fmt->tx->style_colors.glow.a;
	}

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param t: The Ewl_Text to set the text outline colour of
 * @param r: The red value
 * @param g: The green value
 * @param b: The blue value
 * @param a: The alpha value
 * @return Returns no value
 * @brief Set the outline colour at the cursor
 */
void
ewl_text_outline_color_set(Ewl_Text *t, unsigned int r, unsigned int g,
						unsigned int b, unsigned int a)
{
	Ewl_Text_Context *change;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("t", t);
	DCHECK_TYPE("t", t, EWL_TEXT_TYPE);

	change = ewl_text_context_new();
	change->style_colors.outline.r = r;
	change->style_colors.outline.g = g;
	change->style_colors.outline.b = b;
	change->style_colors.outline.a = a;

	ewl_text_current_fmt_set(t, EWL_TEXT_CONTEXT_MASK_OUTLINE_COLOR, change);
	ewl_text_context_release(change);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param t: The Ewl_Text to set the text outline colour of
 * @param r: The red value
 * @param g: The green value
 * @param b: The blue value
 * @param a: The alpha value
 * @param char_len: The length of text to apply the outline colour over
 * @return Returns no value
 * @brief This will set the outline colour of the text from the current cursor position
 * to the given length.
 */
void
ewl_text_outline_color_apply(Ewl_Text *t, unsigned int r, unsigned int g,
						unsigned int b, unsigned int a,
						unsigned int char_len)
{
	Ewl_Text_Context *tx;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("t", t);
	DCHECK_TYPE("t", t, EWL_TEXT_TYPE);

	if (char_len == 0)
		DRETURN(DLEVEL_STABLE);

	tx = ewl_text_context_new();
	tx->style_colors.outline.r = r;
	tx->style_colors.outline.g = g;
	tx->style_colors.outline.b = b;
	tx->style_colors.outline.a = a;

	ewl_text_fmt_apply(t, EWL_TEXT_CONTEXT_MASK_OUTLINE_COLOR, tx,
						t->cursor_position, char_len);
	ewl_text_context_release(tx);
	t->dirty = TRUE;

	ewl_widget_configure(EWL_WIDGET(t));

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param t: The Ewl_Text to get the text outline colour from
 * @param r: Where to put the red value
 * @param g: Where to put the green value
 * @param b: Where to put the blue value
 * @param a: Where to put the alpha value
 * @param char_idx: The index to get the colour from
 * @return Returns no value
 * @brief Get the outline colour at the given index
 */
void
ewl_text_outline_color_get(Ewl_Text *t, unsigned int *r, unsigned int *g,
						unsigned int *b, unsigned int *a,
						unsigned int char_idx)
{
	Ewl_Text_Fmt *fmt;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("t", t);
	DCHECK_TYPE("t", t, EWL_TEXT_TYPE);

	fmt = ewl_text_fmt_get(t, char_idx);
	if (fmt && fmt->tx)
	{
		if (r) *r = fmt->tx->style_colors.outline.r;
		if (g) *g = fmt->tx->style_colors.outline.g;
		if (b) *b = fmt->tx->style_colors.outline.b;
		if (a) *a = fmt->tx->style_colors.outline.a;
	}

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param t: The Ewl_Text to set the text shadow colour of
 * @param r: The red value
 * @param g: The green value
 * @param b: The blue value
 * @param a: The alpha value
 * @return Returns no value
 * @brief Set the shadow colour at the cursor
 */
void
ewl_text_shadow_color_set(Ewl_Text *t, unsigned int r, unsigned int g,
						unsigned int b, unsigned int a)
{
	Ewl_Text_Context *change;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("t", t);
	DCHECK_TYPE("t", t, EWL_TEXT_TYPE);

	change = ewl_text_context_new();
	change->style_colors.shadow.r = r;
	change->style_colors.shadow.g = g;
	change->style_colors.shadow.b = b;
	change->style_colors.shadow.a = a;

	ewl_text_current_fmt_set(t, EWL_TEXT_CONTEXT_MASK_SHADOW_COLOR, change);
	ewl_text_context_release(change);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param t: The Ewl_Text to set the text shadow colour of
 * @param r: The red value
 * @param g: The green value
 * @param b: The blue value
 * @param a: The alpha value
 * @param char_len: The length of text to apply the shadow colour over
 * @return Returns no value
 * @brief This will set the shadow colour of the text from the current cursor position
 * to the given length.
 */
void
ewl_text_shadow_color_apply(Ewl_Text *t, unsigned int r, unsigned int g,
						unsigned int b, unsigned int a,
						unsigned int char_len)
{
	Ewl_Text_Context *tx;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("t", t);
	DCHECK_TYPE("t", t, EWL_TEXT_TYPE);

	if (char_len == 0)
		DRETURN(DLEVEL_STABLE);

	tx = ewl_text_context_new();
	tx->style_colors.shadow.r = r;
	tx->style_colors.shadow.g = g;
	tx->style_colors.shadow.b = b;
	tx->style_colors.shadow.a = a;

	ewl_text_fmt_apply(t, EWL_TEXT_CONTEXT_MASK_SHADOW_COLOR, tx,
						t->cursor_position, char_len);
	ewl_text_context_release(tx);
	t->dirty = TRUE;

	ewl_widget_configure(EWL_WIDGET(t));

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param t: The Ewl_Text to get the text shadow colour from
 * @param r: Where to put the red value
 * @param g: Where to put the green value
 * @param b: Where to put the blue value
 * @param a: Where to put the alpha value
 * @param char_idx: The index to get the colour from
 * @return Returns no value
 * @brief Retrieve the shadow colour at the given index
 */
void
ewl_text_shadow_color_get(Ewl_Text *t, unsigned int *r, unsigned int *g,
						unsigned int *b, unsigned int *a,
						unsigned int char_idx)
{
	Ewl_Text_Fmt *fmt;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("t", t);
	DCHECK_TYPE("t", t, EWL_TEXT_TYPE);

	fmt = ewl_text_fmt_get(t, char_idx);
	if (fmt && fmt->tx)
	{
		if (r) *r = fmt->tx->style_colors.shadow.r;
		if (g) *g = fmt->tx->style_colors.shadow.g;
		if (b) *b = fmt->tx->style_colors.shadow.b;
		if (a) *a = fmt->tx->style_colors.shadow.a;
	}

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param t: The Ewl_Text to set the text strikethrough colour of
 * @param r: The red value
 * @param g: The green value
 * @param b: The blue value
 * @param a: The alpha value
 * @return Returns no value
 * @brief Set the strikethrough colour at the cursor
 */
void
ewl_text_strikethrough_color_set(Ewl_Text *t, unsigned int r, unsigned int g,
						unsigned int b, unsigned int a)
{
	Ewl_Text_Context *change;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("t", t);
	DCHECK_TYPE("t", t, EWL_TEXT_TYPE);

	change = ewl_text_context_new();
	change->style_colors.strikethrough.r = r;
	change->style_colors.strikethrough.g = g;
	change->style_colors.strikethrough.b = b;
	change->style_colors.strikethrough.a = a;

	ewl_text_current_fmt_set(t, EWL_TEXT_CONTEXT_MASK_STRIKETHROUGH_COLOR, change);
	ewl_text_context_release(change);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param t: The Ewl_Text to set the text strikethrough colour of
 * @param r: The red value
 * @param g: The green value
 * @param b: The blue value
 * @param a: The alpha value
 * @param char_len: The length of text to apply the strikethrough colour over
 * @return Returns no value
 * @brief This will set the strikethrough colour of the text from the current cursor position
 * to the given length.
 */
void
ewl_text_strikethrough_color_apply(Ewl_Text *t, unsigned int r, unsigned int g,
						unsigned int b, unsigned int a,
						unsigned int char_len)
{
	Ewl_Text_Context *tx;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("t", t);
	DCHECK_TYPE("t", t, EWL_TEXT_TYPE);

	if (char_len == 0)
		DRETURN(DLEVEL_STABLE);

	tx = ewl_text_context_new();
	tx->style_colors.strikethrough.r = r;
	tx->style_colors.strikethrough.g = g;
	tx->style_colors.strikethrough.b = b;
	tx->style_colors.strikethrough.a = a;

	ewl_text_fmt_apply(t, EWL_TEXT_CONTEXT_MASK_STRIKETHROUGH_COLOR, tx,
						t->cursor_position, char_len);
	ewl_text_context_release(tx);
	t->dirty = TRUE;

	ewl_widget_configure(EWL_WIDGET(t));

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param t: The Ewl_Text to get the text strikethrough colour from
 * @param r: Where to put the red value
 * @param g: Where to put the green value
 * @param b: Where to put the blue value
 * @param a: Where to put the alpha value
 * @param char_idx: The index to get the colour from
 * @return Returns no value
 * @brief Retrieve the strikethrough colour at the given index
 */
void
ewl_text_strikethrough_color_get(Ewl_Text *t, unsigned int *r, unsigned int *g,
						unsigned int *b, unsigned int *a,
						unsigned int char_idx)
{
	Ewl_Text_Fmt *fmt;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("t", t);
	DCHECK_TYPE("t", t, EWL_TEXT_TYPE);

	fmt = ewl_text_fmt_get(t, char_idx);
	if (fmt && fmt->tx)
	{
		if (r) *r = fmt->tx->style_colors.strikethrough.r;
		if (g) *g = fmt->tx->style_colors.strikethrough.g;
		if (b) *b = fmt->tx->style_colors.strikethrough.b;
		if (a) *a = fmt->tx->style_colors.strikethrough.a;
	}

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param t: The Ewl_Text to set the text underline colour of
 * @param r: The red value
 * @param g: The green value
 * @param b: The blue value
 * @param a: The alpha value
 * @return Returns no value
 * @brief Set the underline colour at the cursor
 */
void
ewl_text_underline_color_set(Ewl_Text *t, unsigned int r, unsigned int g,
						unsigned int b, unsigned int a)
{
	Ewl_Text_Context *change;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("t", t);
	DCHECK_TYPE("t", t, EWL_TEXT_TYPE);

	change = ewl_text_context_new();
	change->style_colors.underline.r = r;
	change->style_colors.underline.g = g;
	change->style_colors.underline.b = b;
	change->style_colors.underline.a = a;

	ewl_text_current_fmt_set(t, EWL_TEXT_CONTEXT_MASK_UNDERLINE_COLOR, change);
	ewl_text_context_release(change);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param t: The Ewl_Text to set the text underline colour of
 * @param r: The red value
 * @param g: The green value
 * @param b: The blue value
 * @param a: The alpha value
 * @param char_len: The length of text to apply the underline colour over
 * @return Returns no value
 * @brief This will set the underline colour of the text from the current cursor position
 * to the given length.
 */
void
ewl_text_underline_color_apply(Ewl_Text *t, unsigned int r, unsigned int g,
						unsigned int b, unsigned int a,
						unsigned int char_len)
{
	Ewl_Text_Context *tx;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("t", t);
	DCHECK_TYPE("t", t, EWL_TEXT_TYPE);

	if (char_len == 0)
		DRETURN(DLEVEL_STABLE);

	tx = ewl_text_context_new();
	tx->style_colors.underline.r = r;
	tx->style_colors.underline.g = g;
	tx->style_colors.underline.b = b;
	tx->style_colors.underline.a = a;

	ewl_text_fmt_apply(t, EWL_TEXT_CONTEXT_MASK_UNDERLINE_COLOR, tx,
						t->cursor_position, char_len);
	ewl_text_context_release(tx);
	t->dirty = TRUE;

	ewl_widget_configure(EWL_WIDGET(t));

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param t: The Ewl_Text to get the text underline colour from
 * @param r: Where to put the red value
 * @param g: Where to put the green value
 * @param b: Where to put the blue value
 * @param a: Where to put the alpha value
 * @param char_idx: The index to get the colour from
 * @return Returns no value
 * @brief Retrieve the underline colour at the given index
 */
void
ewl_text_underline_color_get(Ewl_Text *t, unsigned int *r, unsigned int *g,
						unsigned int *b, unsigned int *a,
						unsigned int char_idx)
{
	Ewl_Text_Fmt *fmt;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("t", t);
	DCHECK_TYPE("t", t, EWL_TEXT_TYPE);

	fmt = ewl_text_fmt_get(t, char_idx);
	if (fmt && fmt->tx)
	{
		if (r) *r = fmt->tx->style_colors.outline.r;
		if (g) *g = fmt->tx->style_colors.outline.g;
		if (b) *b = fmt->tx->style_colors.outline.b;
		if (a) *a = fmt->tx->style_colors.outline.a;
	}

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param t: The Ewl_Text to set the text double underline colour of
 * @param r: The red value
 * @param g: The green value
 * @param b: The blue value
 * @param a: The alpha value
 * @return Returns no value
 * @brief Set the double underline colour at the cursor
 */
void
ewl_text_double_underline_color_set(Ewl_Text *t, unsigned int r, unsigned int g,
						unsigned int b, unsigned int a)
{
	Ewl_Text_Context *change;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("t", t);
	DCHECK_TYPE("t", t, EWL_TEXT_TYPE);

	change = ewl_text_context_new();
	change->style_colors.double_underline.r = r;
	change->style_colors.double_underline.g = g;
	change->style_colors.double_underline.b = b;
	change->style_colors.double_underline.a = a;

	ewl_text_current_fmt_set(t, EWL_TEXT_CONTEXT_MASK_DOUBLE_UNDERLINE_COLOR, change);
	ewl_text_context_release(change);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param t: The Ewl_Text to set the text double underline colour of
 * @param r: The red value
 * @param g: The green value
 * @param b: The blue value
 * @param a: The alpha value
 * @param char_len: The length of text to apply the double underline colour over
 * @return Returns no value
 * @brief This will set the double_underline colour of the text from the current cursor position
 * to the given length.
 */
void
ewl_text_double_underline_color_apply(Ewl_Text *t, unsigned int r, unsigned int g,
							unsigned int b, unsigned int a,
							unsigned int char_len)
{
	Ewl_Text_Context *tx;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("t", t);
	DCHECK_TYPE("t", t, EWL_TEXT_TYPE);

	if (char_len == 0)
		DRETURN(DLEVEL_STABLE);

	tx = ewl_text_context_new();
	tx->style_colors.double_underline.r = r;
	tx->style_colors.double_underline.g = g;
	tx->style_colors.double_underline.b = b;
	tx->style_colors.double_underline.a = a;

	ewl_text_fmt_apply(t, EWL_TEXT_CONTEXT_MASK_DOUBLE_UNDERLINE_COLOR, tx,
						t->cursor_position, char_len);
	ewl_text_context_release(tx);
	t->dirty = TRUE;

	ewl_widget_configure(EWL_WIDGET(t));

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param t: The Ewl_Text to get the text double underline colour from
 * @param r: Where to put the red value
 * @param g: Where to put the green value
 * @param b: Where to put the blue value
 * @param a: Where to put the alpha value
 * @param char_idx: The index to get the colour from
 * @return Returns no value
 * @brief Retrieve the double underline colour at the given index
 */
void
ewl_text_double_underline_color_get(Ewl_Text *t, unsigned int *r, unsigned int *g,
						unsigned int *b, unsigned int *a,
						unsigned int char_idx)
{
	Ewl_Text_Fmt *fmt;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("t", t);
	DCHECK_TYPE("t", t, EWL_TEXT_TYPE);

	fmt = ewl_text_fmt_get(t, char_idx);
	if (fmt && fmt->tx)
	{
		if (r) *r = fmt->tx->style_colors.double_underline.r;
		if (g) *g = fmt->tx->style_colors.double_underline.g;
		if (b) *b = fmt->tx->style_colors.double_underline.b;
		if (a) *a = fmt->tx->style_colors.double_underline.a;
	}

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param t: The text to dump the formatting from
 * @return Returns no value
 * @brief Prints out the formatting information for the given text object
 */
void
ewl_text_fmt_dump(Ewl_Text *t)
{
	Ewl_Text_Fmt *fmt, *cur_fmt;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("t", t);
	DCHECK_TYPE("t", t, EWL_TEXT_TYPE);

	cur_fmt = ecore_dlist_current(t->formatting.nodes);
	ecore_dlist_goto_first(t->formatting.nodes);
	while ((fmt = ecore_dlist_next(t->formatting.nodes)))
	{
		printf("%d chars, %d bytes\n", fmt->char_len, fmt->byte_len);

		if (fmt->tx == ewl_text_default_context)
			printf("    DEFAULT");
		else
			ewl_text_context_print(fmt->tx, "    ");

		printf("\n\n");
	}
	ecore_dlist_goto(t->formatting.nodes, cur_fmt);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/* This will determine the number of bytes to get to char_pos in the text
 * and, if needed will get the number of bytes between char_pos and 
 * char_pos + char_len 
 *
 * This _HAS_ to leave the list with the same current pointer is when it
 * started */
static void
ewl_text_char_to_byte(Ewl_Text *t, unsigned int char_idx, unsigned int char_len,
				unsigned int *byte_idx, unsigned int *byte_len)
{
	Ewl_Text_Fmt *current, *fmt = NULL;
	unsigned int char_count = 0, bidx = 0;
	int dir;
	void *(*move)(Ecore_DList *list);

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("t", t);
	DCHECK_TYPE("t", t, EWL_TEXT_TYPE);

	current = ecore_dlist_current(t->formatting.nodes);

	/*
	 * Select the closest search point, first node, last node or last
	 * accessed node. (XXX Just does current node at the moment)
	 */
	if (t->formatting.current.char_idx < char_idx)
	{
		char_count = t->formatting.current.char_idx;
		bidx = t->formatting.current.byte_idx;

		/* walk forward until we cover the given position */
		fmt = ecore_dlist_current(t->formatting.nodes);
		while ((fmt->char_len + char_count) < char_idx)
		{
			char_count += fmt->char_len;
			bidx += fmt->byte_len;

			ecore_dlist_next(t->formatting.nodes);
			fmt = ecore_dlist_current(t->formatting.nodes);
			if (!fmt) break;
		}

		dir = 1;
	}
	else
	{
		/* walk back until we're less then the given position */
		while (char_count > char_idx)
		{
			ecore_dlist_previous(t->formatting.nodes);
			fmt = ecore_dlist_current(t->formatting.nodes);
			if (!fmt) break;

			char_count -= fmt->char_len;
			bidx -= fmt->byte_len;
		}

		dir = -1;
	}

	/* we still need to count within this node */
	while (char_count < char_idx)
	{
		unsigned int bytes;

		ewl_text_text_next_char(t->text + bidx, &bytes);
		bidx += bytes;

		char_count ++;
	}

	if (byte_len)
	{
		if (char_len == 0)
			*byte_len = 0;
		else
		{
			char *txt;

			txt = t->text + bidx;
			char_count = 0;
			while (char_count < char_len)
			{
				unsigned int bytes;

				txt = ewl_text_text_next_char(txt, &bytes);
				*byte_len += bytes;

				char_count ++;
			}

		}

	}

	if (byte_idx) *byte_idx = bidx;

	/* restore the original list position */
	if (dir > 0) move = ecore_dlist_previous;
	else move = ecore_dlist_next;

	fmt = ecore_dlist_current(t->formatting.nodes);
	while (fmt != current)
	{
		move(t->formatting.nodes);
		fmt = ecore_dlist_current(t->formatting.nodes);
	}

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/* This will determine the number of chars to get to byte_idx in the text
 * and, if needed will get the number of chars between byte_idx and 
 * byte_idx + byte_len */
static void
ewl_text_byte_to_char(Ewl_Text *t, unsigned int byte_idx, unsigned int byte_len, 
			unsigned int *char_idx, unsigned int *char_len)
{
	Ewl_Text_Fmt *current, *fmt = NULL;
	unsigned int byte_count = 0, cidx = 0;
	int dir;
	void *(*move)(Ecore_DList *list);

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("t", t);
	DCHECK_TYPE("t", t, EWL_TEXT_TYPE);

	current = ecore_dlist_current(t->formatting.nodes);

	if (t->formatting.current.byte_idx < byte_idx)
	{
		byte_count = t->formatting.current.byte_idx;
		cidx = t->formatting.current.char_idx;

		/* walk forward until we cover the given position */
		fmt = ecore_dlist_current(t->formatting.nodes);
		while ((fmt->byte_len + byte_count) < byte_idx)
		{
			byte_count += fmt->byte_len;
			cidx += fmt->char_len;

			ecore_dlist_next(t->formatting.nodes);
			fmt = ecore_dlist_current(t->formatting.nodes);
			if (!fmt) break;
		}

		dir = 1;
	
		if (!fmt)
			ecore_dlist_goto_last(t->formatting.nodes);
	}
	else
	{
		/* walk back until we're less then the given position */
		fmt = ecore_dlist_current(t->formatting.nodes);
		while (byte_count > byte_idx)
		{
			ecore_dlist_previous(t->formatting.nodes);
			fmt = ecore_dlist_current(t->formatting.nodes);
			if (!fmt) break;

			byte_count -= fmt->byte_len;
			cidx -= fmt->char_len;
		}

		dir = -1;

		if (!fmt)
			ecore_dlist_goto_first(t->formatting.nodes);
	}

	/* we still need to count within this node */
	while (byte_count < byte_idx)
	{
		unsigned int bytes;

		ewl_text_text_next_char(t->text + byte_count, &bytes);
		byte_count += bytes;
		cidx ++;
	}

	if (char_len)
	{
		if (byte_len == 0)
			*char_len = 0;

		else
		{
			char *txt;

			txt = t->text + byte_idx;
			byte_count = 0;
			while (byte_count < byte_len)
			{
				unsigned int bytes;

				txt = ewl_text_text_next_char(txt, &bytes);
				byte_count += bytes;
				(*char_len) ++;
			}

		}

	}

	if (char_idx) *char_idx = cidx;

	/* restore the original list position */
	if (dir > 0) move = ecore_dlist_previous;
	else move = ecore_dlist_next;

	fmt = ecore_dlist_current(t->formatting.nodes);
	while (fmt != current)
	{
		move(t->formatting.nodes);
		fmt = ecore_dlist_current(t->formatting.nodes);
	}

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/*
 * This function checks if a given character is a utf character.
 * It only checks the first character in the string.
 */
static int
ewl_text_char_is_legal_utf8(const char *c)
{
	unsigned const char *t;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("c", c, FALSE);

	t = (unsigned const char *)c;
	if (!t) DRETURN_INT(FALSE, DLEVEL_STABLE);

	if (t[0] < 0x80)
	{
		/* 
		 * this a noraml 7-bit ASCII character
		 * -> legal utf8
		 */
		DRETURN_INT(TRUE, DLEVEL_STABLE);
	}

	switch (EWL_TEXT_CHAR_BYTE_LEN(t)) 
	{
		case 2:
			/* 2 byte */
			if ((t[1] & 0xc0) != 0x80)
				DRETURN_INT(FALSE, DLEVEL_STABLE);
			break;

		case 3:
			/* 3 byte */
			if (((t[1] & 0xc0) != 0x80)
					|| ((t[2] & 0xc0) != 0x80))
				DRETURN_INT(FALSE, DLEVEL_STABLE);
			break;

		case 4:
			/* 4 byte */
			if (((t[1] & 0xc0) != 0x80)
					|| ((t[2] & 0xc0) != 0x80)
					|| ((t[3] & 0xc0) != 0x80))
				DRETURN_INT(FALSE, DLEVEL_STABLE);
			break;

		default:
			/* 
			 * this is actually:
			 * case 1: 
			 * 	We already checked if it is a 7-bit ASCII character,
			 * 	so anything else with the length of 1 byte is not
			 * 	a valid utf8 character
			 * case 5: case 6:
			 * 	Although a character sequences of the length 5 or 6
			 * 	is possible it is not a legal utf8 character
			 */
			return FALSE;
	}

	DRETURN_INT(TRUE, DLEVEL_STABLE);
}

/*
 * This function return the next character of a utf string.
 * The text pointer should point on the leading byte of the
 * current character, otherwise it will return the adress of
 * the next byte. 
 */
static char *
ewl_text_text_next_char(const char *text, unsigned int *idx)
{
	int len;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("text", text, NULL);

	len = EWL_TEXT_CHAR_BYTE_LEN(text);
	if (idx) *idx = len;

	DRETURN_PTR(text + len, DLEVEL_STABLE);
}

static void
ewl_text_display(Ewl_Text *t)
{
	Evas_Coord w = 0, h = 0;
	Evas_Textblock_Cursor *cursor;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("t", t);
	DCHECK_TYPE("t", t, EWL_TEXT_TYPE);

	evas_object_textblock_clear(t->textblock);

	cursor = (Evas_Textblock_Cursor *)
		evas_object_textblock_cursor_get(t->textblock);
	evas_textblock_cursor_text_append(cursor, "");
	
	if (t->length.chars > 0)
	{
		Ewl_Text_Context *cur_tx;
		Ewl_Text_Fmt *cur_fmt;
		unsigned int cur_pos;

		/* save these so we can restore the list state */
		cur_pos = ewl_text_cursor_position_get(t);
		cur_tx = t->formatting.current.tx;
		cur_fmt = ecore_dlist_current(t->formatting.nodes);
		if (cur_tx) ewl_text_context_acquire(cur_tx);

		ewl_text_fmt_walk(t);

		ecore_dlist_goto(t->formatting.nodes, cur_fmt);
		ewl_text_cursor_position_set(t, cur_pos);
		t->formatting.current.tx = cur_tx;
	}

	evas_object_textblock_size_formatted_get(t->textblock, &w, &h);

	/* Fallback, just in case we hit a corner case */
	if (!h) h = 1;
	if (!w) w = 1;

	ewl_object_preferred_inner_size_set(EWL_OBJECT(t), (int)w, (int)h);
	t->dirty = FALSE;

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static void
ewl_text_fmt_walk(Ewl_Text *t)
{
	Ewl_Text_Fmt *fmt;
	unsigned int byte_idx = 0;
	char *ptr, tmp;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("t", t);
	DCHECK_TYPE("t", t, EWL_TEXT_TYPE);

	ecore_dlist_goto_first(t->formatting.nodes);
	while ((fmt = ecore_dlist_next(t->formatting.nodes)))
	{
		Evas_Textblock_Cursor *cursor;

		ewl_text_context_format_string_create(fmt->tx);

		/* Don't free this as it's suppost to be const ... */
		cursor = (Evas_Textblock_Cursor *)
				evas_object_textblock_cursor_get(t->textblock);

		/* only need to append if we aren't default */
		if (fmt->tx != ewl_text_default_context)
			evas_textblock_cursor_format_append(cursor,
							fmt->tx->format);

		ptr = t->text + byte_idx;
		tmp = *(ptr + fmt->byte_len);
		*(ptr + fmt->byte_len) = '\0';

		ewl_text_plaintext_parse(t->textblock, ptr);
		*(ptr + fmt->byte_len) = tmp;

		evas_textblock_cursor_format_append(cursor, "-");

		byte_idx += fmt->byte_len;
	}

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static void
ewl_text_plaintext_parse(Evas_Object *tb, char *txt)
{
	Evas_Textblock_Cursor *cursor;
	char *tmp;
	unsigned int idx;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("tb", tb);

	if (!txt) 
		DRETURN(DLEVEL_STABLE);

	/* we don't free this cursor as it is actually const
	 * Evas_Textblock_Cursor * and i'm casting it...  */
	cursor = (Evas_Textblock_Cursor *)evas_object_textblock_cursor_get(tb);
	for (tmp = txt; *tmp; tmp = ewl_text_text_next_char(tmp, &idx)) 
	{
		if (*tmp == '\n') 
		{
			*tmp = '\0';
			if (*txt) evas_textblock_cursor_text_append(cursor, txt);
			evas_textblock_cursor_format_append(cursor, "\n");
			*tmp = '\n';

			txt = ewl_text_text_next_char(tmp, &idx);
		}
		else if (*tmp == '\r' && *(tmp + 1) == '\n') 
		{
			*tmp = '\0';
			if (*txt) evas_textblock_cursor_text_append(cursor, txt);
			evas_textblock_cursor_format_append(cursor, "\n");
			*tmp = '\r';
			tmp = ewl_text_text_next_char(tmp, &idx);
			txt = ewl_text_text_next_char(tmp, &idx); // XXX was tmp + 2 is that right?
		}
		else if (*tmp == '\t') 
		{
			*tmp = '\0';
			if (*txt) evas_textblock_cursor_text_append(cursor, txt);
			evas_textblock_cursor_format_append(cursor, "\t");
			*tmp = '\t';
			txt = ewl_text_text_next_char(tmp, &idx);
		}
	}
	if (*txt) evas_textblock_cursor_text_append(cursor, txt);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/* This will give you the format string to pass to textblock based on the
 * context information. */ 
static void
ewl_text_context_format_string_create(Ewl_Text_Context *ctx)
{
	char *format, *t;
	int pos = 0, i;
	struct 
	{
		char *key;
		char *val;
		int free;
	} fmt[128];

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("ctx", ctx);

	/* only do this once if possible */
	if (ctx->format)
		DRETURN(DLEVEL_STABLE);

	/* create the style string */
	if (ctx->styles != EWL_TEXT_STYLE_NONE)
	{
		if ((ctx->styles & EWL_TEXT_STYLE_UNDERLINE) || 
				(ctx->styles & EWL_TEXT_STYLE_DOUBLE_UNDERLINE))
		{
			t = ((ctx->styles & EWL_TEXT_STYLE_UNDERLINE) ? "single" : "double");

			fmt[pos].key = "underline_color";
			fmt[pos].val = ewl_text_color_string_get(
					ctx->style_colors.underline.r,
					ctx->style_colors.underline.g,
					ctx->style_colors.underline.b,
					ctx->style_colors.underline.a);
			fmt[pos++].free = TRUE;

			if (ctx->styles & EWL_TEXT_STYLE_DOUBLE_UNDERLINE)
			{
				fmt[pos].key = "underline2_color";
				fmt[pos].val = ewl_text_color_string_get(
						ctx->style_colors.double_underline.r,
						ctx->style_colors.double_underline.g,
						ctx->style_colors.double_underline.b,
						ctx->style_colors.double_underline.a);
				fmt[pos++].free = TRUE;
			}
		}
		else t = "off";

		fmt[pos].key = "underline";
		fmt[pos].val = t;
		fmt[pos++].free = FALSE;

		if (ctx->styles & EWL_TEXT_STYLE_STRIKETHROUGH)
		{
			t = "on";

			fmt[pos].key = "strikethrough_color";
			fmt[pos].val = ewl_text_color_string_get(
					ctx->style_colors.strikethrough.r,
					ctx->style_colors.strikethrough.g,
					ctx->style_colors.strikethrough.b,
					ctx->style_colors.strikethrough.a);
			fmt[pos++].free = TRUE;
		}
		else t = "off";

		fmt[pos].key = "strikethrough";
		fmt[pos].val = t;
		fmt[pos++].free = FALSE;

		if ((ctx->styles & EWL_TEXT_STYLE_SHADOW) 
				|| (ctx->styles & EWL_TEXT_STYLE_SOFT_SHADOW)
				|| (ctx->styles & EWL_TEXT_STYLE_FAR_SHADOW)
				|| (ctx->styles & EWL_TEXT_STYLE_OUTLINE)
				|| (ctx->styles & EWL_TEXT_STYLE_GLOW))
		{
			fmt[pos].key = "shadow_color";
			fmt[pos].val = ewl_text_color_string_get(
					ctx->style_colors.shadow.r,
					ctx->style_colors.shadow.g,
					ctx->style_colors.shadow.b,
					ctx->style_colors.shadow.a);
			fmt[pos++].free = TRUE;

			if (ctx->styles & EWL_TEXT_STYLE_GLOW)
			{
				t = "glow";

				fmt[pos].key = "glow_color";
				fmt[pos].val = ewl_text_color_string_get(
						ctx->style_colors.glow.r,
						ctx->style_colors.glow.g,
						ctx->style_colors.glow.b,
						ctx->style_colors.glow.a);
				fmt[pos++].free = TRUE;
			}
			else if (ctx->styles & EWL_TEXT_STYLE_OUTLINE)
			{
				if (ctx->styles & EWL_TEXT_STYLE_SHADOW)
					t = "outline_shadow";
				else if (ctx->styles & EWL_TEXT_STYLE_SOFT_SHADOW)
					t = "outline_soft_shadow";
				else t = "outline";

				fmt[pos].key = "outline_color";
				fmt[pos].val = ewl_text_color_string_get(
						ctx->style_colors.outline.r,
						ctx->style_colors.outline.g,
						ctx->style_colors.outline.b,
						ctx->style_colors.outline.a);
				fmt[pos++].free = TRUE;
			}
			else if (ctx->styles & EWL_TEXT_STYLE_SHADOW)
				t = "shadow";

			else if (ctx->styles & EWL_TEXT_STYLE_FAR_SHADOW)
			{
				if (ctx->styles & EWL_TEXT_STYLE_SOFT_SHADOW)
					t = "far_soft_shadow";
				else t = "far_shadow";
			}
			else if (ctx->styles & EWL_TEXT_STYLE_SOFT_SHADOW)
				t = "soft_shadow";
		}
		else t = "off";

		fmt[pos].key = "style";
		fmt[pos].val = t;
		fmt[pos++].free = FALSE;
	}
	else
	{
		fmt[pos].key = "underline";
		fmt[pos].val = "off";
		fmt[pos++].free = FALSE;

		fmt[pos].key = "strikethrough";
		fmt[pos].val = "off";
		fmt[pos++].free = FALSE;

		fmt[pos].key = "style";
		fmt[pos].val = "off";
		fmt[pos++].free = FALSE;
	}

	/* create the alignment string */
	if (ctx->align == EWL_FLAG_ALIGN_CENTER) t = "center";
	else if (ctx->align == EWL_FLAG_ALIGN_RIGHT) t = "right";
	else t = "left";

	fmt[pos].key = "align";
	fmt[pos].val = t;
	fmt[pos++].free = FALSE;

	if (ctx->wrap == EWL_TEXT_WRAP_WORD) t = "word";
	else if (ctx->wrap == EWL_TEXT_WRAP_CHAR) t = "char";
	else t = "off";

	fmt[pos].key = "wrap";
	fmt[pos].val = t;
	fmt[pos++].free = FALSE;

	t = NEW(char, 128);

	fmt[pos].key = "font_source";
	if (ctx->font_source)
	{
		fmt[pos].val = ctx->font_source;
		fmt[pos++].free = FALSE;

		t = strdup(ctx->font);
	}
	else
	{
		fmt[pos].val = (char *)ewl_theme_path_get();
		fmt[pos++].free = FALSE;

		snprintf(t, 128, "fonts/%s", ctx->font);
	}

	fmt[pos].key = "font";
	fmt[pos].val = t;
	fmt[pos++].free = TRUE;

	t = NEW(char, 5);
	snprintf(t, 5, "%d", ctx->size);
	fmt[pos].key = "font_size";
	fmt[pos].val = t;
	fmt[pos++].free = TRUE;

	fmt[pos].key = "backing_color";
	fmt[pos].val = ewl_text_color_string_get(
			ctx->style_colors.bg.r, ctx->style_colors.bg.g,
			ctx->style_colors.bg.b, ctx->style_colors.bg.a);
	fmt[pos++].free = TRUE;

	fmt[pos].key = "color";
	fmt[pos].val = ewl_text_color_string_get(
			ctx->color.r, ctx->color.g,
			ctx->color.b, ctx->color.a);
	fmt[pos++].free = TRUE;

	/* create the formatting string */
	format = NEW(char, 2048);
	strcat(format, "+");

	for (i = 0; i < pos; i ++)
	{
		strcat(format, fmt[i].key);
		strcat(format, "=");
		strcat(format, fmt[i].val);
		strcat(format, " ");

		if (fmt[i].free) FREE(fmt[i].val);
	}

	ctx->format = ecore_string_instance(format);
	FREE(format);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static char *
ewl_text_color_string_get(int r, int g, int b, int a)
{
	char buf[10];

	DENTER_FUNCTION(DLEVEL_STABLE);

	snprintf(buf, sizeof(buf), "#%02x%02x%02x%02x", r, g, b, a);

	DRETURN_PTR(strdup(buf), DLEVEL_STABLE);
}

/* This will give you a cursor into the textblock setup for your given
 * character index. You _MUST_ call evas_textblock_cursor_free(cursor) 
 * on this object so it won't leak */
static Evas_Textblock_Cursor *
ewl_text_textblock_cursor_position(Ewl_Text *t, unsigned int char_idx)
{
	Evas_Textblock_Cursor *cursor;
	unsigned int cur_char_idx = 0;
	const char *txt;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("t", t, NULL);
	DCHECK_TYPE_RET("t", t, EWL_TEXT_TYPE, NULL);

	/* place the cursor at the first node in the textblock */
	cursor = evas_object_textblock_cursor_new(t->textblock);
	evas_textblock_cursor_node_first(cursor);

	while(1)
	{
		txt = evas_textblock_cursor_node_format_get(cursor);

		/* if we have text this is a formatting node, need to see if
		 * this is a \n or \t as they are special */
		if (txt)
		{
			/* do we have a \n or \t node? */
			if ((!strcmp(txt, "\n")) || (!strcmp(txt, "\t")))
			{
				/* will this push us past the end? */
				if ((cur_char_idx + 1) > char_idx)
				{
					evas_textblock_cursor_pos_set(cursor, 
							char_idx - cur_char_idx);
					break;
				}
				else
					cur_char_idx ++;
			}
		}
		else
		{
			int pos;

			/* this is a text node, so check the length of the
			 * text against our current position and the idx we
			 * are looking for */
			pos = evas_textblock_cursor_node_text_length_get(cursor); 

			/* if this would move us past our index, find the
			 * difference between our desired index and the
			 * current index and set that */
			if ((cur_char_idx + pos) > char_idx)
			{
				evas_textblock_cursor_pos_set(cursor, 
							char_idx - cur_char_idx);
				break;
			}
			cur_char_idx += pos;
		}

		/* if we fail to goto the next node, just assume we're at
		 * the end of the text and jump the cursor there */
		if (!evas_textblock_cursor_node_next(cursor))
		{
			evas_textblock_cursor_node_last(cursor);
			evas_textblock_cursor_char_last(cursor);
			break;
		}

		/* This shouldn't happen, we've moved past our index. Just
		 * checking so the loop isn't (hopefully) infinite */
		if (cur_char_idx > char_idx)
		{
			DWARNING("This shouldn't happen, breaking loop.");
			break;
		}
	}

	DRETURN_PTR(cursor, DLEVEL_STABLE);
}

static unsigned int
ewl_text_textblock_cursor_to_index(Evas_Textblock_Cursor *cursor)
{
	unsigned int char_idx = 0;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("cursor", cursor, 0);

	/* this gives the index inside the _node_ the cursor points to, we
	 * then need to add the length of all the nodes before it plus any
	 * formatting nodes that are \n or \t */
	char_idx = evas_textblock_cursor_pos_get(cursor);
	while (evas_textblock_cursor_node_prev(cursor))
	{
		const char *txt;

		txt = evas_textblock_cursor_node_format_get(cursor);
		if (!txt) char_idx += evas_textblock_cursor_node_text_length_get(cursor);
		else if (!strcmp(txt, "\n")) char_idx ++;
		else if (!strcmp(txt, "\t")) char_idx ++;
	}

	DRETURN_INT(char_idx, DLEVEL_STABLE);
}

/**
 * @internal
 * @param w: The widget to work with
 * @param ev: UNUSED
 * @param data: UNUSED
 * @return Returns no value
 * @brief The configure callback
 */
void
ewl_text_cb_configure(Ewl_Widget *w, void *ev __UNUSED__, 
					void *data __UNUSED__)
{
	Ewl_Text *t;
	int xx, yy, ww, hh;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);
	DCHECK_TYPE("w", w, EWL_WIDGET_TYPE);

	/* don't do anything if we're obscured */
	if (OBSCURED(w)) 
		DRETURN(DLEVEL_STABLE);

	t = EWL_TEXT(w);

	xx = CURRENT_X(w);
	yy = CURRENT_Y(w);
	hh = CURRENT_H(w);
	ww = CURRENT_W(w);

	if (t->textblock)
	{
		evas_object_move(t->textblock, xx + t->offset.x,
						 yy + t->offset.y);
		evas_object_resize(t->textblock, ww - t->offset.x,
						hh - t->offset.y);

		if (t->dirty) ewl_text_display(t);

		/* XXX ewl_text_triggers_realize here? */
		ewl_text_triggers_configure(t);

		/* re-configure the selection to make sure it resizes
		 * if needed */
		if (t->selection)
			ewl_widget_configure(EWL_WIDGET(t->selection));
	}

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @internal
 * @param w: The widget to work with
 * @param ev: UNUSED
 * @param data: UNUSED
 * @return Returns no value
 * @brief The reveal callback
 */
void
ewl_text_cb_reveal(Ewl_Widget *w, void *ev __UNUSED__, void *data __UNUSED__)
{
	Ewl_Text *t;
	Ewl_Embed *emb;
	Ewl_Text_Context *ctx;
	Evas_Textblock_Style *st;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);
	DCHECK_TYPE("w", w, EWL_WIDGET_TYPE);

	t = EWL_TEXT(w);

	if (t->textblock) 
	{
		DWARNING("We have a textblock when we shoudn't.");
		DRETURN(DLEVEL_STABLE);
	}

	/* find the embed so we know the evas */
	emb = ewl_embed_widget_find(w);
	if (!emb) DRETURN(DLEVEL_STABLE);

	/* create the textblock */
	t->textblock = ewl_embed_object_request(emb, "textblock");
	if (!t->textblock)
		t->textblock = evas_object_textblock_add(emb->evas);

	if (t->textblock) 
	{
		char *fmt2;
		int len;

		ctx = ewl_text_context_default_create(t);
		ewl_text_context_format_string_create(ctx);

		len = strlen(ctx->format) + 12;  /* 12 == DEFAULT='' + \n + \0 */
		fmt2 = NEW(char, len);
		snprintf(fmt2, len, "DEFAULT='%s'\n", ctx->format);

		st = evas_textblock_style_new();
		evas_textblock_style_set(st, fmt2);
		evas_object_textblock_style_set(t->textblock, st);
		evas_textblock_style_free(st);

		ewl_text_context_release(ctx);
		FREE(fmt2);

		if (w->fx_clip_box)
			evas_object_clip_set(t->textblock, w->fx_clip_box);

		evas_object_pass_events_set(t->textblock, 1);

		evas_object_smart_member_add(t->textblock, w->smart_object);
		if (w->fx_clip_box)
			evas_object_stack_below(t->textblock, w->fx_clip_box);

		ewl_text_display(t);
		evas_object_show(t->textblock);
	}

	ewl_text_triggers_realize(t);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @internal
 * @param w: The widget to work with
 * @param ev: UNUSED
 * @param data: UNUSED
 * @return Returns no value
 * @brief The obscure callback
 */
void
ewl_text_cb_obscure(Ewl_Widget *w, void *ev __UNUSED__, 
					void *data __UNUSED__)
{
	Ewl_Text *t;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);
	DCHECK_TYPE("w", w, EWL_WIDGET_TYPE);

	t = EWL_TEXT(w);

	if (t->textblock)
	{
		Ewl_Embed *emb;

		emb = ewl_embed_widget_find(w);
		evas_object_textblock_clear(t->textblock);
		ewl_embed_object_cache(emb, t->textblock);
		t->textblock = NULL;
	}

	ewl_text_triggers_unrealize(t);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @internal
 * @param w: The widget to work with
 * @param ev: UNUSED
 * @param data: UNUSED
 * @return Returns no value
 * @brief The show callback
 */
void
ewl_text_cb_show(Ewl_Widget *w, void *ev __UNUSED__, void *data __UNUSED__)
{
	Ewl_Text *t;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);
	DCHECK_TYPE("w", w, EWL_WIDGET_TYPE);

	t = EWL_TEXT(w);
	if (t->textblock)
	{
		evas_object_show(t->textblock);
		ewl_text_triggers_show(t);
	}

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @internal
 * @param w: The widget to work with
 * @param ev: UNUSED
 * @param data: UNUSED
 * @return Returns no value
 * @brief The hide callback
 */
void
ewl_text_cb_hide(Ewl_Widget *w, void *ev __UNUSED__, void *data __UNUSED__)
{
	Ewl_Text *t;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);
	DCHECK_TYPE("w", w, EWL_WIDGET_TYPE);

	t = EWL_TEXT(w);
	evas_object_hide(t->textblock);
	ewl_text_triggers_hide(t);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @internal
 * @param w: The widget to work with
 * @param ev: UNUSED
 * @param data: UNUSED
 * @return Returns no value
 * @brief The destroy callback
 */
void
ewl_text_cb_destroy(Ewl_Widget *w, void *ev __UNUSED__, void *data __UNUSED__)
{
	Ewl_Text *t;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);
	DCHECK_TYPE("w", w, EWL_WIDGET_TYPE);

	t = EWL_TEXT(w);

	/* Note, we don't explictly destroy the triggers or the selection
	 * because they will be cleared as children of the text widget
	 * itself */
	if (t->triggers)
	{
		ecore_list_destroy(t->triggers);
		t->triggers = NULL;
	}
	t->selection = NULL;

	ecore_dlist_destroy(t->formatting.nodes);
	t->formatting.nodes = NULL;
	t->formatting.current.tx = NULL;
	t->formatting.current.char_idx = 0;
	t->formatting.current.byte_idx = 0;

	IF_FREE(t->text);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @internal
 * @param w: The widget to work with
 * @param ev: The Ewl_Event_Mouse_Down
 * @param data: UNUSED
 * @return Returns no value
 * @brief The mouse down callback
 */
void
ewl_text_cb_mouse_down(Ewl_Widget *w, void *ev, void *data __UNUSED__)
{
	Ewl_Text *t;
	Ewl_Event_Mouse_Down *event;
	unsigned int char_idx = 0;
	unsigned int modifiers;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);
	DCHECK_TYPE("w", w, EWL_WIDGET_TYPE);

	event = ev;
	t = EWL_TEXT(w);

	ewl_callback_append(w, EWL_CALLBACK_MOUSE_MOVE,
				ewl_text_cb_mouse_move, NULL);

	if (!t->selection)
	{
		/* create the selection */
		t->selection = EWL_TEXT_TRIGGER(ewl_text_trigger_new(
					EWL_TEXT_TRIGGER_TYPE_SELECTION));
		ewl_container_child_append(EWL_CONTAINER(t), 
					EWL_WIDGET(t->selection));
		ewl_widget_internal_set(EWL_WIDGET(t->selection), TRUE);

		ewl_text_trigger_start_pos_set(t->selection, 0);
		ewl_text_trigger_length_set(t->selection, 0);
		t->selection->text_parent = t;

		ewl_widget_show(EWL_WIDGET(t->selection));
	}

	char_idx = ewl_text_coord_index_map(EWL_TEXT(w), event->x, event->y);
	modifiers = ewl_ev_modifiers_get();
	if (modifiers & EWL_KEY_MODIFIER_SHIFT)
		ewl_text_selection_select_to(t->selection, char_idx);
	else
	{
		ewl_widget_hide(EWL_WIDGET(t->selection));

		/* cleanup any old areas */
		if (t->selection->areas)
		{
			Ewl_Text_Trigger_Area *area;

			while ((area = ecore_list_remove_first(t->selection->areas)))
				ewl_widget_destroy(EWL_WIDGET(area));
		}
		ewl_widget_show(EWL_WIDGET(t->selection));

		ewl_text_trigger_start_pos_set(t->selection, char_idx);
		ewl_text_trigger_base_set(t->selection, char_idx);
		ewl_text_trigger_length_set(t->selection, 0);
	}		   
	t->in_select = TRUE;

	ewl_text_trigger_position(t, t->selection);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}	   

/**
 * @internal
 * @param w: The widget to work with
 * @param ev: The Ewl_Event_Mouse_Up data
 * @param data: UNUSED
 * @return Returns no value
 * @brief The mouse up callback
 */
void
ewl_text_cb_mouse_up(Ewl_Widget *w, void *ev, void *data __UNUSED__)
{
	Ewl_Text *t;
	Ewl_Event_Mouse_Up *event;
	unsigned int modifiers;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);
	DCHECK_TYPE("w", w, EWL_WIDGET_TYPE);

	event = ev;
	t = EWL_TEXT(w);

	if (!t->in_select)
		DRETURN(DLEVEL_STABLE);

	t->in_select = FALSE;
	ewl_callback_del(w, EWL_CALLBACK_MOUSE_MOVE, ewl_text_cb_mouse_move);

	modifiers = ewl_ev_modifiers_get();
	if (modifiers & EWL_KEY_MODIFIER_SHIFT)
	{
		unsigned int char_idx = 0;

		char_idx = ewl_text_coord_index_map(EWL_TEXT(w), event->x, event->y);
		ewl_text_selection_select_to(t->selection, char_idx);
	}
	ewl_text_trigger_position(t, t->selection);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @internal
 * @param w: The widget to work with
 * @param ev: The Ewl_Event_Mouse_Move data
 * @param data: UNUSED
 * @return Returns no value
 * @brief The mouse move callback
 */
void
ewl_text_cb_mouse_move(Ewl_Widget *w, void *ev, void *data __UNUSED__)
{
	Ewl_Text *t;
	Ewl_Event_Mouse_Move *event;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);
	DCHECK_TYPE("w", w, EWL_WIDGET_TYPE);

	event = ev;
	t = EWL_TEXT(w);

	if (t->in_select)
	{
		unsigned int char_idx = 0;

		char_idx = ewl_text_coord_index_map(EWL_TEXT(w), event->x, event->y);
		ewl_text_selection_select_to(t->selection, char_idx);
		ewl_text_trigger_position(t, t->selection);
	}

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @internal
 * @param c: The container to work with
 * @param w: The widget to work with
 * @return Returns no value
 * @brief The child add callback
 */
void
ewl_text_cb_child_add(Ewl_Container *c, Ewl_Widget *w)
{
	char *appearance;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("c", c);
	DCHECK_PARAM_PTR("w", w);
	DCHECK_TYPE("c", c, EWL_CONTAINER_TYPE);
	DCHECK_TYPE("w", w, EWL_WIDGET_TYPE);

	if (!(appearance = ewl_widget_appearance_get(w))) 
		DRETURN(DLEVEL_STABLE);

	/* if this is a trigger then add it as such */
	if (!strcmp(appearance, EWL_TEXT_TRIGGER_TYPE))
		ewl_text_trigger_add(EWL_TEXT(c), EWL_TEXT_TRIGGER(w));

	FREE(appearance);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @internal
 * @param c: The container to work with
 * @param w: The widget to work with
 * @param idx: UNUSED
 * @return Returns no value
 * @brief The child del callback
 */
void
ewl_text_cb_child_del(Ewl_Container *c, Ewl_Widget *w, int idx __UNUSED__)
{
	char *appearance;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("c", c);
	DCHECK_PARAM_PTR("w", w);
	DCHECK_TYPE("c", c, EWL_CONTAINER_TYPE);
	DCHECK_TYPE("w", w, EWL_WIDGET_TYPE);

	if (!(appearance = ewl_widget_appearance_get(w)))
		DRETURN(DLEVEL_STABLE);

	/* if this is a trigger, remove it as such */
	if (!strcmp(appearance, EWL_TEXT_TRIGGER_TYPE))
		ewl_text_trigger_del(EWL_TEXT(c), EWL_TEXT_TRIGGER(w));

	FREE(appearance);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static Ewl_Text_Fmt *
ewl_text_fmt_get(Ewl_Text *t, unsigned int char_idx)
{
	Ewl_Text_Fmt *fmt;
	Ewl_Text_Context *cur_tx;
	unsigned int cur_idx = 0;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("t", t, NULL);
	DCHECK_TYPE_RET("t", t, EWL_TEXT_TYPE, NULL);

	/* save the current index and the current context */
	cur_idx = ewl_text_cursor_position_get(t);
	cur_tx = t->formatting.current.tx;
	if (cur_tx) ewl_text_context_acquire(cur_tx);

	ewl_text_cursor_position_set(t, char_idx);
	fmt = ecore_dlist_current(t->formatting.nodes);

	/* reset the current position and context */
	ewl_text_cursor_position_set(t, cur_idx);
	t->formatting.current.tx = cur_tx;

	DRETURN_PTR(fmt, DLEVEL_STABLE);
}

static void
ewl_text_current_fmt_set(Ewl_Text *t, unsigned int context_mask, 
				Ewl_Text_Context *change)
{
	Ewl_Text_Context *old, *new;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("t", t);
	DCHECK_PARAM_PTR("change", change);
	DCHECK_TYPE("t", t, EWL_TEXT_TYPE);

	/* if we've already made some formatting changes before setting the
	 * text make sure we use the current context. Else, use the current
	 * nodes context. */
	if (t->formatting.current.tx)
		old = t->formatting.current.tx;
	else
	{
		Ewl_Text_Fmt *fmt;

		fmt = ecore_dlist_current(t->formatting.nodes);
		old = fmt->tx;
	}

	new = ewl_text_context_find(old, context_mask, change);
	if (t->formatting.current.tx)
		ewl_text_context_release(t->formatting.current.tx);

	t->formatting.current.tx = new;

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static void
ewl_text_fmt_apply(Ewl_Text *t, unsigned int context_mask, 
			Ewl_Text_Context *change, unsigned int char_idx,
			unsigned int char_len)
{
	Ewl_Text_Fmt *fmt, *new;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("t", t);
	DCHECK_PARAM_PTR("change", change);
	DCHECK_TYPE("t", t, EWL_TEXT_TYPE);

	if ((char_len == 0) || (context_mask == EWL_TEXT_CONTEXT_MASK_NONE))
		DRETURN(DLEVEL_STABLE);

	/* XXX we actually move the cursor here, this might need 
	 * to be changed */
	ewl_text_cursor_position_set(t, char_idx);

	fmt = ecore_dlist_current(t->formatting.nodes);
	while (char_len > 0)
	{
		/* we've walked off the end of the list */
		if (!fmt)
		{
			DWARNING("Off end of formatting list (%d left).",
								char_len);
			break;
		}

		new = NEW(Ewl_Text_Fmt, 1);

		/* covers entire node */
		if ((char_idx == t->formatting.current.char_idx) 
				&& (fmt->char_len <= char_len))
		{
			Ewl_Text_Context *new_tx;

			new_tx = ewl_text_context_find(fmt->tx,
						context_mask, change);
			ewl_text_context_release(fmt->tx);
			fmt->tx = new_tx;

			char_idx += fmt->char_len;
			char_len -= fmt->char_len;
			ecore_dlist_next(t->formatting.nodes);
			t->formatting.current.char_idx += fmt->char_len;
			t->formatting.current.byte_idx += fmt->byte_len;

			FREE(new);
		}
		/* start is the same, node is longer then needed */
		else if (char_idx == t->formatting.current.char_idx)
		{
			unsigned int blen = 0;

			new->tx = ewl_text_context_find(fmt->tx,
						context_mask, change);
			new->char_len = char_len;
			ewl_text_char_to_byte(t,
				t->formatting.current.char_idx, 
				new->char_len, NULL, &blen);
			new->byte_len = blen;

			ecore_dlist_insert(t->formatting.nodes, new);

			fmt->char_len -= new->char_len;
			fmt->byte_len -= new->byte_len;

			ecore_dlist_next(t->formatting.nodes);
			t->formatting.current.char_idx += new->char_len;
			t->formatting.current.byte_idx += new->byte_len;

			char_idx += new->char_len;
			char_len -= new->char_len;
		}
		/* starts are different, need to set context on end of node */
		else
		{
			unsigned int blen = 0;

			new->tx = fmt->tx;

			new->char_len = char_idx -
				t->formatting.current.char_idx;
			ewl_text_char_to_byte(t,
				t->formatting.current.char_idx, 
				new->char_len, NULL, &blen);
			new->byte_len = blen;

			ecore_dlist_insert(t->formatting.nodes, new);
			ecore_dlist_next(t->formatting.nodes);

			t->formatting.current.char_idx += new->char_len;
			t->formatting.current.byte_idx += new->byte_len;

			fmt->tx = ewl_text_context_find(fmt->tx,
						context_mask, change);

			fmt->char_len -= new->char_len;
			fmt->byte_len -= new->byte_len;

			/* the rest of the node is covered */
			if (fmt->char_len <= char_len)
			{
				char_len -= fmt->char_len;
				char_idx += fmt->char_len;

				ecore_dlist_next(t->formatting.nodes);
				t->formatting.current.char_idx += fmt->char_len;
				t->formatting.current.byte_idx += fmt->byte_len;
			}
			/* we need to split the node */
			else
			{
				Ewl_Text_Context *tx;
				unsigned int blen = 0;

				tx = new->tx;

				new = NEW(Ewl_Text_Fmt, 1);
				new->tx = fmt->tx;
				new->char_len = char_len;
				ewl_text_char_to_byte(t, char_idx, char_len,
								NULL, &blen);
				new->byte_len = blen;

				ecore_dlist_insert(t->formatting.nodes, new);
				ecore_dlist_next(t->formatting.nodes);
				t->formatting.current.char_idx += new->char_len;
				t->formatting.current.byte_idx += new->byte_len;

				fmt->tx = tx;
				ewl_text_context_acquire(fmt->tx);
				fmt->char_len -= new->char_len;
				fmt->byte_len -= new->byte_len;

				char_len -= new->char_len;
				char_idx += new->char_len;
			}

		}
		fmt = ecore_dlist_current(t->formatting.nodes);
	}

	/* make sure we're still pointing at a node */
	if (!fmt) fmt = ecore_dlist_goto_last(t->formatting.nodes);

	/* put the cursor at the end of the formatted area */
	ewl_text_cursor_position_set(t, char_idx + char_len);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static void
ewl_text_text_insert_private(Ewl_Text *t, const char *txt, unsigned int char_idx,
			unsigned int *char_len, unsigned int *byte_len)
{
	unsigned int new_byte_len, clen = 0, blen = 0, bidx = 0;
	char *tmp, *ptr;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("t", t);
	DCHECK_TYPE("t", t, EWL_TEXT_TYPE);

	/* nothign to do if no text */
	if (!txt) DRETURN(DLEVEL_STABLE);

	/* count the number of chars in the text */
	tmp = (char *)txt;
	while (*tmp) 
	{
		if (ewl_text_char_is_legal_utf8(tmp)) 
			tmp = ewl_text_text_next_char(tmp, NULL);
		else
			tmp++;

		clen++;
	}
	blen = tmp - txt;

	new_byte_len = t->length.bytes + blen;
	if ((new_byte_len + 1) >= t->total_size)
	{
		int extend;

		/*
		 * Determine the size in blocks of EWL_TEXT_EXTEND_VAL
		 */
		extend = ((new_byte_len + 1) / EWL_TEXT_EXTEND_VAL);
		extend = (extend + 1) * EWL_TEXT_EXTEND_VAL;

		t->text = realloc(t->text, extend * sizeof(char));
		t->total_size = extend;
	}

	ewl_text_char_to_byte(t, char_idx, 0, &bidx, NULL);
	if (char_idx < t->length.chars)
		memmove(t->text + bidx + blen, t->text + bidx, 
					t->length.bytes - bidx);

	/* copy the text over, replace invalid UTF-8 chars */
	tmp = (char *)txt;
	ptr = t->text + bidx;
	while (*tmp) 
	{
		if (ewl_text_char_is_legal_utf8(tmp)) 
		{
			char *s;

			s = tmp;
			tmp = ewl_text_text_next_char(tmp, NULL);
			for ( ; s != tmp; s++, ptr++)
				*ptr = *s;
		}
		else 
		{
			*ptr = '?';
			tmp++;

			DWARNING("Found a non-UTF8 character.");
		}
	}

	/* update the text information */
	t->length.chars += clen;
	t->length.bytes += blen;
	t->text[t->length.bytes] = '\0';

	if (char_len) *char_len = clen;
	if (byte_len) *byte_len = blen;

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/*
 * Trigger stuff 
 */

/**
 * @param type: The type of trigger to create
 * @return Returns a new ewl_text_trigger widget
 * @brief Creates a new trigger for the text object
 */
Ewl_Text_Trigger *
ewl_text_trigger_new(Ewl_Text_Trigger_Type type)
{
	Ewl_Text_Trigger *trigger;

	DENTER_FUNCTION(DLEVEL_STABLE);

	trigger = NEW(Ewl_Text_Trigger, 1);
	if (!trigger)
		DRETURN_PTR(NULL, DLEVEL_STABLE);

	if (!ewl_text_trigger_init(trigger, type))
	{
		ewl_widget_destroy(EWL_WIDGET(trigger));
		DRETURN_PTR(NULL, DLEVEL_STABLE);
	}

	DRETURN_PTR(trigger, DLEVEL_STABLE);
}

/**
 * @param trigger: The trigger to initialize
 * @param type: The type of the triger
 * @return Returns TRUE if successful of FALSE otherwise
 * @brief Initializes a trigger to default values
 */
int
ewl_text_trigger_init(Ewl_Text_Trigger *trigger, Ewl_Text_Trigger_Type type)
{
	char *type_str;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("trigger", trigger, FALSE);

	if (type == EWL_TEXT_TRIGGER_TYPE_TRIGGER)
		type_str = EWL_TEXT_TRIGGER_TYPE;
	else if (type == EWL_TEXT_TRIGGER_TYPE_SELECTION)
		type_str = EWL_TEXT_SELECTION_TYPE;
	else
		DRETURN_INT(FALSE, DLEVEL_STABLE);

	if (!ewl_widget_init(EWL_WIDGET(trigger)))
		DRETURN_INT(FALSE, DLEVEL_STABLE);

	ewl_widget_appearance_set(EWL_WIDGET(trigger), type_str);
	ewl_widget_inherit(EWL_WIDGET(trigger), EWL_TEXT_TRIGGER_TYPE);

	ewl_callback_prepend(EWL_WIDGET(trigger), EWL_CALLBACK_DESTROY,
			ewl_text_trigger_cb_destroy, NULL);

	trigger->areas = ecore_list_new();
	trigger->type = type;

	ewl_widget_focusable_set(EWL_WIDGET(trigger), FALSE);

	/* XXX should these be internal? */
	ewl_widget_internal_set(EWL_WIDGET(trigger), TRUE);

	DRETURN_INT(TRUE, DLEVEL_STABLE);
}

/**
 * @internal
 * @param w: The widget to work with
 * @param ev_data: UNUSED
 * @param user_data: UNUSED
 * @return Returns no value
 * @brief The trigger destroy callback
 */
void
ewl_text_trigger_cb_destroy(Ewl_Widget *w, void *ev_data __UNUSED__, 
						void *user_data __UNUSED__)
{
	Ewl_Text_Trigger *t;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);
	DCHECK_TYPE("w", w, EWL_TEXT_TRIGGER_TYPE);

	t = EWL_TEXT_TRIGGER(w);

	if (t->areas)
	{
		Ewl_Text_Trigger_Area *area;

		while ((area = ecore_list_remove_first(t->areas)))
			ewl_widget_destroy(EWL_WIDGET(area));

		ecore_list_destroy(t->areas);
	}

	/* remove ourself from the parents trigger list, if needed */
	if ((t->text_parent) && (t->text_parent->triggers)
			&& (ecore_list_goto(t->text_parent->triggers, t)))
		ecore_list_remove(t->text_parent->triggers);

	t->text_parent = NULL;
	t->areas = NULL;

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param t: The trigger to work with
 * @return Returns the type of the trigger
 * @brief Retrieves the type of the trigger
 */
Ewl_Text_Trigger_Type 
ewl_text_trigger_type_get(Ewl_Text_Trigger *t)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("t", t, EWL_TEXT_TRIGGER_TYPE_NONE);
	DCHECK_TYPE_RET("t", t, EWL_TEXT_TRIGGER_TYPE, EWL_TEXT_TRIGGER_TYPE_NONE);

	DRETURN_INT(t->type, DLEVEL_STABLE);
}

/**
 * @param t: The trigger to work with
 * @param char_pos: The position to set
 * @return Returns no value
 * @brief Sets the start position of the trigger @a t to position @a pos
 */
void 
ewl_text_trigger_start_pos_set(Ewl_Text_Trigger *t, unsigned int char_pos)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("t", t);
	DCHECK_TYPE("t", t, EWL_TEXT_TRIGGER_TYPE);

	t->char_pos = char_pos;

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param t: The trigger to work with
 * @return Returns the current start position of the trigger
 * @brief Retrieves the start position of the trigger
 */
unsigned int
ewl_text_trigger_start_pos_get(Ewl_Text_Trigger *t)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("t", t, 0);
	DCHECK_TYPE_RET("t", t, EWL_TEXT_TRIGGER_TYPE, 0);

	DRETURN_INT(t->char_pos, DLEVEL_STABLE);
}

/**
 * @param t: The trigger to work with
 * @param char_len: The length to set on the cursor
 * @return Returns no value
 * @brief Sets the length @a len on the trigger @a t
 */
void
ewl_text_trigger_length_set(Ewl_Text_Trigger *t, unsigned int char_len)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("t", t);
	DCHECK_TYPE("t", t, EWL_TEXT_TRIGGER_TYPE);

	t->char_len = char_len;

	/* if the length is set to 0 remove the areas */
	if (char_len == 0)
	{
		if (t->areas)
		{
			Ewl_Text_Trigger_Area *area;
			while ((area = ecore_list_remove_first(t->areas)))
				ewl_widget_destroy(EWL_WIDGET(area));
		}
	}

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param t: The trigger to work with
 * @return Returns the length of the trigger
 * @brief Retrieves the length from the cursor @a t
 */
unsigned int
ewl_text_trigger_length_get(Ewl_Text_Trigger *t)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("t", t, 0);
	DCHECK_TYPE_RET("t", t, EWL_TEXT_TRIGGER_TYPE, 0);

	DRETURN_INT(t->char_len, DLEVEL_STABLE);
}

/**
 * @param t: The trigger to work with
 * @param char_pos: The position to set as the base for the cursor
 * @return Returns no value
 * @brief Sets the given position @a pos as the base for the trigger @a t
 */
void
ewl_text_trigger_base_set(Ewl_Text_Trigger *t, unsigned int char_pos)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("t", t);
	DCHECK_TYPE("t", t, EWL_TEXT_TRIGGER_TYPE);

	t->char_base = char_pos;

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param t: The trigger to work with
 * @return Returns the current base position of the cursor
 * @brief Retrieves the current base position of the cursor
 */
unsigned int
ewl_text_trigger_base_get(Ewl_Text_Trigger *t)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("t", t, 0);
	DCHECK_TYPE_RET("t", t, EWL_TEXT_TRIGGER_TYPE, 0);

	DRETURN_INT(t->char_base, DLEVEL_STABLE);
}

/**
 * @internal
 * @param t: The ewl_text widget
 * @return Returns no value
 * @brief Configures the position and size of all the triggers within the 
 * text widget @a t.
 */
void
ewl_text_triggers_configure(Ewl_Text *t)
{
	Ewl_Text_Trigger_Area *area;
	Ewl_Text_Trigger *cur;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("t", t);
	DCHECK_TYPE("t", t, EWL_TEXT_TYPE);

	if (t->triggers)
	{
		ecore_list_goto_first(t->triggers);
		while ((cur = ecore_list_next(t->triggers)))
		{
			if (!cur->areas) continue;

			ecore_list_goto_first(cur->areas);
			while ((area = ecore_list_next(cur->areas)))
				ewl_widget_configure(EWL_WIDGET(area));
		}
	}

	if (t->selection)
	{
		ecore_list_goto_first(t->selection->areas);
		while ((area = ecore_list_next(t->selection->areas)))
			ewl_widget_configure(EWL_WIDGET(area));
	}

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static void
ewl_text_trigger_area_add(Ewl_Text *t, Ewl_Text_Trigger *cur, 
					Evas_Coord x, Evas_Coord y, 
					Evas_Coord w, Evas_Coord h)
{
	Ewl_Widget *area;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("t", t);
	DCHECK_PARAM_PTR("cur", cur);
	DCHECK_TYPE("t", t, EWL_TEXT_TYPE);
	DCHECK_TYPE("cur", cur, EWL_TEXT_TRIGGER_TYPE);

	area = ewl_text_trigger_area_new(cur->type);
	ewl_container_child_append(EWL_CONTAINER(t), area);
	ewl_widget_internal_set(area, TRUE);
	ewl_object_geometry_request(EWL_OBJECT(area), x, y, w, h);

	ewl_callback_append(area, EWL_CALLBACK_MOUSE_IN, 
			ewl_text_trigger_cb_mouse_in, cur);
	ewl_callback_append(area, EWL_CALLBACK_MOUSE_OUT,
			ewl_text_trigger_cb_mouse_out, cur);
	ewl_callback_append(area, EWL_CALLBACK_MOUSE_DOWN,
			ewl_text_trigger_cb_mouse_down, cur);
	ewl_callback_append(area, EWL_CALLBACK_MOUSE_UP,
			ewl_text_trigger_cb_mouse_up, cur);
	ewl_widget_show(area);

	ecore_list_append(cur->areas, area);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static void
ewl_text_trigger_position(Ewl_Text *t, Ewl_Text_Trigger *trig)
{
	Evas_Textblock_Cursor *cur1, *cur2;
	Evas_List *rects;
	unsigned int byte_idx = 0, byte_len = 0;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("t", t);
	DCHECK_PARAM_PTR("trig", trig);
	DCHECK_TYPE("t", t, EWL_TEXT_TYPE);
	DCHECK_TYPE("trig", trig, EWL_TEXT_TRIGGER_TYPE);

	if (trig->char_len == 0) 
		DRETURN(DLEVEL_STABLE);

	/* clean out the old areas if needed */
	if (trig->areas) 
	{
		Ewl_Text_Trigger_Area *area;

		while ((area = ecore_list_remove_first(trig->areas)))
			ewl_widget_destroy(EWL_WIDGET(area));
	}
	else
		trig->areas = ecore_list_new();

	ewl_text_char_to_byte(t, trig->char_pos, trig->char_len - 1, 
						&byte_idx, &byte_len);

	cur1 = ewl_text_textblock_cursor_position(t, byte_idx);
	cur2 = ewl_text_textblock_cursor_position(t, byte_idx + byte_len);

	/* get all the rectangles and create areas with them */
	rects = evas_textblock_cursor_range_geometry_get(cur1, cur2);
	while (rects)
	{
		Evas_Textblock_Rectangle *tr;

		tr = rects->data;
		ewl_text_trigger_area_add(t, trig, tr->x + CURRENT_X(t), 
						tr->y + CURRENT_Y(t), 
						tr->w, tr->h);

		FREE(tr);
		rects = evas_list_remove_list(rects, rects);
	}
	evas_textblock_cursor_free(cur1);
	evas_textblock_cursor_free(cur2);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static void
ewl_text_triggers_remove(Ewl_Text *t)
{
	Ewl_Text_Trigger *trig;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("t", t);
	DCHECK_TYPE("t", t, EWL_TEXT_TYPE);

	if (!t->triggers) 
		DRETURN(DLEVEL_STABLE);

	while ((trig = ecore_list_remove_first(t->triggers))) 
	{
		trig->text_parent = NULL;
		ewl_widget_destroy(EWL_WIDGET(trig));
	}

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/* if we move the text (insertion, deleteion, etc) we need to shift the
 * position of the current cursors so they appear in the correct positions */
static void
ewl_text_triggers_shift(Ewl_Text *t, unsigned int char_pos, 
					unsigned int char_len,
					unsigned int del)
{
	Ewl_Text_Trigger *cur;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("t", t);
	DCHECK_TYPE("t", t, EWL_TEXT_TYPE);

	if (!t->triggers)
		DRETURN(DLEVEL_STABLE);

	ecore_list_goto_first(t->triggers);
	while ((cur = ecore_list_next(t->triggers)))
	{
		/* check if the change is after the trigger */
		if (char_pos >= (cur->char_pos + cur->char_len))
			continue;

		/* change is completely before the trigger */
		if ((char_pos + char_len) < cur->char_pos)
 		{
			if (del) cur->char_pos -= char_len;
			else cur->char_pos += char_len;
			continue;
		}

		if (del)
		{
			/* delete the entire trigger? */
			if ((char_pos <= cur->char_pos) && 
					((char_pos + char_len) >= 
					 	(cur->char_pos + cur->char_len)))
			{
				int index;

				index = ecore_list_index(t->triggers);
				if (index == 0)
				{
					DWARNING("is this possible?.");
				}
				else
				{
					index --;

					/* remove the node before the
					 * current one as _next will put us
					 * on the next node */
					ecore_list_goto_index(t->triggers, index);
					ecore_list_remove(t->triggers);
					ecore_list_goto_index(t->triggers, index);
				}
				continue;
			}

			/* delete part of the start of the trigger */
			if (char_pos <= cur->char_pos)
			{
				cur->char_len -= ((char_pos + char_len) - cur->char_pos);
				continue;
			}

			/* delete from the center of the trigger */
			if ((char_pos >= cur->char_pos) && 
					((char_pos + char_len) <= 
					 	(cur->char_pos + cur->char_len)))
			{
				cur->char_len -= char_len;
				continue;
			}

			/* must be deleted from the end of the trigger then */
			cur->char_len = char_pos - cur->char_pos;
		}
		else
		{
			/* we are inserting, just see if we are before */
			if (char_pos < cur->char_pos)
			{
				cur->char_pos += char_len;
				continue;
			}
			cur->char_len += char_len;
		}
  	}

 	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @internal
 * @param t: The ewl_text to work with
 * @return Returns no value
 * @brief Sets all of the triggers in the text @a t as realized
 */
void
ewl_text_triggers_realize(Ewl_Text *t)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("t", t);
	DCHECK_TYPE("t", t, EWL_TEXT_TYPE);

	if (t->triggers)
	{
		Ewl_Text_Trigger *cur;

		ecore_list_goto_first(t->triggers);
		while ((cur = ecore_list_next(t->triggers)))
			ewl_text_trigger_position(t, cur);
	}

	if (t->selection)
		ewl_text_trigger_position(t, t->selection);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @internal
 * @param t: The ewl_text to work with
 * @return Returns no value
 * @brief Sets all of the triggers in the text @a t as unrealized
 */
void
ewl_text_triggers_unrealize(Ewl_Text *t)
{
	Ewl_Text_Trigger *cur;
	Ewl_Text_Trigger_Area *area;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("t", t);
	DCHECK_TYPE("t", t, EWL_TEXT_TYPE);

	if (t->triggers)
	{
		ecore_list_goto_first(t->triggers);
		while ((cur = ecore_list_next(t->triggers)))
		{
			while ((area = ecore_list_remove_first(cur->areas)))
				ewl_widget_destroy(EWL_WIDGET(area));
		}
	}

	if (t->selection)
	{
		while ((area = ecore_list_remove_first(t->selection->areas)))
			ewl_widget_destroy(EWL_WIDGET(area));
	}

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @internal
 * @param t: The ewl_text to work with
 * @return Returns no value
 * @brief Shows all triggers in text @a t
 */
void
ewl_text_triggers_show(Ewl_Text *t)
{
	Ewl_Widget *area;
	Ewl_Text_Trigger *cur;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("t", t);
	DCHECK_TYPE("t", t, EWL_TEXT_TYPE);

	if (t->triggers)
	{
		ecore_list_goto_first(t->triggers);
		while ((cur = ecore_list_next(t->triggers)))
		{
			if (!cur->areas) continue;

			ecore_list_goto_first(cur->areas);
			while ((area = ecore_list_next(cur->areas)))
				ewl_widget_show(area);
		}
	}

	if (t->selection)
	{
		ecore_list_goto_first(t->selection->areas);
		while ((area = ecore_list_next(t->selection->areas)))
			ewl_widget_show(area);
	}

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @internal
 * @param t: The text to work with
 * @return Returns no value
 * @brief Hides all of the triggers in the text @a t
 */
void
ewl_text_triggers_hide(Ewl_Text *t)
{
	Ewl_Widget *area;
	Ewl_Text_Trigger *cur;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("t", t);
	DCHECK_TYPE("t", t, EWL_TEXT_TYPE);

	/* hide the triggers */
	if (t->triggers)
	{
		ecore_list_goto_first(t->triggers);
		while ((cur = ecore_list_next(t->triggers)))
		{
			if (!cur->areas) continue;

			ecore_list_goto_first(cur->areas);
			while ((area = ecore_list_next(cur->areas)))
				ewl_widget_hide(area);
		}
	}

	/* hide the selection */
	if (t->selection && t->selection->areas) 
	{
		ecore_list_goto_first(t->selection->areas);
		while ((area = ecore_list_next(t->selection->areas)))
			ewl_widget_hide(area);
	}

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static void
ewl_text_trigger_add(Ewl_Text *t, Ewl_Text_Trigger *trigger)
{
	Ewl_Text_Trigger *cur = NULL;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("t", t);
	DCHECK_PARAM_PTR("trigger", trigger);
	DCHECK_TYPE("t", t, EWL_TEXT_TYPE);
	DCHECK_TYPE("trigger", trigger, EWL_TEXT_TRIGGER_TYPE);

	/* create the trigger list if needed */
	if (!t->triggers)
		t->triggers = ecore_list_new();

	/* if we have no length, we start past the end of the text, or we
	 * extend past the end of the text then return an error */
	if ((trigger->char_len == 0) || (trigger->char_pos > t->length.chars)
			|| ((trigger->char_pos + trigger->char_len) > t->length.chars))
		DRETURN(DLEVEL_STABLE);

	trigger->text_parent = t;

	/* only need to check for overlappign if this is a trigger (not a
	 * selection) */
	if (trigger->type == EWL_TEXT_TRIGGER_TYPE_TRIGGER)
	{
		ecore_list_goto_first(t->triggers);
		while ((cur = ecore_list_next(t->triggers)))
		{
			if (trigger->char_pos < cur->char_pos)
			{
				if ((trigger->char_pos + trigger->char_len) < cur->char_pos)
					break;

				DWARNING("Overlapping triggers are not allowed.");
				DRETURN(DLEVEL_STABLE);
			}

			if ((trigger->char_pos > (cur->char_pos + cur->char_len)))
				continue;

			if ((trigger->char_pos >= cur->char_pos) 
						&& (trigger->char_pos <= (cur->char_pos + cur->char_len)))
			{
				DWARNING("Overlapping triggers are not allowed.");
				DRETURN(DLEVEL_STABLE);
			}
		}
	}

	if (cur)
	{
		/* we need to set our position to the one before the one we 
		 * are on because the _next call in the while will have
		 * advanced us to the next node, but we want to insert
	 	 * at the one before that */
		ecore_list_goto_index(t->triggers, ecore_list_index(t->triggers) - 1);
		ecore_list_insert(t->triggers, trigger);
	}
	else 
		ecore_list_append(t->triggers, trigger);

	DRETURN(DLEVEL_STABLE);
}

static void
ewl_text_trigger_del(Ewl_Text *t, Ewl_Text_Trigger *trigger)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("t", t);
	DCHECK_PARAM_PTR("trigger", trigger);
	DCHECK_TYPE("t", t, EWL_TEXT_TYPE);
	DCHECK_TYPE("trigger", trigger, EWL_TEXT_TRIGGER_TYPE);

	/* nothign to do if we have no triggers */
	if (!t->triggers)
		DRETURN(DLEVEL_STABLE);

	ecore_list_goto(t->triggers, trigger);
	ecore_list_remove(t->triggers);

	trigger->text_parent = NULL;

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @internal
 * @param w: UNUSED
 * @param ev: The event data
 * @param data: The Ewl_Text_Trigger
 * @return Returns no value
 * @brief The trigger mouse in callback
 */
void
ewl_text_trigger_cb_mouse_in(Ewl_Widget *w __UNUSED__, void *ev, void *data)
{
	Ewl_Text_Trigger *trigger;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("data", data);

	trigger = data;
	ewl_callback_call_with_event_data(EWL_WIDGET(trigger), 
						EWL_CALLBACK_MOUSE_IN, ev);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @internal
 * @param w: UNUSED
 * @param ev: The event data
 * @param data: The Ewl_Text_Trigger
 * @return Returns no value
 * @brief The trigger mouse out callback
 */
void
ewl_text_trigger_cb_mouse_out(Ewl_Widget *w __UNUSED__, void *ev, void *data)
{
	Ewl_Text_Trigger *trigger;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("data", data);

	trigger = data;
	ewl_callback_call_with_event_data(EWL_WIDGET(trigger), 
						EWL_CALLBACK_MOUSE_OUT, ev);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @internal
 * @param w: UNUSED
 * @param ev: The event data
 * @param data: The Ewl_Text_Trigger
 * @return Returns no value
 * @brief The trigger mouse up callback
 */
void
ewl_text_trigger_cb_mouse_up(Ewl_Widget *w __UNUSED__, void *ev, void *data)
{
	Ewl_Text_Trigger *trigger;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("data", data);

	trigger = data;
	ewl_callback_call_with_event_data(EWL_WIDGET(trigger),
						EWL_CALLBACK_MOUSE_UP, ev);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @internal
 * @param w: UNUSED
 * @param ev: The event data
 * @param data: The Ewl_Text_Trigger
 * @return Returns no value
 * @brief The trigger mouse down callback
 */
void
ewl_text_trigger_cb_mouse_down(Ewl_Widget *w __UNUSED__, void *ev, void *data)
{
	Ewl_Text_Trigger *trigger;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("data", data);

	trigger = data;
	ewl_callback_call_with_event_data(EWL_WIDGET(trigger), 
						EWL_CALLBACK_MOUSE_DOWN, ev);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/*
 * Ewl_Text_Trigger_Area stuff
 */

/**
 * @internal
 * @param type: The trigger area type to create
 * @return Returns a new trigger area of the given type
 * @brief Creates and returns a new trigger_area of the given type
 */
Ewl_Widget *
ewl_text_trigger_area_new(Ewl_Text_Trigger_Type type)
{
	Ewl_Text_Trigger_Area *area;

	DENTER_FUNCTION(DLEVEL_STABLE);

	area = NEW(Ewl_Text_Trigger_Area, 1);
	if (!area)
		DRETURN_PTR(NULL, DLEVEL_STABLE);

	if (!ewl_text_trigger_area_init(area, type))
	{
		ewl_widget_destroy(EWL_WIDGET(area));
		DRETURN_PTR(NULL, DLEVEL_STABLE);
	}

	DRETURN_PTR(area, DLEVEL_STABLE);
}

/**
 * @internal
 * @param area: The trigger area to initialize
 * @param type: The type of the trigger area
 * @return Returns TRUE on success or FALSE on failure
 * @brief Initializes a triggger area to default values
 */
int
ewl_text_trigger_area_init(Ewl_Text_Trigger_Area *area, 
				Ewl_Text_Trigger_Type type)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("area", area, FALSE);

	if (!ewl_widget_init(EWL_WIDGET(area)))
		DRETURN_INT(FALSE, DLEVEL_STABLE);

	ewl_widget_appearance_set(EWL_WIDGET(area),
			((type == EWL_TEXT_TRIGGER_TYPE_SELECTION) ?
			 "selection_area" : "trigger_area"));
	ewl_widget_inherit(EWL_WIDGET(area), "trigger_area");

	if (type == EWL_TEXT_TRIGGER_TYPE_TRIGGER)
		ewl_widget_color_set(EWL_WIDGET(area), 0, 0, 0, 0);

	ewl_widget_focusable_set(EWL_WIDGET(area), FALSE);
	ewl_widget_internal_set(EWL_WIDGET(area), TRUE);

	DRETURN_INT(TRUE, DLEVEL_STABLE);
}

/*
 * Selection stuff
 */
static void
ewl_text_selection_select_to(Ewl_Text_Trigger *s, unsigned int char_idx)
{
	unsigned int start_pos;
	unsigned int base;
	char *txt;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("s", s);
	DCHECK_TYPE("s", s, EWL_TEXT_TRIGGER_TYPE);

	base = ewl_text_trigger_base_get(s);
	start_pos = ewl_text_trigger_start_pos_get(s);

	if (char_idx < base)
	{
		ewl_text_trigger_start_pos_set(s, char_idx);
		ewl_text_trigger_length_set(s, base - char_idx);
	}
	else	
	{
		ewl_text_trigger_start_pos_set(s, base);
		ewl_text_trigger_length_set(s, char_idx - base);
	}

	/* set the clipboard text */
	txt = ewl_text_selection_text_get(EWL_TEXT(s->text_parent));
	if (txt)
	{
		Ewl_Embed *emb;
		Ewl_Window *win;

		emb = ewl_embed_widget_find(EWL_WIDGET(s->text_parent));
		win = ewl_window_window_find(emb->evas_window);

		ewl_window_selection_text_set(win, txt);
		FREE(txt);
	}

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/*
 * Ewl_Text_Context Stuff
 */

/**
 * @internal
 * @return Returns TRUE on success or FALSE on failure
 * @brief Initializes the context system
 */
int
ewl_text_context_init(void)
{
	DENTER_FUNCTION(DLEVEL_STABLE);

	if (!context_hash) 
	{
		context_hash = ecore_hash_new(ewl_text_context_hash_key,
				ewl_text_context_hash_cmp);
		ecore_hash_set_free_value(context_hash, ewl_text_context_cb_free);
	}

	DRETURN_INT(TRUE, DLEVEL_STABLE);
}

/**
 * @internal
 * @return Returns no value
 * @brief Shuts the context system down
 */
void
ewl_text_context_shutdown(void)
{
	DENTER_FUNCTION(DLEVEL_STABLE);

	if (context_hash) {
		ecore_hash_destroy(context_hash);
		context_hash = NULL;
	}

	if (ewl_text_default_context)
	{
		IF_FREE(ewl_text_default_context->font);
		if (ewl_text_default_context->format) 
			ecore_string_release(ewl_text_default_context->format);
		FREE(ewl_text_default_context);
	}

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @internal
 * @return Returns a new text context
 * @brief Creates and returns a new text context
 */
Ewl_Text_Context *
ewl_text_context_new(void)
{
	Ewl_Text_Context *tx;

	DENTER_FUNCTION(DLEVEL_STABLE);

	tx = NEW(Ewl_Text_Context, 1);
	tx->ref_count = 1;

	DRETURN_PTR(tx, DLEVEL_STABLE);;
}

/**
 * @internal
 * @param old: The context to duplicate
 * @return Returns a new context with the same values
 * @brief Duplicates the given context and returns the new version
 */
Ewl_Text_Context *
ewl_text_context_dup(Ewl_Text_Context *old)
{
	Ewl_Text_Context *tx;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("old", old, NULL);

	tx = ewl_text_context_new();
	memcpy(tx, old, sizeof(Ewl_Text_Context));

	/* make sure we get our own pointer to the font so it dosen't get
	 * free'd behind our back */
	tx->font = ((old->font) ? strdup(old->font) : NULL);
	tx->ref_count = 1;

	tx->format = ((old->format) ? ecore_string_instance((char *)old->format) : NULL);

	DRETURN_PTR(tx, DLEVEL_STABLE);
}

/**
 * @param t: The Ewl_Text to work with
 * @return Returns the default Ewl_Text_Context
 * @brief Retrieves the default Ewl_Text_Context.
 */
Ewl_Text_Context *
ewl_text_context_default_create(Ewl_Text *t)
{
	Ewl_Text_Context *tx = NULL, *tmp;
	int i;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("t", t, NULL);
	DCHECK_TYPE_RET("t", t, EWL_TEXT_TYPE, NULL);

	if (ewl_text_default_context)
	{
		ewl_text_context_acquire(ewl_text_default_context);
		DRETURN_PTR(ewl_text_default_context, DLEVEL_STABLE);
	}

	tmp = ewl_text_context_new();

	/* handle default values */
	tmp->font = ewl_theme_data_str_get(EWL_WIDGET(t), "font");
	tmp->font_source = NULL;
	tmp->size = ewl_theme_data_int_get(EWL_WIDGET(t), "font_size");

	tmp->color.r = ewl_theme_data_int_get(EWL_WIDGET(t), "color/r");
	tmp->color.g = ewl_theme_data_int_get(EWL_WIDGET(t), "color/g");
	tmp->color.b = ewl_theme_data_int_get(EWL_WIDGET(t), "color/b");
	tmp->color.a = ewl_theme_data_int_get(EWL_WIDGET(t), "color/a");

	i = ewl_theme_data_int_get(EWL_WIDGET(t), "underline");
	if (i)
	{
		tmp->styles |= EWL_TEXT_STYLE_UNDERLINE;
		tmp->style_colors.underline.r = ewl_theme_data_int_get(EWL_WIDGET(t),
				"underline/color/r");
		tmp->style_colors.underline.g = ewl_theme_data_int_get(EWL_WIDGET(t),
				"underline/color/g");
		tmp->style_colors.underline.b = ewl_theme_data_int_get(EWL_WIDGET(t),
				"underline/color/b");
		tmp->style_colors.underline.a = ewl_theme_data_int_get(EWL_WIDGET(t),
				"underline/color/a");
	}

	i = ewl_theme_data_int_get(EWL_WIDGET(t), "double_underline");
	if (i)
	{
		tmp->styles |= EWL_TEXT_STYLE_DOUBLE_UNDERLINE;
		tmp->style_colors.double_underline.r = ewl_theme_data_int_get(EWL_WIDGET(t),
				"double_underline/color/r");
		tmp->style_colors.double_underline.g = ewl_theme_data_int_get(EWL_WIDGET(t),
				"double_underline/color/g");
		tmp->style_colors.double_underline.b = ewl_theme_data_int_get(EWL_WIDGET(t),
				"double_underline/color/b");
		tmp->style_colors.double_underline.a = ewl_theme_data_int_get(EWL_WIDGET(t),
				"double_underline/color/a");
	}

	i = ewl_theme_data_int_get(EWL_WIDGET(t), "strikethrough");
	if (i)
	{
		tmp->styles |= EWL_TEXT_STYLE_STRIKETHROUGH;
		tmp->style_colors.strikethrough.r = ewl_theme_data_int_get(EWL_WIDGET(t),
				"strikethrough/color/r");
		tmp->style_colors.strikethrough.g = ewl_theme_data_int_get(EWL_WIDGET(t),
				"strikethrough/color/g");
		tmp->style_colors.strikethrough.b = ewl_theme_data_int_get(EWL_WIDGET(t),
				"strikethrough/color/b");
		tmp->style_colors.strikethrough.a = ewl_theme_data_int_get(EWL_WIDGET(t),
				"strikethrough/color/a");
	}

	i = ewl_theme_data_int_get(EWL_WIDGET(t), "shadow");
	if (i)
	{
		tmp->styles |= EWL_TEXT_STYLE_SHADOW;
		tmp->style_colors.shadow.r = ewl_theme_data_int_get(EWL_WIDGET(t),
				"shadow/color/r");
		tmp->style_colors.shadow.g = ewl_theme_data_int_get(EWL_WIDGET(t),
				"shadow/color/g");
		tmp->style_colors.shadow.b = ewl_theme_data_int_get(EWL_WIDGET(t),
				"shadow/color/b");
		tmp->style_colors.shadow.a = ewl_theme_data_int_get(EWL_WIDGET(t),
				"shadow/color/a");
	}

	i = ewl_theme_data_int_get(EWL_WIDGET(t), "soft_shadow");
	if (i)
	{
		tmp->styles |= EWL_TEXT_STYLE_SOFT_SHADOW;
		tmp->style_colors.shadow.r = ewl_theme_data_int_get(EWL_WIDGET(t),
				"shadow/color/r");
		tmp->style_colors.shadow.g = ewl_theme_data_int_get(EWL_WIDGET(t),
				"shadow/color/g");
		tmp->style_colors.shadow.b = ewl_theme_data_int_get(EWL_WIDGET(t),
				"shadow/color/b");
		tmp->style_colors.shadow.a = ewl_theme_data_int_get(EWL_WIDGET(t),
				"shadow/color/a");
	}

	i = ewl_theme_data_int_get(EWL_WIDGET(t), "far_shadow");
	if (i)
	{
		tmp->styles |= EWL_TEXT_STYLE_FAR_SHADOW;
		tmp->style_colors.shadow.r = ewl_theme_data_int_get(EWL_WIDGET(t),
				"shadow/color/r");
		tmp->style_colors.shadow.g = ewl_theme_data_int_get(EWL_WIDGET(t),
				"shadow/color/g");
		tmp->style_colors.shadow.b = ewl_theme_data_int_get(EWL_WIDGET(t),
				"shadow/color/b");
		tmp->style_colors.shadow.a = ewl_theme_data_int_get(EWL_WIDGET(t),
				"shadow/color/a");
	}

	i = ewl_theme_data_int_get(EWL_WIDGET(t), "outline");
	if (i)
	{
		tmp->styles |= EWL_TEXT_STYLE_OUTLINE;
		tmp->style_colors.outline.r = ewl_theme_data_int_get(EWL_WIDGET(t),
				"outline/color/r");
		tmp->style_colors.outline.g = ewl_theme_data_int_get(EWL_WIDGET(t),
				"outline/color/g");
		tmp->style_colors.outline.b = ewl_theme_data_int_get(EWL_WIDGET(t),
				"outline/color/b");
		tmp->style_colors.outline.a = ewl_theme_data_int_get(EWL_WIDGET(t),
				"outline/color/a");
	}

	i = ewl_theme_data_int_get(EWL_WIDGET(t), "glow");
	if (i)
	{
		tmp->styles |= EWL_TEXT_STYLE_GLOW;
		tmp->style_colors.glow.r = ewl_theme_data_int_get(EWL_WIDGET(t),
				"glow/color/r");
		tmp->style_colors.glow.g = ewl_theme_data_int_get(EWL_WIDGET(t),
				"glow/color/g");
		tmp->style_colors.glow.b = ewl_theme_data_int_get(EWL_WIDGET(t),
				"glow/color/b");
		tmp->style_colors.glow.a = ewl_theme_data_int_get(EWL_WIDGET(t),
				"glow/color/a");
	}

	/* XXX grab the alignment and wrap data from the theme here */
	tmp->align = EWL_FLAG_ALIGN_LEFT;

	tx = ewl_text_context_find(tmp, EWL_TEXT_CONTEXT_MASK_NONE, NULL);
	ewl_text_context_release(tmp);

	/* setup the default context and acquire a ref on it so 
	 * it won't go away */
	ewl_text_default_context = tx;

	DRETURN_PTR(tx, DLEVEL_STABLE);
}

static unsigned int
ewl_text_context_hash_key(const void *ctx)
{
	unsigned int key;
	const Ewl_Text_Context *tx = ctx;

	DENTER_FUNCTION(DLEVEL_STABLE);

	key = 0;
	if (tx->font)
		key ^= ecore_str_hash(tx->font);
	if (tx->font_source)
		key ^= ecore_str_hash(tx->font_source);
	key ^= (tx->size << 5);
	key ^= (tx->styles << 7);

#define COLOR_HASH(r, g, b, a) (r << 24 | g << 16 | b << 8 | a)
	key ^= COLOR_HASH(tx->color.r, tx->color.g, tx->color.b, tx->color.a);

	key ^= (COLOR_HASH(tx->style_colors.bg.r,
				tx->style_colors.bg.g,
				tx->style_colors.bg.b,
				tx->style_colors.bg.a) << 1);
	key ^= (COLOR_HASH(tx->style_colors.glow.r,
				tx->style_colors.glow.g,
				tx->style_colors.glow.b,
				tx->style_colors.glow.a) >> 1);
	key ^= (COLOR_HASH(tx->style_colors.outline.r,
				tx->style_colors.outline.g,
				tx->style_colors.outline.b,
				tx->style_colors.outline.a) << 3);
	key ^= (COLOR_HASH(tx->style_colors.shadow.r,
				tx->style_colors.shadow.g,
				tx->style_colors.shadow.b,
				tx->style_colors.shadow.a) >> 3);
	key ^= (COLOR_HASH(tx->style_colors.strikethrough.r,
				tx->style_colors.strikethrough.g,
				tx->style_colors.strikethrough.b,
				tx->style_colors.strikethrough.a) << 5);
	key ^= (COLOR_HASH(tx->style_colors.underline.r,
				tx->style_colors.underline.g,
				tx->style_colors.underline.b,
				tx->style_colors.underline.a) >> 5);
	key ^= (COLOR_HASH(tx->style_colors.double_underline.r,
				tx->style_colors.double_underline.g,
				tx->style_colors.double_underline.b,
				tx->style_colors.double_underline.a) << 7);

	DRETURN_INT(key, DLEVEL_STABLE);
}

static int
ewl_text_context_hash_cmp(const void *ctx1, const void *ctx2)
{
	unsigned int key1, key2;
	const Ewl_Text_Context *tx1 = ctx1;
	const Ewl_Text_Context *tx2 = ctx2;

	DENTER_FUNCTION(DLEVEL_STABLE);

#define KEY_COMPARE(k1, k2) if (k1 > k2) goto CTX1_LARGER; else if (k2 > k1) goto CTX2_LARGER;
	key1 = 0;
	key2 = 0;

	if (tx1->font)
		key1 = ecore_str_hash(tx1->font);
	if (tx2->font)
		key2 = ecore_str_hash(tx2->font);

	KEY_COMPARE(key1, key2);

	KEY_COMPARE(tx1->size, tx2->size);
	KEY_COMPARE(tx1->styles, tx2->styles);

	key1 = (tx1->color.r | tx1->color.g | tx1->color.b | tx1->color.a);
	key2 = (tx2->color.r | tx2->color.g | tx2->color.b | tx2->color.a);

	KEY_COMPARE(key1, key2);

	key1 = (tx1->style_colors.bg.r |
				tx1->style_colors.bg.g |
				tx1->style_colors.bg.b |
				tx1->style_colors.bg.a);

	key2 = (tx2->style_colors.bg.r |
				tx2->style_colors.bg.g |
				tx2->style_colors.bg.b |
				tx2->style_colors.bg.a);

	KEY_COMPARE(key1, key2);

	key1 = (tx1->style_colors.outline.r |
				tx1->style_colors.outline.g |
				tx1->style_colors.outline.b |
				tx1->style_colors.outline.a);

	key2 = (tx2->style_colors.outline.r |
				tx2->style_colors.outline.g |
				tx2->style_colors.outline.b |
				tx2->style_colors.outline.a);

	KEY_COMPARE(key1, key2);

	key1 = (tx1->style_colors.shadow.r |
				tx1->style_colors.shadow.g |
				tx1->style_colors.shadow.b |
				tx1->style_colors.shadow.a);

	key2 = (tx2->style_colors.shadow.r |
				tx2->style_colors.shadow.g |
				tx2->style_colors.shadow.b |
				tx2->style_colors.shadow.a);

	KEY_COMPARE(key1, key2);

	key1 = (tx1->style_colors.strikethrough.r |
				tx1->style_colors.strikethrough.g |
				tx1->style_colors.strikethrough.b |
				tx1->style_colors.strikethrough.a);

	key2 = (tx2->style_colors.strikethrough.r |
				tx2->style_colors.strikethrough.g |
				tx2->style_colors.strikethrough.b |
				tx2->style_colors.strikethrough.a);

	KEY_COMPARE(key1, key2);

	key1 = (tx1->style_colors.underline.r |
				tx1->style_colors.underline.g |
				tx1->style_colors.underline.b |
				tx1->style_colors.underline.a);

	key2 = (tx2->style_colors.underline.r |
				tx2->style_colors.underline.g |
				tx2->style_colors.underline.b |
				tx2->style_colors.underline.a);

	KEY_COMPARE(key1, key2);

	key1 = (tx1->style_colors.double_underline.r |
				tx1->style_colors.double_underline.g |
				tx1->style_colors.double_underline.b |
				tx1->style_colors.double_underline.a);

	key2 = (tx2->style_colors.double_underline.r |
				tx2->style_colors.double_underline.g |
				tx2->style_colors.double_underline.b |
				tx2->style_colors.double_underline.a);

	KEY_COMPARE(key1, key2);

	DRETURN_INT(0, DLEVEL_STABLE);
CTX1_LARGER:
	DRETURN_INT(-1, DLEVEL_STABLE);
CTX2_LARGER:
	DRETURN_INT(1, DLEVEL_STABLE);
}

#if 0
static char *
ewl_text_context_name_get(Ewl_Text_Context *tx, unsigned int context_mask,
						Ewl_Text_Context *tx_change)
{
	char name[2048];
	char *t = NULL, *t2 = NULL, *s = NULL, *s2 = NULL;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("tx", tx, NULL);

	if (context_mask > 0)
	{
		DCHECK_PARAM_PTR_RET("tx_change", tx_change, NULL);

		if (!tx_change->font) t2 = "";
		else t2 = tx_change->font;

		if (!tx_change->font_source) s2 = "";
		else s2 = tx_change->font_source;
	}

	if (!tx->font) t = "";
	else t = tx->font;

	if (!tx->font_source) s = "";
	else s = tx->font_source;

	snprintf(name, sizeof(name), "f%s%ss%ds%da%dw%dr%dg%db%da%dcbg%d%d%d%dcg%d%d%d%d"
				"co%d%d%d%dcs%d%d%d%dcst%d%d%d%dcu%d%d%d%dcdu%d%d%d%d", 
		((context_mask & EWL_TEXT_CONTEXT_MASK_FONT) ? s2 : s),
		((context_mask & EWL_TEXT_CONTEXT_MASK_FONT) ? t2 : t),
		((context_mask & EWL_TEXT_CONTEXT_MASK_SIZE) ? tx_change->size : tx->size),
		((context_mask & EWL_TEXT_CONTEXT_MASK_STYLES) ? tx_change->styles : tx->styles),
		((context_mask & EWL_TEXT_CONTEXT_MASK_ALIGN) ? tx_change->align : tx->align),
		((context_mask & EWL_TEXT_CONTEXT_MASK_WRAP) ? tx_change->wrap : tx->wrap),
		((context_mask & EWL_TEXT_CONTEXT_MASK_COLOR) ? tx_change->color.r : tx->color.r),
		((context_mask & EWL_TEXT_CONTEXT_MASK_COLOR) ? tx_change->color.g : tx->color.g),
		((context_mask & EWL_TEXT_CONTEXT_MASK_COLOR) ? tx_change->color.b : tx->color.b),
		((context_mask & EWL_TEXT_CONTEXT_MASK_COLOR) ? tx_change->color.a : tx->color.a),
		((context_mask & EWL_TEXT_CONTEXT_MASK_BG_COLOR) ? 
		 			tx_change->style_colors.bg.r : tx->style_colors.bg.r),
		((context_mask & EWL_TEXT_CONTEXT_MASK_BG_COLOR) ? 
		 			tx_change->style_colors.bg.g : tx->style_colors.bg.g),
		((context_mask & EWL_TEXT_CONTEXT_MASK_BG_COLOR) ? 
		 			tx_change->style_colors.bg.b : tx->style_colors.bg.b),
		((context_mask & EWL_TEXT_CONTEXT_MASK_BG_COLOR) ? 
		 			tx_change->style_colors.bg.a : tx->style_colors.bg.a),
		((context_mask & EWL_TEXT_CONTEXT_MASK_GLOW_COLOR) ? 
		 			tx_change->style_colors.glow.r : tx->style_colors.glow.r),
		((context_mask & EWL_TEXT_CONTEXT_MASK_GLOW_COLOR) ? 
		 			tx_change->style_colors.glow.g : tx->style_colors.glow.g),
		((context_mask & EWL_TEXT_CONTEXT_MASK_GLOW_COLOR) ? 
		 			tx_change->style_colors.glow.b : tx->style_colors.glow.b),
		((context_mask & EWL_TEXT_CONTEXT_MASK_GLOW_COLOR) ? 
		 			tx_change->style_colors.glow.a : tx->style_colors.glow.a),
		((context_mask & EWL_TEXT_CONTEXT_MASK_OUTLINE_COLOR) ? 
		 			tx_change->style_colors.outline.r : tx->style_colors.outline.r),
		((context_mask & EWL_TEXT_CONTEXT_MASK_OUTLINE_COLOR) ? 
		 			tx_change->style_colors.outline.g : tx->style_colors.outline.g),
		((context_mask & EWL_TEXT_CONTEXT_MASK_OUTLINE_COLOR) ? 
		 			tx_change->style_colors.outline.b : tx->style_colors.outline.b),
		((context_mask & EWL_TEXT_CONTEXT_MASK_OUTLINE_COLOR) ? 
		 			tx_change->style_colors.outline.a : tx->style_colors.outline.a),
		((context_mask & EWL_TEXT_CONTEXT_MASK_SHADOW_COLOR) ? 
		 			tx_change->style_colors.shadow.r : tx->style_colors.shadow.r),
		((context_mask & EWL_TEXT_CONTEXT_MASK_SHADOW_COLOR) ? 
		 			tx_change->style_colors.shadow.g : tx->style_colors.shadow.g),
		((context_mask & EWL_TEXT_CONTEXT_MASK_SHADOW_COLOR) ? 
		 			tx_change->style_colors.shadow.b : tx->style_colors.shadow.b),
		((context_mask & EWL_TEXT_CONTEXT_MASK_SHADOW_COLOR) ? 
		 			tx_change->style_colors.shadow.a : tx->style_colors.shadow.a),
		((context_mask & EWL_TEXT_CONTEXT_MASK_STRIKETHROUGH_COLOR) ? 
		 			tx_change->style_colors.strikethrough.r : tx->style_colors.strikethrough.r),
		((context_mask & EWL_TEXT_CONTEXT_MASK_STRIKETHROUGH_COLOR) ? 
		 			tx_change->style_colors.strikethrough.g : tx->style_colors.strikethrough.g),
		((context_mask & EWL_TEXT_CONTEXT_MASK_STRIKETHROUGH_COLOR) ? 
		 			tx_change->style_colors.strikethrough.b : tx->style_colors.strikethrough.b),
		((context_mask & EWL_TEXT_CONTEXT_MASK_STRIKETHROUGH_COLOR) ? 
		 			tx_change->style_colors.strikethrough.a : tx->style_colors.strikethrough.a),
		((context_mask & EWL_TEXT_CONTEXT_MASK_UNDERLINE_COLOR) ? 
		 			tx_change->style_colors.underline.r : tx->style_colors.underline.r),
		((context_mask & EWL_TEXT_CONTEXT_MASK_UNDERLINE_COLOR) ? 
		 			tx_change->style_colors.underline.g : tx->style_colors.underline.g),
		((context_mask & EWL_TEXT_CONTEXT_MASK_UNDERLINE_COLOR) ? 
		 			tx_change->style_colors.underline.b : tx->style_colors.underline.b),
		((context_mask & EWL_TEXT_CONTEXT_MASK_UNDERLINE_COLOR) ? 
		 			tx_change->style_colors.underline.a : tx->style_colors.underline.a),
		((context_mask & EWL_TEXT_CONTEXT_MASK_DOUBLE_UNDERLINE_COLOR) ? 
		 			tx_change->style_colors.double_underline.r : tx->style_colors.double_underline.r),
		((context_mask & EWL_TEXT_CONTEXT_MASK_DOUBLE_UNDERLINE_COLOR) ? 
		 			tx_change->style_colors.double_underline.g : tx->style_colors.double_underline.g),
		((context_mask & EWL_TEXT_CONTEXT_MASK_DOUBLE_UNDERLINE_COLOR) ? 
		 			tx_change->style_colors.double_underline.b : tx->style_colors.double_underline.b),
		((context_mask & EWL_TEXT_CONTEXT_MASK_DOUBLE_UNDERLINE_COLOR) ? 
		 			tx_change->style_colors.double_underline.a : tx->style_colors.double_underline.a));

	DRETURN_PTR(strdup(name), DLEVEL_STABLE);
}
#endif

static void
ewl_text_context_merge(Ewl_Text_Context *tx, unsigned int context_mask,
						Ewl_Text_Context *tx_change)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("tx", tx);

	tx->font = ((context_mask & EWL_TEXT_CONTEXT_MASK_FONT) ? tx_change->font : tx->font);
	tx->font_source = ((context_mask & EWL_TEXT_CONTEXT_MASK_FONT) ? tx_change->font_source : tx->font_source);
	tx->size = ((context_mask & EWL_TEXT_CONTEXT_MASK_SIZE) ? tx_change->size : tx->size);
	tx->styles = ((context_mask & EWL_TEXT_CONTEXT_MASK_STYLES) ? tx_change->styles : tx->styles),
	tx->align = ((context_mask & EWL_TEXT_CONTEXT_MASK_ALIGN) ? tx_change->align : tx->align);
	tx->wrap = ((context_mask & EWL_TEXT_CONTEXT_MASK_WRAP) ? tx_change->wrap : tx->wrap);
	tx->color.r = ((context_mask & EWL_TEXT_CONTEXT_MASK_COLOR) ? tx_change->color.r : tx->color.r);
	tx->color.g = ((context_mask & EWL_TEXT_CONTEXT_MASK_COLOR) ? tx_change->color.g : tx->color.g);
	tx->color.b = ((context_mask & EWL_TEXT_CONTEXT_MASK_COLOR) ? tx_change->color.b : tx->color.b);
	tx->color.a = ((context_mask & EWL_TEXT_CONTEXT_MASK_COLOR) ? tx_change->color.a : tx->color.a);
	tx->style_colors.bg.r = ((context_mask & EWL_TEXT_CONTEXT_MASK_BG_COLOR) ? 
		 			tx_change->style_colors.bg.r : tx->style_colors.bg.r);
	tx->style_colors.bg.g = ((context_mask & EWL_TEXT_CONTEXT_MASK_BG_COLOR) ? 
		 			tx_change->style_colors.bg.g : tx->style_colors.bg.g);
	tx->style_colors.bg.b = ((context_mask & EWL_TEXT_CONTEXT_MASK_BG_COLOR) ? 
		 			tx_change->style_colors.bg.b : tx->style_colors.bg.b);
	tx->style_colors.bg.a = ((context_mask & EWL_TEXT_CONTEXT_MASK_BG_COLOR) ? 
		 			tx_change->style_colors.bg.a : tx->style_colors.bg.a);
	tx->style_colors.glow.r = ((context_mask & EWL_TEXT_CONTEXT_MASK_GLOW_COLOR) ? 
		 			tx_change->style_colors.glow.r : tx->style_colors.glow.r);
	tx->style_colors.glow.g = ((context_mask & EWL_TEXT_CONTEXT_MASK_GLOW_COLOR) ? 
		 			tx_change->style_colors.glow.g : tx->style_colors.glow.g);
	tx->style_colors.glow.b = ((context_mask & EWL_TEXT_CONTEXT_MASK_GLOW_COLOR) ? 
		 			tx_change->style_colors.glow.b : tx->style_colors.glow.b);
	tx->style_colors.glow.a = ((context_mask & EWL_TEXT_CONTEXT_MASK_GLOW_COLOR) ? 
		 			tx_change->style_colors.glow.a : tx->style_colors.glow.a);
	tx->style_colors.outline.r = ((context_mask & EWL_TEXT_CONTEXT_MASK_OUTLINE_COLOR) ? 
		 			tx_change->style_colors.outline.r : tx->style_colors.outline.r);
	tx->style_colors.outline.g = ((context_mask & EWL_TEXT_CONTEXT_MASK_OUTLINE_COLOR) ? 
		 			tx_change->style_colors.outline.g : tx->style_colors.outline.g);
	tx->style_colors.outline.b = ((context_mask & EWL_TEXT_CONTEXT_MASK_OUTLINE_COLOR) ? 
		 			tx_change->style_colors.outline.b : tx->style_colors.outline.b);
	tx->style_colors.outline.a = ((context_mask & EWL_TEXT_CONTEXT_MASK_OUTLINE_COLOR) ? 
		 			tx_change->style_colors.outline.a : tx->style_colors.outline.a);
	tx->style_colors.shadow.r = ((context_mask & EWL_TEXT_CONTEXT_MASK_SHADOW_COLOR) ? 
		 			tx_change->style_colors.shadow.r : tx->style_colors.shadow.r);
	tx->style_colors.shadow.g = ((context_mask & EWL_TEXT_CONTEXT_MASK_SHADOW_COLOR) ? 
		 			tx_change->style_colors.shadow.g : tx->style_colors.shadow.g);
	tx->style_colors.shadow.b = ((context_mask & EWL_TEXT_CONTEXT_MASK_SHADOW_COLOR) ? 
		 			tx_change->style_colors.shadow.b : tx->style_colors.shadow.b);
	tx->style_colors.shadow.a = ((context_mask & EWL_TEXT_CONTEXT_MASK_SHADOW_COLOR) ? 
		 			tx_change->style_colors.shadow.a : tx->style_colors.shadow.a);
	tx->style_colors.strikethrough.r = ((context_mask & EWL_TEXT_CONTEXT_MASK_STRIKETHROUGH_COLOR) ? 
		 			tx_change->style_colors.strikethrough.r : tx->style_colors.strikethrough.r);
	tx->style_colors.strikethrough.g = ((context_mask & EWL_TEXT_CONTEXT_MASK_STRIKETHROUGH_COLOR) ? 
		 			tx_change->style_colors.strikethrough.g : tx->style_colors.strikethrough.g);
	tx->style_colors.strikethrough.b = ((context_mask & EWL_TEXT_CONTEXT_MASK_STRIKETHROUGH_COLOR) ? 
		 			tx_change->style_colors.strikethrough.b : tx->style_colors.strikethrough.b);
	tx->style_colors.strikethrough.a = ((context_mask & EWL_TEXT_CONTEXT_MASK_STRIKETHROUGH_COLOR) ? 
		 			tx_change->style_colors.strikethrough.a : tx->style_colors.strikethrough.a);
	tx->style_colors.underline.r = ((context_mask & EWL_TEXT_CONTEXT_MASK_UNDERLINE_COLOR) ? 
		 			tx_change->style_colors.underline.r : tx->style_colors.underline.r),
	tx->style_colors.underline.g = ((context_mask & EWL_TEXT_CONTEXT_MASK_UNDERLINE_COLOR) ? 
		 			tx_change->style_colors.underline.g : tx->style_colors.underline.g),
	tx->style_colors.underline.b = ((context_mask & EWL_TEXT_CONTEXT_MASK_UNDERLINE_COLOR) ? 
		 			tx_change->style_colors.underline.b : tx->style_colors.underline.b),
	tx->style_colors.underline.a = ((context_mask & EWL_TEXT_CONTEXT_MASK_UNDERLINE_COLOR) ? 
		 			tx_change->style_colors.underline.a : tx->style_colors.underline.a),
	tx->style_colors.double_underline.r = ((context_mask & EWL_TEXT_CONTEXT_MASK_DOUBLE_UNDERLINE_COLOR) ? 
		 			tx_change->style_colors.double_underline.r : tx->style_colors.double_underline.r),
	tx->style_colors.double_underline.g = ((context_mask & EWL_TEXT_CONTEXT_MASK_DOUBLE_UNDERLINE_COLOR) ? 
		 			tx_change->style_colors.double_underline.g : tx->style_colors.double_underline.g),
	tx->style_colors.double_underline.b = ((context_mask & EWL_TEXT_CONTEXT_MASK_DOUBLE_UNDERLINE_COLOR) ? 
		 			tx_change->style_colors.double_underline.b : tx->style_colors.double_underline.b),
	tx->style_colors.double_underline.a = ((context_mask & EWL_TEXT_CONTEXT_MASK_DOUBLE_UNDERLINE_COLOR) ? 
		 			tx_change->style_colors.double_underline.a : tx->style_colors.double_underline.a);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static Ewl_Text_Context *
ewl_text_context_find(Ewl_Text_Context *tx, unsigned int context_mask,
					Ewl_Text_Context *tx_change)
{
	Ewl_Text_Context tmp_tx;
	Ewl_Text_Context *new_tx;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("tx", tx, NULL);

	/* only need the tx_change if we have a context mask */
	if (context_mask > 0)
		DCHECK_PARAM_PTR_RET("tx_change", tx_change, NULL);

	memcpy(&tmp_tx, tx, sizeof(Ewl_Text_Context));
	ewl_text_context_merge(&tmp_tx, context_mask, tx_change);
	new_tx = ecore_hash_get(context_hash, &tmp_tx);
	if (!new_tx)
	{
		if ((new_tx = ewl_text_context_dup(tx)))
		{
			if (context_mask & EWL_TEXT_CONTEXT_MASK_FONT)
			{
				IF_FREE(new_tx->font);
				new_tx->font = strdup(tx_change->font);

				IF_FREE(new_tx->font_source);
				if (tx_change->font_source)
					new_tx->font_source = strdup(tx_change->font_source);
			}
			else if (context_mask & EWL_TEXT_CONTEXT_MASK_SIZE)
				new_tx->size = tx_change->size;

			else if (context_mask & EWL_TEXT_CONTEXT_MASK_STYLES)
				new_tx->styles = tx_change->styles;

			else if (context_mask & EWL_TEXT_CONTEXT_MASK_ALIGN)
				new_tx->align = tx_change->align;

			else if (context_mask & EWL_TEXT_CONTEXT_MASK_WRAP)
				new_tx->wrap = tx_change->wrap;

			else if (context_mask & EWL_TEXT_CONTEXT_MASK_COLOR)
			{
				new_tx->color.r = tx_change->color.r;
				new_tx->color.g = tx_change->color.g;
				new_tx->color.b = tx_change->color.b;
				new_tx->color.a = tx_change->color.a;
			}
			else if (context_mask & EWL_TEXT_CONTEXT_MASK_BG_COLOR)
			{
				new_tx->style_colors.bg.r = tx_change->style_colors.bg.r;
				new_tx->style_colors.bg.g = tx_change->style_colors.bg.g;
				new_tx->style_colors.bg.b = tx_change->style_colors.bg.b;
				new_tx->style_colors.bg.a = tx_change->style_colors.bg.a;
			}
			else if (context_mask & EWL_TEXT_CONTEXT_MASK_GLOW_COLOR)
			{
				new_tx->style_colors.glow.r = tx_change->style_colors.glow.r;
				new_tx->style_colors.glow.g = tx_change->style_colors.glow.g;
				new_tx->style_colors.glow.b = tx_change->style_colors.glow.b;
				new_tx->style_colors.glow.a = tx_change->style_colors.glow.a;
			}
			else if (context_mask & EWL_TEXT_CONTEXT_MASK_OUTLINE_COLOR)
			{
				new_tx->style_colors.outline.r = tx_change->style_colors.outline.r;
				new_tx->style_colors.outline.g = tx_change->style_colors.outline.g;
				new_tx->style_colors.outline.b = tx_change->style_colors.outline.b;
				new_tx->style_colors.outline.a = tx_change->style_colors.outline.a;
			}
			else if (context_mask & EWL_TEXT_CONTEXT_MASK_SHADOW_COLOR)
			{
				new_tx->style_colors.shadow.r = tx_change->style_colors.shadow.r;
				new_tx->style_colors.shadow.g = tx_change->style_colors.shadow.g;
				new_tx->style_colors.shadow.b = tx_change->style_colors.shadow.b;
				new_tx->style_colors.shadow.a = tx_change->style_colors.shadow.a;
			}
			else if (context_mask & EWL_TEXT_CONTEXT_MASK_STRIKETHROUGH_COLOR)
			{
				new_tx->style_colors.strikethrough.r = tx_change->style_colors.strikethrough.r;
				new_tx->style_colors.strikethrough.g = tx_change->style_colors.strikethrough.g;
				new_tx->style_colors.strikethrough.b = tx_change->style_colors.strikethrough.b;
				new_tx->style_colors.strikethrough.a = tx_change->style_colors.strikethrough.a;
			}
			else if (context_mask & EWL_TEXT_CONTEXT_MASK_UNDERLINE_COLOR)
			{
				new_tx->style_colors.underline.r = tx_change->style_colors.underline.r;
				new_tx->style_colors.underline.g = tx_change->style_colors.underline.g;
				new_tx->style_colors.underline.b = tx_change->style_colors.underline.b;
				new_tx->style_colors.underline.a = tx_change->style_colors.underline.a;
			}
			else if (context_mask & EWL_TEXT_CONTEXT_MASK_DOUBLE_UNDERLINE_COLOR)
			{
				new_tx->style_colors.double_underline.r = tx_change->style_colors.double_underline.r;
				new_tx->style_colors.double_underline.g = tx_change->style_colors.double_underline.g;
				new_tx->style_colors.double_underline.b = tx_change->style_colors.double_underline.b;
				new_tx->style_colors.double_underline.a = tx_change->style_colors.double_underline.a;
			}

			if (new_tx->format) ecore_string_release(new_tx->format);
			new_tx->format = NULL;

			ecore_hash_set(context_hash, new_tx, new_tx);
		}
	}
	if (new_tx) ewl_text_context_acquire(new_tx);

	DRETURN_PTR(new_tx, DLEVEL_STABLE);
}

/**
 * @internal
 * @param tx: The context to work with
 * @return Returns no value
 * @brief Acquires a reference to the given context
 */
void
ewl_text_context_acquire(Ewl_Text_Context *tx)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("tx", tx);

	tx->ref_count ++;

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @internal
 * @param tx: The context to work with
 * @return Returns no value
 * @brief Releases a reference on the given context. 
 * Do not use the context after this as it will be deallocated if it's 
 * reference count drops to zero.
 */
void
ewl_text_context_release(Ewl_Text_Context *tx)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("tx", tx);

	tx->ref_count --;
	if (tx->ref_count > 0) return;

	ecore_hash_remove(context_hash, tx);

	IF_FREE(tx->font);
	if (tx->format) ecore_string_release(tx->format);
	FREE(tx);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @internal
 * @param a: The first context
 * @param b: The second context
 * @return Returns TRUE if the two contexts are the same
 * @brief Check if the tuw contexts are the same. Returns TRUE if so.
 */
int
ewl_text_context_compare(Ewl_Text_Context *a, Ewl_Text_Context *b)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("a", a, 0);
	DCHECK_PARAM_PTR_RET("b", b, 0);

	DRETURN_INT((a == b), DLEVEL_STABLE);
}

static void
ewl_text_context_print(Ewl_Text_Context *tx, const char *indent)
{
	char *t, *s;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("tx", tx);

	if (!tx->font) t = "";
	else t = tx->font;

	if (!tx->font_source) s = "";
	else s = tx->font_source;

	printf("%sfont: %s (source: %s)\n"
		"%ssize %d\n"
		"%sstyle %d\n"
		"%salign %d\n"
		"%swrap %d\n"
		"%sred %d\n"
		"%sgreen %d\n"
		"%sblue %d\n" 
		"%salpha %d",
			indent, t, s, indent, tx->size, indent, 
			tx->styles, indent, tx->align, 
			indent, tx->wrap, indent, tx->color.r, 
			indent, tx->color.g, indent, tx->color.b, 
			indent, tx->color.a);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static void
ewl_text_context_cb_free(void *data)
{
	Ewl_Text_Context *tx;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("data", data);

	tx = data;
	ewl_text_context_release(tx);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static void
ewl_text_cb_fmt_free(Ewl_Text_Fmt *fmt)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("fmt", fmt);

	ewl_text_context_release(fmt->tx);
	FREE(fmt);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}



