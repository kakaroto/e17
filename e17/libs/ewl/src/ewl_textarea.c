#include <Ewl.h>

void            __ewl_textarea_realize(Ewl_Widget * w, void *ev_data,
				       void *user_data);
void            __ewl_textarea_unrealize(Ewl_Widget * w, void *ev_data,
				       void *user_data);
void            __ewl_textarea_reparent(Ewl_Widget * w, void *ev_data,
				        void *user_data);
void            __ewl_textarea_configure(Ewl_Widget * w, void *ev_data,
					 void *user_data);
void            __ewl_textarea_update_size(Ewl_TextArea * ta);

/**
 * @param text: the initial text of the textarea
 * @return Returns a pointer to a new textarea on success, NULL on failure.
 * @brief Allocate a new text area widget
 *
 * Sets the text initially to @a text if not NULL.
 */
Ewl_Widget     *ewl_textarea_new(char *text)
{
	Ewl_TextArea   *ta;

	DENTER_FUNCTION(DLEVEL_STABLE);

	ta = NEW(Ewl_TextArea, 1);
	if (!ta)
		DRETURN_PTR(NULL, DLEVEL_STABLE);

	ewl_textarea_init(ta, text);

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
void ewl_textarea_init(Ewl_TextArea * ta, char *text)
{
	Ewl_Widget     *w;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("ta", ta);

	w = EWL_WIDGET(ta);

	ewl_widget_init(EWL_WIDGET(w), "textarea");

	ewl_callback_append(w, EWL_CALLBACK_REALIZE, __ewl_textarea_realize,
			    NULL);
	ewl_callback_append(w, EWL_CALLBACK_UNREALIZE, __ewl_textarea_unrealize,
			    NULL);
	ewl_callback_append(w, EWL_CALLBACK_REPARENT, __ewl_textarea_reparent,
			    NULL);
	ewl_callback_append(w, EWL_CALLBACK_CONFIGURE, __ewl_textarea_configure,
			    NULL);

	if (text)
		ewl_textarea_set_text(ta, text);

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
void ewl_textarea_set_text(Ewl_TextArea * ta, char *text)
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
	 * Update the etox and the sizing of the textarea widget.
	 */
	if (ta->etox) {

		etox_set_text(ta->etox, text);
		__ewl_textarea_update_size(ta);
	}

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param ta: the text widget to retrieve text contents
 * @return Returns a copy of the text in @a ta on success, NULL on failure.
 * @brief Retrieve the text of a text widget
 */
char           *ewl_textarea_get_text(Ewl_TextArea * ta)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("ta", ta, NULL);

	DRETURN_PTR(strdup(ta->text), DLEVEL_STABLE);
}

/**
 * @param ta: the textarea to reveal it's etox
 * @return Returns a pointer to the textarea's etox on success, NULL on failure.
 * @brief Retrieve the etox for performing text manipulation
 */
Evas_Object    *ewl_textarea_get_etox(Ewl_TextArea * ta)
{
	DCHECK_PARAM_PTR_RET("ta", ta, NULL);

	DRETURN_PTR(ta->etox, DLEVEL_STABLE);
}

/**
 * @param ta: the textarea to be assigned a context
 * @param context: the context to be set for the text area
 * @return Returns no value.
 * @brief Put a context into the textarea for etox creation
 *
 * Uses @a context when creating/modifying the etox in @a ta.
 */
void ewl_textarea_set_context(Ewl_TextArea * ta, Etox_Context * context)
{
	DENTER_FUNCTION(DLEVEL_STABLE);

	DCHECK_PARAM_PTR("ta", ta);

	ta->etox_context = context;

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param ta: the textarea to retrieve it's assigned context
 * @return Returns the assigned context in @a ta if one exists, otherwise NULL.
 * @brief Return the setup context for the text area
 */
Etox_Context   *ewl_textarea_get_context(Ewl_TextArea * ta)
{
	DENTER_FUNCTION(DLEVEL_STABLE);

	DRETURN_PTR(ta->etox_context, DLEVEL_STABLE);
}

void __ewl_textarea_realize(Ewl_Widget * w, void *ev_data, void *user_data)
{
	int             r, g, b, a;
	char           *style;
	Ewl_Embed      *emb;
	Ewl_TextArea   *ta;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);

	ta = EWL_TEXTAREA(w);

	/*
	 * Find the embed so we know which evas to draw onto.
	 */
	emb = ewl_embed_find_by_widget(w);

	/*
	 * Create the etox
	 */
	ta->etox = etox_new(emb->evas);

	/*
	 * If the user setup their own context, use that.
	 */
	if (ta->etox_context) {
		etox_context_load(ta->etox, ta->etox_context);
		etox_context_free(ta->etox_context);
		ta->etox_context = NULL;
	} else {
		char key[PATH_MAX];

		/*
		 * Get the default style and color based on the theme.
		 */
		snprintf(key, PATH_MAX, "%s/style", w->appearance);
		style = ewl_theme_data_get_str(w, key);

		snprintf(key, PATH_MAX, "%s/r", w->appearance);
		r = ewl_theme_data_get_int(w, key);

		snprintf(key, PATH_MAX, "%s/g", w->appearance);
		g = ewl_theme_data_get_int(w, key);

		snprintf(key, PATH_MAX, "%s/b", w->appearance);
		b = ewl_theme_data_get_int(w, key);

		snprintf(key, PATH_MAX, "%s/a", w->appearance);
		a = ewl_theme_data_get_int(w, key);

		/*
		 * Set the default style
		 */
		etox_context_set_style(etox_get_context(ta->etox), style);
		IF_FREE(style);

		/*
		 * Set the default color for the text.
		 */
		etox_context_set_color(etox_get_context(ta->etox), r, g, b, a);
	}

	evas_object_clip_set(ta->etox, w->fx_clip_box);

	/*
	 * Now set the text and display it.
	 */
	etox_set_text(ta->etox, ta->text);
	evas_object_show(ta->etox);

	/*
	 * Update the size of the textarea
	 */
	__ewl_textarea_update_size(ta);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void __ewl_textarea_unrealize(Ewl_Widget * w, void *ev_data, void *user_data)
{
	Ewl_TextArea   *ta;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);

	ta = EWL_TEXTAREA(w);

	ta->etox_context = etox_context_save(ta->etox);

	evas_object_clip_unset(ta->etox);
	evas_object_del(ta->etox);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void __ewl_textarea_reparent(Ewl_Widget * w, void *ev_data, void *user_data)
{
	Ewl_TextArea *ta;

	DENTER_FUNCTION(DLEVEL_STABLE);

	ta = EWL_TEXTAREA(w);
	if (ta->etox)
		evas_object_layer_set(ta->etox, ewl_widget_get_layer_sum(w));

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void __ewl_textarea_configure(Ewl_Widget * w, void *ev_data, void *user_data)
{
	Ewl_TextArea   *ta;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);

	ta = EWL_TEXTAREA(w);

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
void __ewl_textarea_update_size(Ewl_TextArea * ta)
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
