#ifndef __INTERFACE_H
#define __INTERFACE_H

#include "eplayer.h"

void show_playlist(ePlayer *player);
void setup_ecore(ePlayer *player);

void refresh_volume(ePlayer *player, int read);
int refresh_time(ePlayer *player, int time);

#endif

