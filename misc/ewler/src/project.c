#include <Ewl.h>
#include <errno.h>
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

static int
project_close( void )
{
	static char buf[1024];
	char *filename;

	if( project->members ) {
		ecore_list_goto_first( project->members );

		while( (filename = ecore_list_next( project->members )) ) {
			if( *filename != '/' )
				sprintf( buf, "%s/%s", project->path, filename );
			else
				strcpy( buf, project->filename );

			if( form_is_open( buf ) && form_is_dirty( buf ) )
				return -1;
		}
	}

	IF_FREE(project->path);
	IF_FREE(project->filename);

	if( project->members ) {
		ecore_list_goto_first( project->members );

		while( (filename = ecore_list_remove( project->members )) ) {
			if( *filename != '/' )
				sprintf( buf, "%s/%s", project->path, project->filename );
			else
				strcpy( buf, project->filename );

			form_close( buf );
			free( filename );
		}

		ecore_list_destroy( project->members );
		project->members = NULL;
	}

	return 0;
}

static void
__projects_destroy_cb( Ewl_Widget *w, void *ev_data, void *user_data )
{
	projects_toggle();
}

static void
__project_new_cb( Ewl_Widget *w, void *ev_data, void *user_data )
{
	char *headers[2] = { NULL, NULL };
	static char buf[256];

	if( !project_close() ) {
		project->filename = strdup( "untitled.ewl" );
		sprintf( buf, "%s/Untitled", getenv( "HOME" ) );
		project->path = strdup( buf );
		project->members = ecore_list_new();
		ecore_list_set_free_cb( project->members, free );

		project->dirty = 0;
		project->filename_set = 0;

		headers[0] = project->filename;
		headers[1] = "";

		ewl_container_reset( EWL_CONTAINER(project_tree) );
		ewl_tree_headers_set( EWL_TREE(project_tree), headers );
	} else {
		ewler_error_dialog( "The current project has modified forms, a new "
												"project cannot be created now." );
	}
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

	cell = ewl_row_column_get( EWL_ROW(w), 1 );

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

	ewl_window_title_set( EWL_WINDOW(project_win), "Project Overview" );
	ewl_object_minimum_size_set( EWL_OBJECT(project_win), 300, 200 );
	ewl_object_insets_set( EWL_OBJECT(project_tree), 5, 5, 5, 5 );

	ewl_callback_append( project_win, EWL_CALLBACK_DELETE_WINDOW,
											 __projects_destroy_cb, NULL );

	project = NEW(Ewler_Project, 1);

	project_tree = ewl_tree_new( 2 );
	ewl_container_child_append( EWL_CONTAINER(project_win), project_tree );

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

	headers[0] = project->filename;
	headers[1] = "";

	ewl_container_reset( EWL_CONTAINER(project_tree) );
	ewl_tree_headers_set( EWL_TREE(project_tree), headers );

	sorted_list = ecore_sheap_new( ecore_str_compare,
																 ecore_list_nodes( project->members ) );

	ecore_list_goto_first( project->members );

	while( (filename = ecore_list_next( project->members )) )
		ecore_sheap_insert( sorted_list, filename );

	ecore_sheap_sort( sorted_list );

	for( i=0;i<sorted_list->size;i++ ) {
		Ewl_Widget *text, *image, *row;

		filename = ecore_sheap_item( sorted_list, i );

		if( filename ) {
			Ewl_Widget *row_widgets[2] = { NULL, NULL };
			char *ext;

			ext = strrchr( filename, '.' ) + 1;

			text = ewl_text_new( filename );
			ewl_object_alignment_set( EWL_OBJECT(text), EWL_FLAG_ALIGN_LEFT );
			ewl_widget_show( text );

			if( !strcmp( ext, "ui" ) )
				image = ewl_image_new( PACKAGE_DATA_DIR"/images/form-small.png",
															 NULL );
			else if( !strcmp( ext, "c" ) || !strcmp( ext, "h" ) )
				image = ewl_image_new( PACKAGE_DATA_DIR"/images/source-small.png",
															 NULL );
			else
				image = ewl_text_new( "" );
			ewl_object_fill_policy_set( EWL_OBJECT(image), EWL_FLAG_FILL_NONE );
			ewl_object_alignment_set( EWL_OBJECT(image), EWL_FLAG_ALIGN_RIGHT );
			ewl_widget_show( image );

			row_widgets[0] = image;
			row_widgets[1] = text;

			row = ewl_tree_row_add( EWL_TREE(project_tree), NULL, row_widgets );
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

void
__create_path( Ewl_Widget *w, void *ev_data, void *user_data )
{
	char *path, *errstr = NULL;

	path = ewl_entry_text_get( EWL_ENTRY(params[1]) );

	mkdir( path, 0777 );
	switch( errno ) {
		case EPERM: errstr = "%s: cannot create directory on filesystem"; break;
		case EEXIST: errstr = "%s: file exists"; break;
		case EFAULT: errstr = "%s: bad address"; break;
		case EACCES: errstr = "%s: permission denied"; break;
		case ENAMETOOLONG: errstr = "%s: directory does not exist"; break;
		case ENOENT: errstr = "%s: out of memory"; break;
		case EROFS: errstr = "%s: read-only filesystem"; break;
		case ENOSPC: errstr = "%s: filesystem full"; break;
	}

	if( errstr )
		ewler_error_dialog( errstr, path );
}

static void
__apply_changes( Ewl_Widget *w, void *ev_data, void *user_data )
{
	char *name, *path, *filename;
	int delay = 0;

	name = ewl_entry_text_get( EWL_ENTRY(params[0]) );
	path = ewl_entry_text_get( EWL_ENTRY(params[1]) );
	if( strlen( path ) == 0 )
		path = NULL;
	filename = ewl_entry_text_get( EWL_ENTRY(params[2]) );
	if( strlen( filename ) == 0 )
		filename = NULL;

	if( path ) {
		struct stat st_buf;
		char *errstr = "%s: unknown error";

		if( stat( path, &st_buf ) < 0 ) {
			switch( errno ) {
				case ENOENT:
					ewler_yesno_dialog( __create_path, NULL, NULL,
															"Project directory does not exist, create it?" );
					break;
				case ENOTDIR: errstr = "%s: no such directory in path"; break;
				case ELOOP: errstr = "%s: too many links"; break;
				case EFAULT: errstr = "%s: bad address"; break;
				case EACCES: errstr = "%s: permission denied"; break;
				case ENOMEM: errstr = "%s: out of memory"; break;
				case ENAMETOOLONG: errstr = "%s: filename too long"; break;
			}
			if( errno != ENOENT )
				ewler_error_dialog( errstr, path );
			delay = 1;
		} else if( !S_ISDIR(st_buf.st_mode) ) {
			ewler_error_dialog( "%s: is not a directory", path );
			delay = 1;
		} else {
			IF_FREE(project->path);
			project->path = ewl_entry_text_get( EWL_ENTRY(params[1]) );
		}
	}

	IF_FREE(project->filename);

	project->filename = ewl_entry_text_get( EWL_ENTRY(params[2]) );
	if( strlen( project->filename ) == 0 )
		project->filename = NULL;

	project->filename_set = 1;

	if( !delay ) {
		__destroy_dialog(options_dialog, NULL, options_dialog );
		ewl_callback_call( options_dialog, EWL_CALLBACK_VALUE_CHANGED );
	}

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
	ewl_object_minimum_w_set( EWL_OBJECT(options_dialog), 400 );
	ewl_window_title_set( EWL_WINDOW(options_dialog), "Project Options" );
	ewl_callback_append( options_dialog, EWL_CALLBACK_DELETE_WINDOW,
											 __destroy_dialog, options_dialog );

	vbox = ewl_vbox_new();
	ewl_container_child_prepend( EWL_CONTAINER(EWL_DIALOG(options_dialog)->vbox),
															 vbox );
	ewl_widget_show( vbox );

	hbox = ewl_hbox_new();
	ewl_container_child_append( EWL_CONTAINER(vbox), hbox );
	ewl_widget_show( hbox );
	label = ewl_text_new( "Project Directory:" );
	params[1] = ewl_entry_new( project->path ? project->path : "" );
	ewl_container_child_append( EWL_CONTAINER(hbox), label );
	ewl_container_child_append( EWL_CONTAINER(hbox), params[1] );
	ewl_widget_show( label );
	ewl_widget_show( params[1] );

	hbox = ewl_hbox_new();
	ewl_container_child_append( EWL_CONTAINER(vbox), hbox );
	ewl_widget_show( hbox );
	label = ewl_text_new( "Project File:" );
	params[2] = ewl_entry_new( project->filename ? project->filename : "" );
	ewl_container_child_append( EWL_CONTAINER(hbox), label );
	ewl_container_child_append( EWL_CONTAINER(hbox), params[2] );
	ewl_widget_show( label );
	ewl_widget_show( params[2] );

	button = ewl_dialog_button_add( EWL_DIALOG(options_dialog),
																	EWL_STOCK_OK, EWL_RESPONSE_OK );
	ewl_callback_append( button, EWL_CALLBACK_CLICKED,
											 __apply_changes, NULL );

	button = ewl_dialog_button_add( EWL_DIALOG(options_dialog),
																	EWL_STOCK_CANCEL, EWL_RESPONSE_CANCEL );
	ewl_callback_append( button, EWL_CALLBACK_CLICKED,
											 __destroy_dialog, options_dialog );

	ewl_widget_show( options_dialog );
}

static void
__project_save_cb( Ewl_Widget *w, void *ev_data, void *user_data )
{
	char *member, *filename = user_data;
	FILE *fptr;

	fptr = fopen( filename, "w" );

	if( fptr ) {
		fprintf( fptr, "FORMS = " );

		ecore_list_goto_first( project->members );

		while( (member = ecore_list_next( project->members )) ) {
			fprintf( fptr, "%s", member );

			if( ecore_list_current( project->members ) )
				fprintf( fptr, " \\\n\t" );
		}

		fclose( fptr );
	} else {
		ewler_error_dialog( "Unable to open project file %s", filename );
	}
}

static void
__project_setup_save_cb( Ewl_Widget *w, void *ev_data, void *user_data )
{
	static char buf[1024];
	struct stat st_buf;

	sprintf( buf, "%s/%s", project->path, project->filename );

	if( stat( buf, &st_buf ) < 0 ) {
		ewler_yesno_dialog( __project_save_cb, NULL, buf,
												"Project file %s does not exist, create it?", buf );
	} else
		__project_save_cb( NULL, NULL, buf );
}

void
project_save( void )
{
	if( !project->filename_set ) {
		project_options_dialog();

		ewl_callback_append( options_dialog, EWL_CALLBACK_VALUE_CHANGED,
												 __project_setup_save_cb, NULL );
	} else
		__project_setup_save_cb( NULL, NULL, NULL );
}

static void
project_setup( char *pathname )
{
	char *filename;
	static char path[256];
	int pathlen;

	filename = strrchr( pathname, '/' ) + 1;
	pathlen = filename - pathname - 1;
	strncpy( path, pathname, pathlen );
	path[pathlen] = '\0';

	project->filename = strdup( filename );
	project->path = strdup( path );

	project->members = ecore_list_new();

	project->dirty = 0;
	project->filename_set = 1;
}

static void
project_parse( FILE *fptr )
{
	static char buf[256];
	int had_equals, item_done;
	int word_len;
	char *ptr;

	while( !feof( fptr ) ) {
		if( !fgets( buf, 255, fptr ) )
			break;

		ptr = buf;
		had_equals = 0;

		if( !strncmp( buf, "FORMS", strlen( "FORMS" ) ) ) {
			static char file_buf[256];

			ptr += strlen( "FORMS" );
			while( !had_equals )
				if( *ptr++ == '=' )
					had_equals = 1;

			item_done = 0;

			while( !item_done ) {
				while( *ptr == ' ' || *ptr == '\t' )
					*ptr++;

				if( *ptr && *ptr != '\\' && *ptr != '\n' ) {
					word_len = strcspn( ptr, " \t\\\n" );
					strncpy( file_buf, ptr, word_len );
					ptr += word_len;
					
					ecore_list_append( project->members, strdup( file_buf ) );
				} else if( *ptr == '\\' ) {
					if( !fgets( buf, 255, fptr ) )
						break;
					ptr = buf;
				} else
					item_done = 1;
			}
		}
	}
}

static void
__project_open_cb( Ewl_Widget *w, void *ev_data, void *user_data )
{
	FILE *fptr;
	char *filename = ev_data;

	if( !ev_data ) {
		ewl_callback_call( w, EWL_CALLBACK_DELETE_WINDOW );
		return;
	}

	if( !(fptr = fopen( filename, "r" )) ) {
		ewler_error_dialog( "Unable to open file: %s", filename );
		return;
	}

	__destroy_dialog( w, NULL, w->parent );

	project_setup( filename );
	project_parse( fptr );

	fclose( fptr );

	project_update();
}

static void
__project_setup_open_cb( Ewl_Widget *w, void *ev_data, void *user_data )
{
	Ewl_Widget *dialog, *window;

	if( !project_close() ) {
		window = ewl_window_new();
		ewl_window_title_set( EWL_WINDOW(window), "Open New Project" );
		ewl_object_minimum_size_set( EWL_OBJECT(window), 400, 600 );
		ewl_widget_show( window );

		dialog = ewl_filedialog_new( EWL_FILEDIALOG_TYPE_OPEN );

		ewl_container_child_append( EWL_CONTAINER(window), dialog );
		ewl_callback_append( dialog, EWL_CALLBACK_VALUE_CHANGED,
												 __project_open_cb, NULL );
		ewl_callback_append( dialog, EWL_CALLBACK_DELETE_WINDOW,
												 __destroy_dialog, window );
		ewl_widget_show( dialog );
	} else {
		ewler_error_dialog( "The current project has modified forms, a new "
												"project cannot be opened now." );
	}
}

void
project_open( void )
{
	if( project->dirty ) {
		ewler_yesno_dialog( __project_setup_open_cb, NULL, NULL,
												"Current project has not been saved, do you wish "
												"to continue?" );
	} else {
		__project_setup_open_cb( NULL, NULL, NULL );
	}
}
