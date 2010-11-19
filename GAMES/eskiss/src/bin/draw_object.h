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

#ifndef DRAW_OBJECT_H
#define DRAW_OBJECT_H

#include "common.h"

//default colors for brush
#define BRUSH_RED { 227, 65, 65, 255 }
#define BRUSH_BLUE { 22, 156, 233, 255 }
#define BRUSH_GREEN { 55, 201, 44, 255 }
#define BRUSH_YELLOW { 223, 221, 34, 255 }
#define BRUSH_PINK { 223, 34, 221, 255 }
#define BRUSH_CYAN { 27, 200, 182, 255 }
#define BRUSH_ORANGE { 231, 167, 28, 255 }
#define BRUSH_VIOLET { 151, 29, 214, 255 }

#define BRUSH_COUNT 8

typedef struct _BrushColor
{
        int r, g, b, a;
} BrushColor;

extern BrushColor default_colors[];


void            draw_object_add(Evas *evas);
void            draw_object_del(void);

void            draw_object_enabled_set(Eina_Bool en);
Eina_Bool       draw_object_enabled_get(void);

void            draw_object_color_set(int r, int g, int b, int a);
void            draw_object_brush_set(BrushColor brush);
void            draw_object_resize(int w, int h);

void            draw_object_new_callback_add(void (*func) (Evas_Object *new_obj, Eina_List *point_list, void *data), void *data);
void            draw_object_new_callback_del(void (*func) (Evas_Object *new_obj, Eina_List *point_list, void *data), void *data);

Evas_Object    *draw_object_create(Evas *e, Eina_List *point_list, BrushColor color);


#endif
