/* --------------------------------------
 * Retina - Evas powered image viewer
 * (C) 2000, Joshua Deere
 * dphase@dphase.net
 * --------------------------------------
 * See COPYING for license information
 * --------------------------------------
 */

#include "retina.h"

extern Evas e_area;
extern Evas_Object e_img;
extern Evas_Object e_checks;
extern Evas_Object e_btn1;
extern GtkWidget *area, *window;
extern GdkVisual *gdk_vis;
extern GdkColormap *gdk_cmap;
extern Visual *vis;
extern Colormap cmap;
extern guint current_idle;

gint
view_redraw(gpointer data)
{
   evas_render(e_area);
   current_idle = 0;
   return FALSE;
}


void
r_evas_create()
{
   /* Initialize the Evas */
   
   e_area = evas_new();
   evas_set_image_cache(e_area, 8 * 1024 * 1024);
   evas_set_output_method(e_area, RENDER_METHOD_ALPHA_SOFTWARE);
   vis = evas_get_optimal_visual(e_area, GDK_WINDOW_XDISPLAY(GDK_ROOT_PARENT()));
   cmap = evas_get_optimal_colormap(e_area, GDK_WINDOW_XDISPLAY(GDK_ROOT_PARENT()));
   gdk_vis = gdkx_visual_get(XVisualIDFromVisual(vis));
   gdk_cmap = gdkx_colormap_get(cmap);
   ((GdkColormapPrivate *)gdk_cmap)->visual = gdk_vis;
   gtk_widget_push_visual(gdk_vis);
   gtk_widget_push_colormap(gdk_cmap);
}

void
r_evas_init()
{
   int w, h;
   
   gtk_widget_realize(GTK_WIDGET(area));
   gdk_window_set_back_pixmap(area->window, NULL, FALSE);
   evas_set_output(e_area,
		   GDK_WINDOW_XDISPLAY(area->window),
		   GDK_WINDOW_XWINDOW(area->window),
		   GDK_VISUAL_XVISUAL(gtk_widget_get_visual(area)),
		   GDK_COLORMAP_XCOLORMAP(gtk_widget_get_colormap(area)));
   
   evas_set_output_size(e_area, area->allocation.width, area->allocation.height);
   evas_set_output_viewport(e_area, 0, 0, area->allocation.width, area->allocation.height);

   
   /* Checkerboard */
   e_checks = evas_add_image_from_file(e_area, "../img/checks.png");
   evas_get_image_size(e_area, e_checks, &w, &h);
   evas_set_image_fill(e_area, e_checks, 0, 0, w, h);
   evas_show(e_area, e_checks);
   evas_move(e_area, e_checks, 0, 0);
   evas_resize(e_area, e_checks, 9999, 9999);
}

void
r_evas_config_event(GtkWidget *area, GdkEventConfigure *event)
{
   /* The Configure event */
   
   /* If we have an e_img, scale it to the new window size */
   if(e_img){
      evas_resize(e_area, e_img, event->width, event->height);
      evas_set_image_fill(e_area, e_img, 0, 0, event->width, event->height);
   }
   
   evas_set_output_size(e_area,
			event->width,
			event->height);
   evas_set_output_viewport(e_area, 0, 0,
			    event->width,
			    event->height);
   QUEUE_DRAW;
}

void
r_evas_expose_event(GtkWidget *area, GdkEventExpose *event)
{
   evas_update_rect(e_area, event->area.x, event->area.y, 
		    event->area.width, event->area.height);
   QUEUE_DRAW;
}

void
r_evas_toolbar_init()
{
   /* Create the toolbar, but do not display */
   int w, h;
   
   e_btn1 = evas_add_image_from_file(e_area, "../img/toolbar.png");
   evas_get_image_size(e_area, e_btn1, &w, &h);
   evas_move(e_area, e_btn1, 0, 0);
   evas_resize(e_area, e_btn1, w, h);
   evas_set_layer(e_area, e_checks, 0);
   evas_set_layer(e_area, e_img, 1);
   evas_set_layer(e_area, e_btn1, 2);
}

int
r_evas_load(char *img)
{
   /* Load an image onto the Evas_Object 'e_img' */
   int w, h;
   
   e_img = evas_add_image_from_file(e_area, img);
   evas_get_image_size(e_area, e_img, &w, &h);
   evas_show(e_area, e_img);
   evas_move(e_area, e_img, 0, 0);
   evas_resize(e_area, e_img, w, h);
   
   gtk_widget_set_usize(GTK_WIDGET(area), w, h);
   gtk_widget_set_usize(GTK_WIDGET(window), w, h);
   
   QUEUE_DRAW;
   
   return 0;
}

int
r_evas_save(char *img)
{
   /* Save an image to a file */
   return 0;
}
