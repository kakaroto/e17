/*
 * enna_event.c
 * Copyright (C) Nicolas Aguirre 2006,2007,2008 <aguirre.nicolas@gmail.com>
 *
 * enna_event.c is free software copyrighted by Nicolas Aguirre.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name ``Nicolas Aguirre'' nor the name of any other
 *    contributor may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * enna_event.c IS PROVIDED BY Nicolas Aguirre ``AS IS'' AND ANY EXPRESS
 * OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL Nicolas Aguirre OR ANY OTHER CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
 * BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "enna.h"
#include <stdio.h>
#include <stdlib.h>
#include "enna_config.h"
#include "enna_mainmenu.h"
#include "enna_module.h"
#include "enna_module_music.h"
#include "enna_module_video.h"
#include "enna_module_playlist.h"
#include "enna_mediaplayer.h"
#include "enna_miniplayer.h"
#include "enna_lirc.h"
#include "enna_event.h"

struct keyboard_mapping
{
   char               *name;
   char               *code;
   enna_event          evt;
} keyboard_mapping[] =
{
   {
   "Back", "BackSpace", enna_event_escape},
   {
   "Exit", "q", enna_event_exit},
   {
   "Exit", "Q", enna_event_exit},
   {
   "Play", "p", enna_event_play},
   {
   "Pause", "o", enna_event_pause},
   {
   "FastForward", "Next", enna_event_fastforward},
   {
   "Rewind", "Prior", enna_event_rewind},
   {
   "Stop", "Supr", enna_event_stop},
   {
   "Mute", "m", enna_event_mute},
   {
   "Down", "Down", enna_event_down},
   {
   "Up", "Up", enna_event_up},
   {
   "Right", "Right", enna_event_right},
   {
   "Left", "Left", enna_event_left},
   {
   "Aspect", "a", enna_event_aspect},
   {
   "Language", "l", enna_event_language},
   {
   "FullScreen", "f", enna_event_fullscreen},
   {
   "AddToPlaylist", "KP_Delete", enna_event_addtopl},
   {
   "Select", "Return", enna_event_enter},
   {
   "Next", "n", enna_event_next},
   {
   "Prev", "b", enna_event_prev},
   {
   "0", "0", enna_event_0},
   {
   "1", "1", enna_event_1},
   {
   "2", "2", enna_event_2},
   {
   "3", "3", enna_event_3},
   {
   "4", "4", enna_event_4},
   {
   "5", "5", enna_event_5},
   {
   "6", "6", enna_event_6},
   {
   "7", "7", enna_event_7},
   {
   "8", "8", enna_event_8},
   {
   "9", "9", enna_event_9},
   {
   "0", "KP_0", enna_event_0},
   {
   "1", "KP_1", enna_event_1},
   {
   "2", "KP_2", enna_event_2},
   {
   "3", "KP_3", enna_event_3},
   {
   "4", "KP_4", enna_event_4},
   {
   "5", "KP_5", enna_event_5},
   {
   "6", "KP_6", enna_event_6},
   {
   "7", "KP_7", enna_event_7},
   {
   "8", "KP_8", enna_event_8},
   {
   "9", "KP_9", enna_event_9},
   {
   "", "", enna_event_none}
};

EAPI void
enna_event_process_event(Enna * enna, enna_event event)
{

   if (enna->fs_obj)
     {
	enna_module_process_event(enna->modules, event);
	return;
     }
   else if (enna_mediaplayer_has_focus_get(enna->mediaplayer))
     {
	enna_mediaplayer_process_event(enna->mediaplayer, event);
	return;
     }

   switch (event)
     {
     case enna_event_up:
     case enna_event_down:
	{
	   if (!enna_module_has_focus_get(enna->modules))
	     {
		enna_module_has_focus_set(enna->modules, 1);
		enna_mainmenu_has_focus_set(enna->mainmenu, 0);
		enna_module_process_event(enna->modules, event);
	     }
	   else
	     {
		enna_module_process_event(enna->modules, event);
	     }
	   break;
	}
     case enna_event_left:
     case enna_event_right:
	{
	   if (enna_module_has_focus_get(enna->modules))
	     {
		enna_module_has_focus_set(enna->modules, 0);
		enna_mainmenu_has_focus_set(enna->mainmenu, 1);
		enna_mainmenu_process_event(enna->mainmenu, event);
	     }
	   else
	     {
		enna_mainmenu_process_event(enna->mainmenu, event);
	     }
	   break;
	}
     case enna_event_exit:
	{
	   if (enna_mediaplayer_has_focus_get(enna->mediaplayer))
	     {
		enna_mediaplayer_process_event(enna->mediaplayer, event);
		return;
	     }
	   else
	      ecore_main_loop_quit();
	   return;
	}
     case enna_event_fullscreen:
	{
	   enna_togle_fullscreen(enna);
	   break;
	}
     case enna_event_enter:
	{
	   if (enna_mainmenu_has_focus_get(enna->mainmenu))
	      enna_mainmenu_process_event(enna->mainmenu, event);
	   else if (enna_module_has_focus_get(enna->modules))
	      enna_module_process_event(enna->modules, event);
	   else
	      enna_module_process_event(enna->modules, event);
	   break;
	}
     default:
	{
	   enna_module_process_event(enna->modules, event);
	}
     }
}

EAPI void
enna_event_lirc_code(void *data, char *code)
{
   Enna               *enna;

   enna_event          event = enna_event_none;
   int                 i = 0;

   enna = (Enna *) data;

   if (!enna)
      return;

   while (keyboard_mapping[i].evt != enna_event_none)
     {
	if (!strcmp(code, keyboard_mapping[i].name))
	  {
	     event = keyboard_mapping[i].evt;
	     break;
	  }
	i++;
     }

   enna_event_process_event(enna, event);
}

/* key down callback */
EAPI void
enna_event_bg_key_down_cb(void *data, Evas * e, Evas_Object * obj,
			  void *event_info)
{
   Enna               *enna;
   Evas_Event_Key_Down *ev;
   enna_event          event = enna_event_none;
   int                 i = 0;

   enna = (Enna *) data;
   ev = (Evas_Event_Key_Down *) event_info;

   if (!ev || !enna)
      return;
   /* if left or right pressed and if mainemnu doesn't have focus
    * set focus to mainemenu */
   /* if up down esc pressed and if moduled doesn't have focus
    * set focus to modules */

   //dbg("ev->keyname : %s; ev->key : %s; ev->string : %s; ev->compose : %s\n", ev->keyname, ev->key, ev->string, ev->compose);
   while (keyboard_mapping[i].evt != enna_event_none)
     {
	if (!strcmp(ev->key, keyboard_mapping[i].code))
	  {
	     event = keyboard_mapping[i].evt;
	     break;
	  }
	i++;
     }

   enna_event_process_event(enna, event);
}

EAPI void
enna_togle_fullscreen(Enna * enna)
{
   if (ecore_evas_fullscreen_get(enna->ee))
      ecore_evas_fullscreen_set(enna->ee, 0);
   else
      ecore_evas_fullscreen_set(enna->ee, 1);
}

EAPI void
enna_event_init(Enna * enna)
{
   int                 i = 0;

   while (keyboard_mapping[i].evt != enna_event_none)
     {
	char               *value =
	   enna_config_get_conf_value("keyboard", keyboard_mapping[i].name);
	if (value)
	   keyboard_mapping[i].code = strdup(value);
	i++;
     }
};
