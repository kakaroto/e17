/*****************************************
 * Electric Eyes 2                       *
 *****************************************
 * (c) 2000, Joshua Deere                *
 * dphase@locnet.net                     *
 *****************************************/

#include "ee2.h"

static GtkWidget *BrWin, *BrClist, *area2, *infol;

void
browser_init(void)
{
  GtkWidget *scroller, *hbox1, *vbox1, *frame1, *frame2, *btn;

  BrWin = gtk_window_new(GTK_WINDOW_TOPLEVEL);
  gtk_container_set_border_width(GTK_CONTAINER(BrWin), 2);
  gtk_window_set_title(GTK_WINDOW(BrWin), "Electric Eyes2 Image Browser");
	
	hbox1 = gtk_hbox_new(FALSE, 0);
	gtk_container_add(GTK_CONTAINER(BrWin), hbox1);

  scroller = gtk_scrolled_window_new(NULL, NULL);
  gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scroller),
				 GTK_POLICY_NEVER, GTK_POLICY_ALWAYS);
  gtk_box_pack_start(GTK_BOX(hbox1), scroller, TRUE, TRUE, 0);

  BrClist = gtk_clist_new(1);
  gtk_widget_set_usize(BrClist, 410, 150);
  gtk_container_add(GTK_CONTAINER(scroller), BrClist);
  gtk_widget_show(BrClist);
	
	vbox1 = gtk_vbox_new(FALSE, 0);
	gtk_box_pack_start(GTK_BOX(hbox1), vbox1, TRUE, TRUE, 0);
	gtk_widget_show(vbox1);
	
	frame1 = gtk_frame_new("Image Preview");
	gtk_box_pack_start(GTK_BOX(vbox1), frame1, TRUE, TRUE, 0);
	gtk_widget_show(frame1);
	
	area2 = gtk_drawing_area_new();
	gtk_widget_set_usize(area2, 125, 125);
	gtk_container_add(GTK_CONTAINER(frame1), area2);
	gtk_widget_show(area2);
	
	frame2 = gtk_frame_new(NULL);
	gtk_box_pack_start(GTK_BOX(vbox1), frame2, TRUE, TRUE, 0);
	gtk_frame_set_shadow_type(GTK_FRAME(frame2), GTK_SHADOW_IN);
	gtk_widget_show(frame2);
	
	infol = gtk_label_new("Resolution: UNKNOWN\n"
												"File Size: UNKNOWN\n"
												"Last Modification: UNKNOWN\n"
												"Has Alpha: UNKNOWN");
	gtk_label_set_justify(GTK_LABEL(infol), GTK_JUSTIFY_LEFT);
	gtk_container_add(GTK_CONTAINER(frame2), infol);
	gtk_widget_show(infol);

  gtk_widget_show(scroller);
	gtk_widget_show(hbox1);
}

void
browser_show(void)
{
  gtk_widget_show(BrWin);
}

void
browser_hide(void)
{
  gtk_widget_hide(BrWin);
}

void 
AddList(char *foo)
{
  gtk_clist_append(GTK_CLIST(BrClist), (gchar **) &foo);
}
