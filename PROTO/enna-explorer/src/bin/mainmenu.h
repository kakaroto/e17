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

#ifndef MAINMENU_H
#define MAINMENU_H

#include "activity.h"
#include "vfs.h"

Evas_Object *enna_mainmenu_add(Evas_Object *parent);
void enna_mainmenu_shutdown(Evas_Object *obj);
void enna_mainmenu_append(Evas_Object *obj, Enna_File *f);
void enna_mainmenu_show(Evas_Object *obj);
void enna_mainmenu_hide(Evas_Object *obj);
Eina_Bool enna_mainmenu_visible(Evas_Object *obj);
Eina_Bool enna_mainmenu_exit_visible(Evas_Object *obj);
Enna_File *enna_mainmenu_selected_activity_get(Evas_Object *obj);
void enna_mainmenu_background_add(Evas_Object *obj, Enna_File *file);
void enna_mainmenu_background_select(Evas_Object *obj, Enna_File *file);

#endif /* MAINMENU_H */
