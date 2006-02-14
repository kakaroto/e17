#include <Ewl.h>
#include "ewl_debug.h"
#include "ewl_macros.h"
#include "ewl_private.h"

static Ewl_Text_Context *ewl_text_default_context = NULL;

/* This counts how many deletes we have before trigger a condense operation
 * on the tree */
#define EWL_TEXT_TREE_CONDENSE_COUNT  5

/* how much do we extend the text by when we need more space? */
#define EWL_TEXT_EXTEND_VAL  4096

/*
 * TODO
 * - need a way to handle fonts that aren't in the theme .edj
 * - need to setup the styles/align/wrap data from the theme in 
 *   ewl_text_context_default_create
 *   - new theme keys for the align/wrap stuff
 * - need to fill in the condense function
 */

/* Make a static hash to look up the context's. They can be shared between
 * the different text blocks. Just need to ref count them so we know when
 * they can be destroyed
 */
static Ecore_Hash *context_hash = NULL;

static void ewl_text_context_cb_free(void *data);
static void ewl_text_context_print(Ewl_Text_Context *tx, const char *indent);
static char *ewl_text_context_name_get(Ewl_Text_Context *tx, 
			unsigned int context_mask, Ewl_Text_Context *tx_change);
static Ewl_Text_Context *ewl_text_context_find(Ewl_Text_Context *tx,
			unsigned int context_mask, Ewl_Text_Context *tx_change);
static Ewl_Text_Context *ewl_text_context_default_create(Ewl_Text *t);

static void ewl_text_display(Ewl_Text *t);
static void ewl_text_plaintext_parse(Evas_Object *tb, char *txt);

static void ewl_text_tree_walk(Ewl_Text *t);
static void ewl_text_tree_node_walk(Ewl_Text *t, Ewl_Text_Tree *tree,
						unsigned int pos);
static int ewl_text_tree_idx_start_count_get(Ewl_Text_Tree *tree, 
					unsigned int idx, unsigned int inclusive);
static Ewl_Text_Tree *ewl_text_tree_node_split(Ewl_Text_Tree *tree, 
					unsigned int count, unsigned int pos, 
					unsigned int len, unsigned int context_mask, 
							Ewl_Text_Context *tx);
static void ewl_text_tree_node_delete(Ewl_Text *t, Ewl_Text_Tree *tree);

static void ewl_text_tree_shrink(Ewl_Text_Tree *tree);
static char *ewl_text_format_get(Ewl_Text_Context *ctx);
static Evas_Textblock_Cursor *ewl_text_textblock_cursor_position(Ewl_Text *t, 
							unsigned int idx);
static unsigned int ewl_text_textblock_cursor_to_index(Evas_Textblock_Cursor *cursor);

static void ewl_text_triggers_remove(Ewl_Text *t);
static void ewl_text_triggers_shift(Ewl_Text *t, unsigned int pos, 
					unsigned int len, unsigned int del);
static void ewl_text_trigger_position(Ewl_Text *t, Ewl_Text_Trigger *trig);

static void ewl_text_trigger_add(Ewl_Text *t, Ewl_Text_Trigger *trigger);
static void ewl_text_trigger_del(Ewl_Text *t, Ewl_Text_Trigger *trigger);
static void ewl_text_trigger_area_add(Ewl_Text *t, Ewl_Text_Trigger *cur, 
			Evas_Coord x, Evas_Coord y, Evas_Coord w, Evas_Coord h);

static void ewl_text_selection_select_to(Ewl_Text_Trigger *s, unsigned int idx);

/**
 * @return Returns a new Ewl_Text widget on success, NULL on failure.
 */
Ewl_Widget *
ewl_text_new(void)
{
	Ewl_Widget *w;
	DENTER_FUNCTION(DLEVEL_STABLE);

	w = NEW(Ewl_Text, 1);
	if (!w)
	{
		DRETURN_PTR(NULL, DLEVEL_STABLE);
	}

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
 */
int
ewl_text_init(Ewl_Text *t)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("t", t, FALSE);

	if (!ewl_container_init(EWL_CONTAINER(t)))
	{
		DRETURN_INT(FALSE, DLEVEL_STABLE);
	}
	ewl_widget_appearance_set(EWL_WIDGET(t), EWL_TEXT_TYPE);
	ewl_widget_inherit(EWL_WIDGET(t), EWL_TEXT_TYPE);

	ewl_object_fill_policy_set(EWL_OBJECT(t), EWL_FLAG_FILL_NONE);

	/* create the formatting tree before we do any formatting */
	t->formatting.tree = ewl_text_tree_new();
	if (!t->formatting.tree) 
	{
		DRETURN_INT(FALSE, DLEVEL_STABLE);
	}

	t->formatting.tree->tx = ewl_text_context_default_create(t);
	ewl_text_context_acquire(t->formatting.tree->tx);

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
 * @return Returns the length of the text in the widget @a t
 */
unsigned int
ewl_text_length_get(Ewl_Text *t)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("t", t, 0);
	DCHECK_TYPE_RET("t", t, EWL_TEXT_TYPE, 0);

	DRETURN_INT(t->length, DLEVEL_STABLE);
}

/**
 * @param t: The Ewl_Text to get the geometry from
 * @param idx: The index to get the geometry for
 * @param x: Where to put the x value
 * @param y: Where to put the y value
 * @param w: Where to put the w value
 * @param h: Where to put the h value
 */
void
ewl_text_index_geometry_map(Ewl_Text *t, unsigned int idx, int *x, int *y, 
							int *w, int *h)
{
	Evas_Coord tx = 0, ty = 0, tw = 0, th = 0;
	Evas_Textblock_Cursor *cursor;
	int shifting = 0;

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
	if (t->dirty)
		ewl_text_display(t);

	if (idx >= t->length)
	{
		idx = t->length - 1;
		shifting = 1;
	}

	cursor = ewl_text_textblock_cursor_position(t, idx);
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
 * @return Returns the index of the given coordinates
 */
unsigned int
ewl_text_coord_index_map(Ewl_Text *t, int x, int y)
{
	Evas_Textblock_Cursor *cursor;
	unsigned int idx = 0;
	Evas_Coord tx, ty, cx = 0, cy, cw, ch;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("t", t, 0);
	DCHECK_TYPE_RET("t", t, EWL_TEXT_TYPE, 0);

	if ((!REALIZED(t)) || (!t->textblock) || (!t->text))
	{
		DRETURN_INT(0, DLEVEL_STABLE);
	}

	/* force a display of the text */
	if (t->dirty)
		ewl_text_display(t);

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
					idx ++;
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
			 idx ++;
	}

	idx += ewl_text_textblock_cursor_to_index(cursor);
	evas_textblock_cursor_free(cursor);

	DRETURN_INT(idx, DLEVEL_STABLE);
}

/**
 * @param t: The Ewl_Text to get the text from
 * @return Returns the text in the widget @a t or NULL if no text is set
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
 */
void
ewl_text_clear(Ewl_Text *t)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("t", t);
	DCHECK_TYPE("t", t, EWL_TEXT_TYPE);

	if (t->length > 0)
	{
		ewl_text_cursor_position_set(t, 0);
		ewl_text_text_delete(t, t->length);
	}
	t->text = NULL;
	t->length = 0;
	t->total_size = 0;
	t->cursor_position = 0;

	/* clean out the triggers */
	if (t->triggers)
	{
		Ewl_Text_Trigger *trig;

		while ((trig = ecore_list_remove_first(t->triggers)))
		{
			trig->text_parent = NULL;
			ewl_widget_destroy(EWL_WIDGET(trig));
		}
	}

	/* cleanup the selection */
	if (t->selection)
		ewl_widget_destroy(EWL_WIDGET(t->selection));
	t->selection = NULL;
	t->dirty = TRUE;
	
	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param t: The Ewl_Text to set the text into
 * @param text: The text to set into the widget
 * @return Returns no value
 */
void
ewl_text_text_set(Ewl_Text *t, const char *text)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("t", t);
	DCHECK_TYPE("t", t, EWL_TEXT_TYPE);

	ewl_text_clear(t);
	ewl_text_text_insert(t, text, t->cursor_position);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param t: The Ewl_Text to set the text into
 * @param text: The text to set into the widget
 * @return Returns no value
 */
void 
ewl_text_text_prepend(Ewl_Text *t, const char *text)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("t", t);

	ewl_text_text_insert(t, text, 0);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param t: The Ewl_Text to set the text into
 * @param text: The text to set into the widget
 * @return Returns no value
 */
void
ewl_text_text_append(Ewl_Text *t, const char *text)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("t", t);
	DCHECK_TYPE("t", t, EWL_TEXT_TYPE);

	ewl_text_text_insert(t, text, t->length);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param t: The Ewl_Text to set the text into
 * @param text: The text to set into the widget
 * @param idx: The index to insert the text at
 * @return Returns no value
 */
void
ewl_text_text_insert(Ewl_Text *t, const char *text, unsigned int idx)
{
	int len = 0;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("t", t);
	DCHECK_TYPE("t", t, EWL_TEXT_TYPE);

	if (text) len = strlen(text);

	/* setup the cursor position to begin with. If this is the same
	 * position as before nothing will change (we'll keep our current
	 * pointer */
	ewl_text_cursor_position_set(t, idx);

	if (!text)
	{
		ewl_text_clear(t);
	}
	else if (!t->text)
	{
		t->text = strdup(text);
		t->length = len;
		t->total_size = t->length + 1;

		ewl_text_cursor_position_set(t, 0);
		ewl_text_tree_insert(t, t->cursor_position, t->length);
		ewl_text_cursor_position_set(t, t->length);
	}
	else
	{
		if ((t->length + len + 1) >= t->total_size)
		{
			int extend;

			extend = len;
			if (extend < EWL_TEXT_EXTEND_VAL)
				extend = EWL_TEXT_EXTEND_VAL;

			t->text = realloc(t->text, (t->length + extend + 1) * sizeof(char));
			t->total_size += extend + 1;
		}

		if (idx == t->length)
			strncat(t->text, text, len);
		else
		{
			memmove(t->text + idx + len, t->text + idx, t->length - idx);
			memcpy(t->text + idx, text, len);
		}
		t->length += len;
		t->text[t->length] = '\0';

		ewl_text_tree_insert(t, idx, len);
		ewl_text_cursor_position_set(t, idx + len);
	}

	t->dirty = TRUE;

	if (text) ewl_text_triggers_shift(t, idx, len, FALSE);
	else ewl_text_triggers_remove(t);

	ewl_widget_configure(EWL_WIDGET(t));

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param t: The Ewl_Text to delete the text from
 * @param length: The length of text to delete
 * @return Returns no value
 *
 * This will delete the specified length of text from the current cursor
 * position
 */
void
ewl_text_text_delete(Ewl_Text *t, unsigned int length)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("t", t);
	DCHECK_TYPE("t", t, EWL_TEXT_TYPE);

	if ((!t->text) || (length == 0) || (t->cursor_position >= t->length))
		DRETURN(DLEVEL_STABLE);

	if ((t->length - t->cursor_position) < length)
		length = t->length - t->cursor_position;

	t->length -= length;
	if (t->length > 0)
	{
		memmove(t->text + t->cursor_position, 
				t->text + t->cursor_position + length,
				t->length - t->cursor_position);

		t->text[t->length] = '\0';

		ewl_text_triggers_shift(t, t->cursor_position, length, TRUE);
	}
	else
	{
		IF_FREE(t->text);
		ewl_text_triggers_remove(t);
	}

	/* cleanup the nodes in the tree */
	ewl_text_tree_delete(t, t->cursor_position, length);
	t->delete_count ++;

	if (t->delete_count == EWL_TEXT_TREE_CONDENSE_COUNT)
	{
		ewl_text_tree_condense(t->formatting.tree);
		t->delete_count = 0;
	}

	t->dirty = TRUE;

	if (t->cursor_position > t->length)
		ewl_text_cursor_position_set(t, t->length);

	ewl_widget_configure(EWL_WIDGET(t));

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param t: The text to set the selectable value of
 * @param selectable: The selectable value to set
 * @return Returns no value
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
 * @retun Returns the selectable value of the widget
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

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("t", t, NULL);
	DCHECK_TYPE_RET("t", t, EWL_TEXT_TYPE, NULL);

	if ((!t->selection) || t->selection->len == 0)
		DRETURN_PTR(NULL, DLEVEL_STABLE);

	ret = malloc(sizeof(char) * (t->selection->len + 1));
	if (!ret) DRETURN_PTR(NULL, DLEVEL_STABLE);

	memcpy(ret, t->text + t->selection->pos, t->selection->len);
	ret[t->selection->len] = '\0';

	DRETURN_PTR(ret, DLEVEL_STABLE);
}

/**
 * @param t: The Ewl_Text to get the selection from
 * @return Returns the selection object of this text or NULL if no current
 * selection
 */
Ewl_Text_Trigger *
ewl_text_selection_get(Ewl_Text *t)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("t", t, NULL);
	DCHECK_TYPE_RET("t", t, EWL_TEXT_TYPE, NULL);

	if (t->selection && ewl_text_trigger_length_get(t->selection) > 0)
	{
		DRETURN_PTR(t->selection, DLEVEL_STABLE);
	}

	DRETURN_PTR(NULL, DLEVEL_STABLE);
}

/**
 * @param t: The text to check if there is a selection
 * @return Returns TRUE if there is selected text, FALSE otherwise
 */
unsigned int
ewl_text_has_selection(Ewl_Text *t)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("t", t, FALSE);
	DCHECK_TYPE_RET("t", t, EWL_TEXT_TYPE, FALSE);

	if (ewl_text_selection_get(t))
	{
		DRETURN_INT(TRUE, DLEVEL_STABLE);
	}

	DRETURN_INT(FALSE, DLEVEL_STABLE);
}

/**
 * @param t: The Ewl_Text widget to set the position into
 * @param pos: The position to set
 * @return Returns no value.
 */
void
ewl_text_cursor_position_set(Ewl_Text *t, unsigned int pos)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("t", t);
	DCHECK_TYPE("t", t, EWL_TEXT_TYPE);

	/* it's the same position, do nothing */
	if (pos == t->cursor_position)
		DRETURN(DLEVEL_STABLE);

	/* make sure we aren't more then the next char past the end of the
	 * text */
	if (pos > t->length) pos = t->length;
	t->cursor_position = pos;

	/* reset the current pointer */
	ewl_text_tree_current_node_set(t, NULL);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param t: The Ewl_Text to get the cursor position from
 * @return Returns the current cursor position in the widget
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
 */
unsigned int
ewl_text_cursor_position_line_up_get(Ewl_Text *t)
{
	Evas_Textblock_Cursor *cursor;
	unsigned int cur_idx, idx;
	Evas_Coord cx, cw;
	Evas_Coord lx, ly, lw, lh;
	int line;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("t", t, t->cursor_position);
	DCHECK_TYPE_RET("t", t, EWL_TEXT_TYPE, t->cursor_position);

	cur_idx = ewl_text_cursor_position_get(t);
	cursor = ewl_text_textblock_cursor_position(t, cur_idx);
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
	idx = ewl_text_textblock_cursor_to_index(cursor);

	DRETURN_INT(idx, DLEVEL_STABLE);
}

/**
 * @param t: The Ewl_Text to get the cursor position one line down from
 * @return Returns the cursor position if we moved down one line
 */
unsigned int
ewl_text_cursor_position_line_down_get(Ewl_Text *t)
{
	Evas_Textblock_Cursor *cursor;
	unsigned int cur_idx, idx;
	Evas_Coord cx, cw;
	Evas_Coord lx, ly, lw, lh;
	int line;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("t", t, t->cursor_position);
	DCHECK_TYPE_RET("t", t, EWL_TEXT_TYPE, t->cursor_position);

	cur_idx = ewl_text_cursor_position_get(t);
	cursor = ewl_text_textblock_cursor_position(t, cur_idx);
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
	idx = ewl_text_textblock_cursor_to_index(cursor);

	DRETURN_INT(idx, DLEVEL_STABLE);
}

/**
 * @param t: The Ewl_Widget to set the font into
 * @param font: The font to set
 *
 * This will set the current font to be used when we insert more text
 */
void
ewl_text_font_set(Ewl_Text *t, const char *font)
{
	Ewl_Text_Context *change;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("t", t);
	DCHECK_TYPE("t", t, EWL_TEXT_TYPE);

	change = ewl_text_context_new();

	/* null font will go back to the theme default */
	if (!font) change->font = ewl_theme_data_str_get(EWL_WIDGET(t), "font");
	else change->font = strdup(font);

	ewl_text_tree_context_set(t, EWL_TEXT_CONTEXT_MASK_FONT, change);
	ewl_text_context_release(change);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param t: The Ewl_Text to set the font too
 * @param font: The font to set
 * @param length: The distance to set the font over
 *
 * This will apply the specfied @a font from the current cursor position to
 * the length specified
 */
void
ewl_text_font_apply(Ewl_Text *t, const char *font, unsigned int length)
{
	Ewl_Text_Context *tx;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("t", t);
	DCHECK_TYPE("t", t, EWL_TEXT_TYPE);

	/* if length is 0 we have nothing to do */
	if (length == 0) 
	{
		DRETURN(DLEVEL_STABLE);
	}

	tx = ewl_text_context_new();

	/* null font will go back to the theme default */
	if (!font) tx->font = ewl_theme_data_str_get(EWL_WIDGET(t), "font");
	else tx->font = strdup(font);

	ewl_text_tree_context_apply(t, EWL_TEXT_CONTEXT_MASK_FONT, tx,
						t->cursor_position, length);
	ewl_text_context_release(tx);
	t->dirty = TRUE;

	ewl_widget_configure(EWL_WIDGET(t));

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param t: The Ewl_Text to get the font from
 * @param idx: The index to get the font at
 *
 * This will retrive the font used at the specified index in the text
 */
char *
ewl_text_font_get(Ewl_Text *t, unsigned int idx)
{
	char *font = NULL;
	Ewl_Text_Context *tx;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("t", t, NULL);
	DCHECK_TYPE("t", t, EWL_TEXT_TYPE);

	tx = ewl_text_tree_context_get(t->formatting.tree, idx);
	if (tx && tx->font)
		font = strdup(tx->font);

	DRETURN_PTR(font, DLEVEL_STABLE);
}

/**
 * @param t: The Ewl_Text to set the size too
 * @param size: The size to set the font too
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

	ewl_text_tree_context_set(t, EWL_TEXT_CONTEXT_MASK_SIZE, change);
	ewl_text_context_release(change);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param t: The Ewl_Text to set the size into
 * @param size: The size to set
 * @param length: Length of block to get the new size
 *
 * This will apply the font size to the text from the current cursor
 * position for the given length
 */
void
ewl_text_font_size_apply(Ewl_Text *t, unsigned int size, unsigned int length)
{
	Ewl_Text_Context *tx;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("t", t);
	DCHECK_TYPE("t", t, EWL_TEXT_TYPE);

	if (length == 0)
	{
		DRETURN(DLEVEL_STABLE);
	}

	tx = ewl_text_context_new();
	tx->size = size;
	ewl_text_tree_context_apply(t, EWL_TEXT_CONTEXT_MASK_SIZE, tx,
						t->cursor_position, length);
	ewl_text_context_release(tx);
	t->dirty = TRUE;

	ewl_widget_configure(EWL_WIDGET(t));

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param t: The Ewl_Text to get the size from
 * @param idx: The index you want to get the size for
 */
unsigned int
ewl_text_font_size_get(Ewl_Text *t, unsigned int idx)
{
	Ewl_Text_Context *tx;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("t", t, 0);
	DCHECK_TYPE_RET("t", t, EWL_TEXT_TYPE, 0);

	tx = ewl_text_tree_context_get(t->formatting.tree, idx);

	DRETURN_INT(((tx) ? tx->size : 0), DLEVEL_STABLE);
}

/*
 * @param t: The Ewl_Text to set the colour on
 * @parma r: The red value
 * @param g: The green value
 * @param b: The blue value
 * @param a: The alpha value
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

	ewl_text_tree_context_set(t, EWL_TEXT_CONTEXT_MASK_COLOR, change);
	ewl_text_context_release(change);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param t: The Ewl_Text to set the colour into
 * @param r: The red value to set
 * @param g: The green value to set
 * @param b: The blue value to set
 * @param a: The alpha value to set
 * @param length: The length of text to apply the colour over
 *
 * This will set the given colour from the current cursor position for the
 * specified length
 */
void 
ewl_text_color_apply(Ewl_Text *t, unsigned int r, unsigned int g,
				unsigned int b, unsigned int a, 
				unsigned int length)
{
	Ewl_Text_Context *tx;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("t", t);
	DCHECK_TYPE("t", t, EWL_TEXT_TYPE);

	/* set this into the b-tree if we have length */
	if (length == 0)
	{
		DRETURN(DLEVEL_STABLE);
	}

	tx = ewl_text_context_new();
	tx->color.r = r;
	tx->color.g = g;
	tx->color.b = b;
	tx->color.a = a;

	ewl_text_tree_context_apply(t, EWL_TEXT_CONTEXT_MASK_COLOR, tx,
						t->cursor_position, length);
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
 * @param idx: The index to get the colour from 
 */
void
ewl_text_color_get(Ewl_Text *t, unsigned int *r, unsigned int *g,
				unsigned int *b, unsigned int *a,
				unsigned int idx)
{
	Ewl_Text_Context *tx;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("t", t);
	DCHECK_TYPE("t", t, EWL_TEXT_TYPE);

	tx = ewl_text_tree_context_get(t->formatting.tree, idx);
	if (tx)
	{
		if (r) *r = tx->color.r;
		if (g) *g = tx->color.g;
		if (b) *b = tx->color.b;
		if (a) *a = tx->color.a;
	}

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param t: The Ewl_Text ot set the alignment into
 * @param align: The alignment to set
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

	ewl_text_tree_context_set(t, EWL_TEXT_CONTEXT_MASK_ALIGN, change);
	ewl_text_context_release(change);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param t: The Ewl_Text to apply the alignment too
 * @param align: The alignment to apply
 * @param length: The length to apply the alignment for
 *
 * This will set the given alignment from the current cursor position for
 * the given length of text
 */
void
ewl_text_align_apply(Ewl_Text *t, unsigned int align, unsigned int length)
{
	Ewl_Text_Context *tx;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("t", t);
	DCHECK_TYPE("t", t, EWL_TEXT_TYPE);

	if (length == 0)
	{
		DRETURN(DLEVEL_STABLE);
	}

	tx = ewl_text_context_new();
	tx->align = align;
	ewl_text_tree_context_apply(t, EWL_TEXT_CONTEXT_MASK_ALIGN, tx,
						t->cursor_position, length);
	ewl_text_context_release(tx);
	t->dirty = TRUE;

	ewl_widget_configure(EWL_WIDGET(t));

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param t: The Ewl_Text to get the alignment from
 * @param idx: The index to get the alignment from
 */
unsigned int
ewl_text_align_get(Ewl_Text *t, unsigned int idx)
{
	Ewl_Text_Context *tx;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("t", t, 0);
	DCHECK_TYPE_RET("t", t, EWL_TEXT_TYPE, 0);

	tx = ewl_text_tree_context_get(t->formatting.tree, idx);

	DRETURN_INT(tx->align, DLEVEL_STABLE);
}

/**
 * @param t: The Ewl_Text to set the style into
 * @param styles: The styles to set into the text
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

	ewl_text_tree_context_set(t, EWL_TEXT_CONTEXT_MASK_STYLES, change);
	ewl_text_context_release(change);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param t: The Ewl_Text to apply the style too
 * @param styles: The styles to set into the text
 * @param length: The length of text to apply the style too
 *
 * This will set the given style from the current cursor position for the
 * given length of text
 */
void
ewl_text_styles_apply(Ewl_Text *t, unsigned int styles, unsigned int length)
{
	Ewl_Text_Context *tx;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("t", t);
	DCHECK_TYPE("t", t, EWL_TEXT_TYPE);

	if (length == 0)
	{
		DRETURN(DLEVEL_STABLE);
	}

	tx = ewl_text_context_new();
	tx->styles = styles;
	ewl_text_tree_context_apply(t, EWL_TEXT_CONTEXT_MASK_STYLES, tx,
						t->cursor_position, length);
	ewl_text_context_release(tx);
	t->dirty = TRUE;

	ewl_widget_configure(EWL_WIDGET(t));

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
  * @param t: The text to add the style too
  * @param style: The style to add to the text
  * @param length: The lenght of text to add the style too
  *
  * This will add the given style to the text from the cursor up to length
  * characters
  */
 void
 ewl_text_style_add(Ewl_Text *t, Ewl_Text_Style style, unsigned int length)
 {
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("t", t);
	DCHECK_TYPE("t", t, EWL_TEXT_TYPE);
 
	ewl_text_tree_context_style_apply(t, style, t->cursor_position, length, FALSE);
 
	DLEAVE_FUNCTION(DLEVEL_STABLE);
 }
 
/**
 * @param t: The text to delete the style from
 * @param style: The style to delete from the text 
 * @param length: The lenght of text to delete the style from 
 *
 * This will delete the given style from the text starting at the cursor up 
 * to length characters
 */
void
ewl_text_style_del(Ewl_Text *t, Ewl_Text_Style style, unsigned int length)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("t", t);
	DCHECK_TYPE("t", t, EWL_TEXT_TYPE);
 
	ewl_text_tree_context_style_remove(t, style, t->cursor_position, length);
	t->dirty = TRUE;

	ewl_widget_configure(EWL_WIDGET(t));

 
	DLEAVE_FUNCTION(DLEVEL_STABLE);
 }
 
/**
 * @param t: The text to invert the style on
 * @param style: The style to invert in the text 
 * @param length: The lenght of text to invert the style on 
 *
 * This will invert the given style in the text starting at the cursor up 
 * to length characters
 */
void
ewl_text_style_invert(Ewl_Text *t, Ewl_Text_Style style, unsigned int length)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("t", t);
	DCHECK_TYPE("t", t, EWL_TEXT_TYPE);
 
	ewl_text_tree_context_style_apply(t, style, t->cursor_position, length, TRUE);
	t->dirty = TRUE;

	ewl_widget_configure(EWL_WIDGET(t));
		  
	DLEAVE_FUNCTION(DLEVEL_STABLE);
}
  
/**
 * @param t: The text to check for the style
 * @param style: The style to check for
 * @param idx: The index to check for the style
 */
unsigned int
ewl_text_style_has(Ewl_Text *t, Ewl_Text_Style style, unsigned int idx)
{
	Ewl_Text_Tree *child;
 
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("t", t, FALSE);
	DCHECK_TYPE_RET("t", t, EWL_TEXT_TYPE, FALSE);
 
	child = ewl_text_tree_node_get(t->formatting.tree, idx, TRUE);
	if ((!child) || (!child->tx))
		 DRETURN_INT((0 == style), DLEVEL_STABLE);
 
	DRETURN_INT((child->tx->styles & style), DLEVEL_STABLE);
}

/**
 * @param t: The Ewl_Text to get the style from
 * @param idx: The index to get the style from
 */
unsigned int
ewl_text_styles_get(Ewl_Text *t, unsigned int idx)
{
	Ewl_Text_Context *tx;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("t", t, 0);
	DCHECK_TYPE_RET("t", t, EWL_TEXT_TYPE, 0);

	tx = ewl_text_tree_context_get(t->formatting.tree, idx);

	DRETURN_INT((tx ? tx->styles : 0), DLEVEL_STABLE);
}

/**
 * @param t: The Ewl_Text to set the wrap into
 * @param wrap: The wrap value to set
 */
void
ewl_text_wrap_set(Ewl_Text *t, unsigned int wrap)
{
	Ewl_Text_Context *change;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("t", t);
	DCHECK_TYPE("t", t, EWL_TEXT_TYPE);

	change = ewl_text_context_new();
	change->wrap = wrap;

	ewl_text_tree_context_set(t, EWL_TEXT_CONTEXT_MASK_WRAP, change);
	ewl_text_context_release(change);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param t: The Ewl_Text to apply the wrap value too
 * @param wrap: The wrap value to apply
 * @param length: The length of text to apply the wrap value over
 *
 * This will apply the given wrap value from the current cursor position for
 * the given length of text
 */
void
ewl_text_wrap_apply(Ewl_Text *t, unsigned int wrap, unsigned int length)
{
	Ewl_Text_Context *tx;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("t", t);
	DCHECK_TYPE("t", t, EWL_TEXT_TYPE);

	if (length == 0)
	{
		DRETURN(DLEVEL_STABLE);
	}

	tx = ewl_text_context_new();
	tx->wrap = wrap;
	ewl_text_tree_context_apply(t, EWL_TEXT_CONTEXT_MASK_WRAP, tx,
						t->cursor_position, length);
	ewl_text_context_release(tx);
	t->dirty = TRUE;

	ewl_widget_configure(EWL_WIDGET(t));

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param t: The Ewl_Text to get the wrap value for
 * @param idx: The index to get the wrap value from
 */
unsigned int
ewl_text_wrap_get(Ewl_Text *t, unsigned int idx)
{
	Ewl_Text_Context *tx;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("t", t, 0);
	DCHECK_TYPE_RET("t", t, EWL_TEXT_TYPE, 0);

	tx = ewl_text_tree_context_get(t->formatting.tree, idx);

	DRETURN_INT(tx->wrap, DLEVEL_STABLE);
}

/**
 * @param t: The Ewl_Text to set the text background colour of
 * @param r: The red value
 * @param g: The green value
 * @param b: The blue value
 * @param a: The alpha value
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

	ewl_text_tree_context_set(t, EWL_TEXT_CONTEXT_MASK_BG_COLOR, change);
	ewl_text_context_release(change);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param t: The Ewl_Text to set the text background colour of
 * @param r: The red value
 * @param g: The green value
 * @param b: The blue value
 * @param a: The alpha value
 * @param length: The length of text to apply the bg colour over
 *
 * This will set the bg colour of the text from the current cursor position
 * to the given length.
 */
void
ewl_text_bg_color_apply(Ewl_Text *t, unsigned int r, unsigned int g,
					unsigned int b, unsigned int a,
					unsigned int length)
{
	Ewl_Text_Context *tx;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("t", t);
	DCHECK_TYPE("t", t, EWL_TEXT_TYPE);

	/* set this into the b-tree if we have length */
	if (length == 0)
	{
		DRETURN(DLEVEL_STABLE);
	}

	tx = ewl_text_context_new();
	tx->style_colors.bg.r = r;
	tx->style_colors.bg.g = g;
	tx->style_colors.bg.b = b;
	tx->style_colors.bg.a = a;

	ewl_text_tree_context_apply(t, EWL_TEXT_CONTEXT_MASK_BG_COLOR, tx,
						t->cursor_position, length);
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
 * @param idx: The index to get the colour from
 */
void
ewl_text_bg_color_get(Ewl_Text *t, unsigned int *r, unsigned int *g,
					unsigned int *b, unsigned int *a,
					unsigned int idx)
{
	Ewl_Text_Context *tx;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("t", t);
	DCHECK_TYPE("t", t, EWL_TEXT_TYPE);

	tx = ewl_text_tree_context_get(t->formatting.tree, idx);
	if (tx)
	{
		if (r) *r = tx->style_colors.bg.r;
		if (g) *g = tx->style_colors.bg.g;
		if (b) *b = tx->style_colors.bg.b;
		if (a) *a = tx->style_colors.bg.a;
	}

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param t: The Ewl_Text to set the text glow colour of
 * @param r: The red value
 * @param g: The green value
 * @param b: The blue value
 * @param a: The alpha value
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

	ewl_text_tree_context_set(t, EWL_TEXT_CONTEXT_MASK_GLOW_COLOR, change);
	ewl_text_context_release(change);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param t: The Ewl_Text to set the text glow colour of
 * @param r: The red value
 * @param g: The green value
 * @param b: The blue value
 * @param a: The alpha value
 * @param length: The length of text to apply the glow colour over
 *
 * This will set the glow colour of the text from the current cursor position
 * to the given length.
 */
void
ewl_text_glow_color_apply(Ewl_Text *t, unsigned int r, unsigned int g,
					unsigned int b, unsigned int a,
					unsigned int length)
{
	Ewl_Text_Context *tx;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("t", t);
	DCHECK_TYPE("t", t, EWL_TEXT_TYPE);

	/* set this into the b-tree if we have length */
	if (length == 0)
	{
		DRETURN(DLEVEL_STABLE);
	}

	tx = ewl_text_context_new();
	tx->style_colors.glow.r = r;
	tx->style_colors.glow.g = g;
	tx->style_colors.glow.b = b;
	tx->style_colors.glow.a = a;

	ewl_text_tree_context_apply(t, EWL_TEXT_CONTEXT_MASK_GLOW_COLOR, tx,
							t->cursor_position, length);
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
 * @param idx: The index to get the colour from
 */
void
ewl_text_glow_color_get(Ewl_Text *t, unsigned int *r, unsigned int *g,
					unsigned int *b, unsigned int *a,
					unsigned int idx)
{
	Ewl_Text_Context *tx;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("t", t);
	DCHECK_TYPE("t", t, EWL_TEXT_TYPE);

	tx = ewl_text_tree_context_get(t->formatting.tree, idx);
	if (tx)
	{
		if (r) *r = tx->style_colors.glow.r;
		if (g) *g = tx->style_colors.glow.g;
		if (b) *b = tx->style_colors.glow.b;
		if (a) *a = tx->style_colors.glow.a;
	}

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param t: The Ewl_Text to set the text outline colour of
 * @param r: The red value
 * @param g: The green value
 * @param b: The blue value
 * @param a: The alpha value
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

	ewl_text_tree_context_set(t, EWL_TEXT_CONTEXT_MASK_OUTLINE_COLOR, change);
	ewl_text_context_release(change);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param t: The Ewl_Text to set the text outline colour of
 * @param r: The red value
 * @param g: The green value
 * @param b: The blue value
 * @param a: The alpha value
 * @param length: The length of text to apply the outline colour over
 *
 * This will set the outline colour of the text from the current cursor position
 * to the given length.
 */
void
ewl_text_outline_color_apply(Ewl_Text *t, unsigned int r, unsigned int g,
						unsigned int b, unsigned int a,
						unsigned int length)
{
	Ewl_Text_Context *tx;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("t", t);
	DCHECK_TYPE("t", t, EWL_TEXT_TYPE);

	/* set this into the b-tree if we have length */
	if (length == 0)
	{
		DRETURN(DLEVEL_STABLE);
	}

	tx = ewl_text_context_new();
	tx->style_colors.outline.r = r;
	tx->style_colors.outline.g = g;
	tx->style_colors.outline.b = b;
	tx->style_colors.outline.a = a;

	ewl_text_tree_context_apply(t, EWL_TEXT_CONTEXT_MASK_OUTLINE_COLOR, tx,
							t->cursor_position, length);
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
 * @param idx: The index to get the colour from
 */
void
ewl_text_outline_color_get(Ewl_Text *t, unsigned int *r, unsigned int *g,
						unsigned int *b, unsigned int *a,
						unsigned int idx)
{
	Ewl_Text_Context *tx;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("t", t);
	DCHECK_TYPE("t", t, EWL_TEXT_TYPE);

	tx = ewl_text_tree_context_get(t->formatting.tree, idx);
	if (tx)
	{
		if (r) *r = tx->style_colors.outline.r;
		if (g) *g = tx->style_colors.outline.g;
		if (b) *b = tx->style_colors.outline.b;
		if (a) *a = tx->style_colors.outline.a;
	}

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param t: The Ewl_Text to set the text shadow colour of
 * @param r: The red value
 * @param g: The green value
 * @param b: The blue value
 * @param a: The alpha value
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

	ewl_text_tree_context_set(t, EWL_TEXT_CONTEXT_MASK_SHADOW_COLOR, change);
	ewl_text_context_release(change);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param t: The Ewl_Text to set the text shadow colour of
 * @param r: The red value
 * @param g: The green value
 * @param b: The blue value
 * @param a: The alpha value
 * @param length: The length of text to apply the shadow colour over
 *
 * This will set the shadow colour of the text from the current cursor position
 * to the given length.
 */
void
ewl_text_shadow_color_apply(Ewl_Text *t, unsigned int r, unsigned int g,
						unsigned int b, unsigned int a,
						unsigned int length)
{
	Ewl_Text_Context *tx;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("t", t);
	DCHECK_TYPE("t", t, EWL_TEXT_TYPE);

	/* set this into the b-tree if we have length */
	if (length == 0)
	{
		DRETURN(DLEVEL_STABLE);
	}

	tx = ewl_text_context_new();
	tx->style_colors.shadow.r = r;
	tx->style_colors.shadow.g = g;
	tx->style_colors.shadow.b = b;
	tx->style_colors.shadow.a = a;

	ewl_text_tree_context_apply(t, EWL_TEXT_CONTEXT_MASK_SHADOW_COLOR, tx,
							t->cursor_position, length);
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
 * @param idx: The index to get the colour from
 */
void
ewl_text_shadow_color_get(Ewl_Text *t, unsigned int *r, unsigned int *g,
						unsigned int *b, unsigned int *a,
						unsigned int idx)
{
	Ewl_Text_Context *tx;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("t", t);
	DCHECK_TYPE("t", t, EWL_TEXT_TYPE);

	tx = ewl_text_tree_context_get(t->formatting.tree, idx);
	if (tx)
	{
		if (r) *r = tx->style_colors.shadow.r;
		if (g) *g = tx->style_colors.shadow.g;
		if (b) *b = tx->style_colors.shadow.b;
		if (a) *a = tx->style_colors.shadow.a;
	}

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param t: The Ewl_Text to set the text strikethrough colour of
 * @param r: The red value
 * @param g: The green value
 * @param b: The blue value
 * @param a: The alpha value
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

	ewl_text_tree_context_set(t, EWL_TEXT_CONTEXT_MASK_STRIKETHROUGH_COLOR, change);
	ewl_text_context_release(change);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param t: The Ewl_Text to set the text strikethrough colour of
 * @param r: The red value
 * @param g: The green value
 * @param b: The blue value
 * @param a: The alpha value
 * @param length: The length of text to apply the strikethrough colour over
 *
 * This will set the strikethrough colour of the text from the current cursor position
 * to the given length.
 */
void
ewl_text_strikethrough_color_apply(Ewl_Text *t, unsigned int r, unsigned int g,
						unsigned int b, unsigned int a,
						unsigned int length)
{
	Ewl_Text_Context *tx;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("t", t);
	DCHECK_TYPE("t", t, EWL_TEXT_TYPE);

	/* set this into the b-tree if we have length */
	if (length == 0)
	{
		DRETURN(DLEVEL_STABLE);
	}

	tx = ewl_text_context_new();
	tx->style_colors.strikethrough.r = r;
	tx->style_colors.strikethrough.g = g;
	tx->style_colors.strikethrough.b = b;
	tx->style_colors.strikethrough.a = a;

	ewl_text_tree_context_apply(t, EWL_TEXT_CONTEXT_MASK_STRIKETHROUGH_COLOR, tx,
							t->cursor_position, length);
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
 * @param idx: The index to get the colour from
 */
void
ewl_text_strikethrough_color_get(Ewl_Text *t, unsigned int *r, unsigned int *g,
						unsigned int *b, unsigned int *a,
						unsigned int idx)
{
	Ewl_Text_Context *tx;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("t", t);
	DCHECK_TYPE("t", t, EWL_TEXT_TYPE);

	tx = ewl_text_tree_context_get(t->formatting.tree, idx);
	if (tx)
	{
		if (r) *r = tx->style_colors.strikethrough.r;
		if (g) *g = tx->style_colors.strikethrough.g;
		if (b) *b = tx->style_colors.strikethrough.b;
		if (a) *a = tx->style_colors.strikethrough.a;
	}

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param t: The Ewl_Text to set the text underline colour of
 * @param r: The red value
 * @param g: The green value
 * @param b: The blue value
 * @param a: The alpha value
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

	ewl_text_tree_context_set(t, EWL_TEXT_CONTEXT_MASK_UNDERLINE_COLOR, change);
	ewl_text_context_release(change);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param t: The Ewl_Text to set the text underline colour of
 * @param r: The red value
 * @param g: The green value
 * @param b: The blue value
 * @param a: The alpha value
 * @param length: The length of text to apply the underline colour over
 *
 * This will set the underline colour of the text from the current cursor position
 * to the given length.
 */
void
ewl_text_underline_color_apply(Ewl_Text *t, unsigned int r, unsigned int g,
						unsigned int b, unsigned int a,
						unsigned int length)
{
	Ewl_Text_Context *tx;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("t", t);
	DCHECK_TYPE("t", t, EWL_TEXT_TYPE);

	/* set this into the b-tree if we have length */
	if (length == 0)
	{
		DRETURN(DLEVEL_STABLE);
	}

	tx = ewl_text_context_new();
	tx->style_colors.underline.r = r;
	tx->style_colors.underline.g = g;
	tx->style_colors.underline.b = b;
	tx->style_colors.underline.a = a;

	ewl_text_tree_context_apply(t, EWL_TEXT_CONTEXT_MASK_UNDERLINE_COLOR, tx,
							t->cursor_position, length);
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
 * @param idx: The index to get the colour from
 */
void
ewl_text_underline_color_get(Ewl_Text *t, unsigned int *r, unsigned int *g,
						unsigned int *b, unsigned int *a,
						unsigned int idx)
{
	Ewl_Text_Context *tx;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("t", t);
	DCHECK_TYPE("t", t, EWL_TEXT_TYPE);

	tx = ewl_text_tree_context_get(t->formatting.tree, idx);
	if (tx)
	{
		if (r) *r = tx->style_colors.outline.r;
		if (g) *g = tx->style_colors.outline.g;
		if (b) *b = tx->style_colors.outline.b;
		if (a) *a = tx->style_colors.outline.a;
	}

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param t: The Ewl_Text to set the text double underline colour of
 * @param r: The red value
 * @param g: The green value
 * @param b: The blue value
 * @param a: The alpha value
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

	ewl_text_tree_context_set(t, EWL_TEXT_CONTEXT_MASK_DOUBLE_UNDERLINE_COLOR, change);
	ewl_text_context_release(change);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param t: The Ewl_Text to set the text double underline colour of
 * @param r: The red value
 * @param g: The green value
 * @param b: The blue value
 * @param a: The alpha value
 * @param length: The length of text to apply the double underline colour over
 *
 * This will set the double_underline colour of the text from the current cursor position
 * to the given length.
 */
void
ewl_text_double_underline_color_apply(Ewl_Text *t, unsigned int r, unsigned int g,
							unsigned int b, unsigned int a,
							unsigned int length)
{
	Ewl_Text_Context *tx;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("t", t);
	DCHECK_TYPE("t", t, EWL_TEXT_TYPE);

	/* set this into the b-tree if we have length */
	if (length == 0)
	{
		DRETURN(DLEVEL_STABLE);
	}

	tx = ewl_text_context_new();
	tx->style_colors.double_underline.r = r;
	tx->style_colors.double_underline.g = g;
	tx->style_colors.double_underline.b = b;
	tx->style_colors.double_underline.a = a;

	ewl_text_tree_context_apply(t, EWL_TEXT_CONTEXT_MASK_DOUBLE_UNDERLINE_COLOR, tx,
							t->cursor_position, length);
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
 * @param idx: The index to get the colour from
 */
void
ewl_text_double_underline_color_get(Ewl_Text *t, unsigned int *r, unsigned int *g,
						unsigned int *b, unsigned int *a,
						unsigned int idx)
{
	Ewl_Text_Context *tx;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("t", t);
	DCHECK_TYPE("t", t, EWL_TEXT_TYPE);

	tx = ewl_text_tree_context_get(t->formatting.tree, idx);
	if (tx)
	{
		if (r) *r = tx->style_colors.double_underline.r;
		if (g) *g = tx->style_colors.double_underline.g;
		if (b) *b = tx->style_colors.double_underline.b;
		if (a) *a = tx->style_colors.double_underline.a;
	}

	DLEAVE_FUNCTION(DLEVEL_STABLE);
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

	cursor = (Evas_Textblock_Cursor *)evas_object_textblock_cursor_get(t->textblock);
	evas_textblock_cursor_text_append(cursor, "");

	ewl_text_tree_walk(t);
	evas_object_textblock_size_native_get(t->textblock, &w, &h);

	/* Fallback, just in case we hit a corner case */
	if (!h) h = 1;
	if (!w) w = 1;

	ewl_object_preferred_inner_size_set(EWL_OBJECT(t), (int)w, (int)h);
	t->dirty = FALSE;

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static void
ewl_text_plaintext_parse(Evas_Object *tb, char *txt)
{
	Evas_Textblock_Cursor *cursor;
	char *tmp;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("tb", tb);

	if (!txt) 
	{
		DRETURN(DLEVEL_STABLE);
	}

	/* we don't free this cursor as it is actually const
	 * Evas_Textblock_Cursor * and i'm casting it...  */
	cursor = (Evas_Textblock_Cursor *)evas_object_textblock_cursor_get(tb);
	for (tmp = txt; *tmp; tmp++) 
	{
		if (*tmp == '\n') 
		{
			*tmp = '\0';
			if (*txt) evas_textblock_cursor_text_append(cursor, txt);
			evas_textblock_cursor_format_append(cursor, "\n");
			*tmp = '\n';
			txt = tmp + 1;
		}
		else if (*tmp == '\r' && *(tmp + 1) == '\n') 
		{
			*tmp = '\0';
			if (*txt) evas_textblock_cursor_text_append(cursor, txt);
			evas_textblock_cursor_format_append(cursor, "\n");
			*tmp = '\r';
			tmp++;
			txt = tmp + 2;
		}
		else if (*tmp == '\t') 
		{
			*tmp = '\0';
			if (*txt) evas_textblock_cursor_text_append(cursor, txt);
			evas_textblock_cursor_format_append(cursor, "\t");
			*tmp = '\t';
			txt = tmp + 1;
		}
	}
	if (*txt) evas_textblock_cursor_text_append(cursor, txt);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/* This will give you the format string to pass to textblock based on the
 * context information. You _MUST_ free this format when your done with it */
static char *
ewl_text_format_get(Ewl_Text_Context *ctx)
{
	char *fmt;
	char *ptr;
	char style[512];
	char align[128];

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("ctx", ctx, NULL);

	fmt = NEW(char, 2048);

	style[0] = '\0';
	align[0] = '\0';

	/* create the style string */
	ptr = style;
	if (ctx->styles != EWL_TEXT_STYLE_NONE)
	{
		if ((ctx->styles & EWL_TEXT_STYLE_UNDERLINE) || 
				(ctx->styles & EWL_TEXT_STYLE_DOUBLE_UNDERLINE))
		{
			snprintf(ptr, sizeof(style) - strlen(style), 
					"underline=%s underline_color=#%02x%02x%02x%02x "
					"underline2_color=#%02x%02x%02x%02x ",
					((ctx->styles & EWL_TEXT_STYLE_UNDERLINE) ?
					 "on" : "double"),
					ctx->style_colors.underline.r,
					ctx->style_colors.underline.g,
					ctx->style_colors.underline.b,
					ctx->style_colors.underline.a,
					ctx->style_colors.double_underline.r,
					ctx->style_colors.double_underline.g,
					ctx->style_colors.double_underline.b,
					ctx->style_colors.double_underline.a);
		}
		else
			snprintf(ptr, sizeof(style) - strlen(style), "underline=off ");
		ptr = style + strlen(style);

		if (ctx->styles & EWL_TEXT_STYLE_STRIKETHROUGH)
			snprintf(ptr, sizeof(style) - strlen(style), 
					"strikethrough=on strikethrough_color=#%02x%02x%02x%02x ",
					ctx->style_colors.strikethrough.r,
					ctx->style_colors.strikethrough.g,
					ctx->style_colors.strikethrough.b,
					ctx->style_colors.strikethrough.a);
		else
			snprintf(ptr, sizeof(style) - strlen(style), "strkethrough=off ");
		ptr = style + strlen(style);

		if ((ctx->styles & EWL_TEXT_STYLE_SHADOW) 
				|| (ctx->styles & EWL_TEXT_STYLE_SOFT_SHADOW)
				|| (ctx->styles & EWL_TEXT_STYLE_FAR_SHADOW)
				|| (ctx->styles & EWL_TEXT_STYLE_OUTLINE)
				|| (ctx->styles & EWL_TEXT_STYLE_GLOW))
		{
			if (ctx->styles & EWL_TEXT_STYLE_GLOW)
				snprintf(ptr, sizeof(style) - strlen(style), 
						"style=glow glow_color=#%02x%02x%02x%02x ",
						ctx->style_colors.glow.r,
						ctx->style_colors.glow.g,
						ctx->style_colors.glow.b,
						ctx->style_colors.glow.a);

			else if (ctx->styles & EWL_TEXT_STYLE_OUTLINE)
			{
				if (ctx->styles & EWL_TEXT_STYLE_SHADOW)
					snprintf(ptr, sizeof(style) - strlen(style), 
							"style=outline_shadow ");
				else if (ctx->styles & EWL_TEXT_STYLE_SOFT_SHADOW)
					snprintf(ptr, sizeof(style) - strlen(style), 
							"style=outline_soft_shadow ");
				else
					snprintf(ptr, sizeof(style) - strlen(style), 
							"style=outline ");
				ptr = style + strlen(style);
				snprintf(ptr, sizeof(style) - strlen(style),
						"outline_color=#%02x%02x%02x%02x ", 
						ctx->style_colors.outline.r,
						ctx->style_colors.outline.g,
						ctx->style_colors.outline.b,
						ctx->style_colors.outline.a);
			}
			else if (ctx->styles & EWL_TEXT_STYLE_SHADOW)
				snprintf(ptr, sizeof(style) - strlen(style), 
						"style=shadow ");

			else if (ctx->styles & EWL_TEXT_STYLE_FAR_SHADOW)
			{
				if (ctx->styles & EWL_TEXT_STYLE_SOFT_SHADOW)
					snprintf(ptr, sizeof(style) - strlen(style), 
							"style=far_soft_shadow ");
				else
					snprintf(ptr, sizeof(style) - strlen(style), 
							"style=far_shadow ");
			}
			else if (ctx->styles & EWL_TEXT_STYLE_SOFT_SHADOW)
			{
				snprintf(ptr, sizeof(style) - strlen(style), 
						"style=soft_shadow ");
			}
			ptr = style + strlen(style);
			snprintf(ptr, sizeof(style) - strlen(style),
					"shadow_color=#%02x%02x%02x%02x ",
					ctx->style_colors.shadow.r,
					ctx->style_colors.shadow.g,
					ctx->style_colors.shadow.b,
					ctx->style_colors.shadow.a);
		}
		else
			snprintf(ptr, sizeof(style) - strlen(style), "style=off ");
		ptr = style + strlen(style);
	}
	else
	{
		snprintf(style, sizeof(style), "underline=off strikethrough=off style=off ");
	}

	/* create the alignment string */
	if (ctx->align > 0)
	{
		if (ctx->align & EWL_FLAG_ALIGN_CENTER)
			snprintf(align, sizeof(align), "align=center");

		else if (ctx->align & EWL_FLAG_ALIGN_RIGHT)
			snprintf(align, sizeof(align), "align=right");

		else
			snprintf(align, sizeof(align), "align=left");	
	}
	else
		snprintf(align, sizeof(align), "align=left");

	ptr = ewl_theme_path_get();
	/* create the formatting string */
	snprintf(fmt, 2048, "+font=fonts/%s font_source=%s font_size=%d "
			"backing_color=#%02x%02x%02x%02x color=#%02x%02x%02x%02x "
			"%s wrap=%s %s\n", ctx->font, 
			ptr, ctx->size,
			ctx->style_colors.bg.r, ctx->style_colors.bg.g,
			ctx->style_colors.bg.b, ctx->style_colors.bg.a,
			ctx->color.r, ctx->color.g,
			ctx->color.b, ctx->color.a, style, 
			((ctx->wrap) ? "word" : "off"), align);

	IF_FREE(ptr);

	DRETURN_PTR(fmt, DLEVEL_STABLE);
}

/* This will give you a cursor into the textblock setup for your given
 * index. You _MUST_ call evas_textblock_cursor_free(cursor) on this object
 * so it won't leak */
static Evas_Textblock_Cursor *
ewl_text_textblock_cursor_position(Ewl_Text *t, unsigned int idx)
{
	Evas_Textblock_Cursor *cursor;
	unsigned int cur_idx = 0;
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
				if ((cur_idx + 1) > idx)
				{
					evas_textblock_cursor_pos_set(cursor, idx - cur_idx);
					break;
				}
				else
					cur_idx ++;
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
			if ((cur_idx + pos) > idx)
			{
				evas_textblock_cursor_pos_set(cursor, idx - cur_idx);
				break;
			}
			cur_idx += pos;
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
		if (cur_idx > idx)
		{
			DWARNING("This shoudln't happen, breaking loop\n");
			break;
		}
	}

	DRETURN_PTR(cursor, DLEVEL_STABLE);
}

static unsigned int
ewl_text_textblock_cursor_to_index(Evas_Textblock_Cursor *cursor)
{
	unsigned int idx = 0;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("cursor", cursor, idx);

	/* this gives the index inside the _node_ the cursor points to, we
	 * then need to add the length of all the nodes before it plus any
	 * formatting nodes that are \n or \t */
	idx = evas_textblock_cursor_pos_get(cursor);
	while (evas_textblock_cursor_node_prev(cursor))
	{
		const char *txt;
		txt = evas_textblock_cursor_node_format_get(cursor);
		if (!txt) idx += evas_textblock_cursor_node_text_length_get(cursor);
		else if (!strcmp(txt, "\n")) idx ++;
		else if (!strcmp(txt, "\t")) idx ++;
	}

	DRETURN_INT(idx, DLEVEL_STABLE);
}

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
	if (OBSCURED(w)) DRETURN(DLEVEL_STABLE);

	t = EWL_TEXT(w);

	xx = CURRENT_X(w);
	yy = CURRENT_Y(w);
	hh = CURRENT_H(w);
	ww = CURRENT_W(w);

	if (t->textblock)
	{
		evas_object_move(t->textblock, xx, yy);
		evas_object_resize(t->textblock, ww, hh);

		if (t->dirty)
			ewl_text_display(t);

		/* XXX ewl_text_triggers_realize here? */
		ewl_text_triggers_configure(t);

		/* re-configure the selection to make sure it resizes
		 * if needed */
		if (t->selection)
			ewl_widget_configure(EWL_WIDGET(t->selection));
	}

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void
ewl_text_cb_reveal(Ewl_Widget *w, void *ev __UNUSED__, void *data __UNUSED__)
{
	Ewl_Text *t;
	Ewl_Embed *emb;
	Ewl_Text_Context *ctx;
	Evas_Textblock_Style *st;
	char *fmt, *fmt2;
	int len;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);
	DCHECK_TYPE("w", w, EWL_WIDGET_TYPE);

	t = EWL_TEXT(w);

	if (t->textblock) {
		DWARNING("We have a textblock when we shoudn't");
		DRETURN(DLEVEL_STABLE);
	}

	/* find the embed so we know the evas */
	emb = ewl_embed_widget_find(w);
	if (!emb) DRETURN(DLEVEL_STABLE);

	ctx = ewl_text_context_default_create(t);
	fmt = ewl_text_format_get(ctx);
	ewl_text_context_release(ctx);

	len = strlen(fmt) + 12;  /* 12 = strlen("DEFAULT=''") + \n + \0 */
	fmt2 = NEW(char, len);
	snprintf(fmt2, len, "DEFAULT='%s'\n", fmt);
	FREE(fmt);

	/* create the textblock */
	t->textblock = ewl_embed_object_request(emb, "textblock");
	if (!t->textblock)
		t->textblock = evas_object_textblock_add(emb->evas);

	if (t->textblock) {
		st = evas_textblock_style_new();
		evas_textblock_style_set(st, fmt2);
		evas_object_textblock_style_set(t->textblock, st);
		evas_textblock_style_free(st);

		FREE(fmt2);

		if (w->fx_clip_box)
			evas_object_clip_set(t->textblock, w->fx_clip_box);

		evas_object_pass_events_set(t->textblock, 1);

		evas_object_layer_set(t->textblock, ewl_widget_layer_sum_get(w));
		ewl_text_display(t);
		evas_object_show(t->textblock);
	}

	ewl_text_triggers_realize(t);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void
ewl_text_cb_obscure(Ewl_Widget *w, void *ev __UNUSED__, 
					void *data __UNUSED__)
{
	Ewl_Text *t;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);
	DCHECK_TYPE("w", w, EWL_WIDGET_TYPE);

	t = EWL_TEXT(w);

	if (t->textblock) {
		Ewl_Embed *emb;

		emb = ewl_embed_widget_find(w);
		evas_object_textblock_clear(t->textblock);
		ewl_embed_object_cache(emb, t->textblock);
		t->textblock = NULL;
	}

	ewl_text_triggers_unrealize(t);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

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

	ewl_text_tree_free(t->formatting.tree);
	t->formatting.tree = NULL;
	t->formatting.current = NULL;

	IF_FREE(t->text);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void
ewl_text_cb_mouse_down(Ewl_Widget *w, void *ev, void *data __UNUSED__)
{
	Ewl_Text *t;
	Ewl_Event_Mouse_Down *event;
	unsigned int idx = 0;
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
		t->selection = EWL_TEXT_TRIGGER(ewl_text_trigger_new(EWL_TEXT_TRIGGER_TYPE_SELECTION));
		ewl_container_child_append(EWL_CONTAINER(t), EWL_WIDGET(t->selection));
		ewl_widget_internal_set(EWL_WIDGET(t->selection), TRUE);

		ewl_text_trigger_start_pos_set(t->selection, 0);
		ewl_text_trigger_length_set(t->selection, 0);
		t->selection->text_parent = t;

		ewl_widget_show(EWL_WIDGET(t->selection));
	}

	idx = ewl_text_coord_index_map(EWL_TEXT(w), event->x, event->y);
	modifiers = ewl_ev_modifiers_get();
	if (modifiers & EWL_KEY_MODIFIER_SHIFT)
		ewl_text_selection_select_to(t->selection, idx);
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

		ewl_text_trigger_start_pos_set(t->selection, idx);
		ewl_text_trigger_base_set(t->selection, idx);
		ewl_text_trigger_length_set(t->selection, 0);
	}		   
	t->in_select = TRUE;

	ewl_text_trigger_position(t, t->selection);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}	   

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
		unsigned int idx = 0;
		idx = ewl_text_coord_index_map(EWL_TEXT(w), event->x, event->y);
		ewl_text_selection_select_to(t->selection, idx);
	}
	ewl_text_trigger_position(t, t->selection);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

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
		unsigned int idx = 0;

		idx = ewl_text_coord_index_map(EWL_TEXT(w), event->x, event->y);
		ewl_text_selection_select_to(t->selection, idx);
		ewl_text_trigger_position(t, t->selection);
	}

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

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

/*
 * Trigger stuff 
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

Ewl_Text_Trigger_Type 
ewl_text_trigger_type_get(Ewl_Text_Trigger *t)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("t", t, EWL_TEXT_TRIGGER_TYPE_NONE);
	DCHECK_TYPE_RET("t", t, EWL_TEXT_TRIGGER_TYPE, EWL_TEXT_TRIGGER_TYPE_NONE);

	DRETURN_INT(t->type, DLEVEL_STABLE);
}

void 
ewl_text_trigger_start_pos_set(Ewl_Text_Trigger *t, unsigned int pos)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("t", t);
	DCHECK_TYPE("t", t, EWL_TEXT_TRIGGER_TYPE);

	t->pos = pos;

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

unsigned int
ewl_text_trigger_start_pos_get(Ewl_Text_Trigger *t)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("t", t, 0);
	DCHECK_TYPE_RET("t", t, EWL_TEXT_TRIGGER_TYPE, 0);

	DRETURN_INT(t->pos, DLEVEL_STABLE);
}

void
ewl_text_trigger_length_set(Ewl_Text_Trigger *t, unsigned int len)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("t", t);
	DCHECK_TYPE("t", t, EWL_TEXT_TRIGGER_TYPE);

	t->len = len;

	/* if the length is set to 0 remove the areas */
	if (len == 0)
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

unsigned int
ewl_text_trigger_length_get(Ewl_Text_Trigger *t)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("t", t, 0);
	DCHECK_TYPE_RET("t", t, EWL_TEXT_TRIGGER_TYPE, 0);

	DRETURN_INT(t->len, DLEVEL_STABLE);
}

void
ewl_text_trigger_base_set(Ewl_Text_Trigger *t, unsigned int pos)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("t", t);
	DCHECK_TYPE("t", t, EWL_TEXT_TRIGGER_TYPE);

	t->base = pos;

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

unsigned int
ewl_text_trigger_base_get(Ewl_Text_Trigger *t)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("t", t, 0);
	DCHECK_TYPE_RET("t", t, EWL_TEXT_TRIGGER_TYPE, 0);

	DRETURN_INT(t->base, DLEVEL_STABLE);
}

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

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("t", t);
	DCHECK_PARAM_PTR("trig", trig);
	DCHECK_TYPE("t", t, EWL_TEXT_TYPE);
	DCHECK_TYPE("trig", trig, EWL_TEXT_TRIGGER_TYPE);

	if (trig->len == 0) 
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

	cur1 = ewl_text_textblock_cursor_position(t, trig->pos);
	cur2 = ewl_text_textblock_cursor_position(t, trig->pos + trig->len - 1);

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
ewl_text_triggers_shift(Ewl_Text *t, unsigned int pos, unsigned int len,
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
		if (pos >= (cur->pos + cur->len))
			continue;

		/* change is completely before the trigger */
		if ((pos + len) < cur->pos)
 		{
			if (del) cur->pos -= len;
			else cur->pos += len;
			continue;
		}

		if (del)
		{
			/* delete the entire trigger? */
			if ((pos <= cur->pos) && 
					((pos + len) >= (cur->pos + cur->len)))
			{
				int index;
				 
				index = ecore_list_index(t->triggers);
				if (index == 0)
				{
					DWARNING("is this possible?\n");
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
			if (pos <= cur->pos)
			{
				cur->len -= ((pos + len) - cur->pos);
				continue;
			}

			/* delete from the center of the trigger */
			if ((pos >= cur->pos) && 
					((pos + len) <= (cur->pos + cur->len)))
			{
				cur->len -= len;
				continue;
			}

			/* must be deleted from the end of the trigger then */
			cur->len = pos - cur->pos;
		}
		else
		{
			/* we are inserting, just see if we are before */
			if (pos < cur->pos)
			{
				cur->pos += len;
				continue;
			}
			cur->len += len;
		}
  	}
 
 	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

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
	if ((trigger->len == 0) || (trigger->pos > t->length)
			|| ((trigger->pos + trigger->len) > t->length))
		DRETURN(DLEVEL_STABLE);

	trigger->text_parent = t;

	/* only need to check for overlappign if this is a trigger (not a
	 * selection) */
	if (trigger->type == EWL_TEXT_TRIGGER_TYPE_TRIGGER)
	{
		ecore_list_goto_first(t->triggers);
		while ((cur = ecore_list_next(t->triggers)))
		{
			if (trigger->pos < cur->pos)
			{
				if ((trigger->pos + trigger->len) < cur->pos)
					break;
				/* overlapping triggers */
				DWARNING("Overlapping triggers are not allowed.\n");
				DRETURN(DLEVEL_STABLE);
			}

			if ((trigger->pos > (cur->pos + cur->len)))
				continue;

			/* do not allow overlapping triggers */
			if ((trigger->pos >= cur->pos) && (trigger->pos <= (cur->pos + cur->len)))
			{
				DWARNING("Overlapping triggers are not allowed.\n");
				DRETURN(DLEVEL_STABLE);
			}
		}
	}

	if (cur)
	{
		/* we need to set our position to the one before the one we 
		 * are on because the _next callin the while will have
		 * advanced usto the next node, but we want to insert
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
Ewl_Widget *
ewl_text_trigger_area_new(Ewl_Text_Trigger_Type type)
{
	Ewl_Text_Trigger_Area *area;

	DENTER_FUNCTION(DLEVEL_STABLE);

	area = NEW(Ewl_Text_Trigger_Area, 1);
	if (!area)
	{
		DRETURN_PTR(NULL, DLEVEL_STABLE);
	}

	if (!ewl_text_trigger_area_init(area, type))
	{
		ewl_widget_destroy(EWL_WIDGET(area));
		DRETURN_PTR(NULL, DLEVEL_STABLE);
	}

	DRETURN_PTR(area, DLEVEL_STABLE);
}

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
ewl_text_selection_select_to(Ewl_Text_Trigger *s, unsigned int idx)
{
	unsigned int start_pos;
	unsigned int base;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("s", s);
	DCHECK_TYPE("s", s, EWL_TEXT_TRIGGER_TYPE);

	base = ewl_text_trigger_base_get(s);
	start_pos = ewl_text_trigger_start_pos_get(s);
				
	if (idx <= start_pos)
	{
		if (idx < base)
		{
			ewl_text_trigger_start_pos_set(s, idx);
			ewl_text_trigger_length_set(s, base - idx);
		}
		else	
		{
			ewl_text_trigger_start_pos_set(s, base);
			ewl_text_trigger_length_set(s, idx - base);
		}
	}
	else
	{
		ewl_text_trigger_start_pos_set(s, base);
		ewl_text_trigger_length_set(s, idx - base);
	}

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/*
 * Ewl_Text_Context Stuff
 */
int
ewl_text_context_init(void)
{
	DENTER_FUNCTION(DLEVEL_STABLE);

	if (!context_hash) 
	{
		context_hash = ecore_hash_new(ecore_str_hash, ecore_str_compare);
		ecore_hash_set_free_key(context_hash, free);
		ecore_hash_set_free_value(context_hash, ewl_text_context_cb_free);
	}

	DRETURN_INT(TRUE, DLEVEL_STABLE);
}

void
ewl_text_context_shutdown(void)
{
	DENTER_FUNCTION(DLEVEL_STABLE);

	if (context_hash) {
		ecore_hash_destroy(context_hash);
		context_hash = NULL;
	}

	if (ewl_text_default_context)
		ewl_text_context_release(ewl_text_default_context);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

Ewl_Text_Context *
ewl_text_context_new(void)
{
	Ewl_Text_Context *tx;

	DENTER_FUNCTION(DLEVEL_STABLE);

	tx = NEW(Ewl_Text_Context, 1);
	tx->ref_count = 1;

	DRETURN_PTR(tx, DLEVEL_STABLE);;
}

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

	DRETURN_PTR(tx, DLEVEL_STABLE);
}

static Ewl_Text_Context *
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

	/* XXX grap the alignment and wrap data from the theme here */

	tx = ewl_text_context_find(tmp, EWL_TEXT_CONTEXT_MASK_NONE, NULL);
	ewl_text_context_release(tmp);

	/* setup the default context and acquire a ref on it so 
	 * it won't go away */
	ewl_text_default_context = tx;
	ewl_text_context_acquire(tx);

	DRETURN_PTR(tx, DLEVEL_STABLE);
}

static char *
ewl_text_context_name_get(Ewl_Text_Context *tx, unsigned int context_mask,
						Ewl_Text_Context *tx_change)
{
	char name[2048];
	char *t = NULL, *t2 = NULL;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("tx", tx, NULL);
	
	if (context_mask > 0)
	{
		DCHECK_PARAM_PTR_RET("tx_change", tx_change, NULL);
		if (!tx_change->font) t2 = "";
		else t2 = tx_change->font;
	}

	if (!tx->font) t = "";
	else t = tx->font;

	snprintf(name, sizeof(name), "f%ss%ds%da%dw%dr%dg%db%da%dcbg%d%d%d%dcg%d%d%d%d"
				"co%d%d%d%dcs%d%d%d%dcst%d%d%d%dcu%d%d%d%dcdu%d%d%d%d", 
		((context_mask & EWL_TEXT_CONTEXT_MASK_FONT) ? t2: t),
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

static Ewl_Text_Context *
ewl_text_context_find(Ewl_Text_Context *tx, unsigned int context_mask,
					Ewl_Text_Context *tx_change)
{
	char *t;
	Ewl_Text_Context *new_tx;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("tx", tx, NULL);

	/* only need the tx_change if we have a context mask */
	if (context_mask > 0)
		DCHECK_PARAM_PTR_RET("tx_change", tx_change, NULL);

	t = ewl_text_context_name_get(tx, context_mask, tx_change);
	new_tx = ecore_hash_get(context_hash, t);
	if (!new_tx)
	{
		if ((new_tx = ewl_text_context_dup(tx)))
		{
			if (context_mask & EWL_TEXT_CONTEXT_MASK_FONT)
			{
				IF_FREE(new_tx->font);
				new_tx->font = strdup(tx_change->font);
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

			ecore_hash_set(context_hash, strdup(t), new_tx);
		}
	}
	if (new_tx) ewl_text_context_acquire(new_tx);
	FREE(t);

	DRETURN_PTR(new_tx, DLEVEL_STABLE);
}

void
ewl_text_context_acquire(Ewl_Text_Context *tx)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("tx", tx);

	tx->ref_count ++;

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void
ewl_text_context_release(Ewl_Text_Context *tx)
{
	char *t;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("tx", tx);

	tx->ref_count --;
	if (tx->ref_count > 0) return;

	t = ewl_text_context_name_get(tx, 0, NULL);
	ecore_hash_remove(context_hash, t);

	IF_FREE(tx->font);
	FREE(tx);
	FREE(t);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/*
 * if they contain the same data the they should be the same pointer... 
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
	char *t;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("tx", tx);

	if (!tx->font) t = "";
	else t = tx->font;

	printf("%sfont: %s\n"
		"%ssize %d\n"
		"%sstyle %d\n"
		"%salign %d\n"
		"%swrap %d\n"
		"%sred %d\n"
		"%sgreen %d\n"
		"%sblue %d\n" 
		"%salpha %d\n", 
			indent, t, indent, tx->size, indent, 
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

/*
 * Ewl_Text_Tree stuff
 */
Ewl_Text_Tree *
ewl_text_tree_new(void)
{
	Ewl_Text_Tree *tree;

	DENTER_FUNCTION(DLEVEL_STABLE);

	tree = NEW(Ewl_Text_Tree, 1);
	if (!tree)
	{
		DRETURN_PTR(NULL, DLEVEL_STABLE);
	}

	DRETURN_PTR(tree, DLEVEL_STABLE);
}

void
ewl_text_tree_free(Ewl_Text_Tree *tree)
{
	DENTER_FUNCTION(DLEVEL_STABLE);

	if (!tree) DRETURN(DLEVEL_STABLE);

	tree->parent = NULL;
	if (tree->children) 
	{
		Ewl_Text_Tree *child;

		while ((child = ecore_list_remove_first(tree->children)))
			ewl_text_tree_free(child);

		ecore_list_destroy(tree->children);
		tree->children = NULL;
	}

	if (tree->tx)
	{
		ewl_text_context_release(tree->tx);
		tree->tx = NULL;
	}
	FREE(tree);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

Ewl_Text_Tree *
ewl_text_tree_node_get(Ewl_Text_Tree *tree, unsigned int idx, 
					unsigned int inclusive)
{
	Ewl_Text_Tree *child = NULL, *last = NULL;
	unsigned int count = 0;

	DENTER_FUNCTION(DLEVEL_STABLE);
 	DCHECK_PARAM_PTR_RET("tree", tree, NULL);

	/* make sure the idx is in the tree */
	if (idx > tree->length)
		DRETURN_PTR(NULL, DLEVEL_STABLE);
	
	if ((!tree->children) || (ecore_list_nodes(tree->children) == 0))
		DRETURN_PTR(tree, DLEVEL_STABLE);

	child = tree;
	ecore_list_goto_first(tree->children);
	while ((child = ecore_list_next(tree->children)))
	{
		last = child;

		/* we don't always want this to be inclusive ... */
		if (((inclusive && ((count + child->length) >= idx)))
				|| (!inclusive && ((count + child->length > idx))))
		{
			child = ewl_text_tree_node_get(child, idx - count, inclusive);
			break;
		}
		count += child->length;
	}

	/* we've gone to the end of hte list and didn't find anything, use
	 * the last node in the list */
	if (!child) child = last;

	DRETURN_PTR(child, DLEVEL_STABLE);
}

void
ewl_text_tree_current_node_set(Ewl_Text *t, Ewl_Text_Tree *current)
{
 	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("t", t);
	DCHECK_TYPE("t", t, EWL_TEXT_TYPE);
	 
	if (t->formatting.current == current)
 		DRETURN(DLEVEL_STABLE);

	/* if the current node has no length then we can kill it off */
	if ((t->formatting.current) && (t->formatting.current->length == 0))
 	{
		/* remove the current node from the parent */
		if (t->formatting.current->parent)
 		{
			Ecore_List *children;
			Ewl_Text_Tree *c;
			int idx, idx2;

			children = t->formatting.current->parent->children;

			idx = ecore_list_index(children);
			c = ecore_list_goto(children, t->formatting.current);
			idx2 = ecore_list_index(children);

			if (c) ecore_list_remove(children);

			/* we removed from before us, don't want to skip an
			 * entry */
			if (idx2 < idx) idx --;
			ecore_list_goto_index(children, idx);
		}
		ewl_text_tree_free(t->formatting.current);
 	}
	t->formatting.current = current;
	 
 	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void
ewl_text_tree_insert(Ewl_Text *t, unsigned int idx, unsigned int len)
{
	Ewl_Text_Tree *parent;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("t", t);
	DCHECK_TYPE("t", t, EWL_TEXT_TYPE);
	
	/* if we have a current node, we just insert there as this is where
	 * the formatting has been setup. If the cursor was moved then
	 * current would have been set to NULL. */
	if (t->formatting.current)
		parent = t->formatting.current;
	else
 	{
		parent = ewl_text_tree_node_get(t->formatting.tree, idx, TRUE);
		if (!parent)
			DRETURN(DLEVEL_STABLE);
 	}

	parent->length += len;
	while ((parent = parent->parent))
		parent->length += len;

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void
ewl_text_tree_delete(Ewl_Text *t, unsigned int idx, unsigned int len)
{
	Ewl_Text_Tree *child = NULL, *parent = NULL;
	int remaining = 0, removed = 0;
	int node_remaining = 0;
	unsigned int pos = 0;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("t", t);
	DCHECK_TYPE("t", t, EWL_TEXT_TYPE);

	child = ewl_text_tree_node_get(t->formatting.tree, idx, FALSE);
	if (!child) DRETURN(DLEVEL_STABLE);

	pos = ewl_text_tree_idx_start_count_get(t->formatting.tree, idx, TRUE);
	node_remaining = child->length - (idx - pos);

	/* length is fully inside the child */
	if ((unsigned int)node_remaining >= len)
   	{
		child->length -= len;
		removed = len;
 	}
	else 
   	{
		remaining = len - node_remaining;
		removed = node_remaining;
		child->length -= removed;
 	}

	/* update the parents with the changed size */
	parent = child->parent;

	/* this node is empty, remove it */
	if (child->length == 0)
		ewl_text_tree_node_delete(t, child);

	/* update parents */
	while (parent)
	{
		Ewl_Text_Tree *c;
					 
		c = parent;
		c->length -= removed;
		parent = c->parent;

		/* remove the node if zero length */
		if (c->length == 0)
			ewl_text_tree_node_delete(t, c);
	}

	/* we have more text to remove ... */
	if (remaining > 0)
		ewl_text_tree_delete(t, idx, remaining);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static void
ewl_text_tree_node_delete(Ewl_Text *t, Ewl_Text_Tree *tree)
{
	Ewl_Text_Tree *parent;
	int current;
		  
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("t", t);
	DCHECK_PARAM_PTR("tree", tree);
		  
	parent = tree->parent;
		  
	/* we don't want to destroy the root node ... */
	if (parent == NULL)
	{
		if (tree->children)
		{
			Ewl_Text_Tree *child;

			while ((child = ecore_list_remove_first(tree->children)))
				ewl_text_tree_free(child);

			ecore_list_destroy(tree->children);
			tree->children = NULL;
		}
					 
		if (tree->tx)
		{
			ewl_text_context_release(tree->tx);
			tree->tx= NULL;
		}
							 
		tree->length = 0;
		tree->tx = ewl_text_context_default_create(t);

		/* if the whole tree is gone make sure we get 
		 * rid of the current pointer */
		t->formatting.current = NULL;

		DRETURN(DLEVEL_STABLE);
	}

	/* store the current list position, remove the child and then return to
	 * the current position */
	current = ecore_list_index(parent->children);
	ecore_list_goto(parent->children, tree);
	ecore_list_remove(parent->children);
	ecore_list_goto_index(parent->children, current);

	ewl_text_tree_free(tree);
							 
	DLEAVE_FUNCTION(DLEVEL_STABLE);
}
 
Ewl_Text_Context *
ewl_text_tree_context_get(Ewl_Text_Tree *tree, unsigned int idx)
{
	Ewl_Text_Tree *child;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("tree", tree, NULL);

	child = ewl_text_tree_node_get(tree, idx, TRUE);
	if (!child) DRETURN_PTR(NULL, DLEVEL_STABLE);

	DRETURN_PTR(child->tx, DLEVEL_STABLE);
}

void
ewl_text_tree_context_set(Ewl_Text *t, unsigned int context_mask,
						Ewl_Text_Context *tx)
{
	Ewl_Text_Tree *tree = NULL;
	Ewl_Text_Context *old_tx;
	  
  	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("t", t);
	DCHECK_PARAM_PTR("tx", tx);
	DCHECK_TYPE("t", t, EWL_TEXT_TYPE);
		
	/* just update current if possible */
	if (t->formatting.current)
	{
		tree = t->formatting.current;

		/* if the tree is current, and it's length is greater then
		 * zero we won't be inserting into it, so make it non current 
		 * and lookup our node */
		if (tree->length > 0)
		{
			ewl_text_tree_current_node_set(t, NULL);
			tree = ewl_text_tree_node_get(t->formatting.tree, t->cursor_position, TRUE);
		}
	}
	else
		tree = ewl_text_tree_node_get(t->formatting.tree, t->cursor_position, TRUE);
		
	if (!tree)
	{
		printf("no current node in context set %d, %d\n", t->cursor_position, t->length);
		DRETURN(DLEVEL_STABLE);
	}
		 
	if (tree->length == 0)
	{
		t->formatting.current = tree;

		/* set the current context */
		old_tx = t->formatting.current->tx;
		t->formatting.current->tx= ewl_text_context_find(old_tx, 
							context_mask, tx);
		ewl_text_context_release(old_tx);
	}
	else
	{
		Ewl_Text_Tree *current;
		unsigned int count = 0;
				 
		count = ewl_text_tree_idx_start_count_get(t->formatting.tree, 
								t->cursor_position, TRUE);
		current = ewl_text_tree_node_split(tree, count, 
						t->cursor_position, 0, 
						context_mask, tx);

		ewl_text_tree_current_node_set(t, current);
	}
				 
	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void
ewl_text_tree_context_apply(Ewl_Text *t, unsigned int context_mask,
				Ewl_Text_Context *tx, unsigned int idx,
				unsigned int len)
 {
	Ewl_Text_Tree *child;
	int node_remaining = 0, remaining = 0;
	unsigned int pos = 0, next_idx = 0;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("t", t);
	DCHECK_PARAM_PTR("tx", tx);
	DCHECK_TYPE("t", t, EWL_TEXT_TYPE);
			 
	/* remove the current node as it isnt' valid after this */
	ewl_text_tree_current_node_set(t, NULL);

	/* we don't want the teststo be inclusive cuz we need to move tho
	 * the next node if we are at the right edge of a node */
	child = ewl_text_tree_node_get(t->formatting.tree, idx, FALSE);
	if (!child) DRETURN(DLEVEL_STABLE);
			 
	pos = ewl_text_tree_idx_start_count_get(t->formatting.tree, idx, FALSE);
	node_remaining = child->length - (idx - pos);

	/* length is fully inside the child */
	if ((unsigned int)node_remaining >= len)
		ewl_text_tree_node_split(child, pos, idx, len, context_mask, tx);
	else 
	{
		ewl_text_tree_node_split(child, pos, idx, node_remaining,
							context_mask, tx);
		remaining = len - node_remaining;
		next_idx = idx + node_remaining;
			 
		/* we have more text to apply too ... */
		if (remaining > 0)
			ewl_text_tree_context_apply(t, context_mask, tx, next_idx, remaining);
	}
			 
	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void
ewl_text_tree_context_style_apply(Ewl_Text *t, Ewl_Text_Style style,
					unsigned int idx, unsigned int len,
							unsigned int invert)
{
	Ewl_Text_Tree *child;
	int node_remaining = 0, remaining = 0;
	unsigned int pos = 0, next_idx = 0;
	Ewl_Text_Context *tx;
	  
  	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("t", t);
	DCHECK_TYPE("t", t, EWL_TEXT_TYPE);
		 
	/* remove the current node as it isnt' valid after this */
	ewl_text_tree_current_node_set(t, NULL);
		 
	child = ewl_text_tree_node_get(t->formatting.tree, idx, TRUE);
	if (!child) DRETURN(DLEVEL_STABLE);
		 
	tx = ewl_text_context_new();
	tx->styles = child->tx->styles;

	if (invert) tx->styles ^= style;
	else tx->styles |= style;

	pos = ewl_text_tree_idx_start_count_get(t->formatting.tree, idx, TRUE);
	node_remaining = child->length - (idx - pos);

	/* length is fully inside the child */
	if ((unsigned int)node_remaining >= len)
		ewl_text_tree_node_split(child, pos, idx, len, 
						EWL_TEXT_CONTEXT_MASK_STYLES, tx);
	else 
	{
		ewl_text_tree_node_split(child, pos, idx, node_remaining,
						EWL_TEXT_CONTEXT_MASK_STYLES, tx);
		remaining = len - node_remaining;
		next_idx = idx + node_remaining;
	}
	ewl_text_context_release(tx);
		 
	/* we have more text to apply too ... */
	if (remaining > 0)
		ewl_text_tree_context_style_apply(t, style, next_idx, remaining, invert);
		 
	DLEAVE_FUNCTION(DLEVEL_STABLE);
}
 
void
ewl_text_tree_context_style_remove(Ewl_Text *t, Ewl_Text_Style style, 
					unsigned int idx, unsigned int len)
{
	Ewl_Text_Tree *child;
	int node_remaining = 0, remaining = 0;
	unsigned int pos = 0, next_idx = 0;
	Ewl_Text_Context *tx;
	  
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("t", t);
	DCHECK_TYPE("t", t, EWL_TEXT_TYPE);
	  
	/* remove the current node as it isnt' valid after this */
	ewl_text_tree_current_node_set(t, NULL);
	  
	child = ewl_text_tree_node_get(t->formatting.tree, idx, TRUE);
	if (!child) DRETURN(DLEVEL_STABLE);

	tx = ewl_text_context_new();
	tx->styles = child->tx->styles;
	tx->styles &= ~style;

	pos = ewl_text_tree_idx_start_count_get(t->formatting.tree, idx, TRUE);
	node_remaining = child->length - (idx - pos);

	/* length is fully inside the child */
	if ((unsigned int)node_remaining >= len)
		ewl_text_tree_node_split(child, pos, idx, len, 
						EWL_TEXT_CONTEXT_MASK_STYLES, tx);
	else 
	{
		ewl_text_tree_node_split(child, pos, idx, node_remaining,
						EWL_TEXT_CONTEXT_MASK_STYLES, tx);
		remaining = len - node_remaining;
		next_idx = idx + node_remaining;
	}
	ewl_text_context_release(tx);
			  
	/* we have more text to apply too ... */
	if (remaining > 0)
		ewl_text_tree_context_style_remove(t, style, next_idx, remaining);
			  
	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static int
ewl_text_tree_idx_start_count_get(Ewl_Text_Tree *tree, unsigned int idx, 
						unsigned int inclusive)
{
	int count = 0;
	Ewl_Text_Tree *child, *parent;
	  
  	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("tree", tree, 0);

	child = ewl_text_tree_node_get(tree, idx, inclusive);
	parent = child->parent;
	while (parent)
	{
		Ewl_Text_Tree *sibling;

		/* count up the siblings before us */
		ecore_list_goto_first(parent->children);
		while ((sibling = ecore_list_next(parent->children)) != child)
			count += sibling->length;

		child = parent;
		parent = child->parent;
	}

	DRETURN_INT(count, DLEVEL_STABLE);
}

static Ewl_Text_Tree * 
ewl_text_tree_node_split(Ewl_Text_Tree *tree, unsigned int count, unsigned int pos, 
					unsigned int len, unsigned int context_mask, 
								Ewl_Text_Context *tx)
{
	Ewl_Text_Tree *t1 = NULL, *t2 = NULL, *current = NULL;
	Ewl_Text_Context *old_tx;
	unsigned int diff;
						  
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("tree", tree, NULL);
	DCHECK_PARAM_PTR_RET("tx", tx, NULL);

	diff = pos - count;

	if (diff > 0)
	{
		/* the start */
		t1 = ewl_text_tree_new();
		t1->parent = tree;
		t1->length = diff;
		t1->tx = tree->tx;
		ewl_text_context_acquire(t1->tx);
	}

	if ((tree->length - diff - len) > 0)
	{
		/* the rest */
		t2 = ewl_text_tree_new();
		t2->parent = tree;
		t2->length = tree->length - diff - len;
		t2->tx = tree->tx;
		ewl_text_context_acquire(t2->tx);
	}
						  
	old_tx = tree->tx;
	tree->tx = NULL;

	/* only do this if we have at least one sibling */
	if (t1 || t2)
	{
		if (!tree->children)
			tree->children = ecore_list_new();
									 
		if (t1) ecore_list_append(tree->children, t1);
									 
		/* the new current node */
		current = ewl_text_tree_new();
		current->parent = tree;
		current->length = len;
		current->tx = ewl_text_context_find(old_tx, context_mask, tx);
		ecore_list_append(tree->children, current);

		if (t2) ecore_list_append(tree->children, t2);
	}
	else
	{
		/* we have no children, so just update the context on the
		 * tree */
		tree->tx = ewl_text_context_find(old_tx, context_mask, tx);
	}
	ewl_text_context_release(old_tx);

	DRETURN_PTR(current, DLEVEL_STABLE);
}

/* this just merges nodes back into their parent or deletes the parent if it
 * has no children */
static void
ewl_text_tree_shrink(Ewl_Text_Tree *tree)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("tree", tree);

	/* if we have one child left we need to merge it up to us */
	if (ecore_list_nodes(tree->children) == 1)
	{
		Ewl_Text_Tree *child;
		child = ecore_list_goto_first(tree->children);

		ecore_list_destroy(tree->children);
		tree->children = NULL;

		tree->tx = child->tx;
		ewl_text_context_acquire(tree->tx);

		ewl_text_tree_free(child);
	} 
	else if (ecore_list_nodes(tree->children) == 0)
	{
		if (tree->parent != NULL)
		{
			ecore_list_goto(tree->parent->children, tree);
			ecore_list_remove(tree->parent->children);
			ewl_text_tree_shrink(tree->parent);
		}
		else
		{
			/* we must be the root and there is nothing left in
			 * the tree, just reset everything */
			ecore_list_destroy(tree->children);
			tree->length = 0;
			tree->children = NULL;
			tree->tx = NULL;
		}
	}

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/* search for siblings that are the same and merge the nodes */
void
ewl_text_tree_condense(Ewl_Text_Tree *tree)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("tree", tree);

	/* XXX write this sometime ... */

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void
ewl_text_tree_dump(Ewl_Text_Tree *tree, const char *indent)
{
	Ewl_Text_Tree *child;
	int len = 0;
	char *t;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("tree", tree);

	printf("%s---\n", indent);
	printf("%snode (%d)\n", indent, tree->length);

	/* if we have a context, print it */
	if (tree->tx) ewl_text_context_print(tree->tx, indent);
	else printf("%sNo Context\n", indent);

	if (!tree->children)
		DRETURN(DLEVEL_STABLE);

	len = strlen(indent) + 3;
	t = NEW(char, len);
	if (!t) DRETURN(DLEVEL_STABLE);

	snprintf(t, len, "%s  ", (char *)indent);

	/* else we need to go through the children */
	ecore_list_goto_first(tree->children);
	while ((child = ecore_list_next(tree->children)))
		ewl_text_tree_dump(child, t);

	FREE(t);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static void
ewl_text_tree_walk(Ewl_Text *t)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("t", t);
	DCHECK_TYPE("t", t, EWL_TEXT_TYPE);

	if (!t->text) 
		DRETURN(DLEVEL_STABLE);

	ewl_text_tree_node_walk(t, t->formatting.tree, 0);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static void
ewl_text_tree_node_walk(Ewl_Text *t, Ewl_Text_Tree *tree, unsigned int pos)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("tree", tree);
	DCHECK_PARAM_PTR("t", t);
	DCHECK_TYPE("t", t, EWL_TEXT_TYPE);

	/* if we have a context we have something to say */
	if (tree->tx)
	{
		char *fmt, *ptr, tmp;
		Evas_Textblock_Cursor *cursor;

		fmt = ewl_text_format_get(tree->tx);

		/* we don't free this cursor as it is actually const
		 * Evas_Textblock_Cursor * and i'm casting it...  */
		cursor = (Evas_Textblock_Cursor *)evas_object_textblock_cursor_get(t->textblock);
		evas_textblock_cursor_format_append(cursor, fmt);
		FREE(fmt);

		ptr = t->text + pos;
		tmp = *(ptr + tree->length);
		*(ptr + tree->length) = '\0';

		ewl_text_plaintext_parse(t->textblock, ptr);
		*(ptr + tree->length) = tmp;	

		evas_textblock_cursor_format_append(cursor, "-");
	}
	else if (tree->children)
	{
		Ewl_Text_Tree *child;

		ecore_list_goto_first(tree->children);
		while ((child = ecore_list_next(tree->children)))
		{
			ewl_text_tree_node_walk(t, child, pos);
			pos += child->length;
		}
	}

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

