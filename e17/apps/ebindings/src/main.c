/**************************************************************************
 * main.c
 * Project: Ebindings
 * Programmer: Corey Donohoe<atmos@atmos.org>
 * October 10, 2001
 * Gets the ebindings ball rolling
 *************************************************************************/
#include "interface.h"

/* Pretty complex huh ? =) */
int
main(int argc, char *argv[])
{
	GtkWidget *win;
	
	gtk_init(&argc, &argv);
	
	win = create_main_ebindings_window();

	gtk_main();

	return 0;
}
