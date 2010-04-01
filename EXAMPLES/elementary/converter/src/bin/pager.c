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

static int _log_domain = -1;
#define CRITICAL(...) EINA_LOG_DOM_CRIT(_log_domain, __VA_ARGS__)
#define ERR(...) EINA_LOG_DOM_ERR(_log_domain, __VA_ARGS__)
#define WRN(...) EINA_LOG_DOM_WARN(_log_domain, __VA_ARGS__)
#define INF(...) EINA_LOG_DOM_INFO(_log_domain, __VA_ARGS__)

#define DBG(...) EINA_LOG_DOM_DBG(_log_domain, __VA_ARGS__)

#define KM_LABEL    "Kilometer"
#define M_LABEL     "Meter"
#define CM_LABEL    "Centimeter"
#define KILOMETER   0
#define METER       1
#define CENTIMETER  2

void
back_page_1(void *data, Evas_Object *obj, const char *emission,
		const char *source)
{
   Pginfo *info = data;
   elm_pager_content_promote(info->pager, info->pg1);
}

void
on_number_button_clicked(void *data, Evas_Object *obj, const char *emission,
				const char *source)
{
   const char *n = data;
   char buf[1024];
   int aux;
   int current = atoi(edje_object_part_text_get(obj, "input"));

   aux = atoi(n);
   aux += current*10;
   snprintf(buf, sizeof(buf), "%d", aux);

   edje_object_part_text_set(obj, "input", buf);
}

float
converter(float n, int from, int to)
{
   if (from == to)
     return n;

   switch (from)
     {
      case KILOMETER:
	 n *= 1000;
	 break;
      case CENTIMETER:
	 n /= 100;
	 break;
      default:
	 break;
     }

   switch (to)
     {
      case KILOMETER:
	 n /= 1000;
	 break;
      case CENTIMETER:
	 n *= 100;
	 break;
      default:
	 break;
     }

   return n;
}

void
on_number_button_enter(void *data, Evas_Object *obj, const char *emission,
			const char *source)
{
   Measurements_Lists *ml = data;
   ml = data;
   float n_in, n_out;
   int m_in, m_out;
   char label_in[128], label_out[128];
   char result[64];
   Elm_List_Item *it_in, *it_out;
   it_in = elm_list_selected_item_get(ml->list_in);
   it_out = elm_list_selected_item_get(ml->list_out);

   if (!it_in || !it_out ||
	!strcmp((edje_object_part_text_get(obj, "input")), ""))
     {
	edje_object_part_text_set(obj, "input", "");
	edje_object_part_text_set(obj, "output", "");
	return;
     }

   edje_object_part_text_set(obj, "equal", "=");
   snprintf(label_in, sizeof(label_in), "%s", elm_list_item_label_get(it_in));
   snprintf(label_out, sizeof(label_out), "%s",
		elm_list_item_label_get(it_out));

   if (!strcmp(label_in, KM_LABEL)) {
	m_in = KILOMETER;
   } else if(!strcmp(label_in, M_LABEL)) {
	m_in = METER;
   } else if(!strcmp(label_in, CM_LABEL)) {
	m_in = CENTIMETER;
   }

   if (!strcmp(label_out, KM_LABEL))
     m_out = KILOMETER;
   else if(!strcmp(label_out, M_LABEL))
     m_out = METER;
   else if(!strcmp(label_out, CM_LABEL))
     m_out = CENTIMETER;

   n_in = atof(edje_object_part_text_get(obj, "input"));
   n_out = converter(n_in, m_in, m_out);

   snprintf(result, sizeof(result), "%f", n_out);
   edje_object_part_text_set(obj, "output", result);
}

void
on_number_button_clear(void *data, Evas_Object *obj, const char *emission,
			const char *source)
{
   edje_object_part_text_set(obj, "input", "");
   edje_object_part_text_set(obj, "output", "");
}

void
create_pager(Evas_Object *parent, Evas_Object *pg, Pginfo *info,
		Measurements_Lists *ml)
{
   Evas_Object *ed, *converters_list, *layout;
   pg = elm_pager_add(parent);
   elm_win_resize_object_add(parent, pg);
   evas_object_size_hint_weight_set(pg, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   evas_object_size_hint_align_set(pg, EVAS_HINT_FILL, EVAS_HINT_FILL);
   evas_object_show(pg);
   info->pager = pg;

   /*main page*/
   layout = elm_layout_add(parent);
   if (!elm_layout_file_set(layout, PACKAGE_DATA_DIR "/default.edj", "main"))
     {
	DBG("Can't load Edje Layout %s", PACKAGE_DATA_DIR "/default.edj");
	return;
     }
   evas_object_show(layout);
   elm_pager_content_push(pg, layout);
   info->pg1 = layout;

   converters_list = elm_list_add(parent);
   elm_layout_content_set(layout, "converters_list", converters_list);
   evas_object_size_hint_weight_set(converters_list,
					EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   evas_object_size_hint_fill_set(converters_list,
					EVAS_HINT_FILL, EVAS_HINT_FILL);
   evas_object_show(converters_list);
   populate_converters_list(converters_list, info);

   /* distance conerter*/
   layout = elm_layout_add(parent);
   if (!elm_layout_file_set(layout, PACKAGE_DATA_DIR "/default.edj",
				"distance_converter"))
     {
	DBG("Can't load Edje Layout %s", PACKAGE_DATA_DIR "/default.edj");
	return;
     }
   evas_object_show(layout);
   ed = elm_layout_edje_get(layout);
   edje_object_part_text_set(ed, "equal", "");
   edje_object_part_text_set(ed, "input", "");
   edje_object_part_text_set(ed, "output", "");

   ml->list_in = elm_list_add(parent);
   elm_layout_content_set(layout, "measurements_in", ml->list_in);
   evas_object_size_hint_weight_set(ml->list_in,
					EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   evas_object_size_hint_fill_set(ml->list_in,
					EVAS_HINT_FILL, EVAS_HINT_FILL);
   evas_object_show(ml->list_in);
   populate_measurements_list_in(ml->list_in, ed, info);

   ml->list_out = elm_list_add(parent);
   elm_layout_content_set(layout, "measurements_out", ml->list_out);
   evas_object_size_hint_weight_set(ml->list_out,
					EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   evas_object_size_hint_fill_set(ml->list_out,
					EVAS_HINT_FILL, EVAS_HINT_FILL);
   evas_object_show(ml->list_out);
   populate_measurements_list_out(ml->list_out, ed, info);

   /* Set callback functions */
   edje_object_signal_callback_add(ed, "mouse,clicked,1", "bt_1",
					on_number_button_clicked, "1");
   edje_object_signal_callback_add(ed, "mouse,clicked,1", "bt_2",
					on_number_button_clicked, "2");
   edje_object_signal_callback_add(ed, "mouse,clicked,1", "bt_3",
					on_number_button_clicked, "3");
   edje_object_signal_callback_add(ed, "mouse,clicked,1", "bt_4",
					on_number_button_clicked, "4");
   edje_object_signal_callback_add(ed, "mouse,clicked,1", "bt_5",
					on_number_button_clicked, "5");
   edje_object_signal_callback_add(ed, "mouse,clicked,1", "bt_6",
					on_number_button_clicked, "6");
   edje_object_signal_callback_add(ed, "mouse,clicked,1", "bt_7",
					on_number_button_clicked, "7");
   edje_object_signal_callback_add(ed, "mouse,clicked,1", "bt_8",
					on_number_button_clicked, "8");
   edje_object_signal_callback_add(ed, "mouse,clicked,1", "bt_9",
					on_number_button_clicked, "9");
   edje_object_signal_callback_add(ed, "mouse,clicked,1", "bt_0",
					on_number_button_clicked, "0");
   edje_object_signal_callback_add(ed, "mouse,clicked,1", "bt_clear",
					on_number_button_clear, ml);
   edje_object_signal_callback_add(ed, "mouse,clicked,1", "bt_enter",
					on_number_button_enter, ml);
   edje_object_signal_callback_add(ed, "mouse,clicked,1", "back",
					 back_page_1, info);
   elm_pager_content_push(pg, layout);
   info->pg2 = layout;
   elm_pager_content_promote(info->pager, info->pg1);
}
