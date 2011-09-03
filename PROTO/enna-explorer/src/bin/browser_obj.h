
/*
 * GeeXboX Enna Media Center.
 * Copyright (C) 2005-2010 The Enna Project
 *
 * This file is part of Enna.
 *
 * Enna is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * Enna is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with Enna; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */

#ifndef BROWSER_OBJ_H
#define BROWSER_OBJ_H

#include <Elementary.h>

typedef enum _Enna_Browser_View_Type
  {
    ENNA_BROWSER_VIEW_LIST,
    ENNA_BROWSER_VIEW_GRID,
  }Enna_Browser_View_Type;

Evas_Object *enna_browser_obj_add(Evas_Object *parent, const char *style);
void enna_browser_obj_root_set(Evas_Object *obj, const char *uri);
void enna_browser_obj_uri_set(Evas_Object *obj, const char *uri);
void enna_browser_obj_view_type_set(Evas_Object *obj, Enna_Browser_View_Type view_type);
Eina_List *enna_browser_obj_files_get(Evas_Object *obj);
Eina_List *enna_browser_obj_selected_files_get(Evas_Object *obj);

#endif /* BROWSER_OBJ_H */
