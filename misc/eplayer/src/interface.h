#ifndef __INTERFACE_H
#define __INTERFACE_H

#include "eplayer.h"

void show_playlist(ePlayer *player);
int setup_gui(ePlayer *player);

int refresh_volume(void *udata);
int refresh_time(ePlayer *player, int time);

#endif

