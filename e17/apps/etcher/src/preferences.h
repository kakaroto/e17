#ifndef ETCHER_PREF_H
#define ETCHER_PREF_H

#include <gtk/gtk.h>

int
pref_init(void);


void
pref_defaults(void);

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

void
pref_gridimage_changed                   (GtkEditable     *editable,
					  gpointer         user_data);

#endif
