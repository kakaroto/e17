#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <gtk/gtk.h>

#include "interface.h"
#include "support.h"

GtkWidget *main_win;

int
main (int argc, char *argv[])
{
   
#ifdef ENABLE_NLS
   bindtextdomain (PACKAGE, PACKAGE_LOCALE_DIR);
   textdomain (PACKAGE);
#endif
   
   gtk_set_locale ();
   gtk_init (&argc, &argv);
   
   add_pixmap_directory (PACKAGE_DATA_DIR "/pixmaps");
   add_pixmap_directory (PACKAGE_SOURCE_DIR "/pixmaps");
   
   main_win = create_main();
   gtk_widget_show(main_win);
   
   gtk_main ();
   
   return 0;
}

