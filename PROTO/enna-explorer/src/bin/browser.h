
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

#ifndef BROWSER_H
#define BROWSER_H

#include "file.h"

typedef struct _Enna_Browser Enna_Browser;

Enna_Browser *enna_browser_add(void (*add)(void *data, Enna_File *file), void *add_data,
                               void (*del)(void *data, Enna_File *file), void *del_data,
                               void (*update)(void *data, Enna_File *file), void *update_data,
                               const char *uri);
void enna_browser_browse(Enna_Browser *b);
void enna_browser_del(Enna_Browser *b);
void enna_browser_file_add(Enna_Browser *b, Enna_File *file);
void enna_browser_file_del(Enna_Browser *b, Enna_File *file);
Enna_File *enna_browser_file_update(Enna_Browser *b, Enna_File *file);
Enna_File *enna_browser_get_file(const char *uri);
const char *enna_browser_uri_get(Enna_Browser *b);
Eina_List *enna_browser_files_get(Enna_Browser *b);
Eina_List *enna_browser_selected_files_get(Enna_Browser *b);
void enna_browser_filter(Enna_Browser *b, const char *filter);
#endif /* BROWSER_H */
