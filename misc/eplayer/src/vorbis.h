#ifndef __VORBIS_H
#define __VORBIS_H

#include "eplayer.h"

int track_play_chunk(void *udata);
int update_time(void *udata);

void track_open(ePlayer *player);
void track_close(ePlayer *player);

void cb_seek_forward(void *udata, Evas_Object *obj,
                     const char *emission, const char *src);
void cb_seek_backward(void *udata, Evas_Object *obj,
                      const char *emission, const char *src);
#endif

