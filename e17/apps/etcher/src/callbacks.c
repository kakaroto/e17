#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <gtk/gtk.h>

#include <Evas.h>
#include <gdk/gdkx.h>

#include "callbacks.h"
#include "interface.h"
#include "support.h"

extern GtkWidget *main_win;

static Evas view_evas = NULL;
static Evas_Object o_logo = NULL;
static Evas_Object o_handle1 = NULL, o_handle2, o_handle3, o_handle4, o_edge1, o_edge2, o_edge3, o_edge4, o_backing, o_pointer = NULL;
static double backing_x, backing_y, backing_w, backing_h;

static gint
view_redraw(gpointer data)
{
   evas_render(view_evas);
   return FALSE;
}

static void
handle_mouse_down (void *_data, Evas _e, Evas_Object _o, int _b, int _x, int _y)
{
   evas_put_data(_e, _o, "clicked", (void *)1);
   evas_put_data(_e, _o, "x", (void *)_x);
   evas_put_data(_e, _o, "y", (void *)_y);
}

static void
handle_mouse_up (void *_data, Evas _e, Evas_Object _o, int _b, int _x, int _y)
{
   evas_remove_data(_e, _o, "clicked");
}

static void
handle_mouse_move (void *_data, Evas _e, Evas_Object _o, int _b, int _x, int _y)
{
   if (evas_get_data(_e, _o, "clicked"))
     {
	int x, y;
	
	x = evas_get_data(_e, _o, "x");
	y = evas_get_data(_e, _o, "y");
	evas_put_data(_e, _o, "x", (void *)_x);
	evas_put_data(_e, _o, "y", (void *)_y);
	if (_o == o_handle1)
	  {
	     backing_x += (_x - x);
	     backing_y += (_y - y);
	     backing_w += (x - _x);
	     backing_h += (y - _y);
	     if (backing_w < 1) 
		{
		   backing_x -= 1 - backing_w;
		   backing_w = 1;
		}
	     if (backing_h < 1) 
		{
		   backing_y -= 1 - backing_h;
		   backing_h = 1;
		}
	  }
	else if (_o == o_handle2)
	  {
	     backing_y += (_y - y);
	     backing_w += (_x - x);
	     backing_h += (y - _y);
	     if (backing_w < 1) 
		{
		   backing_w = 1;
		}
	     if (backing_h < 1) 
		{
		   backing_y -= 1 - backing_h;
		   backing_h = 1;
		}
	  }
	else if (_o == o_handle3)
	  {
	     backing_x += (_x - x);
	     backing_w += (x - _x);
	     backing_h += (_y - y);
	     if (backing_w < 1) 
		{
		   backing_x -= 1 - backing_w;
		   backing_w = 1;
		}
	     if (backing_h < 1) 
		{
		   backing_h = 1;
		}
	  }
	else if (_o == o_handle4)
	  {
	     backing_w += (_x - x);
	     backing_h += (_y - y);
	     if (backing_w < 1) 
		{
		   backing_w = 1;
		}
	     if (backing_h < 1) 
		{
		   backing_h = 1;
		}
	  }
	else if (_o == o_backing)
	  {
	     backing_x += (_x - x);
	     backing_y += (_y - y);
	  }
	view_configure_handles(NULL);
     }
}

static gint
view_configure_handles(gpointer data)
{
   int w, h;
   GtkWidget *entry;
   gchar buf[32];
   
   evas_get_image_size(view_evas, o_handle1, &w, &h);
   evas_move(view_evas, o_handle1, backing_x - w, backing_y - h);

   evas_get_image_size(view_evas, o_handle2, &w, &h);
   evas_move(view_evas, o_handle2, backing_x + backing_w, backing_y - h);
   
   evas_get_image_size(view_evas, o_handle3, &w, &h);
   evas_move(view_evas, o_handle3, backing_x - w, backing_y + backing_h);
   
   evas_get_image_size(view_evas, o_handle4, &w, &h);
   evas_move(view_evas, o_handle4, backing_x + backing_w, backing_y + backing_h);

   evas_move(view_evas, o_backing, backing_x - 1, backing_y  - 1);
   evas_resize(view_evas, o_backing, backing_w + 2, backing_h + 2);
   
   evas_set_line_xy(view_evas, o_edge1, 
		    backing_x - 1, backing_y - 1, 
		    backing_x - 1, backing_y + backing_h);
   evas_set_line_xy(view_evas, o_edge2, 
		    backing_x - 1, backing_y  - 1, 
		    backing_x + backing_w, backing_y - 1);
   evas_set_line_xy(view_evas, o_edge3, 
		    backing_x - 1, backing_y + backing_h, 
		    backing_x + backing_w, backing_y + backing_h);
   evas_set_line_xy(view_evas, o_edge4, 
		    backing_x + backing_w, backing_y  - 1, 
		    backing_x + backing_w, backing_y + backing_h);
   entry = gtk_object_get_data(GTK_OBJECT(main_win), "x");
   g_snprintf(buf, sizeof(buf), "%3.0f", backing_x);
   gtk_entry_set_text(GTK_ENTRY(entry), buf);
   entry = gtk_object_get_data(GTK_OBJECT(main_win), "y");
   g_snprintf(buf, sizeof(buf), "%3.0f", backing_y);
   gtk_entry_set_text(GTK_ENTRY(entry), buf);
   entry = gtk_object_get_data(GTK_OBJECT(main_win), "width");
   g_snprintf(buf, sizeof(buf), "%3.0f", backing_w);
   gtk_entry_set_text(GTK_ENTRY(entry), buf);
   entry = gtk_object_get_data(GTK_OBJECT(main_win), "height");
   g_snprintf(buf, sizeof(buf), "%3.0f", backing_h);
   gtk_entry_set_text(GTK_ENTRY(entry), buf);
   
   gtk_idle_add(view_redraw, NULL);
}

static gint
view_create_handles(gpointer data)
{
   evas_show(view_evas, o_handle1 = evas_add_image_from_file(view_evas, PACKAGE_SOURCE_DIR"/pixmaps/handle1.png"));
   evas_show(view_evas, o_handle2 = evas_add_image_from_file(view_evas, PACKAGE_SOURCE_DIR"/pixmaps/handle2.png"));
   evas_show(view_evas, o_handle3 = evas_add_image_from_file(view_evas, PACKAGE_SOURCE_DIR"/pixmaps/handle3.png"));
   evas_show(view_evas, o_handle4 = evas_add_image_from_file(view_evas, PACKAGE_SOURCE_DIR"/pixmaps/handle4.png"));
   evas_show(view_evas, o_edge1 = evas_add_line(view_evas));
   evas_show(view_evas, o_edge2 = evas_add_line(view_evas));
   evas_show(view_evas, o_edge3 = evas_add_line(view_evas));
   evas_show(view_evas, o_edge4 = evas_add_line(view_evas));
   evas_show(view_evas, o_backing = evas_add_image_from_file(view_evas, PACKAGE_SOURCE_DIR"/pixmaps/backing.png"));
   evas_set_color(view_evas, o_edge1, 0, 0, 0, 255);
   evas_set_color(view_evas, o_edge2, 0, 0, 0, 255);
   evas_set_color(view_evas, o_edge3, 0, 0, 0, 255);
   evas_set_color(view_evas, o_edge4, 0, 0, 0, 255);
   evas_set_layer(view_evas, o_handle1, 5);
   evas_set_layer(view_evas, o_handle2, 5);
   evas_set_layer(view_evas, o_handle3, 5);
   evas_set_layer(view_evas, o_handle4, 5);
   evas_set_layer(view_evas, o_edge1, 4);
   evas_set_layer(view_evas, o_edge2, 4);
   evas_set_layer(view_evas, o_edge3, 4);
   evas_set_layer(view_evas, o_edge4, 4);
   evas_set_layer(view_evas, o_backing, 3);
   evas_callback_add(view_evas, o_handle1, CALLBACK_MOUSE_DOWN, handle_mouse_down, NULL);
   evas_callback_add(view_evas, o_handle1, CALLBACK_MOUSE_UP, handle_mouse_up, NULL);
   evas_callback_add(view_evas, o_handle1, CALLBACK_MOUSE_MOVE, handle_mouse_move, NULL);
   evas_callback_add(view_evas, o_handle2, CALLBACK_MOUSE_DOWN, handle_mouse_down, NULL);
   evas_callback_add(view_evas, o_handle2, CALLBACK_MOUSE_UP, handle_mouse_up, NULL);
   evas_callback_add(view_evas, o_handle2, CALLBACK_MOUSE_MOVE, handle_mouse_move, NULL);
   evas_callback_add(view_evas, o_handle3, CALLBACK_MOUSE_DOWN, handle_mouse_down, NULL);
   evas_callback_add(view_evas, o_handle3, CALLBACK_MOUSE_UP, handle_mouse_up, NULL);
   evas_callback_add(view_evas, o_handle3, CALLBACK_MOUSE_MOVE, handle_mouse_move, NULL);
   evas_callback_add(view_evas, o_handle4, CALLBACK_MOUSE_DOWN, handle_mouse_down, NULL);
   evas_callback_add(view_evas, o_handle4, CALLBACK_MOUSE_UP, handle_mouse_up, NULL);
   evas_callback_add(view_evas, o_handle4, CALLBACK_MOUSE_MOVE, handle_mouse_move, NULL);
   evas_callback_add(view_evas, o_backing, CALLBACK_MOUSE_DOWN, handle_mouse_down, NULL);
   evas_callback_add(view_evas, o_backing, CALLBACK_MOUSE_UP, handle_mouse_up, NULL);
   evas_callback_add(view_evas, o_backing, CALLBACK_MOUSE_MOVE, handle_mouse_move, NULL);

   view_configure_handles(NULL);
}

static gint
view_shrink_logo(gpointer data)
{
   double x, y, w, h, hh;
   
   if (!o_logo) return FALSE;
   evas_get_geometry(view_evas, o_logo, &x, &y, &w, &h);
   w -= 8;
   hh = h;
   h = h * (w / (w + 8));
   if ((w > 0) && (h > 0))
     {
	evas_move(view_evas, o_logo,
		  x + 4, y + ((hh - h) / 2));
	evas_resize(view_evas, o_logo, w, h);
	evas_set_image_fill(view_evas, o_logo, 0, 0, w, h);
     }
   if (w > 0)
     {
	gtk_idle_add(view_redraw, NULL);
	gtk_timeout_add(50, view_shrink_logo, NULL);
     }
   else
     {
	evas_del_object(view_evas, o_logo);
	o_logo = NULL;
	if (!o_handle1) view_create_handles(NULL);
	gtk_idle_add(view_redraw, NULL);
     }
   return FALSE;
}

static gint
view_scroll_logo(gpointer data)
{
   double x, y, w, h;
   int eh;

   if (!o_logo) return FALSE;
   evas_get_geometry(view_evas, o_logo, &x, &y, &w, &h);
   evas_get_drawable_size(view_evas, NULL, &eh);
   evas_move(view_evas, o_logo,
	     x, y + ((((eh - h) / 2) -y) / 10) + 1);
   if (y < ((eh - h) / 2))
     {
	gtk_idle_add(view_redraw, NULL);
	gtk_timeout_add(50, view_scroll_logo, NULL);
     }
   else
     {
	gtk_idle_add(view_redraw, NULL);
	gtk_timeout_add(3000, view_shrink_logo, NULL);
     }
   return FALSE;
}



void
on_open1_activate                      (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{

}


void
on_save1_activate                      (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{

}


void
on_save_as1_activate                   (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{

}


void
on_preferences1_activate               (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{

}


void
on_exit1_activate                      (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
   exit(0);
}


void
on_delete1_activate                    (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{

}


void
on_about1_activate                     (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
   int w, h, ew;

   if (o_logo)
     {
	evas_del_object(view_evas, o_logo);
	o_logo = NULL;
     }
   o_logo = evas_add_image_from_file(view_evas, 
				     PACKAGE_SOURCE_DIR"/pixmaps/etcher.png");
   evas_set_layer(view_evas, o_logo, 900);
   evas_show(view_evas, o_logo);
   evas_get_image_size(view_evas, o_logo, &w, &h);
   evas_get_drawable_size(view_evas, &ew, NULL);
   evas_move(view_evas, o_logo, 
	     (ew - w) / 2,
	     -h);
   gtk_timeout_add(50, view_scroll_logo, NULL);   
}


gboolean
on_view_motion_notify_event            (GtkWidget       *widget,
                                        GdkEventMotion  *event,
                                        gpointer         user_data)
{
   if (o_pointer)
     {
	evas_move(view_evas, o_pointer, event->x, event->y);
	gtk_idle_add(view_redraw, NULL);
     }
   evas_event_move(view_evas, event->x, event->y);
   return FALSE;
}


gboolean
on_view_button_press_event             (GtkWidget       *widget,
                                        GdkEventButton  *event,
                                        gpointer         user_data)
{
   evas_event_button_down(view_evas, event->x, event->y, event->button);
   return FALSE;
}


gboolean
on_view_button_release_event           (GtkWidget       *widget,
                                        GdkEventButton  *event,
                                        gpointer         user_data)
{
   evas_event_button_up(view_evas, event->x, event->y, event->button);
   return FALSE;
}

gboolean
on_view_enter_notify_event             (GtkWidget       *widget,
                                        GdkEventCrossing *event,
                                        gpointer         user_data)
{
   evas_show(view_evas, o_pointer);
   gtk_idle_add(view_redraw, NULL);
   return FALSE;
}


gboolean
on_view_leave_notify_event             (GtkWidget       *widget,
                                        GdkEventCrossing *event,
                                        gpointer         user_data)
{
   evas_hide(view_evas, o_pointer);
   gtk_idle_add(view_redraw, NULL);
   return FALSE;
}


gboolean
on_view_expose_event                   (GtkWidget       *widget,
                                        GdkEventExpose  *event,
                                        gpointer         user_data)
{
   if (!view_evas)
     {
	Evas_Object o_bg;
	int w, h;
	
	view_evas = evas_new();
	
	evas_set_output_method(view_evas, RENDER_METHOD_ALPHA_SOFTWARE);
	evas_set_output(view_evas, 
			GDK_WINDOW_XDISPLAY(widget->window), 
			GDK_WINDOW_XWINDOW(widget->window), 
			GDK_VISUAL_XVISUAL(gtk_widget_get_visual(widget)), 
			GDK_COLORMAP_XCOLORMAP(gtk_widget_get_colormap(widget)));
	evas_set_output_size(view_evas, 
			     widget->allocation.width, 
			     widget->allocation.height);
	evas_set_output_viewport(view_evas, 
				 0, 0, 
				 widget->allocation.width, 
				 widget->allocation.height);
	
	evas_set_font_cache(view_evas, 1 * 1024 * 1024);
	evas_set_image_cache(view_evas, 8 * 1024 * 1024);
	o_bg = evas_add_image_from_file(view_evas,
					  PACKAGE_SOURCE_DIR"/pixmaps/tile.png");
	evas_get_image_size(view_evas, o_bg, &w, &h);
	evas_set_image_fill(view_evas, o_bg, 0, 0, w, h);
	evas_show(view_evas, o_bg);
	evas_move(view_evas, o_bg, 0, 0);
	evas_resize(view_evas, o_bg, 9999, 9999);
	
	o_logo = evas_add_image_from_file(view_evas, 
					  PACKAGE_SOURCE_DIR"/pixmaps/etcher.png");
	evas_set_layer(view_evas, o_logo, 900);
	evas_show(view_evas, o_logo);
	evas_get_image_size(view_evas, o_logo, &w, &h);
	evas_move(view_evas, o_logo, 
		  (widget->allocation.width - w) / 2,
		  -h);
	backing_x = 32;
	backing_y = 32;
	backing_w = widget->allocation.width - 64;
	backing_h = widget->allocation.height - 64;
	o_pointer = evas_add_image_from_file(view_evas, PACKAGE_SOURCE_DIR"/pixmaps/pointer.png");
	evas_set_layer(view_evas, o_pointer, 999);
	evas_set_pass_events(view_evas, o_pointer, 1);	
	gtk_timeout_add(50, view_scroll_logo, NULL);
	  {
	     GdkPixmap *src, *mask;
	     GdkColor fg, bg;
	     GdkGC *gc;
	     GdkCursor *cursor;
	     
	     src = gdk_pixmap_new(widget->window, 1, 1, 1);
	     mask = gdk_pixmap_new(widget->window, 1, 1, 1);
	     gc = gdk_gc_new(mask);
	     fg.pixel = 0;
	     gdk_gc_set_foreground(gc, &fg);
	     gdk_draw_rectangle(mask, gc, 1, 0, 0, 1, 1);
	     cursor = gdk_cursor_new_from_pixmap(src, mask, &fg, &bg, 0, 0);
	     gdk_window_set_cursor(widget->window, cursor);
	     gdk_cursor_destroy(cursor);
	     gdk_pixmap_unref(src);
	     gdk_pixmap_unref(mask);
	     gdk_gc_destroy(gc);
	  }
     }
   evas_update_rect(view_evas, 
		    event->area.x,
		    event->area.y,
		    event->area.width,
		    event->area.height);
   gtk_idle_add(view_redraw, NULL);
   
   return FALSE;
}


gboolean
on_view_configure_event                (GtkWidget       *widget,
                                        GdkEventConfigure *event,
                                        gpointer         user_data)
{
   if (!widget->window) return;
   if (!view_evas) return;
   
   evas_set_output_size(view_evas, 
			widget->allocation.width, 
			widget->allocation.height);
   evas_set_output_viewport(view_evas, 
			    0, 0, 
			    widget->allocation.width, 
			    widget->allocation.height);
   evas_update_rect(view_evas, 
		    0, 0, 
		    widget->allocation.width,
		    widget->allocation.height);
   gtk_idle_add(view_redraw, NULL);
   return FALSE;
}


gboolean
on_zoom_configure_event                (GtkWidget       *widget,
                                        GdkEventConfigure *event,
                                        gpointer         user_data)
{

  return FALSE;
}


gboolean
on_zoom_expose_event                   (GtkWidget       *widget,
                                        GdkEventExpose  *event,
                                        gpointer         user_data)
{

  return FALSE;
}


void
on_images_select_row                   (GtkCList        *clist,
                                        gint             row,
                                        gint             column,
                                        GdkEvent        *event,
                                        gpointer         user_data)
{

}


void
on_states_select_row                   (GtkCList        *clist,
                                        gint             row,
                                        gint             column,
                                        GdkEvent        *event,
                                        gpointer         user_data)
{

}


void
on_new_image_clicked                   (GtkButton       *button,
                                        gpointer         user_data)
{

}


void
on_new_icon_clicked                    (GtkButton       *button,
                                        gpointer         user_data)
{

}


void
on_new_text_clicked                    (GtkButton       *button,
                                        gpointer         user_data)
{

}


void
on_raise_clicked                       (GtkButton       *button,
                                        gpointer         user_data)
{

}


void
on_lower_clicked                       (GtkButton       *button,
                                        gpointer         user_data)
{

}


void
on_delete_clicked                      (GtkButton       *button,
                                        gpointer         user_data)
{

}


void
on_reset_clicked                       (GtkButton       *button,
                                        gpointer         user_data)
{

}


void
on_prop_apply_clicked                  (GtkButton       *button,
                                        gpointer         user_data)
{

}


void
on_prop_reset_clicked                  (GtkButton       *button,
                                        gpointer         user_data)
{

}


void
on_draft_toggled                       (GtkToggleButton *togglebutton,
                                        gpointer         user_data)
{

}


gboolean
on_main_delete_event                   (GtkWidget       *widget,
                                        GdkEvent        *event,
                                        gpointer         user_data)
{
  exit(0);
  return FALSE;
}


gboolean
on_filesel_delete_event                (GtkWidget       *widget,
                                        GdkEvent        *event,
                                        gpointer         user_data)
{

  return FALSE;
}

