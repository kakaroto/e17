#ifndef _ENNA_CAROUSEL_H
#define _ENNA_CAROUSEL_H

#include "enna.h"

Evas_Object *enna_carousel_add(Evas * evas);
void enna_carousel_object_append(Evas_Object *obj, Evas_Object *child, const char *label);
void enna_carousel_select_nth(Evas_Object *obj, int col);
void enna_carousel_activate_nth(Evas_Object *obj, int n);
void enna_carousel_event_feed(Evas_Object *obj, void *event_info);
void enna_carousel_selected_geometry_get(Evas_Object *obj, int *x, int *y, int *w, int *h);
Evas_Object *enna_carousel_selected_object_get(Evas_Object *obj);
int enna_carousel_selected_get(Evas_Object *obj);

#endif
