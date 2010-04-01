/*
 * @file
 *
 * Copyright (C) 2010 by ProFUSION embedded systems
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or (at your
 * option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE.  See the  GNU General Public License
 * for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 * @author Fabiano Fidêncio <fidencio@profusion.mobi>
 */

#include <Elementary.h>
#include <stdio.h>
#include "gui.h"

void converter_distance(void *data, Evas_Object *obj, void *event_info);
void converter_temperature(void *data, Evas_Object *obj, void *event_info);
void km_in(void *data, Evas_Object *obj, void *event_info);
void m_in(void *data, Evas_Object *obj, void *event_info);
void cm_in(void *data, Evas_Object *obj, void *event_info);
void km_out(void *data, Evas_Object *obj, void *event_info);
void m_out(void *data, Evas_Object *obj, void *event_info);
void cm_out(void *data, Evas_Object *obj, void *event_info);

void
populate_converters_list(Evas_Object *list, Pginfo *info)
{
   elm_list_item_append(list, "Distance Converter", NULL, NULL,
			converter_distance, info);
   elm_list_item_append(list, "Temperature Converter", NULL, NULL,
			converter_temperature, NULL);
   elm_list_go(list);
}

void
converter_distance(void *data, Evas_Object *obj, void *event_info)
{
   Elm_List_Item *it;
   it = elm_list_selected_item_get(obj);
   elm_list_item_selected_set(it, EINA_FALSE);

   Pginfo *info = data;
   elm_pager_content_promote(info->pager, info->pg2);
}

void
converter_temperature(void *data, Evas_Object *obj, void *event_info)
{
   printf("Sample not implemented\n");
}

void
populate_measurements_list_in(Evas_Object *list, Evas_Object *ed,
				Pginfo *info)
{
   elm_list_item_append(list, "Kilometer", NULL, NULL, km_in, ed);
   elm_list_item_append(list, "Meter", NULL, NULL, m_in, ed);
   elm_list_item_append(list, "Centimeter", NULL, NULL, cm_in, ed);
   elm_list_go(list);
}

void
populate_measurements_list_out(Evas_Object *list, Evas_Object *ed,
				Pginfo *info)
{
   elm_list_item_append(list, "Kilometer", NULL, NULL, km_out, ed);
   elm_list_item_append(list, "Meter", NULL, NULL, m_out, ed);
   elm_list_item_append(list, "Centimeter", NULL, NULL, cm_out, ed);
   elm_list_go(list);
}

void
km_in(void *data, Evas_Object *obj, void *event_info) {
     Evas_Object *ed = data;

     edje_object_part_text_set(ed, "input", "");
     edje_object_part_text_set(ed, "output", "");
     edje_object_part_text_set(ed, "equal", "");
     edje_object_part_text_set(ed, "in_mes", "km");
}

void
m_in(void *data, Evas_Object *obj, void *event_info)
{
   Evas_Object *ed = data;

   edje_object_part_text_set(ed, "input", "");
   edje_object_part_text_set(ed, "output", "");
   edje_object_part_text_set(ed, "equal", "");
   edje_object_part_text_set(ed, "in_mes", "m");
}

void
cm_in(void *data, Evas_Object *obj, void *event_info)
{
   Evas_Object *ed = data;

   edje_object_part_text_set(ed, "input", "");
   edje_object_part_text_set(ed, "output", "");
   edje_object_part_text_set(ed, "equal", "");
   edje_object_part_text_set(ed, "in_mes", "cm");
}

void
km_out(void *data, Evas_Object *obj, void *event_info)
{
   Evas_Object *ed = data;

   edje_object_part_text_set(ed, "output", "");
   edje_object_part_text_set(ed, "out_mes", "km");
}

void
m_out(void *data, Evas_Object *obj, void *event_info)
{
   Evas_Object *ed = data;

   edje_object_part_text_set(ed, "output", "");
   edje_object_part_text_set(ed, "out_mes", "m");
}

void
cm_out(void *data, Evas_Object *obj, void *event_info)
{
   Evas_Object *ed = data;

   edje_object_part_text_set(ed, "output", "");
   edje_object_part_text_set(ed, "out_mes", "cm");
}
