#include <gtk/gtk.h>

void
pref_preferences1_activate               (GtkMenuItem     *menuitem,
					  gpointer         user_data);


void
pref_ok_clicked                          (GtkButton       *button,
					  gpointer         user_data);

void
pref_cancel_clicked                          (GtkButton       *button,
					      gpointer         user_data);

void
pref_tintcolor_button_clicked            (GtkButton       *button,
					  gpointer         user_data);

void
pref_color_changed(GtkWidget *widget, GtkColorSelection * colorsel);
