#include <Ewl.h>
#include "ewl_debug.h"
#include "ewl_macros.h"
#include "ewl_private.h"

/*
 * this array needs to have it's items in the same order as they
 * appear in the Ewl_Stock_Type enum
 */
struct 
{
	char *label;
	char *image_key;
	char *tooltip;
} ewl_stock_items[] = {
		{"Apply", EWL_ICON_DIALOG_APPLY, "Apply"},
		{/*Arrow*/"Down", EWL_ICON_GO_DOWN, "Down"},
		{/*Arrow*/"Left", EWL_ICON_GO_PREVIOUS, "Previous"},
		{/*Arrow*/"Right", EWL_ICON_GO_NEXT, "Next"},
		{/*Arrow*/"Up",	EWL_ICON_GO_UP, "Up"},
		{"Cancel", EWL_ICON_DIALOG_CANCEL, "Cancel"},
		{"FF", EWL_ICON_MEDIA_SEEK_FORWARD, "Fast Forward"},
		{"Home", EWL_ICON_GO_HOME, "Home"},
		{"Ok", EWL_ICON_DIALOG_OK, "OK"},
		{"Open", EWL_ICON_DOCUMENT_OPEN, "Open"},
		{"Pause", EWL_ICON_MEDIA_PLAYBACK_PAUSE, "Pause"},
		{"Play", EWL_ICON_MEDIA_PLAYBACK_START, "Play"},
		{"Quit", EWL_ICON_SYSTEM_LOG_OUT, "Quit"},
		{"Rewind", EWL_ICON_MEDIA_SEEK_BACKWARD, "Rewind"},
		{"Save", EWL_ICON_DOCUMENT_SAVE, "Save"},
		{"Stop", EWL_ICON_MEDIA_PLAYBACK_STOP, "Stop"}
	};

/**
 * @param s: the stock widget to initialize
 * @return Returns TRUE on success, FALSE on failure.
 * @brief Initialize the stock widget and inherited fields
 *
 * Clears the contents of the stock widget and stores the
 * default values.
 */
int
ewl_stock_init(Ewl_Stock *s)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("s", s, FALSE);

	if (!ewl_box_init(EWL_BOX(s)))
		DRETURN_INT(FALSE, DLEVEL_STABLE);

	ewl_widget_inherit(EWL_WIDGET(s), EWL_STOCK_TYPE);

	DRETURN_INT(TRUE, DLEVEL_STABLE);
}

/**
 * @internal
 * @param s: The stock to work with
 * @param funcs: The stock functions to set.
 * @return Returns no value
 * @brief Sets the given stock functions onto the stock widget
 */
void
ewl_stock_functions_set(Ewl_Stock *s, Ewl_Stock_Funcs *funcs)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("s", s);
	DCHECK_PARAM_PTR("funcs", funcs);
	DCHECK_TYPE("s", s, EWL_STOCK_TYPE);

	s->stock_funcs = funcs;

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param s: The stock to set the stock type on
 * @param stock: The Ewl_Stock_Type to set on the stock widget
 * @return Returns no value.
 * @brief Set the type of the stock to use
 */
void
ewl_stock_type_set(Ewl_Stock *s, Ewl_Stock_Type stock)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("s", s);
	DCHECK_TYPE("s", s, EWL_STOCK_TYPE);

	if (stock == s->stock_type)
		DRETURN(DLEVEL_STABLE);

	s->stock_type = stock;

	/* we're done if it's none */
	if (s->stock_type == EWL_STOCK_NONE)
		DRETURN(DLEVEL_STABLE);

	/* Can't do anything without the stock funcs */
	if (!s->stock_funcs)
		DRETURN(DLEVEL_STABLE);

	/* set the label */
	if (s->stock_funcs->label_set)
		s->stock_funcs->label_set(s, 
			 ewl_stock_items[s->stock_type].label);

	/* set the image */
	if (s->stock_funcs->image_set) {
		const char *data, *key;
		
		/* check for an image key */
		data = ewl_icon_theme_icon_path_get(
				ewl_stock_items[s->stock_type].image_key,
				EWL_ICON_SIZE_MEDIUM);
		key = ewl_icon_theme_icon_key_get(data, 
				ewl_stock_items[s->stock_type].image_key);

		s->stock_funcs->image_set(s, data, key);
	}

	/* set the tooltip */
	if (s->stock_funcs->tooltip_set)
		s->stock_funcs->tooltip_set(s,
			ewl_stock_items[s->stock_type].tooltip);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param s: The stock widget to get the stock type from
 * @return Returns the Ewl_Stock_Type of the stock widget
 * @brief Get the stock type in use
 */
Ewl_Stock_Type 
ewl_stock_type_get(Ewl_Stock *s)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("s", s, EWL_STOCK_NONE);
	DCHECK_TYPE_RET("s", s, EWL_STOCK_TYPE, EWL_STOCK_NONE);

	DRETURN_INT(s->stock_type, DLEVEL_STABLE);
}


