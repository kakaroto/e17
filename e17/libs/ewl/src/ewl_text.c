#include <Ewl.h>

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

	ewl_entry_init(EWL_ENTRY(ta), text);
	ewl_entry_multiline_set(EWL_ENTRY(ta), TRUE);
	ewl_entry_editable_set(EWL_ENTRY(ta), FALSE);

	ewl_widget_appearance_set(w, "text");
	ewl_widget_inherit(w, "text");
	ewl_object_fill_policy_set(EWL_OBJECT(w), EWL_FLAG_FILL_NONE);

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

	ewl_entry_text_set(EWL_ENTRY(ta), text);

	if (text) {
		text = strdup(text);
		ewl_callback_call_with_event_data(EWL_WIDGET(ta), 
				EWL_CALLBACK_VALUE_CHANGED, text);
		IF_FREE(text);
	}

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
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("ta", ta);
	DCHECK_PARAM_PTR("text", text);

	ewl_entry_text_append(EWL_ENTRY(ta), text);

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
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("ta", ta);
	DCHECK_PARAM_PTR("text", text);

	ewl_entry_text_prepend(EWL_ENTRY(ta), text);

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
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("ta", ta);
	DCHECK_PARAM_PTR("text", text);

	ewl_entry_text_insert(EWL_ENTRY(ta), text, index);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param ta: the text widget to retrieve text contents
 * @return Returns a copy of the text in @a ta on success, NULL on failure.
 * @brief Retrieve the text of a text widget
 */
char *ewl_text_text_get(Ewl_Text * ta)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("ta", ta, NULL);

	DRETURN_PTR(ewl_entry_text_get(EWL_ENTRY(ta)), DLEVEL_STABLE);
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

	DRETURN_INT(ewl_entry_length_get(EWL_ENTRY(ta)), DLEVEL_STABLE);
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
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("ta", ta);

	ewl_entry_color_set(EWL_ENTRY(ta), r, g, b, a);

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
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("ta", ta);

	ewl_entry_font_set(EWL_ENTRY(ta), font, size);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param ta: the text widget to retrieve the current font
 * @brief Retrieve the name of the currently used font.
 * @return Returns a copied string containing the name of the current font.
 */
char *ewl_text_font_get(Ewl_Text *ta)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("ta", ta, NULL);

	DRETURN_PTR(ewl_entry_font_get(EWL_ENTRY(ta)), DLEVEL_STABLE);
}

/**
 * @param ta: the text widget to change style
 * @param style: the name of the style
 * @brief Changes the currently applied style of the text to specified values
 * @return Returns no value.
 */
void ewl_text_style_set(Ewl_Text *ta, char *style)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("ta", ta);

	ewl_entry_style_set(EWL_ENTRY(ta), style);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param ta: the text widget to get the current style
 * @brief Retrieves the currently used text style from a text widget.
 * @return Returns the currently used text style.
 */
char *ewl_text_style_get(Ewl_Text *ta)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("ta", ta, NULL);

	DRETURN_PTR(ewl_entry_style_get(EWL_ENTRY(ta)), DLEVEL_STABLE);
}

/**
 * @param ta: the text widget to change alignment
 * @param align: the new alignment of the text widget
 * @brief Changes the currently applied alignment of the text to specified value
 * @return Returns no value.
 */
void ewl_text_align_set(Ewl_Text *ta, unsigned int align)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("ta", ta);

	ewl_entry_align_set(EWL_ENTRY(ta), align);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param ta: the text widget to get the current alignment
 * @brief Retrieves the currently used text alignment from a text widget.
 * @return Returns the currently used text alignment.
 */
unsigned int ewl_text_align_get(Ewl_Text *ta)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("ta", ta, 0);

	DRETURN_INT(ewl_entry_align_get(EWL_ENTRY(ta)), DLEVEL_STABLE);
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
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("ta", ta, 0);

	DRETURN_INT(ewl_entry_coord_index_map(EWL_ENTRY(ta), x, y), DLEVEL_STABLE);
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
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("ta", ta);

	ewl_entry_index_geometry_map(EWL_ENTRY(ta), index, x, y, w, h);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

