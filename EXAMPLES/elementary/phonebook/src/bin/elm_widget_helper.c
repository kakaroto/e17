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

#include "elm_widget_helper.h"
const char *
entry_value_get(Evas_Object *obj, char *part_name)
{
   Evas_Object *entry_text;
   entry_text = edje_object_part_external_object_get(obj, part_name);
   return elm_scrolled_entry_entry_get(entry_text);
}

void
entry_value_set(Evas_Object *obj, char *part_name, const char *value)
{
   Evas_Object *entry_text;
   entry_text = edje_object_part_external_object_get(obj, part_name);
   elm_scrolled_entry_entry_set(entry_text, value);
}

int
radio_value_get(Evas_Object *obj, char *part_name)
{
   Evas_Object *radio_value;
   radio_value = edje_object_part_external_object_get(obj, part_name);
   return elm_radio_value_get(radio_value);
}

void
radio_value_set(Evas_Object *obj, char *part_name, int value)
{
   Evas_Object *radio_value;
   radio_value = edje_object_part_external_object_get(obj, part_name);
   elm_radio_value_set(radio_value, value);
}

void
list_selected_set(Evas_Object *obj, const char *value)
{
   Elm_List_Item *it;
   const Eina_List *iter;

   EINA_LIST_FOREACH(elm_list_items_get(obj), iter, it)
     if (strstr(elm_list_item_label_get(it), value)){
	elm_list_item_selected_set(it, EINA_TRUE);
	return;
     }
}

void
button_hide(Evas_Object *obj, char *part_name)
{
   Evas_Object *button;
   button = edje_object_part_external_object_get(obj, part_name);
   evas_object_hide(button);
}

void
button_show(Evas_Object *obj, char *part_name)
{
   Evas_Object *button;
   button = edje_object_part_external_object_get(obj, part_name);
   evas_object_show(button);
}
