#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <gtk/gtk.h>
#include <gdk/gdkx.h>

#include <Evas.h>
#include <Edb.h>
#include <stdio.h>
#include <stdlib.h>

#include "interface.h"
#include "support.h"
#include "preferences.h"
#include "recent.h"
#include "globals.h"

GtkWidget *main_win;
GdkVisual *gdk_vis = NULL;
GdkColormap *gdk_cmap = NULL;

int
main (int argc, char *argv[])
{
   int i;
   
   no_splash = FALSE;

   for (i=1; i<argc; i++)
     {
	if (!strcmp(argv[i], "--no-splash"))
	  {
	    no_splash = TRUE;
	  }
	else if ((!strcmp(argv[i], "--help")) ||
		 (!strcmp(argv[i], "-help")) ||
		 (!strcmp(argv[i], "-h")))
	  {
	     printf("Usage: %s [--no-splash] [ebits file]\n", argv[0]);
	     exit(0);
	  }
	else 
	  {
	     load_file = argv[i];
	     no_splash = TRUE;
	  }
     }

#ifdef ENABLE_NLS
   bindtextdomain (PACKAGE, PACKAGE_LOCALE_DIR);
   textdomain (PACKAGE);
#endif

   gtk_set_locale ();
   gtk_init (&argc, &argv);

   if (!pref_init())
     {
       pref_defaults();
     }

   view_evas = evas_new();
   if (render_method == 1)
     evas_set_output_method(view_evas, RENDER_METHOD_3D_HARDWARE);
   else if (render_method == 0)
     evas_set_output_method(view_evas, RENDER_METHOD_ALPHA_SOFTWARE);

   {
     Visual *vis;
     Colormap cmap;
     
     vis = evas_get_optimal_visual(view_evas, GDK_WINDOW_XDISPLAY(GDK_ROOT_PARENT()));
     gdk_vis = gdkx_visual_get(XVisualIDFromVisual(vis));
     cmap = evas_get_optimal_colormap(view_evas, GDK_WINDOW_XDISPLAY(GDK_ROOT_PARENT()));
     gdk_cmap = gdkx_colormap_get(cmap);
     /* workaround for bug in gdk - well oversight in api */
     ((GdkColormapPrivate *)gdk_cmap)->visual = gdk_vis;
   }
      
   add_pixmap_directory (PACKAGE_DATA_DIR "/pixmaps");
   add_pixmap_directory (PACKAGE_SOURCE_DIR "/pixmaps");
   
   /*******/
   gtk_widget_push_visual(gdk_vis);
   gtk_widget_push_colormap(gdk_cmap);
   /*******/
   
   main_win = create_toplevel();
   recent_init();

   /*******/
   gtk_widget_pop_visual();
   gtk_widget_pop_colormap();
   /*******/
   
     {
	GtkWidget *w;
	gchar *text;
	
	w = gtk_object_get_data(GTK_OBJECT(main_win), "states");
	text = "Normal"; gtk_clist_append(GTK_CLIST(w), &text);
	text = "Hilited"; gtk_clist_append(GTK_CLIST(w), &text);
	text = "Clicked"; gtk_clist_append(GTK_CLIST(w), &text);
	text = "Disabled"; gtk_clist_append(GTK_CLIST(w), &text);
     }
   gtk_widget_show(main_win);
   
   gtk_main ();
   
   return 0;
}

