#ifndef __INTERFACE_H
#define __INTERFACE_H

/*
 * $Id$
 */

#include "euphoria.h"

bool ui_init(Euphoria *e);
bool ui_init_edje(Euphoria *e, const char *name);

void ui_shutdown_edje(Euphoria *e);
void ui_shutdown(Euphoria *e);

void ui_fill_track_info(Euphoria *e, PlayListItem *pli);
void ui_fill_playlist(Euphoria *e);

int ui_refresh_volume(void *udata);
bool ui_refresh_time(Euphoria *e, int time);
bool ui_refresh_seeker(Euphoria *e, double song_pos);

#endif

