/*
 * Enlightement Hidden Ninjas.
 *
 * Copyright 2012 Hermet (ChunEon Park)
 *
 * This application is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This application is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301, USA
 */
#include <Eina.h>
#include <Elementary.h>
#include <cassert>
#include "object.h"

using namespace ehidden_ninjas;

static Eina_Bool init_app()
{
   //Init window
   Evas_Object *win = elm_win_add(NULL, "eHidden Ninjas", ELM_WIN_BASIC);
   if (!win) return EINA_FALSE;
   elm_win_title_set(win, "eHidden Ninjas");
   evas_object_resize(win, 400, 400);
   evas_object_show(win);

   Evas *e = evas_object_evas_get(win);

   //Set Background
   Evas_Object *rect = evas_object_rectangle_add(e);
   if (!rect) return EINA_FALSE;
   evas_object_color_set(rect, 0, 0, 0, 255);
   evas_object_resize(rect, 400, 400);
   evas_object_show(rect);

   /*
      Block block = new Block();
      if (!block) return EINA_FALSE;
    */

   return EINA_TRUE;
}

int main(int argc, char **argv)
{
   elm_init(argc, argv);
   init_app();
   elm_run();
   elm_exit();
   return 0;
}

