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
extern Evas_Object e_btn1, e_btn2, e_btn3, e_btn4, e_btn5, e_bs;
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
	 int w, h, x = 0 , y = 0;
	 int imgw, imgh;

   /* If we have an e_img, center it in the window */
   if(e_img){
		 evas_get_image_size(e_area, e_img, &imgw, &imgh);
		 w = window->allocation.width;
		 h = window->allocation.height;

		 if(w > imgw){
			 x = (w - imgw) / 2;
			 w = imgw;
		 } else {
			 x = 0;
		 }

		 if(h > imgh){
			 y = (h - imgh) / 2;
			 h = imgh;
		 } else {
			 y = 0;
		 }
   }

	 evas_move(e_area, e_img, x, y);

	 /* move the scrollbar for the browser window */
	 r_scrollbar_render();
   
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
	 e_bs = evas_add_image_from_file(e_area, "../img/toolbar_shadow.png");
   e_btn1 = evas_add_image_from_file(e_area, "../img/button_open.png");
	 e_btn2 = evas_add_image_from_file(e_area, "../img/button_save.png");
	 e_btn3 = evas_add_image_from_file(e_area, "../img/button_browse.png");
	 e_btn4 = evas_add_image_from_file(e_area, "../img/button_info.png");
	 e_btn5 = evas_add_image_from_file(e_area, "../img/button_close.png");
	 
	 evas_move(e_area, e_bs, 3, 3);
   evas_move(e_area, e_btn1, 3, 3);
	 evas_move(e_area, e_btn2, 24, 3);
	 evas_move(e_area, e_btn3, 45, 3);
	 evas_move(e_area, e_btn4, 66, 3);
	 evas_move(e_area, e_btn5, 87, 3);
	 
	 evas_resize(e_area, e_bs, 115, 30);
   evas_resize(e_area, e_btn1, 21, 21);
	 evas_resize(e_area, e_btn2, 21, 21);
	 evas_resize(e_area, e_btn3, 21, 21);
	 evas_resize(e_area, e_btn4, 21, 21);
	 evas_resize(e_area, e_btn5, 21, 21);
   
	 evas_set_layer(e_area, e_checks, 0);
   evas_set_layer(e_area, e_img, 1);
	 evas_set_layer(e_area, e_bs, 3);
   evas_set_layer(e_area, e_btn1, 3);
	 evas_set_layer(e_area, e_btn2, 3);
	 evas_set_layer(e_area, e_btn3, 3);
	 evas_set_layer(e_area, e_btn4, 3);
	 evas_set_layer(e_area, e_btn5, 3);
}

int
r_evas_load(char *img)
{
   /* Load an image onto the Evas_Object 'e_img' */
   int w, h;

	 /* If there is already an image in the object, destroy it */
	 if(e_img)
		 evas_del_object(e_area, e_img);
   
   e_img = evas_add_image_from_file(e_area, img);
   evas_get_image_size(e_area, e_img, &w, &h);
   evas_show(e_area, e_img);
   evas_move(e_area, e_img, 0, 0);
   evas_resize(e_area, e_img, w, h);

	 evas_callback_add(e_area, e_img, CALLBACK_MOUSE_DOWN, r_cb_m_down, NULL);
	 evas_callback_add(e_area, e_checks, CALLBACK_MOUSE_DOWN, r_cb_m_down, NULL);
   
	 if(w < 300 && h > 180)
		 gtk_widget_set_usize(GTK_WIDGET(window), 300, h+20);
	 else if(h < 180 && w > 300)
		 gtk_widget_set_usize(GTK_WIDGET(window), w+20, 180);
	 else if(w < 300 && h < 180)
		 gtk_widget_set_usize(GTK_WIDGET(window), 300, 180);
	 else
		 gtk_widget_set_usize(GTK_WIDGET(window), w, h);
   
   QUEUE_DRAW;

	 {
		 gchar wintitle[255];
		 sprintf(wintitle, "Retina - %s", img);
		 gtk_window_set_title(GTK_WINDOW(window), wintitle);
	 }
   
   return 0;
}

int
r_evas_save(char *img)
{
   /* Save an image to a file */
   return 0;
}
