#include <Ewl.h>
#include "ewl_debug.h"
#include "ewl_macros.h"

/* This counts how many deletes we have before trigger a condense operation
 * on the btree */
#define EWL_TEXT_BTREE_CONDENSE_COUNT  5

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
static int ewl_text_context_compare(Ewl_Text_Context *a, Ewl_Text_Context *b);
static void ewl_text_context_print(Ewl_Text_Context *tx, char *indent);
static Ewl_Text_Context *ewl_text_context_dup(Ewl_Text_Context *old);
static char *ewl_text_context_name_get(Ewl_Text_Context *tx, 
			unsigned int context_mask, Ewl_Text_Context *tx_change);
static Ewl_Text_Context *ewl_text_context_find(Ewl_Text_Context *tx,
			unsigned int context_mask, Ewl_Text_Context *tx_change);
static Ewl_Text_Context *ewl_text_context_default_create(Ewl_Text *t);
static void ewl_text_display(Ewl_Text *t);
static void ewl_text_plaintext_parse(Evas_Object *tb, char *txt);
static void ewl_text_btree_walk(Ewl_Text *t);
static void ewl_text_btree_node_walk(Ewl_Text_BTree *tree, Ewl_Text *t, 
						unsigned int text_pos);
static void ewl_text_btree_shrink(Ewl_Text_BTree *tree);
static void ewl_text_op_set(Ewl_Text *t, unsigned int context_mask, 
						Ewl_Text_Context *tx_change);

static void ewl_text_triggers_remove(Ewl_Text *t);
static void ewl_text_trigger_cb_free(void *value, void *data);
static void ewl_text_triggers_shift(Ewl_Text *t, unsigned int pos, 
						unsigned int len);
static void ewl_text_trigger_position(Ewl_Text *t, Ewl_Text_Trigger *trig, 
					int tb_length, unsigned int *cur_idx, 
							unsigned int *cur_tb_idx);

static void ewl_text_trigger_add(Ewl_Text *t, Ewl_Text_Trigger *trigger);
static void ewl_text_trigger_del(Ewl_Text *t, Ewl_Text_Trigger *trigger);
static void ewl_text_trigger_area_add(Ewl_Text *t, Ewl_Text_Trigger *cur, 
			Evas_Coord x, Evas_Coord y, Evas_Coord w, Evas_Coord h);

static void ewl_text_selection_select_to(Ewl_Text_Trigger *s, unsigned int idx);

/**
 * @param text: The text to set into the widget
 * @return Returns a new Ewl_Text widget on success, NULL on failure.
 */
Ewl_Widget *
ewl_text_new(const char *text)
{
	Ewl_Widget *w;
	DENTER_FUNCTION(DLEVEL_STABLE);

	w = NEW(Ewl_Text, 1);
	if (!w)
	{
		DRETURN_PTR(NULL, DLEVEL_STABLE);
	}

	if (!ewl_text_init(EWL_TEXT(w), text))
	{
		ewl_widget_destroy(w);
		DRETURN_PTR(NULL, DLEVEL_STABLE);
	}

	DRETURN_PTR(w, DLEVEL_STABLE);
}

/**
 * @param t: The Ewl_Text widget
 * @param text: The text to set into the widget
 * @return Returns TRUE on successfully init or FALSE on failure
 */
int
ewl_text_init(Ewl_Text *t, const char *text)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("t", t, FALSE);

	if (!ewl_container_init(EWL_CONTAINER(t), "text"))
	{
		DRETURN_INT(FALSE, DLEVEL_STABLE);
	}
	ewl_widget_inherit(EWL_WIDGET(t), "text");

	t->triggers = ecore_list_new();

	/* create the formatting tree before we do any formatting */
	t->formatting = ewl_text_btree_new();
	if (!t->formatting) 
	{
		DRETURN_INT(FALSE, DLEVEL_STABLE);
	}
	t->current_context = NULL;

	if (text)
		ewl_text_text_set(t, text);

	ewl_callback_append(EWL_WIDGET(t), EWL_CALLBACK_CONFIGURE, 
					ewl_text_cb_configure, NULL);
	ewl_callback_append(EWL_WIDGET(t), EWL_CALLBACK_REALIZE,
					ewl_text_cb_realize, NULL);
	ewl_callback_append(EWL_WIDGET(t), EWL_CALLBACK_UNREALIZE,
					ewl_text_cb_unrealize, NULL);
	ewl_callback_append(EWL_WIDGET(t), EWL_CALLBACK_SHOW,
					ewl_text_cb_show, NULL);
	ewl_callback_append(EWL_WIDGET(t), EWL_CALLBACK_HIDE,
					ewl_text_cb_hide, NULL);
	ewl_callback_append(EWL_WIDGET(t), EWL_CALLBACK_DESTROY,
					ewl_text_cb_destroy, NULL);
	ewl_callback_append(EWL_WIDGET(t), EWL_CALLBACK_MOUSE_DOWN,
					ewl_text_cb_mouse_down, NULL);
	ewl_callback_append(EWL_WIDGET(t), EWL_CALLBACK_MOUSE_UP,
					ewl_text_cb_mouse_up, NULL);

	ewl_container_add_notify_set(EWL_CONTAINER(t), 
					ewl_text_cb_child_add);
	ewl_container_remove_notify_set(EWL_CONTAINER(t), 
					ewl_text_cb_child_del);

	/* create the selection */
	t->selection = EWL_TEXT_TRIGGER(ewl_text_trigger_new(EWL_TEXT_TRIGGER_TYPE_SELECTION));
	ewl_text_trigger_start_pos_set(t->selection, 0);
	ewl_text_trigger_length_set(t->selection, 0);
	t->selection->text_parent = t;
	ewl_callback_append(EWL_WIDGET(t->selection), EWL_CALLBACK_CONFIGURE,
					ewl_text_selection_cb_configure, NULL);
	ewl_container_child_append(EWL_CONTAINER(t), EWL_WIDGET(t->selection));

	DRETURN_INT(TRUE, DLEVEL_STABLE);
}

/**
 * @param t: The Ewl_Text to get the length from
 * @return Returns the length of the text in the widget @a t
 */
int
ewl_text_length_get(Ewl_Text *t)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("t", t, 0);

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
	Evas_Coord tx, ty, tw, th;
	Evas_Bool ret;
	char *ptr;
	unsigned int tb_idx = 0;
	int fiddled = 0;
	int i = 0;


	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("t", t);

	/* make sure we are in the text */
	if (idx > t->length) idx = t->length;

	/* if we have no text then use the default font size */
	if ((!t->textblock) || (!t->text))
	{
		if (x) *x = 0;
		if (y) *y = 0;
		if (w) *w = 0;
		if (h)
		{
			if (t->current_context)
				*h = t->current_context->size;
			else
				*h = ewl_theme_data_int_get(EWL_WIDGET(t), "font_size");
		}
		DRETURN(DLEVEL_STABLE);
	}

	/* we need to remove the \n \r \t from the index count so that the
	 * textblock number will be right */
	for (ptr = t->text; *ptr; ptr ++)
	{
		if (i == idx) break;

		if ((*ptr != '\n') && (*ptr != '\r') && (*ptr != '\t')) 
			tb_idx ++;
		i++;
	}

	/* length is one after the end of the text, so make sure we take one
	 * less as tb won't recognize length itself, it will return the same
	 * as if you clicked off the start */
	if (tb_idx == t->length)
	{
		tb_idx --;
		fiddled = 1;
	}

	ret = evas_object_textblock_char_pos_get(t->textblock, tb_idx, &tx, &ty, 
								&tw, &th);

	/* we had to add the width of the last char into the x value given
	 * by tb in order to get stuck at the end of the text */
	if (fiddled)
		tx += tw;

	if (x) *x = (int)(tx + CURRENT_X(t));
	if (y) *y = (int)(ty + CURRENT_Y(t));
	if (w) *w = (int)tw;
	if (h) *h = (int)th;

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
	int tb_idx, i = 0;
	unsigned int idx = 0;
	char *ptr;
	Evas_Coord tcx, tcw;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("t", t, 0);

	if ((!t->textblock) || (!t->text))
	{
		DRETURN_INT(0, DLEVEL_STABLE);
	}

	tb_idx = evas_object_textblock_char_coords_get(t->textblock,
			(Evas_Coord)(x - CURRENT_X(t)),
			(Evas_Coord)(y - CURRENT_Y(t)), 
			&tcx, NULL, &tcw, NULL);

	/* if this is less then 0 then we clicked off of one end of the
	 * textblock or the other. if the click position is inside the size
	 * of a char we are at the start, else we are at the end */
	/* XXX WRONG */
	if (tb_idx < 0)
	{
		int cx, size;

		size = ewl_text_font_size_get(t, 0);
		cx = x - CURRENT_X(t);

		if (cx < size)
		{
			DRETURN_INT(0, DLEVEL_STABLE);
		}
		else
		{
			DRETURN_INT(t->length, DLEVEL_STABLE);
		}
	}
	else
	{
		Evas_Coord xpos;

		xpos = (Evas_Coord)(x - CURRENT_X(t));

		/* if we clicked on the right side of the char move us over
		 * to next index over */
		if (xpos > (tcx + (tcw / 2)))
			tb_idx ++;
	}

	idx = tb_idx;

	/* need to add \n \r \t stuff back into the count */
	for (ptr = t->text; *ptr; ptr ++)
	{
		if (i == tb_idx) break;

		if ((*ptr == '\n') || (*ptr == '\r') || (*ptr == '\t')) 
			idx ++;
		else
			i ++;
	}

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

	DRETURN_PTR(((t->text) ? strdup(t->text) : NULL), DLEVEL_STABLE);
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
	
	ewl_text_text_insert(t, NULL, 0);
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
	int len;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("t", t);

	if (!text)
	{
		if (t->length > 0)
		{
			ewl_text_cursor_position_set(t, 0);
			ewl_text_text_delete(t, t->length);
		}
		t->text = NULL;
		t->length = 0;
		t->cursor_position = 0;

	}
	else if (!t->text)
	{
		t->text = strdup(text);
		t->length = strlen(text);

		if (!t->current_context)
			t->current_context = ewl_text_context_default_create(t);

		ewl_text_btree_text_context_insert(t->formatting, t->current_context, 
									idx, t->length);
		t->cursor_position = t->length;
	}
	else
	{
		char *new = NULL;

		len = strlen(text);
		if (!t->current_context)
			t->current_context = ewl_text_context_default_create(t);

		new = malloc(sizeof(char) * (t->length + len + 1));
		if (idx == 0)
			sprintf(new, "%s%s", text, t->text);
		else if (idx == t->length)
			sprintf(new, "%s%s", t->text, text);
		else
		{
			memcpy(new, t->text, idx);
			memcpy(new + idx, text, len);
			memcpy(new + idx + len, t->text + idx, t->length - idx);
		}
		FREE(t->text);
		t->text = new;
		t->length = t->length + len;
		t->text[t->length] = '\0';

		ewl_text_btree_text_context_insert(t->formatting, t->current_context, idx, len);
		t->cursor_position = idx + len;
	}

	if (text)
		ewl_text_triggers_shift(t, idx, strlen(text));
	else
		ewl_text_triggers_remove(t);

	if (REALIZED(t))
		ewl_text_display(t);

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
	char *old, *ptr;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("t", t);

	if ((length == 0) || (t->cursor_position >= t->length))
	{
		DRETURN(DLEVEL_STABLE);
	}

	if (!t->text)
	{
		DRETURN(DLEVEL_STABLE);
	}

	if ((t->length - t->cursor_position) < length)
		length = t->length - t->cursor_position;

	t->length -= length;
	if (t->length > 0)
	{
		old = t->text;
		*(old + t->cursor_position) = '\0';
		ptr = old + t->cursor_position + length;

		t->text = malloc(sizeof(char) * (t->length + 1));
		snprintf(t->text, (t->length + 1), "%s%s", ((old) ? old : ""), ((ptr) ? ptr : ""));
		IF_FREE(old);
	}
	else
	{
		IF_FREE(t->text);
	}

	/* cleanup the nodes in the btree */
	ewl_text_btree_text_delete(t->formatting, t->cursor_position, length);
	t->delete_count ++;

	if (t->delete_count == EWL_TEXT_BTREE_CONDENSE_COUNT)
	{
		ewl_text_btree_condense(t->formatting);
		t->delete_count = 0;
	}

	if (t->cursor_position > t->length)
		t->cursor_position = t->length;

	if (REALIZED(t))
		ewl_text_display(t);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
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

	if (t->selection->len == 0)
	{
		DRETURN_PTR(NULL, DLEVEL_STABLE);
	}

	ret = malloc(sizeof(char) * (t->selection->len + 1));
	if (!ret)
	{
		DRETURN_PTR(NULL, DLEVEL_STABLE);
	}

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

	if (ewl_text_trigger_length_get(t->selection) > 0)
	{
		DRETURN_PTR(t->selection, DLEVEL_STABLE);
	}
	DRETURN_PTR(NULL, DLEVEL_STABLE);
}

/**
 * @param t: The Ewl_Text widget to set the position into
 * @param pos: The position to set
 * @return Returns no value.
 */
void
ewl_text_cursor_position_set(Ewl_Text *t, unsigned int pos)
{
	Ewl_Text_Context *tx;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("t", t);

	/* make sure we aren't more then the next char past the end of the
	 * text */
	if (pos > t->length) pos = t->length;
	t->cursor_position = pos;

	/* switch the current context to the context at the given position */
	tx = t->current_context;

	t->current_context = ewl_text_btree_context_get(t->formatting, pos);
	if (t->current_context)
	{
		tx->ref_count --;
		t->current_context->ref_count ++;
	}
	else
		t->current_context = tx;

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

	DRETURN_INT(t->cursor_position, DLEVEL_STABLE);
}

static void
ewl_text_op_set(Ewl_Text *t, unsigned int context_mask, Ewl_Text_Context *tx_change)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("t", t);

	/* do we have a current context? */
	if (t->current_context)
	{
		Ewl_Text_Context *ctx;

		ctx = t->current_context;
		t->current_context = ewl_text_context_find(t->current_context, context_mask, tx_change);
		ctx->ref_count --;
	}
	else
	{
		Ewl_Text_Context *ctx;

		ctx = ewl_text_context_default_create(t);
		t->current_context = ewl_text_context_find(ctx, context_mask, tx_change);
		ctx->ref_count --;
	}
	DLEAVE_FUNCTION(DLEVEL_STABLE);
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

	change = ewl_text_context_new();

	/* null font will go back to the theme default */
	if (!font) change->font = ewl_theme_data_str_get(EWL_WIDGET(t), "font");
	else change->font = strdup(font);

	ewl_text_op_set(t, EWL_TEXT_CONTEXT_MASK_FONT, change);
	ewl_text_context_free(change);

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

	/* if length is 0 we have nothing to do */
	if (length == 0) 
	{
		DRETURN(DLEVEL_STABLE);
	}

	tx = ewl_text_context_new();
	tx->font = strdup(font);
	ewl_text_btree_context_apply(t->formatting, tx, EWL_TEXT_CONTEXT_MASK_FONT, 
							t->cursor_position, length);
	ewl_text_context_free(tx);

	if (REALIZED(t))
		ewl_text_display(t);

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

	tx = ewl_text_btree_context_get(t->formatting, idx);
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

	change = ewl_text_context_new();
	change->size = size;

	ewl_text_op_set(t, EWL_TEXT_CONTEXT_MASK_SIZE, change);
	ewl_text_context_free(change);

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

	if (length == 0)
	{
		DRETURN(DLEVEL_STABLE);
	}

	tx = ewl_text_context_new();
	tx->size = size;
	ewl_text_btree_context_apply(t->formatting, tx, EWL_TEXT_CONTEXT_MASK_SIZE, 
							t->cursor_position, length);
	ewl_text_context_free(tx);

	if (REALIZED(t))
		ewl_text_display(t);

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

	tx = ewl_text_btree_context_get(t->formatting, idx);

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

	change = ewl_text_context_new();
	change->color.r = r;
	change->color.g = g;
	change->color.b = b;
	change->color.a = a;

	ewl_text_op_set(t, EWL_TEXT_CONTEXT_MASK_COLOR, change);
	ewl_text_context_free(change);

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

	ewl_text_btree_context_apply(t->formatting, tx, EWL_TEXT_CONTEXT_MASK_COLOR, 
							t->cursor_position, length);
	ewl_text_context_free(tx);

	if (REALIZED(t))
		ewl_text_display(t);

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

	tx = ewl_text_btree_context_get(t->formatting, idx);
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

	change = ewl_text_context_new();
	change->align = align;

	ewl_text_op_set(t, EWL_TEXT_CONTEXT_MASK_ALIGN, change);
	ewl_text_context_free(change);

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

	if (length == 0)
	{
		DRETURN(DLEVEL_STABLE);
	}

	tx = ewl_text_context_new();
	tx->align = align;
	ewl_text_btree_context_apply(t->formatting, tx, EWL_TEXT_CONTEXT_MASK_ALIGN, 
							t->cursor_position, length);
	ewl_text_context_free(tx);

	if (REALIZED(t))
		ewl_text_display(t);

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

	tx = ewl_text_btree_context_get(t->formatting, idx);

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

	change = ewl_text_context_new();
	change->styles = styles;

	ewl_text_op_set(t, EWL_TEXT_CONTEXT_MASK_STYLES, change);
	ewl_text_context_free(change);

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

	if (length == 0)
	{
		DRETURN(DLEVEL_STABLE);
	}

	tx = ewl_text_context_new();
	tx->styles = styles;
	ewl_text_btree_context_apply(t->formatting, tx, EWL_TEXT_CONTEXT_MASK_STYLES, 
							t->cursor_position, length);
	ewl_text_context_free(tx);

	if (REALIZED(t))
		ewl_text_display(t);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
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

	tx = ewl_text_btree_context_get(t->formatting, idx);

	DRETURN_INT(tx->styles, DLEVEL_STABLE);
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

	change = ewl_text_context_new();
	change->wrap = wrap;

	ewl_text_op_set(t, EWL_TEXT_CONTEXT_MASK_WRAP, change);
	ewl_text_context_free(change);

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

	if (length == 0)
	{
		DRETURN(DLEVEL_STABLE);
	}

	tx = ewl_text_context_new();
	tx->wrap = wrap;
	ewl_text_btree_context_apply(t->formatting, tx, EWL_TEXT_CONTEXT_MASK_WRAP, 
							t->cursor_position, length);
	ewl_text_context_free(tx);

	if (REALIZED(t))
		ewl_text_display(t);

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

	tx = ewl_text_btree_context_get(t->formatting, idx);

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

	change = ewl_text_context_new();
	change->style_colors.bg.r = r;
	change->style_colors.bg.g = g;
	change->style_colors.bg.b = b;
	change->style_colors.bg.a = a;

	ewl_text_op_set(t, EWL_TEXT_CONTEXT_MASK_BG_COLOR, change);
	ewl_text_context_free(change);

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

	ewl_text_btree_context_apply(t->formatting, tx, EWL_TEXT_CONTEXT_MASK_BG_COLOR, 
							t->cursor_position, length);
	ewl_text_context_free(tx);

	if (REALIZED(t))
		ewl_text_display(t);

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

	tx = ewl_text_btree_context_get(t->formatting, idx);
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

	change = ewl_text_context_new();
	change->style_colors.glow.r = r;
	change->style_colors.glow.g = g;
	change->style_colors.glow.b = b;
	change->style_colors.glow.a = a;

	ewl_text_op_set(t, EWL_TEXT_CONTEXT_MASK_GLOW_COLOR, change);
	ewl_text_context_free(change);

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

	ewl_text_btree_context_apply(t->formatting, tx, EWL_TEXT_CONTEXT_MASK_GLOW_COLOR, 
							t->cursor_position, length);
	ewl_text_context_free(tx);

	if (REALIZED(t))
		ewl_text_display(t);

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

	tx = ewl_text_btree_context_get(t->formatting, idx);
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

	change = ewl_text_context_new();
	change->style_colors.outline.r = r;
	change->style_colors.outline.g = g;
	change->style_colors.outline.b = b;
	change->style_colors.outline.a = a;

	ewl_text_op_set(t, EWL_TEXT_CONTEXT_MASK_OUTLINE_COLOR, change);
	ewl_text_context_free(change);

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

	ewl_text_btree_context_apply(t->formatting, tx, EWL_TEXT_CONTEXT_MASK_OUTLINE_COLOR, 
							t->cursor_position, length);
	ewl_text_context_free(tx);

	if (REALIZED(t))
		ewl_text_display(t);

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

	tx = ewl_text_btree_context_get(t->formatting, idx);
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

	change = ewl_text_context_new();
	change->style_colors.shadow.r = r;
	change->style_colors.shadow.g = g;
	change->style_colors.shadow.b = b;
	change->style_colors.shadow.a = a;

	ewl_text_op_set(t, EWL_TEXT_CONTEXT_MASK_SHADOW_COLOR, change);
	ewl_text_context_free(change);

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

	ewl_text_btree_context_apply(t->formatting, tx, EWL_TEXT_CONTEXT_MASK_SHADOW_COLOR, 
							t->cursor_position, length);
	ewl_text_context_free(tx);

	if (REALIZED(t))
		ewl_text_display(t);

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

	tx = ewl_text_btree_context_get(t->formatting, idx);
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

	change = ewl_text_context_new();
	change->style_colors.strikethrough.r = r;
	change->style_colors.strikethrough.g = g;
	change->style_colors.strikethrough.b = b;
	change->style_colors.strikethrough.a = a;

	ewl_text_op_set(t, EWL_TEXT_CONTEXT_MASK_STRIKETHROUGH_COLOR, change);
	ewl_text_context_free(change);

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

	ewl_text_btree_context_apply(t->formatting, tx, EWL_TEXT_CONTEXT_MASK_STRIKETHROUGH_COLOR, 
							t->cursor_position, length);
	ewl_text_context_free(tx);

	if (REALIZED(t))
		ewl_text_display(t);

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

	tx = ewl_text_btree_context_get(t->formatting, idx);
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

	change = ewl_text_context_new();
	change->style_colors.underline.r = r;
	change->style_colors.underline.g = g;
	change->style_colors.underline.b = b;
	change->style_colors.underline.a = a;

	ewl_text_op_set(t, EWL_TEXT_CONTEXT_MASK_UNDERLINE_COLOR, change);
	ewl_text_context_free(change);

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

	ewl_text_btree_context_apply(t->formatting, tx, EWL_TEXT_CONTEXT_MASK_UNDERLINE_COLOR, 
							t->cursor_position, length);
	ewl_text_context_free(tx);

	if (REALIZED(t))
		ewl_text_display(t);

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

	tx = ewl_text_btree_context_get(t->formatting, idx);
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

	change = ewl_text_context_new();
	change->style_colors.double_underline.r = r;
	change->style_colors.double_underline.g = g;
	change->style_colors.double_underline.b = b;
	change->style_colors.double_underline.a = a;

	ewl_text_op_set(t, EWL_TEXT_CONTEXT_MASK_DOUBLE_UNDERLINE_COLOR, change);
	ewl_text_context_free(change);

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

	ewl_text_btree_context_apply(t->formatting, tx, EWL_TEXT_CONTEXT_MASK_DOUBLE_UNDERLINE_COLOR, 
							t->cursor_position, length);
	ewl_text_context_free(tx);

	if (REALIZED(t))
		ewl_text_display(t);

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

	tx = ewl_text_btree_context_get(t->formatting, idx);
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
ewl_text_triggers_remove(Ewl_Text *t)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("t", t);

	if (!t->triggers) 
		return;

	ecore_list_for_each(t->triggers, ewl_text_trigger_cb_free, NULL);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}


/* if we move the text (insertion, deleteion, etc) we need to shift the
 * position of the current cursors so they appear in the correct positions */
static void
ewl_text_triggers_shift(Ewl_Text *t, unsigned int pos, unsigned int len)
{
	Ewl_Text_Trigger *cur;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("t", t);

	ecore_list_goto_first(t->triggers);
	while ((cur = ecore_list_next(t->triggers)))
	{
		/* its before us */
		if ((pos + len) < cur->pos)
		{
			cur->pos += len;
			continue;
		}
		
		/* inserted into trigger */
		if ((cur->pos <= pos) && ((cur->pos + cur->len) > pos))
		{
			cur->len += len;
			continue;
		}
	}

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
	{
		DRETURN_PTR(NULL, DLEVEL_STABLE);
	}

	if (!ewl_text_trigger_init(trigger, type))
	{
		FREE(trigger);
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
		type_str = strdup("trigger");
	else if (type == EWL_TEXT_TRIGGER_TYPE_SELECTION)
		type_str = strdup("selection");
	else
	{
		DRETURN_INT(FALSE, DLEVEL_STABLE);
	}

	if (!ewl_widget_init(EWL_WIDGET(trigger), type_str))
	{
		DRETURN_INT(FALSE, DLEVEL_STABLE);
	}
	ewl_widget_inherit(EWL_WIDGET(trigger), type_str);

	trigger->areas = ecore_list_new();
	trigger->type = type;

	FREE(type_str);

	DRETURN_INT(TRUE, DLEVEL_STABLE);
}

void
ewl_text_trigger_free(Ewl_Text_Trigger *t)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("t", t);

	if (t->areas)
		ecore_list_destroy(t->areas);

	t->text_parent = NULL;
	t->areas = NULL;
	FREE(t);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static void
ewl_text_trigger_cb_free(void *value, void *data)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("data", data);

	ewl_text_trigger_free(data);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

Ewl_Text_Trigger_Type 
ewl_text_trigger_type_get(Ewl_Text_Trigger *t)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("t", t, EWL_TEXT_TRIGGER_TYPE_NONE);

	DRETURN_INT(t->type, DLEVEL_STABLE);
}

void 
ewl_text_trigger_start_pos_set(Ewl_Text_Trigger *t, unsigned int pos)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("t", t);

	t->pos = pos;

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

unsigned int
ewl_text_trigger_start_pos_get(Ewl_Text_Trigger *t)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("t", t, 0);

	DRETURN_INT(t->pos, DLEVEL_STABLE);
}

void
ewl_text_trigger_length_set(Ewl_Text_Trigger *t, unsigned int len)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("t", t);

	t->len = len;

	/* if the length is set to 0 remove the areas */
	if (len == 0)
	{
		if (t->areas)
		{
			Ewl_Text_Trigger_Area *area;

			ecore_list_goto_first(t->areas);
			while ((area = ecore_list_next(t->areas)))
				ewl_widget_hide(EWL_WIDGET(area));

			ecore_list_clear(t->areas);
		}
	}

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

unsigned int
ewl_text_trigger_length_get(Ewl_Text_Trigger *t)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("t", t, 0);

	DRETURN_INT(t->len, DLEVEL_STABLE);
}

void
ewl_text_trigger_base_set(Ewl_Text_Trigger *t, unsigned int pos)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("t", t);

	t->base = pos;

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

unsigned int
ewl_text_trigger_base_get(Ewl_Text_Trigger *t)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("t", t, 0);

	DRETURN_INT(t->base, DLEVEL_STABLE);
}

/*
 * Internal stuff 
 */
void
ewl_text_triggers_configure(Ewl_Text *t)
{
	Ewl_Text_Trigger_Area *area;
	Ewl_Text_Trigger *cur;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("t", t);

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

	area = ewl_text_trigger_area_new(cur->type);
	ewl_container_child_append(EWL_CONTAINER(t), area);
	ewl_widget_internal_set(area, TRUE);
	ewl_object_geometry_request(EWL_OBJECT(area), x, y, w, h);

	ewl_callback_append(area, EWL_CALLBACK_FOCUS_IN, 
			ewl_text_trigger_cb_focus_in, cur);
	ewl_callback_append(area, EWL_CALLBACK_FOCUS_OUT,
			ewl_text_trigger_cb_focus_out, cur);
	ewl_callback_append(area, EWL_CALLBACK_MOUSE_DOWN,
			ewl_text_trigger_cb_mouse_down, cur);
	ewl_callback_append(area, EWL_CALLBACK_MOUSE_UP,
			ewl_text_trigger_cb_mouse_up, cur);
	ewl_widget_show(area);

	ecore_list_append(cur->areas, area);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static void
ewl_text_trigger_position(Ewl_Text *t, Ewl_Text_Trigger *trig, int tb_length,
				unsigned int *cur_idx, unsigned int *cur_tb_idx)
{
	Evas_Coord sx, sy, sh, ex, ey, ew, eh;
	char *ptr;
	int start_line = 0, end_line = 0;
	int fiddled;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("t", t);
	DCHECK_PARAM_PTR("trig", trig);

	/* clean out the old areas */
	/* XXX this needs to be smartened such that it will re-use the 
	 * previously created areas instead of deleting them every time */
	{
		Ewl_Text_Trigger_Area *area;

		if (ecore_list_nodes(trig->areas) > 0)
		{
			ecore_list_goto_first(trig->areas);
			while ((area = ecore_list_next(trig->areas)))
			{
				/* i'm deleting in the iteration _after_ the one in which it was actually
				 * deleted cuz i'm getting fucked up events on the areas taht have already
				 * been deleted....which is bad...
				 * I think it's because I'm deleting in the configure callback?
				 */
				if (area->deleted)
					ewl_widget_destroy(EWL_WIDGET(area));
				else
				{
					ewl_widget_hide(EWL_WIDGET(area));
					area->deleted = TRUE;
				}
			}
			ecore_list_clear(trig->areas);
		}
		else
			trig->areas = ecore_list_new();
	}

	if (trig->len == 0) return;

	fiddled = 0;

	/* get the index of the start of the trigger */
	for (ptr = (t->text + *cur_idx); *cur_idx < trig->pos; (*cur_idx)++)
	{
		/* if this isn't a \r\n or \t increment the
		 * textblock position */
		if ((*ptr != '\r') && (*ptr != '\n') && (*ptr != '\t')) 
			(*cur_tb_idx) ++;
		ptr ++;
	}

	/* get the position of the start character */
	evas_object_textblock_char_pos_get(t->textblock, *cur_tb_idx,
			&sx, &sy, NULL, &sh);

	/* get the line number we're on */
	evas_object_textblock_cursor_pos_set(t->textblock, *cur_tb_idx);
	start_line = evas_object_textblock_cursor_line_get(t->textblock);


	/* get the index of the end of the trigger */
	for (ptr = (t->text + *cur_idx);
			*cur_idx < (trig->pos + trig->len); (*cur_idx)++)
	{
		/* if this isn't a \r\n or \t increment the
		 * textblock position */
		if ((*ptr != '\r') && (*ptr != '\n') && (*ptr != '\t')) 
			(*cur_tb_idx) ++;
		ptr ++;
	}

	if ((*cur_tb_idx) >= tb_length)
	{
		*cur_tb_idx = tb_length - 1;
		fiddled = 1;
	}
	evas_object_textblock_char_pos_get(t->textblock, *cur_tb_idx,
			&ex, &ey, &ew, &eh);

	/* get the line number we're on */
	evas_object_textblock_cursor_pos_set(t->textblock, *cur_tb_idx);
	end_line = evas_object_textblock_cursor_line_get(t->textblock);

	if (fiddled)
		ex += ew;

	/* whole trigger is on one line */
	if (start_line == end_line)
	{
		ewl_text_trigger_area_add(t, trig, sx, sy, (ex - sx), 
							(ey - sy) + eh);
	}
	/* multiline trigger */
	else
	{
		int i, missed = 0;
		Evas_Coord lx, ly, lw, lh;
		Evas_Coord llx = 0, lly = 0, llw = 0, llh = 0;

		/* get the coords for all the lines and deal with
		 * them */
		for (i = start_line; i <= end_line; i++)
		{
			if (!evas_object_textblock_line_get(t->textblock, i, 
							&lx, &ly, &lw, &lh))
			{
				/* if we don't get a response from
				 * evas then this is a blank line.
				 * wait until we get the next real line
				 * to deal with it */
				missed = 1;

				lly = ly + lh;
				llw = lw;
				llh = lh;

				continue;
			}
			else
			{
				if (i == start_line)
				{
					lx = sx;
					ly = sy;
					lw -= ex;
				}
				else if (i == end_line)
				{
					lw = ex - ew;
				}
					
				ewl_text_trigger_area_add(t, trig, lx, ly, lw, lh);
			}

			/* deal with the missed line */
			if (missed)
			{
				ewl_text_trigger_area_add(t, trig, 0, lly, 2, ly - lly);

				missed = 0;
				llx = lly = llw = llh = 0;
			}
		}
	}

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void
ewl_text_triggers_realize(Ewl_Text *t)
{
	Ewl_Text_Trigger *cur;
	int tb_length = 0;
	unsigned int cur_idx = 0, cur_tb_idx = 0;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("t", t);

	/* we need to get the length of the textblock to make sure we aren't
	 * trying to access one past its end. if we are, we need to do the
	 * previous one then add the previous's width to get the correct x
	 * position */
	tb_length = evas_object_textblock_length_get(t->textblock);

	if (t->triggers)
	{
		ecore_list_goto_first(t->triggers);
		while ((cur = ecore_list_next(t->triggers)))
			ewl_text_trigger_position(t, cur, tb_length, &cur_idx, &cur_tb_idx);
	}

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void
ewl_text_triggers_unrealize(Ewl_Text *t)
{
	Ewl_Text_Trigger *cur;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("t", t);

	if (t->triggers)
	{
		ecore_list_goto_first(t->triggers);
		while ((cur = ecore_list_next(t->triggers)))
			ecore_list_clear(cur->areas);
	}

	if (t->selection)
		ecore_list_clear(t->selection->areas);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void
ewl_text_triggers_show(Ewl_Text *t)
{
	Ewl_Widget *area;
	Ewl_Text_Trigger *cur;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("t", t);

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

	/* hide the triggers */
	if (t->triggers)
	{
		ecore_list_goto_first(t->triggers);
		while ((cur = ecore_list_next(t->triggers)))
		{

			if (!cur->areas)
				continue;

			ecore_list_goto_first(cur->areas);
			while ((area = ecore_list_next(cur->areas)))
				ewl_widget_hide(area);
		}
	}

	/* hide the selection */
	if (t->selection->areas) 
	{
		ecore_list_goto_first(t->selection->areas);
		while ((area = ecore_list_next(t->selection->areas)))
			ewl_widget_hide(area);
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

	if (!(appearance = ewl_widget_appearance_get(w))) 
	{
		DRETURN(DLEVEL_STABLE);
	}

	/* if this is a trigger then add it as such */
	if ((!strcmp(appearance, "trigger"))
			|| (!strcmp(appearance, "selection")))
		ewl_text_trigger_add(EWL_TEXT(c), EWL_TEXT_TRIGGER(w));

	FREE(appearance);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void
ewl_text_cb_child_del(Ewl_Container *c, Ewl_Widget *w)
{
	char *appearance;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("c", c);
	DCHECK_PARAM_PTR("w", w);

	if (!(appearance = ewl_widget_appearance_get(w)))
	{
		DRETURN(DLEVEL_STABLE);
	}

	/* if this is a trigger, remove it as such */
	if ((!strcmp(appearance, "trigger"))
			|| (!strcmp(appearance, "selection")))
		ewl_text_trigger_del(EWL_TEXT(c), EWL_TEXT_TRIGGER(w));

	FREE(appearance);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static void
ewl_text_trigger_add(Ewl_Text *t, Ewl_Text_Trigger *trigger)
{
	Ewl_Text_Trigger *cur = NULL;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("t", t);
	DCHECK_PARAM_PTR("trigger", trigger);

	/* if we have no length, we start past the end of the text, or we
	 * extend past the end of the text then return an error */
	if ((trigger->len == 0) || (trigger->pos > t->length)
			|| ((trigger->pos + trigger->len) > t->length))
	{
		DRETURN(DLEVEL_STABLE);
	}

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

	ecore_list_goto(t->triggers, trigger);
	ecore_list_remove(t->triggers);

	trigger->text_parent = NULL;

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void
ewl_text_cb_configure(Ewl_Widget *w, void *ev, void *data)
{
	Ewl_Text *t;
	int xx, yy, ww, hh;

	DENTER_FUNCTION(DLEVEL_STABLE);

	t = EWL_TEXT(w);

	xx = CURRENT_X(w);
	yy = CURRENT_Y(w);
	hh = CURRENT_H(w);
	ww = CURRENT_W(w);

	if (t->textblock)
	{
		evas_object_move(t->textblock, xx, yy);
		evas_object_resize(t->textblock, ww, hh);
		evas_object_layer_set(t->textblock, 
					ewl_widget_layer_sum_get(w));

		/* This needs to be here to cause the scrollpane to update
		 * as you scroll, tho I think Evas should be doing this
		 * itself behind the scenes */
		evas_damage_rectangle_add(evas_object_evas_get(t->textblock),
							xx, yy, ww, hh);

		ewl_text_triggers_configure(t);
	}

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void
ewl_text_cb_realize(Ewl_Widget *w, void *ev, void *data)
{
	Ewl_Text *t;
	Ewl_Embed *emb;

	DENTER_FUNCTION(DLEVEL_STABLE);

	t = EWL_TEXT(w);

	/* find the embed so we know the evas */
	emb = ewl_embed_widget_find(w);
	if (!emb)
	{
		DRETURN(DLEVEL_STABLE);
	}

	/* create the textblock */
	t->textblock = evas_object_textblock_add(emb->evas);

	if (w->fx_clip_box)
		evas_object_clip_set(t->textblock, w->fx_clip_box);

	evas_object_pass_events_set(t->textblock, 1);

	ewl_text_display(t);
	ewl_text_triggers_realize(t);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void
ewl_text_cb_unrealize(Ewl_Widget *w, void *ev, void *data)
{
	Ewl_Text *t;

	DENTER_FUNCTION(DLEVEL_STABLE);

	t = EWL_TEXT(w);

	evas_object_clip_unset(t->textblock);
	evas_object_del(t->textblock);
	t->textblock = NULL;

	ewl_text_triggers_unrealize(t);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void
ewl_text_cb_show(Ewl_Widget *w, void *ev, void *data)
{
	Ewl_Text *t;

	DENTER_FUNCTION(DLEVEL_STABLE);

	t = EWL_TEXT(w);
	evas_object_show(t->textblock);

	ewl_text_triggers_show(t);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void
ewl_text_cb_hide(Ewl_Widget *w, void *ev, void *data)
{
	Ewl_Text *t;

	DENTER_FUNCTION(DLEVEL_STABLE);

	t = EWL_TEXT(w);
	evas_object_hide(t->textblock);

	ewl_text_triggers_hide(t);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void
ewl_text_cb_destroy(Ewl_Widget *w, void *ev, void *data)
{
	Ewl_Text *t;

	DENTER_FUNCTION(DLEVEL_STABLE);

	t = EWL_TEXT(w);

	if (t->triggers)
	{
		ecore_list_destroy(t->triggers);
		t->triggers = NULL;
	}
	t->selection = NULL;

	ewl_text_btree_free(t->formatting);
	t->formatting = NULL;

	if (t->current_context)
	{
		t->current_context->ref_count --;
		if (t->current_context->ref_count == 0)
			ewl_text_context_free(t->current_context);

		t->current_context = NULL;
	}

	IF_FREE(t->text);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void
ewl_text_cb_mouse_down(Ewl_Widget *w, void *ev, void *data)
{
        Ewl_Text *t;
        Ewl_Event_Mouse_Down *event;
        unsigned int idx = 0;
        unsigned int modifiers;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR("w", w);

        event = ev;
        t = EWL_TEXT(w);

        ewl_callback_append(w, EWL_CALLBACK_MOUSE_MOVE,
                                ewl_text_cb_mouse_move, NULL);
        
        idx = ewl_text_coord_index_map(EWL_TEXT(w), event->x, event->y);
       
        modifiers = ewl_ev_modifiers_get();
        if (modifiers & EWL_KEY_MODIFIER_SHIFT)
                ewl_text_selection_select_to(t->selection, idx);
        else
        {
                ewl_text_trigger_start_pos_set(t->selection, idx);
                ewl_text_trigger_base_set(t->selection, idx);
                ewl_text_trigger_length_set(t->selection, 0);
        }               
        t->in_select = TRUE;

	ewl_text_selection_cb_configure(EWL_WIDGET(t->selection), NULL, NULL);
                
        DLEAVE_FUNCTION(DLEVEL_STABLE);
}       

void
ewl_text_cb_mouse_up(Ewl_Widget *w, void *ev, void *data)
{
        Ewl_Text *t;
        Ewl_Event_Mouse_Up *event;
        unsigned int modifiers;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR("w", w);

        event = ev;
        t = EWL_TEXT(w);

        modifiers = ewl_ev_modifiers_get();
        if (modifiers & EWL_KEY_MODIFIER_SHIFT)
        {
                unsigned int idx = 0;
                idx = ewl_text_coord_index_map(EWL_TEXT(w), event->x, event->y);
                ewl_text_selection_select_to(t->selection, idx);
        }

        ewl_callback_del(w, EWL_CALLBACK_MOUSE_MOVE, ewl_text_cb_mouse_move);
        t->in_select = FALSE;

	ewl_text_selection_cb_configure(EWL_WIDGET(t->selection), NULL, NULL);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void
ewl_text_cb_mouse_move(Ewl_Widget *w, void *ev, void *data)
{
        Ewl_Text *t;
        Ewl_Event_Mouse_Move *event;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR("w", w);

        event = ev;
        t = EWL_TEXT(w);

        if (t->in_select)
        {
                unsigned int idx = 0;

		idx = ewl_text_coord_index_map(EWL_TEXT(w), event->x, event->y);
                ewl_text_selection_select_to(t->selection, idx);
		ewl_text_selection_cb_configure(EWL_WIDGET(t->selection), NULL, NULL);
        }

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void
ewl_text_trigger_cb_focus_in(Ewl_Widget *w, void *ev, void *data)
{
	Ewl_Text_Trigger *trigger;

	DENTER_FUNCTION(DLEVEL_STABLE);

	trigger = data;
	ewl_callback_call_with_event_data(EWL_WIDGET(trigger), 
					EWL_CALLBACK_FOCUS_IN, ev);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void
ewl_text_trigger_cb_focus_out(Ewl_Widget *w, void *ev, void *data)
{
	Ewl_Text_Trigger *trigger;

	DENTER_FUNCTION(DLEVEL_STABLE);

	trigger = data;
	ewl_callback_call_with_event_data(EWL_WIDGET(trigger), 
					EWL_CALLBACK_FOCUS_OUT, ev);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void
ewl_text_trigger_cb_mouse_up(Ewl_Widget *w, void *ev, void *data)
{
	Ewl_Text_Trigger *trigger;

	DENTER_FUNCTION(DLEVEL_STABLE);

	trigger = data;
	ewl_callback_call_with_event_data(EWL_WIDGET(trigger),
					EWL_CALLBACK_MOUSE_UP, ev);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void
ewl_text_trigger_cb_mouse_down(Ewl_Widget *w, void *ev, void *data)
{
	Ewl_Text_Trigger *trigger;

	DENTER_FUNCTION(DLEVEL_STABLE);

	trigger = data;
	ewl_callback_call_with_event_data(EWL_WIDGET(trigger), 
					EWL_CALLBACK_MOUSE_DOWN, ev);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static Ewl_Text_Context *
ewl_text_context_default_create(Ewl_Text *t)
{
	Ewl_Text_Context *tx = NULL, *tmp;
    int i;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("t", t, NULL);

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
	ewl_text_context_free(tmp);

	DRETURN_PTR(tx, DLEVEL_STABLE);
}

/*
 * Ewl_Text_Context Stuff
 */
void
ewl_text_context_init(void)
{
	if (!context_hash) 
	{
		context_hash = ecore_hash_new(ecore_str_hash, ecore_str_compare);
		ecore_hash_set_free_key(context_hash, free);
		ecore_hash_set_free_value(context_hash, ewl_text_context_cb_free);
	}
}

void
ewl_text_context_shutdown(void)
{
	if (context_hash)
		ecore_hash_destroy(context_hash);
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
	tx->ref_count = 0;

	DRETURN_PTR(tx, DLEVEL_STABLE);
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
	{
		DCHECK_PARAM_PTR_RET("tx_change", tx_change, NULL);
	}

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
	if (new_tx) new_tx->ref_count ++;
	FREE(t);

	DRETURN_PTR(new_tx, DLEVEL_STABLE);
}

/*
 * if they contain the same data the they should be the same pointer... 
 */
static int
ewl_text_context_compare(Ewl_Text_Context *a, Ewl_Text_Context *b)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("a", a, 0);
	DCHECK_PARAM_PTR_RET("b", b, 0);

	DRETURN_INT((a == b), DLEVEL_STABLE);
}

static void
ewl_text_context_print(Ewl_Text_Context *tx, char *indent)
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

Ewl_Text_Context *
ewl_text_context_new(void)
{
	Ewl_Text_Context *tx;

	DENTER_FUNCTION(DLEVEL_STABLE);

	tx = NEW(Ewl_Text_Context, 1);

	DRETURN_PTR(tx, DLEVEL_STABLE);;
}

void
ewl_text_context_free(Ewl_Text_Context *tx)
{
	char *t;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("tx", tx);

	if (tx->ref_count > 0) return;

	t = ewl_text_context_name_get(tx, 0, NULL);
	ecore_hash_remove(context_hash, t);

	IF_FREE(tx->font);
	FREE(tx);
	FREE(t);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static void
ewl_text_context_cb_free(void *data)
{
	Ewl_Text_Context *tx;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("data", data);

	tx = data;
	ewl_text_context_free(tx);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void
ewl_text_context_font_set(Ewl_Text_Context *tx, const char *font)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("tx", tx);
	DCHECK_PARAM_PTR("font", font);

	IF_FREE(tx->font);
	tx->font = strdup(font);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

char *
ewl_text_context_font_get(Ewl_Text_Context *tx)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("tx", tx, NULL);

	DRETURN_PTR(tx->font, DLEVEL_STABLE);
}

void
ewl_text_context_font_size_set(Ewl_Text_Context *tx, unsigned int size)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("tx", tx);

	tx->size = size;

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

unsigned int
ewl_text_context_font_size_get(Ewl_Text_Context *tx)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("tx", tx, 0);

	DRETURN_INT(tx->size, DLEVEL_STABLE);
}

void
ewl_text_context_styles_set(Ewl_Text_Context *tx, unsigned int styles)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("tx", tx);

	tx->styles = styles;

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

unsigned int
ewl_text_context_styles_get(Ewl_Text_Context *tx)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("tx", tx, 0);

	DRETURN_INT(tx->styles, DLEVEL_STABLE);
}

void
ewl_text_context_align_set(Ewl_Text_Context *tx, unsigned int align)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("tx", tx);

	tx->align = align;

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

unsigned int
ewl_text_context_align_get(Ewl_Text_Context *tx)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("tx", tx, 0);

	DRETURN_INT(tx->align, DLEVEL_STABLE);
}

void
ewl_text_context_wrap_set(Ewl_Text_Context *tx, unsigned int wrap)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("tx", tx);

	tx->wrap = wrap;

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

unsigned int
ewl_text_context_wrap_get(Ewl_Text_Context *tx)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("tx", tx, 0);

	DRETURN_INT(tx->wrap, DLEVEL_STABLE);
}

void
ewl_text_context_color_set(Ewl_Text_Context *tx, unsigned int r,
			unsigned int g, unsigned int b, unsigned int a)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("tx", tx);

	tx->color.r = ((r < 255) ? r : 255);
	tx->color.g = ((g < 255) ? g : 255);
	tx->color.b = ((b < 255) ? b : 255);
	tx->color.a = ((a < 255) ? a : 255);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void
ewl_text_context_color_get(Ewl_Text_Context *tx, unsigned int *r,
			unsigned int *g, unsigned int *b, unsigned int *a)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("tx", tx);

	if (r) *r = tx->color.r;
	if (g) *g = tx->color.g;
	if (b) *b = tx->color.b;
	if (a) *a = tx->color.a;

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void
ewl_text_context_bg_color_set(Ewl_Text_Context *tx, unsigned int r,
			unsigned int g, unsigned int b, unsigned int a)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("tx", tx);

	tx->style_colors.bg.r = ((r < 255) ? r : 255);
	tx->style_colors.bg.g = ((g < 255) ? g : 255);
	tx->style_colors.bg.b = ((b < 255) ? b : 255);
	tx->style_colors.bg.a = ((a < 255) ? a : 255);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void
ewl_text_context_bg_color_get(Ewl_Text_Context *tx, unsigned int *r,
			unsigned int *g, unsigned int *b, unsigned int *a)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("tx", tx);

	if (r) *r = tx->style_colors.bg.r;
	if (g) *g = tx->style_colors.bg.g;
	if (b) *b = tx->style_colors.bg.b;
	if (a) *a = tx->style_colors.bg.a;

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void
ewl_text_context_glow_color_set(Ewl_Text_Context *tx, unsigned int r,
			unsigned int g, unsigned int b, unsigned int a)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("tx", tx);

	tx->style_colors.glow.r = ((r < 255) ? r : 255);
	tx->style_colors.glow.g = ((g < 255) ? g : 255);
	tx->style_colors.glow.b = ((b < 255) ? b : 255);
	tx->style_colors.glow.a = ((a < 255) ? a : 255);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void
ewl_text_context_glow_color_get(Ewl_Text_Context *tx, unsigned int *r,
			unsigned int *g, unsigned int *b, unsigned int *a)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("tx", tx);

	if (r) *r = tx->style_colors.glow.r;
	if (g) *g = tx->style_colors.glow.g;
	if (b) *b = tx->style_colors.glow.b;
	if (a) *a = tx->style_colors.glow.a;

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void
ewl_text_context_outline_color_set(Ewl_Text_Context *tx, unsigned int r,
			unsigned int g, unsigned int b, unsigned int a)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("tx", tx);

	tx->style_colors.outline.r = ((r < 255) ? r : 255);
	tx->style_colors.outline.g = ((g < 255) ? g : 255);
	tx->style_colors.outline.b = ((b < 255) ? b : 255);
	tx->style_colors.outline.a = ((a < 255) ? a : 255);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void
ewl_text_context_outline_color_get(Ewl_Text_Context *tx, unsigned int *r,
			unsigned int *g, unsigned int *b, unsigned int *a)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("tx", tx);

	if (r) *r = tx->style_colors.outline.r;
	if (g) *g = tx->style_colors.outline.g;
	if (b) *b = tx->style_colors.outline.b;
	if (a) *a = tx->style_colors.outline.a;

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void
ewl_text_context_shadow_color_set(Ewl_Text_Context *tx, unsigned int r,
			unsigned int g, unsigned int b, unsigned int a)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("tx", tx);

	tx->style_colors.shadow.r = ((r < 255) ? r : 255);
	tx->style_colors.shadow.g = ((g < 255) ? g : 255);
	tx->style_colors.shadow.b = ((b < 255) ? b : 255);
	tx->style_colors.shadow.a = ((a < 255) ? a : 255);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void
ewl_text_context_shadow_color_get(Ewl_Text_Context *tx, unsigned int *r,
			unsigned int *g, unsigned int *b, unsigned int *a)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("tx", tx);

	if (r) *r = tx->style_colors.shadow.r;
	if (g) *g = tx->style_colors.shadow.g;
	if (b) *b = tx->style_colors.shadow.b;
	if (a) *a = tx->style_colors.shadow.a;

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void
ewl_text_context_strikethrough_color_set(Ewl_Text_Context *tx, unsigned int r,
			unsigned int g, unsigned int b, unsigned int a)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("tx", tx);

	tx->style_colors.strikethrough.r = ((r < 255) ? r : 255);
	tx->style_colors.strikethrough.g = ((g < 255) ? g : 255);
	tx->style_colors.strikethrough.b = ((b < 255) ? b : 255);
	tx->style_colors.strikethrough.a = ((a < 255) ? a : 255);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void
ewl_text_context_strikethrough_color_get(Ewl_Text_Context *tx, unsigned int *r,
			unsigned int *g, unsigned int *b, unsigned int *a)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("tx", tx);

	if (r) *r = tx->style_colors.strikethrough.r;
	if (g) *g = tx->style_colors.strikethrough.g;
	if (b) *b = tx->style_colors.strikethrough.b;
	if (a) *a = tx->style_colors.strikethrough.a;

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void
ewl_text_context_underline_color_set(Ewl_Text_Context *tx, unsigned int r,
			unsigned int g, unsigned int b, unsigned int a)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("tx", tx);

	tx->style_colors.underline.r = ((r < 255) ? r : 255);
	tx->style_colors.underline.g = ((g < 255) ? g : 255);
	tx->style_colors.underline.b = ((b < 255) ? b : 255);
	tx->style_colors.underline.a = ((a < 255) ? a : 255);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void
ewl_text_context_underline_color_get(Ewl_Text_Context *tx, unsigned int *r,
			unsigned int *g, unsigned int *b, unsigned int *a)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("tx", tx);

	if (r) *r = tx->style_colors.underline.r;
	if (g) *g = tx->style_colors.underline.g;
	if (b) *b = tx->style_colors.underline.b;
	if (a) *a = tx->style_colors.underline.a;

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void
ewl_text_context_double_underline_color_set(Ewl_Text_Context *tx, unsigned int r,
			unsigned int g, unsigned int b, unsigned int a)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("tx", tx);

	tx->style_colors.double_underline.r = ((r < 255) ? r : 255);
	tx->style_colors.double_underline.g = ((g < 255) ? g : 255);
	tx->style_colors.double_underline.b = ((b < 255) ? b : 255);
	tx->style_colors.double_underline.a = ((a < 255) ? a : 255);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void
ewl_text_context_double_underline_color_get(Ewl_Text_Context *tx, unsigned int *r,
			unsigned int *g, unsigned int *b, unsigned int *a)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("tx", tx);

	if (r) *r = tx->style_colors.double_underline.r;
	if (g) *g = tx->style_colors.double_underline.g;
	if (b) *b = tx->style_colors.double_underline.b;
	if (a) *a = tx->style_colors.double_underline.a;

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/*
 * Ewl_Text_BTree stuff
 */
Ewl_Text_BTree *
ewl_text_btree_new(void)
{
	Ewl_Text_BTree *tree;

	DENTER_FUNCTION(DLEVEL_STABLE);

	tree = NEW(Ewl_Text_BTree, 1);
	if (!tree)
	{
		DRETURN_PTR(NULL, DLEVEL_STABLE);
	}

	DRETURN_PTR(tree, DLEVEL_STABLE);
}

void
ewl_text_btree_free(Ewl_Text_BTree *tree)
{
	DENTER_FUNCTION(DLEVEL_STABLE);

	if (!tree) return;

	tree->parent = NULL;
	if (tree->children) 
	{
		Ewl_Text_BTree *child;

		ecore_list_goto_first(tree->children);
		while ((child = ecore_list_next(tree->children)))
			ewl_text_btree_free(child);

		ecore_list_destroy(tree->children);
		tree->children = NULL;
	}

	if (tree->tx)
	{
		tree->tx->ref_count --;
		if (tree->tx->ref_count == 0)
			ewl_text_context_free(tree->tx);

		tree->tx = NULL;
	}
	FREE(tree);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

Ewl_Text_Context *
ewl_text_btree_context_get(Ewl_Text_BTree *tree, unsigned int idx)
{
	Ewl_Text_Context *tx = NULL;
	Ewl_Text_BTree *child;
	int count = 0;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("tree", tree, NULL);

	/* can't find a node past the length of our text */
	if (idx > tree->length) 
	{
		DRETURN_PTR(NULL, DLEVEL_STABLE);
	}

	/* if we have a leaf node, return it */
	if (tree->tx)
	{
		DRETURN_PTR(tree->tx, DLEVEL_STABLE);
	}

	/* something bad...no context...no children...wtf? */
	if (!tree->children)
	{
		DWARNING("No children, no context.... wtf?");
		DRETURN_PTR(NULL, DLEVEL_STABLE);
	}

	/* else we need to go through the children */
	ecore_list_goto_first(tree->children);
	while ((child = ecore_list_next(tree->children)))
	{
		/* if we end at a point that is bigger then the index then
		 * this is the node we want. We know the index is in this
		 * node or it would have matched the one before us */
		if ((count + child->length) > idx)
		{
			tx = ewl_text_btree_context_get(child, idx);
			DRETURN_PTR(tx, DLEVEL_STABLE);
		}
		count += child->length;
	}
	
	DWARNING("Got to the end of function... is that possible?");
	DRETURN_PTR(tx, DLEVEL_STABLE);
}

void
ewl_text_btree_text_context_insert(Ewl_Text_BTree *tree, Ewl_Text_Context *tx,
					unsigned int idx, unsigned int len)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("tree", tree);
	DCHECK_PARAM_PTR("tx", tx);

	/* no children no context ... ? root? */
	if ((!tree->children) && (!tree->tx))
	{
		tree->tx = tx;
		tree->length = len;
		tx->ref_count ++;

		DRETURN(DLEVEL_STABLE);
	}

	/* no children but we have a tx, we need to split this node */
	if (!tree->children)
	{
		Ewl_Text_BTree *old, *new;

		/* create a node for the text */
		new = ewl_text_btree_new();
		new->tx = tx;
		new->tx->ref_count ++;
		new->length = len;
		new->parent = tree;

		/* see if the contexts are the same */
		if (ewl_text_context_compare(tree->tx, tx))
		{
			tree->length += len;
			DRETURN(DLEVEL_STABLE);
		}

		/* this is the old node */
		old = ewl_text_btree_new();
		old->tx = tree->tx;
		old->length = tree->length;
		old->parent = tree;

		tree->length += len;
		tree->tx = NULL;
		tree->children = ecore_list_new();

		if (idx == 0)
		{
			ecore_list_append(tree->children, new);
			ecore_list_append(tree->children, old);
		}
		else if (idx >= old->length)
		{
			ecore_list_append(tree->children, old);
			ecore_list_append(tree->children, new);
		}
		else
		{
			Ewl_Text_BTree *n;

			/* both of these nodes have the tx ++'d because when
			 * we free the old it will dec the tx */

			/* grap left part */
			n = ewl_text_btree_new();
			n->tx = old->tx;
			n->tx->ref_count ++;
			n->length = idx;
			n->parent = tree;
			ecore_list_append(tree->children, n);

			ecore_list_append(tree->children, new);

			/* grap right part */
			n = ewl_text_btree_new();
			n->tx = old->tx;
			n->tx->ref_count ++;
			n->length = old->length - (idx);
			n->parent = tree;
			ecore_list_append(tree->children, n);

			ewl_text_btree_free(old);
		}
	}
	else /* has to be in the tree somewhere */
	{
		Ewl_Text_BTree *child;
		int sum = 0;

		ecore_list_goto_first(tree->children);
		while ((child = ecore_list_next(tree->children)))
		{
			/* goes in this child */
			if ((sum <= idx) && ((child->length + sum) >= idx))
			{
				tree->length += len;
				ewl_text_btree_text_context_insert(child, 
							tx, idx - sum, len);
				break;
			}
			sum += child->length;
		}
	}

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void
ewl_text_btree_context_apply(Ewl_Text_BTree *tree, Ewl_Text_Context *tx,
				unsigned int context_mask, unsigned int idx, 
				unsigned int len)
{
	Ewl_Text_BTree *child;
	int sum = 0;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("tree", tree);
	DCHECK_PARAM_PTR("tx", tx);

	if (context_mask & EWL_TEXT_CONTEXT_MASK_NONE)
	{
		DRETURN(DLEVEL_STABLE);
	}

	/* if we are a leaf node */
	if (tree->tx)
	{
		Ewl_Text_Context *new_tx, *ctx;

		ctx = tree->tx;
		new_tx = ewl_text_context_find(ctx, context_mask, tx);

		/* apply covers entire node */
		if ((idx == 0) && ((idx + len) >= tree->length))
		{
			ctx->ref_count --;
			if (ctx->ref_count == 0)
				ewl_text_context_free(ctx);
			tree->tx = new_tx;
		}
		else
		{
			Ewl_Text_BTree *old, *new;

			new = ewl_text_btree_new();
			new->parent = tree;
			new->tx = new_tx;

			if ((tree->length - idx) > len)
				new->length = len;
			else
				new->length = (tree->length - idx);

			old = ewl_text_btree_new();
			old->parent = tree;
			old->tx = tree->tx;
			old->length = (tree->length - new->length);

			tree->tx = NULL;
			tree->children = ecore_list_new();

			if (idx == 0)
			{
				ecore_list_append(tree->children, new);
				ecore_list_append(tree->children, old);
			} 
			else if ((idx + len) >= tree->length)
			{
				ecore_list_append(tree->children, old);
				ecore_list_append(tree->children, new);
			}
			else
			{
				Ewl_Text_BTree *old2;

				old->length = idx;
				ecore_list_append(tree->children, old);
				ecore_list_append(tree->children, new);

				old2 = ewl_text_btree_new();
				old2->parent = tree;
				old2->tx = old->tx;
				old2->tx->ref_count ++;
				old2->length = (tree->length - (idx + len));
				ecore_list_append(tree->children, old2);
			}
		}
		DRETURN(DLEVEL_STABLE);
	}

	if (!tree->children)
	{
		DWARNING("No context, no children....?\n");
		DRETURN(DLEVEL_STABLE);
	}

	ecore_list_goto_first(tree->children);
	while ((child = ecore_list_next(tree->children)))
	{

		if ((sum <= idx) && ((sum + child->length) > idx))
		{
			int new_len;

			ewl_text_btree_context_apply(child, tx, context_mask,
								idx - sum, len);
			new_len = len - (child->length - (idx - sum));
			idx += (child->length - (child->length - (idx - sum)));

			if (new_len <= 0) break;
			len = new_len;
		}
		sum += child->length;
	}

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/* just do the removal here. may need to merge up to parents if we
 * are empty, or remove parents but don't try to merge siblings. */
void
ewl_text_btree_text_delete(Ewl_Text_BTree *tree, unsigned int idx, unsigned int len)
{
	Ewl_Text_BTree *child;
	int sum = 0;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("tree", tree);

	/* we have a context, delete from us */
	if (tree->tx)
	{
		if (len > tree->length)
			tree->length = 0;
		else
			tree->length -= len;
		DRETURN(DLEVEL_STABLE);
	}

	if (!tree->children)
	{
		DWARNING("No context, no children....?\n");
		DRETURN(DLEVEL_STABLE);
	}

	ecore_list_goto_first(tree->children);
	while ((child = ecore_list_current(tree->children)))
	{
		int deleted = 0;

		if ((sum <= idx) && ((sum + child->length) > idx))
		{
			int del_length;
			int new_len;

			del_length = (child->length - (idx - sum));
			if (del_length > len)
				del_length = len;

			ewl_text_btree_text_delete(child, idx - sum, del_length);

			tree->length -= del_length;
			new_len = len - del_length;

			if (child->length == 0)
			{
				ecore_list_goto(tree->children, child);
				ecore_list_remove(tree->children);

				ewl_text_btree_free(child);
				deleted ++;
			}

			if (new_len <= 0) break;
			len = new_len;
		}
		sum += child->length;

		if (!deleted)
			ecore_list_next(tree->children);
	}
	ewl_text_btree_shrink(tree);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/* this just merges nodes back into their parent or deletes the parent if it
 * has no children */
static void
ewl_text_btree_shrink(Ewl_Text_BTree *tree)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("tree", tree);

	/* if we have one child left we need to merge it up to us */
	if (ecore_list_nodes(tree->children) == 1)
	{
		Ewl_Text_BTree *child;
		child = ecore_list_goto_first(tree->children);

		ecore_list_destroy(tree->children);
		tree->children = NULL;

		tree->tx = child->tx;
		tree->tx->ref_count ++;

		ewl_text_btree_free(child);
	} 
	else if (ecore_list_nodes(tree->children) == 0)
	{
		if (tree->parent != NULL)
		{
			ecore_list_goto(tree->parent->children, tree);
			ecore_list_remove(tree->parent->children);
			ewl_text_btree_shrink(tree->parent);
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
ewl_text_btree_condense(Ewl_Text_BTree *tree)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("tree", tree);

	/* XXX */

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void
ewl_text_btree_dump(Ewl_Text_BTree *tree, char *indent)
{
	Ewl_Text_BTree *child;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("tree", tree);

	printf("%s---\n", indent);
	printf("%snode (%d)\n", indent, tree->length);

	/* if we have a leaf node, return it */
	if (tree->tx)
	{
		ewl_text_context_print(tree->tx, indent);
		DRETURN(DLEVEL_STABLE);
	}

	/* something bad...no context...no children...wtf? */
	if (!tree->children)
	{
		DWARNING("No children, no context.... wtf?\n");
		DRETURN(DLEVEL_STABLE);
	}

	/* else we need to go through the children */
	ecore_list_goto_first(tree->children);
	while ((child = ecore_list_next(tree->children)))
	{
		char *t;
		t = NEW(char, strlen(indent) + 3);
		if (!t)
		{
			printf("OUT OF MEMORY...\n");
			DRETURN(DLEVEL_STABLE);
		}

		sprintf(t, "%s  ", indent);
		ewl_text_btree_dump(child, t);
		FREE(t);
	}

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static void
ewl_text_display(Ewl_Text *t)
{
	Evas_Coord w = 0, h = 0;

	evas_object_textblock_clear(t->textblock);
	ewl_text_btree_walk(t);

	evas_object_textblock_native_size_get(t->textblock, &w, &h);
	w += 2;

	/* if we don't get a height back try to set the height to the height of
	* the font, if we don't have a font size, make it 1 */
	if (!h) 
	{
		int size;
		size = ewl_text_font_size_get(t, 0);
		h = size;
		if (!h) h = 1;
	}

	ewl_object_preferred_inner_size_set(EWL_OBJECT(t), (int)w, (int)h);
	ewl_widget_configure(EWL_WIDGET(t));
}

static void
ewl_text_plaintext_parse(Evas_Object *tb, char *txt)
{
	char *tmp;

	DENTER_FUNCTION(DLEVEL_STABLE);

	if (!txt) 
	{
		DRETURN(DLEVEL_STABLE);
	}

	for (tmp = txt; *tmp; tmp++) 
	{
		if (*tmp == '\n') 
		{
			*tmp = '\0';
			evas_object_textblock_text_insert(tb, txt);
			evas_object_textblock_format_insert(tb, "\n");
			*tmp = '\n';
			txt = tmp + 1;
		}
		else if (*tmp == '\r' && *(tmp + 1) == '\n') 
		{
			*tmp = '\0';
			evas_object_textblock_text_insert(tb, txt);
			evas_object_textblock_format_insert(tb, "\n");
			*tmp = '\r';
			tmp++;
			txt = tmp + 2;
		}
		else if (*tmp == '\t') 
		{
			*tmp = '\0';
			evas_object_textblock_text_insert(tb, txt);
			evas_object_textblock_format_insert(tb, "\t");
			*tmp = '\t';
			txt = tmp + 1;
		}
	}
	if (*txt)
		evas_object_textblock_text_insert(tb, txt);
}

static void
ewl_text_btree_walk(Ewl_Text *t)
{
	DENTER_FUNCTION(DLEVEL_STABLE);

	if (!t->text) 
	{
		DRETURN(DLEVEL_STABLE);
	}

	ewl_text_btree_node_walk(t->formatting, t, 0);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static void
ewl_text_btree_node_walk(Ewl_Text_BTree *tree, Ewl_Text *t, unsigned int text_pos)
{
	DENTER_FUNCTION(DLEVEL_STABLE);

	/* if we have a context we are a leaf node */
	if (tree->tx)
	{
		char fmt[2048];
		char tmp;
		char *ptr;
		char style[512];
		char align[128];
		Ewl_Text_Context *ctx;

		ctx = tree->tx;
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
						"double_underline_color=#%02x%02x%02x%02x",
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
						"strkethrough=on strikethrough_color=#%02x%02x%02x%02x ",
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
			snprintf(style, sizeof(style), "underline=off strikethrough=off style=off");
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

		/* create the formatting string */
		snprintf(fmt, sizeof(fmt), "font=fonts/%s source=%s size=%d "
					"backing_color=#%02x%02x%02x%02x color=#%02x%02x%02x%02x "
					"%s wrap=%s %s", ctx->font, 
					ewl_theme_path_get(), ctx->size,
					ctx->style_colors.bg.r, ctx->style_colors.bg.g,
					ctx->style_colors.bg.b, ctx->style_colors.bg.a,
					ctx->color.r, ctx->color.g,
					ctx->color.b, ctx->color.a, style, 
					((ctx->wrap) ? "on" : "off"), align);
		evas_object_textblock_format_insert(t->textblock, fmt);

		ptr = t->text + text_pos;
		tmp = *(ptr + tree->length);
		*(ptr + tree->length) = '\0';

		ewl_text_plaintext_parse(t->textblock, ptr);
		*(ptr + tree->length) = tmp;	
	}
	else if (!tree->children)
	{
		DWARNING("Non-Context, non-child node...\n");
	}
	else
	{
		Ewl_Text_BTree *child;

		ecore_list_goto_first(tree->children);
		while ((child = ecore_list_next(tree->children)))
		{
			ewl_text_btree_node_walk(child, t, text_pos);
			text_pos += child->length;
		}
	}

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

	if (!ewl_widget_init(EWL_WIDGET(area), 
		((type == EWL_TEXT_TRIGGER_TYPE_SELECTION) 
		 	? "selection_area" : "trigger_area")))
	{
		DRETURN_INT(FALSE, DLEVEL_STABLE);
	}

	if (type == EWL_TEXT_TRIGGER_TYPE_TRIGGER)
		ewl_widget_color_set(EWL_WIDGET(area), 0, 0, 0, 0);

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

void
ewl_text_selection_cb_configure(Ewl_Widget *w, void *ev, void *data)
{
	Ewl_Text_Trigger *trig;
	int tb_length;
	unsigned int cur_idx = 0, cur_tb_idx = 0;
	
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);

	trig = EWL_TEXT_TRIGGER(w);

	tb_length = evas_object_textblock_length_get(trig->text_parent->textblock);
	ewl_text_trigger_position(trig->text_parent, trig, tb_length, &cur_idx, &cur_tb_idx);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}


