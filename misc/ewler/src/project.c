/**
 * Project manager for ewler
 */

#include "ewler.h"

static Ewler_Project project;

static EWL_CALLBACK_DEFN(_ewler_project_file_save_cb);

static Ewler_Widget *
_read_widget( EXML *xml, Ewler_Project_File *form, Ewl_Widget *parent )
{
	Ewler_Widget *w;
	char *tag, *type;

	tag = exml_tag_get( xml );
	type = exml_attribute_get( xml, "type" );

	if( !strcmp( tag, "widget" ) ) {
		/* error msg and return */
	}

	w = ewler_widget_new( type, form );
	if( exml_down( xml ) ) {
		do {
			tag = exml_tag_get( xml );
			
			if( !strcmp( tag, "widget" ) )
				_read_widget( xml, form, w->ewl_w );
			else if( !strcmp( tag, "attr" ) )
				ewler_widget_attr_set_from_xml( w, w->attrs, xml );
		} while( exml_next_nomove( xml ) );
	}
	ewl_container_child_append( EWL_CONTAINER(parent), w->bg );
	ewl_widget_show( w->bg );

	return w;
}

static Ewler_Widget *
_exml_to_ewler( EXML *xml, Ewler_Project_File *form )
{
	exml_goto_top( xml );

	FORM(form).window = ewl_window_new();

	ewl_object_minimum_size_set( EWL_OBJECT(FORM(form).window), 800, 600 );
	ewl_widget_appearance_set( FORM(form).window, "background" );
	ewl_theme_data_str_set( FORM(form).window, "/background/file",
													PACKAGE_DATA_DIR"/themes/ewler.eet" );
	ewl_theme_data_str_set( FORM(form).window,
													"/background/group", "background" );
	ewl_callback_append( FORM(form).window, EWL_CALLBACK_FOCUS_IN,
											 ewler_form_focus_cb, form );
	ewl_callback_append( FORM(form).window, EWL_CALLBACK_DELETE_WINDOW,
											 ewler_form_close_cb, form );
	ewl_callback_append( FORM(form).window, EWL_CALLBACK_MOUSE_DOWN,
											 ewler_form_mouse_down_cb, form );
	ewl_widget_show( FORM(form).window );

	/* top is always a single widget */
	return _read_widget( xml, form, FORM(form).window );
}

void
ewler_project_init( void )
{
	project.filename = strdup( "untitled" );
	project.path = NULL;
	project.descr = strdup( "Untitled Project" );
	project.type = EWLER_PROJECT_BASIC;
	project.dirty = false;

	project.cflags = NULL;
	project.ldflags = NULL;

	project.files = ecore_list_new();
}

Ecore_List *
ewler_project_files_get( void )
{
	return project.files;
}

int
ewler_project_file_new( char *filename, int type )
{
	Ewler_Project_File *file;

	ecore_list_goto_first( project.files );

	while( (file = ecore_list_next( project.files )) )
		if( !strcmp( file->filename, filename ) )
			return false;

	file = NEW(Ewler_Project_File, 1);
	if( !file )
		return false;

	file->filename = strdup( filename );
	file->type = type;

	file->dirty = true;

	if( file->type == EWLER_PROJECT_FILE_FORM ) {
		Ewl_Widget *menu, *menu_item;

		FORM(file).includes = ecore_list_new();
		FORM(file).xml = exml_new();
		
		FORM(file).window = ewl_window_new();

		ewl_object_minimum_size_set( EWL_OBJECT(FORM(file).window), 800, 600 );
		ewl_widget_appearance_set( FORM(file).window, "background" );
		ewl_theme_data_str_set( FORM(file).window, "/background/file",
														PACKAGE_DATA_DIR"/themes/ewler.eet" );
		ewl_theme_data_str_set( FORM(file).window,
														"/background/group", "background" );
		ewl_callback_append( FORM(file).window, EWL_CALLBACK_FOCUS_IN,
												 ewler_form_focus_cb, file );
		ewl_callback_append( FORM(file).window, EWL_CALLBACK_DELETE_WINDOW,
												 ewler_form_close_cb, file );
		ewl_callback_append( FORM(file).window, EWL_CALLBACK_MOUSE_DOWN,
												 ewler_form_mouse_down_cb, file );
		ewl_widget_show( FORM(file).window );

		FORM(file).top = ewler_widget_new( "Ewl_Overlay", file );
		ewl_object_fill_policy_set( EWL_OBJECT(FORM(file).top->bg),
																EWL_FLAG_FILL_FILL );
		ewl_object_fill_policy_set( EWL_OBJECT(FORM(file).top->fg),
																EWL_FLAG_FILL_FILL );
		ewl_object_fill_policy_set( EWL_OBJECT(FORM(file).top->ewl_w),
																EWL_FLAG_FILL_FILL );
		ewl_container_child_append( EWL_CONTAINER(FORM(file).window),
																FORM(file).top->bg );
		ewl_callback_append( FORM(file).top->ewl_w, EWL_CALLBACK_MOUSE_MOVE,
												 ewler_form_mouse_move_cb, file );
		ewl_widget_show( FORM(file).top->bg );

#define MENU_ITEM_ADD(img, str, func, user_data) \
	menu_item = ewl_menu_item_new( img, str ); \
	ewl_container_child_append( EWL_CONTAINER(menu), menu_item ); \
	ewl_callback_append( menu_item, EWL_CALLBACK_SELECT, \
											 func, user_data ); \
	ewl_widget_show( menu_item );

		menu = ewl_imenu_new( NULL, file->filename );
		MENU_ITEM_ADD( NULL, "Save", _ewler_project_file_save_cb, file);
		MENU_ITEM_ADD( NULL, "Save As...", NULL, file);
		MENU_ITEM_ADD( NULL, "Close", NULL, file);
		ewl_container_child_append( EWL_CONTAINER(FORM(file).window), menu );

		FORM(file).menu = menu;

#undef MENU_ITEM_ADD

		ewler_form_active_set( file );
	}

	ecore_list_append( project.files, file );
	project.dirty = true;

	return true;
}

int
ewler_project_file_open( char *filename, int type )
{
	Ewler_Project_File *file;

	ecore_list_goto_first( project.files );

	while( (file = ecore_list_next( project.files )) )
		if( !strcmp( file->filename, filename ) )
			return false;

	file = NEW(Ewler_Project_File, 1);
	if( !file )
		return false;

	file->filename = strdup( filename );
	file->type = type;

	if( file->type == EWLER_PROJECT_FILE_FORM ) {
		FORM(file).xml = exml_new();

		exml_file_read( FORM(file).xml, file->filename );
		FORM(file).top = _exml_to_ewler( FORM(file).xml, file );
	}

	file->dirty = false;

	ecore_list_append( project.files, file );
	project.dirty = true;

	return true;
}

int
ewler_project_file_save( Ewler_Project_File *file )
{
	if( !file )
		return 0;

	switch( file->type ) {
		case EWLER_PROJECT_FILE_FORM: ewler_form_save( file ); break;
	}

	return 0;
}

int
ewler_project_file_del( char *filename )
{
	Ewler_Project_File *file;

	ecore_list_goto_first( project.files );

	while( (file = ecore_list_next( project.files )) )
		if( !strcmp( file->filename, filename ) ) {
			ecore_list_goto( project.files, file );
			ecore_list_remove_destroy( project.files );
			return true;
		}

	return false;
}

int
ewler_project_file_rename( char *old, char *filename )
{
	Ewler_Project_File *file;

	ecore_list_goto_first( project.files );

	while( (file = ecore_list_next( project.files )) )
		if( !strcmp( file->filename, old ) ) {
			FREE(file->filename);
			file->filename = strdup( filename  );
			return true;
		}

	return false;
}

static EWL_CALLBACK_DEFN(_ewler_project_file_save_cb)
{
	Ewler_Project_File *file;

	file = user_data;

	ewler_project_file_save( file );
}
