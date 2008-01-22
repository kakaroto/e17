
/*
 * enna_module.c
 * Copyright (C) Nicolas Aguirre 2006,2007,2008 <aguirre.nicolas@gmail.com>
 *
 * enna_module.c is free software copyrighted by Nicolas Aguirre.
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
 * enna_module.c IS PROVIDED BY Nicolas Aguirre ``AS IS'' AND ANY EXPRESS
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

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <Evas.h>
#include <Ecore.h>
#include <Ecore_File.h>
#include <Ecore_Evas.h>
#include <Edje.h>
#include "enna.h"
#include "enna_module.h"
#include "enna_util.h"

#define SMART_NAME "enna_module"
#define API_ENTRY E_Smart_Data *sd; sd = evas_object_smart_data_get(obj); if ((!obj) || (!sd) || (evas_object_type_get(obj) && strcmp(evas_object_type_get(obj), SMART_NAME)))
#define INTERNAL_ENTRY E_Smart_Data *sd; sd = evas_object_smart_data_get(obj); if (!sd) return;

typedef struct _E_Smart_Data E_Smart_Data;
typedef struct _E_Smart_Data_Item E_Smart_Data_Item;

struct _E_Smart_Data
{
   Evas_Coord          x, y, w, h;
   Evas_Object        *edje;
   Evas_Object        *obj;
   Evas_List          *modules;
   unsigned char       selected;
   unsigned int        has_focus;
};

struct _E_Smart_Data_Item
{
   Evas_Object        *obj;
   char               *name;
   unsigned int        selected:1;
   unsigned char       type;
};

/* local subsystem functions */
static void         _focus_module_set(E_Smart_Data_Item * si,
				      unsigned int focus);
static int          _late_clear(void *data);
static void         _e_smart_reconfigure(E_Smart_Data * sd);
static void         _e_smart_add(Evas_Object * obj);
static void         _e_smart_del(Evas_Object * obj);
static void         _e_smart_move(Evas_Object * obj, Evas_Coord x,
				  Evas_Coord y);
static void         _e_smart_resize(Evas_Object * obj, Evas_Coord w,
				    Evas_Coord h);
static void         _e_smart_show(Evas_Object * obj);
static void         _e_smart_hide(Evas_Object * obj);
static void         _e_smart_color_set(Evas_Object * obj, int r, int g, int b,
				       int a);
static void         _e_smart_clip_set(Evas_Object * obj, Evas_Object * clip);
static void         _e_smart_clip_unset(Evas_Object * obj);
static void         _e_smart_init(void);

/* local subsystem globals */
static Evas_Smart  *_e_smart = NULL;

/* externally accessible functions */

/**
* @internal
* @brief Create and add enna_module to an Evas
* @param evas an evas
* @return Returns the new Evas_Object
*/
EAPI Evas_Object   *
enna_module_add(Evas * evas)
{
   _e_smart_init();
   return evas_object_smart_add(evas, _e_smart);
}

/**
* @internal
* @brief Gets the type of an Etk_Alignment
* @return Returns the type of an Etk_Alignment
*/
EAPI unsigned int
enna_module_has_focus_get(Evas_Object * obj)
{
   API_ENTRY           return 0;

   return sd->has_focus;
}

/**
* @internal
* @brief Create and add enna_module to an Evas
* @param evas en evas
* @return Returns the new Evas_Object
*/
EAPI void
enna_module_has_focus_set(Evas_Object * obj, unsigned int focus)
{
   API_ENTRY           return;

   if (focus == sd->has_focus)
      return;

   sd->has_focus = focus;
   if (focus)
      edje_object_signal_emit(sd->edje, "enna,state,focus", "enna");
   else
      edje_object_signal_emit(sd->edje, "enna,state,unfocus", "enna");
}

EAPI void
enna_module_append(Evas_Object * obj, Evas_Object * obj_module, char *name,
		   unsigned char type)
{
   API_ENTRY           return;
   E_Smart_Data_Item  *si;

   si = (E_Smart_Data_Item *) malloc(sizeof(E_Smart_Data_Item));
   si->name = strdup(name);
   si->obj = obj_module;
   si->selected = 1;
   si->type = type;
   sd->modules = evas_list_append(sd->modules, si);

}

EAPI void
enna_module_prepend(Evas_Object * obj, Evas_Object * obj_module, char *name,
		    unsigned char type)
{
   API_ENTRY;
   E_Smart_Data_Item  *si;

   si = (E_Smart_Data_Item *) malloc(sizeof(E_Smart_Data_Item));
   si->name = strdup(name);
   si->obj = obj_module;
   si->selected = 1;
   si->type = type;
   sd->modules = evas_list_prepend(sd->modules, si);
}

EAPI int
enna_module_selected_name_set(Evas_Object * obj, char *name)
{
   E_Smart_Data_Item  *si;
   int                 i;
   Evas_List          *l;
   API_ENTRY           return -1;

   if (!sd || !sd->modules)
      return -1;

   for (i = 0, l = sd->modules; l; l = l->next, i++)
     {
	si = l->data;
	if (!strcmp(si->name, name))
	  {
	     enna_module_selected_set(obj, i);
	     return i;
	  }
     }

   return -1;

}

EAPI void
enna_module_selected_set(Evas_Object * obj, int n)
{
   E_Smart_Data_Item  *si, *psi;
   Evas_Object        *obj1;
   API_ENTRY           return;

   if (!sd->modules)
      return;
   if (n >= evas_list_count(sd->modules))
      n = evas_list_count(sd->modules) - 1;
   else if (n < 0)
      n = 0;
   if (sd->selected == n)
      return;

   si = evas_list_nth(sd->modules, n);
   if (!si)
      return;

   si->selected = 1;

   //evas_object_show (si->obj);
   psi = evas_list_nth(sd->modules, sd->selected);
   if (!psi)
      obj1 = NULL;
   else
     {
	obj1 = psi->obj;
	psi->selected = 0;

     }

   enna_module_has_focus_set(obj, 1);
   enna_util_switch_objects(sd->edje, obj1, si->obj);
   ecore_timer_add(1.0, _late_clear, obj1);

   _focus_module_set(psi, 0);
   _focus_module_set(si, 1);

   sd->selected = n;
}

EAPI int
enna_module_selected_get(Evas_Object * obj)
{
   API_ENTRY           return -1;

   if (!sd->modules)
      return -1;
   return sd->selected;
}

static void
_focus_module_set(E_Smart_Data_Item * si, unsigned int focus)
{

   if (!si)
      return;

   switch (si->type)
     {
     case ENNA_MODULE_MUSIC:
	enna_module_music_focus_set(si->obj, focus);
	break;
     case ENNA_MODULE_VIDEO:
	enna_module_video_focus_set(si->obj, focus);
	break;
     case ENNA_MODULE_PHOTO:
	enna_module_photo_focus_set(si->obj, focus);
	break;
     case ENNA_MODULE_PLAYLIST:
	enna_module_playlist_focus_set(si->obj, focus);
	break;
     case ENNA_MODULE_MEDIAPLAYER:
	enna_mediaplayer_focus_set(si->obj, focus);
	break;
     default:
	dbg("Module Type Unknown : %d %s\n", si->type, si->name);
     }
}

EAPI void
enna_module_process_event(Evas_Object * obj, enna_event event)
{
   E_Smart_Data_Item  *si;
   API_ENTRY           return;

   if (!sd->modules)
      return;
   si = evas_list_nth(sd->modules, sd->selected);
   if (!si) return;

   switch (si->type)
     {
     case ENNA_MODULE_MUSIC:
	enna_module_music_process_event(si->obj, event);
	break;
     case ENNA_MODULE_VIDEO:
	enna_module_video_process_event(si->obj, event);
	break;
     case ENNA_MODULE_PHOTO:
	enna_module_photo_process_event(si->obj, event);
	break;
     case ENNA_MODULE_PLAYLIST:
	enna_module_playlist_process_event(si->obj, event);
	break;
     case ENNA_MODULE_MEDIAPLAYER:
	enna_mediaplayer_process_event(si->obj, event);
	break;
     default:
	dbg("Module Type Unknown : %d %s\n", si->type, si->name);
     }
}

static int
_late_clear(void *data)
{
   Evas_Object        *obj;

   obj = (Evas_Object *) data;
   evas_object_hide(obj);
   return 0;
}

static void
_e_smart_reconfigure(E_Smart_Data * sd)
{
   evas_object_move(sd->edje, sd->x, sd->y);
   evas_object_resize(sd->edje, sd->w, sd->h);
}

static void
_e_smart_event_mouse_down(void *data, Evas * e, Evas_Object * obj,
			  void *event_info)
{
   Evas_Event_Mouse_Down *ev;

   ev = event_info;
}

static void
_e_smart_event_mouse_up(void *data, Evas * e, Evas_Object * obj,
			void *event_info)
{
   Evas_Event_Mouse_Up *ev;

   ev = event_info;

}

static void
_e_smart_add(Evas_Object * obj)
{
   E_Smart_Data       *sd;

   sd = calloc(1, sizeof(E_Smart_Data));
   if (!sd)
      return;
   evas_object_smart_data_set(obj, sd);

   sd->obj = obj;
   sd->x = 0;
   sd->y = 0;
   sd->w = 0;
   sd->h = 0;
   sd->selected = -1;
   sd->modules = NULL;
   sd->has_focus = 0;
   sd->edje = edje_object_add(evas_object_evas_get(obj));
   edje_object_file_set(sd->edje, enna_config_theme_get(), "enna/switcher");
   evas_object_smart_member_add(sd->edje, obj);
   evas_object_show(sd->edje);
   evas_object_event_callback_add(obj, EVAS_CALLBACK_MOUSE_DOWN,
				  _e_smart_event_mouse_down, NULL);
   evas_object_event_callback_add(obj, EVAS_CALLBACK_MOUSE_UP,
				  _e_smart_event_mouse_up, NULL);
   evas_object_propagate_events_set(obj, 0);
}

static void
_e_smart_del(Evas_Object * obj)
{
   INTERNAL_ENTRY;
   evas_object_del(sd->edje);
   free(sd);
}

static void
_e_smart_move(Evas_Object * obj, Evas_Coord x, Evas_Coord y)
{
   INTERNAL_ENTRY;
   if ((sd->x == x) && (sd->y == y))
      return;
   sd->x = x;
   sd->y = y;
   _e_smart_reconfigure(sd);
}

static void
_e_smart_resize(Evas_Object * obj, Evas_Coord w, Evas_Coord h)
{
   INTERNAL_ENTRY;

   if ((sd->w == w) && (sd->h == h))
      return;
   sd->w = w;
   sd->h = h;
   _e_smart_reconfigure(sd);
}

static void
_e_smart_show(Evas_Object * obj)
{
   INTERNAL_ENTRY;
   //evas_object_show (sd->edje);
}

static void
_e_smart_hide(Evas_Object * obj)
{
   INTERNAL_ENTRY;
   //evas_object_hide (sd->edje);
}

static void
_e_smart_color_set(Evas_Object * obj, int r, int g, int b, int a)
{
   INTERNAL_ENTRY;
   evas_object_color_set(sd->edje, r, g, b, a);
}

static void
_e_smart_clip_set(Evas_Object * obj, Evas_Object * clip)
{
   INTERNAL_ENTRY;
   evas_object_clip_set(sd->edje, clip);
}

static void
_e_smart_clip_unset(Evas_Object * obj)
{
   INTERNAL_ENTRY;
   evas_object_clip_unset(sd->edje);
}

/* never need to touch this */

static void
_e_smart_init(void)
{
   if (_e_smart)
      return;
   static const Evas_Smart_Class sc = {
      SMART_NAME,
      EVAS_SMART_CLASS_VERSION,
      _e_smart_add,
      _e_smart_del,
      _e_smart_move,
      _e_smart_resize,
      _e_smart_show,
      _e_smart_hide,
      _e_smart_color_set,
      _e_smart_clip_set,
      _e_smart_clip_unset,
      NULL
   };
   _e_smart = evas_smart_class_new(&sc);
}
