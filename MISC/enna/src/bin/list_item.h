#ifndef _ENNA_listitem_H
#define _ENNA_listitem_H

#include "enna.h"

EAPI Evas_Object *enna_listitem_add(Evas * evas);
EAPI void enna_listitem_create_simple(Evas_Object *obj, Evas_Object *icon,
        const char *label);
EAPI void enna_listitem_create_full(Evas_Object *obj, Evas_Object *icon,
        const char *info, const char *title, const char *album,
        const char *artist);
EAPI void enna_listitem_min_size_get(Evas_Object *obj, Evas_Coord *w,
        Evas_Coord *h);
EAPI void enna_listitem_min_size_set(Evas_Object *obj, Evas_Coord w,
        Evas_Coord h);
EAPI void enna_listitem_select(Evas_Object *obj);
EAPI const char *enna_listitem_label_get(Evas_Object *obj);
EAPI void enna_listitem_unselect(Evas_Object *obj);

#endif
