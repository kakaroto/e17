#include <stdarg.h>
#include <stdio.h>
#include <Ewl.h>

#include "ewler.h"
#include "form.h"
#include "widgets.h"
#include "inspector.h"
#include "project.h"

Ewl_Widget *tool_tree;
char *active_tool = NULL;

extern void
__destroy_main_window( Ewl_Widget *w, void *ev_data, void *user_data );

static void
__destroy_dialog( Ewl_Widget *w, void *ev_data, void *user_data )
{
	Ewl_Widget *dialog = user_data;

	ewl_widget_destroy( dialog );
}

static void
__create_new_form( Ewl_Widget *w, void *ev_data, void *user_data )
{
	form_new();
}

static void
__create_new_project( Ewl_Widget *w, void *ev_data, void *user_data )
{
	project_new_dialog();
}

static void
__open_project( Ewl_Widget *w, void *ev_data, void *user_data )
{
	project_open_dialog();
}

static void
__open_form_cb( Ewl_Widget *w, void *ev_data, void *user_data )
{
	char *filename = ev_data;
	Ewl_Widget *window = user_data;

	if( filename ) {
		ewler_open_file( filename );
		ewl_widget_destroy( window );
	}
}

static void
__open_form( Ewl_Widget *w, void *ev_data, void *user_data )
{
	Ewl_Widget *window, *dialog;

	window = ewl_window_new();
	ewl_widget_show( window );

	dialog = ewl_filedialog_new( EWL_FILEDIALOG_TYPE_OPEN );
	ewl_container_append_child( EWL_CONTAINER(window), dialog );
	ewl_callback_append( dialog, EWL_CALLBACK_VALUE_CHANGED,
											 __open_form_cb, window );
	ewl_callback_append( window, EWL_CALLBACK_DELETE_WINDOW,
											 __destroy_dialog, window );
	ewl_widget_show( dialog );
}

static void
__save( Ewl_Widget *w, void *ev_data, void *user_data )
{
	int save_as = (int) user_data;

	form_save_file( NULL, save_as );
}

static void
__toggle_inspector( Ewl_Widget *w, void *ev_data, void *user_data )
{
	if( inspector_visible() )
		inspector_hide();
	else
		inspector_show();
}

void
ewler_open_file( char *filename )
{
	FILE *fptr;

	if( (fptr = fopen( filename, "r" )) ) {
		fclose( fptr );
		form_open_file( filename );
	} else {
		ewler_error_dialog( "Unable to open file '%s'", filename );
	}
}

void
ewler_menu_init( Ewl_Widget *main_layout )
{
	Ewl_Widget *menu_layout, *separator;
	Ewl_Widget *menu_item;
	Ewl_Widget *menu;

	menu_layout = ewl_hbox_new();
	ewl_object_set_fill_policy( EWL_OBJECT(menu_layout), EWL_FLAG_FILL_HFILL );
	ewl_container_append_child( EWL_CONTAINER(main_layout), menu_layout );
	ewl_widget_show( menu_layout );

	separator = ewl_hseparator_new();
	ewl_container_append_child( EWL_CONTAINER(main_layout), separator );
	ewl_widget_show( separator );

	menu = ewl_imenu_new( NULL, "File" );
	ewl_object_set_fill_policy( EWL_OBJECT(menu), EWL_FLAG_FILL_NONE);
	ewl_container_append_child( EWL_CONTAINER(menu_layout), menu );
	ewl_widget_show( menu );

	menu_item = ewl_menu_item_new( NULL, "New Form" );
	ewl_container_append_child( EWL_CONTAINER(menu), menu_item );
	ewl_callback_append( menu_item, EWL_CALLBACK_SELECT,
											 __create_new_form, NULL );
	ewl_widget_show( menu_item );

	menu_item = ewl_menu_item_new( NULL, "New Project" );
	ewl_container_append_child( EWL_CONTAINER(menu), menu_item );
	ewl_callback_append( menu_item, EWL_CALLBACK_SELECT,
											 __create_new_project, NULL );
	ewl_widget_show( menu_item );

	menu_item = ewl_menu_item_new( NULL, "Open" );
	ewl_container_append_child( EWL_CONTAINER(menu), menu_item );
	ewl_callback_append( menu_item, EWL_CALLBACK_SELECT,
											 __open_form, NULL );
	ewl_widget_show( menu_item );

	menu_item = ewl_menu_item_new( NULL, "Open Project" );
	ewl_container_append_child( EWL_CONTAINER(menu), menu_item );
	ewl_callback_append( menu_item, EWL_CALLBACK_SELECT,
											 __open_project, NULL );
	ewl_widget_show( menu_item );

	menu_item = EWL_WIDGET(ewl_menu_separator_new());
	ewl_object_set_fill_policy( EWL_OBJECT(menu_item), EWL_FLAG_FILL_HFILL );
	ewl_container_append_child( EWL_CONTAINER(menu), menu_item );
	ewl_widget_show( menu_item );

	menu_item = ewl_menu_item_new( NULL, "Save" );
	ewl_container_append_child( EWL_CONTAINER(menu), menu_item );
	ewl_callback_append( menu_item, EWL_CALLBACK_SELECT,
											 __save, (void *) 0 );
	ewl_widget_show( menu_item );

	menu_item = ewl_menu_item_new( NULL, "Save As" );
	ewl_container_append_child( EWL_CONTAINER(menu), menu_item );
	ewl_callback_append( menu_item, EWL_CALLBACK_SELECT,
											 __save, (void *) 1 );
	ewl_widget_show( menu_item );

	menu_item = EWL_WIDGET(ewl_menu_separator_new());
	ewl_object_set_fill_policy( EWL_OBJECT(menu_item), EWL_FLAG_FILL_HFILL );
	ewl_container_append_child( EWL_CONTAINER(menu), menu_item );
	ewl_widget_show( menu_item );

	menu_item = ewl_menu_item_new( NULL, "Quit" );
	ewl_container_append_child( EWL_CONTAINER(menu), menu_item );
	ewl_callback_append( menu_item, EWL_CALLBACK_SELECT,
											 __destroy_main_window, NULL );
	ewl_widget_show( menu_item );

	menu = ewl_imenu_new( NULL, "Window" );
	ewl_object_set_fill_policy( EWL_OBJECT(menu), EWL_FLAG_FILL_NONE);
	ewl_container_append_child( EWL_CONTAINER(menu_layout), menu );
	ewl_widget_show( menu );

	menu_item = ewl_menu_item_new( NULL, "Inspector" );
	ewl_container_append_child( EWL_CONTAINER(menu), menu_item );
	ewl_callback_append( menu_item, EWL_CALLBACK_SELECT,
											 __toggle_inspector, NULL );
	ewl_widget_show( menu_item );
}

char *
tool_get_name( void )
{
	return active_tool;
}

void
tool_set_name( Ewl_Widget *w, void *ev_data, void *user_data )
{
	active_tool = ewl_button_get_label( EWL_BUTTON(w) );
}

void
tool_clear_name( void )
{
	active_tool = NULL;
}

void
ewler_tools_init( Ewl_Widget *main_layout )
{
	tool_tree = ewl_tree_new( 1 );
	ewl_object_set_fill_policy( EWL_OBJECT(tool_tree), EWL_FLAG_FILL_ALL );
	ewl_container_append_child( EWL_CONTAINER(main_layout), tool_tree );

	ewler_populate_tools();
	ewl_widget_show( tool_tree );
}

static void
add_tools_with_parent( const char *parent, Ewl_Widget *prow )
{
	Ecore_List *children = widgets_get_children( parent );
	char *class;

	ecore_list_goto_first( children );

	while( (class = ecore_list_remove( children )) ) {
		Ewl_Widget *row;
		Ewl_Widget *text[] = { NULL, NULL };

		if( widget_is_visible( class ) ) {
			if( widget_get_ctor( class ) ) {
				text[0] = ewl_button_new( class );
				ewl_callback_append( text[0], EWL_CALLBACK_CLICKED,
														 tool_set_name, NULL );
			} else
				text[0] = ewl_text_new( class );
			ewl_object_set_fill_policy( EWL_OBJECT(text[0]), EWL_FLAG_FILL_NONE );
			ewl_widget_show( text[0] );

			row = ewl_tree_add_row( EWL_TREE(tool_tree), EWL_ROW(prow), text );
			ewl_object_set_fill_policy( EWL_OBJECT(row->parent), EWL_FLAG_FILL_FILL );
			ewl_tree_set_row_expand( EWL_ROW(row), EWL_TREE_NODE_EXPANDED );

			add_tools_with_parent( class, row );
			ewl_widget_show( row );
		}
	}

	ecore_list_destroy( children );
}

void
ewler_populate_tools( void )
{
	char *tool_header[] = { "Tools", NULL };

	ewl_container_reset( EWL_CONTAINER(tool_tree) );
	ewl_tree_set_headers( EWL_TREE(tool_tree), tool_header );
	
	add_tools_with_parent( NULL, NULL );
}

void
ewler_error_dialog( const char *fmt, ... )
{
	Ewl_Widget *dialog, *text, *button;
	static char buf[1024];
	va_list ap;

	dialog = ewl_dialog_new(EWL_POSITION_BOTTOM);
	ewl_window_set_title( EWL_WINDOW(dialog), "Error" );
	ewl_callback_append( dialog, EWL_CALLBACK_DELETE_WINDOW,
											 __destroy_dialog, dialog );

	va_start( ap, fmt );
	vsnprintf( buf, 1023, fmt, ap );
	va_end( ap );

	text = ewl_text_new( buf );
	ewl_object_set_padding( EWL_OBJECT(text), 5, 5, 5, 5 );
	ewl_container_prepend_child( EWL_CONTAINER(EWL_DIALOG(dialog)->vbox), text );
	ewl_widget_show( text );

	button = ewl_dialog_add_button( EWL_DIALOG(dialog),
																	EWL_STOCK_OK, EWL_RESPONSE_OK );
	ewl_callback_append( button, EWL_CALLBACK_CLICKED,
											 __destroy_dialog, dialog );

	ewl_widget_show( dialog );
}

