/*
 * Copyright (C) 2000, Tuncer M. Ayaz
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies of the Software, its documentation and marketing & publicity
 * materials, and acknowledgment shall be given in the documentation, materials
 * and software packages that this Software was used.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#include <gtk/gtk.h>

/* when clicked on the button exit correctly */
gint
On_Btn_Clicked ( GtkWidget *widget,
						GdkEvent *event,
						gpointer data )
{
	gtk_main_quit();
	return (TRUE);
}

int 
main ( int argc, char **argv )
{
	GdkEventClient rcevent;	/* for sending the GDK_SIGNAL  */
	GtkWidget *wnd, *btn;
  	
	gtk_init (&argc, &argv);

	/* create window with button */
	wnd = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title(GTK_WINDOW(wnd),"E-GtkRc-Tester");
	gtk_signal_connect ( GTK_OBJECT (wnd), "destroy",
						GTK_SIGNAL_FUNC (gtk_exit), NULL);
	gtk_signal_connect ( GTK_OBJECT (wnd), "delete_event",
						GTK_SIGNAL_FUNC(gtk_exit), NULL);
	gtk_container_set_border_width(GTK_CONTAINER(wnd), 0);
	btn = gtk_button_new_with_label("This is how the selected theme looks like.");
	gtk_signal_connect ( GTK_OBJECT(btn), "clicked",
						GTK_SIGNAL_FUNC(On_Btn_Clicked), NULL);

	gtk_container_add ( GTK_CONTAINER(wnd), btn );
	/* make button DEFAULT-button */
	GTK_WIDGET_SET_FLAGS (btn, GTK_CAN_DEFAULT);
	gtk_widget_grab_default (btn);
	gtk_widget_show (btn);
	gtk_widget_show (wnd);

	/* send GDK_SIGNAL to reload the Gtk-Theme */
	rcevent.type = GDK_CLIENT_EVENT;
	rcevent.window = btn->window;
	rcevent.send_event = TRUE;
	rcevent.message_type = gdk_atom_intern("_GTK_READ_RCFILES", FALSE);
	rcevent.data_format = 8;
	gdk_event_send_clientmessage_toall((GdkEvent *)&rcevent);
	
	gtk_main();
 
	return (0);
}
