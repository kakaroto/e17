/* vim: set sw=8 ts=8 sts=8 noexpandtab: */
#include "ewl_base.h"
#include "ewl_text_context.h"   
#include "ewl_private.h"
#include "ewl_macros.h"
#include "ewl_debug.h"

/* Make a static hash to look up the context's. They can be shared between
 * the different text blocks. Just need to ref count them so we know when
 * they can be destroyed
 */
static Ecore_Hash *context_hash = NULL;

static void ewl_text_context_cb_free(void *data);
static void ewl_text_context_merge(Ewl_Text_Context *tx, unsigned int context_mask,
                        Ewl_Text_Context *tx_change);

static Ewl_Text_Context *ewl_text_context_dup(Ewl_Text_Context *old);

static unsigned int ewl_text_context_hash_key(const void *ctx);
static int ewl_text_context_hash_cmp(const void *ctx1, const void *ctx2);

static char *ewl_text_context_color_string_get(int r, int g, int b, int a);

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
		ecore_hash_set_free_value(context_hash, 
						ewl_text_context_cb_free);
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

	IF_FREE_HASH(context_hash);

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
 * @param tx: The context to search for
 * @param context_mask the mask of differences
 * @param tx_changes: the set of differences to the given context to use
 * @return Returns the desired context
 * @brief Retrieves, or creates, the needed context
 */
Ewl_Text_Context *
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
			IF_RELEASE(new_tx->format);
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
 * @return Returns the number of references left on this context
 * @brief Releases a reference on the given context. 
 * Do not use the context after this as it will be deallocated if it's 
 * reference count drops to zero.
 */
int
ewl_text_context_release(Ewl_Text_Context *tx)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("tx", tx, 0);

	tx->ref_count --;
	if (tx->ref_count > 0) DRETURN_INT(tx->ref_count, DLEVEL_STABLE);

	ecore_hash_remove(context_hash, tx);

	IF_FREE(tx->font);
	IF_RELEASE(tx->format);
	FREE(tx);

	DRETURN_INT(0, DLEVEL_STABLE);
}

/**
 * @internal
 * @param tx: The context to print
 * @param indent: The indent level to use
 * @return Returns no value
 * @brief Prints out the context information
 **/
void
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

/**
 * @internal
 * @param ctx: The context to work with
 * @return Creates the formatting string and sets it into the context as
 * needed by evas textblock
 * @brief This will give you the format string to pass to textblock based on the
 * context information. 
 **/ 
void
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
			fmt[pos].val = ewl_text_context_color_string_get(
					ctx->style_colors.underline.r,
					ctx->style_colors.underline.g,
					ctx->style_colors.underline.b,
					ctx->style_colors.underline.a);
			fmt[pos++].free = TRUE;

			if (ctx->styles & EWL_TEXT_STYLE_DOUBLE_UNDERLINE)
			{
				fmt[pos].key = "underline2_color";
				fmt[pos].val = ewl_text_context_color_string_get(
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
			fmt[pos].val = ewl_text_context_color_string_get(
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
			fmt[pos].val = ewl_text_context_color_string_get(
					ctx->style_colors.shadow.r,
					ctx->style_colors.shadow.g,
					ctx->style_colors.shadow.b,
					ctx->style_colors.shadow.a);
			fmt[pos++].free = TRUE;

			if (ctx->styles & EWL_TEXT_STYLE_GLOW)
			{
				t = "glow";

				fmt[pos].key = "glow_color";
				fmt[pos].val = ewl_text_context_color_string_get(
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
				fmt[pos].val = ewl_text_context_color_string_get(
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
	fmt[pos].val = ewl_text_context_color_string_get(
			ctx->style_colors.bg.r, ctx->style_colors.bg.g,
			ctx->style_colors.bg.b, ctx->style_colors.bg.a);
	fmt[pos++].free = TRUE;

	fmt[pos].key = "color";
	fmt[pos].val = ewl_text_context_color_string_get(
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
	tx->ref_count = 1;

	tx->format = ((old->format) ? ecore_string_instance((char *)old->format) : NULL);

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

static void
ewl_text_context_cb_free(void *data)
{
	Ewl_Text_Context *tx;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("data", data);

	tx = data;
	while (ewl_text_context_release(tx) > 0) 
		;

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static char *
ewl_text_context_color_string_get(int r, int g, int b, int a)
{
	char buf[10];

	DENTER_FUNCTION(DLEVEL_STABLE);

	snprintf(buf, sizeof(buf), "#%02x%02x%02x%02x", r, g, b, a);

	DRETURN_PTR(strdup(buf), DLEVEL_STABLE);
}

