#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <gtk/gtk.h>
#include <gdk/gdkx.h>

#include <Evas.h>
#include <Edb.h>
#include <stdlib.h>

#include "interface.h"
#include "support.h"

GtkWidget *main_win;
char etcher_config[4096];
extern Evas view_evas;
extern gint render_method;

GdkVisual *gdk_vis = NULL;
GdkColormap *gdk_cmap = NULL;

int
main (int argc, char *argv[])
{
   int config_ok = 0, config_render_method;
   
#ifdef ENABLE_NLS
   bindtextdomain (PACKAGE, PACKAGE_LOCALE_DIR);
   textdomain (PACKAGE);
#endif

   gtk_set_locale ();
   gtk_init (&argc, &argv);

   g_snprintf(etcher_config, sizeof(etcher_config), "%s/.etcher.db", getenv("HOME"));
   E_DB_INT_GET(etcher_config, "/display/render_method", 
		config_render_method, config_ok);
   e_db_flush();
   if (!config_ok)
     {
	GtkWidget *dialog;
	
	dialog = create_render_method();
	gtk_widget_show(dialog);
	
	gtk_main ();
     }
   else
     {
	render_method = config_render_method;
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
   
   
   main_win = create_main();
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

