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
		{"Apply", 	"/stock/apply"},
		{/*Arrow*/"Down",	"/stock/arrow/down"},
		{/*Arrow*/"Left",	"/stock/arrow/left"},
		{/*Arrow*/"Right",	"/stock/arrow/right"},
		{/*Arrow*/"Up",	"/stock/arrow/up"},
		{"Cancel", 	"/stock/cancel"},
		{"FF", 		"/stock/ff"},
		{"Home", 	"/stock/home"},
		{"Ok", 		"/stock/ok"},
		{"Open", 	"/stock/open"},
		{"Pause", 	"/stock/pause"},
		{"Play", 	"/stock/play"},
		{"Quit", 	"/stock/quit"},
		{"Rewind", 	"/stock/rewind"},
		{"Save", 	"/stock/save"},
		{"Stop", 	"/stock/stop"}
	};

/**
 * @return Returns NULL on failure, a pointer to a new button on success
 * @brief Allocate and initialize a new button
 */
Ewl_Widget *
ewl_button_new(void)
{
	Ewl_Button *b;

	DENTER_FUNCTION(DLEVEL_STABLE);

	b = NEW(Ewl_Button, 1);
	if (!b)
		return NULL;

	if (!ewl_button_init(b)) {
		ewl_widget_destroy(EWL_WIDGET(b));
		b = NULL;
	}

	DRETURN_PTR(EWL_WIDGET(b), DLEVEL_STABLE);
}

/**
 * @param b: the button to initialize
 * @return Returns no value.
 * @brief Initialize a button to starting values
 *
 * Initializes a button to default values and callbacks.
 */
int
ewl_button_init(Ewl_Button *b)
{
	Ewl_Widget *w;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("b", b, 0);

	w = EWL_WIDGET(b);

	if (!ewl_box_init(EWL_BOX(b))) {
		DRETURN_INT(FALSE, DLEVEL_STABLE);
	}
	ewl_widget_inherit(w, EWL_BUTTON_TYPE);

	ewl_button_stock_type_set(b, EWL_STOCK_NONE);

	ewl_box_orientation_set(EWL_BOX(b), EWL_ORIENTATION_VERTICAL);
	ewl_container_callback_notify(EWL_CONTAINER(b), EWL_CALLBACK_FOCUS_IN);
	ewl_container_callback_notify(EWL_CONTAINER(b), EWL_CALLBACK_FOCUS_OUT);

	b->body = ewl_hbox_new();
	ewl_container_child_append(EWL_CONTAINER(b), b->body);
	ewl_widget_appearance_set(b->body, "body");
	ewl_object_alignment_set(EWL_OBJECT(b->body), EWL_FLAG_ALIGN_CENTER);
	ewl_object_fill_policy_set(EWL_OBJECT(b->body), EWL_FLAG_FILL_VFILL);
	ewl_widget_internal_set(b->body, TRUE);
	ewl_widget_show(b->body);

	ewl_container_redirect_set(EWL_CONTAINER(b), EWL_CONTAINER(b->body));
	ewl_widget_appearance_set(w, EWL_BUTTON_TYPE);

	ewl_widget_focusable_set(w, TRUE);

	ewl_callback_append(w, EWL_CALLBACK_KEY_DOWN, 
				ewl_button_cb_key_down, NULL);

	DRETURN_INT(TRUE, DLEVEL_STABLE);
}

/**
 * @param b: the buttons whose label will be changed
 * @param l: the new label for the button
 * @return Returns no value.
 * @brief Change the label of the specified button
 */
void
ewl_button_label_set(Ewl_Button *b, const char *l)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("b", b);
	DCHECK_TYPE("b", b, EWL_BUTTON_TYPE);

	if ((!l) && (b->label_object)) {
		ewl_widget_destroy(b->label_object);
		b->label_object = NULL;
	}
	else if (!b->label_object) {
		b->label_object = ewl_label_new();
		ewl_label_text_set(EWL_LABEL(b->label_object), l);
		ewl_object_fill_policy_set(EWL_OBJECT(b->label_object),
					   EWL_FLAG_FILL_VFILL);
		ewl_container_child_append(EWL_CONTAINER(b), b->label_object);
		ewl_widget_internal_set(b->label_object, TRUE);
		ewl_widget_show(b->label_object);
	}
	else
		ewl_label_text_set(EWL_LABEL(b->label_object), l);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param b: the buttons whose label will be returned
 * @return A newly allocated copy of the label on the button.
 * @brief Retrieve the label of the specified button
 */
const char *
ewl_button_label_get(Ewl_Button *b)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("b", b, NULL);
	DCHECK_TYPE_RET("b", b, EWL_BUTTON_TYPE, NULL);

	if (b->label_object)
		DRETURN_PTR(ewl_label_text_get(EWL_LABEL(b->label_object)), 
								DLEVEL_STABLE);

	DRETURN_PTR(NULL, DLEVEL_STABLE);
}

/**
 * @param b: The button to set the stock type on
 * @param stock: The Ewl_Stock_Type to set on the button
 * @return Returns no value.
 */
void
ewl_button_stock_type_set(Ewl_Button *b, Ewl_Stock_Type stock)
{
	char *data;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("b", b);
	DCHECK_TYPE("b", b, EWL_BUTTON_TYPE);

	if (stock == b->stock_type) {
		DRETURN(DLEVEL_STABLE);
	}
	b->stock_type = stock;

	/* we're done if it's none */
	if (b->stock_type == EWL_STOCK_NONE) {
		DRETURN(DLEVEL_STABLE);
	}

	ewl_button_label_set(b, ewl_stock_items[b->stock_type].label);

	/* check for an image key */
	data = ewl_theme_data_str_get(EWL_WIDGET(b), 
				ewl_stock_items[b->stock_type].image_key);
	if (data) {
		char *theme;

		theme = ewl_theme_path_get();
		ewl_button_image_set(b, theme, data);
		FREE(theme);
		FREE(data);
	}

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param b: The button to get the stock type from
 * @return Returns the Ewl_Stock_Type of the button
 */
Ewl_Stock_Type 
ewl_button_stock_type_get(Ewl_Button *b)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("b", b, EWL_STOCK_NONE);
	DCHECK_TYPE_RET("b", b, EWL_BUTTON_TYPE, EWL_STOCK_NONE);

	DRETURN_INT(b->stock_type, DLEVEL_STABLE);
}

/**
 * @param b: The button to set the image on
 * @param file: The file to use for the image
 * @param key: The edje key to use for the image (or NULL if not using edje)
 * @return Returns no value.
 */
void
ewl_button_image_set(Ewl_Button *b, const char *file, const char *key)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("b", b);
	DCHECK_PARAM_PTR("file", file);
	DCHECK_TYPE("b", b, EWL_BUTTON_TYPE);

	if (b->image_object)
		ewl_widget_destroy(b->image_object);

	b->image_object = ewl_image_new();
	ewl_container_child_prepend(EWL_CONTAINER(b), b->image_object);
	ewl_image_file_set(EWL_IMAGE(b->image_object), file, key);
	ewl_widget_internal_set(b->image_object, TRUE);
	ewl_widget_show(b->image_object);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param b: The button to the the image file from
 * @return Returns the image file used in the button or NULL on failure
 */
const char *
ewl_button_image_get(Ewl_Button *b)
{
	const char *file;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("b", b, NULL);
	DCHECK_TYPE_RET("b", b, EWL_BUTTON_TYPE, NULL);

	if (!b->image_object)
		file = NULL;
	else 
		file = ewl_image_file_path_get(EWL_IMAGE(b->image_object));

	DRETURN_PTR(file, DLEVEL_STABLE);
}

void
ewl_button_cb_key_down(Ewl_Widget *w, void *ev, void *data __UNUSED__)
{
	Ewl_Event_Key_Down *event;

	DENTER_FUNCTION(DLEVEL_STABLE);

	event = ev;
	if ((!strcmp(event->keyname, "Return")) 
			|| (!strcmp(event->keyname, "KP_Return"))
			|| (!strcmp(event->keyname, "Enter"))
			|| (!strcmp(event->keyname, "KP_Enter"))
			|| (!strcmp(event->keyname, " ")))
	{
		Ewl_Event_Mouse_Up e;

		e.x = 0;
		e.y = 0;
		e.button = 1;
		e.modifiers = 0;

		ewl_callback_call_with_event_data(w, EWL_CALLBACK_CLICKED, &e);
	}

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

