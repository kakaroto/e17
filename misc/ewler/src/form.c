/**
 * form.c
 *
 * George Gensure
 */
#include <Ewl.h>

#include "ewler.h"
#include "widgets.h"
#include "form.h"
#include "form_file.h"
#include "project.h"
#include "inspector.h"
#include "selected.h"
#include "layout.h"

Ecore_List *forms;
static int widget_selected = 0;
static int widget_created = 0;
static Ecore_Hash *key_bindings = NULL;
Ewl_Widget *widget_container = NULL;

static struct {
	int active;
	int ox, oy;
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
	ewl_callback_del_type( EWL_WIDGET(val), EWL_CALLBACK_DESTROY );
}

static void
__free_elements_rev_cb( void *val )
{
	FREE(val);
}

static void
__destroy_form( Ewl_Widget *w, void *ev_data, void *user_data )
{
	Ewler_Form *form = user_data;

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
__destroy_popup( Ewl_Widget *w, void *ev_data, void *user_data )
{
	Ewler_Form *form = user_data;

	if( form->popup ) {
		ewl_widget_destroy( form->popup );
		form->popup = NULL;
	}
}

static void
__mouse_down_widget( Ewl_Widget *w, void *ev_data, void *user_data )
{
	Ewler_Form *form = EWLER_FORM(user_data);
	Ewl_Event_Mouse_Down *ev = ev_data;
	Ewler_Ctor tool_ctor;

	tool_ctor = widget_get_ctor( tool_get_name() );
	if( ev->button == 1 ) {
		if( tool_ctor && widget_is_type( w, "Ewl_Container" ) ) {
			widget_container = w;
			dragging.active = 0;
		} else if( !widget_selected ) {
			/* check for shift down */
			if( ev->modifiers != EWL_KEY_MODIFIER_SHIFT &&
					ev->modifiers != EWL_KEY_MODIFIER_CTRL )
				form_selected_clear( form );

			form_selected_append( form, w->parent );

			dragging.ox = ev->x;
			dragging.oy = ev->y;
			dragging.active = 1;

			widget_selected = 1;
		}

		if( form->popup ) {
			ewl_widget_destroy(form->popup);
			form->popup = NULL;
		}
	}
}

static void
__mouse_move_widget( Ewl_Widget *w, void *ev_data, void *user_data )
{
	Ewler_Form *form = EWLER_FORM(user_data);
	Ewl_Event_Mouse_Move *ev = ev_data;
	Ewl_Widget *c_s;
	int dx, dy;

	dx = ev->x - dragging.ox;
	dy = ev->y - dragging.oy;

	if( dragging.active ) {
		ecore_list_goto_first( form->selected );

		while( (c_s = ecore_list_next(form->selected)) ) {
			if( c_s != form->overlay )
				ewl_object_request_position(EWL_OBJECT(c_s),
																		CURRENT_X(c_s) + dx,
																		CURRENT_Y(c_s) + dy);
		}
	}

	dragging.ox = ev->x;
	dragging.oy = ev->y;
}
		
static void
__mouse_up_widget( Ewl_Widget *w, void *ev_data, void *user_data )
{
	Ewler_Form *form = user_data;
	Ewl_Event_Mouse_Up *ev = ev_data;
	Ewl_Widget *c_s;
	int dx, dy;

	dx = ev->x - dragging.ox;
	dy = ev->y - dragging.oy;

	if( dragging.active ) {
		ecore_list_goto_first( form->selected );

		while( (c_s = ecore_list_next( form->selected )) ) {
			Ewl_Widget *c_w = ewler_selected_get( EWLER_SELECTED(c_s) );

			if( c_w != form->overlay ) {
				ewl_object_request_position(EWL_OBJECT(c_s),
																		CURRENT_X(c_s) - dx,
																		CURRENT_Y(c_s) - dy);
				ewl_object_set_fill_policy( EWL_OBJECT(c_s),
																		ewl_object_get_fill_policy(EWL_OBJECT(c_w)) );
				ewl_object_set_alignment( EWL_OBJECT(c_s),
																	ewl_object_get_alignment(EWL_OBJECT(c_w)) );
				widget_changed(c_w);
			}
		}
		dragging.active = 0;
	}
}

static void
__destroy_widget( Ewl_Widget *w, void *ev_data, void *user_data )
{
	Ewler_Form *form = user_data;
	char *name;

	name = ecore_hash_remove( form->elements_rev, w );
	widget_destroy_info(w);
	ecore_hash_remove( form->elements, name );
	FREE( name );
}

static void
__mouse_in_form( Ewl_Widget *w, void *ev_data, void *user_data )
{
	Ewler_Form *form = EWLER_FORM(user_data);
	widget_container = form->overlay;

	inspector_set_form( form );
}

static void
__mouse_move_form( Ewl_Widget *w, void *ev_data, void *user_data )
{
	Ewler_Form *form = user_data;
	Ewl_Widget *c_s;

	ecore_list_goto_first( form->selected );

	while( (c_s = ecore_list_next( form->selected )) )
		if( c_s != form->overlay )
			ewl_callback_call_with_event_data(c_s, EWL_CALLBACK_MOUSE_MOVE, ev_data);
}

static void
__mouse_down_form( Ewl_Widget *w, void *ev_data, void *user_data )
{
	Ewl_Event_Mouse_Down *ev = ev_data;
	Ewl_Widget *nw, *menu_item;
	Ewler_Ctor tool_ctor;
	Ewler_Form *form = EWLER_FORM(user_data);
	static char widget_name_buf[256];


	if( form->popup ) {
		ewl_widget_destroy(form->popup);
		form->popup = NULL;
	}

	switch( ev->button ) {
		case 1:
			if( !widget_selected ) {
				tool_ctor = widget_get_ctor( tool_get_name() );
				if( tool_ctor ) {
					char *widget_name;
					Ewl_Widget *s;

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
					ewl_callback_del_type( nw, EWL_CALLBACK_SELECT );
					ewl_callback_del_type( nw, EWL_CALLBACK_DESELECT );
					ewl_callback_del_type( nw, EWL_CALLBACK_MOUSE_DOWN );
					ewl_callback_del_type( nw, EWL_CALLBACK_MOUSE_UP );
					ewl_callback_del_type( nw, EWL_CALLBACK_MOUSE_MOVE );

					ewl_callback_append( nw, EWL_CALLBACK_MOUSE_DOWN,
															 __mouse_down_widget, form );
					ewl_callback_append( nw, EWL_CALLBACK_FOCUS_IN,
															 __mouse_in_form, form );
					ewl_callback_append( nw, EWL_CALLBACK_DESTROY,
															 __destroy_widget, form );
					ewl_object_request_position( EWL_OBJECT(nw), ev->x, ev->y );
					ewl_container_append_child( EWL_CONTAINER(widget_container), nw );
					ewl_widget_show( nw );

					s = ewler_selected_new( nw );
					ewl_widget_show( s );

					form_selected_clear( form );

					form_selected_append( form, s );

					form_set_dirty( form );

					ecore_hash_set( form->elements, widget_name, nw );
					ecore_hash_set( form->elements_rev, nw, widget_name );
					widget_created = 1;
				} else {
					form_selected_clear( form );
					form_selected_append( form, form->overlay );
				}

				tool_clear_name();
			}
			break;
		case 2: break;
		case 3:
			form->popup = ewl_imenu_new(NULL, "Configure");
			ewl_object_request_position(EWL_OBJECT(form->popup), ev->x, ev->y);
			ewl_object_set_fill_policy(EWL_OBJECT(form->popup), EWL_FLAG_FILL_NONE);
			ewl_container_append_child(EWL_CONTAINER(form->overlay), form->popup);
			ewl_widget_show(form->popup);
			ewl_callback_call(form->popup, EWL_CALLBACK_SELECT);

			menu_item = ewl_menu_item_new(NULL, "Lay Out Horizontally");
			ewl_callback_append( menu_item, EWL_CALLBACK_SELECT,
													 layout_horizontal_cb, form );
			ewl_callback_append( menu_item, EWL_CALLBACK_SELECT,
													 __destroy_popup, form );
			ewl_container_append_child(EWL_CONTAINER(form->popup), menu_item);
			ewl_widget_show(menu_item);

			menu_item = ewl_menu_item_new(NULL, "Lay Out Vertically");
			ewl_callback_append( menu_item, EWL_CALLBACK_SELECT,
													 layout_vertical_cb, form );
			ewl_callback_append( menu_item, EWL_CALLBACK_SELECT,
													 __destroy_popup, form );
			ewl_container_append_child(EWL_CONTAINER(form->popup), menu_item);
			ewl_widget_show(menu_item);

			menu_item = ewl_menu_item_new(NULL, "Break Layout");
			ewl_callback_append( menu_item, EWL_CALLBACK_SELECT,
													 layout_break_cb, form );
			ewl_callback_append( menu_item, EWL_CALLBACK_SELECT,
													 __destroy_popup, form );
			ewl_container_append_child(EWL_CONTAINER(form->popup), menu_item);
			ewl_widget_show(menu_item);

			menu_item = EWL_WIDGET(ewl_menu_separator_new());
			ewl_container_append_child(EWL_CONTAINER(form->popup), menu_item);
			ewl_widget_show(menu_item);

			menu_item = ewl_menu_item_new(NULL, "Edit Callbacks");
			ewl_container_append_child(EWL_CONTAINER(form->popup), menu_item);
			ewl_callback_append( menu_item, EWL_CALLBACK_SELECT,
													 __destroy_popup, form );
#if 0
			ewl_callback_append( menu_item, EWL_CALLBACK_SELECT,
													 callbacks_show, NULL );
#endif

			ewl_widget_show(menu_item);
			break;
	}

	widget_container = w;
	widget_selected = 0;
}

static void
__key_down_form( Ewl_Widget *w, void *ev_data, void *user_data )
{
	Ewler_Form *form = user_data;
	Ewl_Event_Key_Down *ev = ev_data;
	Ewler_Form_Key_Cb key_cb = ecore_hash_get( key_bindings, ev->keyname );

	if( key_cb )
		key_cb( form );
}

void
form_set_widget_selected( void )
{
	widget_selected = 1;
}

void
form_clear_widget_selected( void )
{
	widget_selected = 0;
}

int
form_widget_created( void )
{
	return widget_created;
}

void
form_clear_widget_created( void )
{
	widget_created = 0;
}

void
form_set_widget_dragging( Ewl_Widget *w, Ewl_Event_Mouse_Down *ev )
{
	dragging.active = 1;

	widget_selected = 1;
}

void
form_clear_widget_dragging( void )
{
	dragging.active = 0;
}

void
form_add_widget( Ewler_Form *form, char *name, Ewl_Widget *w )
{
	ewl_callback_del_type( w, EWL_CALLBACK_CLICKED );
	ewl_callback_del_type( w, EWL_CALLBACK_MOUSE_DOWN );
	ewl_callback_del_type( w, EWL_CALLBACK_MOUSE_UP );
	ewl_callback_del_type( w, EWL_CALLBACK_MOUSE_MOVE );

	ewl_callback_append( w, EWL_CALLBACK_MOUSE_DOWN,
											 __mouse_down_widget, form );
	ewl_callback_append( w, EWL_CALLBACK_FOCUS_IN,
											 __mouse_in_form, form );

	ewl_object_set_fill_policy( EWL_OBJECT(w), EWL_FLAG_FILL_NONE );

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
	ewl_object_set_minimum_size( EWL_OBJECT(form->window), 800, 600 );
	ewl_object_set_fill_policy( EWL_OBJECT(form->window), EWL_FLAG_FILL_SHRINK );
	ewl_widget_show( form->window );

	snprintf( buf, 255, "Ewl_Form%d", count ); 

	form->overlay = ewl_overlay_new();

	ewl_widget_set_appearance(form->overlay, "background");
	ewl_theme_data_set_str(form->overlay, "/background/file",
												 PACKAGE_DATA_DIR"/themes/ewler.eet");
	ewl_theme_data_set_str(form->overlay, "/background/group", "background");

	ewl_object_set_fill_policy( EWL_OBJECT(form->overlay), EWL_FLAG_FILL_ALL );
	widget_create_info( form->overlay, "Ewl_Overlay", strdup( buf ) );
	ewl_callback_append( form->overlay, EWL_CALLBACK_MOUSE_DOWN,
											 __mouse_down_form, form );
	ewl_callback_append( form->overlay, EWL_CALLBACK_MOUSE_MOVE,
											 __mouse_move_form, form );
	ewl_callback_append( form->overlay, EWL_CALLBACK_FOCUS_IN,
											 __mouse_in_form, form );
	ewl_callback_append( form->overlay, EWL_CALLBACK_MOUSE_MOVE,
											 __mouse_move_widget, form );
	ewl_callback_append( form->overlay, EWL_CALLBACK_KEY_DOWN,
											 __key_down_form, form );
	ewl_container_append_child( EWL_CONTAINER(form->window), form->overlay );
	ewl_widget_set_data( form->overlay, "unsizable", (void *) 1 );
	ewl_widget_show( form->overlay );

#if 0
	form->selector = ewler_selected_new( form->overlay );
	ewl_callback_call( form->selector, EWL_CALLBACK_DESELECT );
	ewl_object_set_preferred_size( EWL_OBJECT(form->selector), 800, 600 );
	ewl_widget_show( form->selector );
#endif

	form->selected = ecore_list_new();
	form->elements = ecore_hash_new( ecore_str_hash, ecore_str_compare );
	form->elements_rev =
		ecore_hash_new( ecore_direct_hash, ecore_direct_compare );
	form->has_been_saved = 0;
	form->dirty = 0;
	form->cnt = 0;
	form->popup = NULL;
	form->layout = NULL;

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
			ewl_window_set_title( EWL_WINDOW(form->window), form->filename );
			form_save_file( form, 0 );
			ewl_widget_destroy( w->parent );
		} else
			ewler_error_dialog( "Unable to open file '%s' for writing", filename );
	} else {
		ewl_widget_destroy( w->parent );
	}
}

void
form_save_file( Ewler_Form *form, int save_as )
{
	if( !form )
		form = inspector_get_form();

	if( form ) {
		if( !form->has_been_saved || save_as ) {
			Ewl_Widget *window, *dialog;

			window = ewl_window_new();
			ewl_widget_show( window );

			dialog = ewl_filedialog_new( EWL_FILEDIALOG_TYPE_SAVE );
			ewl_container_append_child( EWL_CONTAINER(window), dialog );
			ewl_callback_append( dialog, EWL_CALLBACK_VALUE_CHANGED,
													 __save_form_cb, form );
			ewl_callback_append( window, EWL_CALLBACK_DELETE_WINDOW,
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
	ewl_object_set_minimum_size( EWL_OBJECT(form->window), 800, 600 );
	ewl_object_set_fill_policy( EWL_OBJECT(form->window), EWL_FLAG_FILL_SHRINK );
	ewl_widget_show( form->window );

	form->elements = ecore_hash_new( ecore_str_hash, ecore_str_compare );
	form->elements_rev =
		ecore_hash_new( ecore_direct_hash, ecore_direct_compare );

	form->popup = NULL;

	if( form_file_read( form ) ) {
		/* add support for more verbose errors from form_file */
		ewler_error_dialog( "Error opening form '%s'", form->filename );
		FREE( form->filename );
		FREE( form );
		return;
	}

	ewl_widget_set_data( form->overlay, "unsizable", (void *) 1 );
	ewl_object_set_fill_policy( EWL_OBJECT(form->overlay), EWL_FLAG_FILL_FILL );

	ewl_widget_set_appearance(form->overlay, "background");
	ewl_theme_data_set_str(form->overlay, "/background/file",
												 PACKAGE_DATA_DIR"/themes/ewler.eet");
	ewl_theme_data_set_str(form->overlay, "/background/group", "background");

	ewl_callback_del_type( form->overlay, EWL_CALLBACK_MOUSE_DOWN );
	ewl_callback_del_type( form->overlay, EWL_CALLBACK_MOUSE_UP );
	ewl_callback_del_type( form->overlay, EWL_CALLBACK_FOCUS_IN );

	ewl_callback_append( form->overlay, EWL_CALLBACK_MOUSE_DOWN,
											 __mouse_down_form, form );
	ewl_callback_append( form->overlay, EWL_CALLBACK_MOUSE_MOVE,
											 __mouse_move_form, form );
	ewl_callback_append( form->overlay, EWL_CALLBACK_FOCUS_IN,
											 __mouse_in_form, form );
	ewl_callback_append( form->overlay, EWL_CALLBACK_MOUSE_MOVE,
											 __mouse_move_widget, form );
	ewl_callback_append( form->overlay, EWL_CALLBACK_KEY_DOWN,
											 __key_down_form, form );
	ewl_container_append_child( EWL_CONTAINER(form->window), form->overlay );

	form->selected = ecore_list_new();
	form->has_been_saved = 1;
	form->dirty = 0;
	form->cnt = 0;
	form->layout = NULL;

	ecore_list_append( forms, form );
}

void
form_selected_clear( Ewler_Form *form )
{
	Ewl_Widget *s;
	ecore_list_goto_first( form->selected );

	while( (s = ecore_list_remove(form->selected)) ) {
		if( s == form->overlay )
			continue;

		ewl_callback_del(s, EWL_CALLBACK_MOUSE_MOVE,
										 __mouse_move_widget);
		ewl_callback_del(ewler_selected_get(EWLER_SELECTED(s)),
										 EWL_CALLBACK_MOUSE_UP,
										 __mouse_up_widget);
		ewl_callback_call(s, EWL_CALLBACK_DESELECT);
	}
}

void
form_selected_append( Ewler_Form *form, Ewl_Widget *s )
{
	if( s != form->overlay ) {
		ewl_callback_call( s, EWL_CALLBACK_SELECT );
		ewl_callback_append(s, EWL_CALLBACK_MOUSE_MOVE,
												__mouse_move_widget, form);
		ewl_callback_append(ewler_selected_get(EWLER_SELECTED(s)),
												EWL_CALLBACK_MOUSE_UP,
												__mouse_up_widget, form);
	}

	if( ecore_list_goto( form->selected, s ) == NULL ) {
		ecore_list_append( form->selected, s );
		inspector_reset();
	}
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
__form_delete( Ewler_Form *form )
{
	Ewl_Widget *s;

	ecore_list_goto_first( form->selected );

	while( (s = ecore_list_remove( form->selected)) ) {
		if( s == form->overlay )
			continue;

		ewl_widget_destroy( s );
	}

	form_selected_clear( form );
}

void
__form_select_up( Ewler_Form *form )
{
	Ewl_Widget *s = ecore_list_goto_first( form->selected );

	if( !s || s == form->overlay ) {
		return;
	} else {
		form_selected_clear( form );
		if( (s = s->parent) == form->overlay )
			form_selected_append( form, s );
		else if( (s = s->parent) )
			form_selected_append( form, s );
	}
}

void
__form_select_down( Ewler_Form *form )
{
	Ewl_Widget *s = ecore_list_goto_first( form->selected );

	if( !s ) 
		return;

	if(s != form->overlay) {
		s = ewler_selected_get( EWLER_SELECTED(s) );
		if( widget_is_type(s, "Ewl_Container") &&
				!ecore_list_is_empty( EWL_CONTAINER(s)->children ) ) {
			s = ecore_list_goto_first( EWL_CONTAINER(s)->children );
			form_selected_clear( form );
			form_selected_append( form, s );
		}
	} else if( !ecore_list_is_empty( EWL_CONTAINER(s)->children ) ) {
		s = ecore_list_goto_first( EWL_CONTAINER(s)->children );
		form_selected_clear( form );
		form_selected_append( form, s );
	}
}

void
ewler_forms_init( void )
{
	forms = ecore_list_new();

	key_bindings = ecore_hash_new( ecore_str_hash, ecore_str_compare );

	ecore_hash_set( key_bindings, "Delete", __form_delete );
	ecore_hash_set( key_bindings, "Up", __form_select_up );
	ecore_hash_set( key_bindings, "Down", __form_select_down );
}
