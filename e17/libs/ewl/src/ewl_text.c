#include <Ewl.h>

static void ewl_text_ops_apply(Ewl_Text *ta);
static void ewl_text_op_prune_list(Ewl_Text *ta, int rstart, int rend, 
				   int bstart, int bend);
static void ewl_text_op_free(void *data);
static Ewl_Text_Op *ewl_text_op_relevant_find(Ewl_Text *ta,
					      Ewl_Text_Op_Type type);
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

static Ewl_Text_Op *ewl_text_op_align_new(Ewl_Text *ta, unsigned int align);
static void ewl_text_op_align_apply(Ewl_Text *ta, Ewl_Text_Op *op);

static Ewl_Text_Op * ewl_text_op_text_set_new(Ewl_Text *ta, char *text);
static Ewl_Text_Op *ewl_text_op_text_append_new(Ewl_Text *ta, char *text);
static Ewl_Text_Op *ewl_text_op_text_prepend_new(Ewl_Text *ta, char *text);
static Ewl_Text_Op *ewl_text_op_text_insert_new(Ewl_Text *ta, char *text,
						int index);
static void ewl_text_op_text_apply(Ewl_Text *ta, Ewl_Text_Op *op);
static void ewl_text_op_text_free(void *op);

/**
 * @param text: the initial text of the text
 * @return Returns a pointer to a new text on success, NULL on failure.
 * @brief Allocate a new text area widget
 *
 * Sets the text initially to @a text if not NULL. This initial text will use
 * the default formatting information from the theme. Pass @a text as NULL and
 * use ewl_text_text_set after changing the settings to change attributes the
 * text.
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
	ewl_object_fill_policy_set(EWL_OBJECT(w), EWL_FLAG_FILL_NONE);

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

	ta->ops = ecore_dlist_new();
	ta->applied = ecore_dlist_new();

	ecore_dlist_set_free_cb(ta->ops, ewl_text_op_free);
	ecore_dlist_set_free_cb(ta->applied, ewl_text_op_free);

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
	Ewl_Text_Op *op;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("ta", ta);

	op = ewl_text_op_text_set_new(ta, text);
	ewl_text_op_prune_list(ta, EWL_TEXT_OP_TYPE_TEXT_SET,
				   EWL_TEXT_OP_TYPE_TEXT_DELETE, -1, -1);
	ecore_dlist_append(ta->ops, op);
	if (REALIZED(ta))
		ewl_text_ops_apply(ta);

	IF_FREE(ta->text);
	if (text) {
		ta->text = strdup(text);
		text = strdup(text);
		ta->length = strlen(text);
	}
	else
		ta->length = 0;

	ewl_callback_call_with_event_data(EWL_WIDGET(ta),
					  EWL_CALLBACK_VALUE_CHANGED, text);
	IF_FREE(text);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param ta: the text area widget to append the text
 * @param text: the text to append in the text area widget @a ta
 * @return Returns no value.
 * @brief Append text to a text area widget
 *
 * Appends text to the text area widget @a ta.
 */
void ewl_text_text_append(Ewl_Text * ta, char *text)
{
	int len = 0;
	Ewl_Text_Op *op;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("ta", ta);
	DCHECK_PARAM_PTR("text", text);

	if (ta->text) {
		len = strlen(ta->text) + strlen(text);
		ta->text = realloc(ta->text, sizeof(char) * (len + 1));
		strcat(ta->text, text);
	}
	else {
		ta->text = strdup(text);
		len = strlen(text);
	}

	ta->length = len;

	op = ewl_text_op_text_append_new(ta, text);
	ecore_dlist_append(ta->ops, op);
	if (REALIZED(ta))
		ewl_text_ops_apply(ta);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param ta: the text area widget to prepend the text
 * @param text: the text to prepend in the text area widget @a ta
 * @return Returns no value.
 * @brief Append text to a text area widget
 *
 * Appends text to the text area widget @a ta.
 */
void ewl_text_text_prepend(Ewl_Text * ta, char *text)
{
	Ewl_Text_Op *op;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("ta", ta);
	DCHECK_PARAM_PTR("text", text);

	op = ewl_text_op_text_prepend_new(ta, text);
	ecore_dlist_append(ta->ops, op);
	if (REALIZED(ta))
		ewl_text_ops_apply(ta);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param ta: the text area widget to insert the text
 * @param text: the text to insert in the text area widget @a ta
 * @param index: the index into the text to start inserting new text
 * @return Returns no value.
 * @brief Append text to a text area widget
 *
 * Appends text to the text area widget @a ta.
 */
void ewl_text_text_insert(Ewl_Text * ta, char *text, int index)
{
	Ewl_Text_Op *op;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("ta", ta);
	DCHECK_PARAM_PTR("text", text);

	op = ewl_text_op_text_insert_new(ta, text, index);
	ecore_dlist_prepend(ta->ops, op);
	if (REALIZED(ta))
		ewl_text_ops_apply(ta);

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
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("ta", ta, 0);

	DRETURN_INT(ta->length, DLEVEL_STABLE);
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
	/*
	 * Remove all color sets prior to a text addition/set operation.
	 */
	ewl_text_op_prune_list(ta, EWL_TEXT_OP_TYPE_COLOR_SET,
				   EWL_TEXT_OP_TYPE_COLOR_SET,
				   EWL_TEXT_OP_TYPE_TEXT_SET,
				   EWL_TEXT_OP_TYPE_TEXT_INSERT);
	ecore_dlist_append(ta->ops, op);
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
	ewl_text_op_prune_list(ta, EWL_TEXT_OP_TYPE_FONT_SET,
				   EWL_TEXT_OP_TYPE_FONT_SET,
				   EWL_TEXT_OP_TYPE_TEXT_SET,
				   EWL_TEXT_OP_TYPE_TEXT_INSERT);
	ecore_dlist_append(ta->ops, op);
	if (REALIZED(ta))
		ewl_text_ops_apply(ta);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param ta: the text widget to retrieve the current font
 * @brief Retrieve the name of the currently used font.
 * @return Returns a copied string containing the name of the current font.
 */
char *ewl_text_font_get(Ewl_Text *ta)
{
	Ewl_Text_Op *op;
	Ewl_Text_Op_Font *opf;
	char *font = NULL;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("ta", ta, NULL);

	op = ewl_text_op_relevant_find(ta, EWL_TEXT_OP_TYPE_FONT_SET);
	opf = (Ewl_Text_Op_Font *)op;
	if (opf && opf->font) {
		font = strdup(opf->font);
	}

	DRETURN_PTR(font, DLEVEL_STABLE);
}

/**
 * @param ta: the text widget to change style
 * @param style: the name of the style
 * @brief Changes the currently applied style of the text to specified values
 * @return Returns no value.
 */
void ewl_text_style_set(Ewl_Text *ta, char *style)
{
	Ewl_Text_Op *op;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("ta", ta);

	op = ewl_text_op_style_new(ta, style);
	ewl_text_op_prune_list(ta, EWL_TEXT_OP_TYPE_STYLE_SET,
				   EWL_TEXT_OP_TYPE_STYLE_SET,
				   EWL_TEXT_OP_TYPE_TEXT_SET,
				   EWL_TEXT_OP_TYPE_TEXT_INSERT);
	ecore_dlist_append(ta->ops, op);
	if (REALIZED(ta))
		ewl_text_ops_apply(ta);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param ta: the text widget to get the current style
 * @brief Retrieves the currently used text style from a text widget.
 * @return Returns the currently used text style.
 */
char *ewl_text_style_get(Ewl_Text *ta)
{
	Ewl_Text_Op *op;
	Ewl_Text_Op_Style *ops;
	char *style = NULL;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("ta", ta, NULL);

	op = ewl_text_op_relevant_find(ta, EWL_TEXT_OP_TYPE_FONT_SET);
	ops = (Ewl_Text_Op_Style *)op;
	if (ops && ops->style) {
		style = strdup(ops->style);
	}

	DRETURN_PTR(style, DLEVEL_STABLE);
}

/**
 * @param ta: the text widget to change alignment
 * @param align: the new alignment of the text widget
 * @brief Changes the currently applied alignment of the text to specified value
 * @return Returns no value.
 */
void ewl_text_align_set(Ewl_Text *ta, unsigned int align)
{
	Ewl_Text_Op *op;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("ta", ta);

	op = ewl_text_op_align_new(ta, align);
	ewl_text_op_prune_list(ta, EWL_TEXT_OP_TYPE_ALIGN_SET,
				   EWL_TEXT_OP_TYPE_ALIGN_SET,
				   EWL_TEXT_OP_TYPE_TEXT_SET,
				   EWL_TEXT_OP_TYPE_TEXT_INSERT);
	ecore_dlist_append(ta->ops, op);
	if (REALIZED(ta))
		ewl_text_ops_apply(ta);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param ta: the text widget to get the current alignment
 * @brief Retrieves the currently used text alignment from a text widget.
 * @return Returns the currently used text alignment.
 */
unsigned int ewl_text_align_get(Ewl_Text *ta)
{
	unsigned int align = 0;
	Ewl_Text_Op *op;
	Ewl_Text_Op_Align *opa;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("ta", ta, 0);

	op = ewl_text_op_relevant_find(ta, EWL_TEXT_OP_TYPE_FONT_SET);
	opa = (Ewl_Text_Op_Align *)op;
	if (opa) {
		align = opa->align;
	}

	DRETURN_INT(align, DLEVEL_STABLE);
}

static Ewl_Text_Op *
ewl_text_op_relevant_find(Ewl_Text *ta, Ewl_Text_Op_Type type)
{
	Ecore_DList *l;
	void *(*traverse)(Ecore_DList *l);
	Ewl_Text_Op *op = NULL;

	DENTER_FUNCTION(DLEVEL_STABLE);

	if (REALIZED(ta)) {
		l = ta->applied;
		ecore_dlist_goto_first(l);
		traverse = ecore_dlist_next;
	}
	else {
		l = ta->ops;
		ecore_dlist_goto_last(l);
		traverse = ecore_dlist_previous;
	}

	while ((op = traverse(ta->ops)))
		if (op->type == type)
			break;

	DRETURN_PTR(op, DLEVEL_STABLE);
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

	while ((op = ecore_dlist_remove_first(ta->ops))) {
		op->apply(ta, op);
		ecore_dlist_append(ta->applied, op);
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
	ecore_dlist_destroy(ta->ops);
	ta->ops = NULL;

	ecore_dlist_destroy(ta->applied);
	ta->applied = NULL;

	/*
	 * Finished with the etox, so now would be a good time to cleanup
	 * extra resources hanging around. FIXME: Should be called at some
	 * regular interval too, in case the text changes, but is never freed.
	 */
	etox_gc_collect();

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void ewl_text_reparent_cb(Ewl_Widget * w, void *ev_data, void *user_data)
{
	Ewl_Text *ta;

	DENTER_FUNCTION(DLEVEL_STABLE);

	ta = EWL_TEXT(w);
	if (ta->etox)
		evas_object_layer_set(ta->etox, ewl_widget_layer_sum_get(w));

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
		evas_object_layer_set(ta->etox, ewl_widget_layer_sum_get(w));
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
	if (!width)
		width = 1;
	if (!height)
		height = 1;

	/*
	 * Set the preferred size to the size of the etox and request that
	 * size for the widget.
	 */
	ewl_object_preferred_inner_size_set(EWL_OBJECT(ta), (int)(width),
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

static Ewl_Text_Op *
ewl_text_op_text_set_new(Ewl_Text *ta, char *text)
{
	Ewl_Text_Op *op;
	Ewl_Text_Op_Text *ops;

	DENTER_FUNCTION(DLEVEL_STABLE);

	op = NEW(Ewl_Text_Op_Text, 1);
	if (op) {
		ops = (Ewl_Text_Op_Text *)op;
		op->type = EWL_TEXT_OP_TYPE_TEXT_SET;
		op->apply = ewl_text_op_text_apply;
		op->free = ewl_text_op_text_free;
		ops->text = text ? strdup(text) : NULL;
	}

	DRETURN_PTR(op, DLEVEL_STABLE);
}

static Ewl_Text_Op *
ewl_text_op_text_append_new(Ewl_Text *ta, char *text)
{
	Ewl_Text_Op *op;
	Ewl_Text_Op_Text *ops;

	DENTER_FUNCTION(DLEVEL_STABLE);

	op = NEW(Ewl_Text_Op_Text, 1);
	if (op) {
		ops = (Ewl_Text_Op_Text *)op;
		op->type = EWL_TEXT_OP_TYPE_TEXT_APPEND;
		op->apply = ewl_text_op_text_apply;
		op->free = ewl_text_op_text_free;
		ops->text = strdup(text);
	}

	DRETURN_PTR(op, DLEVEL_STABLE);
}

static Ewl_Text_Op *
ewl_text_op_text_prepend_new(Ewl_Text *ta, char *text)
{
	Ewl_Text_Op *op;
	Ewl_Text_Op_Text *ops;

	DENTER_FUNCTION(DLEVEL_STABLE);

	op = NEW(Ewl_Text_Op_Text, 1);
	if (op) {
		ops = (Ewl_Text_Op_Text *)op;
		op->type = EWL_TEXT_OP_TYPE_TEXT_PREPEND;
		op->apply = ewl_text_op_text_apply;
		op->free = ewl_text_op_text_free;
		ops->text = strdup(text);
	}

	DRETURN_PTR(op, DLEVEL_STABLE);
}

static Ewl_Text_Op *
ewl_text_op_text_insert_new(Ewl_Text *ta, char *text, int index)
{
	Ewl_Text_Op *op;
	Ewl_Text_Op_Text *ops;

	DENTER_FUNCTION(DLEVEL_STABLE);

	op = NEW(Ewl_Text_Op_Text, 1);
	if (op) {
		ops = (Ewl_Text_Op_Text *)op;
		op->type = EWL_TEXT_OP_TYPE_TEXT_INSERT;
		op->apply = ewl_text_op_text_apply;
		op->free = ewl_text_op_text_free;
		ops->text = strdup(text);
		ops->index = index;
	}

	DRETURN_PTR(op, DLEVEL_STABLE);
}

static void
ewl_text_op_text_apply(Ewl_Text *ta, Ewl_Text_Op *op)
{
	Ewl_Text_Op_Text *opt = (Ewl_Text_Op_Text *)op;

	DENTER_FUNCTION(DLEVEL_STABLE);

	if (op->type == EWL_TEXT_OP_TYPE_TEXT_SET)
		etox_set_text(ta->etox, opt->text);
	else if (op->type == EWL_TEXT_OP_TYPE_TEXT_APPEND)
		etox_append_text(ta->etox, opt->text);
	else if (op->type == EWL_TEXT_OP_TYPE_TEXT_PREPEND)
		etox_prepend_text(ta->etox, opt->text);
	else if (op->type == EWL_TEXT_OP_TYPE_TEXT_INSERT)
		etox_insert_text(ta->etox, opt->text, opt->index);

	ewl_text_update_size(ta);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static void
ewl_text_op_text_free(void *op)
{
	Ewl_Text_Op_Text *opt = (Ewl_Text_Op_Text *)op;
	DENTER_FUNCTION(DLEVEL_STABLE);

	FREE(opt->text);
	FREE(opt);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static void
ewl_text_op_prune_list(Ewl_Text *ta, int rstart, int rend, int bstart, int bend)
{
	Ewl_Text_Op *op;

	ecore_dlist_goto_last(ta->ops);
	while ((op = ecore_dlist_current(ta->ops))) {
		/*
		 * Stop searching the list if we hit these events.
		 */
		if (op->type >= bstart && op->type <= bend)
			break;
		if (op->type >= rstart && op->type <= rend) {
			ecore_dlist_remove(ta->ops);
			if (op->free)
				op->free(op);
			else {
				FREE(op);
			}
		}
		ecore_dlist_previous(ta->ops);
	}
}
