#include <stdlib.h>
#include <Edje.h>
#include <Ecore_X.h>
#include <Ecore_Evas.h>
#include "band.h"

Elicit_Band *
elicit_band_new(const char *theme_file)
{
  Elicit_Band *band;

  band = calloc(1, sizeof(Elicit_Band));

  //XXX allow other engines
  band->ee = ecore_evas_software_x11_new(0,0,0,0,10,10);
  ecore_evas_borderless_set(band->ee, 1);
  if (ecore_x_screen_is_composited(0))
    ecore_evas_alpha_set(band->ee, 1);
  else
    ecore_evas_shaped_set(band->ee, 1);

  band->obj = edje_object_add(ecore_evas_get(band->ee));

  //XXX theme file!
  if (!edje_object_file_set(band->obj, theme_file, "elicit.band"))
  {
    fprintf(stderr, "[Elicit] Error: Can't set band theme\n");
  }
  evas_object_move(band->obj, 0, 0);
  evas_object_show(band->obj);

  return band;
}

void
elicit_band_free(Elicit_Band *band)
{
  if (band->obj)
    evas_object_del(band->obj);

  if (band->ee)
    ecore_evas_free(band->ee);

  free(band);
}

void
elicit_band_show(Elicit_Band *band)
{
  ecore_evas_show(band->ee);
}

void
elicit_band_hide(Elicit_Band *band)
{
  ecore_evas_hide(band->ee);
}

void
elicit_band_move_resize(Elicit_Band *band, int x, int y, int w, int h)
{
  ecore_evas_move_resize(band->ee, x, y, w, h);
  evas_object_resize(band->obj, w, h);
}

