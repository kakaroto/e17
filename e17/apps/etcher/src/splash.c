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

static gint         already_running(void);
static gint         view_shrink_logo(gpointer data);
static gint         view_fade_info(gpointer data);
static gint         view_scroll_logo(gpointer data);
static gint         view_scroll_info(gpointer data);

int                 new_fade = 0;
Evas *                splash_evas;
Evas_Object        *o_logo = NULL, *o_info1 = NULL, *o_info2, *o_info3, *o_info4;
gint               *splash_idle;
GtkFunction         splash_evas_redraw;

static              gint
already_running(void)
{
   return (o_logo || o_info1 || o_info2 || o_info3 || o_info4);
}

static              gint
view_shrink_logo(gpointer data)
{
   double              x, y, w, h, hh;

   if (!o_logo)
      return FALSE;
   evas_object_geometry_get(o_logo, &x, &y, &w, &h);
   w -= 8;
   hh = h;
   h = h * (w / (w + 8));
   if ((w > 0) && (h > 0))
     {
	evas_object_move(o_logo, x + 4, y + ((hh - h) / 2));
	evas_object_resize(o_logo, w, h);
	evas_object_image_fill_set(o_logo, 0, 0, w, h);
     }
   if (w > 0)
     {
	QUEUE_DRAW(*splash_idle, splash_evas_redraw);
	gtk_timeout_add(50, view_shrink_logo, NULL);
     }
   else
     {
	evas_object_del(o_logo);
	o_logo = NULL;
	QUEUE_DRAW(*splash_idle, splash_evas_redraw);
     }
   return FALSE;
}

static              gint
view_fade_info(gpointer data)
{
   static double       val;
   int                 alpha;

   if (new_fade)
     {
	val = 0.0;
	new_fade = 0;
     }
   if (!o_info1)
      return FALSE;
   alpha = (int)(255 * (1.0 - val));
   evas_object_color_set(o_info1, 255, 255, 255, alpha);
   evas_object_color_set(o_info2, 255, 255, 255, alpha);
   evas_object_color_set(o_info3, 255, 255, 255, alpha);
   evas_object_color_set(o_info4, 255, 255, 255, alpha);

   if (val < 1.0)
     {
	val += 0.01;
	QUEUE_DRAW(*splash_idle, splash_evas_redraw);
	gtk_timeout_add(50, view_fade_info, NULL);
     }
   else
     {
	evas_object_del(o_info1);
	evas_object_del(o_info2);
	evas_object_del(o_info3);
	evas_object_del(o_info4);
	o_info1 = NULL;
	o_info2 = NULL;
	o_info3 = NULL;
	o_info4 = NULL;
	QUEUE_DRAW(*splash_idle, splash_evas_redraw);
     }
   return FALSE;
}

static              gint
view_scroll_logo(gpointer data)
{
   double              x, y, w, h;
   int                 eh;

   if (!o_logo)
      return FALSE;
   evas_object_geometry_get(o_logo, &x, &y, &w, &h);
   evas_output_size_get(splash_evas, NULL, &eh);
   evas_object_move(o_logo, x, y + ((((eh - h) / 2) - y) / 10) + 1);
   if (y < ((eh - h) / 2))
     {
	QUEUE_DRAW(*splash_idle, splash_evas_redraw);
	gtk_timeout_add(50, view_scroll_logo, NULL);
     }
   else
     {
	QUEUE_DRAW(*splash_idle, splash_evas_redraw);
	gtk_timeout_add(3000, view_shrink_logo, NULL);
     }
   return FALSE;
}

static Evas_Object*
e_evas_add_text(Evas *evas, char *font, double size, char *text)
{
  Evas_Object *et;

  et = evas_object_text_add(evas);
  evas_object_text_font_set(et, font, size);
  evas_object_text_text_set(et, text);
  return et;
}

static              gint
view_scroll_info(gpointer data)
{
   double              x, y, w, h, hh;
   static double       pos, val;
   int                 ew, eh;

   if (!o_info1)
     {
	val = 0;
	evas_font_path_append(splash_evas, PACKAGE_DATA_DIR "/pixmaps");
	o_info1 = e_evas_add_text(splash_evas, "nationff", 20,
				_("Copyright (C) The Rasterman 2000"));
	o_info2 = e_evas_add_text(splash_evas, "nationff", 20, _("Version 1.0-Evas2"));
	o_info3 = e_evas_add_text(splash_evas, "nationff", 20,
				_("Enlightenment Graphical Ebit Editor"));
	o_info4 =
	   e_evas_add_text(splash_evas, "nationff", 20,
			 "http://www.enlightenment.org");
	evas_object_color_set(o_info1, 255, 255, 255, 255);
	evas_object_color_set(o_info2, 255, 255, 255, 255);
	evas_object_color_set(o_info3, 255, 255, 255, 255);
	evas_object_color_set(o_info4, 255, 255, 255, 255);
	evas_object_layer_set(o_info1, 900);
	evas_object_layer_set(o_info2, 900);
	evas_object_layer_set(o_info3, 900);
	evas_object_layer_set(o_info4, 900);
	evas_object_show(o_info1);
	evas_object_show(o_info2);
	evas_object_show(o_info3);
	evas_object_show(o_info4);
     }
   pos = cos((1.0 - val) * (3.141592654 / 2));
   evas_output_size_get(splash_evas, &ew, &eh);
   evas_object_geometry_get(o_info1, &x, &y, &w, &h);
   evas_object_move(o_info1,
	     (pos * (((double)ew - w) / 2)) + ((1.0 - pos) * (-w)),
	     ((eh / 2) + 16));
   hh = h;
   evas_object_geometry_get(o_info2, &x, &y, &w, &h);
   evas_object_move(o_info2,
	     (pos * (((double)ew - w) / 2)) + ((1.0 - pos) * (ew)),
	     ((eh / 2) + 16 + hh));
   hh += h;
   evas_object_geometry_get(o_info3, &x, &y, &w, &h);
   evas_object_move(o_info3,
	     (ew - w) / 2, (pos * ((eh / 2) + 16 + hh)) + ((1.0 - pos) * (eh)));
   hh += h;
   evas_object_geometry_get(o_info4, &x, &y, &w, &h);
   evas_object_move(o_info4,
	     (ew - w) / 2, (pos * ((eh / 2) + 16 + hh)) + ((1.0 - pos) * (-h)));
   hh += h;

   if (val < 1.0)
     {
	val += 0.02;
	QUEUE_DRAW(*splash_idle, splash_evas_redraw);
	gtk_timeout_add(50, view_scroll_info, NULL);
     }
   else
     {
	QUEUE_DRAW(*splash_idle, splash_evas_redraw);
	new_fade = 1;
	gtk_timeout_add(1000, view_fade_info, NULL);
     }
   return FALSE;
}

void
show_splash(Evas * evas, gint * idle, GtkFunction redraw_func)
{
   int                 w, h, ew;

   if (already_running())
     {
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

   o_logo = evas_object_image_add(splash_evas);
   e_evas_object_image_file_set(o_logo,
				PACKAGE_DATA_DIR "/pixmaps/etcher.png", NULL);
   evas_object_layer_set(o_logo, 900);
   evas_object_show(o_logo);
   evas_object_image_size_get(o_logo, &w, &h);
   evas_output_size_get(splash_evas, &ew, NULL);
   evas_object_move(o_logo, (ew - w) / 2, -h);
   gtk_timeout_add(50, view_scroll_logo, NULL);
   gtk_timeout_add(50, view_scroll_info, NULL);
}

void
e_evas_object_image_file_set(Evas_Object *_o, char *file, char *key)
{
  int iw,ih;

  evas_object_image_file_set(_o, file, key);
  evas_object_image_size_get(_o, &iw, &ih);
  evas_object_image_fill_set(_o, 0, 0, iw, ih);
  evas_object_resize(_o, iw, ih);
}

/*eof*/
