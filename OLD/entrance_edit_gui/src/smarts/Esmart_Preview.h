#ifndef _ENTRANCE_THUMB_SMART_H
#define _ENTRANCE_THUMB_SMART_H

#include <Evas.h>


Evas_Object *esmart_preview_new(Evas *e);
Evas *esmart_preview_evas_get(Evas_Object *o);
void esmart_preview_virtual_size_set(Evas_Object *o, Evas_Coord w, Evas_Coord h);


#endif
