/*****************************************************************************/
/* VA Linux Systems Flipbook demo                                            */
/*****************************************************************************/
/*
 * Copyright (C) 1999 Brad Grantham, Geoff Harrison, and VA Linux Systems
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies of the Software, its documentation and marketing & publicity
 * materials, and acknowledgment shall be given in the documentation, materials
 * and software packages that this Software was used.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

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
GtkWidget *open_template=NULL;
GtkWidget *movie_info=NULL;
extern GtkWidget *file_selector;

void on_open_from_file(GtkWidget *widget, gpointer user_data) {


	if(user_data) {
		widget = NULL;
	}

	if(!file_selector) {
	file_selector = gtk_file_selection_new("Please Select the Template "
			"File To Use");
	gtk_window_set_modal(GTK_WINDOW(file_selector), TRUE);

	gtk_signal_connect_object(GTK_OBJECT(GTK_FILE_SELECTION(file_selector)->ok_button),
			"clicked", GTK_SIGNAL_FUNC (gtk_widget_destroy),
			(gpointer) file_selector);

	gtk_signal_connect_object(GTK_OBJECT(GTK_FILE_SELECTION(file_selector)->cancel_button),
			"clicked", GTK_SIGNAL_FUNC (gtk_widget_destroy),
			(gpointer) file_selector);
	gtk_widget_show(file_selector);
	}
}

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

void on_open_template_close(GtkWidget *widget, gpointer user_data) {

	if(user_data) {
		widget = NULL;
	}

	gtk_widget_destroy (GTK_WIDGET(open_template));
	open_template=NULL;

}

void on_open_from_template(GtkWidget *widget, gpointer user_data) {

	if(user_data) {
		widget = NULL;
	}

	if(!open_template) {
		open_template = create_open_template();
		gtk_widget_show(open_template);
		gtk_signal_connect (GTK_OBJECT (open_template), "destroy",
				GTK_SIGNAL_FUNC (on_open_template_close), NULL);
		gtk_signal_connect (GTK_OBJECT (open_template), "delete_event",
				GTK_SIGNAL_FUNC (on_open_template_close), NULL);
	}

}

void on_movie_info_close(GtkWidget *widget, gpointer user_data) {

	if(user_data) {
		widget = NULL;
	}

	gtk_widget_destroy (GTK_WIDGET(movie_info));
	movie_info=NULL;

}


void on_movie_info(GtkWidget *widget, gpointer user_data) {

	if(user_data) {
		widget = NULL;
	}

	if(!movie_info) {
		movie_info = create_movie_info();
		gtk_widget_show(movie_info);
		gtk_signal_connect (GTK_OBJECT (movie_info), "destroy",
				GTK_SIGNAL_FUNC (on_movie_info_close), NULL);
		gtk_signal_connect (GTK_OBJECT (movie_info), "delete_event",
				GTK_SIGNAL_FUNC (on_movie_info_close), NULL);
	}

}
