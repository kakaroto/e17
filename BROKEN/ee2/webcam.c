/* webcam stuff */

#include "ee2.h"

static GtkWidget *WCwin;

void
webcam_init(void)
{
  WCwin = gtk_window_new(GTK_WINDOW_TOPLEVEL);
  gtk_container_set_border_width(GTK_CONTAINER(WCwin), 2);
  gtk_window_set_title(GTK_WINDOW(WCwin), "Electric Eyes 2 WebCam Viewer");
}

void
webcam_show(void)
{
  gtk_widget_show(WCwin);
}

void
webcam_hide(void)
{
  gtk_widget_hide(WCwin);
}
