#ifndef __INTERFACE_H
#define __INTERFACE_H

#include "eplayer.h"

bool ui_init(ePlayer *player);
bool ui_init_edje(ePlayer *player, const char *name);
void ui_deinit();

void ui_fill_track_info(ePlayer *player);
void ui_fill_playlist(ePlayer *player);

int ui_refresh_volume(void *udata);
bool ui_refresh_time(ePlayer *player, int time);
bool ui_refresh_seeker(ePlayer *player, double song_pos);

#endif

