#ifndef _EBONY_MANAGER_H
#define _EBONY_MANAGER_H

#include <stdio.h>
#include "interface.h"
#include "callbacks.h"

void manager_entry_changed(GtkWidget *w, gpointer data);
void manager_add_clicked(GtkWidget *w, gpointer data);
void manager_remove_clicked(GtkWidget *w, gpointer data);
void
bg_list_item_selected(GtkWidget *w, gint row, gint column, GdkEventButton *ev,
			gpointer data);

#endif
