#ifndef _ENTRANCE_CALLBACKS
#define _ENTRANCE_CALLBACKS

#include "entrance.h"

int entrance_return_key_cb(Entrance_Session e, char *buffer);

void entrance_select_next_session(Entrance_Session e);
void entrance_select_prev_session(Entrance_Session e);
void entrance_select_session(Entrance_Session e, int index);
void entrance_select_named_session(Entrance_Session e, char *name);

#endif
