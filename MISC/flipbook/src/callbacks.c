/*****************************************************************************/
/* VA Linux Systems Flipbook demo                                            */
/*****************************************************************************/
/*
 * Copyright (C) 2000 Brad Grantham, Geoff Harrison, and VA Linux Systems
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
#include <gdk-pixbuf/gdk-pixbuf.h>


#include <stdio.h>
#include <stdlib.h>
#include "callbacks.h"
#include "interface.h"
#include "support.h"
#include "loadfiles.h"
#include "hooks.h"
#include "idle.h"
#include "controls.h"

GtkWidget *Preferences_Window=NULL;
GtkWidget *choose_segment=NULL;
GtkWidget *open_template=NULL;
GtkWidget *movie_info=NULL;
extern GtkWidget *file_selector;
extern GtkWidget *drawspot;

GtkWidget *play_window=NULL;

static void
expose_func(GtkWidget * drawing_area, GdkEventExpose * event, gpointer data)
{
	GdkPixbuf *pixbuf;

	if(data) {

	}
	pixbuf =
		(GdkPixbuf *) gtk_object_get_data(GTK_OBJECT(drawing_area), "pixbuf");

	if (!pixbuf) {
		g_warning("art_pixbuf is NULL in expose_func!!\n");
		return;
	}

	gdk_draw_rgb_image(drawing_area->window,
			drawing_area->style->white_gc,
			event->area.x, event->area.y,
			event->area.width,
			event->area.height,
			GDK_RGB_DITHER_NORMAL,
			gdk_pixbuf_get_pixels(pixbuf)
			+ (event->area.y * gdk_pixbuf_get_rowstride(pixbuf))
			+ (event->area.x * gdk_pixbuf_get_n_channels(pixbuf)),
			gdk_pixbuf_get_rowstride(pixbuf));
}

void on_open_from_file(GtkWidget *widget, gpointer user_data) {

	if(user_data) {
		widget = NULL;
	}

	file_selector = gtk_file_selection_new("Please Select the Template "
			"File To Use");
	gtk_window_set_modal(GTK_WINDOW(file_selector), TRUE);
	gtk_file_selection_complete(GTK_FILE_SELECTION(file_selector), "*.movie");

	gtk_signal_connect_object(
			GTK_OBJECT(GTK_FILE_SELECTION(file_selector)->ok_button),
			"clicked", on_open_from_file_close, NULL);

	gtk_signal_connect_object(
			GTK_OBJECT(GTK_FILE_SELECTION(file_selector)->cancel_button),
			"clicked", GTK_SIGNAL_FUNC (gtk_widget_destroy),
			(gpointer) file_selector);
	gtk_widget_show(file_selector);
}

void on_open_from_file_close(GtkWidget *widget, gpointer user_data) {

	gchar *selected_filename;
	if(user_data) {
		widget = NULL;
	}

	selected_filename =
		gtk_file_selection_get_filename(GTK_FILE_SELECTION(file_selector));
	/*printf("selected %s\n",selected_filename); */
	unload_movie();
	load_my_list_of_files(selected_filename);
	load_files();
	gtk_widget_destroy (GTK_WIDGET(file_selector));

}

void on_preferences1_close(GtkWidget * widget, gpointer user_data) {

	if(user_data) {
		widget = NULL;
	}

	gtk_widget_destroy (GTK_WIDGET(Preferences_Window));

	Preferences_Window=NULL;

}

void on_save_preferences(GtkWidget *widget, gpointer user_data) {

	printf("saving user preferences.\n");
	on_preferences1_close(widget, user_data);

}

void on_choose_movie_segment1_close(GtkWidget * widget, gpointer user_data) {

	if(user_data) {
		widget = NULL;
	}

	gtk_widget_destroy (GTK_WIDGET(choose_segment));
	choose_segment=NULL;

}

void on_chose_segment(GtkWidget *widget, gpointer user_data) {

	printf("chose segment.\n");
	on_choose_movie_segment1_close(widget, user_data);

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

void on_close_movie(GtkWidget *widget, gpointer user_data) {

	if(user_data) {
		widget = NULL;
	}


	if(play_window) {
		gtk_widget_destroy (GTK_WIDGET(play_window));
		play_window = NULL;
		drawspot = NULL;

	}

	unload_movie();

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

void on_play_button(GtkWidget *widget, gpointer user_data) {

	if(user_data) {
		widget = NULL;
	}

	if(get_total_frames() == 0) {
		return;
	}

	if(!play_window) {
		GtkWidget *vbox;
		GtkWidget *drawing_area;
		GdkPixbuf *pic;


		play_window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
		gtk_object_set_data(GTK_OBJECT(play_window), "Animation", play_window);
		gtk_widget_set_usize(play_window, get_width(), get_height());
		{
			char s[256];
			sprintf(s,"VA Flipbook (%s)",get_movie_name());
			gtk_window_set_title(GTK_WINDOW(play_window),s); 
		}
		vbox = gtk_vbox_new(FALSE, 0);
		gtk_container_add(GTK_CONTAINER(play_window), vbox);
		gtk_widget_show(vbox);
		pic = get_next_pic(1);
		drawing_area = gtk_drawing_area_new();
		drawspot = drawing_area;
		gtk_drawing_area_size(GTK_DRAWING_AREA(drawing_area), get_width(), 
				get_height());
		gtk_signal_connect(GTK_OBJECT(drawing_area), "expose_event",
				GTK_SIGNAL_FUNC(expose_func), NULL);

		gtk_box_pack_start(GTK_BOX(vbox),drawing_area , FALSE, FALSE, 0);
		gtk_object_set_data(GTK_OBJECT(drawing_area), "pixbuf", pic);
		gtk_widget_show(drawing_area);

		gtk_widget_show_all(play_window);

		gtk_signal_connect (GTK_OBJECT (play_window), "destroy",
				GTK_SIGNAL_FUNC (on_close_movie), NULL);
		gtk_signal_connect (GTK_OBJECT (play_window), "delete_event",
				GTK_SIGNAL_FUNC (on_close_movie), NULL);


		gtk_widget_hide(play_pixmap);
		pause_pixmap = create_pixmap(play_pixmap, "pause.xpm");
		gtk_container_remove(GTK_CONTAINER(play_button),play_pixmap);
		gtk_widget_show(pause_pixmap);
		gtk_container_add(GTK_CONTAINER(play_button),pause_pixmap);

		launch_monitor();
		if(fastasicanplay) {
			gtk_idle_add(play_movie,NULL);
		} else {
			gtk_timeout_add(30,play_movie,NULL);
		}

	} else {
		get_next_pic(-1);
	}

}

void on_rewind_button(GtkWidget *widget, gpointer user_data) {

	int framelist;
	int index;
	if(user_data) {
		widget = NULL;
	}

	if((framelist = get_total_frames()) == 0) {
		return;
	}

	index = get_current_index();

	if(index > 0) {
		gtk_adjustment_set_value(adjust,index);
	} else {
		gtk_adjustment_set_value(adjust,framelist);
	}

	gtk_adjustment_value_changed(adjust);

	return;

}

void on_forward_button(GtkWidget *widget, gpointer user_data) {

	int framelist;
	int index;

	if(user_data) {
		widget = NULL;
	}

	if((framelist = get_total_frames()) == 0) {
		return;
	}

	index = get_current_index();
	gtk_adjustment_value_changed(adjust);

	if(index < framelist-1) {
		gtk_adjustment_set_value(adjust,index+2);
	} else {
		gtk_adjustment_set_value(adjust,1);
	}

	gtk_adjustment_value_changed(adjust);
}

