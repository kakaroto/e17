/*
 * Eyesight - EFL-based document renderer
 * Copyright (C) 2010-2011 Vincent Torri <vtorri at univ-evry dot fr>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include <Evas.h>
#include <Ecore.h>

#include "eyesight_popup.h"

struct _Popup
{
  Evas_Object *background;
  Evas_Object *text;
  Ecore_Timer *timer;
  double time;
};

static Eina_Bool _popup_timer_cb(void *data)
{
  Popup *p;

  p = (Popup *)data;

  evas_object_show(p->background);
  evas_object_show(p->text);

  if (p->time < 0.5)
    {
      int alpha = (int)(255 * p->time);

      evas_object_color_set (p->background, alpha, alpha, alpha, alpha);
      evas_object_color_set (p->text, 0, 0, 0, alpha);
      p->time += 0.01;
      return ECORE_CALLBACK_RENEW;
    }
  else if (p->time < 1.0)
    {
      int alpha = (int)(255 * (1.0 - p->time));
      evas_object_color_set (p->background, alpha, alpha, alpha, alpha);
      evas_object_color_set (p->text, 0, 0, 0, alpha);
      p->time += 0.01;
      return ECORE_CALLBACK_RENEW;
    }
  else
    {
      evas_object_hide(p->text);
      evas_object_hide(p->background);
      return ECORE_CALLBACK_RENEW;
    }
}

Popup *
popup_new(Evas *evas)
{
  Popup *p;
  Evas_Object *o;

  p = (Popup *)malloc(sizeof(Popup));
  if (!p)
    return NULL;

  o = evas_object_image_add(evas);
  p->background = o;

  o = evas_object_text_add(evas);
  evas_object_text_font_source_set(o, "/usr/share/fonts/truetype/ttf-dejavu");
  evas_object_text_font_set(o, "Serif", 18);
  p->text = o;

  p->timer = ecore_timer_add(0.01, _popup_timer_cb, p);
  p->time = 10.0;

  return p;
}

void
popup_free(Popup *p)
{
  if (!p)
    return;

  ecore_timer_del(p->timer);
  free(p);
}

unsigned char
popup_background_set(Popup *p, const char *filename)
{
  char buf[1024];
  Evas_Coord w;
  Evas_Coord h;
  int err;

  if (!p)
    return 0;

  snprintf(buf, sizeof(buf), "%s/%s", PACKAGE_DATA_DIR, filename);
  evas_object_image_file_set(p->background, buf, NULL);
  err = evas_object_image_load_error_get(p->background);
  if (err != EVAS_LOAD_ERROR_NONE)
    {
      const char *datadir = getenv("EYESIGHT_DATA_DIR");
      if (datadir)
        {
          snprintf(buf, sizeof(buf), "%s/gradient.png", datadir);
          evas_object_image_file_set(p->background, buf, NULL);
          err = evas_object_image_load_error_get(p->background);
          if (err != EVAS_LOAD_ERROR_NONE)
            return 0;
        }
      else
        return 0;
    }

  evas_object_image_size_get (p->background, &w, &h);
  evas_object_image_size_set(p->background, w, h);

  return 1;
}

void
popup_text_set(Popup *p, const char *text)
{
  Evas *evas;
  Evas_Coord ew;
  Evas_Coord eh;
  Evas_Coord w;
  Evas_Coord h;

  if (!p)
    return;

  evas = evas_object_evas_get(p->text);
  evas_output_viewport_get(evas, NULL, NULL, &ew, &eh);

  evas_object_text_text_set(p->text, text);
  evas_object_geometry_get(p->text, NULL, NULL, &w, &h);
  evas_object_move (p->text, (ew - w) / 2, (eh - h) / 2);

  w += 6;
  h += 6;
  evas_object_image_fill_set(p->background, 0, 0, w, h);
  evas_object_move (p->background, (ew - w) / 2, (eh - h) / 2);
  evas_object_resize (p->background, w, h);
}

void
popup_timer_start(Popup *p)
{
  if (!p)
    return;

  p->time = 0.0;
}
