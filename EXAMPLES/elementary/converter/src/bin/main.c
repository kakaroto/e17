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

static void
_win_delete_request(void *data, Evas_Object *obj, void *event_info)
{
   elm_exit();
}

int
elm_main(int argc, char **argv)
{
   Evas_Object *bg, *win, *pg = NULL;
   static Pginfo info;
   Evas_Coord w = 316, h = 520;
   Measurements_Lists *ml = malloc(sizeof(Measurements_Lists));

   win = elm_win_add(NULL, "converters-list", ELM_WIN_BASIC);
   evas_object_resize(win, w, h);
   elm_win_title_set(win, "Converters");

   bg = elm_bg_add(win);
   elm_win_resize_object_add(win, bg);
   evas_object_size_hint_weight_set(bg, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   evas_object_smart_callback_add(win, "delete-request",
					_win_delete_request, NULL);
   evas_object_show(bg);

   create_pager(win, pg, &info, ml);

   evas_object_show(win);

   elm_run();
   elm_shutdown();

   free(ml);
   return 0;
}
ELM_MAIN()
