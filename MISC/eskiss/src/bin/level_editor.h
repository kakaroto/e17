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

#ifndef LEVEL_EDITOR_H
#define LEVEL_EDITOR_H

#include "common.h"
#include "draw_object.h"
#include "level.h"
#include "game.h"

typedef struct _EditorObject
{
        Evas_Object *obj;
        LevelObject *level_obj;
} EditorObject;


void level_editor_edit(Evas *evas, Level *level, void (*edit_done)(Level *level, void *data), void *data);
void level_editor_close(void);

#endif

