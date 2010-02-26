#ifndef BAND_H
#define BAND_H

#include <Ecore_Evas.h>
#include <Evas.h>

struct Elicit_Band
{
  Ecore_Evas *ee;
  Evas_Object *obj;
};

typedef struct Elicit_Band Elicit_Band;

Elicit_Band *elicit_band_new(const char *theme_file);
void elicit_band_free(Elicit_Band *band);
void elicit_band_show(Elicit_Band *band);
void elicit_band_hide(Elicit_Band *band);
void elicit_band_move_resize(Elicit_Band *band, int x, int y, int w, int h);


#endif
