/* --------------------------------------
 * Retina - Evas powered image viewer
 * (C) 2000, Joshua Deere
 * dphase@dphase.net
 * --------------------------------------
 * See COPYING for license information
 * --------------------------------------
 */

#include "retina.h"
#include "../img/listing.xpm"

extern Evas_Object e_area, e_br_bg;
extern Evas_Object e_scr_t;
extern GtkWidget *window;
extern int br_status;

void
r_browser_dismiss()
{
}

void
r_browser_init()
{
	int w, h;
	e_br_bg = evas_add_image_from_file(e_area, "../img/browser_bg.png");
	evas_set_layer(e_area, e_br_bg, 1);
	evas_get_image_size(e_area, e_br_bg, &w, &h);
	evas_set_image_fill(e_area, e_br_bg, 0, 0, w, h);
	evas_move(e_area, e_br_bg, 0, 0);
	evas_resize(e_area, e_br_bg, 9999, 9999);

	/* setup scrollbar images */
	e_scr_t = evas_add_image_from_file(e_area, "../img/scrollbar_t.png");
	evas_set_layer(e_area, e_scr_t, 1);

	r_scrollbar_render();
}

void
r_scrollbar_render()
{
	int w, h;
	w = window->allocation.width;
	h = window->allocation.height;

	evas_set_image_fill(e_area, e_scr_t, 0, 0, 12, h);
	evas_resize(e_area, e_scr_t, 12, h);
	evas_move(e_area, e_scr_t, w-12, 0);
}

void
r_browser_add(char *img)
{
}
