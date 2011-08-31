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

#ifndef GRID_H
#define GRID_H

#include "enna.h"
#include "vfs.h"

Evas_Object *enna_grid_add (Evas_Object *parent);
void enna_grid_file_append(Evas_Object *obj, Enna_File *file,
                           void (*func_activated) (void *data), void *data);
void enna_grid_file_remove(Evas_Object *obj, Enna_File *file);
void enna_grid_file_update(Evas_Object *obj, Enna_File *file);
void enna_grid_select_nth(Evas_Object *obj, int nth);
Eina_List* enna_grid_files_get(Evas_Object* obj);
int enna_grid_selected_get(Evas_Object *obj);
void * enna_grid_selected_data_get(Evas_Object *obj);
int enna_grid_jump_label(Evas_Object *obj, const char *label);
void enna_grid_jump_ascii(Evas_Object *obj, char k);
void enna_grid_clear(Evas_Object *obj);
Eina_List *enna_grid_selected_files_get(Evas_Object *obj);

#endif /* GRID_H */

