#ifndef __VORBIS_H
#define __VORBIS_H

#include "eplayer.h"

int vorbis_play_chunk(void *udata);
int vorbis_update_time(void *udata);

void vorbis_open(ePlayer *player);
void vorbis_close(ePlayer *player);

void cb_seek_forward(void *udata, Evas_Object *obj,
                     const char *emission, const char *src);
void cb_seek_backward(void *udata, Evas_Object *obj,
                      const char *emission, const char *src);
#endif

