/*
 * $Id$
 *
 * Copyright (C) 2004 Tilman Sauerbeck (tilman at code-monkey de)
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of version 2 of the GNU General Public License as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307, USA.
 */

#include <Ecore.h>
#include <Ecore_X.h>
#include <Ecore_X_Cursor.h>

#include <X11/Xlib.h>
#include <Imlib2.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <limits.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>

#define _GNU_SOURCE
#include <getopt.h>

#define VERSION "0.0.2"

/* FIXME: should be configurable, but i'm too lazy to add the necessary
 * sanity checks :)
 */
#define FILE_FMT "enthrall_dump%.5lu"

typedef struct {
	Ecore_X_Display *disp;

	int fps;
	int quality;
	char output_dir[PATH_MAX];

	struct {
		Ecore_X_Window id;
		int w, h;
	} window;

	struct {
		Imlib_Image id;
		int w, h;
	} cursor;

	unsigned long frame_count;
} Enthrall;

static int
on_timer (void *udata)
{
	Enthrall *e = udata;
	Imlib_Image im;
	Bool b;
	int ptr_x = 0, ptr_y = 0, unused1;
	unsigned int unused2;
	char buf[PATH_MAX];
	Window dw, childw = None;

	/* FIXME: check whether e->window.id still points to a
	 *        valid window. not sure whether this really should be
	 *        done every time we enter this function.
	 */

	snprintf (buf, sizeof (buf), "%s/"FILE_FMT".jpeg",
	          e->output_dir, e->frame_count);

	im = imlib_create_image_from_drawable (0, 0, 0,
	                                       e->window.w, e->window.h,
	                                       true);
	imlib_context_set_image (im);

	/* if we have a cursor, find out where it's at */
	if (e->cursor.id) {
		b = XQueryPointer (e->disp, e->window.id, &dw, &childw,
		                   &unused1, &unused1, &ptr_x, &ptr_y, &unused2);

		if (b == True && childw != None)
			imlib_blend_image_onto_image (e->cursor.id, true, 0, 0,
			                              e->cursor.w, e->cursor.h,
			                              ptr_x, ptr_y,
			                              e->cursor.w, e->cursor.h);
	}

	imlib_image_attach_data_value ("quality", NULL, e->quality, NULL);
	imlib_image_set_format ("jpeg");

	imlib_save_image (buf);
	imlib_free_image ();

	e->frame_count++;

	return 1; /* keep going */
}

static void
show_usage ()
{
	printf ("enthrall " VERSION "\n\n"
	        "Usage: enthrall [options]\n\n"
	        "Options:\n"
	        "  -f, --fps=FPS               "
	        "frames per second (1-50, default: 25)\n"
	        "  -o, --output-directory=DIR  "
	        "output directory (default: working directory)\n"
	        "  -p, --pointer=FILE          "
	        "path to pointer image file\n"
	        "  -q, --quality=QUALITY       "
	        "JPEG quality (0-100, default: 90)\n"
	        "  -w, --window=WINDOW         "
	        "window to grab\n");
}

static void
init_imlib (Enthrall *e)
{
	/* FIXME: check whether this actually improves performance */
	imlib_set_cache_size (0);

	imlib_context_set_display (e->disp);
	imlib_context_set_visual (DefaultVisual (e->disp,
	                          DefaultScreen (e->disp)));
	imlib_context_set_colormap (DefaultColormap (e->disp,
	                            DefaultScreen (e->disp)));
	imlib_context_set_drawable (e->window.id);
}

bool
file_exists (char *file)
{
	struct stat st;

	if (!file || !*file)
		return false;

	return !stat (file, &st);
}

int
main (int argc, char **argv)
{
	Enthrall e;
	DIR *d;
	char pointer_img[PATH_MAX];
	double start;
	struct option options[] = {
		{"help", no_argument, NULL, 'h'},
		{"fps", required_argument, NULL, 'f'},
		{"output-directory", required_argument, NULL, 'o'},
		{"pointer", required_argument, NULL, 'p'},
		{"quality", required_argument, NULL, 'q'},
		{"window", required_argument, NULL, 'w'},
		{NULL, no_argument, NULL, 0}};
	int c;

	memset (&e, 0, sizeof (Enthrall));

	e.fps = 25;
	e.quality = 90;

	strcpy (e.output_dir, ".");

	while ((c = getopt_long (argc, argv, "hf:q:w:", options, NULL)) != -1) {
		int base;

		switch (c) {
			case 'h':
				show_usage ();
				return EXIT_SUCCESS;
			case 'f':
				e.fps = atoi (optarg);
				break;
			case 'o':
				snprintf (e.output_dir, sizeof (e.output_dir), "%s",
				          optarg);
				break;
			case 'p':
				snprintf (pointer_img, sizeof (pointer_img), "%s",
				          optarg);
				break;
			case 'q':
				e.quality = atoi (optarg);
				break;
			case 'w':
				base = strncasecmp (optarg, "0x", 2) ? 10 : 16;
				e.window.id = strtoul (optarg, NULL, base);
				break;
		}
	}

	if (!e.window.id) {
		show_usage ();

		return EXIT_FAILURE;
	}

	if (e.quality < 0 || e.quality > 100) {
		show_usage ();

		return EXIT_FAILURE;
	}

	if (e.fps < 1 || e.fps > 50) {
		show_usage ();

		return EXIT_FAILURE;
	}

	d = opendir (e.output_dir);
	if (!d) {
		fprintf (stderr, "Error: cannot open output directory.\n");

		return EXIT_FAILURE;
	}

	closedir (d);

	ecore_init ();
	ecore_x_init (NULL);

	start = ecore_time_get ();

	e.disp = ecore_x_display_get ();

	if (file_exists (pointer_img)) {
		e.cursor.id = imlib_load_image (pointer_img);
		imlib_context_set_image (e.cursor.id);
		e.cursor.w = imlib_image_get_width ();
		e.cursor.h = imlib_image_get_height ();
	}

	ecore_x_window_geometry_get (e.window.id, NULL, NULL,
	                             &e.window.w, &e.window.h);

	init_imlib (&e);

	ecore_timer_add (1.0 / e.fps, on_timer, &e);

	ecore_main_loop_begin ();

	ecore_x_shutdown ();
	ecore_shutdown ();

	if (e.cursor.id) {
		imlib_context_set_image (e.cursor.id);
		imlib_free_image ();
	}

	printf ("Wrote %lu frames in %f seconds.\n\n", e.frame_count,
	        ecore_time_get () - start);
	printf ("Suggested MEncoder call to encode the video:\n\n"
	        "mencoder \"mf://%s/*.jpeg\" \\\n"
	        "    -mf w=%i:h=%i:fps=%i:type=jpeg -ovc lavc \\\n"
	        "    -lavcopts vcodec=mpeg4:vbitrate=16000:vhq:autoaspect \\\n"
	        "    -o out.avi\n\n",
	        e.output_dir, e.window.w, e.window.h, e.fps);

	return EXIT_SUCCESS;
}
