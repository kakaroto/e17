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

#define VERSION "0.0.2"

#define IMG_FROM_RECT(r) \
	imlib_create_image_from_drawable (0, \
	                                  (r).x, (r).y, (r).width, (r).height, \
	                                  true)
#define MIN(a, b) (a) < (b) ? (a) : (b)
#define MAX(a, b) (a) > (b) ? (a) : (b)

typedef struct Enthrall Enthrall;
struct Enthrall {
	Ecore_X_Display *disp;

	struct {
		Ecore_X_Window id;
		int w, h;
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

	int (*render)(Enthrall *e);
};

static int
render_damage(Enthrall *e)
{
	Imlib_Image tmp;
	int result = 0;

	/* was there any change at all?
	 * if not, just link the last written frame to the current.
	 */
	if (!e->damage_valid) {
		goto out;
	}

	/* grab the damaged rectangle */
	tmp = IMG_FROM_RECT (e->damage);
	if (!tmp) {
		result = -1;
		goto out;
	}

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
	e->damage_valid = false;

out:
	e->frame_count++;
	return result;
}

static int
render_nodamage(Enthrall *e)
{
	Imlib_Image tmp;
	Ecore_X_Rectangle update;
	int result = 0;

	update.x = 0;
	update.y = 0;
	update.width = e->window.w;
	update.height = e->window.h;

	/* grab the current rectangle */
	tmp = IMG_FROM_RECT (update);
	if (!tmp) {
		result = -1;
		goto out;
	}

	/* and blend it onto the previous shot */
	imlib_blend_image_onto_image (tmp, true, 0, 0,
	                              update.width, update.height,
	                              update.x + e->window.offset_x,
	                              update.y + e->window.offset_y,
	                              update.width, update.height);

	/* free the temporary grab */
	imlib_context_set_image (tmp);
	imlib_free_image ();
	imlib_context_set_image (e->prev_img);

out:
	e->frame_count++;
	return result;
}

static int
on_timer (void *udata)
{
	Bool b;
	int result;
	uint32_t *data;
	int ptr_x = 0, ptr_y = 0, unused1;
	unsigned int unused2;
	Window dw, childw = None;
	Enthrall *e = udata;

	/* FIXME: check whether e->window.id still points to a
	 *        valid window. not sure whether this really should be
	 *        done every time we enter this function.
	 */

	result = e->render(e);
	if (result < 0) {
		fprintf(stderr, "Failed to render frame... exiting.\n");

		enthrall_theora_encode_frame (&e->theora, NULL);
		exit (result);
	}

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
	enthrall_theora_encode_frame (&e->theora, data);
	imlib_image_put_back_data (data);

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
	int w16, h16;
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

	/*
	 * Setup our render callback based on whether we support the damage
	 * extension on this display.
	 */
	if (ecore_x_damage_query ()) {
		e.render = render_damage;
		ecore_x_damage_new (e.window.id,
					ECORE_X_DAMAGE_REPORT_RAW_RECTANGLES);
	}
	else
		e.render = render_nodamage;

	start = ecore_time_get ();

	e.disp = ecore_x_display_get ();

	if (file_exists (pointer_img)) {
		e.cursor.id = imlib_load_image (pointer_img);
		imlib_context_set_image (e.cursor.id);
		e.cursor.w = imlib_image_get_width ();
		e.cursor.h = imlib_image_get_height ();
	}

	ecore_x_window_size_get (e.window.id, &e.window.w, &e.window.h);

	init_imlib (&e);

	w16 = e.window.w;
	h16 = e.window.h;

	s = enthrall_theora_init (&e.theora, output_file,
	                          quality, &w16, &h16,
	                          &e.window.offset_x, &e.window.offset_y);
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

	e.prev_img = imlib_create_image (w16, h16);
	imlib_context_set_image (e.prev_img);

	/* init image data */
	data = imlib_image_get_data ();
	memset (data, 0, w16 * h16 * 4);
	imlib_image_put_back_data (data);

	tmp = IMG_FROM_RECT (e.damage);
	imlib_blend_image_onto_image (tmp, true, 0, 0,
	                              e.window.w, e.window.h,
	                              e.window.offset_x, e.window.offset_y,
	                              e.window.w, e.window.h);

	printf ("Starting recording...\n");
	ecore_main_loop_begin ();

	enthrall_theora_encode_frame (&e.theora, NULL);
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
