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

#ifndef LEVEL_H
#define LEVEL_H

#include "common.h"
#include "physic_object.h"
#include "draw_object.h"

typedef struct _LevelObject
{
        Eina_List *point_list;
        BrushColor color;

        Eina_Bool has_physic; /* true is object respond to physics */
        Eina_Bool static_object; /* true if static physic object */

} LevelObject;

typedef struct _Level
{
        Eina_List *objects;

        DrawPoint ball; //ball position
        DrawPoint goal; //goal position

        char *eet_file;

} Level;

void            level_init(void);
void            level_shutdown(void);

Level          *level_add(void);
void            level_del(Level *level);

Eina_Bool       level_load_file(Level **level, const char *file);
Eina_Bool       level_save_file(Level *level, const char *file);

Eina_List      *level_search_level_files(void);
Evas_Object    *level_get_thumb(Evas *evas, Level *level);

void            level_object_add(Level *level, LevelObject *obj);
void            level_object_del(Level *level, LevelObject *obj);

#endif
