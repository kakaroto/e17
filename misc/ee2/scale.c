/*
	image scaling
*/

#include "ee2.h"

void
scale_init(void)
{
	GtkWidget *nfr1, *nlbl, *vbox1;
	GtkWidget *b1;
	GtkWidget *cur_lbl;
	int scw, sch;
	char lbl[255];

	nlbl = gtk_label_new("Image Scaling");
  nfr1 = gtk_frame_new("Image Scaling");
	gtk_container_set_border_width(GTK_CONTAINER(nfr1), 3);
	gtk_notebook_insert_page(GTK_NOTEBOOK(ModMdi), nfr1, nlbl, 6);
  gtk_widget_show(nlbl);
  gtk_widget_show(nfr1);

	vbox1 = gtk_vbox_new(FALSE, 0);
	gtk_container_add(GTK_CONTAINER(nfr1), vbox1);
	b1 = gtk_hbox_new(FALSE, 0);
	gtk_box_pack_start(GTK_BOX(vbox1), b1, FALSE, TRUE, 0);
	
	imlib_context_set_image(im);
	scw = imlib_image_get_width();
	sch = imlib_image_get_height();
		
	sprintf(lbl, "Current Width: %d\nCurrent Height: %d\n", scw, sch);
	cur_lbl = gtk_label_new(lbl);
	gtk_box_pack_start(GTK_BOX(vbox1), cur_lbl, FALSE, TRUE, 0);
	gtk_widget_show(cur_lbl);

	gtk_widget_show(vbox1);
	gtk_widget_show(b1);
}

