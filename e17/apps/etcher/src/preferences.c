#include <stdlib.h>
#include <string.h>
#include <Edb.h>
#include <Evas.h>
#include <Evas_Engine_Software_X11.h>
#include <gdk/gdkx.h>

#include "macros.h"
#include "preferences.h"
#include "callbacks.h"
#include "interface.h"
#include "support.h"
#include "bits.h"
#include "globals.h"
#include "workspace.h"

typedef struct etcher_preferences
{
   char                etcher_config[4096];	/* config file name */
   Evas_List *           files;	/* List of char*, files to load on startup */

   gint                render_method;
   gint                zoom_method;
   gboolean            splash;

   Window              win;

   DATA8               colors[3];
   DATA8               colors_old[3];

   char               *grid_image_file;
   char               *grid_image_file_old;

   Evas *                pref_evas;
   Evas_Object        *o_pref_image, *bg_rect;
   gint                pref_idle;
   gint                new_pref_evas;
   gint                smartsize;

}
EtcherPreferences;

EtcherPreferences   pref;

GtkWidget          *pref_dialog = NULL;
GtkWidget          *color_dialog = NULL;

/* previous context */
Imlib_Image         im_old;
Pixmap              pm_old;

static void         pref_update_preview(void);
static void         pref_colors_ok(GtkWidget * widget, gpointer data);
static void         pref_colors_cancel(GtkWidget * widget, gpointer data);

gint
pref_view_redraw(gpointer data)
{
   evas_render(pref.pref_evas);
   pref.pref_idle = 0;
   return FALSE;
}

static void
pref_update_preview(void)
{
   if (!pref.new_pref_evas)
     {
	if (pref.grid_image_file)
	   free(pref.grid_image_file);
	pref.grid_image_file = NULL;

	pref.grid_image_file =
	   strdup(gtk_entry_get_text
		  (GTK_ENTRY
		   (gtk_object_get_data(GTK_OBJECT(pref_dialog), "entry1"))));
	evas_object_image_file_set(pref.o_pref_image,
				   pref.grid_image_file, NULL);
	evas_object_move(pref.o_pref_image, 0, 0);
	evas_object_resize(pref.o_pref_image, 99999, 99999);
	evas_object_color_set(pref.o_pref_image,
		       pref.colors[0], pref.colors[1], pref.colors[2], 255);
	QUEUE_DRAW(pref.pref_idle, pref_view_redraw);
     }
}

gboolean
on_pref_da_expose_event2(GtkWidget * widget,
			 GdkEventExpose * event, gpointer user_data)
{
   if (pref.new_pref_evas)
     {
	char               *file;

	pref.new_pref_evas = 0;
	/* replaced
	evas_set_output(pref.pref_evas,
			GDK_WINDOW_XDISPLAY(widget->window),
			GDK_WINDOW_XWINDOW(widget->window),
			GDK_VISUAL_XVISUAL(gtk_widget_get_visual
					   (widget)),
			GDK_COLORMAP_XCOLORMAP(gtk_widget_get_colormap
					       (widget)));
	*/
	{
	  Evas_Engine_Info_Software_X11 *einfo;

	  einfo = (Evas_Engine_Info_Software_X11 *) evas_engine_info_get(pref.pref_evas);

	  /* the following is specific to the engine */
	  einfo->info.display = GDK_WINDOW_XDISPLAY(widget->window);
	  einfo->info.visual = GDK_VISUAL_XVISUAL(gtk_widget_get_visual
						  (widget));
	  einfo->info.colormap = GDK_COLORMAP_XCOLORMAP(gtk_widget_get_colormap
							(widget));

	  einfo->info.drawable = GDK_WINDOW_XWINDOW(widget->window);
	  einfo->info.depth = DefaultDepth(einfo->info.display, DefaultScreen(einfo->info.display));
	  einfo->info.rotation = 0;
	  einfo->info.debug = 0;
	  evas_engine_info_set(pref.pref_evas, (Evas_Engine_Info *) einfo);
	}

	evas_output_size_set(pref.pref_evas, widget->allocation.width,
			     widget->allocation.height);
	evas_output_viewport_set(pref.pref_evas, 0, 0,
				 widget->allocation.width,
				 widget->allocation.height);
	evas_object_image_cache_set(pref.pref_evas, 8 * 1024 * 1024);
	gdk_window_set_back_pixmap(widget->window, NULL, FALSE);
	file = gtk_entry_get_text(GTK_ENTRY
				  (gtk_object_get_data
				   (GTK_OBJECT(pref_dialog), "entry1")));
	pref.o_pref_image = evas_object_image_add(pref.pref_evas);
	evas_object_image_file_set(pref.o_pref_image, file, NULL);
	evas_object_show(pref.o_pref_image);

	pref.bg_rect = evas_object_rectangle_add(pref.pref_evas);
	evas_object_layer_set(pref.bg_rect, -9999);
	evas_object_color_set(pref.bg_rect, 255, 255, 255, 255);
	evas_object_move(pref.bg_rect, 0, 0);
	evas_object_resize(pref.bg_rect, 9999, 9999);
	evas_object_show(pref.bg_rect);
	pref_update_preview();
     }

   evas_damage_rectangle_add(pref.pref_evas, event->area.x, event->area.y,
			     event->area.width, event->area.height);
   QUEUE_DRAW(pref.pref_idle, pref_view_redraw);

   return FALSE;
}

static void
pref_colors_ok(GtkWidget * widget, gpointer data)
{
   if (color_dialog)
      gtk_widget_hide(color_dialog);
}

static void
pref_colors_cancel(GtkWidget * widget, gpointer data)
{
   pref.colors[0] = pref.colors_old[0];
   pref.colors[1] = pref.colors_old[1];
   pref.colors[2] = pref.colors_old[2];

   pref_update_preview();

   if (pref.grid_image_file)
      free(pref.grid_image_file);
   pref.grid_image_file = NULL;
   pref.grid_image_file = strdup(pref.grid_image_file_old);

   if (color_dialog)
      gtk_widget_hide(color_dialog);
}

int
pref_exists(void)
{
   char                s[1024];
   int                 dummy, config_ok = 0;

   g_snprintf(s, sizeof(s), "%s/.etcher.db", getenv("HOME"));
   E_DB_INT_GET(s, "/display/zoom_method", dummy, config_ok);

   if (!config_ok)
      return 0;

   g_snprintf(pref.etcher_config, sizeof(pref.etcher_config),
	      "%s/.etcher.db", getenv("HOME"));
   return 1;
}

int
pref_init(void)
{
   int                 config_ok = 0, config_zoom_method, config_render_method;
   int                 col_r, col_g, col_b, splash, smarts;
   char               *s = NULL;

   E_DB_INT_GET(pref.etcher_config, "/display/zoom_method",
		config_zoom_method, config_ok);

   if (!config_ok)
      return 0;

   E_DB_INT_GET(pref.etcher_config, "/display/render_method",
		config_render_method, config_ok);

   pref.render_method = config_render_method;
   pref.zoom_method = config_zoom_method;
   pref.colors[0] = 255;
   pref.colors[1] = 255;
   pref.colors[2] = 255;
   pref.splash = 1;
   pref.grid_image_file = NULL;

   E_DB_INT_GET(pref.etcher_config, "/grid/r", col_r, config_ok);
   if (config_ok)
      pref.colors[0] = (DATA8) col_r;
   E_DB_INT_GET(pref.etcher_config, "/grid/g", col_g, config_ok);
   if (config_ok)
      pref.colors[1] = (DATA8) col_g;
   E_DB_INT_GET(pref.etcher_config, "/grid/b", col_b, config_ok);
   if (config_ok)
      pref.colors[2] = (DATA8) col_b;
   E_DB_INT_GET(pref.etcher_config, "/splash/show", splash, config_ok);
   if (config_ok)
      pref.splash = splash;
   E_DB_STR_GET(pref.etcher_config, "/grid/image", s, config_ok);
   if (config_ok)
     {
	pref.grid_image_file = strdup(s);
	free(s);
     }
   E_DB_INT_GET(pref.etcher_config, "/settings/smartsize", smarts, config_ok);
   if (config_ok)
      pref.smartsize = smarts;

   pref.pref_evas = NULL;
   pref.o_pref_image = NULL;
   pref.pref_idle = 0;
   pref.new_pref_evas = 1;

   return 1;
}

void
pref_set_defaults(void)
{
   GtkWidget          *dialog;

   g_snprintf(pref.etcher_config, sizeof(pref.etcher_config),
	      "%s/.etcher.db", getenv("HOME"));
   E_DB_STR_SET(pref.etcher_config, "/grid/image",
		PACKAGE_DATA_DIR "/pixmaps/tile.png");
   E_DB_INT_SET(pref.etcher_config, "/grid/r", 255);
   E_DB_INT_SET(pref.etcher_config, "/grid/g", 255);
   E_DB_INT_SET(pref.etcher_config, "/grid/b", 255);
   E_DB_INT_SET(pref.etcher_config, "/display/zoom_method", (int)Smooth);
   E_DB_INT_SET(pref.etcher_config, "/splash/show", 1);
   E_DB_INT_SET(pref.etcher_config, "/settings/smartsize", 0);
   e_db_flush();

   dialog = create_render_method();
   gtk_widget_show(dialog);

   gtk_main();
}

char               *
pref_get_config(void)
{
   return pref.etcher_config;
}

void
pref_add_file(char *filename)
{
   pref.files = evas_list_append(pref.files, filename);
}

Evas_List *
pref_get_files(void)
{
   return pref.files;
}

void
pref_set_grid_image(char *filename)
{
   GtkWidget          *w;

   if (!filename)
      return;

   w = gtk_object_get_data(GTK_OBJECT(pref_dialog), "entry1");
   if (w)
      gtk_entry_set_text(GTK_ENTRY(w), filename);
}

void
pref_set_render_method(EtcherRenderMethod method)
{
   pref.render_method = method;
   E_DB_INT_SET(pref.etcher_config, "/display/render_method",
		(int)pref.render_method);
   e_db_flush();
}

EtcherRenderMethod
pref_get_render_method(void)
{
   return pref.render_method;
}

void
pref_set_zoom_method(EtcherZoomMethod method)
{
   pref.zoom_method = method;
   E_DB_INT_SET(pref.etcher_config, "/display/zoom_method",
		(int)pref.zoom_method);
   e_db_flush();
}

EtcherZoomMethod
pref_get_zoom_method(void)
{
   return pref.zoom_method;
}

void
pref_enable_splashscreen(gboolean yesno)
{
   pref.splash = yesno;
}

gboolean
pref_splashscreen_enabled(void)
{
   return pref.splash;
}

gboolean
pref_smartsize_enabled(void)
{
   return pref.smartsize;
}

void
pref_preferences1_activate(GtkMenuItem * menuitem, gpointer user_data)
{
   GtkWidget          *wid;
   int                 config_ok;
   char               *s = NULL;

   if (!pref_dialog)
     {
	GdkVisual          *gdk_pref_vis;
	GdkColormap        *gdk_pref_cmap;

	pref.pref_evas = evas_new();
	evas_output_method_set(pref.pref_evas, evas_render_method_lookup("software_x11"));
	{
	   Visual             *vis;
	   Colormap            cmap;

	   /* checkme
	   vis = evas_get_optimal_visual(pref.pref_evas,
					 GDK_WINDOW_XDISPLAY
					 (GDK_ROOT_PARENT()));
	   */
	   vis = DefaultVisual(GDK_WINDOW_XDISPLAY(GDK_ROOT_PARENT()), DefaultScreen(GDK_WINDOW_XDISPLAY(GDK_ROOT_PARENT())));
	   gdk_pref_vis = gdkx_visual_get(XVisualIDFromVisual(vis));
	   /* checkme 
           cmap = evas_get_optimal_colormap(pref.pref_evas,
					    GDK_WINDOW_XDISPLAY
					    (GDK_ROOT_PARENT()));
	   */
	   cmap = DefaultColormap(GDK_WINDOW_XDISPLAY(GDK_ROOT_PARENT()), DefaultScreen(GDK_WINDOW_XDISPLAY(GDK_ROOT_PARENT())));
		 gdk_pref_cmap = gdkx_colormap_get(cmap);
	   /* workaround for bug in gdk - well oversight in api */
	   ((GdkColormapPrivate *) gdk_pref_cmap)->visual = gdk_pref_vis;
	}

      /*******/
	gtk_widget_push_visual(gdk_pref_vis);
	gtk_widget_push_colormap(gdk_pref_cmap);

      /*******/

	pref_dialog = create_preferences();

      /*******/
	gtk_widget_pop_visual();
	gtk_widget_pop_colormap();

      /*******/
     }

   if (pref.render_method == Software)
     {
	wid = gtk_object_get_data(GTK_OBJECT(pref_dialog), "render1");
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(wid), 1);
     }
   else
     {
	wid = gtk_object_get_data(GTK_OBJECT(pref_dialog), "render2");
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(wid), 1);
     }

   if (pref.zoom_method == Smooth)
     {
	wid = gtk_object_get_data(GTK_OBJECT(pref_dialog), "zoom1");
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(wid), 1);
     }
   else
     {
	wid = gtk_object_get_data(GTK_OBJECT(pref_dialog), "zoom2");
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(wid), 1);
     }

   wid = gtk_object_get_data(GTK_OBJECT(pref_dialog), "splash");
   if (pref.splash)
      gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(wid), 1);
   else
      gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(wid), 0);

   wid = gtk_object_get_data(GTK_OBJECT(pref_dialog), "pref_da");
   gtk_widget_realize(wid);
   pref.win = GDK_WINDOW_XWINDOW(wid->window);

   wid = gtk_object_get_data(GTK_OBJECT(pref_dialog), "entry1");
   E_DB_STR_GET(pref.etcher_config, "/grid/image", s, config_ok);
   if (s)
     {
	gtk_entry_set_text(GTK_ENTRY(wid), s);

	if (pref.grid_image_file_old)
	   free(pref.grid_image_file_old);

	pref.grid_image_file_old = NULL;
	pref.grid_image_file_old = strdup(s);
	free(s);
     }

   gtk_widget_show(pref_dialog);
   pref_update_preview();
}

void
pref_ok_clicked(GtkButton * button, gpointer user_data)
{
   GtkWidget          *top, *w;

   top = gtk_widget_get_toplevel(GTK_WIDGET(button));

   w = gtk_object_get_data(GTK_OBJECT(top), "render1");
   if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(w)))
      pref.render_method = Software;
   else
      pref.render_method = Hardware;

   w = gtk_object_get_data(GTK_OBJECT(top), "zoom1");
   if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(w)))
      pref.zoom_method = Smooth;
   else
      pref.zoom_method = Framebuffer;

   w = gtk_object_get_data(GTK_OBJECT(top), "splash");
   if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(w)))
      pref.splash = 1;
   else
      pref.splash = 0;

   E_DB_INT_SET(pref.etcher_config, "/grid/r", pref.colors[0]);
   E_DB_INT_SET(pref.etcher_config, "/grid/g", pref.colors[1]);
   E_DB_INT_SET(pref.etcher_config, "/grid/b", pref.colors[2]);
   E_DB_INT_SET(pref.etcher_config, "/display/render_method",
		(int)pref.render_method);
   E_DB_INT_SET(pref.etcher_config, "/display/zoom_method",
		(int)pref.zoom_method);

   if (!pref.grid_image_file || !strlen(pref.grid_image_file))
      pref.grid_image_file = g_strdup(PACKAGE_DATA_DIR "/pixmaps/tile.png");
   E_DB_STR_SET(pref.etcher_config, "/grid/image", pref.grid_image_file);
   E_DB_INT_SET(pref.etcher_config, "/splash/show", (int)pref.splash);
   e_db_flush();

   imlib_context_set_image(im_old);
   imlib_context_set_drawable(pm_old);

   if (color_dialog)
      gtk_widget_hide(color_dialog);

   workspace_set_grid_image(pref.grid_image_file);
   QUEUE_DRAW(pref.pref_idle, pref_view_redraw);

   gtk_widget_hide(top);
}

void
pref_cancel_clicked(GtkButton * button, gpointer user_data)
{
   GtkWidget          *top;

   imlib_context_set_image(im_old);
   imlib_context_set_drawable(pm_old);

   if (pref.grid_image_file)
      free(pref.grid_image_file);

   pref.grid_image_file = NULL;

   if (pref.grid_image_file_old)
      pref.grid_image_file = strdup(pref.grid_image_file_old);

   if (color_dialog)
      gtk_widget_hide(color_dialog);
   top = gtk_widget_get_toplevel(GTK_WIDGET(button));
   gtk_widget_hide(top);
}

void
pref_tintcolor_button_clicked(GtkButton * button, gpointer user_data)
{
   GtkWidget          *colorsel;

   if (!color_dialog)
      color_dialog = gtk_color_selection_dialog_new("Select Tint Color");

   colorsel = GTK_COLOR_SELECTION_DIALOG(color_dialog)->colorsel;
   gtk_signal_connect(GTK_OBJECT(colorsel), "color_changed",
		      (GtkSignalFunc) pref_color_changed, (gpointer) colorsel);
   gtk_signal_connect(GTK_OBJECT
		      (GTK_COLOR_SELECTION_DIALOG(color_dialog)->
		       ok_button), "clicked",
		      (GtkSignalFunc) pref_colors_ok, (gpointer) NULL);
   gtk_signal_connect(GTK_OBJECT
		      (GTK_COLOR_SELECTION_DIALOG(color_dialog)->
		       cancel_button), "clicked",
		      (GtkSignalFunc) pref_colors_cancel, (gpointer) NULL);

   pref.colors_old[0] = pref.colors[0];
   pref.colors_old[1] = pref.colors[1];
   pref.colors_old[2] = pref.colors[2];

   gtk_widget_show(color_dialog);
}

void
pref_color_changed(GtkWidget * widget, GtkColorSelection * colorsel)
{
   gdouble             color[3];

   gtk_color_selection_get_color(colorsel, color);

   pref.colors[0] = (DATA8) (255 * color[0]);
   pref.colors[1] = (DATA8) (255 * color[1]);
   pref.colors[2] = (DATA8) (255 * color[2]);

   pref_update_preview();
}

void
pref_gridimage_changed(GtkEditable * editable, gpointer user_data)
{
   pref_update_preview();
}
