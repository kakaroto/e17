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
#include "defines.h"
#include "ehninjas.h"

using namespace ehninjas;

static void _key_down_cb(void *data,
                         Evas *e,
                         Evas_Object *obj,
                         void *event_info)
{
   printf("KEY DOWN!\n");
}

Eina_Bool App:: Initialize(int argc, char **argv)
{
   //Prevent multiple initialization
   if (this->initialized == EINA_TRUE)
     {
        PRINT_DBG("App is initialized already!");
        return EINA_FALSE;
     }

   //Initialize elementary
   elm_init(argc, argv);

   //window
   Evas_Object *win = elm_win_add(NULL, "eHidden Ninjas", ELM_WIN_BASIC);
   if (!win) return EINA_FALSE;
   elm_win_autodel_set(win, EINA_TRUE);
   elm_win_title_set(win, "eHidden Ninjas");
   evas_object_resize(win, 400, 400);
   evas_object_show(win);

   Evas *e = evas_object_evas_get(win);
   assert(e);

   //background
   Evas_Object *bg = elm_bg_add(win);
   assert(bg);

   evas_object_size_hint_weight_set(bg,
                                    EVAS_HINT_EXPAND,
                                    EVAS_HINT_EXPAND);
   //TODO: Need to set bg image instead of color.
   elm_bg_color_set(bg, 0, 0, 255);
   elm_win_resize_object_add(win, bg);
   evas_object_show(bg);

   /*
      Block block = new Block();
      if (!block) return EINA_FALSE;
    */

   //Set key events callbacks to window
   evas_object_event_callback_add(win,
                                  EVAS_CALLBACK_KEY_DOWN,
                                  _key_down_cb,
                                  this);

   this->win = win;
   this->bg = bg;
   this->initialized = EINA_TRUE;

   return EINA_TRUE;
}

Eina_Bool App:: Run()
{
   //Run elementary main loop
   elm_run();

   return EINA_TRUE;
}

Eina_Bool App:: Terminate()
{
   //Terminate elementary
   elm_exit();
   this->initialized = EINA_FALSE;

   return EINA_TRUE;
}

int main(int argc, char **argv)
{
   App app;
   app.Initialize(argc, argv);
   app.Run();
   app.Terminate();

   return 0;
}
