#ifndef __ENNA_LOCATION_H__
#define __ENNA_LOCATION_H__

#include "enna.h"

Evas_Object *enna_location_add(Evas * evas);
void enna_location_append(Evas_Object *obj, const char *label,
        Evas_Object *icon, void (*func) (void *data, void *data2), void *data, void *data2);
void enna_location_remove_nth(Evas_Object *obj, int n);
int enna_location_count(Evas_Object *obj);
const char *enna_location_label_get_nth(Evas_Object *obj, int n);
#endif
