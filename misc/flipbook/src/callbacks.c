#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <gtk/gtk.h>

#include <stdio.h>
#include "callbacks.h"
#include "interface.h"
#include "support.h"


void on_preferences1_activate(GtkMenuItem * menuitem,
							  gpointer user_data)
{
	GtkWidget *Preferences_Window;

	if(user_data) {
		menuitem = NULL;
	}

	Preferences_Window = create_Preferences_Window();
	gtk_widget_show(Preferences_Window);

}

void on_choose_movie_segment1_activate(GtkMenuItem * menuitem,
									   gpointer user_data)
{

	GtkWidget *choose_segment;

	if(user_data) {
		menuitem = NULL;
	}
	
	choose_segment = create_choose_segment();
	gtk_widget_show(choose_segment);

}
