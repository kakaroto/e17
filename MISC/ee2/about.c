/*****************************************
 * Electric Eyes 2                       *
 *****************************************
 * (c) 2000, Joshua Deere                *
 * dphase@locnet.net                     *
 *****************************************/

#include "ee2.h"

static GtkWidget *AboutWindow;

void
about_init(void)
{
  GtkWidget *button, *label;

  AboutWindow = gtk_dialog_new();
  gtk_container_set_border_width(GTK_CONTAINER(AboutWindow), 7);
  gtk_window_set_title(GTK_WINDOW(AboutWindow), "About Electric Eyes 2");

  label = gtk_label_new("Electric Eyes 2\n"
                        "(C) 2000 Joshua Deere (distantPhase)\n"
                        "dphase@locnet.net\n\n"
                        "Please e-mail me with bug reports, feature\n"
                        "requests, and anything else that you might\n"
                        "think up that I would need to know.\n");
  gtk_box_pack_start(GTK_BOX(GTK_DIALOG(AboutWindow)->vbox), label, TRUE, TRUE, 0);
  gtk_widget_show(label);

  button = gtk_button_new_with_label("Close");
  gtk_signal_connect(GTK_OBJECT(button), "clicked", GTK_SIGNAL_FUNC(about_hide), NULL);
  gtk_box_pack_start(GTK_BOX(GTK_DIALOG(AboutWindow)->action_area), button, TRUE, TRUE, 0);
  gtk_widget_show(button);

}

void
about_show(void)
{
  gtk_widget_show(AboutWindow);
}

void
about_hide(void)
{
  gtk_widget_hide(AboutWindow);
}
