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
#include <stdio.h>
#include <string.h>

#include <gdk/gdkkeysyms.h>
#include <gtk/gtk.h>
#include <gdk-pixbuf/gdk-pixbuf.h>


#include <stdlib.h>

#include "hooks.h"
#include "loadfiles.h"
#include "idle.h"
#include "support.h"

#include "controls.h"

extern char fastasicanplay;
extern GdkPixbuf *lastpixmap;

gint play_movie(gpointer data) {

	GdkPixbuf *pixbuf;
	if(data) {

	}

	pixbuf = get_next_pic(0);

	if(pixbuf == lastpixmap) {
		return 1;
	}

	lastpixmap = pixbuf;

	if(!pixbuf) {
		play_pixmap = create_pixmap(pause_pixmap, "play.xpm");
		gtk_container_remove(GTK_CONTAINER(play_button),pause_pixmap);
		gtk_widget_show(play_pixmap);
		gtk_container_add(GTK_CONTAINER(play_button),play_pixmap);
		return 0;
	}

	if(drawspot) {
		gtk_object_set_data(GTK_OBJECT(drawspot), "pixbuf", pixbuf);
		g_mutex_lock(gdk_threads_mutex);
		gdk_draw_rgb_image(drawspot->window,
				drawspot->style->white_gc,
				0, 0,
				get_width(),
				get_height(),
				GDK_RGB_DITHER_NORMAL,
				gdk_pixbuf_get_pixels(pixbuf),
				gdk_pixbuf_get_rowstride(pixbuf));
		g_mutex_unlock(gdk_threads_mutex);
		gtk_adjustment_set_value(adjust,get_current_index()+1);
		gtk_adjustment_changed(adjust);
	} else {
		play_pixmap = create_pixmap(pause_pixmap, "play.xpm");
		gtk_container_remove(GTK_CONTAINER(play_button),pause_pixmap);
		gtk_widget_show(play_pixmap);
		gtk_container_add(GTK_CONTAINER(play_button),play_pixmap);
		return 0;
	}
	return 1;
}
