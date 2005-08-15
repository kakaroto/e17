#include <Ewl.h>
#include <stdarg.h>

#include "ewler.h"

static EWL_CALLBACK_DEFN(err_dialog_cb)
{
	Ewl_Widget *dialog = user_data;

	ewl_widget_destroy(dialog);
}

void
ewler_error( char *fmt, ... )
{
	Ewl_Widget *err_dialog, *err_text, *err_button;
	char buf[256];
	va_list ap;

	va_start(ap, fmt);
	
	vsnprintf(buf, 255, fmt, ap);

	va_end(ap);

	err_dialog = ewl_dialog_new(EWL_POSITION_BOTTOM);
	if( !err_dialog )
		return;
	ewl_window_title_set(EWL_WINDOW(err_dialog), "Ewler Error");
	ewl_callback_append(err_dialog, EWL_CALLBACK_DELETE_WINDOW,
											err_dialog_cb, err_dialog);

	err_text = ewl_text_new(buf);
	ewl_dialog_widget_add(EWL_DIALOG(err_dialog), err_text);
	ewl_widget_show(err_text);

	err_button = ewl_dialog_button_add(EWL_DIALOG(err_dialog),
																		 EWL_STOCK_OK, EWL_RESPONSE_OK);
	ewl_callback_append(err_button, EWL_CALLBACK_VALUE_CHANGED,
											err_dialog_cb, err_dialog);
	ewl_widget_show(err_button);

	ewl_widget_show(err_dialog);
}

void
ewler_shutdown( void )
{
	ewl_main_quit();
	ewl_shutdown();
	exit(EXIT_SUCCESS);
}

EWL_CALLBACK_DEFN(ewler_exit)
{
	ewler_shutdown();
}

int
main( int argc, char *argv[] )
{
	ewl_init( &argc, argv );

	if( specs_init() < 0 ||
			projects_init(argv[1]) < 0 ||
			forms_init() < 0 ||
			tools_init() < 0 ||
			inspector_init() < 0 ||
			callbacks_init() < 0 )
		exit( EXIT_FAILURE );

	ewl_main();

	return 1;
}
