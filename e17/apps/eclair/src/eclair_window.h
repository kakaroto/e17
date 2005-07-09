#ifndef _ECLAIR_WINDOW_H_
#define _ECLAIR_WINDOW_H_

#include "eclair_private.h"

Eclair_Window *eclair_window_create(const char *edje_file, const char *window_name, const char *title, Eclair_Engine engine, Eclair *eclair, Evas_Bool main_window);
void eclair_window_del(Eclair_Window *window);
void eclair_window_add_default_callbacks(Eclair_Window *window, Eclair *eclair);
void eclair_window_resize(Eclair_Window *window, Evas_Coord w, Evas_Coord h, Evas_Bool force);
void eclair_window_open(Eclair_Window *window);
void eclair_window_close(Eclair_Window *window);
void eclair_window_minimize_cb(void *data, Evas_Object *edje_object, const char *emission, const char *source);
void eclair_window_open_cb(void *data, Evas_Object *edje_object, const char *emission, const char *source);
void eclair_window_close_cb(void *data, Evas_Object *edje_object, const char *emission, const char *source);

#endif
