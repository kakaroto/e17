/* --------------------------------------
 * Retina - Evas powered image viewer
 * (C) 2000, Joshua Deere
 * dphase@dphase.net
 * --------------------------------------
 * See COPYING for license information
 * --------------------------------------
 */

#include "retina.h"

void
r_gtk_init()
{
	extern GtkWidget *window, *area;
	
	window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title(GTK_WINDOW(window), "Retina - Nothing Loaded...");

	area = gtk_drawing_area_new();
	gtk_container_add(GTK_CONTAINER(window), area);
	gtk_widget_show(area);
	gtk_widget_ref(area);
	gtk_widget_set_events(area, GDK_EXPOSURE_MASK | GDK_POINTER_MOTION_MASK |
		GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK | GDK_ENTER_NOTIFY_MASK |
		GDK_LEAVE_NOTIFY_MASK);
	gtk_signal_connect(GTK_OBJECT(area), "expose_event",
		GTK_SIGNAL_FUNC(r_evas_expose_event), NULL);
	gtk_signal_connect(GTK_OBJECT(area), "configure_event",
		GTK_SIGNAL_FUNC(r_evas_expose_event), NULL);
	gtk_signal_connect(GTK_OBJECT(area), "button_press_event",
		GTK_SIGNAL_FUNC(r_gtk_area_b_press), NULL);
	gtk_signal_connect(GTK_OBJECT(area), "button_release_event",
		GTK_SIGNAL_FUNC(r_gtk_area_b_release), NULL);
}

gboolean
r_gtk_area_b_press(GtkWidget *area, GdkEventButton *event, gpointer data)
{
	extern Evas e_area;
	extern mouse_button;

	mouse_button = event->button;
	
   printf("mouse down %f %f %i\n", event->x, event->y, event->button);
	evas_event_button_down(e_area, (int)event->x, (int)event->y, (int)event->button);
}

gboolean
r_gtk_area_b_release(GtkWidget *area, GdkEventButton *event, gpointer data)
{
	extern Evas e_area;
	extern mouse_button;

	mouse_button = event->button;
	
   printf("mouse up %f %f %i\n", event->x, event->y, event->button);
	evas_event_button_up(e_area, (int)event->x, (int)event->y, (int)event->button);
}
