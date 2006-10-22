/*
 * Copyright (C) 2004-2006 Tilman Sauerbeck (tilman at code-monkey de)
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

#define _GNU_SOURCE
#include <getopt.h>

#include "theora.h"
#include "rgb2yuv420.h"

#define VERSION "0.0.2"

/* FIXME: should be configurable, but i'm too lazy to add the necessary
 * sanity checks :)
 */
#define FILE_FMT "enthrall_dump%.5lu"

#define IMG_FROM_RECT(r) \
	imlib_create_image_from_drawable (0, \
	                                  (r).x, (r).y, (r).width, (r).height, \
	                                  true)
#define MIN(a, b) (a) < (b) ? (a) : (b)
#define MAX(a, b) (a) > (b) ? (a) : (b)

typedef struct {
	Ecore_X_Display *disp;

	struct {
		Ecore_X_Window id;
		int w, h, w16, h16;
		int offset_x, offset_y;
	} window;

	struct {
		Imlib_Image id;
		int w, h;
	} cursor;

	Imlib_Image prev_img;
	Ecore_X_Rectangle damage;
	bool damage_valid;

	unsigned long frame_count;

	EnthrallTheora theora;

	uint8_t *y, *u, *v;
} Enthrall;

static int
on_timer (void *udata)
{
	Enthrall *e = udata;
	Imlib_Image tmp;
	Bool b;
	int ptr_x = 0, ptr_y = 0, unused1;
	unsigned int unused2;
	uint32_t *data;
	Window dw, childw = None;

	/* FIXME: check whether e->window.id still points to a
	 *        valid window. not sure whether this really should be
	 *        done every time we enter this function.
	 */

	/* was there any change at all?
	 * if not, just link the last written frame to the current.
	 */
	if (!e->damage_valid) {
		/* FIXME:
		 * According to this ticket
		 * https://trac.xiph.org/changeset/11119
		 * it seems we can just put in an empty packet to repeat
		 * the last frame.
		 */
		enthrall_theora_encode_frame (&e->theora, false);
		goto out;
	}

	/* grab the damaged rectangle */
	tmp = IMG_FROM_RECT (e->damage);

	/* and blend it onto the previous shot */
	imlib_blend_image_onto_image (tmp, true, 0, 0,
	                              e->damage.width, e->damage.height,
	                              e->damage.x + e->window.offset_x,
	                              e->damage.y + e->window.offset_y,
	                              e->damage.width, e->damage.height);

	/* free the temporary grab */
	imlib_context_set_image (tmp);
	imlib_free_image ();
	imlib_context_set_image (e->prev_img);

	/* if we have a cursor, find out where it's at */
	if (e->cursor.id) {
		b = XQueryPointer (e->disp, e->window.id, &dw, &childw,
		                   &unused1, &unused1, &ptr_x, &ptr_y, &unused2);

		if (b == True)
			imlib_blend_image_onto_image (e->cursor.id, true, 0, 0,
			                              e->cursor.w, e->cursor.h,
			                              ptr_x + e->window.offset_x,
			                              ptr_y + e->window.offset_y,
			                              e->cursor.w, e->cursor.h);
	}

	data = imlib_image_get_data_for_reading_only ();
	rgb2yuv420 (data, e->window.w16, e->window.h16, e->y, e->u, e->v);
	imlib_image_put_back_data (data);

	enthrall_theora_encode_frame (&e->theora, false);

	e->damage_valid = false;

out:
	e->frame_count++;

	return 1; /* keep going */
}

static inline void
combine_rects (Ecore_X_Rectangle *a, Ecore_X_Rectangle *b)
{
	int ax2 = a->x + a->width;
	int ay2 = a->y + a->height;

	int bx2 = b->x + b->width;
	int by2 = b->y + b->height;

	a->x = MIN (a->x, b->x);
	a->y = MIN (a->y, b->y);

	a->width = MAX (ax2, bx2) - a->x;
	a->height = MAX (ay2, by2) - a->y;
}

static int
on_damage (void *udata, int type, void *event)
{
	Enthrall *e = udata;
	Ecore_X_Event_Damage *ev = event;

	if (e->damage_valid)
		combine_rects (&e->damage, &ev->area);
	else {
		e->damage.x = ev->area.x;
		e->damage.y = ev->area.y;
		e->damage.width = ev->area.width;
		e->damage.height = ev->area.height;
		e->damage_valid = true;
	}

	return 0;
}

static void
show_usage ()
{
	printf ("enthrall " VERSION "\n\n"
	        "Usage: enthrall [options]\n\n"
	        "Options:\n"
	        "  -f, --fps=FPS               "
	        "frames per second (1-50, default: 25)\n"
	        "  -o, --output-file=F         "
	        "output file\n"
	        "  -p, --pointer=FILE          "
	        "path to pointer image file\n"
	        "  -q, --quality=QUALITY       "
	        "video quality (0-100, default: 90)\n"
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
	Imlib_Image tmp;
	char pointer_img[PATH_MAX], output_file[PATH_MAX] = {0};
	uint32_t *data;
	double start;
	bool s;
	int fps = 25, quality = 90;
	struct option options[] = {
		{"help", no_argument, NULL, 'h'},
		{"fps", required_argument, NULL, 'f'},
		{"output-file", required_argument, NULL, 'o'},
		{"pointer", required_argument, NULL, 'p'},
		{"quality", required_argument, NULL, 'q'},
		{"window", required_argument, NULL, 'w'},
		{NULL, no_argument, NULL, 0}};
	int c;

	memset (&e, 0, sizeof (Enthrall));

	while ((c = getopt_long (argc, argv, "hf:o:p:q:w:", options, NULL)) != -1) {
		int base;

		switch (c) {
			case 'h':
				show_usage ();
				return EXIT_SUCCESS;
			case 'f':
				fps = atoi (optarg);
				break;
			case 'o':
				snprintf (output_file, sizeof (output_file), "%s",
				          optarg);
				break;
			case 'p':
				snprintf (pointer_img, sizeof (pointer_img), "%s",
				          optarg);
				break;
			case 'q':
				quality = atoi (optarg);
				break;
			case 'w':
				base = strncasecmp (optarg, "0x", 2) ? 10 : 16;
				e.window.id = strtoul (optarg, NULL, base);
				break;
		}
	}

	if (!e.window.id || !*output_file) {
		show_usage ();

		return EXIT_FAILURE;
	}

	if (quality < 0 || quality > 100) {
		show_usage ();

		return EXIT_FAILURE;
	}

	if (fps < 1 || fps > 50) {
		show_usage ();

		return EXIT_FAILURE;
	}

	ecore_init ();
	ecore_x_init (NULL);

	if (!ecore_x_damage_query ()) {
		fprintf (stderr, "Error: X damage extension not available.\n");

		return EXIT_FAILURE;
	}

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

	ecore_x_damage_new (e.window.id,
		                ECORE_X_DAMAGE_REPORT_RAW_RECTANGLES);

	init_imlib (&e);

	e.window.w16 = e.window.w;
	e.window.h16 = e.window.h;

	s = enthrall_theora_init (&e.theora, output_file,
	                          quality, &e.window.w16, &e.window.h16,
	                          &e.window.offset_x, &e.window.offset_y,
	                          &e.y, &e.u, &e.v);
	if (!s) {
		fprintf (stderr, "Error: Cannot initialize theora encoder.\n");

		return EXIT_FAILURE;
	}

	ecore_timer_add (1.0 / fps, on_timer, &e);

	ecore_event_handler_add (ECORE_X_EVENT_DAMAGE_NOTIFY,
	                         on_damage, &e);

	e.damage.x = e.damage.y = 0;
	e.damage.width = e.window.w;
	e.damage.height = e.window.h;
	e.damage_valid = true;

	e.prev_img = imlib_create_image (e.window.w16, e.window.h16);
	imlib_context_set_image (e.prev_img);

	/* init image data */
	data = imlib_image_get_data ();
	memset (data, 0, e.window.w16 * e.window.h16 * 4);
	imlib_image_put_back_data (data);

	tmp = IMG_FROM_RECT (e.damage);
	imlib_blend_image_onto_image (tmp, true, 0, 0,
	                              e.window.w, e.window.h,
	                              e.window.offset_x, e.window.offset_y,
	                              e.window.w, e.window.h);

	printf ("Starting recording...\n");
	ecore_main_loop_begin ();

	enthrall_theora_encode_frame (&e.theora, true);
	enthrall_theora_finish (&e.theora);

	ecore_x_shutdown ();
	ecore_shutdown ();

	if (e.cursor.id) {
		imlib_context_set_image (e.cursor.id);
		imlib_free_image ();
	}

	imlib_context_set_image (e.prev_img);
	imlib_free_image ();

	printf ("Wrote %lu frames in %f seconds.\n\n", e.frame_count,
	        ecore_time_get () - start);

	return EXIT_SUCCESS;
}
