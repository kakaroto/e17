#ifndef __ENVISION_H
#define __ENVISION_H

#include <Ecore_Evas.h>
#include <Ecore.h>
#include <Emotion.h>
#include <Edje.h>

typedef struct {
   int width;
   int height;
} Config;

typedef struct {
   Ecore_Evas  *  ee;
   Evas        *  evas;
   Evas_Object *  edje;
   Evas_Object *  emotion;
} Gui;

typedef struct {
   Evas_List   *  playlist;
   Gui            gui;
   Config         config;
} Envision;

// envision.c
Envision * envision_new();
void envision_delete(Envision *e);

// callbacks.c
void canvas_resize(Ecore_Evas *ee);
void ecore_resize(Ecore_Evas *ee);
void update_timer(void *data, Evas_Object *obj, void *event_info);
void quit_edjecallback(void *data, Evas_Object *obj, const char *emission, 
      const char *source);
void raisevol_edjecallback(void *data, Evas_Object *obj, const char *emission,
      const char *source);
void lowervol_edjecallback(void *data, Evas_Object *obj, const char *emission,
      const char *source);
void keydown_evascallback(void *data, Evas *e, Evas_Object *obj,
      void *event_info);
void seekforward_edjecallback(void *data, Evas_Object *obj,
      const char *emission, const char *source);
void seekbackward_edjecallback(void *data, Evas_Object *obj,
      const char *emission, const char *source);
void pause_callback(void *data, Evas_Object *obj, const char *emission,
      const char *source);
void play_callback(void *data, Evas_Object *obj, const char *emission,
      const char *source);

#endif /* __ENVISION_H */
