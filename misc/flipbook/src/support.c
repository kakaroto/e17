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

#include <gtk/gtk.h>

#include "support.h"

/* This is an internally used function to check if a pixmap file exists. */
static gchar *check_file_exists(const gchar * directory,
								const gchar * filename);

/* This is an internally used function to create pixmaps. */
static GtkWidget *create_dummy_pixmap(GtkWidget * widget);

/* This is a dummy pixmap we use when a pixmap can't be found. */
static char *dummy_pixmap_xpm[] =
{
/* columns rows colors chars-per-pixel */
	"1 1 1 1",
	"  c None",
/* pixels */
	" "
};

/* This is an internally used function to create pixmaps. */
static GtkWidget *
 create_dummy_pixmap(GtkWidget * widget)
{
	GdkColormap *colormap;
	GdkPixmap *gdkpixmap;
	GdkBitmap *mask;
	GtkWidget *pixmap;

	colormap = gtk_widget_get_colormap(widget);
	gdkpixmap = gdk_pixmap_colormap_create_from_xpm_d(NULL, colormap, &mask,
													  NULL, dummy_pixmap_xpm);
	if (gdkpixmap == NULL)
		g_error("Couldn't create replacement pixmap.");
	pixmap = gtk_pixmap_new(gdkpixmap, mask);
	gdk_pixmap_unref(gdkpixmap);
	gdk_bitmap_unref(mask);
	return pixmap;
}

static GList *pixmaps_directories = NULL;

/* Use this function to set the directory containing installed pixmaps. */
void add_pixmap_directory(const gchar * directory)
{
	pixmaps_directories = g_list_prepend(pixmaps_directories,
										 g_strdup(directory));
}

/* This is an internally used function to create pixmaps. */
GtkWidget *
 create_pixmap(GtkWidget * widget,
			   const gchar * filename)
{
	gchar *found_filename = NULL;
	GdkColormap *colormap;
	GdkPixmap *gdkpixmap;
	GdkBitmap *mask;
	GtkWidget *pixmap;
	GList *elem;

	/* We first try any pixmaps directories set by the application. */
	elem = pixmaps_directories;
	while (elem) {
		found_filename = check_file_exists((gchar *) elem->data, filename);
		if (found_filename)
			break;
		elem = elem->next;
	}

	/* If we haven't found the pixmap, try the source directory. */
	if (!found_filename) {
		found_filename = check_file_exists("../pixmaps", filename);
	}
	if (!found_filename) {
		g_warning("Couldn't find pixmap file: %s", filename);
		return create_dummy_pixmap(widget);
	}
	colormap = gtk_widget_get_colormap(widget);
	gdkpixmap = gdk_pixmap_colormap_create_from_xpm(NULL, colormap, &mask,
													NULL, found_filename);
	if (gdkpixmap == NULL) {
		g_warning("Error loading pixmap file: %s", found_filename);
		g_free(found_filename);
		return create_dummy_pixmap(widget);
	}
	g_free(found_filename);
	pixmap = gtk_pixmap_new(gdkpixmap, mask);
	gdk_pixmap_unref(gdkpixmap);
	gdk_bitmap_unref(mask);
	return pixmap;
}

/* This is an internally used function to check if a pixmap file exists. */
gchar *
 check_file_exists(const gchar * directory,
				   const gchar * filename)
{
	gchar *full_filename;
	struct stat s;
	gint status;

	full_filename = (gchar *) g_malloc(strlen(directory) + 1
									   + strlen(filename) + 1);
	strcpy(full_filename, directory);
	strcat(full_filename, G_DIR_SEPARATOR_S);
	strcat(full_filename, filename);

	status = stat(full_filename, &s);
	if (status == 0 && S_ISREG(s.st_mode))
		return full_filename;
	g_free(full_filename);
	return NULL;
}
