#include <Ewl.h>

static void ewl_text_ops_apply(Ewl_Text *ta);
static void ewl_text_op_free(void *data);
static void ewl_text_update_size(Ewl_Text * ta);

/*
 * Private functions for applying operations to the text at realize time.
 */
static Ewl_Text_Op *ewl_text_op_color_new(Ewl_Text *ta, int r, int g, int b,
					  int a);
static void ewl_text_op_color_apply(Ewl_Text *ta, Ewl_Text_Op *op);

static Ewl_Text_Op *ewl_text_op_font_new(Ewl_Text *ta, char *font, int size);
static void ewl_text_op_font_apply(Ewl_Text *ta, Ewl_Text_Op *op);
static void ewl_text_op_font_free(void *op);

static Ewl_Text_Op *ewl_text_op_style_new(Ewl_Text *ta, char *style);
static void ewl_text_op_style_apply(Ewl_Text *ta, Ewl_Text_Op *op);
static void ewl_text_op_style_free(void *op);

static Ewl_Text_Op * ewl_text_op_align_new(Ewl_Text *ta, unsigned int align);
static void ewl_text_op_align_apply(Ewl_Text *ta, Ewl_Text_Op *op);

/**
 * @param text: the initial text of the text
 * @return Returns a pointer to a new text on success, NULL on failure.
 * @brief Allocate a new text area widget
 *
 * Sets the text initially to @a text if not NULL.
 */
Ewl_Widget     *ewl_text_new(char *text)
{
	Ewl_Text   *ta;

	DENTER_FUNCTION(DLEVEL_STABLE);

	ta = NEW(Ewl_Text, 1);
	if (!ta)
		DRETURN_PTR(NULL, DLEVEL_STABLE);

	ewl_text_init(ta, text);

	DRETURN_PTR(EWL_WIDGET(ta), DLEVEL_STABLE);
}

/**
 * @param ta: the text area to be initialized
 * @param text: the text to be displayed initially in the text area
 * @return Returns no value.
 * @brief Initialize the fields and callbacks of a text area
 *
 * Sets the internal fields and callbacks of a text area to their defaults.
 */
void ewl_text_init(Ewl_Text * ta, char *text)
{
	Ewl_Widget *w;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("ta", ta);

	w = EWL_WIDGET(ta);

	ewl_widget_init(EWL_WIDGET(w), "text");

	ewl_callback_append(w, EWL_CALLBACK_REALIZE, ewl_text_realize_cb,
			    NULL);
	ewl_callback_append(w, EWL_CALLBACK_UNREALIZE,
			    ewl_text_unrealize_cb, NULL);
	ewl_callback_append(w, EWL_CALLBACK_DESTROY,
			    ewl_text_destroy_cb, NULL);
	ewl_callback_append(w, EWL_CALLBACK_REPARENT, ewl_text_reparent_cb,
			    NULL);
	ewl_callback_append(w, EWL_CALLBACK_CONFIGURE,
			    ewl_text_configure_cb, NULL);

	ta->ops = ecore_list_new();
	ta->applied = ecore_list_new();

	ecore_list_set_free_cb(ta->ops, ewl_text_op_free);
	ecore_list_set_free_cb(ta->applied, ewl_text_op_free);

	if (text)
		ewl_text_text_set(ta, text);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param ta: the text area widget to set the text
 * @param text: the text to set in the text area widget @a ta
 * @return Returns no value.
 * @brief Set the text of a text area widget
 *
 * Sets the text of the text area widget @a ta to a copy of the contents of
 * @a text.
 */
void ewl_text_text_set(Ewl_Text * ta, char *text)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("ta", ta);
	DCHECK_PARAM_PTR("text", text);

	/*
	 * Keep a copy of the text for quick access and for creating the etox
	 * when needed.
	 */
	ta->text = strdup(text);

	/*
	 * Update the etox and the sizing of the text widget.
	 */
	if (ta->etox) {
		etox_set_text(ta->etox, text);
		ewl_text_update_size(ta);
	}

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param ta: the text widget to retrieve text contents
 * @return Returns a copy of the text in @a ta on success, NULL on failure.
 * @brief Retrieve the text of a text widget
 */
char *ewl_text_text_get(Ewl_Text * ta)
{
	char *txt = NULL;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("ta", ta, NULL);

	if (ta->etox)
		txt = etox_get_text(ta->etox);
	else if (ta->text)
		txt = strdup(ta->text);

	DRETURN_PTR(txt, DLEVEL_STABLE);
}

/**
 * @param ta: the text widget to retrieve length
 * @return Returns the length of the text contained in the widget.
 * @brief Retrieve the length of the text displayed by the text widget.
 */
int ewl_text_length_get(Ewl_Text *ta)
{
	int len = 0;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("ta", ta, 0);

	if (ta->etox)
		len = etox_get_length(ta->etox);
	else if (ta->text)
		len = strlen(ta->text);

	DRETURN_INT(len, DLEVEL_STABLE);
}

/**
 * @param ta: the text widget to change color
 * @param r: the new red value
 * @param g: the new green value
 * @param b: the new blue value
 * @param a: the new alpha value
 * @brief Changes the currently applied color of the text to specified values
 * @return Returns no value.
 */
void ewl_text_color_set(Ewl_Text *ta, int r, int g, int b, int a)
{
	Ewl_Text_Op *op;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("ta", ta);

	op = ewl_text_op_color_new(ta, r, g, b, a);
	ecore_list_append(ta->ops, op);
	if (REALIZED(ta))
		ewl_text_ops_apply(ta);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param ta: the text widget to change font
 * @param font: the name of the font
 * @param size: the size of the font
 * @brief Changes the currently applied font of the text to specified values
 * @return Returns no value.
 */
void ewl_text_font_set(Ewl_Text *ta, char *font, int size)
{
	Ewl_Text_Op *op;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("ta", ta);

	op = ewl_text_op_font_new(ta, font, size);
	ecore_list_append(ta->ops, op);
	if (REALIZED(ta))
		ewl_text_ops_apply(ta);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param ta: the text widget to map a coordinate to a character index
 * @param x: the x coordinate over the desired character
 * @param y: the y coordinate over the desired character
 * @brief Finds the index of the character under the specified coordinates
 * @return Returns the index of the found character on success, 0 otherwise.
 */
int ewl_text_coord_index_map(Ewl_Text *ta, int x, int y)
{
	int index;
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("ta", ta, 0);

	if (!ta->etox)
		DRETURN_INT(0, DLEVEL_STABLE);

	index = etox_coord_to_index(ta->etox, (Evas_Coord)(x), (Evas_Coord)(y));
	DRETURN_INT(index, DLEVEL_STABLE);
}

/**
 * @param ta: the text widget to map index to character geometry
 * @param index: character index to be mapped
 * @param x: pointer to store determined character x coordinate
 * @param y: pointer to store determined character y coordinate
 * @param w: pointer to store determined character width
 * @param h: pointer to store determined character height
 * @return Returns no value.
 * @brief Maps a character index to a set of coordinates and sizes.
 *
 * Any of the coordinate parameters may be NULL, they will be ignored. If the
 * index fails to map successfully, the values at the locations pointed to by
 * the coordinate pointers will not be altered. This function can only succeed
 * after the text widget has been realized.
 */
void ewl_text_index_geometry_map(Ewl_Text *ta, int index, int *x, int *y,
				 int *w, int *h)
{
	Evas_Coord tx, ty, tw, th;

	DENTER_FUNCTION(DLEVEL_STABLE);
	if (!ta->etox)
		DRETURN(DLEVEL_STABLE);

	etox_index_to_geometry(ta->etox, index, &tx, &ty, &tw, &th);
	if (x)
		*x = (int)(tx);
	if (y)
		*y = (int)(ty);
	if (w)
		*w = (int)(tw);
	if (h)
		*h = (int)(th);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void ewl_text_realize_cb(Ewl_Widget * w, void *ev_data, void *user_data)
{
	char      *tmp;
	Ewl_Text  *ta;
	Ewl_Embed *emb;
	int r, g, b, a;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);

	ta = EWL_TEXT(w);

	/*
	 * Find the embed so we know which evas to draw onto.
	 */
	emb = ewl_embed_find_by_widget(w);

	/*
	 * Create the etox
	 */
	ta->etox = etox_new(emb->evas);
	ta->context = etox_get_context(ta->etox);

	tmp = ewl_theme_data_get_str(w, "font");
	etox_context_set_font(ta->context, tmp,
			      ewl_theme_data_get_int(w, "font_size"));
	IF_FREE(tmp);

	tmp = ewl_theme_data_get_str(w, "style");
	etox_context_set_style(ta->context, tmp);
	IF_FREE(tmp);

	r = ewl_theme_data_get_int(w, "color/r");
	g = ewl_theme_data_get_int(w, "color/g");
	b = ewl_theme_data_get_int(w, "color/b");
	a = ewl_theme_data_get_int(w, "color/a");
	etox_context_set_color(ta->context, r, g, b, a);

	if (w->fx_clip_box)
		evas_object_clip_set(ta->etox, w->fx_clip_box);

	/*
	 * Now set the text and display it.
	 */
	etox_set_text(ta->etox, ta->text);
	evas_object_show(ta->etox);

	ewl_text_ops_apply(ta);

	/*
	 * Update the size of the text
	 */
	ewl_text_update_size(ta);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static void ewl_text_ops_apply(Ewl_Text *ta)
{
	Ewl_Text_Op *op;

	while ((op = ecore_list_remove(ta->ops))) {
		op->apply(ta, op);
		ecore_list_append(ta->applied, op);
	}
}

static void ewl_text_op_free(void *data)
{
	Ewl_Text_Op *op = data;
	if (op->free)
		op->free(op);
	else
		FREE(op)
}

void ewl_text_unrealize_cb(Ewl_Widget * w, void *ev_data, void *user_data)
{
	Ewl_Text   *ta;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);

	ta = EWL_TEXT(w);

	evas_object_clip_unset(ta->etox);
	evas_object_del(ta->etox);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void ewl_text_destroy_cb(Ewl_Widget * w, void *ev_data, void *user_data)
{
	Ewl_Text   *ta;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);

	ta = EWL_TEXT(w);
	ecore_list_destroy(ta->ops);
	ta->ops = NULL;

	ecore_list_destroy(ta->applied);
	ta->applied = NULL;

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void ewl_text_reparent_cb(Ewl_Widget * w, void *ev_data, void *user_data)
{
	Ewl_Text *ta;

	DENTER_FUNCTION(DLEVEL_STABLE);

	ta = EWL_TEXT(w);
	if (ta->etox)
		evas_object_layer_set(ta->etox, ewl_widget_get_layer_sum(w));

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void ewl_text_configure_cb(Ewl_Widget * w, void *ev_data, void *user_data)
{
	Ewl_Text   *ta;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);

	ta = EWL_TEXT(w);

	/*
	 * Update the etox position and size.
	 */
	if (ta->etox) {
		evas_object_move(ta->etox, CURRENT_X(w), CURRENT_Y(w));
		evas_object_layer_set(ta->etox, ewl_widget_get_layer_sum(w));
	}

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/*
 * Set the size of the text area to the size of the etox.
 */
static void ewl_text_update_size(Ewl_Text * ta)
{
	Evas_Coord x, y, width, height;

	/*
	 * Adjust the properties of the widget to indicate the size of the text.
	 */
	evas_object_geometry_get(ta->etox, &x, &y, &width, &height);

	/*
	 * Set the preferred size to the size of the etox and request that
	 * size for the widget.
	 */
	ewl_object_set_preferred_size(EWL_OBJECT(ta), (int)(width),
				      (int)(height));

	/*
	 * FIXME: Should we really be doing this? Probably not, test it out
	 * more thoroughly.
	 */
	ewl_object_set_custom_size(EWL_OBJECT(ta), (int)(width),
				   (int)(height));
}

static Ewl_Text_Op *
ewl_text_op_color_new(Ewl_Text *ta, int r, int g, int b, int a)
{
	Ewl_Text_Op *op;
	Ewl_Text_Op_Color *opc;

	DENTER_FUNCTION(DLEVEL_STABLE);

	op = NEW(Ewl_Text_Op_Color, 1);
	if (op) {
		opc = (Ewl_Text_Op_Color *)op;
		op->type = EWL_TEXT_OP_TYPE_COLOR_SET;
		op->apply = ewl_text_op_color_apply;
		op->free = free;
		opc->r = r;
		opc->g = g;
		opc->b = b;
		opc->a = a;
	}

	DRETURN_PTR(op, DLEVEL_STABLE);
}

static void
ewl_text_op_color_apply(Ewl_Text *ta, Ewl_Text_Op *op)
{
	int or, og, ob, oa;
	Ewl_Text_Op_Color *opc = (Ewl_Text_Op_Color *)op;

	DENTER_FUNCTION(DLEVEL_STABLE);

	etox_context_get_color(ta->context, &or, &og, &ob, &oa);
	etox_context_set_color(ta->context, opc->r, opc->g, opc->b, opc->a);

	/*
	 * Store the previous values for undoing.
	 */
	opc->r = or;
	opc->g = og;
	opc->b = ob;
	opc->a = oa;

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static Ewl_Text_Op *
ewl_text_op_font_new(Ewl_Text *ta, char *font, int size)
{
	Ewl_Text_Op *op;
	Ewl_Text_Op_Font *opf;

	DENTER_FUNCTION(DLEVEL_STABLE);

	op = NEW(Ewl_Text_Op_Font, 1);
	if (op) {
		opf = (Ewl_Text_Op_Font *)op;
		op->type = EWL_TEXT_OP_TYPE_FONT_SET;
		op->apply = ewl_text_op_font_apply;
		op->free = ewl_text_op_font_free;
		opf->font = strdup(font);
		opf->size = size;
	}

	DRETURN_PTR(op, DLEVEL_STABLE);
}

static void
ewl_text_op_font_apply(Ewl_Text *ta, Ewl_Text_Op *op)
{
	char *of;
	int size;
	Ewl_Text_Op_Font *opf = (Ewl_Text_Op_Font *)op;

	DENTER_FUNCTION(DLEVEL_STABLE);

	of = etox_context_get_font(ta->context, &size);

	etox_context_set_font(ta->context, opf->font, opf->size);

	FREE(opf->font);
	opf->font = of;
	opf->size = size;

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static void
ewl_text_op_font_free(void *op)
{
	Ewl_Text_Op_Font *opf = (Ewl_Text_Op_Font *)op;
	DENTER_FUNCTION(DLEVEL_STABLE);

	FREE(opf->font);
	FREE(opf);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static Ewl_Text_Op *
ewl_text_op_style_new(Ewl_Text *ta, char *style)
{
	Ewl_Text_Op *op;
	Ewl_Text_Op_Style *ops;

	DENTER_FUNCTION(DLEVEL_STABLE);

	op = NEW(Ewl_Text_Op_Style, 1);
	if (op) {
		ops = (Ewl_Text_Op_Style *)op;
		op->type = EWL_TEXT_OP_TYPE_STYLE_SET;
		op->apply = ewl_text_op_style_apply;
		op->free = ewl_text_op_style_free;
		ops->style = strdup(style);
	}

	DRETURN_PTR(op, DLEVEL_STABLE);
}

static void
ewl_text_op_style_apply(Ewl_Text *ta, Ewl_Text_Op *op)
{
	char *style;
	Ewl_Text_Op_Style *ops = (Ewl_Text_Op_Style *)op;

	DENTER_FUNCTION(DLEVEL_STABLE);

	style = etox_context_get_style(ta->context);
	etox_context_set_style(ta->context, ops->style);

	FREE(ops->style);
	ops->style = style;

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static void
ewl_text_op_style_free(void *op)
{
	Ewl_Text_Op_Style *ops = (Ewl_Text_Op_Style *)op;
	DENTER_FUNCTION(DLEVEL_STABLE);

	FREE(ops->style);
	FREE(ops);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static Ewl_Text_Op *
ewl_text_op_align_new(Ewl_Text *ta, unsigned int align)
{
	Ewl_Text_Op *op;
	Ewl_Text_Op_Align *opa;

	DENTER_FUNCTION(DLEVEL_STABLE);

	op = NEW(Ewl_Text_Op_Align, 1);
	if (op) {
		opa = (Ewl_Text_Op_Align *)op;
		op->type = EWL_TEXT_OP_TYPE_ALIGN_SET;
		op->apply = ewl_text_op_align_apply;
		op->free = free;
		opa->align = align;
	}

	DRETURN_PTR(op, DLEVEL_STABLE);
}

static void
ewl_text_op_align_apply(Ewl_Text *ta, Ewl_Text_Op *op)
{
	unsigned int align;
	Ewl_Text_Op_Align *opa = (Ewl_Text_Op_Align *)op;

	DENTER_FUNCTION(DLEVEL_STABLE);

	align = etox_context_get_align(ta->context);
	etox_context_set_align(ta->context, opa->align);

	opa->align = align;

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}
