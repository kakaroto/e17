#include "ewler.h"

static Ewl_Widget *inspector;
static Ewl_Widget *i_tree;
static Ewler_Widget *i_widget = NULL; 

static Ecore_Hash *i_entries = NULL;

static EWL_CALLBACK_DEFN(_ewler_inspector_toggle);
static void _inspector_populate_elem( void *value, void *user_data );
static void inspector_populate( void );

void
ewler_inspector_init( void )
{
	char *headers[] = { "Attribute", "Value" };

	inspector = ewl_window_new();

	ewl_window_title_set( EWL_WINDOW(inspector), "Inspector" );
	ewl_object_maximum_w_set( EWL_OBJECT(inspector), 300 );
	ewl_object_preferred_inner_size_set( EWL_OBJECT(inspector), 300, 400 );
	ewl_callback_append( inspector, EWL_CALLBACK_DELETE_WINDOW,
											 _ewler_inspector_toggle, NULL );
	ewl_object_fill_policy_set( EWL_OBJECT(inspector), EWL_FLAG_FILL_NONE );

	i_tree = ewl_tree_new( 2 );
	ewl_tree_headers_set( EWL_TREE(i_tree), headers );
	ewl_object_fill_policy_set( EWL_OBJECT(i_tree), EWL_FLAG_FILL_ALL );
	ewl_container_child_append( EWL_CONTAINER(inspector), i_tree );
	ewl_widget_show( i_tree );

	i_entries = ecore_hash_new( ecore_direct_hash, ecore_direct_compare );

	ewl_widget_show( inspector );
}

void
ewler_inspector_toggle( void )
{
	if( VISIBLE(inspector) )
		ewl_widget_hide( inspector );
	else
		ewl_widget_show( inspector );
}

static EWL_CALLBACK_DEFN(_ewler_inspector_toggle)
{
	ewler_inspector_toggle();
}

void
ewler_inspector_widget_set( Ewler_Widget *w )
{
	if( i_widget != w ) {
		i_widget = w;

		if( i_widget )
			inspector_populate();
	}
}

static EWL_CALLBACK_DEFN(_entry_call_set)
{
	Ewler_Elem *elem;
	char *entry_val, *endptr;
	int int_val;

	elem = user_data;
	entry_val = ewl_entry_text_get( EWL_ENTRY(w) );

	switch( elem->spec->type ) {
		case EWLER_SPEC_ELEM_INT_TYPE:
			int_val = strtol(entry_val, &endptr, 0);
			if( !endptr ) {
				elem->v.int_val = int_val;
				if( elem->spec->set_func )
					elem->spec->set_func( i_widget->ewl_w, elem->v.int_val );
			} else {
				/* error dialog */
			}
			ewl_entry_text_set( EWL_ENTRY(w), ewler_widget_elem_to_s( elem ) );
			free( entry_val );
			break;
		case EWLER_SPEC_ELEM_STRING_TYPE:
			if( elem->v.string_val )
				free( elem->v.string_val );
			elem->v.string_val = entry_val;

			if( elem->spec->set_func )
				elem->spec->set_func( i_widget->ewl_w, elem->v.string_val );
			ewl_entry_text_set( EWL_ENTRY(w), ewler_widget_elem_to_s( elem ) );
			break;
		case EWLER_SPEC_ELEM_ENUM_TYPE:
			int_val = (int) ecore_hash_get( elem->spec->type_info.enum_type.map,
																			entry_val );

			if( ecore_hash_get( elem->spec->type_info.enum_type.map_rev,
													(void *) int_val ) &&
					elem->spec->set_func ) {
				elem->v.enum_val = int_val;
				elem->spec->set_func( i_widget->ewl_w, elem->v.enum_val );
			}
			ewl_entry_text_set( EWL_ENTRY(w), ewler_widget_elem_to_s( elem ) );
			break;
		default:
			free( entry_val );
	}
}

void
ewler_inspector_update( void )
{
	Ecore_Hash_Node *node;
	Ewl_Widget *w;
	Ewler_Elem *elem;

	ecore_hash_goto_first( i_entries );

	while( (node = ecore_hash_next( i_entries )) ) {
		w = node->key;
		elem = node->value;

		switch( elem->spec->type ) {
			case EWLER_SPEC_ELEM_INT_TYPE:
			case EWLER_SPEC_ELEM_STRING_TYPE:
			case EWLER_SPEC_ELEM_POINTER_TYPE:
				if( elem->changed )
					ewl_entry_text_set( EWL_ENTRY(w), ewler_widget_elem_to_s( elem ) );
				break;
		}
	}
}

static void
_inspector_populate_elem( void *value, void *user_data )
{
	Ecore_Hash_Node *node;
	Ewler_Elem *elem;
	Ewl_Widget *pair[2];
	Ewl_Widget *prow;
	Ewl_Widget *row;

	prow = user_data;

	node = value;
	elem = node->value;

	pair[0] = ewl_text_new( node->key );
	ewl_object_fill_policy_set( EWL_OBJECT(pair[0]), EWL_FLAG_FILL_VFILL );
	ewl_widget_show( pair[0] );

	switch( elem->spec->type ) {
		case EWLER_SPEC_ELEM_STRUCT_TYPE: pair[1] = NULL; break;
		case EWLER_SPEC_ELEM_ENUM_TYPE:
			pair[1] = ewl_combo_new( "" );
			break;
		default:
			pair[1] = ewl_entry_new( ewler_widget_elem_to_s( elem ) );
	}

	if( pair[1] ) {
		ecore_hash_set( i_entries, pair[1], elem );
		ewl_callback_append( pair[1], EWL_CALLBACK_VALUE_CHANGED,
												 _entry_call_set, elem );
		ewl_widget_show( pair[1] );
	}

	row = ewl_tree_row_add( EWL_TREE(i_tree), EWL_ROW(prow), pair );

	if( elem->spec->type == EWLER_SPEC_ELEM_STRUCT_TYPE )
		ecore_hash_for_each_node( elem->v.struct_val, _inspector_populate_elem,
															row );
}

static void
inspector_populate( void )
{
	ewl_container_reset( EWL_CONTAINER(i_tree) );
	ecore_hash_destroy( i_entries );
	i_entries = ecore_hash_new( ecore_direct_hash, ecore_direct_compare );

	ecore_hash_for_each_node( i_widget->attrs, _inspector_populate_elem, NULL );
}
