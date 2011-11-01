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

#define ENNA_CONFIG_DD_NEW(str, typ) \
  enna_config_descriptor_new(str, sizeof(typ))
#define ENNA_CONFIG_DD_FREE(eed) if (eed) { eet_data_descriptor_free((eed)); (eed) = NULL; }
#define ENNA_CONFIG_VAL(edd, type, member, dtype) EET_DATA_DESCRIPTOR_ADD_BASIC(edd, type, #member, member, dtype)
#define ENNA_CONFIG_SUB(edd, type, member, eddtype) EET_DATA_DESCRIPTOR_ADD_SUB(edd, type, #member, member, eddtype)
#define ENNA_CONFIG_LIST(edd, type, member, eddtype) EET_DATA_DESCRIPTOR_ADD_LIST(edd, type, #member, member, eddtype)
#define ENNA_CONFIG_HASH(edd, type, member, eddtype) EET_DATA_DESCRIPTOR_ADD_HASH(edd, type, #member, member, eddtype)

#define CHAR   EET_T_CHAR
#define SHORT  EET_T_SHORT
#define INT    EET_T_INT
#define LL     EET_T_LONG_LONG
#define FLOAT  EET_T_FLOAT
#define DOUBLE EET_T_DOUBLE
#define UCHAR  EET_T_UCHAR
#define USHORT EET_T_USHORT
#define UINT   EET_T_UINT
#define ULL    EET_T_ULONG_LONG
#define STR    EET_T_STRING

typedef Eet_Data_Descriptor Enna_Config_DD;


Enna_Config *enna_config;

void enna_config_load_theme (void);
const char *enna_config_theme_get(void);
const char *enna_config_theme_file_get(const char *s);
Enna_Config_DD *enna_config_descriptor_new(const char *name, int size);
/*********************************/


void enna_config_init(void);
void enna_config_shutdown(void);

#endif /* ENNA_CONFIG_H */
