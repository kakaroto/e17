#ifndef __TRACK_H
#define __TRACK_H

#include "eplayer.h"

int track_play_chunk(void *udata);
int track_update_time(void *udata);

void track_open(ePlayer *player);
void track_close(ePlayer *player);

#endif

