/*
	brightness and other fun stuff
*/

#include "ee2.h"

GtkObject *adj;
double brite_val;

void
brite_init(void)
{
	GtkWidget *nfr1, *nlbl, *vbox1, *btn;
	GtkWidget *b1;
	GtkWidget *r1;

	nlbl = gtk_label_new("Brightness/Contrast");
	nfr1 = gtk_frame_new("Brightness/Contrast");
	gtk_container_set_border_width(GTK_CONTAINER(nfr1), 3);
	gtk_notebook_insert_page(GTK_NOTEBOOK(ModMdi), nfr1, nlbl, 5);
	gtk_widget_show(nlbl);
	gtk_widget_show(nfr1);

	vbox1 = gtk_vbox_new(FALSE, 0);
	gtk_container_add(GTK_CONTAINER(nfr1), vbox1);
	b1 = gtk_hbox_new(FALSE, 0);
	gtk_box_pack_start(GTK_BOX(vbox1), b1, FALSE, TRUE, 0);

	adj  = gtk_adjustment_new(0.0, -1.0, 1.0, 0.1, 0.5, 0.0);
	gtk_signal_connect(GTK_OBJECT(adj), "value_changed",
										GTK_SIGNAL_FUNC(brite_cha), NULL);

	r1 = gtk_hscale_new(GTK_ADJUSTMENT(adj));
	gtk_widget_show(r1);
	gtk_box_pack_start(GTK_BOX(b1), r1, TRUE, TRUE, 0);

	btn = gtk_button_new_with_label("Apply Brightness");
	gtk_signal_connect(GTK_OBJECT(btn), "clicked",
										GTK_SIGNAL_FUNC(brite_b), NULL);
	gtk_widget_show(btn);
	gtk_box_pack_start(GTK_BOX(b1), btn, FALSE, TRUE, 0);

	gtk_widget_show(vbox1);
	gtk_widget_show(b1);
}

void
brite_b(void)
{
	Imlib_Color_Modifier icm;
	
	imlib_context_set_image(im);
	icm = imlib_create_color_modifier();
	imlib_context_set_color_modifier(icm);
	imlib_modify_color_modifier_brightness(brite_val);
	imlib_apply_color_modifier();
	imlib_free_color_modifier();

	DrawImage(im, imlib_image_get_width(), imlib_image_get_height());
}

void
brite_cha(GtkAdjustment *a)
{
	brite_val = (double)a->value;
}
