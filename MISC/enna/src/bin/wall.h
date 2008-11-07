#ifndef _ENNA_WALL_H
#define _ENNA_WALL_H

#include "enna.h"

Evas_Object *enna_wall_add(Evas * evas);
void enna_wall_picture_append(Evas_Object *obj, const char *filename);
void enna_wall_right_select(Evas_Object *obj);
void enna_wall_left_select(Evas_Object *obj);
void enna_wall_up_select(Evas_Object *obj);
void enna_wall_down_select(Evas_Object *obj);
void enna_wall_select_nth(Evas_Object *obj, int col, int row);
#endif
