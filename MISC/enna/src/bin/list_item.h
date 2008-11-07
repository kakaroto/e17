#ifndef _ENNA_listitem_H
#define _ENNA_listitem_H

#include "enna.h"

Evas_Object *enna_listitem_add(Evas * evas);
void enna_listitem_create_simple(Evas_Object *obj, Evas_Object *icon,
        const char *label);
void enna_listitem_create_full(Evas_Object *obj, Evas_Object *icon,
        const char *info, const char *title, const char *album,
        const char *artist);
void enna_listitem_min_size_get(Evas_Object *obj, Evas_Coord *w,
        Evas_Coord *h);
void enna_listitem_min_size_set(Evas_Object *obj, Evas_Coord w,
        Evas_Coord h);
void enna_listitem_select(Evas_Object *obj);
const char *enna_listitem_label_get(Evas_Object *obj);
void enna_listitem_unselect(Evas_Object *obj);

#endif
