#include "ee2.h"

GtkWidget *t1, *t2;

void
fx_init(void)
{
  GtkWidget *nfr1, *nlbl, *vbox1, *btn;
	GtkWidget *b1, *b2;
	GtkWidget *l1;
	GtkAdjustment *adj, *adj2;

	GtkWidget *dead = gtk_label_new("  ");
	gtk_widget_show(dead);;

  nlbl = gtk_label_new("Effects/Filters");
  nfr1 = gtk_frame_new("Effects and Filters");
  gtk_container_set_border_width(GTK_CONTAINER(nfr1), 3);
  gtk_notebook_insert_page(GTK_NOTEBOOK(ModMdi), nfr1, nlbl, 8);

  vbox1 = gtk_vbox_new(FALSE, 0);
  gtk_container_add(GTK_CONTAINER(nfr1), vbox1);

	b1 = gtk_hbox_new(FALSE, 0);
	gtk_widget_show(b1);
	gtk_container_add(GTK_CONTAINER(vbox1), b1);

	l1 = gtk_label_new("Blur Amount");
	gtk_widget_show(l1);
	gtk_box_pack_start(GTK_BOX(b1), l1, FALSE, TRUE, 0);

	adj  = (GtkAdjustment *) gtk_adjustment_new(1, 0, 100, 1, 5, 0);
	adj2 = (GtkAdjustment *) gtk_adjustment_new(1, 0, 100, 1, 5, 0);
	t1 = gtk_spin_button_new(adj, 0, 0);
	gtk_widget_show(t1);
	gtk_box_pack_start(GTK_BOX(b1), t1, TRUE, TRUE, 0);

	btn = gtk_button_new_with_label("Blur");
	gtk_widget_set_usize(btn, 50, 7);
	gtk_signal_connect(GTK_OBJECT(btn), "clicked",
										GTK_SIGNAL_FUNC(fx_blur), NULL);
	gtk_widget_show(btn);
	gtk_box_pack_start(GTK_BOX(b1), btn, FALSE, FALSE, 0);

	b2 = gtk_hbox_new(FALSE, 0);
	gtk_widget_show(b2);
	gtk_box_pack_start(GTK_BOX(vbox1), b2, TRUE, TRUE, 0);

	l1 = gtk_label_new("Sharpen Amount");
	gtk_widget_show(l1);
	gtk_box_pack_start(GTK_BOX(b2), l1, FALSE, TRUE, 0);
	
	t2 = gtk_spin_button_new(adj2, 0, 0);
	gtk_widget_show(t2);
	gtk_box_pack_start(GTK_BOX(b2), t2, TRUE, TRUE, 0);

	btn = gtk_button_new_with_label("Sharpen");
	gtk_widget_set_usize(btn, 50, 7);
	gtk_signal_connect(GTK_OBJECT(btn), "clicked",
										GTK_SIGNAL_FUNC(fx_sharpen), NULL);
	gtk_widget_show(btn);
	gtk_box_pack_start(GTK_BOX(b2), btn, FALSE, TRUE, 0);

	/* this is an ugly hack */
	gtk_widget_set_usize(dead, 300, 310);
	gtk_box_pack_start(GTK_BOX(vbox1), dead, FALSE, TRUE, 0);

  gtk_widget_show(nfr1);
  gtk_widget_show(nlbl);
  gtk_widget_show(vbox1);
}

void
fx_blur(void)
{
	gint blum = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(t1));
	imlib_context_set_image(im);
	imlib_image_blur((int)blum);
	DrawImage(im, imlib_image_get_width(), imlib_image_get_height());
}

void
fx_sharpen(void)
{
	gint blum = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(t2));
	imlib_context_set_image(im);
	imlib_image_sharpen((int)blum);
	DrawImage(im, imlib_image_get_width(), imlib_image_get_height());
}
