/**
 * Primary ui setup for ewler, includes toolbar, project view, modules view,
 * inspector, and callback config/editor
 */
#include "ewler.h"

Ewl_Widget *project_tree;

static int new_file_type = -1;

static EWL_CALLBACK_DEFN(_ewler_new_file_close)
{
	Ewl_Widget **new_file_win = user_data;

	ewl_widget_destroy( *new_file_win );
	*new_file_win = NULL;
}

static EWL_CALLBACK_DEFN(_ewler_close)
{
	ewl_main_quit();
}

static EWL_CALLBACK_DEFN(_ewler_toolbar)
{
	ewler_toolbar_toggle();
}

static EWL_CALLBACK_DEFN(_ewler_inspector)
{
	ewler_inspector_toggle();
}

static EWL_CALLBACK_DEFN(_ewler_new_file_ok)
{
	static int count = 0;
	Ewl_Widget **new_file_win = user_data;
	char *name = "untitled";
	char buf[12];
	char *ext;

	switch( new_file_type ) {
		case EWLER_PROJECT_FILE_FORM: ext = "ui"; break;
		default: /* unknown file type */ return;
	}

	if( count )
		sprintf( buf, "%s%d.%s", name, count, ext );
	else
		sprintf( buf, "%s.%s", name, ext );

	ewler_project_file_new( buf, new_file_type );

	ewl_widget_destroy( *new_file_win );
	*new_file_win = NULL;

	ewler_ui_project_update();
}

static EWL_CALLBACK_DEFN(_new_file_toggle_select)
{
	new_file_type = (int) user_data;
}

static EWL_CALLBACK_DEFN(_ewler_new_file)
{
	static Ewl_Widget *new_file_win;
	Ewl_Widget *button, *vbox, *scroll;
	Ewl_Widget *image, *text;

	if( new_file_win == NULL ) {
		new_file_type = -1;

		new_file_win = ewl_dialog_new( EWL_POSITION_BOTTOM );
		ewl_window_title_set( EWL_WINDOW(new_file_win), "New File" );
		ewl_object_preferred_inner_size_set( EWL_OBJECT(new_file_win), 400, 300 );
		ewl_callback_append( new_file_win, EWL_CALLBACK_DELETE_WINDOW,
												 _ewler_new_file_close, &new_file_win );
		ewl_widget_show( new_file_win );

		vbox = ewl_vbox_new();
		ewl_dialog_widget_add( EWL_DIALOG(new_file_win), vbox );
		ewl_widget_show( vbox );

		scroll = ewl_scrollpane_new();
		ewl_container_child_append( EWL_CONTAINER(vbox), scroll );
		ewl_widget_show( scroll );

		vbox = ewl_vbox_new();
		ewl_container_child_append( EWL_CONTAINER(scroll), vbox );
		ewl_object_fill_policy_set( EWL_OBJECT(vbox), EWL_FLAG_FILL_NONE );
		ewl_callback_append( vbox, EWL_CALLBACK_MOUSE_DOWN,
												 _new_file_toggle_select, EWLER_PROJECT_FILE_FORM );
		ewl_widget_show( vbox );
		
		image = ewl_image_new( PACKAGE_IMAGE_DIR"/form.png", "form" );
		ewl_container_child_append( EWL_CONTAINER(vbox), image );
		ewl_widget_show( image );

		text = ewl_text_new( "Form" );
		ewl_container_child_append( EWL_CONTAINER(vbox), text );
		ewl_object_alignment_set( EWL_OBJECT(text), EWL_FLAG_ALIGN_CENTER );
		ewl_widget_show( text );

		button = ewl_dialog_button_add( EWL_DIALOG(new_file_win),
																		EWL_STOCK_OK, EWL_RESPONSE_OK );
		ewl_callback_append( button, EWL_CALLBACK_CLICKED,
												 _ewler_new_file_ok, &new_file_win );

		button = ewl_dialog_button_add( EWL_DIALOG(new_file_win),
																		EWL_STOCK_CANCEL, EWL_RESPONSE_CANCEL );
		ewl_callback_append( button, EWL_CALLBACK_CLICKED,
												 _ewler_new_file_close, &new_file_win );
	}
}

void
ewler_ui_init( void )
{
	Ewl_Widget *menubar, *menu, *menu_item, *win, *vbox;

	char *project_header[] = { "Untitled", NULL };

#define MENU_ITEM_ADD(img, str, func, user_data) \
	menu_item = ewl_menu_item_new( img, str ); \
	ewl_container_child_append( EWL_CONTAINER(menu), menu_item ); \
	ewl_callback_append( menu_item, EWL_CALLBACK_SELECT, \
											 func, user_data ); \
	ewl_widget_show( menu_item )

	win = ewl_window_new();

	ewl_window_title_set( EWL_WINDOW(win), "Ewler" );
	ewl_object_preferred_inner_size_set( EWL_OBJECT(win), 300, 400 );
	ewl_callback_append( win, EWL_CALLBACK_DELETE_WINDOW,
											 _ewler_close, NULL );
	ewl_widget_show( win );

	vbox = ewl_vbox_new();
	ewl_container_child_append( EWL_CONTAINER(win), vbox );
	ewl_widget_show( vbox );

	menubar = ewl_menubar_new( EWL_ORIENTATION_HORIZONTAL );
	ewl_container_child_append( EWL_CONTAINER(vbox), menubar );
	ewl_widget_show( menubar );

	menu = ewl_menubar_menu_add( EWL_MENUBAR(menubar), NULL, "File" );

	MENU_ITEM_ADD(NULL, "New", _ewler_new_file, NULL);
	MENU_ITEM_ADD(NULL, "Quit", _ewler_close, NULL);

	menu = ewl_menubar_menu_add( EWL_MENUBAR(menubar), NULL, "Window" );

	MENU_ITEM_ADD(NULL, "Toolbar", _ewler_toolbar, NULL);
	MENU_ITEM_ADD(NULL, "Inspector", _ewler_inspector, NULL);

#undef MENU_ITEM_ADD

	project_tree = ewl_tree_new( 1 );
	ewl_tree_headers_set( EWL_TREE(project_tree), project_header );
	ewl_container_child_append( EWL_CONTAINER(vbox), project_tree );
	ewl_widget_show( project_tree );

	ewler_toolbar_init();
	ewler_inspector_init();
}

void
ewler_ui_project_update( void )
{
	Ecore_List *files;
	Ewler_Project_File *file;

	files = ewler_project_files_get();

	ewl_container_reset( EWL_CONTAINER(project_tree) );

	ecore_list_goto_first( files );

	while( (file = ecore_list_next( files )) )
		ewl_tree_text_row_add( EWL_TREE(project_tree), NULL, &file->filename );
}
