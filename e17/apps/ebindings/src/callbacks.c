/**************************************************************************
 * Name: callback.c 
 * Project: Ebindings
 * Programmer: Corey Donohoe<atmos@atmos.org>
 * Date: October 10, 2001
 * Description: Gtk Callback function definitions done here.  Every
 * important widget in the system has a name, as in gtk_widget_get_name()
 * returns the name of the widget.  This is used to identify widget and take
 * apprpriate actions as to modifying the data.  NOTE: Don't free the
 * repsonse to gtk_widget_get_name() or you won't be able to find the widget
 * by its name again. 
 *************************************************************************/
#include"callbacks.h"

gint
status_bar_clear(gpointer data)
{
   gtk_statusbar_pop(GTK_STATUSBAR(ebindings_statusbar), 1);
   return FALSE;
   UN(data);
}

void
status_bar_message(gchar * message, gint delay)
{
   gtk_statusbar_push(GTK_STATUSBAR(ebindings_statusbar), 1, message);
   gtk_timeout_add(delay, (GtkFunction) status_bar_clear, ebindings_statusbar);
   return;
   UN(delay);
   UN(message);
}
