/* --------------------------------------
 * Retina - Evas powered image viewer
 * (C) 2000, Joshua Deere
 * dphase@dphase.net
 * --------------------------------------
 * See COPYING for license information
 * --------------------------------------
 */

#include "retina.h"
#include "../img/listing.xpm"

void
r_browser_init()
{
	extern GtkWidget *bwin, *list;
	GtkWidget *fr, *btn, *sep, *logo, *b1;
	GdkPixmap *gpix;
	GdkBitmap *gbit;

	bwin = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title(GTK_WINDOW(bwin), "Retina - Browser");
	gtk_container_set_border_width(GTK_CONTAINER(bwin), 2);
	
	b1 = gtk_vbox_new(FALSE, 0);
	gtk_container_add(GTK_CONTAINER(bwin), b1);
	gtk_widget_show(b1);

	fr = gtk_frame_new(NULL);
	gtk_frame_set_shadow_type(GTK_FRAME(fr), GTK_SHADOW_IN);
	gtk_box_pack_start(GTK_BOX(b1), fr, FALSE, FALSE, 1);
	gtk_widget_show(fr);

	gpix = gdk_pixmap_create_from_xpm_d(GDK_ROOT_PARENT(), &gbit,
			NULL, listing_xpm);
	logo = gtk_pixmap_new(gpix, gbit);
	gtk_widget_set_usize(GTK_WIDGET(logo), 200, 25);
	gtk_widget_show(logo);
	gtk_container_add(GTK_CONTAINER(fr), logo);

	{
		gchar *listt[2] = {"  Thumbnail  ","  Filename  "};
		list = gtk_clist_new_with_titles(2, listt);
		gtk_box_pack_start(GTK_BOX(b1), list, TRUE, TRUE, 1);
		gtk_widget_set_usize(GTK_WIDGET(list), 200, 250);
		gtk_widget_show(list);
	}
}
