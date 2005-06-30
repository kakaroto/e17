#include <Ewl.h>
#include "ewl_debug.h"
#include "ewl_macros.h"

/* This counts how many deletes we have before trigger a condense operation
 * on the btree */
#define EWL_TEXT_BTREE_CONDENSE_COUNT  5

/*
 * TODO
 * - need to add api to set/get colours of things like underline, bg,
 *   strikethrough etc
 * - need a way to handle fonts that aren't in the theme .edj
 * - add missing doxygen
 * - need to setup the styles/align/wrap data from the theme in 
 *   ewl_text_context_default_create
 *   - new theme keys for the align/wrap stuff
 * - need to fill in the condense function
 * - need to wrap the click->coord stuff. This has to be careful and add/sub
 *   the \n and \t from the text when going too/from textblock
 */

/* Make a static hash to look up the context's. They can be shared between
 * the differetn text blocks. Just need to ref count them so we know when
 * they can be destroyed
 */
static Ecore_Hash *context_hash = NULL;
static int ewl_text_context_compare(Ewl_Text_Context *a, Ewl_Text_Context *b);
static void ewl_text_context_print(Ewl_Text_Context *tx, char *indent);
static char *ewl_text_context_name_get(const char *font, unsigned int size, 
			unsigned int styles, unsigned int align, 
			unsigned int wrap, unsigned int r, 
			unsigned int g, unsigned int b,
			unsigned int a);
static Ewl_Text_Context *ewl_text_context_find(const char *font, 
			unsigned int size, unsigned int styles, 
			unsigned int align, unsigned int wrap, 
			unsigned int r, unsigned int g, 
			unsigned int b, unsigned int a);
static Ewl_Text_Context *ewl_text_context_default_create(Ewl_Text *t);
static void ewl_text_display(Ewl_Text *t);
static void ewl_text_plaintext_parse(Evas_Object *tb, char *txt);
static void ewl_text_btree_walk(Ewl_Text *t);
static void ewl_text_btree_node_walk(Ewl_Text_BTree *tree, Ewl_Text *t, 
						unsigned int text_pos);
static void ewl_text_btree_shrink(Ewl_Text_BTree *tree);

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

	if (!ewl_widget_init(EWL_WIDGET(t), "text"))
	{
		DRETURN_INT(FALSE, DLEVEL_STABLE);
	}
	ewl_widget_inherit(EWL_WIDGET(t), "text");

	/* create the formatting tree before we do any formatting */
	t->formatting = ewl_text_btree_new();
	if (!t->formatting) 
	{
		DRETURN_INT(FALSE, DLEVEL_STABLE);
	}
	t->current_context = ewl_text_context_default_create(t);

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

		new = malloc(t->length + len + 1);
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
		t->cursor_position += len;
	}

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

	/* remove the text from t->text here, then call */
	old = t->text;
	*(old + t->cursor_position) = '\0';
	ptr = old + t->cursor_position + length;

	t->length -= length;
	t->text = malloc(sizeof(char) * t->length);
	snprintf(t->text, t->length, "%s%s", ((old) ? old : ""), ((ptr) ? ptr : ""));

	/* cleanup the nodes in the btree */
	ewl_text_btree_text_delete(t->formatting, t->cursor_position, length);
	t->delete_count ++;

	if (t->delete_count == EWL_TEXT_BTREE_CONDENSE_COUNT)
	{
		ewl_text_btree_condense(t->formatting);
		t->delete_count = 0;
	}

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param t: The Ewl_Text widget ot set the position into
 * @param pos: The position to set
 * @return Returns no value.
 */
void
ewl_text_cursor_position_set(Ewl_Text *t, unsigned int pos)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("t", t);

	/* make sure we aren't more then the next char past the end of the
	 * text */
	if (pos > t->length) pos = t->length;
	t->cursor_position = pos;
	
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

/**
 * @param t: The Ewl_Widget to set the font into
 * @param font: The font to set
 *
 * This will set the current font to be used when we insert more text
 */
void
ewl_text_font_set(Ewl_Text *t, const char *font)
{
	char *tmp;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("t", t);

	/* null font will go back to the theme default */
	if (!font) tmp = ewl_theme_data_str_get(EWL_WIDGET(t), "font");
	else tmp = strdup(font);

	/* do we have a current context? */
	if (t->current_context)
	{
		/* if font is the same as the current font, done */
		if ((t->current_context->font) && (!strcmp(t->current_context->font, tmp)))
		{
			/* nop, no change to the context */
		}
		else
		{
			Ewl_Text_Context *ctx;

			ctx = t->current_context;
			t->current_context = ewl_text_context_find(tmp, ctx->size,
						ctx->styles, ctx->align, ctx->wrap,
						ctx->color.r, ctx->color.g,
						ctx->color.b, ctx->color.a);
			ctx->ref_count --;
		}
	}
	else
	{
		Ewl_Text_Context *ctx;

		ctx = ewl_text_context_default_create(t);
		t->current_context = ewl_text_context_find(tmp, ctx->size,
						ctx->styles, ctx->align, ctx->wrap,
						ctx->color.r, ctx->color.g,
						ctx->color.b, ctx->color.a);
		ctx->ref_count --;
	}
	IF_FREE(tmp);

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
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("t", t);

	/* do we have a current context? */
	if (t->current_context)
	{
		/* if size is the same as the current size, done */
		if (t->current_context->size == size)
		{
			/* nop, no change to the context */
		}
		else
		{
			Ewl_Text_Context *ctx;

			ctx = t->current_context;
			t->current_context = ewl_text_context_find(ctx->font, size,
						ctx->styles, ctx->align, ctx->wrap,
						ctx->color.r, ctx->color.g,
						ctx->color.b, ctx->color.a);
			ctx->ref_count --;
		}
	}
	else
	{
		Ewl_Text_Context *ctx;

		ctx = ewl_text_context_default_create(t);
		t->current_context = ewl_text_context_find(ctx->font, size,
						ctx->styles, ctx->align, ctx->wrap,
						ctx->color.r, ctx->color.g,
						ctx->color.b, ctx->color.a);
		ctx->ref_count --;
	}

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
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("t", t);

	/* do we have a current context? */
	if (t->current_context)
	{
		/* if colour is the same as the current colour, done */
		if ((t->current_context->color.r == r) 
					&& (t->current_context->color.g == g)
					&& (t->current_context->color.b == b) 
					&& (t->current_context->color.a == a))
		{
			/* nop, no change to the context */
		}
		else
		{
			Ewl_Text_Context *ctx;

			ctx = t->current_context;
			t->current_context = ewl_text_context_find(ctx->font, ctx->size,
						ctx->styles, ctx->align, ctx->wrap,
						r, g, b, a);
			ctx->ref_count --;
		}
	}
	else
	{
		Ewl_Text_Context *ctx;

		ctx = ewl_text_context_default_create(t);
		t->current_context = ewl_text_context_find(ctx->font, ctx->size,
						ctx->styles, ctx->align, ctx->wrap,
						r, g, b, a);
		ctx->ref_count --;
	}

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

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @aparam t: The Ewl_Text to get the colour from
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
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("t", t);

	/* do we have a current context? */
	if (t->current_context)
	{
		/* if alignment is the same as the current font, done */
		if ((t->current_context->align == align) )
		{
			/* nop, no change to the context */
		}
		else
		{
			Ewl_Text_Context *ctx;

			ctx = t->current_context;
			t->current_context = ewl_text_context_find(ctx->font, ctx->size,
						ctx->styles, align, ctx->wrap,
						ctx->color.r, ctx->color.g, 
						ctx->color.b, ctx->color.a);
			ctx->ref_count --;
		}
	}
	else
	{
		Ewl_Text_Context *ctx;

		ctx = ewl_text_context_default_create(t);
		t->current_context = ewl_text_context_find(ctx->font, ctx->size,
						ctx->styles, align, ctx->wrap,
						ctx->color.r, ctx->color.g,
						ctx->color.b, ctx->color.a);
		ctx->ref_count --;
	}

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
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("t", t);

	/* do we have a current context? */
	if (t->current_context)
	{
		/* if style is the same as the current font, done */
		if ((t->current_context->styles == styles) )
		{
			/* nop, no change to the context */
		}
		else
		{
			Ewl_Text_Context *ctx;

			ctx = t->current_context;
			t->current_context = ewl_text_context_find(ctx->font, ctx->size,
						styles, ctx->align, ctx->wrap,
						ctx->color.r, ctx->color.g, 
						ctx->color.b, ctx->color.a);
			ctx->ref_count --;
		}
	}
	else
	{
		Ewl_Text_Context *ctx;

		ctx = ewl_text_context_default_create(t);
		t->current_context = ewl_text_context_find(ctx->font, ctx->size,
						styles, ctx->align, ctx->wrap,
						ctx->color.r, ctx->color.g,
						ctx->color.b, ctx->color.a);
		ctx->ref_count --;
	}

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
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("t", t);

	/* do we have a current context? */
	if (t->current_context)
	{
		/* if wrap is the same as the current font, done */
		if ((t->current_context->wrap == wrap) )
		{
			/* nop, no change to the context */
		}
		else
		{
			Ewl_Text_Context *ctx;

			ctx = t->current_context;
			t->current_context = ewl_text_context_find(ctx->font, ctx->size,
						ctx->styles, ctx->align, wrap,
						ctx->color.r, ctx->color.g, 
						ctx->color.b, ctx->color.a);
			ctx->ref_count --;
		}
	}
	else
	{
		Ewl_Text_Context *ctx;

		ctx = ewl_text_context_default_create(t);
		t->current_context = ewl_text_context_find(ctx->font, ctx->size,
						ctx->styles, ctx->align, wrap,
						ctx->color.r, ctx->color.g,
						ctx->color.b, ctx->color.a);
		ctx->ref_count --;
	}


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

/*
 * Internal stuff 
 */
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
		evas_object_layer_set(t->textblock, ewl_widget_layer_sum_get(w));
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

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void
ewl_text_cb_show(Ewl_Widget *w, void *ev, void *data)
{
	Ewl_Text *t;

	DENTER_FUNCTION(DLEVEL_STABLE);

	t = EWL_TEXT(w);
	evas_object_show(t->textblock);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void
ewl_text_cb_hide(Ewl_Widget *w, void *ev, void *data)
{
	Ewl_Text *t;

	DENTER_FUNCTION(DLEVEL_STABLE);

	t = EWL_TEXT(w);
	evas_object_hide(t->textblock);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static Ewl_Text_Context *
ewl_text_context_default_create(Ewl_Text *t)
{
	char *tmp;
	int size, r, g, b, a;
	Ewl_Text_Context *tx = NULL;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("t", t, NULL);

	/* handle default values */
	tmp = ewl_theme_data_str_get(EWL_WIDGET(t), "font");
	size = ewl_theme_data_int_get(EWL_WIDGET(t), "font_size");

	r = ewl_theme_data_int_get(EWL_WIDGET(t), "color/r");
	g = ewl_theme_data_int_get(EWL_WIDGET(t), "color/g");
	b = ewl_theme_data_int_get(EWL_WIDGET(t), "color/b");
	a = ewl_theme_data_int_get(EWL_WIDGET(t), "color/a");

	/* XXX grap the style, alignment and wrap data from the theme here */

	tx = ewl_text_context_find(tmp, size, 0, 0, 0, r, g, b, a);
	IF_FREE(tmp);

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
	}
}

static char *
ewl_text_context_name_get(const char *font, unsigned int size, unsigned int styles, 
			unsigned int align, unsigned int wrap, unsigned int r, 
			unsigned int g, unsigned int b, unsigned int a)
{
	char name[2048];
	char *t;

	DENTER_FUNCTION(DLEVEL_STABLE);

	if (!font) t = "";
	else t = (char *)font;

	snprintf(name, sizeof(name), "f%ss%ds%da%dw%dr%dg%db%da%d", t, size, styles, 
							align, wrap, r, g, b, a);
	DRETURN_PTR(strdup(name), DLEVEL_STABLE);
}

static Ewl_Text_Context *
ewl_text_context_find(const char *font, unsigned int size, unsigned int styles, 
			unsigned int align, unsigned int wrap, unsigned int r, 
			unsigned int g, unsigned int b, unsigned int a)
{
	char *t;
	Ewl_Text_Context *tx;

	DENTER_FUNCTION(DLEVEL_STABLE);

	t = ewl_text_context_name_get(font, size, styles, align, wrap, r, g, b, a);
	tx = ecore_hash_get(context_hash, t);
	if (!tx)
	{
		if ((tx = ewl_text_context_new()))
		{
			if (font) ewl_text_context_font_set(tx, font);
			ewl_text_context_font_size_set(tx, size);
			ewl_text_context_styles_set(tx, styles);
			ewl_text_context_align_set(tx, align);
			ewl_text_context_wrap_set(tx, wrap);
			ewl_text_context_color_set(tx, r, g, b, a);

			ecore_hash_set(context_hash, strdup(t), tx);
		}
	}
	if (tx) tx->ref_count ++;
	FREE(t);

	DRETURN_PTR(tx, DLEVEL_STABLE);
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

	t = ewl_text_context_name_get(tx->font, tx->size, tx->styles, tx->align,
					tx->wrap, tx->color.r, tx->color.g,
					tx->color.b, tx->color.a);
	ecore_hash_remove(context_hash, t);

	IF_FREE(tx->font);
	FREE(tx);
	FREE(t);

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
		tree->tx--;
		if (tree->tx->ref_count <= 0)
			ewl_text_context_free(tree->tx);

		tree->tx = NULL;
	}
	IF_FREE(tree);

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
		new_tx = ewl_text_context_find(
			((context_mask & EWL_TEXT_CONTEXT_MASK_FONT) ? tx->font : ctx->font),
			((context_mask & EWL_TEXT_CONTEXT_MASK_SIZE) ? tx->size : ctx->size),
			((context_mask & EWL_TEXT_CONTEXT_MASK_STYLES) ? tx->styles : ctx->styles),
			((context_mask & EWL_TEXT_CONTEXT_MASK_ALIGN) ? tx->align : ctx->align),
			((context_mask & EWL_TEXT_CONTEXT_MASK_WRAP) ? tx->wrap : ctx->wrap),
			((context_mask & EWL_TEXT_CONTEXT_MASK_COLOR) ? tx->color.r : ctx->color.r),
			((context_mask & EWL_TEXT_CONTEXT_MASK_COLOR) ? tx->color.g : ctx->color.g),
			((context_mask & EWL_TEXT_CONTEXT_MASK_COLOR) ? tx->color.b : ctx->color.b),
			((context_mask & EWL_TEXT_CONTEXT_MASK_COLOR) ? tx->color.a : ctx->color.a));

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
		if ((sum <= idx) && ((sum + child->length) >= idx))
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

		if ((sum <= idx) && ((sum + child->length) >= idx))
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
		t = NEW(char, strlen(indent + 3));
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

	ewl_text_btree_walk(t);

	evas_object_textblock_native_size_get(t->textblock, &w, &h);
	w += 2;
	if (!h) h = 1;

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
		char style[256];
		char align[128];
		Ewl_Text_Context *ctx;

		ctx = tree->tx;

		/* create the style string */
		ptr = style;
		if (ctx->styles != EWL_TEXT_STYLE_NONE)
		{
			if ((ctx->styles & EWL_TEXT_STYLE_UNDERLINE) || 
					(ctx->styles & EWL_TEXT_STYLE_DOUBLE_UNDERLINE))
			{
				snprintf(ptr, sizeof(style) - strlen(style), 
						"underline=%s underline_color=#000000ff "
						"double_underline_color=#000000ff ",
						((ctx->styles & EWL_TEXT_STYLE_UNDERLINE) ?
						  "on" : "double"));
			}
			else
				snprintf(ptr, sizeof(style) - strlen(style), "underline=off ");
			ptr = style + strlen(style);

			if (ctx->styles & EWL_TEXT_STYLE_STRIKETHROUGH)
				snprintf(ptr, sizeof(style) - strlen(style), 
						"strkethrough=on strikethrough_color=#000000ff ");
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
							"style=glow glow_color=#ff0000ff ");

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
							"outline_color=#aaaaaaff ");
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
							"shadow_color=#444444ff ");
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
					"color=#%02x%02x%02x%02x %s wrap=%s %s", ctx->font, 
					ewl_theme_path_get(), ctx->size,
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


