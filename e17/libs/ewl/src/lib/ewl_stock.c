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
} ewl_stock_items[] = {
		{"Apply", 	EWL_ICON_DIALOG_APPLY},
		{/*Arrow*/"Down", 	EWL_ICON_GO_DOWN},
		{/*Arrow*/"Left", 	EWL_ICON_GO_PREVIOUS},
		{/*Arrow*/"Right", 	EWL_ICON_GO_NEXT},
		{/*Arrow*/"Up",		EWL_ICON_GO_UP},
		{"Cancel", 		EWL_ICON_DIALOG_CANCEL},
		{"FF", 			EWL_ICON_MEDIA_SEEK_FORWARD},
		{"Home", 	EWL_ICON_GO_HOME},
		{"Ok", 		EWL_ICON_DIALOG_OK},
		{"Open", 	EWL_ICON_DOCUMENT_OPEN},
		{"Pause", 	EWL_ICON_MEDIA_PLAYBACK_PAUSE},
		{"Play", 	EWL_ICON_MEDIA_PLAYBACK_START},
		{"Quit", 	EWL_ICON_SYSTEM_LOG_OUT},
		{"Rewind", 	EWL_ICON_MEDIA_SEEK_BACKWARD},
		{"Save", 	EWL_ICON_DOCUMENT_SAVE},
		{"Stop", 	EWL_ICON_MEDIA_PLAYBACK_STOP}
	};

ewl_stock_init(Ewl_Stock *s)
{
	Ewl_Widget *w;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("s", s, NULL);

	w = EWL_WIDGET(s);

	if (!ewl_box_init(EWL_BOX(s))) {
		DRETURN_INT(FALSE, DLEVEL_STABLE);
	}
	ewl_widget_inherit(w, EWL_STOCK_TYPE);


	DRETURN_INT(TRUE, DLEVEL_STABLE);
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

	if (stock == s->stock_type) {
		DRETURN(DLEVEL_STABLE);
	}
	s->stock_type = stock;

	/* we're done if it's none */
	if (s->stock_type == EWL_STOCK_NONE) {
		DRETURN(DLEVEL_STABLE);
	}

	/* set the label */
	if (s->stock_funcs && s->stock_funcs->label_set)
		s->stock_funcs->label_set(s, 
					 ewl_stock_items[s->stock_type].label);

	/* set the image */
	if (s->stock_funcs && s->stock_funcs->image_set) {
		const char *data;
		
		/* check for an image key */
		data = ewl_icon_theme_icon_path_get(
				ewl_stock_items[s->stock_type].image_key,
				EWL_ICON_SIZE_MEDIUM);

		s->stock_funcs->image_set(s, data, 
				ewl_stock_items[s->stock_type].image_key);
	}

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


