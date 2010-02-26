#ifndef CSLIDER_H
#define CSLIDER_H

#include "elicit.h"

typedef struct _Elicit_Cslider Elicit_Cslider;

Evas_Object *elicit_cslider_add(Evas *evas);
int elicit_cslider_theme_set(Evas_Object *o, const char *file, const char *group);
void elicit_cslider_color_set(Evas_Object *o, Color *color, Color_Type type);

#endif
