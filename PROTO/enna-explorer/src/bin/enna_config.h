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

#ifndef ENNA_CONFIG_H
#define ENNA_CONFIG_H

#include <Elementary.h>

typedef struct _Enna_Config Enna_Config;

struct _Enna_Config
{
   Elm_Theme *eth;
   char *theme;
   char *theme_file;
};

Enna_Config *enna_config;

void enna_config_load_theme (void);
const char *enna_config_theme_get(void);
const char *enna_config_theme_file_get(const char *s);

/*********************************/


void enna_config_init(void);
void enna_config_shutdown(void);

#endif /* ENNA_CONFIG_H */
