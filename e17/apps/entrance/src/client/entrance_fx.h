#ifndef _ENTRANCE_FX_H
#define _ENTRANCE_FX_H

#include <Evas.h>
#include <Ecore.h>
#include <stdio.h>
#include <limits.h>
#include <string.h>
#include "entrance_theme.h"

struct _Entrance_FX_Fade_Data
{
   Evas_Object *obj;
   E_Color *orig_color;
   int start_a;                 /* Starting alpha value */
   int end_a;                   /* Ending alpha value */
   double in;                   /* Time Interval */
   double step;                 /* Increment value (fraction of total alpha
                                   difference) */
};

typedef struct _Entrance_FX_Fade_Data Entrance_FX_Fade_Data;

void fx_fade(Evas_Object * o, double t, int start_a, int end_a);
void fx_fade_in(Evas_Object * o, double t);
void fx_fade_out(Evas_Object * o, double t);

#endif
