#ifndef __ENNA_LOCATION_H__
#define __ENNA_LOCATION_H__

#include "enna.h"

EAPI Evas_Object   *enna_location_add(Evas * evas);
EAPI void           enna_location_append (Evas_Object *obj, const char *label, Evas_Object *icon, void (*func) (void *data, void *data2), void *data, void *data2);
EAPI void           enna_location_remove_nth(Evas_Object *obj, int n);
EAPI int            enna_location_count(Evas_Object *obj);
EAPI const char    *enna_location_label_get_nth(Evas_Object *obj, int n);
#endif
