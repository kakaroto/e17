#ifndef _ECLAIR_WINDOW_H_
#define _ECLAIR_WINDOW_H_

#include <Evas.h>
#include <Ecore.h>
#include <Ecore_Evas.h>
#include <Ecore_X.h>
#include "eclair_types.h"

struct _Eclair_Window
{
   Ecore_X_Window x_window;
   Ecore_Evas *ecore_window;
   Evas *evas;
   Evas_Object *edje_object;
   Evas_Object *draggies;
   Evas_Coord min_width;
   Evas_Coord min_height;
   Evas_Coord max_width;
   Evas_Coord max_height;
   char *window_name;
   int x, y;
   Eclair *eclair;

   unsigned char should_resize : 1;
   unsigned char borderless : 1;
   unsigned char shaped : 1;
   unsigned char resizable : 1;
   unsigned char main_window : 1;
};

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
