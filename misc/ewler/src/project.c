#include <Ewl.h>
#include <sys/time.h>
#include <time.h>

#include "form.h"
#include "ewler.h"
#include "project.h"

static Ewl_Widget *project_win;
static Ewl_Widget *project_tree;

static Ewler_Project *project;

static Ewl_Widget *options_dialog;
static Ewl_Widget *params[3];

static void
__projects_destroy_cb( Ewl_Widget *w, void *ev_data, void *user_data )
{
	projects_toggle();
}

static void
__project_new_cb( Ewl_Widget *w, void *ev_data, void *user_data )
{
	char *headers[2] = { NULL, NULL };

	IF_FREE(project->name);
	IF_FREE(project->filename);
	IF_FREE(project->path);
	
	if( project->members ) {
		ecore_list_set_free_cb( project->members, free );
		ecore_list_destroy( project->members );
	}

	project->name = strdup( "Untitled" );
	project->filename = NULL;
	project->path = NULL;
	project->members = ecore_list_new();
	ecore_list_set_free_cb( project->members, free );

	project->dirty = 0;
	project->filename_set = 0;

	headers[0] = project->name;

	ewl_container_reset( EWL_CONTAINER(project_tree) );
	ewl_tree_set_headers( EWL_TREE(project_tree), headers );
}

static void
__project_member_mouse_down( Ewl_Widget *w, void *ev_data, void *user_data )
{
	Ewl_Widget *text, *cell;
	char *filename;
	Ewl_Event_Mouse_Down *ev = ev_data;
	static struct timeval last = { 0, 0 };
	struct timeval now;
	int msec_diff;

	gettimeofday( &now, NULL );

	cell = ewl_row_get_column( EWL_ROW(w), 0 );

	text = ecore_list_goto_first( EWL_CONTAINER(cell)->children );

	filename = ewl_text_text_get( EWL_TEXT(text) );

	if( ev->button == 1 ) {
		msec_diff = (now.tv_sec - last.tv_sec) * 1000 +
								(now.tv_usec - last.tv_usec) / 1000;

		if( msec_diff < 250 ) {
			static char buf[1024];

			if( *filename == '/' ) {
				if( !form_is_open( filename ) )
					form_open_file( filename );
			} else {
				sprintf( buf, "%s/%s", project->path, filename );
				if( !form_is_open( buf ) )
					form_open_file( buf );
			}
		}

		last = now;
	}
}

void
projects_toggle( void )
{
	if( VISIBLE(project_win) )
		ewl_widget_hide( project_win );
	else
		ewl_widget_show( project_win );
}

void
ewler_projects_init( void )
{
	project_win = ewl_window_new();

	ewl_window_set_title( EWL_WINDOW(project_win), "Project Overview" );
	ewl_object_set_minimum_size( EWL_OBJECT(project_win), 300, 200 );
	ewl_object_set_insets( EWL_OBJECT(project_tree), 5, 5, 5, 5 );

	ewl_callback_append( project_win, EWL_CALLBACK_DELETE_WINDOW,
											 __projects_destroy_cb, NULL );

	project = NEW(Ewler_Project, 1);

	project_tree = ewl_tree_new( 1 );
	ewl_container_append_child( EWL_CONTAINER(project_win), project_tree );

	__project_new_cb( NULL, NULL, NULL );

	ewl_widget_show( project_tree );
}

static void
project_update( void )
{
	char *headers[2] = { NULL, NULL };
	Ecore_Sheap *sorted_list;
	char *filename;
	int i;

	headers[0] = project->name;

	ewl_container_reset( EWL_CONTAINER(project_tree) );
	ewl_tree_set_headers( EWL_TREE(project_tree), headers );

	sorted_list = ecore_sheap_new( ecore_str_compare,
																 ecore_list_nodes( project->members ) );

	ecore_list_goto_first( project->members );

	while( (filename = ecore_list_next( project->members )) )
		ecore_sheap_insert( sorted_list, filename );

	ecore_sheap_sort( sorted_list );

	for( i=0;i<sorted_list->size;i++ ) {
		Ewl_Widget *text, *row;

		filename = ecore_sheap_item( sorted_list, i );

		if( filename ) {
			text = ewl_text_new( filename );
			ewl_widget_show( text );

			row = ewl_tree_add_row( EWL_TREE(project_tree), NULL, &text );
			ewl_callback_append( row, EWL_CALLBACK_MOUSE_DOWN,
													 __project_member_mouse_down, NULL );
			ewl_widget_show( row );
		}
	}

	ecore_sheap_destroy( sorted_list );
}

void
project_new( void )
{
	ewler_yesno_dialog( __project_new_cb, NULL, NULL,
											"This will destroy your current project, are you sure?" );
}

void
project_add_file( char *filename )
{
	ecore_list_append( project->members, strdup(filename) );

	project_update();
}

char *
project_set_file( char *old, char *filename )
{
	char *old_name = NULL;

	if( project->path )
		if( !strncmp( project->path, filename, strlen( project->path ) ) ) {
			filename += strlen( project->path );
			if( filename[0] == '/' )
				filename++;
		}

	ecore_list_goto_first( project->members );

	while( (old_name = ecore_list_current( project->members )) ) {
		if( !strcmp( old, old_name ) )
			break;
		ecore_list_next( project->members );
	}

	ecore_list_remove_destroy( project->members );

	ecore_list_append( project->members, strdup( filename ) );

	project_update();

	return filename;
}

void
project_remove_file( char *filename )
{
	char *old_name = NULL;
	
	if( project->path )
		if( !strncmp( project->path, filename, strlen( project->path ) ) ) {
			filename += strlen( project->path );
			while( *filename == '/' )
				filename++;
		}

	ecore_list_goto_first( project->members );

	while( (old_name = ecore_list_current( project->members )) ) {
		if( !strcmp( filename, old_name ) )
			break;
		ecore_list_next( project->members );
	}

	ecore_list_remove_destroy( project->members );

	project_update();
}

char *
project_get_path( void )
{
	return project->path;
}

static void
__apply_changes( Ewl_Widget *w, void *ev_data, void *user_data )
{
	IF_FREE(project->name);
	IF_FREE(project->filename);
	IF_FREE(project->path);

	project->name = ewl_entry_get_text( EWL_ENTRY(params[0]) );
	project->path = ewl_entry_get_text( EWL_ENTRY(params[1]) );
	if( strlen( project->path ) == 0 )
		project->path = NULL;
	project->filename = ewl_entry_get_text( EWL_ENTRY(params[2]) );
	if( strlen( project->filename ) == 0 )
		project->filename = NULL;

	project->filename_set = 1;

	project_update();
}

void
project_options_dialog( void )
{
	Ewl_Widget *label;
	Ewl_Widget *vbox, *hbox;
	Ewl_Widget *button;

	if( options_dialog && VISIBLE(options_dialog) )
		return;

	options_dialog = ewl_dialog_new(EWL_POSITION_BOTTOM);
	ewl_object_set_minimum_w( EWL_OBJECT(options_dialog), 400 );
	ewl_window_set_title( EWL_WINDOW(options_dialog), "Project Options" );
	ewl_callback_append( options_dialog, EWL_CALLBACK_DELETE_WINDOW,
											 __destroy_dialog, options_dialog );

	vbox = ewl_vbox_new();
	ewl_container_prepend_child( EWL_CONTAINER(EWL_DIALOG(options_dialog)->vbox),
															 vbox );
	ewl_widget_show( vbox );

	hbox = ewl_hbox_new();
	ewl_container_append_child( EWL_CONTAINER(vbox), hbox );
	ewl_widget_show( hbox );
	label = ewl_text_new( "Project Name:" );
	params[0] = ewl_entry_new( project->name ? project->name : "" );
	ewl_container_append_child( EWL_CONTAINER(hbox), label );
	ewl_container_append_child( EWL_CONTAINER(hbox), params[0] );
	ewl_widget_show( label );
	ewl_widget_show( params[0] );

	hbox = ewl_hbox_new();
	ewl_container_append_child( EWL_CONTAINER(vbox), hbox );
	ewl_widget_show( hbox );
	label = ewl_text_new( "Project Directory:" );
	params[1] = ewl_entry_new( project->path ? project->path : "" );
	ewl_container_append_child( EWL_CONTAINER(hbox), label );
	ewl_container_append_child( EWL_CONTAINER(hbox), params[1] );
	ewl_widget_show( label );
	ewl_widget_show( params[1] );

	hbox = ewl_hbox_new();
	ewl_container_append_child( EWL_CONTAINER(vbox), hbox );
	ewl_widget_show( hbox );
	label = ewl_text_new( "Project File:" );
	params[2] = ewl_entry_new( project->filename ? project->filename : "" );
	ewl_container_append_child( EWL_CONTAINER(hbox), label );
	ewl_container_append_child( EWL_CONTAINER(hbox), params[2] );
	ewl_widget_show( label );
	ewl_widget_show( params[2] );

	button = ewl_dialog_add_button( EWL_DIALOG(options_dialog),
																	EWL_STOCK_OK, EWL_RESPONSE_OK );
	ewl_callback_append( button, EWL_CALLBACK_CLICKED,
											 __apply_changes, NULL );
	ewl_callback_append( button, EWL_CALLBACK_CLICKED,
											 __destroy_dialog, options_dialog );

	button = ewl_dialog_add_button( EWL_DIALOG(options_dialog),
																	EWL_STOCK_CANCEL, EWL_RESPONSE_CANCEL );
	ewl_callback_append( button, EWL_CALLBACK_CLICKED,
											 __destroy_dialog, options_dialog );

	ewl_widget_show( options_dialog );
}
