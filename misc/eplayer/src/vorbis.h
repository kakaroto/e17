#ifndef __VORBIS_H
#define __VORBIS_H

#include "eplayer.h"

int play_loop(void *udata);
int update_time(ePlayer *eplayer);
void open_track(ePlayer *player);

void seek_forward(void *udata, Evas_Object *obj,
                  const char *emission, const char *src);
void seek_backward(void *udata, Evas_Object *obj,
                   const char *emission, const char *src);
#endif

