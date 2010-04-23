/**
 * @file
 *
 * Copyright (C) 2010 by ProFUSION embedded systems
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or (at your
 * option) any later version.
 *
 * This program is distributed in the hope that it will be useful,  but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE.  See the  GNU General Public License
 * for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301,
 * USA.
 *
 * @author Thiago Ribeiro Masaki <masaki@profusion.mobi>
 */
#ifndef ELM_WIDGET_HELPER
#define ELM_WIDGET_HELPER
#include <Elementary.h>
const char * entry_value_get(Evas_Object *obj, char *part_name);
void entry_value_set(Evas_Object *obj, char *part_name, const char *value);
int radio_value_get(Evas_Object *obj, char *part_name);
void radio_value_set(Evas_Object *obj, char *part_name, int value);
void list_selected_set(Evas_Object *obj, const char *value);
void button_hide(Evas_Object *obj, char *part_name);
void button_show(Evas_Object *obj, char *part_name);
#endif
