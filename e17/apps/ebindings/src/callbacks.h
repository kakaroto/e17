/**************************************************************************
 * Name: callbacks.h
 * Project: Ebindings
 * Programmer: Corey Donohoe<atmos@atmos.org>
 * Date: October 10, 2001
 * Description: Gtk Callback function declarations
 *************************************************************************/
#ifndef __EBINDINGS_GTK_CALLBACKS_H_
#define __EBINDINGS_GTK_CALLBACKS_H_

#include "ebindings.h"

/* Convenience Macro */
#define GTK_ENTRY_SET_TEXT(_entry, _text) 	{ \
	if(_text && (strlen(_text) > 0))	{ \
		gtk_entry_set_text(GTK_ENTRY(_entry), _text); \
	} \
	else if(_entry)	{ \
		gtk_entry_set_text(GTK_ENTRY(_entry), ""); \
	} \
}

GtkWidget *ebindings_statusbar;

gint status_bar_clear(gpointer data);
void status_bar_message(gchar * message, gint delay);

#endif
