#include <stdlib.h>
#include <string.h>
#include <Edb.h>
#include <Evas.h>
#include <gdk/gdkx.h>
#include "preferences.h"
#include "callbacks.h"
#include "interface.h"
#include "support.h"
#include "bits.h"

#define QUEUE_DRAW \
if (current_idle) gtk_idle_remove(current_idle);\
current_idle = gtk_idle_add(view_redraw, NULL);

#define QUEUE_PREF_DRAW \
if (pref_idle) gtk_idle_remove(pref_idle);\
pref_idle = gtk_idle_add(view_pref_redraw, NULL);

extern gint       render_method;
extern gint       zoom_method;
extern GtkWidget *main_win;
extern char       etcher_config[4096];
extern Evas_Object o_bg;
extern Evas        view_evas;
extern guint       current_idle;

GtkWidget *pref_dialog = NULL;
GtkWidget *color_dialog = NULL;

/* previous context */
Imlib_Image im_old;
Pixmap pm_old;

Window win;
DATA8  colors[3] = {255, 255, 255};
DATA8  colors_old[3];
char   etcher_config[4096];
char  *grid_image_file = NULL;
char  *grid_image_file_old = NULL;
Evas   pref_evas = NULL;
Evas_Object o_pref_image = NULL;
gint   pref_idle = 0;
gint   new_pref_evas = 1;

static void pref_update_preview(void);
static void pref_colors_ok(GtkWidget *widget, gpointer data);
static void pref_colors_cancel(GtkWidget *widget,  gpointer data);
gint view_redraw(gpointer data);

gint
view_pref_redraw(gpointer data)
{
   evas_render(pref_evas);
   pref_idle = 0;
   return FALSE;
}

static void 
pref_update_preview(void)
{
   if (!new_pref_evas)
     {
	if (grid_image_file)
	   free(grid_image_file);
	grid_image_file = NULL;
	
	grid_image_file = strdup(gtk_entry_get_text(GTK_ENTRY(gtk_object_get_data(GTK_OBJECT(pref_dialog), "entry1"))));
	evas_set_image_file(pref_evas, o_pref_image, grid_image_file);
	evas_move(pref_evas, o_pref_image, 0, 0);
	evas_resize(pref_evas, o_pref_image, 99999, 99999);
	evas_set_color(pref_evas, o_pref_image, colors[0], colors[1], colors[2], 255);
	QUEUE_PREF_DRAW;
     }
}

gboolean
on_pref_da_expose_event2                (GtkWidget       *widget,
					 GdkEventExpose  *event,
					 gpointer         user_data)
{
   if (new_pref_evas)
     {
	char *file;
	
	new_pref_evas = 0;
        evas_set_output(pref_evas,
			GDK_WINDOW_XDISPLAY(widget->window),
			GDK_WINDOW_XWINDOW(widget->window),
			GDK_VISUAL_XVISUAL(gtk_widget_get_visual(widget)),
			GDK_COLORMAP_XCOLORMAP(gtk_widget_get_colormap(widget)));
	evas_set_output_size(pref_evas,
			     widget->allocation.width,
			     widget->allocation.height);
	evas_set_output_viewport(pref_evas,
				 0, 0,
				 widget->allocation.width,
				 widget->allocation.height);
        evas_set_image_cache(pref_evas, 8 * 1024 * 1024);
	gdk_window_set_back_pixmap(widget->window, NULL, FALSE);
	file = gtk_entry_get_text(GTK_ENTRY(gtk_object_get_data(GTK_OBJECT(pref_dialog), "entry1")));
	o_pref_image = evas_add_image_from_file(pref_evas, file);
	evas_show(pref_evas, o_pref_image);
	pref_update_preview();
     }
   evas_update_rect(pref_evas,
		    event->area.x,
		    event->area.y,
		    event->area.width,
		    event->area.height);
   QUEUE_PREF_DRAW;
   return FALSE;
}

static void
pref_colors_ok(GtkWidget *widget, gpointer data)
{
   if (color_dialog)
      gtk_widget_hide(color_dialog);
}


static void 
pref_colors_cancel(GtkWidget *widget,  gpointer data)
{
   colors[0] = colors_old[0];
   colors[1] = colors_old[1];
   colors[2] = colors_old[2];
   
   pref_update_preview();
   
   if (grid_image_file)
      free(grid_image_file);
   grid_image_file = NULL;
   grid_image_file = strdup(grid_image_file_old);
   
   if (color_dialog)
      gtk_widget_hide(color_dialog);
}


int
pref_init(void)
{
   int config_ok = 0, config_zoom_method, config_render_method;
   int col_r, col_g, col_b;
   
   g_snprintf(etcher_config, sizeof(etcher_config), "%s/.etcher.db", getenv("HOME"));
   E_DB_INT_GET(etcher_config, "/display/zoom_method", 
		config_zoom_method, config_ok);
   
   E_DB_INT_GET(etcher_config, "/display/render_method", 
		config_render_method, config_ok);
   
   if (!config_ok)
      return 0;
   
   render_method = config_render_method;
   zoom_method = config_zoom_method;
   
   E_DB_INT_GET(etcher_config, "/grid/r", col_r, config_ok);
   if (config_ok)
      colors[0] = (DATA8)col_r;
   E_DB_INT_GET(etcher_config, "/grid/g", col_g, config_ok);
   if (config_ok)
      colors[1] = (DATA8)col_g;
   E_DB_INT_GET(etcher_config, "/grid/b", col_b, config_ok);
   if (config_ok)
      colors[2] = (DATA8)col_b;
   
   return 1;
}


void
pref_defaults(void)
{
   GtkWidget *dialog;
   
   E_DB_STR_SET(etcher_config, "/grid/image", PACKAGE_DATA_DIR "/pixmaps/tile.png");  
   e_db_flush();
   
   dialog = create_render_method();
   gtk_widget_show(dialog);
   
   gtk_main ();
}

void
pref_preferences1_activate               (GtkMenuItem     *menuitem,
					  gpointer         user_data)
{
   GtkWidget *wid;
   int config_ok;
   char * s = NULL;
   
   if (!pref_dialog)
     {
	GdkVisual *gdk_pref_vis;
	GdkColormap *gdk_pref_cmap;
	
	pref_evas = evas_new();
	evas_set_output_method(pref_evas, RENDER_METHOD_ALPHA_SOFTWARE);
	  {
	     Visual *vis;
	     Colormap cmap;
	     
	     vis = evas_get_optimal_visual(pref_evas, GDK_WINDOW_XDISPLAY(GDK_ROOT_PARENT()));
	     gdk_pref_vis = gdkx_visual_get(XVisualIDFromVisual(vis));
	     cmap = evas_get_optimal_colormap(pref_evas, GDK_WINDOW_XDISPLAY(GDK_ROOT_PARENT()));
	     gdk_pref_cmap = gdkx_colormap_get(cmap);
	     /* workaround for bug in gdk - well oversight in api */
	     ((GdkColormapPrivate *)gdk_pref_cmap)->visual = gdk_pref_vis;
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
   
   if (render_method == 0)
     {
	wid = gtk_object_get_data(GTK_OBJECT(pref_dialog), "render1");
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(wid), 1);
     }
   else
     {
	wid = gtk_object_get_data(GTK_OBJECT(pref_dialog), "render2");
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(wid), 1);
     }
   
   if (zoom_method == 0)
     {
	wid = gtk_object_get_data(GTK_OBJECT(pref_dialog), "zoom1");
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(wid), 1);
     }
   else
     {
	wid = gtk_object_get_data(GTK_OBJECT(pref_dialog), "zoom2");
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(wid), 1);
     }
   
   wid = gtk_object_get_data(GTK_OBJECT(pref_dialog), "pref_da");
   gtk_widget_realize(wid);
   win = GDK_WINDOW_XWINDOW(wid->window);
   
   wid = gtk_object_get_data(GTK_OBJECT(pref_dialog), "entry1");
   E_DB_STR_GET(etcher_config, "/grid/image", s, config_ok);
   if (s)
     {
	gtk_entry_set_text(GTK_ENTRY(wid), s);
	
	if (grid_image_file_old)
	   free(grid_image_file_old);
	grid_image_file_old = NULL;
	grid_image_file_old = strdup(s);
	free(s);
     }
   
   gtk_widget_show(pref_dialog);
}



void
pref_ok_clicked                          (GtkButton       *button,
					  gpointer         user_data)
{
   GtkWidget *top, *w;
   
   top = gtk_widget_get_toplevel(GTK_WIDGET(button));
   w = gtk_object_get_data(GTK_OBJECT(top), "render1");
   if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(w)))
      render_method = 0;
   else
      render_method = 1;
   w = gtk_object_get_data(GTK_OBJECT(top), "zoom1");
   if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(w)))
      zoom_method = 0;
   else
      zoom_method = 1;
   
   E_DB_INT_SET(etcher_config, "/grid/r", colors[0]);
   E_DB_INT_SET(etcher_config, "/grid/g", colors[1]);
   E_DB_INT_SET(etcher_config, "/grid/b", colors[2]);
   E_DB_INT_SET(etcher_config, "/display/render_method", (int)render_method);
   E_DB_INT_SET(etcher_config, "/display/zoom_method", (int)zoom_method);
   E_DB_STR_SET(etcher_config, "/grid/image", grid_image_file);
   e_db_flush();
   
   imlib_context_set_image(im_old);
   imlib_context_set_drawable(pm_old);
   
   if (color_dialog)
      gtk_widget_hide(color_dialog);

     {
	int ww, hh, ok, r, g, b;
	
	evas_set_image_file(view_evas, o_bg, grid_image_file);
	evas_get_image_size(view_evas, o_bg, &ww, &hh);
	evas_set_image_fill(view_evas, o_bg, 0, 0, ww, hh);
	evas_move(view_evas, o_bg, 0, 0);
	evas_resize(view_evas, o_bg, 9999, 9999);	
	ok = 0;
	E_DB_INT_GET(etcher_config, "/grid/r", r, ok);
	E_DB_INT_GET(etcher_config, "/grid/g", g, ok);
	E_DB_INT_GET(etcher_config, "/grid/b", b, ok);
	if (ok)
	   evas_set_color(view_evas, o_bg, r, g, b, 255);
	e_db_flush();
     }
   QUEUE_DRAW;
   
   gtk_widget_hide(top);
}


void
pref_cancel_clicked                      (GtkButton       *button,
					  gpointer         user_data)
{
   GtkWidget *top;
   
   imlib_context_set_image(im_old);
   imlib_context_set_drawable(pm_old);
   
   if (grid_image_file) free(grid_image_file);
   grid_image_file = NULL;
   if (grid_image_file_old)
      grid_image_file = strdup(grid_image_file_old);
   
   if (color_dialog)
      gtk_widget_hide(color_dialog);
   top = gtk_widget_get_toplevel(GTK_WIDGET(button));
   gtk_widget_hide(top);
}


void
pref_tintcolor_button_clicked            (GtkButton       *button,
					  gpointer         user_data)
{
   GtkWidget * colorsel;
   
   if (!color_dialog)
      color_dialog = gtk_color_selection_dialog_new("Select Tint Color");
   
   colorsel = GTK_COLOR_SELECTION_DIALOG(color_dialog)->colorsel;
   gtk_signal_connect(GTK_OBJECT(colorsel), "color_changed",
		      (GtkSignalFunc)pref_color_changed, (gpointer)colorsel);
   gtk_signal_connect(GTK_OBJECT(GTK_COLOR_SELECTION_DIALOG(color_dialog)->ok_button), "clicked",
		      (GtkSignalFunc)pref_colors_ok, (gpointer)NULL);
   gtk_signal_connect(GTK_OBJECT(GTK_COLOR_SELECTION_DIALOG(color_dialog)->cancel_button), "clicked",
		      (GtkSignalFunc)pref_colors_cancel, (gpointer)NULL);
   
   colors_old[0] = colors[0];
   colors_old[1] = colors[1];
   colors_old[2] = colors[2];
   
   gtk_widget_show(color_dialog);
}


void
pref_color_changed(GtkWidget *widget, GtkColorSelection * colorsel)
{
   gdouble color[3];
   
   gtk_color_selection_get_color(colorsel, color);
   
   colors[0] = (DATA8)(255 * color[0]);
   colors[1] = (DATA8)(255 * color[1]);
   colors[2] = (DATA8)(255 * color[2]);
   
   pref_update_preview();
}


void
pref_gridimage_changed                   (GtkEditable     *editable,
					  gpointer         user_data)
{
   pref_update_preview();
}
