#ifndef __INTERFACE_H
#define __INTERFACE_H

#include "eplayer.h"

int ui_init(ePlayer *player);
int ui_init_edje(ePlayer *player, const char *name);
void ui_deinit();

void ui_fill_track_info(ePlayer *player);
void ui_fill_playlist(ePlayer *player);

int ui_refresh_volume(void *udata);
int ui_refresh_time(ePlayer *player, int time);
int ui_refresh_seeker(ePlayer *player, double song_pos);

#endif

