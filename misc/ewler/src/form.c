/**
 * form.c
 *
 * George Gensure
 */
#include <Ewl.h>

#include "ewl_stock.h"
#include "ewl_dialog.h"
#include "ewl_filedialog_stock.h"

#include "ewler.h"
#include "widgets.h"
#include "form.h"
#include "form_file.h"
#include "project.h"
#include "inspector.h"
#include "selected.h"

Ecore_List *forms;
static int widget_selected = 0;
Ewl_Widget *widget_container = NULL;

static struct {
	int active;
	int x, y;
} dragging;

static void
__destroy_window( Ewl_Widget *w, void *ev_data, void *user_data )
{
	if( user_data )
		ewl_widget_destroy( EWL_WIDGET(user_data) );
	else
		ewl_widget_destroy( w );
}

static void
__free_elements_cb( void *val )
{
	widget_destroy_info( EWL_WIDGET(val) );
}

static void
__free_elements_rev_cb( void *val )
{
	FREE( val );
}

static void
__destroy_form( Ewl_Widget *w, void *ev_data, void *user_data )
{
	Ewler_Form *form = EWLER_FORM(user_data);

	if( form->dirty ) {
		/* manufacture save/ok/cancel dialog */
	}

	FREE( form->filename );
	ecore_list_destroy( form->selected );
	ecore_hash_set_free_value( form->elements, __free_elements_cb );
	ecore_hash_destroy( form->elements );
	ecore_hash_set_free_value( form->elements_rev, __free_elements_rev_cb );
	ecore_hash_destroy( form->elements_rev );
	ewl_widget_destroy( form->window );
	ecore_list_goto( forms, form );
	ecore_list_remove( forms );
	FREE( form );

	inspector_set_form( NULL );
}

static void
__mouse_down_widget( Ewl_Widget *w, void *ev_data, void *user_data )
{
	Ewler_Form *form = EWLER_FORM(user_data);
	Ewl_Event_Mouse_Down *ev = ev_data;
	Ewler_Ctor tool_ctor;

	tool_ctor = widget_get_ctor( tool_get_name() );
	if( tool_ctor && widget_is_type( w, "Ewl_Container" ) ) {
		widget_container = w;
	} else if( !widget_selected ) {
		/* check for shift down */
		form_selected_clear( form );

		form_selected_append( form, w );

		dragging.x = ev->x - CURRENT_X(w);
		dragging.y = ev->y - CURRENT_Y(w);
		dragging.active = 1;

		widget_selected = 1;
	}
}

static void
__mouse_move_widget( Ewl_Widget *w, void *ev_data, void *user_data )
{
	Ewler_Form *form = EWLER_FORM(user_data);
	Ewl_Event_Mouse_Move *ev = ev_data;
	Ewl_Widget *c_s;

	if( dragging.active ) {
		ecore_list_goto_first( form->selected );

		while( (c_s = ecore_list_next(form->selected)) ) {
			ewl_object_request_position(EWL_OBJECT(c_s),
																	ev->x - dragging.x, ev->y - dragging.y);
		}
	}
}
		
static void
__mouse_up_widget( Ewl_Widget *w, void *ev_data, void *user_data )
{
	Ewler_Form *form = EWLER_FORM(user_data);
	Ewl_Event_Mouse_Up *ev = ev_data;
	Ewl_Widget *c_s, *c_w;

	if( dragging.active ) {
		ecore_list_goto_first( form->selected );

		while( (c_s = ecore_list_next( form->selected )) ) {
			ewl_object_request_position(EWL_OBJECT(c_s),
																	ev->x - dragging.x, ev->y - dragging.y);
			widget_changed(ewler_selected_get(EWLER_SELECTED(c_s)));
		}
		dragging.x = dragging.y = 0;
		dragging.active = 0;

		c_w = ewler_selected_get(EWLER_SELECTED(c_s));

	}
}

static void
__mouse_in_form( Ewl_Widget *w, void *ev_data, void *user_data )
{
	Ewler_Form *form = EWLER_FORM(user_data);
	widget_container = form->overlay;

	inspector_set_form( form );
}

static void
__mouse_down_form( Ewl_Widget *w, void *ev_data, void *user_data )
{
	Ewl_Event_Mouse_Down *ev = ev_data;
	Ewl_Widget *nw;
	Ewler_Ctor tool_ctor;
	Ewler_Form *form = EWLER_FORM(user_data);
	static char widget_name_buf[256];

	switch( ev->button ) {
		case 1:
			if( !widget_selected ) {
				tool_ctor = widget_get_ctor( tool_get_name() );
				if( tool_ctor ) {
					char *widget_name;

					if( !widget_container )
						widget_container = form->overlay;

					nw = tool_ctor( tool_get_name(), tool_get_name(), tool_get_name() );

					snprintf( widget_name_buf, 255, "%s%d",
										tool_get_name(), form->cnt++ );
					widget_name = strdup( widget_name_buf );

					widget_create_info( nw, tool_get_name(), widget_name );
					/* set the defaults */
					ewl_object_set_fill_policy( EWL_OBJECT(nw), EWL_FLAG_FILL_NONE );

					ewl_callback_del_type( nw, EWL_CALLBACK_CLICKED );
					ewl_callback_del_type( nw, EWL_CALLBACK_MOUSE_DOWN );
					ewl_callback_del_type( nw, EWL_CALLBACK_MOUSE_UP );
					ewl_callback_del_type( nw, EWL_CALLBACK_MOUSE_MOVE );

					ewl_callback_append( nw, EWL_CALLBACK_MOUSE_DOWN,
															 __mouse_down_widget, form );
					ewl_callback_append( nw, EWL_CALLBACK_FOCUS_IN,
															 __mouse_in_form, form );
					ewl_object_request_position( EWL_OBJECT(nw), ev->x, ev->y );
					ewl_container_append_child( EWL_CONTAINER(widget_container), nw );
					ewl_widget_show( nw );

					form_selected_clear( form );

					form_selected_append( form, nw );

					form_set_dirty( form );

					ecore_hash_set( form->elements, widget_name, nw );
					ecore_hash_set( form->elements_rev, nw, widget_name );
				} else {
					form_selected_clear( form );
					form_selected_append( form, form->overlay );
				}

				tool_clear_name();
			}
			break;
		case 2: break;
		case 3: /* popup menu */
			break;
	}

	widget_container = w;
	widget_selected = 0;
}

void
form_add_widget( Ewler_Form *form, char *name, Ewl_Widget *w )
{
	ewl_callback_del_type( w, EWL_CALLBACK_CLICKED );

	ewl_callback_append( w, EWL_CALLBACK_MOUSE_DOWN,
											 __mouse_down_widget, form );
	ewl_callback_append( w, EWL_CALLBACK_FOCUS_IN,
											 __mouse_in_form, form );

	ecore_hash_set( form->elements, name, w );
	ecore_hash_set( form->elements_rev, w, name );
}

void
form_new( void )
{
	Ewler_Form *form;
	static int count = 0;
	static char buf[256];

	snprintf( buf, 255, "untitled-%d.ui", count );

	form = ALLOC(Ewler_Form);
	form->filename = strdup( buf );

	project_add_form( form->filename );

	form->window = ewl_window_new();
	ewl_window_set_title( EWL_WINDOW(form->window), form->filename );
	ewl_callback_append( form->window, EWL_CALLBACK_DELETE_WINDOW,
											 __destroy_form, form );
	ewl_object_set_preferred_size( EWL_OBJECT(form->window), 800, 600 );
	ewl_widget_show( form->window );

	snprintf( buf, 255, "Ewl_Form%d", count ); 

	form->overlay = ewl_overlay_new();

	ewl_object_set_fill_policy( EWL_OBJECT(form->overlay), EWL_FLAG_FILL_ALL );
	widget_create_info( form->overlay, "Ewl_Overlay", strdup( buf ) );
	ewl_callback_append( form->overlay, EWL_CALLBACK_MOUSE_DOWN,
											 __mouse_down_form, form );
	ewl_callback_append( form->overlay, EWL_CALLBACK_FOCUS_IN,
											 __mouse_in_form, form );
	ewl_callback_append( form->overlay, EWL_CALLBACK_MOUSE_MOVE,
											 __mouse_move_widget, form );
	ewl_container_append_child( EWL_CONTAINER(form->window), form->overlay );
	ewl_widget_show( form->overlay );

	form->selected = ecore_list_new();
	form->elements = ecore_hash_new( ecore_str_hash, ecore_str_compare );
	form->elements_rev =
		ecore_hash_new( ecore_direct_hash, ecore_direct_compare );
	form->has_been_saved = 0;
	form->dirty = 0;
	form->cnt = 0;

	count++;

	ecore_list_append( forms, form );
}

static void
__save_form_cb( Ewl_Widget *w, void *ev_data, void *user_data )
{
	FILE *fptr;
	char *filename = ev_data;
	Ewler_Form *form = user_data;

	if( filename ) {
		if( (fptr = fopen( filename, "w" )) ) {
			fclose( fptr );

			FREE(form->filename);
			form->filename = strdup( filename );

			form->has_been_saved = 1;
			form_save_file( form, 0 );
			ewl_widget_destroy( w );
		} else
			ewler_error_dialog( "Unable to open file '%s' for writing", filename );
	} else {
		ewl_widget_destroy( w );
	}
}

void
form_save_file( Ewler_Form *form, int save_as )
{
	if( !form )
		form = inspector_get_form();

	if( form ) {
		if( !form->has_been_saved || save_as ) {
			Ewl_Widget *dialog;

			dialog = ewl_filedialog_stock_new( EWL_FILEDIALOG_TYPE_SAVE );
			ewl_callback_append( dialog, EWL_CALLBACK_VALUE_CHANGED,
													 __save_form_cb, form );
			ewl_callback_append( dialog, EWL_CALLBACK_DELETE_WINDOW,
													 __destroy_window, NULL );
			ewl_widget_show( dialog );
		} else {
			form->has_been_saved = 0;
			if( form_file_write( form ) )
				ewler_error_dialog( "Error saving file '%s'!", form->filename );
			else {
				form->has_been_saved = 1;
				form->dirty = 0;
			}
		}
	} else {
		/* do a project save */
	}
}

void
form_open_file( char *filename )
{
	Ewler_Form *form;

	form = ALLOC(Ewler_Form);
	form->filename = strdup( filename );

	form->window = ewl_window_new();
	ewl_window_set_title( EWL_WINDOW(form->window), form->filename );
	ewl_callback_append( form->window, EWL_CALLBACK_DELETE_WINDOW,
											 __destroy_form, form );
	ewl_object_set_preferred_size( EWL_OBJECT(form->window), 800, 600 );
	ewl_widget_show( form->window );

	form->elements = ecore_hash_new( ecore_str_hash, ecore_str_compare );
	form->elements_rev =
		ecore_hash_new( ecore_direct_hash, ecore_direct_compare );

	if( form_file_read( form ) ) {
		/* add support for more verbose errors from form_file */
		ewler_error_dialog( "Error opening form '%s'", form->filename );
		FREE( form->filename );
		FREE( form );
		return;
	}

	ewl_container_append_child( EWL_CONTAINER(form->window), form->overlay );
	ewl_widget_set_appearance( form->overlay, "entry" );

	ewl_callback_del_type( form->overlay, EWL_CALLBACK_MOUSE_DOWN );
	ewl_callback_del_type( form->overlay, EWL_CALLBACK_MOUSE_UP );
	ewl_callback_del_type( form->overlay, EWL_CALLBACK_FOCUS_IN );

	ewl_callback_append( form->overlay, EWL_CALLBACK_MOUSE_DOWN,
											 __mouse_down_form, form );
	ewl_callback_append( form->overlay, EWL_CALLBACK_FOCUS_IN,
											 __mouse_in_form, form );

	form->selected = ecore_list_new();
	form->has_been_saved = 1;
	form->dirty = 0;
	form->cnt = 0;

	ecore_list_append( forms, form );
}

void
form_selected_clear( Ewler_Form *form )
{
	Ewl_Widget *s;
	ecore_list_goto_first( form->selected );

	while( (s = ecore_list_remove(form->selected)) ) {
		ewl_callback_call(s, EWL_CALLBACK_DESELECT);
		ewl_widget_destroy( s );
	}
}

void
form_selected_append( Ewler_Form *form, Ewl_Widget *w )
{
	Ewl_Widget *s;
	
	s = ewler_selected_new( w );
	ewl_widget_show(s);
	ewl_callback_append(s, EWL_CALLBACK_MOUSE_MOVE,
											__mouse_move_widget, form);
	ewl_callback_append(s, EWL_CALLBACK_MOUSE_UP,
											__mouse_up_widget, form);

	ecore_list_append( form->selected, s );
	inspector_reset();
}

void
form_set_dirty( Ewler_Form *form )
{
	form->dirty = 1;
}

Ewl_Widget *
form_get_element_by_name( Ewler_Form *form, char *name )
{
	return ecore_hash_get( form->elements, name );
}

void
ewler_forms_close( void )
{
	Ewler_Form *form;

	if( forms ) {
		ecore_list_goto_first( forms );

		while( (form = EWLER_FORM(ecore_list_remove( forms ))) ) {
			if( form->dirty )
				fprintf( stderr, "closing a dirty form\n" );

			FREE( form->filename );
			ewl_widget_destroy( form->window );
			FREE( form );
		}

		ecore_list_destroy( forms );
	}
}

void
ewler_forms_init( void )
{
	forms = ecore_list_new();
}
