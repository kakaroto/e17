/******************************************************************************
**  Copyright (c) 2006-2010, Calaos. All Rights Reserved.
**
**  This is free software; you can redistribute it and/or modify
**  it under the terms of the GNU General Public License as published by
**  the Free Software Foundation; either version 3 of the License, or
**  (at your option) any later version.
**
**  This is distributed in the hope that it will be useful,
**  but WITHOUT ANY WARRANTY; without even the implied warranty of
**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
**  GNU General Public License for more details.
**
**  You should have received a copy of the GNU General Public License
**  along with this; if not, write to the Free Software
**  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
**
******************************************************************************/

#ifndef GAME_H
#define GAME_H

#include "common.h"
#include "draw_object.h"
#include "physic_object.h"
#include "level.h"

typedef void (*Game_Win_Cb)(Level *level);

void game_play_level_list(Evas *e, Eina_List *list, Eina_List *start_at, void (*play_done)(void *data), void *data);

void game_play_level(Evas *evas, Level *level, Game_Win_Cb win_cb, Eina_Bool want_menu_button);
void game_stop_level(void);

void game_restart_level(void);


#endif

