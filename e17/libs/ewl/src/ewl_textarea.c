
#include <Ewl.h>

void            __ewl_textarea_realize(Ewl_Widget * w, void *ev_data,
				       void *user_data);
void            __ewl_textarea_configure(Ewl_Widget * w, void *ev_data,
					 void *user_data);
void            __ewl_textarea_reparent(Ewl_Widget * w, void *ev_data,
					void *user_data);
void            __ewl_textarea_update_size(Ewl_TextArea * ta);

/**
 * ewl_textarea_new - allocate a new text area widget
 * @text: the initial text of the textarea
 *
 * Returns a pointer to a newly allocated text area widget on success, NULL on
 * failure. Sets the text initially to @text if not NULL.
 */
Ewl_Widget     *ewl_textarea_new(char *text)
{
	Ewl_TextArea   *ta;

	DENTER_FUNCTION(DLEVEL_STABLE);

	ta = NEW(Ewl_TextArea, 1);
	ZERO(ta, Ewl_TextArea, 1);

	ewl_textarea_init(ta, text);

	DRETURN_PTR(EWL_WIDGET(ta), DLEVEL_STABLE);
}

/**
 * ewl_textarea_init - initialize the fields and callbacks of a text area
 * @ta: the text area to be initialized
 * @text: the text to be displayed initially in the text area
 *
 * Returns no value. Sets the internal fields and callbacks of a text area to
 * their defaults.
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
	ewl_callback_append(w, EWL_CALLBACK_CONFIGURE, __ewl_textarea_configure,
			    NULL);
	ewl_callback_append(w, EWL_CALLBACK_REPARENT, __ewl_textarea_reparent,
			    NULL);

	if (text)
		ewl_textarea_set_text(ta, text);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * ewl_textarea_set_text - set the text of a text area widget
 * @ta: the text area widget to set the text
 * @text: the text to set in the text area widget @ta
 *
 * Returns no value. Sets the text of the text area widget @ta to a copy of
 * the contents of @text.
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
 * ewl_textarea_get_text - retrieve the text of a text widget
 * @ta: the text widget to retrieve text contents
 *
 * Returns a pointer to a copy of the text in @ta on success, NULL on failure.
 */
char           *ewl_textarea_get_text(Ewl_TextArea * ta)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("ta", ta, NULL);

	DRETURN_PTR(strdup(ta->text), DLEVEL_STABLE);
}

/**
 * ewl_textarea_get_etox - retrieve the etox for performing text manipulation
 * @ta: the textarea to reveal it's etox
 *
 * Returns a pointer to the textarea's etox on success, NULL on failure.
 */
Etox           *ewl_textarea_get_etox(Ewl_TextArea * ta)
{
	DCHECK_PARAM_PTR_RET("ta", ta, NULL);

	DRETURN_PTR(ta->etox, DLEVEL_STABLE);
}

/**
 * ewl_textarea_set_context - put a context into the textarea for etox creation
 * @ta: the textarea to be assigned a context
 * @context: the context to be set for the text area
 *
 * Returns no value. Uses @context when creating/modifying the etox in @ta.
 */
void ewl_textarea_set_context(Ewl_TextArea * ta, Etox_Context * context)
{
	DENTER_FUNCTION(DLEVEL_STABLE);

	DCHECK_PARAM_PTR("ta", ta);

	ta->etox_context = context;

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * ewl_textarea_get_context - return the setup context for the text area
 * @ta: the textarea to retrieve it's assigned context
 *
 * Returns a pointer to the assigned context in @ta if one exists, otherwise
 * NULL.
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
	Ewl_Window     *win;
	Ewl_TextArea   *ta;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);

	ta = EWL_TEXTAREA(w);

	/*
	 * Find the window so we know which evas to draw onto.
	 */
	win = ewl_window_find_window_by_widget(w);

	/*
	 * Create the etox
	 */
	ta->etox = etox_new(win->evas);

	/*
	 * If the user setup their own context, use that.
	 */
	if (ta->etox_context) {
		etox_context_load(ta->etox, ta->etox_context);
		etox_context_free(ta->etox_context);
		ta->etox_context = NULL;
	} else {

		/*
		 * Get the default style and color based on the theme.
		 */
		style = ewl_theme_data_get_str(w,
				"/textarea/base/style");
		r = ewl_theme_data_get_int(w, "/textarea/base/r");
		g = ewl_theme_data_get_int(w, "/textarea/base/g");
		b = ewl_theme_data_get_int(w, "/textarea/base/b");
		a = ewl_theme_data_get_int(w, "/textarea/base/a");

		/*
		 * Set the default style
		 */
		if (style) {
			etox_context_set_style(ta->etox, style);
			FREE(style);
		}

		/*
		 * Set the default color for the text.
		 */
		etox_context_set_color(ta->etox, r, g, b, a);
	}

	/*
	 * Now set the text and display it.
	 */
	etox_set_text(ta->etox, ta->text);
	etox_show(ta->etox);

	/*
	 * Update the size of the textarea
	 */
	__ewl_textarea_update_size(ta);

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
		etox_move(ta->etox, CURRENT_X(w) + INSET_LEFT(w),
			  CURRENT_Y(w) + INSET_TOP(w));
		etox_set_layer(ta->etox, LAYER(w));
	}

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void __ewl_textarea_reparent(Ewl_Widget * w, void *ev_data, void *user_data)
{
	Ewl_TextArea   *ta;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);

	ta = EWL_TEXTAREA(w);

	/*
	 * Change the clipping of the etox to the new parent.
	 */
	if (ta->etox)
		etox_set_clip(ta->etox, w->fx_clip_box);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/*
 * Set the size of the text area to the size of the etox.
 */
void __ewl_textarea_update_size(Ewl_TextArea * ta)
{
	int             x, y, width, height;

	/*
	 * Adjust the properties of the widget to indicate the size of the text.
	 */
	etox_get_geometry(ta->etox, &x, &y, &width, &height);

	width += INSET_LEFT(ta) + INSET_RIGHT(ta);
	height += INSET_TOP(ta) + INSET_BOTTOM(ta);

	/*
	 * Set the preferred size to the size of the etox and request that
	 * size for the widget.
	 */
	ewl_object_set_preferred_size(EWL_OBJECT(ta), width, height);
	ewl_object_set_custom_size(EWL_OBJECT(ta), width, height);
}
