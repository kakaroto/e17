/**
 * callback_editor.c - callback editor
 */
#include <Ewl.h>

#include "callback.h"
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

static int callback_values[] =
{
	EWL_CALLBACK_EXPOSE, 
	EWL_CALLBACK_REALIZE, 
	EWL_CALLBACK_UNREALIZE, 
	EWL_CALLBACK_SHOW, 
	EWL_CALLBACK_HIDE, 
	EWL_CALLBACK_DESTROY, 
	EWL_CALLBACK_DELETE_WINDOW, 
	EWL_CALLBACK_CONFIGURE, 
	EWL_CALLBACK_REPARENT, 
	EWL_CALLBACK_KEY_DOWN, 
	EWL_CALLBACK_KEY_UP, 
	EWL_CALLBACK_MOUSE_DOWN, 
	EWL_CALLBACK_MOUSE_UP, 
	EWL_CALLBACK_MOUSE_MOVE, 
	EWL_CALLBACK_MOUSE_WHEEL, 
	EWL_CALLBACK_FOCUS_IN, 
	EWL_CALLBACK_FOCUS_OUT, 
	EWL_CALLBACK_SELECT, 
	EWL_CALLBACK_DESELECT, 
	EWL_CALLBACK_CLICKED, 
	EWL_CALLBACK_DOUBLE_CLICKED, 
	EWL_CALLBACK_HILITED, 
	EWL_CALLBACK_VALUE_CHANGED, 
	EWL_CALLBACK_STATE_CHANGED, 
	EWL_CALLBACK_APPEARANCE_CHANGED, 
	EWL_CALLBACK_WIDGET_ENABLE, 
	EWL_CALLBACK_WIDGET_DISABLE, 
	EWL_CALLBACK_PASTE, 
	-1
};

static void __add_cb( Ewl_Widget *w, void *event, void *user_data );
static void __modify_cb( Ewl_Widget *w, void *event, void *user_data );
static void __delete_cb( Ewl_Widget *w, void *event, void *user_data );
static void __clear_cb( Ewl_Widget *w, void *event, void *user_data );
static void __cb_changed( Ewl_Widget *w, void *event, void *user_data );
static void __select_row( Ewl_Widget *w, void *event, void *user_data );

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

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("ce", ce);

	w = EWL_WIDGET(ce);

	ewl_box_init(EWL_BOX(w), EWL_ORIENTATION_VERTICAL);
	ewl_object_fill_policy_set(EWL_OBJECT(w), EWL_FLAG_FILL_ALL);

	hbox = ewl_hbox_new();
	ewl_widget_internal_set(hbox, TRUE);
	ewl_object_insets_set(EWL_OBJECT(hbox), 5, 5, 0, 0);
	ewl_box_spacing_set(EWL_BOX(hbox), 5);
	ewl_object_fill_policy_set(EWL_OBJECT(hbox),
														 EWL_FLAG_FILL_HFILL | EWL_FLAG_FILL_HSHRINK);
	ewl_container_child_append(EWL_CONTAINER(ce), hbox);
	ewl_widget_show(hbox);

	label = ewl_text_new( "Callback" );
	ewl_widget_internal_set(label, TRUE);
	ewl_object_alignment_set(EWL_OBJECT(label), EWL_FLAG_ALIGN_CENTER);
	ewl_container_child_append(EWL_CONTAINER(hbox), label);
	ewl_widget_show(label);

	ce->cb_combo = ewl_combo_new( "                 " );
	ewl_widget_internal_set(ce->cb_combo, TRUE);
	ewl_container_child_append(EWL_CONTAINER(hbox), ce->cb_combo);
	ewl_object_fill_policy_set(EWL_OBJECT(ce->cb_combo), EWL_FLAG_FILL_HFILL);
	ewl_callback_append( ce->cb_combo, EWL_CALLBACK_VALUE_CHANGED,
											 __cb_changed, ce );
	ewl_widget_show(ce->cb_combo);

	cb_ptr = callback_strings;

	while( *cb_ptr != NULL ) {
		menu_item = ewl_menu_item_new(NULL, *cb_ptr++);
		ewl_container_child_append(EWL_CONTAINER(ce->cb_combo), menu_item);
		ewl_widget_show(menu_item);
	}

	hbox = ewl_hbox_new();
	ewl_widget_internal_set(hbox, TRUE);
	ewl_object_insets_set(EWL_OBJECT(hbox), 5, 5, 5, 5);
	ewl_box_spacing_set(EWL_BOX(hbox), 5);
	ewl_object_fill_policy_set(EWL_OBJECT(hbox),
														 EWL_FLAG_FILL_HFILL | EWL_FLAG_FILL_HSHRINK);
	ewl_container_child_append(EWL_CONTAINER(ce), hbox);
	ewl_widget_show(hbox);
	
	label = ewl_text_new( "Handler" );
	ewl_widget_internal_set(label, TRUE);
	ewl_object_alignment_set(EWL_OBJECT(label), EWL_FLAG_ALIGN_CENTER);
	ewl_container_child_append(EWL_CONTAINER(hbox), label);
	ewl_widget_show(label);

	ce->handler_entry = ewl_entry_new( "" );
	ewl_widget_internal_set(ce->handler_entry, TRUE);
	ewl_container_child_append(EWL_CONTAINER(hbox), ce->handler_entry);
	ewl_callback_append( ce->handler_entry, EWL_CALLBACK_KEY_DOWN,
											 __cb_changed, ce );
	ewl_widget_show(ce->handler_entry);

	hbox = ewl_hbox_new();
	ewl_widget_internal_set(hbox, TRUE);
	ewl_box_spacing_set(EWL_BOX(hbox), 5);
	ewl_object_alignment_set(EWL_OBJECT(hbox), EWL_FLAG_ALIGN_RIGHT);
	ewl_object_fill_policy_set(EWL_OBJECT(hbox),
														 EWL_FLAG_FILL_HFILL | EWL_FLAG_FILL_HSHRINK);
	ewl_container_child_append(EWL_CONTAINER(ce), hbox);
	ewl_widget_show(hbox);
	
	ce->add_button = ewl_button_new( "Add" );
	ewl_widget_internal_set(ce->add_button, TRUE);
	ewl_object_alignment_set(EWL_OBJECT(ce->add_button), EWL_FLAG_ALIGN_RIGHT);
	ewl_object_fill_policy_set(EWL_OBJECT(ce->add_button), EWL_FLAG_FILL_NONE);
	ewl_container_child_append(EWL_CONTAINER(hbox), ce->add_button);
	ewl_callback_append( ce->add_button, EWL_CALLBACK_CLICKED,
											 __add_cb, ce );
	ewl_widget_show(ce->add_button);

	ce->modify_button = ewl_button_new( "Modify" );
	ewl_widget_internal_set(ce->modify_button, TRUE);
	ewl_object_alignment_set(EWL_OBJECT(ce->modify_button), EWL_FLAG_ALIGN_RIGHT);
	ewl_object_fill_policy_set(EWL_OBJECT(ce->modify_button), EWL_FLAG_FILL_NONE);
	ewl_container_child_append(EWL_CONTAINER(hbox), ce->modify_button);
	ewl_callback_append( ce->modify_button, EWL_CALLBACK_CLICKED,
											 __modify_cb, ce );
	ewl_widget_show(ce->modify_button);

	ce->delete_button = ewl_button_new( "Delete" );
	ewl_widget_internal_set(ce->delete_button, TRUE);
	ewl_object_alignment_set(EWL_OBJECT(ce->delete_button), EWL_FLAG_ALIGN_RIGHT);
	ewl_object_fill_policy_set(EWL_OBJECT(ce->delete_button), EWL_FLAG_FILL_NONE);
	ewl_container_child_append(EWL_CONTAINER(hbox), ce->delete_button);
	ewl_callback_append( ce->delete_button, EWL_CALLBACK_CLICKED,
											 __delete_cb, ce );
	ewl_widget_show(ce->delete_button);

	ce->clear_button = ewl_button_new( "Clear" );
	ewl_widget_internal_set(ce->clear_button, TRUE);
	ewl_object_alignment_set(EWL_OBJECT(ce->clear_button), EWL_FLAG_ALIGN_RIGHT);
	ewl_object_fill_policy_set(EWL_OBJECT(ce->clear_button), EWL_FLAG_FILL_NONE);
	ewl_container_child_append(EWL_CONTAINER(hbox), ce->clear_button);
	ewl_callback_append( ce->clear_button, EWL_CALLBACK_CLICKED,
											 __clear_cb, ce );
	ewl_widget_show(ce->clear_button);

	separator = ewl_separator_new(EWL_ORIENTATION_HORIZONTAL);
	ewl_widget_internal_set(separator, TRUE);
	ewl_container_child_append(EWL_CONTAINER(ce), separator);
	ewl_widget_show(separator);

	ce->tree = ewl_tree_new(2);
	ewl_widget_internal_set(ce->tree, TRUE);
	ewl_widget_appearance_set(EWL_TREE(ce->tree)->scrollarea, "entry" );
	ewl_tree_headers_set(EWL_TREE(ce->tree), headers);
	ewl_object_fill_policy_set(EWL_OBJECT(ce->tree), EWL_FLAG_FILL_ALL );
	ewl_container_child_append(EWL_CONTAINER(ce), ce->tree);
	ewl_widget_show(ce->tree);

	ewl_widget_disable(w);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

int
ewler_callback_value( char *callback )
{
	int i;

	for( i=0;callback_strings[i];i++ )
		if( !strcmp( callback_strings[i], callback ) )
			return callback_values[i];

	return -1;
}

char *
ewler_callback_string( int callback )
{
	int i;

	for( i=0;callback_strings[i];i++ )
		if( callback_values[i] == callback )
			return callback_strings[i];

	return NULL;
}

void
ewler_callback_editor_update( Ewler_Callback_Editor *ce )
{
	Ecore_List *cb_list;
	Ewler_Callback *cb;

	ewl_container_reset( EWL_CONTAINER(ce->tree) );

	cb_list = ewler_callbacks_get( ce->target );

	ecore_list_goto_first( cb_list );
	while( (cb = ecore_list_next( cb_list )) ) {
		Ewl_Widget *row;
		char *row_text[2];

		row_text[0] = cb->handler;
		row_text[1] = ewler_callback_string( cb->callback );

		row = ewl_tree_text_row_add( EWL_TREE(ce->tree), NULL, row_text );
		ewl_callback_append( row, EWL_CALLBACK_SELECT,
												 __select_row, ce );
		ewl_widget_show( row );
	}
}

void
ewler_callback_editor_target_set( Ewler_Callback_Editor *ce, Ewl_Widget *w )
{
	if( w )
		ewl_widget_enable( EWL_WIDGET(ce) );

	if( ce->target != w )
		ewler_callback_editor_update( ce );

	ce->target = w;
}

static void
__add_cb( Ewl_Widget *w, void *event, void *user_data )
{
	Ewler_Callback_Editor *ce = user_data;
	char *handler, *cb_name;
	int callback = -1;

	handler = ewl_entry_text_get( EWL_ENTRY(ce->handler_entry) );

	cb_name = ewl_combo_get_selected( EWL_COMBO(ce->cb_combo) );

	callback = ewler_callback_value( cb_name );
	if( callback < 0 || strlen( handler ) == 0 )
		return;

	ewler_callback_add( ce->target, handler, callback );

	FREE( handler );
	FREE( cb_name );

	ewl_entry_text_set( EWL_ENTRY(ce->handler_entry), "" );
	ewl_combo_set_selected( EWL_COMBO(ce->cb_combo), "             " );

	ewler_callback_editor_update( ce );
}

static void
__modify_cb( Ewl_Widget *w, void *event, void *user_data )
{
  Ewler_Callback_Editor *ce = user_data;
	char *handler, *cb_name;
	int callback = -1;

	handler = ewl_entry_text_get( EWL_ENTRY(ce->handler_entry) );
	cb_name = ewl_combo_get_selected( EWL_COMBO(ce->cb_combo) );

	callback = ewler_callback_value( cb_name );

	ewler_callback_replace( ce->target, ce->active, handler, callback );
	if( callback < 0 || strlen( handler ) == 0 )
		return;

	FREE( handler );
	FREE( cb_name );

	ewl_entry_text_set( EWL_ENTRY(ce->handler_entry), "" );
	ewl_combo_set_selected( EWL_COMBO(ce->cb_combo), "             " );

	ewl_widget_enable( ce->add_button );
	ewl_widget_disable( ce->modify_button );
	ewl_widget_disable( ce->delete_button );
	ewl_widget_disable( ce->clear_button );

	ewler_callback_editor_update( ce );
}

static void
__delete_cb( Ewl_Widget *w, void *event, void *user_data )
{
  Ewler_Callback_Editor *ce = user_data;
	char *handler, *cb_name;
	int callback = -1;

	handler = ewl_entry_text_get( EWL_ENTRY(ce->handler_entry) );
	cb_name = ewl_combo_get_selected( EWL_COMBO(ce->cb_combo) );

	callback = ewler_callback_value( cb_name );

	ewler_callback_del( ce->target, handler, callback );
	if( callback < 0 || strlen( handler ) == 0 )
		return;

	FREE( handler );
	FREE( cb_name );

	ewl_entry_text_set( EWL_ENTRY(ce->handler_entry), "" );
	ewl_combo_set_selected( EWL_COMBO(ce->cb_combo), "             " );

	ewl_widget_enable( ce->add_button );
	ewl_widget_disable( ce->modify_button );
	ewl_widget_disable( ce->delete_button );
	ewl_widget_disable( ce->clear_button );

	ewler_callback_editor_update( ce );
}

static void
__clear_cb( Ewl_Widget *w, void *event, void *user_data )
{
  Ewler_Callback_Editor *ce = user_data;

	ewl_entry_text_set( EWL_ENTRY(ce->handler_entry), "" );
	ewl_combo_set_selected( EWL_COMBO(ce->cb_combo), "             " );

	ewl_widget_disable( ce->add_button );
	ewl_widget_disable( ce->modify_button );
	ewl_widget_disable( ce->delete_button );
	ewl_widget_disable( ce->clear_button );

	ewler_callback_editor_update( ce );
}

static void
__select_row( Ewl_Widget *w, void *event, void *user_data )
{
	Ewler_Callback_Editor *ce = user_data;
	Ewl_Widget *handler_label, *callback_label;
	char *handler, *callback;
	int cb_id;

  handler_label = ewl_row_column_get( EWL_ROW(w), 0 );
	callback_label = ewl_row_column_get( EWL_ROW(w), 1 );

	handler = ewl_text_text_get( EWL_TEXT(handler_label) );
	callback = ewl_text_text_get( EWL_TEXT(callback_label) );
	cb_id = ewler_callback_value( callback );

	ewl_entry_text_set( EWL_ENTRY(ce->handler_entry), handler );
	ewl_combo_set_selected( EWL_COMBO(ce->cb_combo), callback );

	ce->active = ewler_callback_get( ce->target, handler, cb_id );

	FREE( handler );
	FREE( callback );

	ewl_widget_disable( ce->add_button );
	ewl_widget_enable( ce->modify_button );
	ewl_widget_enable( ce->delete_button );
	ewl_widget_enable( ce->clear_button );
}

static void
__cb_changed( Ewl_Widget *w, void *event, void *user_data )
{
	Ewler_Callback_Editor *ce = user_data;

	ewl_widget_enable( ce->add_button );
	ewl_widget_disable( ce->modify_button );
	ewl_widget_disable( ce->delete_button );
	ewl_widget_enable( ce->clear_button );
}
