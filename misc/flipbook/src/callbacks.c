#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <gtk/gtk.h>

#include <stdio.h>
#include "callbacks.h"
#include "interface.h"
#include "support.h"

GtkWidget *Preferences_Window=NULL;
GtkWidget *choose_segment=NULL;

void on_preferences1_close(GtkWidget * widget, gpointer user_data) {

	if(user_data) {
		widget = NULL;
	}

	gtk_widget_destroy (GTK_WIDGET(Preferences_Window));

	Preferences_Window=NULL;

}

void on_choose_movie_segment1_close(GtkWidget * widget, gpointer user_data) {

	if(user_data) {
		widget = NULL;
	}

	gtk_widget_destroy (GTK_WIDGET(choose_segment));
	choose_segment=NULL;

}

void on_exit_application(GtkWidget *widget, gpointer user_data)
{
	if(user_data) {
		widget = NULL;
	}

	gtk_exit(0);

}

void on_preferences1_activate(GtkMenuItem * menuitem, gpointer user_data)
{

	if(user_data) {
		menuitem = NULL;
	}

	if(!Preferences_Window) {
		Preferences_Window = create_Preferences_Window();
		gtk_widget_show(Preferences_Window);
		gtk_signal_connect (GTK_OBJECT (Preferences_Window), "destroy",
				GTK_SIGNAL_FUNC (on_preferences1_close), NULL);
		gtk_signal_connect (GTK_OBJECT (Preferences_Window), "delete_event",
				GTK_SIGNAL_FUNC (on_preferences1_close), NULL);
	}

	return;
}

void on_choose_movie_segment1_activate(GtkMenuItem * menuitem,
									   gpointer user_data)
{


	if(user_data) {
		menuitem = NULL;
	}
	
	if(!choose_segment) {
		choose_segment = create_choose_segment();
		gtk_widget_show(choose_segment);
		gtk_signal_connect (GTK_OBJECT (choose_segment), "destroy",
				GTK_SIGNAL_FUNC (on_choose_movie_segment1_close), NULL);
		gtk_signal_connect (GTK_OBJECT (choose_segment), "delete_event",
				GTK_SIGNAL_FUNC (on_choose_movie_segment1_close), NULL);
	}


	return;

}
