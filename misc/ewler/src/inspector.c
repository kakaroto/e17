/**
 * inspector.c - object inspector for form widgets
 */
#include <Ewl.h>

#include "form.h"
#include "inspector.h"
#include "widgets.h"
#include "selected.h"

static Ewl_Widget *inspector_win;
static Ewler_Form *active_form = NULL;
static Ewl_Widget *active_widget = NULL;
static Ewl_Widget *inspector_tree;

static int visible = 0;

/* temporary statics to get over data structure ineptness */
static Ewl_Widget *prow;

void
__hide_inspector_cb( Ewl_Widget *w, void *ev_data, void *user_data )
{
	ewl_widget_hide( w );
	visible = 0;
}

void
ewler_inspector_init( void )
{
	char *headers[] = { "Property", "Value" };

	inspector_win = ewl_window_new();

	ewl_window_set_title( EWL_WINDOW(inspector_win), "Inspector" );
	ewl_callback_append( inspector_win, EWL_CALLBACK_DELETE_WINDOW,
											 __hide_inspector_cb, NULL );
	ewl_object_set_preferred_size( EWL_OBJECT(inspector_win), 200, 400 );
	ewl_object_set_minimum_size( EWL_OBJECT(inspector_win), 200, 400 );
	ewl_object_set_fill_policy( EWL_OBJECT(inspector_win), EWL_FLAG_FILL_SHRINK );

	inspector_tree = ewl_tree_new( 2 );
	ewl_container_append_child( EWL_CONTAINER(inspector_win), inspector_tree );
	ewl_tree_set_headers( EWL_TREE(inspector_tree), headers );
	ewl_widget_show( inspector_tree );
}

int
inspector_visible( void )
{
	return visible;
}

void
inspector_hide( void )
{
	ewl_widget_hide( inspector_win );
	visible = 0;
}

void
inspector_show( void )
{
	ewl_widget_show( inspector_win );
	visible = 1;
}

/* PRAISE ME! */
static void
call_struct_set( Ewl_Widget *w, Widget_Data_Elem *child, int index, int value )
{
	Ecore_List *info;
	Widget_Type_Elem *type = child->type->w.parent;
	Widget_Data_Elem *parent;
	int args[4], len;

	info = widget_get_info( w );

	ecore_list_goto_first( info );

	while( (parent = ecore_list_next( info )) )
		if( parent->type == type )
			break;

	if( !parent )
		return;

	len = ecore_list_nodes( parent->w_struct.members );

	ecore_list_goto_first( parent->w_struct.members );

	while( (child = ecore_list_next( parent->w_struct.members )) ) {
		if( child->type->w.index != index ) {
			switch( child->type->w.w_type ) {
				case WIDGET_INTEGER_TYPE:
					args[child->type->w.index] = child->w_int.value;
					break;
				case WIDGET_STRING_TYPE:
					args[child->type->w.index] = (int) child->w_int.value;
					break;
				case WIDGET_STRUCT_TYPE:
				case WIDGET_POINTER_TYPE:
					printf( "unsupported set calls\n" );
					return;
			}
		}
	}

	args[index] = value;

	/* maybe put a ridiculous switch on the number of args, but for now just call
	 * it with 4... the stack frame should be fine */
	parent->type->w.set( EWL_OBJECT(w),
											 args[0], args[1], args[2], args[3] );
}

static void
__inspector_entry_changed( Ewl_Widget *w, void *ev_data, void *user_data )
{
	Widget_Data_Elem *data = user_data;
	Ewl_Widget *widget = ewl_widget_get_data( w, "widget" );
	char *text, *endptr;
	int int_val;

	text = ewl_entry_get_text( EWL_ENTRY(w) );

	switch( data->type->w.w_type ) {
		case WIDGET_INTEGER_TYPE:
			int_val = strtol( text, &endptr, 0 );
			if( *endptr ) {
				printf( "invalid integer value %s\n", text );
			} else {
				if( data->type->w.set )
					data->type->w.set( EWL_OBJECT(widget), data->w_int.value );
				else if( data->type->w.parent && data->type->w.parent->w.set )
					call_struct_set( widget, data, data->type->w.index, int_val );
				data->w_int.value = int_val;
			}
			break;
		case WIDGET_STRING_TYPE:
			/* check string for validity */
			if( data->type->w.set )
				data->type->w.set( EWL_OBJECT(widget), text );
			else if( data->type->w.parent && data->type->w.parent->w.set )
				call_struct_set( widget, data, data->type->w.index, (int) text );

			if( !widget_get_error() ) {
				if( data->w_str.value ) {
					FREE(data->w_str.value);
				}
				data->w_str.value = strdup( text );
			}
			break;
	}

	widget_changed( widget );
	inspector_update();
}

static void
__inspector_combo_changed( Ewl_Widget *w, void *ev_data, void *user_data )
{
	Widget_Data_Elem *data = user_data;
	Ewl_Widget *widget = ewl_widget_get_data( w, "widget" );
	char *text;
	int value;

	text = ewl_combo_get_selected( EWL_COMBO(w) );

	if( data->type->w.w_type != WIDGET_ENUM_TYPE )
		return;

	value = (int) ecore_hash_get( data->type->w_enum.map, text );

	if( data->type->w.set )
		data->type->w.set( EWL_OBJECT(widget), value );

	ewl_object_set_fill_policy( EWL_OBJECT(widget->parent),
															ewl_object_get_fill_policy(EWL_OBJECT(widget)) );
	ewl_object_set_alignment( EWL_OBJECT(widget->parent),
														ewl_object_get_alignment(EWL_OBJECT(widget)) );
	widget_changed( widget );
}

static Ewl_Widget *t_combo;

static void
__populate_combo_cb( void *val )
{
	Ecore_Hash_Node *node = val;
	Ewl_Widget *text;

	text = ewl_menu_item_new( NULL, node->key );
	ewl_container_append_child( EWL_CONTAINER(t_combo), text );
	ewl_widget_show( text );
}

static void
__populate_tree( void *val )
{
	Ewl_Widget *trow, *row, *row_elems[3] = { NULL, NULL, NULL };
	Widget_Data_Elem *data = val;
	static char buf[16];

	row_elems[0] = ewl_text_new( data->type->w.name );
	ewl_widget_show( row_elems[0] );

	if( data->type->w.w_type == WIDGET_ENUM_TYPE ) {
		char *enum_val;

		enum_val = ecore_hash_get( data->type->w_enum.map_rev,
															 (void *) data->w_enum.value );
		row_elems[1] = t_combo = ewl_combo_new( enum_val );

		ecore_hash_for_each_node( data->type->w_enum.map, __populate_combo_cb );

		ewl_callback_append( row_elems[1], EWL_CALLBACK_DESELECT,
												 __inspector_combo_changed, data );
		ewl_callback_append( row_elems[1], EWL_CALLBACK_VALUE_CHANGED,
												 __inspector_combo_changed, data );

		ewl_widget_show( row_elems[1] );
		ewl_widget_set_data( row_elems[1], "get", data );
		ewl_widget_set_data( row_elems[1], "widget", active_widget );
	} else if( data->type->w.w_type != WIDGET_STRUCT_TYPE ) {
		row_elems[1] = ewl_entry_new( NULL );

		ewl_callback_append( EWL_WIDGET(row_elems[1]), EWL_CALLBACK_DESELECT,
												 __inspector_entry_changed, data );
		ewl_callback_append( EWL_WIDGET(row_elems[1]), EWL_CALLBACK_VALUE_CHANGED,
												 __inspector_entry_changed, data );

		ewl_widget_show( row_elems[1] );
		ewl_widget_set_data( row_elems[1], "get", data );
		ewl_widget_set_data( row_elems[1], "widget", active_widget );
	}

	if( data->type->w.w_flags & ELEM_NO_MODIFY )
		ewl_entry_set_editable( EWL_ENTRY(row_elems[1]), 0 );

	switch( data->type->w.w_type ) {
		case WIDGET_STRING_TYPE:
			ewl_entry_set_text( EWL_ENTRY(row_elems[1]), data->w_str.value );
			break;
		case WIDGET_INTEGER_TYPE:
			sprintf( buf, "%ld", data->w_int.value );
			ewl_entry_set_text( EWL_ENTRY(row_elems[1]), buf );
			break;
	}

	row = ewl_tree_add_row( EWL_TREE(inspector_tree), EWL_ROW(prow), row_elems );
	ewl_callback_del_type( row->parent, EWL_CALLBACK_CLICKED );
	ewl_widget_show( row );

	switch( data->type->w.w_type ) {
		case WIDGET_STRUCT_TYPE:
			trow = prow;
			prow = row;

			ecore_list_for_each( data->w_struct.members, __populate_tree );

			prow = trow;
			break;
	}
}

static void
inspector_subupdate( Ewl_Container *c )
{
	Ewl_Widget *node, *row, *cell, *entry;
	Widget_Data_Elem *data;

	ewl_container_child_iterate_begin( c );

	while( (row = ewl_container_next_child(c)) != EWL_TREE_NODE(c)->row )
		;

	row = EWL_TREE_NODE(c)->row;

	cell = ewl_row_get_column( EWL_ROW(row), 1 );

	if( cell ) {
		
		ewl_container_child_iterate_begin( EWL_CONTAINER(cell) );

		entry = ewl_container_next_child( EWL_CONTAINER(cell) );

		if( entry ) {
			static char buf[16];

			data = ewl_widget_get_data( entry, "get" );

			switch( data->type->w.w_type ) {
				case WIDGET_STRING_TYPE:
					ewl_entry_set_text( EWL_ENTRY(entry), data->w_str.value );
					break;
				case WIDGET_INTEGER_TYPE:
					sprintf( buf, "%ld", data->w_int.value );
					ewl_entry_set_text( EWL_ENTRY(entry), buf );
					break;
			}
		}
	}

	while( (node = ewl_container_next_child(c)) )
		inspector_subupdate( EWL_CONTAINER(node) );
}

void
inspector_update( void )
{
	Ewl_Widget *node;

	ewl_container_child_iterate_begin( EWL_CONTAINER(inspector_tree) );

	while( (node = ewl_container_next_child( EWL_CONTAINER(inspector_tree))) )
		inspector_subupdate( EWL_CONTAINER(node) );
}

void
inspector_reset( void )
{
	Ewl_Widget *s, *w = NULL;
	Ewl_ScrollPane *scroll;
	Ecore_List *info;

	if( !active_form ) {
		ewl_container_reset( EWL_CONTAINER(inspector_tree) );
		return;
	}

	s = ecore_list_goto_first( active_form->selected );

	if( s == active_form->overlay ) {
		w = s;
	} else if( s ) {
		w = ewler_selected_get(EWLER_SELECTED(s));
	}

	if( w == active_widget ) {
		inspector_update();
		return;
	}

	scroll = EWL_SCROLLPANE(EWL_TREE(inspector_tree)->scrollarea);

	ewl_container_reset( EWL_CONTAINER(inspector_tree) );
	ewl_scrollpane_set_vscrollbar_value( scroll, 0.0 );
	ewl_scrollpane_set_hscrollbar_value( scroll, 0.0 );
	
	if( w ) {
		info = widget_get_info(w);
		active_widget = w;

		prow = NULL;
		ecore_list_for_each( info, __populate_tree );
	}
}

void
inspector_set_form( Ewler_Form *form )
{
	if( active_form != form ) {
		active_form = form;
		inspector_reset();
	}
}

Ewler_Form *
inspector_get_form( void )
{
	return active_form;
}
