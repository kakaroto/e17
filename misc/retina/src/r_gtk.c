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
   extern GtkWidget *window, *area, *filesel;
   
   window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	 gtk_window_set_default_size(GTK_WINDOW(window), 300, 180);
   gtk_window_set_title(GTK_WINDOW(window), "Retina - Nothing Loaded...");

   area = gtk_drawing_area_new();
   gtk_container_add(GTK_CONTAINER(window), area);
   gtk_widget_set_events(area, 
			 GDK_EXPOSURE_MASK | 
			 GDK_POINTER_MOTION_MASK |
			 GDK_BUTTON_PRESS_MASK | 
			 GDK_BUTTON_RELEASE_MASK | 
			 GDK_ENTER_NOTIFY_MASK |
			 GDK_LEAVE_NOTIFY_MASK | 
			 GDK_STRUCTURE_MASK);

   gtk_signal_connect(GTK_OBJECT(area), "expose_event",
		      GTK_SIGNAL_FUNC(r_evas_expose_event), NULL);
   gtk_signal_connect(GTK_OBJECT(area), "configure_event",
		      GTK_SIGNAL_FUNC(r_evas_config_event), NULL);
   gtk_signal_connect(GTK_OBJECT(area), "button_press_event",
		      GTK_SIGNAL_FUNC(r_gtk_area_b_press), NULL);
   gtk_signal_connect(GTK_OBJECT(area), "button_release_event",
		      GTK_SIGNAL_FUNC(r_gtk_area_b_release), NULL);
	 
	 gtk_signal_connect(GTK_OBJECT(window), "key_press_event",
	 		GTK_SIGNAL_FUNC(r_gtk_keypress), NULL);
	 gtk_signal_connect(GTK_OBJECT(window), "key_release_event",
	 		GTK_SIGNAL_FUNC(r_gtk_keyrelease), NULL);

	 gtk_widget_show(area);

	 filesel = gtk_file_selection_new("Retina - Load Image...");
	 gtk_signal_connect(GTK_OBJECT(GTK_FILE_SELECTION(filesel)->ok_button),
	 	"clicked", (GtkSignalFunc) r_filesel_openfile, filesel);
	 gtk_signal_connect(GTK_OBJECT(GTK_FILE_SELECTION(filesel)->cancel_button),
		"clicked", (GtkSignalFunc) r_filesel_hide, filesel);
}

gboolean
r_gtk_area_b_press(GtkWidget *area, GdkEventButton *event, gpointer data)
{
   extern Evas e_area;
   extern int mouse_button;
   
   mouse_button = event->button;
   
   evas_event_button_down(e_area, event->x, event->y, event->button);
   return TRUE;
}

gboolean
r_gtk_area_b_release(GtkWidget *area, GdkEventButton *event, gpointer data)
{
   extern Evas e_area;
   extern int mouse_button;
   
   mouse_button = event->button;
   
   evas_event_button_up(e_area, event->x, event->y, event->button);
   return TRUE;
}

gint
r_gtk_keypress(GtkWidget *area, GdkEventKey *event)
{
	switch(event->keyval){
		case GDK_Down:
			r_browser_move_down();
			return 1;
		case GDK_Up:
			r_browser_move_up();
			return 1;
	}
	return 0;
}

gint
r_gtk_keyrelease(GtkWidget *area, GdkEventKey *event)
{
	printf("up\n");
	return 0;
}
