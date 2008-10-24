#ifndef MAIN_WINDOW_H_
#define MAIN_WINDOW_H_

#define WM_CLASS "Eyesight"
#define WM_NAME "eyesight"
#define WM_TITLE "Eyesight Document Viewer v0.1"

#include <Evas.h>
#include "conf.h"

typedef struct _Main_Window
{
   //Ecore_Evas *ee;
   Evas *evas;
   Evas_Object *main_window;
   Evas_Object *controls;
}
Main_Window;

Main_Window *main_window_create(Args *args, Eina_List **startup_errors);
void main_window_resize_cb(Ecore_Evas *ee);
void main_window_load_cb(void *data, Evas_Object *o, const char *emission,
                         const char *source);
void controls_resize_cb(void *data, Evas *e, Evas_Object *obj, void *event_info);
void exit_clicked_cb(void *data, Evas_Object *o, const char *emission,
                     const char *source);

#endif /*MAIN_WINDOW_H_*/
