#include <Ewl.h>

#include "ewler.h"
#include "project.h"

static Ewl_Widget *project_win;
static Ewl_Widget *new_project_win;

static int project_dirty = 0;
static Ecore_Sheap *project_files;
static char *project_filename;
static Ewl_Widget *project_tree;
static int project_open = 0;

static void create_new_project( const char *filename );

static void
__destroy_window( Ewl_Widget *w, void *ev_data, void *user_data )
{
	if( user_data )
		ewl_widget_destroy( EWL_WIDGET(user_data) );
	else
		ewl_widget_destroy( w );
}

static void
__ok_new_project( Ewl_Widget *w, void *ev_data, void *user_data )
{
	FILE *fptr;
	char *filename = ewl_entry_get_text( EWL_ENTRY(user_data) );

	if( (fptr = fopen( filename, "r" )) ) {
		fclose( fptr );
		ewler_error_dialog( "File already exists" );
	} else {
		create_new_project( filename );

		ewl_widget_destroy( new_project_win );
	}
}

static void
create_new_project( const char *filename )
{
	FILE *fptr;
	char *headers[] = { NULL };

	fptr = fopen( filename, "w" );
	fclose( fptr );

	project_files = ecore_sheap_new( ecore_str_compare, 17 );
	project_filename = strdup( filename );
	project_win = ewl_window_new();
	ewl_window_set_title( EWL_WINDOW(project_win), "Project Overview" );
	/* add hide_project_win */
	ewl_object_set_minimum_size( EWL_OBJECT(project_win), 200, 400 );
	ewl_widget_show( project_win );

	project_tree = ewl_tree_new( 1 );

	headers[0] = project_filename;

	ewl_tree_set_headers( EWL_TREE(project_tree), headers );
	ewl_container_append_child( EWL_CONTAINER(project_win), project_tree );
	ewl_widget_show( project_tree );

	project_open = 1;
}

static void
__open_file_from_project( Ewl_Widget *e, void *ev_data, void *user_data )
{
	ewler_open_file( (char *) user_data );
}

static void
project_rebuild_tree( void )
{
	if( project_tree ) {
		int i;
		char *filename;

		ewl_container_reset( EWL_CONTAINER(project_tree) );

		for( i = 0; (filename = ecore_sheap_item( project_files, i )); i++ ) {
			Ewl_Widget *row;

			row = ewl_tree_add_text_row( EWL_TREE(project_tree), NULL, &filename );
			ewl_callback_del_type( row->parent, EWL_CALLBACK_CLICKED );
			ewl_callback_append( row, EWL_CALLBACK_CLICKED,
													 __open_file_from_project, filename );
			ewl_widget_show( row );
		}
	}
}

void
project_new_dialog( void )
{
	Ewl_Widget *entry_layout;
	Ewl_Widget *label, *entry, *button;

	new_project_win = ewl_dialog_new(EWL_POSITION_BOTTOM);
	ewl_window_set_title( EWL_WINDOW(new_project_win), "New Project" );
	ewl_callback_append( new_project_win, EWL_CALLBACK_DELETE_WINDOW,
											 __destroy_window, NULL );

	entry_layout = ewl_hbox_new();
	ewl_container_prepend_child( EWL_CONTAINER(EWL_DIALOG(new_project_win)->vbox),
															 entry_layout );
	ewl_object_set_insets( EWL_OBJECT(entry_layout), 20, 20, 10, 10 );
	ewl_widget_show( entry_layout );

	label = ewl_text_new( "Project File" );
	ewl_container_append_child( EWL_CONTAINER(entry_layout), label );
	ewl_object_set_alignment( EWL_OBJECT(label), EWL_FLAG_ALIGN_BOTTOM );
	ewl_object_set_padding( EWL_OBJECT(label), 0, 0, 0, 3 );
	ewl_widget_show( label );

	entry = ewl_entry_new( "unnamed.pro" );
	ewl_container_append_child( EWL_CONTAINER(entry_layout), entry );
	ewl_object_set_alignment( EWL_OBJECT(entry), EWL_FLAG_ALIGN_BOTTOM );
	ewl_callback_append( entry, EWL_CALLBACK_VALUE_CHANGED,
											 __ok_new_project, entry );
	ewl_widget_show( entry );

	button = ewl_dialog_add_button( EWL_DIALOG(new_project_win),
																	EWL_STOCK_OK, EWL_RESPONSE_OK );
	ewl_callback_append( button, EWL_CALLBACK_CLICKED, __ok_new_project, entry );

  button = ewl_dialog_add_button( EWL_DIALOG(new_project_win),
																	EWL_STOCK_CANCEL, EWL_RESPONSE_CANCEL);
	ewl_callback_append( button, EWL_CALLBACK_CLICKED,
											 __destroy_window, new_project_win );

	ewl_widget_show( new_project_win );
}

static void
open_project( char *filename )
{
	FILE *fptr;
	char *headers[] = { NULL };
	static char type[64], prj_file[256];

	if( (fptr = fopen( filename, "r" )) == NULL ) {
		ewler_error_dialog( "File '%s' cannot be opened.", filename );
		return;
	}

	/* make sure that the current project is closed */

	project_files = ecore_sheap_new( ecore_str_compare, 17 );
	project_tree = ewl_tree_new( 1 );

	while( !feof( fptr ) ) {
		fscanf( fptr, "%s %s\n", type, prj_file );
		if( !strcmp( type, "form" ) )
			project_add_form( strdup( prj_file ) );
	}

	fclose( fptr );

	project_filename = strdup( filename );
	project_win = ewl_window_new();
	ewl_window_set_title( EWL_WINDOW(project_win), "Project Overview" );
	/* add hide_project_win */
	ewl_object_set_minimum_size( EWL_OBJECT(project_win), 200, 400 );
	ewl_widget_show( project_win );

	headers[0] = project_filename;

	ewl_tree_set_headers( EWL_TREE(project_tree), headers );
	ewl_container_append_child( EWL_CONTAINER(project_win), project_tree );
	ewl_widget_show( project_tree );

	project_dirty = 0;
}

static void
__open_project_cb( Ewl_Widget *w, void *ev_data, void *user_data )
{
	FILE *fptr;
	char *filename = ev_data;

	if( filename ) {
		/* ok clicked */
		if( (fptr = fopen( filename, "r" )) ) {
			fclose( fptr );
			open_project( filename );

			ewl_widget_destroy( w );
		} else {
			ewler_error_dialog( "File '%s' cannot be opened.", filename );
		}
	} else {
		/* cancelled */
		ewl_widget_destroy( w );
	}
}

void
project_open_dialog( void )
{
	Ewl_Widget *window, *dialog;

	window = ewl_window_new();
	ewl_widget_show( window );

	dialog = ewl_filedialog_new( EWL_FILEDIALOG_TYPE_OPEN );
	ewl_container_append_child( EWL_CONTAINER(window), dialog );
	ewl_callback_append( dialog, EWL_CALLBACK_VALUE_CHANGED,
											 __open_project_cb, NULL );
	ewl_callback_append( window, EWL_CALLBACK_DELETE_WINDOW,
											 __destroy_window, NULL );
	ewl_widget_show( dialog );
}

int
project_add_form( char *filename )
{
	if( project_files ) {
		ecore_sheap_insert( project_files, filename );
		ecore_sheap_sort( project_files );

		project_rebuild_tree();
		project_dirty = 1;
		return 0;
	}

	return -1;
}
