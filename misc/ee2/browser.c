/*****************************************
 * Electric Eyes 2                       *
 *****************************************
 * (c) 2000, Joshua Deere                *
 * dphase@locnet.net                     *
 *****************************************/

#include "ee2.h"

static GtkWidget *BrWin, *BrClist;

void
browser_init(void)
{
  GtkWidget *scroller, *button;

  BrWin = gtk_dialog_new();
  gtk_container_set_border_width(GTK_CONTAINER(BrWin), 2);
  gtk_window_set_title(GTK_WINDOW(BrWin), "Image Browser");

  scroller = gtk_scrolled_window_new(NULL, NULL);
  gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scroller),
				 GTK_POLICY_AUTOMATIC, GTK_POLICY_ALWAYS);
  gtk_box_pack_start(GTK_BOX(GTK_DIALOG(BrWin)->vbox),
		     scroller, TRUE, TRUE, 0);

  BrClist = gtk_clist_new(1);
  gtk_widget_set_usize(BrClist, 410, 150);
  gtk_container_add(GTK_CONTAINER(scroller), BrClist);
  gtk_widget_show(BrClist);

  button = gtk_button_new_with_label("Close");
  gtk_signal_connect(GTK_OBJECT(button), "clicked",
		     GTK_SIGNAL_FUNC(browser_hide), NULL);
  gtk_box_pack_start(GTK_BOX(GTK_DIALOG(BrWin)->action_area),
		     button, TRUE, TRUE, 0);
  gtk_widget_show(button);
  gtk_widget_show(scroller);
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
