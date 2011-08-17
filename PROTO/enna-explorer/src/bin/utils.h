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

#ifndef UTILS_H
#define UTILS_H

#include <Evas.h>

#define MMAX(a,b) ((a) > (b) ? (a) : (b))
#define MMIN(a,b) ((a) > (b) ? (b) : (a))

int enna_util_init(void);
int enna_util_shutdown(void);
char         *enna_util_user_home_get(void);
const char *enna_util_data_home_get(void);
const char *enna_util_config_home_get(void);
const char *enna_util_cache_home_get(void);
int           enna_util_has_suffix(char *str, Eina_List * patterns);
unsigned int  enna_util_calculate_font_size(Evas_Coord w, Evas_Coord h);
unsigned char enna_util_uri_has_extension(const char *uri, int type);
char *md5sum (char *str);
char *init_locale(void);
char *get_locale(void);
char *get_lang(void);

void enna_util_env_set(const char *var, const char *val);
char *enna_util_str_chomp(char *str);
double enna_util_atof(const char *nptr);

Eina_List *enna_util_tuple_get (const char *str, const char *delimiter);
char *enna_util_tuple_set (Eina_List *tuple, const char *delimiter);

Eina_Bool enna_util_str_has_prefix(const char *str, const char *prefix);

Eina_Bool enna_util_str_has_suffix(const char *str, const char *suffix);
Eina_Bool enna_util_str_has_extension(const char *str, const char *ext);

const char *enna_util_duration_to_string(const char *length);

Eina_List *enna_util_stringlist_get(const char *str);
void enna_util_stringlist_free(Eina_List *list);

#endif /* UTILS_H */
