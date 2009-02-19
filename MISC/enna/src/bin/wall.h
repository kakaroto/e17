#ifndef _ENNA_WALL_H
#define _ENNA_WALL_H

#include "enna.h"

Evas_Object *enna_wall_add(Evas * evas);
void enna_wall_picture_append(Evas_Object *obj, const char *filename);
void enna_wall_select_nth(Evas_Object *obj, int col, int row);
void enna_wall_event_feed(Evas_Object *obj, void *event_info);
void enna_wall_selected_geometry_get(Evas_Object *obj, int *x, int *y, int *w, int *h);
const char *enna_wall_selected_filename_get(Evas_Object *obj);

#endif
