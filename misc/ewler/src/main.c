#include <Ewl.h>

#include "ewler.h"
#include "form.h"
#include "widgets.h"
#include "inspector.h"

static Ewl_Widget *main_win;
static Ewl_Widget *main_layout;

void
__destroy_main_window( Ewl_Widget *w, void *ev_data, void *user_data )
{
	ewl_widget_destroy( main_win );

	ewler_forms_close();
	ewl_main_quit();
}

int
main( int argc, char *argv[] )
{
	ewl_init( &argc, argv );

	widgets_load( PACKAGE_DATA_DIR"/schemas/widgets.xml" );

	main_win = ewl_window_new();
	ewl_window_set_title( EWL_WINDOW(main_win), "Ewler" );
	ewl_callback_append( main_win, EWL_CALLBACK_DELETE_WINDOW,
											 __destroy_main_window, NULL );
	ewl_object_set_minimum_size( EWL_OBJECT(main_win), 300, 400 );
	ewl_object_set_maximum_size( EWL_OBJECT(main_win), 300, 400 );
	ewl_widget_show( main_win );

	main_layout = ewl_vbox_new();
	ewl_container_append_child( EWL_CONTAINER(main_win), main_layout );
	ewl_widget_show( main_layout );

	ewler_menu_init( main_layout );
	ewler_tools_init( main_layout );
	ewler_forms_init();
	ewler_inspector_init();

	ewl_main();

	return 0;
}
