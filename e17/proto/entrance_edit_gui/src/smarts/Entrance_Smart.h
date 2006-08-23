#ifndef _ENTRANCE_SMART_H
#define _ENTRANCE_SMART_H

#include <Evas.h>


Evas_Object *es_new(Evas *e);
void es_background_edje_set(Evas_Object *o, const char *file);
void es_main_edje_set(Evas_Object *o, const char *file);
void es_session_edje_set(Evas_Object *o, const char *file);

#endif
