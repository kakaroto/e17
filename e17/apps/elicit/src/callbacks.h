#ifndef ELICIT_CALLBACKS_H
#define ELICIT_CALLBACKS_H

#include "math.h"

void elicit_cb_exit(Ecore_Evas *ee);
void elicit_cb_resize(Ecore_Evas *ee);
void elicit_cb_pick(void *data, Evas_Object *o, const char *sig, const char *src);
void elicit_cb_shoot(void *data, Evas_Object *o, const char *sig, const char *src);
void elicit_cb_colors(void *data, Evas_Object *o, const char *sig, const char *src);
void elicit_cb_copy(void *data, Evas_Object *o, const char *sig, const char *src);

#endif
