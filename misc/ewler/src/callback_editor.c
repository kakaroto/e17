/**
 * callback_editor.c - callback editor
 */
#include <Ewl.h>

#include "callback_editor.h"

/**
 * list of all available callback strings
 */
static char *callback_strings[] =
{
	"EWL_CALLBACK_EXPOSE", 
	"EWL_CALLBACK_REALIZE", 
	"EWL_CALLBACK_UNREALIZE", 
	"EWL_CALLBACK_SHOW", 
	"EWL_CALLBACK_HIDE", 
	"EWL_CALLBACK_DESTROY", 
	"EWL_CALLBACK_DELETE_WINDOW", 
	"EWL_CALLBACK_CONFIGURE", 
	"EWL_CALLBACK_REPARENT", 
	"EWL_CALLBACK_KEY_DOWN", 
	"EWL_CALLBACK_KEY_UP", 
	"EWL_CALLBACK_MOUSE_DOWN", 
	"EWL_CALLBACK_MOUSE_UP", 
	"EWL_CALLBACK_MOUSE_MOVE", 
	"EWL_CALLBACK_MOUSE_WHEEL", 
	"EWL_CALLBACK_FOCUS_IN", 
	"EWL_CALLBACK_FOCUS_OUT", 
	"EWL_CALLBACK_SELECT", 
	"EWL_CALLBACK_DESELECT", 
	"EWL_CALLBACK_CLICKED", 
	"EWL_CALLBACK_DOUBLE_CLICKED", 
	"EWL_CALLBACK_HILITED", 
	"EWL_CALLBACK_VALUE_CHANGED", 
	"EWL_CALLBACK_STATE_CHANGED", 
	"EWL_CALLBACK_APPEARANCE_CHANGED", 
	"EWL_CALLBACK_WIDGET_ENABLE", 
	"EWL_CALLBACK_WIDGET_DISABLE", 
	"EWL_CALLBACK_PASTE", 
	NULL
};

/**
 * Allocate and initialize a new callback editor
 */
Ewl_Widget *
ewler_callback_editor_new(void)
{
	Ewler_Callback_Editor *ce;

	DENTER_FUNCTION(DLEVEL_STABLE);

	ce = NEW(Ewler_Callback_Editor, 1);
	if( !ce )
		DRETURN_PTR(NULL, DLEVEL_STABLE);

	ewler_callback_editor_init(ce);

	DRETURN_PTR(EWL_WIDGET(ce), DLEVEL_STABLE);
}

/**
 * Initialize a new callback editor
 */
void
ewler_callback_editor_init(Ewler_Callback_Editor *ce)
{
	char *headers[] = { "Callback", "Handler" }, **cb_ptr;
	Ewl_Widget *w;
	Ewl_Widget *hbox, *label;
	Ewl_Widget *menu_item, *separator;
	Ewl_Widget *button;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("ce", ce);

	w = EWL_WIDGET(ce);

	ewl_box_init(EWL_BOX(w), EWL_ORIENTATION_VERTICAL);
	ewl_object_fill_policy_set(EWL_OBJECT(w), EWL_FLAG_FILL_ALL);

	hbox = ewl_hbox_new();
	ewl_widget_internal_set(hbox, TRUE);
	ewl_object_insets_set(EWL_OBJECT(hbox), 5, 5, 0, 0);
	ewl_box_set_spacing(EWL_BOX(hbox), 5);
	ewl_object_fill_policy_set(EWL_OBJECT(hbox),
														 EWL_FLAG_FILL_HFILL | EWL_FLAG_FILL_HSHRINK);
	ewl_container_append_child(EWL_CONTAINER(ce), hbox);
	ewl_widget_show(hbox);

	label = ewl_text_new( "Callback" );
	ewl_widget_internal_set(label, TRUE);
	ewl_object_alignment_set(EWL_OBJECT(label), EWL_FLAG_ALIGN_CENTER);
	ewl_container_append_child(EWL_CONTAINER(hbox), label);
	ewl_widget_show(label);

	ce->cb_combo = ewl_combo_new( "" );
	ewl_widget_internal_set(ce->cb_combo, TRUE);
	ewl_container_append_child(EWL_CONTAINER(hbox), ce->cb_combo);
	ewl_object_fill_policy_set(EWL_OBJECT(ce->cb_combo), EWL_FLAG_FILL_HFILL);
	ewl_widget_show(ce->cb_combo);

	cb_ptr = callback_strings;

	while( *cb_ptr != NULL ) {
		menu_item = ewl_menu_item_new(NULL, *cb_ptr++);
		ewl_container_append_child(EWL_CONTAINER(ce->cb_combo), menu_item);
		ewl_widget_show(menu_item);
	}

	hbox = ewl_hbox_new();
	ewl_widget_internal_set(hbox, TRUE);
	ewl_object_insets_set(EWL_OBJECT(hbox), 5, 5, 5, 5);
	ewl_box_set_spacing(EWL_BOX(hbox), 5);
	ewl_object_fill_policy_set(EWL_OBJECT(hbox),
														 EWL_FLAG_FILL_HFILL | EWL_FLAG_FILL_HSHRINK);
	ewl_container_append_child(EWL_CONTAINER(ce), hbox);
	ewl_widget_show(hbox);
	
	label = ewl_text_new( "Handler" );
	ewl_widget_internal_set(label, TRUE);
	ewl_object_alignment_set(EWL_OBJECT(label), EWL_FLAG_ALIGN_CENTER);
	ewl_container_append_child(EWL_CONTAINER(hbox), label);
	ewl_widget_show(label);

	ce->handler_entry = ewl_entry_new( "" );
	ewl_widget_internal_set(ce->handler_entry, TRUE);
	ewl_container_append_child(EWL_CONTAINER(hbox), ce->handler_entry);
	ewl_widget_show(ce->handler_entry);

	hbox = ewl_hbox_new();
	ewl_widget_internal_set(hbox, TRUE);
	ewl_box_set_spacing(EWL_BOX(hbox), 5);
	ewl_object_alignment_set(EWL_OBJECT(hbox), EWL_FLAG_ALIGN_RIGHT);
	ewl_object_fill_policy_set(EWL_OBJECT(hbox),
														 EWL_FLAG_FILL_HFILL | EWL_FLAG_FILL_HSHRINK);
	ewl_container_append_child(EWL_CONTAINER(ce), hbox);
	ewl_widget_show(hbox);
	
	button = ewl_button_new( "Add" );
	ewl_widget_internal_set(ce->handler_entry, TRUE);
	ewl_object_alignment_set(EWL_OBJECT(button), EWL_FLAG_ALIGN_RIGHT);
	ewl_object_fill_policy_set(EWL_OBJECT(button), EWL_FLAG_FILL_NONE);
	ewl_container_append_child(EWL_CONTAINER(hbox), button);
	ewl_widget_show(button);

	separator = ewl_separator_new(EWL_ORIENTATION_HORIZONTAL);
	ewl_widget_internal_set(separator, TRUE);
	ewl_container_append_child(EWL_CONTAINER(ce), separator);
	ewl_widget_show(separator);

	ce->tree = ewl_tree_new(2);
	ewl_widget_internal_set(ce->tree, TRUE);
	ewl_widget_appearance_set(EWL_TREE(ce->tree)->scrollarea, "entry" );
	ewl_tree_set_headers(EWL_TREE(ce->tree), headers);
	ewl_object_fill_policy_set(EWL_OBJECT(ce->tree), EWL_FLAG_FILL_ALL );
	ewl_container_append_child(EWL_CONTAINER(ce), ce->tree);
	ewl_widget_show(ce->tree);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}
