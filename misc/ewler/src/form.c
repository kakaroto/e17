/**
 * Form code for managing active forms/selections
 */
#include "ewler.h"

static Ewler_Project_File *active_form = NULL;
static Ecore_List *form_select = NULL;

EWL_CALLBACK_DEFN(ewler_form_mouse_move_cb)
{
	Ewl_Widget *dragging;

	dragging = ewler_widget_dragging_get();

	if( dragging )
		ewl_callback_call_with_event_data( dragging, EWL_CALLBACK_MOUSE_MOVE, ev );
}

EWL_CALLBACK_DEFN(ewler_form_mouse_down_cb)
{
	Ewl_Event_Mouse_Down *ev_data;

	ev_data = ev;

	if( ev_data->button == 3 && active_form ) {
		if( VISIBLE(FORM(active_form).menu) )
			ewl_widget_hide( FORM(active_form).menu );
		else {
			ewl_object_position_request( EWL_OBJECT(FORM(active_form).menu),
																	 ev_data->x, ev_data->y );
			ewl_callback_call( FORM(active_form).menu, EWL_CALLBACK_SELECT );
			ewl_widget_show( FORM(active_form).menu );
		}
	}
}

EWL_CALLBACK_DEFN(ewler_form_focus_cb)
{
	active_form = user_data;
}

EWL_CALLBACK_DEFN(ewler_form_close_cb)
{
	/* close up shop */
	ewler_form_active_set( NULL );
}

Ewler_Project_File *
ewler_form_active_get( void )
{
	return active_form;
}

void
ewler_form_active_set( Ewler_Project_File *form )
{
	if( !form || form->type == EWLER_PROJECT_FILE_FORM )
		active_form = form;
}

void
ewler_form_select_clear( void )
{
	Ecore_List *l;
	Ewler_Widget *w;

	l = ewler_form_select_get();

	ecore_list_goto_first( l );
	while( (w = ecore_list_next(l)) )
		ewler_widget_deselect( w );
}

static void
_form_select_add( Ewler_Widget *w )
{
	if( !w )
		return;

	if( w->selected )
		ecore_list_append( form_select, w );

	if( ewler_spec_is( w->spec, "Ewl_Container" ) ) {
		Ewl_Widget *c;

		ewl_container_child_iterate_begin( EWL_CONTAINER(w->ewl_w) );

		while( (c = ewl_container_child_next( EWL_CONTAINER(w->ewl_w) )) ) {
			_form_select_add( ewler_widget_from_bg(c) );
		}
	}
}

Ecore_List *
ewler_form_select_get( void )
{
	if( form_select == NULL )
		form_select = ecore_list_new();

	ecore_list_clear( form_select );

	if( active_form )
		_form_select_add( FORM(active_form).top );

	ecore_list_goto_first( form_select );

	return form_select;
}

static void
_form_widget_attr_save( void *value, void *user_data )
{
	Ecore_Hash_Node *node = value;
	Ewler_Elem *elem = node->value;
	EXML *xml;

	xml = user_data;

	if( !elem->changed && elem->spec->type != EWLER_SPEC_ELEM_STRUCT_TYPE )
		return;

	exml_start( xml );
	exml_tag_set( xml, "attr" );
	exml_attribute_set( xml, "name", node->key );
	exml_attribute_set( xml, "changed", elem->changed ? "1" : "0" );

	if( elem->spec->type != EWLER_SPEC_ELEM_STRUCT_TYPE )
		exml_value_set( xml, ewler_widget_elem_to_s(elem) );
	else
		ecore_hash_for_each_node( elem->v.struct_val, _form_widget_attr_save, xml );

	exml_end( xml );
}

static void
_form_widget_save( EXML *xml, Ewler_Widget *w )
{
	if( w ) {
		exml_start( xml );
		exml_tag_set( xml, "widget" );
		exml_attribute_set( xml, "type", w->spec->name );
		
		ecore_hash_for_each_node( w->attrs, _form_widget_attr_save, xml );

		if( ewler_spec_is( w->spec, "Ewl_Container" ) ) {
			Ewl_Widget *c;

			ewl_container_child_iterate_begin( EWL_CONTAINER(w->ewl_w) );

			while( (c = ewl_container_child_next( EWL_CONTAINER(w->ewl_w) )) )
				_form_widget_save( xml, ewler_widget_from_bg(c) );
		}

		exml_end( xml );
	}
}

int
ewler_form_save( Ewler_Project_File *form )
{
	EXML *xml;

	xml = exml_new();

	exml_start( xml );
	exml_tag_set( xml, "form" );
	
	_form_widget_save( xml, FORM(form).top );

	exml_end( xml );

	exml_goto_top( xml );

	return exml_file_write( xml, form->filename );
}
