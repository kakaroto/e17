/* --------------------------------------
 * Retina - Evas powered image viewer
 * (C) 2000, Joshua Deere
 * dphase@dphase.net
 * --------------------------------------
 * See COPYING for license information
 * --------------------------------------
 */

#include "retina.h"
#include "../img/error.xpm"

void
r_error(char *msg)
{
	GtkWidget *w, *f, *logo, *btn, *lbl, *b1, *b2;
	GdkPixmap *gpix;
	GdkBitmap *gbit;
	char error_msg[255];

	w = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title(GTK_WINDOW(w), "Retina - ERROR");
	gtk_container_set_border_width(GTK_CONTAINER(w), 2);

	b1 = gtk_hbox_new(FALSE, 0);
	gtk_container_add(GTK_CONTAINER(w), b1);
	gtk_widget_show(b1);

	f = gtk_frame_new(NULL);
	gtk_frame_set_shadow_type(GTK_FRAME(f), GTK_SHADOW_IN);
	gtk_widget_show(f);
	gtk_box_pack_start(GTK_BOX(b1), f, FALSE, FALSE, 1);

	gpix = gdk_pixmap_create_from_xpm_d(GDK_ROOT_PARENT(), &gbit,
			NULL, error_xpm);
	logo = gtk_pixmap_new(gpix, gbit);
	gtk_widget_set_usize(GTK_WIDGET(logo), 85, 115);
	gtk_widget_show(logo);
	gtk_container_add(GTK_CONTAINER(f), logo);

	b2 = gtk_vbox_new(FALSE, 0);
	gtk_widget_set_usize(GTK_WIDGET(b2), 250, 110);
	gtk_widget_show(b2);
	gtk_box_pack_start(GTK_BOX(b1), b2, FALSE, FALSE, 1);

	sprintf(error_msg, "\n%s", msg);
	lbl = gtk_label_new(error_msg);
	gtk_widget_show(lbl);
	gtk_box_pack_start(GTK_BOX(b2), lbl, TRUE, TRUE, 1);

	btn = gtk_button_new_with_label("Dismiss");
	gtk_widget_show(btn);
	gtk_box_pack_start(GTK_BOX(b2), btn, FALSE, FALSE, 1);
	
	gtk_widget_show(w);
}
