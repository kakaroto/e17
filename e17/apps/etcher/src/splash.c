#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <Edb.h>
#include <gdk/gdkx.h>

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "macros.h"
#include "splash.h"
#include "support.h"

static gint     already_running(void);
static gint     view_shrink_logo(gpointer data);
static gint     view_fade_info(gpointer data);
static gint     view_scroll_logo(gpointer data);
static gint     view_scroll_info(gpointer data);

int             new_fade = 0;
Evas            splash_evas;
Evas_Object     o_logo = NULL, o_info1 = NULL, o_info2, o_info3, o_info4;
gint           *splash_idle;
GtkFunction     splash_evas_redraw;


static          gint
already_running(void)
{
	return (o_logo || o_info1 || o_info2 || o_info3 || o_info4);
}

static          gint
view_shrink_logo(gpointer data)
{
	double          x, y, w, h, hh;

	if (!o_logo)
		return FALSE;
	evas_get_geometry(splash_evas, o_logo, &x, &y, &w, &h);
	w -= 8;
	hh = h;
	h = h * (w / (w + 8));
	if ((w > 0) && (h > 0)) {
		evas_move(splash_evas, o_logo, x + 4, y + ((hh - h) / 2));
		evas_resize(splash_evas, o_logo, w, h);
		evas_set_image_fill(splash_evas, o_logo, 0, 0, w, h);
	}
	if (w > 0) {
		QUEUE_DRAW(*splash_idle, splash_evas_redraw);
		gtk_timeout_add(50, view_shrink_logo, NULL);
	} else {
		evas_del_object(splash_evas, o_logo);
		o_logo = NULL;
		QUEUE_DRAW(*splash_idle, splash_evas_redraw);
	}
	return FALSE;
}

static          gint
view_fade_info(gpointer data)
{
	static double   val;
	int             alpha;

	if (new_fade) {
		val = 0.0;
		new_fade = 0;
	}
	if (!o_info1)
		return FALSE;
	alpha = (int) (255 * (1.0 - val));
	evas_set_color(splash_evas, o_info1, 255, 255, 255, alpha);
	evas_set_color(splash_evas, o_info2, 255, 255, 255, alpha);
	evas_set_color(splash_evas, o_info3, 255, 255, 255, alpha);
	evas_set_color(splash_evas, o_info4, 255, 255, 255, alpha);

	if (val < 1.0) {
		val += 0.01;
		QUEUE_DRAW(*splash_idle, splash_evas_redraw);
		gtk_timeout_add(50, view_fade_info, NULL);
	} else {
		evas_del_object(splash_evas, o_info1);
		evas_del_object(splash_evas, o_info2);
		evas_del_object(splash_evas, o_info3);
		evas_del_object(splash_evas, o_info4);
		o_info1 = NULL;
		o_info2 = NULL;
		o_info3 = NULL;
		o_info4 = NULL;
		QUEUE_DRAW(*splash_idle, splash_evas_redraw);
	}
	return FALSE;
}

static          gint
view_scroll_logo(gpointer data)
{
	double          x, y, w, h;
	int             eh;

	if (!o_logo)
		return FALSE;
	evas_get_geometry(splash_evas, o_logo, &x, &y, &w, &h);
	evas_get_drawable_size(splash_evas, NULL, &eh);
	evas_move(splash_evas, o_logo, x, y + ((((eh - h) / 2) - y) / 10) + 1);
	if (y < ((eh - h) / 2)) {
		QUEUE_DRAW(*splash_idle, splash_evas_redraw);
		gtk_timeout_add(50, view_scroll_logo, NULL);
	} else {
		QUEUE_DRAW(*splash_idle, splash_evas_redraw);
		gtk_timeout_add(3000, view_shrink_logo, NULL);
	}
	return FALSE;
}

static          gint
view_scroll_info(gpointer data)
{
	double          x, y, w, h, hh;
	static double   pos, val;
	int             ew, eh;

	if (!o_info1) {
		val = 0;
		evas_font_add_path(splash_evas, PACKAGE_DATA_DIR "/pixmaps");
		o_info1 = evas_add_text(splash_evas, "nationff", 20,
					_("Copyright (C) The Rasterman 2000"));
		o_info2 = evas_add_text(splash_evas, "nationff", 20,
					_("Version 1.0"));
		o_info3 = evas_add_text(splash_evas, "nationff", 20,
					_
					("Enlightenment Graphical Ebit Editor"));
		o_info4 =
			evas_add_text(splash_evas, "nationff", 20,
				      "http://www.enlightenment.org");
		evas_set_color(splash_evas, o_info1, 255, 255, 255, 255);
		evas_set_color(splash_evas, o_info2, 255, 255, 255, 255);
		evas_set_color(splash_evas, o_info3, 255, 255, 255, 255);
		evas_set_color(splash_evas, o_info4, 255, 255, 255, 255);
		evas_set_layer(splash_evas, o_info1, 900);
		evas_set_layer(splash_evas, o_info2, 900);
		evas_set_layer(splash_evas, o_info3, 900);
		evas_set_layer(splash_evas, o_info4, 900);
		evas_show(splash_evas, o_info1);
		evas_show(splash_evas, o_info2);
		evas_show(splash_evas, o_info3);
		evas_show(splash_evas, o_info4);
	}
	pos = cos((1.0 - val) * (3.141592654 / 2));
	evas_get_drawable_size(splash_evas, &ew, &eh);
	evas_get_geometry(splash_evas, o_info1, &x, &y, &w, &h);
	evas_move(splash_evas, o_info1,
		  (pos * (((double) ew - w) / 2)) + ((1.0 - pos) * (-w)),
		  ((eh / 2) + 16));
	hh = h;
	evas_get_geometry(splash_evas, o_info2, &x, &y, &w, &h);
	evas_move(splash_evas, o_info2,
		  (pos * (((double) ew - w) / 2)) + ((1.0 - pos) * (ew)),
		  ((eh / 2) + 16 + hh));
	hh += h;
	evas_get_geometry(splash_evas, o_info3, &x, &y, &w, &h);
	evas_move(splash_evas, o_info3,
		  (ew - w) / 2,
		  (pos * ((eh / 2) + 16 + hh)) + ((1.0 - pos) * (eh)));
	hh += h;
	evas_get_geometry(splash_evas, o_info4, &x, &y, &w, &h);
	evas_move(splash_evas, o_info4,
		  (ew - w) / 2,
		  (pos * ((eh / 2) + 16 + hh)) + ((1.0 - pos) * (-h)));
	hh += h;

	if (val < 1.0) {
		val += 0.02;
		QUEUE_DRAW(*splash_idle, splash_evas_redraw);
		gtk_timeout_add(50, view_scroll_info, NULL);
	} else {
		QUEUE_DRAW(*splash_idle, splash_evas_redraw);
		new_fade = 1;
		gtk_timeout_add(1000, view_fade_info, NULL);
	}
	return FALSE;
}


void
show_splash(Evas evas, gint * idle, GtkFunction redraw_func)
{
	int             w, h, ew;

	if (already_running()) {
		/* There's a context collision here if this is called for two
		 * different Evases (static globals :) If another splash is
		 * running, simply return, doing nothing.
		 */
		return;
	}

	/* set context */
	splash_evas = evas;
	splash_idle = idle;
	splash_evas_redraw = redraw_func;

	o_logo = evas_add_image_from_file(splash_evas,
					  PACKAGE_DATA_DIR
					  "/pixmaps/etcher.png");
	evas_set_layer(splash_evas, o_logo, 900);
	evas_show(splash_evas, o_logo);
	evas_get_image_size(splash_evas, o_logo, &w, &h);
	evas_get_drawable_size(splash_evas, &ew, NULL);
	evas_move(splash_evas, o_logo, (ew - w) / 2, -h);
	gtk_timeout_add(50, view_scroll_logo, NULL);
	gtk_timeout_add(50, view_scroll_info, NULL);
}
