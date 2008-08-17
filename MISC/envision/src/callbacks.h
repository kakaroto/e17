/*
 * $Id$
 * vim:expandtab:ts=3:sts=3:sw=3
 */

#ifndef __CALLBACKS_H
#define __CALLBACKS_H

#include <Ecore_Evas.h>
#include <Ecore.h>
#include <Emotion.h>
#include <Edje.h>

#define EDJE_CALLBACK(name) \
   void callback_edje_##name(void *data, Evas_Object *obj, \
         const char *emission, const char *source)

// callbacks.c
void canvas_resize(Ecore_Evas * ee);
void update_timer(void *data, Evas_Object * obj, void *event_info);
void callback_evas_keydown(void *data, Evas * e, Evas_Object * obj,
      void *event_info);

// Edje Callbacks
EDJE_CALLBACK(quit);
EDJE_CALLBACK(play);
EDJE_CALLBACK(pause);
EDJE_CALLBACK(volume_raise);
EDJE_CALLBACK(volume_lower);
EDJE_CALLBACK(seek_forward);
EDJE_CALLBACK(seek_backward);

#endif /* __CALLBACKS_H */
