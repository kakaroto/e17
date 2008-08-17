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

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>

#include <gdk/gdkkeysyms.h>
#include <gdk-pixbuf/gdk-pixbuf.h>
#include <gtk/gtk.h>

#include <stdlib.h>

#include "hooks.h"
#include "controls.h"
#include "loadfiles.h"
#include "idle.h"


char *flipbook_framerate;
char *flipbook_drawrate;
char *flipbook_missed;
char *movie_name;
int base_framerate;

G_LOCK_DEFINE_STATIC (total_frames);
static volatile int total_frames = 0;

G_LOCK_DEFINE_STATIC (movie_height);
static volatile int movie_height = 0;

G_LOCK_DEFINE_STATIC (movie_width);
static volatile int movie_width = 0;
extern GtkAdjustment *adjust;

char *
get_current_framerate(void) {

	return flipbook_framerate;

}

void
set_current_framerate(char *new_framerate) {

	if(flipbook_framerate)
		free(flipbook_framerate);
	flipbook_framerate = malloc(strlen(new_framerate)+1);
	strcpy(flipbook_framerate,new_framerate);

}

char *
get_current_drawrate(void) {

	return flipbook_drawrate;

}

void
set_current_drawrate(char *new_drawrate) {

	if(flipbook_drawrate)
		free(flipbook_drawrate);
	flipbook_drawrate = malloc(strlen(new_drawrate)+1);
	strcpy(flipbook_drawrate,new_drawrate);

}

char *
get_current_missed(void) {

	return flipbook_missed;

}

void
set_current_missed(char *new_missed) {

	if(flipbook_missed)
		free(flipbook_missed);
	flipbook_missed = malloc(strlen(new_missed)+1);
	strcpy(flipbook_missed,new_missed);

}

void
set_framerate_base(int framerate) {

	base_framerate = framerate;
}

int
get_framerate_base(void) {

	return base_framerate;

}

void draw_next_frame(void) {

	if(drawspot) {

		GdkPixbuf *pixbuf;

		pixbuf = get_next_pic((adjust->value)+1);

		gtk_object_set_data(GTK_OBJECT(drawspot), "pixbuf", pixbuf);
		gdk_draw_rgb_image(drawspot->window,
				drawspot->style->white_gc,
				0, 0,
				get_width(),
				get_height(),
				GDK_RGB_DITHER_NORMAL,
				gdk_pixbuf_get_pixels(pixbuf),
				gdk_pixbuf_get_rowstride(pixbuf));
	}
}

void moved_frames(GtkWidget *widget, gpointer user_data);

void moved_frames(GtkWidget *widget, gpointer user_data) {

	if(user_data) {
		widget = NULL;
	}

	if(adjust->value != (get_current_index()+1)) {
		if(drawspot) {

			GdkPixbuf *pixbuf;

			pixbuf = get_next_pic(adjust->value);
			
			gtk_object_set_data(GTK_OBJECT(drawspot), "pixbuf", pixbuf);
			gdk_draw_rgb_image(drawspot->window,
					drawspot->style->white_gc,
					0, 0,
					get_width(),
					get_height(),
					GDK_RGB_DITHER_NORMAL,
					gdk_pixbuf_get_pixels(pixbuf),
					gdk_pixbuf_get_rowstride(pixbuf));

		}
	}

	return;
}

void set_total_frames(int frames) {

	G_LOCK(total_frames);
	total_frames = frames;
	G_UNLOCK(total_frames);

	if(control_slider) {
		GtkAdjustment *old;
		old=GTK_ADJUSTMENT(gtk_range_get_adjustment(GTK_RANGE(control_slider)));
		adjust = GTK_ADJUSTMENT(gtk_adjustment_new(1,1,frames+1,1,1,1));
		gtk_range_set_adjustment(GTK_RANGE(control_slider), adjust);
		gtk_adjustment_set_value(adjust,2);
		gtk_adjustment_value_changed(adjust);
		gtk_adjustment_set_value(adjust,1);
		gtk_adjustment_value_changed(adjust);

		gtk_signal_connect (GTK_OBJECT (adjust), "value_changed",
				GTK_SIGNAL_FUNC (moved_frames), NULL);
		if(GTK_IS_OBJECT(old))
			gtk_object_destroy(GTK_OBJECT(old));
	}

}

int get_total_frames(void) {

	int temp;

	G_LOCK(total_frames);
	temp = total_frames;
	G_UNLOCK(total_frames);

	return temp;

}

void set_width(int width) {

	G_LOCK(movie_width);
	movie_width = width;
	G_UNLOCK(movie_width);

}

int get_width(void) {

	int temp;
	G_LOCK(movie_width);
	temp = movie_width;
	G_UNLOCK(movie_width);

	return temp;

}

void set_height(int height) {

	G_LOCK(movie_height);
	movie_height = height;
	G_UNLOCK(movie_height);

}

int get_height(void) {

	int temp;
	G_LOCK(movie_height);
	temp = movie_height;
	G_UNLOCK(movie_height);

	return temp;

}

char *get_movie_name(void) {

	return movie_name;

}

void set_movie_name(char *name) {

	if(movie_name)
		free(movie_name);
	movie_name = malloc(strlen(name)+1);
	strcpy(movie_name,name);

}
