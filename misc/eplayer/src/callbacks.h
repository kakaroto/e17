#ifndef __CALLBACKS_H
#define __CALLBACKS_H

#include <Evas.h>

void unpause_playback(void *udata, Evas_Object *obj,
                      const char *emission, const char *src);
void pause_playback(void *udata, Evas_Object *obj,
                    const char *emission, const char *src);

void next_file(void *udata, Evas_Object *obj,
               const char *emission, const char *src);
void prev_file(void *udata, Evas_Object *obj,
               const char *emission, const char *src);

void switch_time_display(void *udata, Evas_Object *obj,
                         const char *emission, const char *src);

void raise_vol(void *udata, Evas_Object *obj, const char *emission,
               const char *src);
void lower_vol(void *udata, Evas_Object *obj, const char *emission,
               const char *src);

#endif

