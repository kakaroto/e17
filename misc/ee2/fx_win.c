#include "ee2.h"

void
fx_init(void)
{
  GtkWidget *nfr1, *nlbl, *vbox1, *sep, *btn;
	GtkWidget *b1, *b2;
	GtkWidget *v1, *c1, *l1, *t1;
	GtkAdjustment *adj;

	GtkWidget *dead = gtk_label_new("  ");
	gtk_widget_show(dead);;

  nlbl = gtk_label_new("Effects/Filters");
  nfr1 = gtk_frame_new("Effects and Filters");
  gtk_container_set_border_width(GTK_CONTAINER(nfr1), 3);
  gtk_notebook_insert_page(GTK_NOTEBOOK(ModMdi), nfr1, nlbl, 9);

  vbox1 = gtk_vbox_new(FALSE, 0);
  gtk_container_add(GTK_CONTAINER(nfr1), vbox1);

	b1 = gtk_hbox_new(FALSE, 0);
	gtk_widget_show(b1);
	gtk_container_add(GTK_CONTAINER(vbox1), b1);

	l1 = gtk_label_new("Blur Amount");
	gtk_widget_show(l1);
	gtk_box_pack_start(GTK_BOX(b1), l1, TRUE, TRUE, 0);

	adj = (GtkAdjustment *) gtk_adjustment_new(1, 0, 100, 1, 5, 0);
	t1 = gtk_spin_button_new(adj, 0, 0);
	gtk_widget_show(t1);
	gtk_box_pack_start(GTK_BOX(b1), t1, TRUE, TRUE, 0);

	btn = gtk_button_new_with_label("Blur");
	gtk_widget_show(btn);
	gtk_box_pack_start(GTK_BOX(b1), btn, TRUE, TRUE, 0);

	/* this is an ugly hack */
	gtk_widget_set_usize(dead, 300, 300);
	gtk_box_pack_start(GTK_BOX(vbox1), dead, TRUE, TRUE, 0);

  gtk_widget_show(nfr1);
  gtk_widget_show(nlbl);
  gtk_widget_show(vbox1);
}

