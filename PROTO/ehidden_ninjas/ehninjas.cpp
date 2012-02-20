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
#include "singleton.h"
#include "memmgr.h"
#include "ehninjas.h"

using namespace ehninjas;

static void _key_down_cb(void *data,
                         Evas *e,
                         Evas_Object *obj,
                         void *event_info)
{
   printf("KEY DOWN!\n");
   elm_exit();
}

Eina_Bool App ::CreateWin(const char *title,
                          unsigned int width,
                          unsigned int height)
{
   Evas_Object *win = elm_win_add(NULL, title, ELM_WIN_BASIC);
   if (!win) return EINA_FALSE;
   elm_win_autodel_set(win, EINA_TRUE);
   elm_win_title_set(win, title);
   evas_object_resize(win, width, height);
   evas_object_show(win);

   this->win = win;

   return EINA_TRUE;
}


Eina_Bool App ::CreateBg(int r, int g, int b)
{
   Evas *e = evas_object_evas_get(this->win);
   assert(e);

   //background
   Evas_Object *bg = elm_bg_add(win);
   if (!bg) return EINA_FALSE;

   evas_object_size_hint_weight_set(bg,
                                    EVAS_HINT_EXPAND,
                                    EVAS_HINT_EXPAND);
   //TODO: Need to set bg image instead of color.
   elm_bg_color_set(bg, r, g, b);
   elm_win_resize_object_add(win, bg);
   evas_object_show(bg);

   return EINA_TRUE;
}

Eina_Bool App ::Initialize(int argc, char **argv)
{
   //Prevent multiple initialization
   if (this->initialized == EINA_TRUE)
     {
        PRINT_DBG("App is initialized already!");
        return EINA_FALSE;
     }

   //Initialize elementary
   elm_init(argc, argv);

   //Initialize Memory Manager
   MemoryMgr *memmgr = new MemoryMgr();
   if (!memmgr)
     {
        PRINT_DBG("Failed to create MemoryMgr!");
        return EINA_FALSE;
     }
   memmgr->Initialize(MEMPOOL_SIZE);

   if (!this->CreateWin("eHidden Ninjas", 400, 400))
     {
        PRINT_DBG("Failed to create elm_win!");
        return EINA_FALSE;
     }

   if (!this->CreateBg(0, 0, 255))
     {
        PRINT_DBG("Failed to create elm_bg!");
     }

   /*
      Block block = new Block();
      if (!block) return EINA_FALSE;
    */

   //Set key events callbacks to window
   evas_object_event_callback_add(win,
                                  EVAS_CALLBACK_KEY_DOWN,
                                  _key_down_cb,
                                  this);

   this->memmgr = memmgr;
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
   this->memmgr->Terminate();

   //Terminate elementary
   elm_shutdown();
   this->initialized = EINA_FALSE;

   return EINA_TRUE;
}

int main(int argc, char **argv)
{
   App app;

   if (!app.Initialize(argc, argv))
     {
        PRINT_DBG("Failed to initialize application!");
        return -1;
     }

   app.Run();

   if (!app.Terminate())
     {
        PRINT_DBG("Failed to terminate application!");
        return -1;
     }

   return 0;
}
