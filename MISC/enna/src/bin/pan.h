#ifndef _ENNA_PAN_H_
#define _ENNA_PAN_H_

#include "enna.h"

Evas_Object *enna_pan_add            (Evas *evas);
void         enna_pan_child_set      (Evas_Object *obj, Evas_Object *child);
Evas_Object *enna_pan_child_get      (Evas_Object *obj);
void         enna_pan_set            (Evas_Object *obj, Evas_Coord x, Evas_Coord y);
void         enna_pan_get            (Evas_Object *obj, Evas_Coord *x, Evas_Coord *y);
void         enna_pan_max_get        (Evas_Object *obj, Evas_Coord *x, Evas_Coord *y);
void         enna_pan_child_size_get (Evas_Object *obj, Evas_Coord *w, Evas_Coord *h);

#endif
