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
#include "maths.h"
#include "defines.h"
#include "singleton.h"
#include "memmgr.h"
#include "player_char.h"
#include "ehninjas.h"



using namespace ehninjas;

static Eina_Bool g_mouse_btns = 0;

static void _win_del_cb(void *data,
                        Evas *e,
                        Evas_Object *obj,
                        void *event_info)
{
   elm_exit();
}

static Eina_Bool _key_up_cb(void *data,
                            int type,
                            void *event)
{
   App *app = static_cast<App *>(data);
   assert(app);

   Ecore_Event_Key *ev = static_cast<Ecore_Event_Key *>(event);
   assert(ev);

   app->DispatchKeyUp(ev->keyname);

   return ECORE_CALLBACK_PASS_ON;
}



static Eina_Bool _key_down_cb(void *data,
                              int type,
                              void *event)
{
   App *app = static_cast<App *>(data);
   assert(app);

   Ecore_Event_Key *ev = static_cast<Ecore_Event_Key *>(event);
   assert(ev);

   app->DispatchKeyDown(ev->keyname);

   return ECORE_CALLBACK_PASS_ON;
}



static Eina_Bool _mouse_down_cb(void *data,
                                int type,
                                void *event)
{
   App *app = static_cast<App *>(data);
   assert(app);

   Ecore_Event_Mouse_Button *ev = static_cast<Ecore_Event_Mouse_Button *>(event);
   assert(ev);

   if (ev->buttons == 0x03)
     g_mouse_btns |= MOUSE_BUTTON2;
   else if (ev->buttons == 0x01)
     g_mouse_btns |= MOUSE_BUTTON1;

   app->DispatchMouseDown();

   return ECORE_CALLBACK_PASS_ON;

}



static Eina_Bool _mouse_move_cb(void *data,
                                int type,
                                void *event)
{
   App *app = static_cast<App *>(data);
   assert(app);

   Ecore_Event_Mouse_Move *ev = static_cast<Ecore_Event_Mouse_Move *>(event);
   assert(ev);

   app->DispatchMouseMove(ev->x, ev->y, g_mouse_btns);

   return ECORE_CALLBACK_PASS_ON;
}



static Eina_Bool _mouse_up_cb(void *data,
                              int type,
                              void *event)
{
   App *app = static_cast<App *>(data);
   assert(app);

   Ecore_Event_Mouse_Button *ev = static_cast<Ecore_Event_Mouse_Button *>(event);
   assert(ev);

   if (ev->buttons == 0x03)
     g_mouse_btns ^= MOUSE_BUTTON2;
   else if (ev->buttons == 0x01)
     g_mouse_btns ^= MOUSE_BUTTON1;

   app->DispatchMouseUp();

   return ECORE_CALLBACK_PASS_ON;
}



void App ::DispatchKeyDown(const char * const keyname)
{
   assert(keyname);
   PRINT_DBG(keyname);

   //Exit Game
   if (!strcmp("Escape", keyname))
     elm_exit();

   //Move Player Character
/*   if (!strcmp(keyname, "w"))
     this->pc->Move(Character ::Up, 20);

   if (!strcmp(keyname, "s"))
     this->pc->Move(Character ::Down, 20);

   if (!strcmp(keyname, "a"))
     this->pc->Move(Character ::Left, 20);

   if (!strcmp(keyname, "d"))
     this->pc->Move(Character ::Right, 20); */
}



void App ::DispatchKeyUp(const char * const keyname)
{
   assert(keyname);
   PRINT_DBG(keyname);
}



void App ::DispatchMouseDown()
{
}



void App ::DispatchMouseMove(int x, int y, int buttons)
{
   if ((buttons & MOUSE_BUTTON2) != MOUSE_BUTTON2)
     return;

   //Calculate Direction
   VECTOR2 dir(static_cast<float>(x), static_cast<float>(y));
   dir -= this->pc->Position();
   dir.Normalize();

   printf("Direction: %f %f\n", dir.x, dir.y);
   this->pc->Move(dir, 2);
}



void App ::DispatchMouseUp()
{

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
   evas_object_event_callback_add(win, EVAS_CALLBACK_DEL, _win_del_cb, this);
   evas_object_show(win);

   this->e = evas_object_evas_get(win);
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



Eina_Bool App ::InitPlayerChar()
{
   assert(this->e);

   PlayerChar *pc = new PlayerChar();
   assert(pc);

   //Player Character for test
   Evas_Object *obj = evas_object_rectangle_add(this->e);
   if (!obj) return EINA_FALSE;
   evas_object_color_set(obj, 255, 255, 0, 255);
   evas_object_resize(obj, 30, 30);
   evas_object_show(obj);

   pc->SetImgObj(obj);

   this->pc = pc;

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

   if (!this->CreateWin("eHidden Ninjas", SCRN_WIDTH, SCRN_HEIGHT))
     {
        PRINT_DBG("Failed to create elm_win!");
        return EINA_FALSE;
     }

   if (!this->CreateBg(0, 0, 255))
     {
        PRINT_DBG("Failed to create elm_bg!");
     }

   //Set key events callbacks to window
   ecore_event_handler_add(ECORE_EVENT_KEY_DOWN,
                           _key_down_cb,
                           this);
   ecore_event_handler_add(ECORE_EVENT_KEY_UP,
                           _key_up_cb,
                           this);
   ecore_event_handler_add(ECORE_EVENT_MOUSE_BUTTON_DOWN,
                           _mouse_down_cb,
                           this);
   ecore_event_handler_add(ECORE_EVENT_MOUSE_MOVE,
                           _mouse_move_cb,
                           this);
   ecore_event_handler_add(ECORE_EVENT_MOUSE_BUTTON_UP,
                           _mouse_up_cb,
                           this);

   if (!this->InitPlayerChar())
     {
        PRINT_DBG("Failed to init player character!");
     }

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
   delete(this->pc);

   //Terminate elementary
   elm_shutdown();
   this->initialized = EINA_FALSE;

   return EINA_TRUE;
}



App ::App() :memmgr(NULL), pc(NULL), e(NULL), win(NULL), bg(NULL)
{
}



App ::~App()
{
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
