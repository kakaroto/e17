#ifndef __CALLBACKS_H
#define __CALLBACKS_H

#include <Evas.h>

void cb_play(void *udata, Evas_Object *obj,
             const char *emission, const char *src);
void cb_pause(void *udata, Evas_Object *obj,
              const char *emission, const char *src);

void cb_track_next(void *udata, Evas_Object *obj,
                   const char *emission, const char *src);
void cb_track_prev(void *udata, Evas_Object *obj,
                   const char *emission, const char *src);

void cb_time_display_toggle(void *udata, Evas_Object *obj,
                            const char *emission, const char *src);

void cb_repeat_mode_toggle(void *udata, Evas_Object *obj,
                           const char *emission, const char *src);

void cb_volume_raise(void *udata, Evas_Object *obj,
                     const char *emission, const char *src);
void cb_volume_lower(void *udata, Evas_Object *obj,
                     const char *emission, const char *src);

void cb_playlist_scroll_up(void *udata, Evas_Object *obj,
                           const char *emission, const char *src);
void cb_playlist_scroll_down(void *udata, Evas_Object *obj,
                             const char *emission, const char *src);

void cb_seek_forward(void *udata, Evas_Object *obj,
                     const char *emission, const char *src);
void cb_seek_backward(void *udata, Evas_Object *obj,
                      const char *emission, const char *src);

#endif

