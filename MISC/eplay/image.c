/*
   Module       : image.c
   Purpose      : Routines dealing with image display
   More         : see eplay README
   Policy       : GNU GPL
   Homepage     : http://mandrake.net
 */

#include <stdio.h>
#include <string.h>
#include <gdk/gdkx.h>
#include <sys/time.h>
#include "eplay.h"

/*
 *    Load & display image
 */

void eplay_load_image()
{

	if (im) {
		gdk_imlib_kill_image(im);
		im = NULL;
	}

	if(!p[image_idx]) {
		im = gdk_imlib_load_image(image_names[image_idx]);
	}


	if (first) {
		w = org_w = im->rgb_width;
		h = org_h = im->rgb_height;
		win_x = (screen_x - w) / 2;
		win_y = (screen_y - h) / 2;
		attr.window_type = GDK_WINDOW_TEMP;
		attr.wclass = GDK_INPUT_OUTPUT;
		attr.event_mask = GDK_ALL_EVENTS_MASK;
		attr.x = attr.y = 0;
		attr.width = screen_x;
		attr.height = screen_y;
		parent = NULL;
		win = gdk_window_new(parent, &attr, ATTRIBUTES_MASK);
		gdk_window_show(win);
		gc = gdk_gc_new(win);
		first = 0;
		gdk_window_set_background(win, &color_bg);
		gdk_keyboard_grab(win, FALSE, CurrentTime);
		gdk_pointer_grab(win, FALSE, event_mask, NULL, cursor, CurrentTime);
		gdk_window_set_hints(win, win_x, win_y, w, h, w, h, GDK_HINT_MIN_SIZE |
							 GDK_HINT_MAX_SIZE | GDK_HINT_POS);
	}
/*	gdk_imlib_free_pixmap(p[image_idx]);  */  /* killing old pixmap */
	if(!p[image_idx]) {
		gdk_imlib_render(im, w, h);     /* Imlib render ... */
		p[image_idx] = gdk_imlib_move_image(im);    /* creating new */
	}

	gdk_draw_pixmap(win, gc, p[image_idx], 0, 0, win_x, win_y, w, h);
	gdk_window_show(win);       /* display image */
}

/*
   Set display settings to startup values
   which are used whenever a new image is loaded.
 */
void reset_display_settings(gint * win_x, gint * win_y, gint * w, gint * h)
{
	if (rotate_press && return_press) {
		gdk_imlib_destroy_image(im);
		im = gdk_imlib_load_image(image_names[image_idx]);
		*w = org_w = im->rgb_width;
		*h = org_h = im->rgb_height;
		*win_x = (screen_x - *w) / 2;
		*win_y = (screen_y - *h) / 2;
		rotate_press = return_press = 0;
	}
	*w = org_w;
	*h = org_h;
	*win_x = (screen_x - *w) / 2;
	*win_y = (screen_y - *h) / 2;
}
