#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <gtk/gtk.h>
#include <gdk/gdkx.h>

#include <Evas.h>
#include <Edb.h>
#include <stdio.h>
#include <stdlib.h>

#include "globals.h"
#include "interface.h"
#include "preferences.h"
#include "recent.h"
#include "support.h"
#include "workspace.h"

GtkWidget   *main_win;
GdkVisual   *gdk_vis = NULL;
GdkColormap *gdk_cmap = NULL;
char        *load_file;

int
main (int argc, char *argv[])
{
  int i;
  
  pref_enable_splashscreen(TRUE);
  
  for (i=1; i<argc; i++)
    {
      if (!strcmp(argv[i], "--no-splash"))
	{
	  pref_enable_splashscreen(FALSE);
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
	  pref_add_file(argv[i]);
	  pref_enable_splashscreen(FALSE);
	}
    }

#ifdef ENABLE_NLS
   bindtextdomain (PACKAGE, PACKAGE_LOCALE_DIR);
   textdomain (PACKAGE);
#endif

   gtk_set_locale();
   gtk_init (&argc, &argv);

   if (!pref_exists())
     pref_set_defaults();

   pref_init();
   workspace_init();

   {
     Visual *vis;
     Colormap cmap;
     
     vis = evas_get_optimal_visual(workspace_get_evas(), GDK_WINDOW_XDISPLAY(GDK_ROOT_PARENT()));
     gdk_vis = gdkx_visual_get(XVisualIDFromVisual(vis));
     cmap = evas_get_optimal_colormap(workspace_get_evas(), GDK_WINDOW_XDISPLAY(GDK_ROOT_PARENT()));
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
   workspace_set_light(Green);

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
	text = "Selected"; gtk_clist_append(GTK_CLIST(w), &text);
     }
   gtk_widget_show(main_win);
   
   gtk_main ();
   
   return 0;
}

