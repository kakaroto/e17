#ifndef _ENTRANCE_CALLBACKS
#define _ENTRANCE_CALLBACKS

#include "entrance.h"

void entrance_select_next_session(Entrance_Session e);
void entrance_select_prev_session(Entrance_Session e);
void entrance_select_session(Entrance_Session e, int index);
void entrance_select_named_session(Entrance_Session e, char *name);
void entrance_start_x(Entrance_Session e);

#endif
