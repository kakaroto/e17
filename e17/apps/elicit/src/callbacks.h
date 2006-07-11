#ifndef ELICIT_CALLBACKS_H
#define ELICIT_CALLBACKS_H

#include "math.h"

void elicit_cb_exit(Ecore_Evas *ee);

void elicit_cb_enter(Ecore_Evas *ee);
void elicit_cb_leave(Ecore_Evas *ee);

void elicit_cb_resize(Ecore_Evas *ee);
void elicit_cb_pick(void *data, Evas_Object *o, const char *sig, const char *src);
void elicit_cb_shoot(void *data, Evas_Object *o, const char *sig, const char *src);
void elicit_cb_move(void *data, Evas_Object *o, const char *sig, const char *src);
void elicit_cb_colors(void *data, Evas_Object *o, const char *sig, const char *src);
void elicit_cb_switch(void *data, Evas_Object *o, const char *sig, const char *src);
void elicit_cb_copy(void *data, Evas_Object *o, const char *sig, const char *src);
void elicit_cb_resize_sig(void *data, Evas_Object *o, const char *sig, const char *src);
void elicit_cb_slider(void *data, Evas_Object *o, const char *sig, const char *src);
void elicit_cb_freeze(void *data, Evas_Object *o, const char *sig, const char *src);
void elicit_cb_thaw(void *data, Evas_Object *o, const char *sig, const char *src);
void elicit_cb_size_min(void *data, Evas_Object *o, const char *sig, const char *src);
void elicit_cb_colorclass(void *data, Evas_Object *o, const char *sig, const char *src);
void elicit_cb_edit(void *data, Evas_Object *o, const char *sig, const char *src);
void elicit_cb_grid(void *data, Evas_Object *o, const char *sig, const char *src);

#endif
