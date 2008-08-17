/*
 * enna_module_playlist.c
 * Copyright (C) Nicolas Aguirre 2006,2007,2008 <aguirre.nicolas@gmail.com>
 *
 * enna_module_playlist.c is free software copyrighted by Nicolas Aguirre.
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
 * enna_module_playlist.c IS PROVIDED BY Nicolas Aguirre ``AS IS'' AND ANY EXPRESS
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
#include <Ecore_Str.h>
#include <Edje.h>

#include "enna.h"
#include "enna_module.h"
#include "enna_list.h"
#include "enna_module_playlist.h"
#include "enna_config.h"
#include "enna_util.h"

#define SMART_NAME "enna_module_playlist"
#define API_ENTRY E_Smart_Data *sd; sd = evas_object_smart_data_get(obj); if ((!obj) || (!sd) || (evas_object_type_get(obj) && strcmp(evas_object_type_get(obj), SMART_NAME)))
#define INTERNAL_ENTRY E_Smart_Data *sd; sd = evas_object_smart_data_get(obj); if (!sd) return;

typedef struct _E_Smart_Data E_Smart_Data;

struct _E_Smart_Data
{
   Evas_Coord          x, y, w, h;
   Evas_Object        *edje;
   Evas_Object        *obj;
   Evas_Object        *switcher;
   Evas_Object        *menu;
   unsigned int        state;
};

/* local subsystem functions */
static void         _new_pl_cb(void *data, void *data2);
static void         _play_pl_cb(void *data, void *data2);
static void         _save_pl_cb(void *data, void *data2);
static void         _open_pl_cb(void *data, void *data2);

/*static void _pl_update(E_Smart_Data *sd);*/
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
EAPI Evas_Object   *
enna_module_playlist_add(Evas * evas)
{
   _e_smart_init();
   return evas_object_smart_add(evas, _e_smart);
}

EAPI void
enna_module_playlist_process_event(Evas_Object * obj, enna_event event)
{
   API_ENTRY           return;

   enna_list_process_event(sd->menu, event);
}

EAPI void
enna_module_playlist_focus_set(Evas_Object * obj, unsigned int focus)
{

}

static void
_new_pl_cb(void *data, void *data2)
{
   E_Smart_Data       *sd;

   sd = (E_Smart_Data *) data;

   dbg("New PL\n");
}

static void
_play_pl_cb(void *data, void *data2)
{
   E_Smart_Data       *sd;
   Enna               *enna;

   sd = (E_Smart_Data *) data;

   enna = evas_data_attach_get(evas_object_evas_get(sd->edje));

}

static void
_save_pl_cb(void *data, void *data2)
{
   E_Smart_Data       *sd;

   sd = (E_Smart_Data *) data;

   dbg("Save PL\n");
}

static void
_open_pl_cb(void *data, void *data2)
{
   E_Smart_Data       *sd;

   sd = (E_Smart_Data *) data;

   dbg("Open PL\n");
}

static void
_pl_update(E_Smart_Data * sd)
{
   Enna               *enna;

   //Enna_Playlist *playlist;
   //Enna_Playlist_Element *element;
   Evas_Object        *icon;

   if (!sd)
      return;
   enna = evas_data_attach_get(evas_object_evas_get(sd->edje));
   if (!enna)
      return;

   enna_list_clear(sd->menu);
   icon = edje_object_add(evas_object_evas_get(sd->obj));
   edje_object_file_set(icon, enna_config_theme_get(), "icon_new");
   enna_list_append(sd->menu, icon, _("New Playlist"), 0, _new_pl_cb, NULL,
		    sd, NULL);
   icon = edje_object_add(evas_object_evas_get(sd->obj));
   edje_object_file_set(icon, enna_config_theme_get(), "icon_play");
   enna_list_append(sd->menu, icon, _("Play Playlist"), 0, _play_pl_cb, NULL,
		    sd, NULL);
   icon = edje_object_add(evas_object_evas_get(sd->obj));
   edje_object_file_set(icon, enna_config_theme_get(), "icon_save");
   enna_list_append(sd->menu, icon, _("Save Playlist"), 0, _save_pl_cb, NULL,
		    sd, NULL);
   icon = edje_object_add(evas_object_evas_get(sd->obj));
   edje_object_file_set(icon, enna_config_theme_get(), "icon_open");
   enna_list_append(sd->menu, icon, _("Open Playlist"), 0, _open_pl_cb, NULL,
		    sd, NULL);

   enna_list_append(sd->menu, NULL, "separator", 1, NULL, NULL, NULL, NULL);

   enna_list_append(sd->menu, NULL, _("No element in playlist"), 0, NULL,
		    NULL, NULL, NULL);

   enna_list_selected_set(sd->menu, 0);
   enna_util_switch_objects(sd->switcher, NULL, sd->menu);
}

static void
_e_smart_reconfigure(E_Smart_Data * sd)
{
   evas_object_move(sd->edje, sd->x, sd->y);
   evas_object_resize(sd->edje, sd->w, sd->h);
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

   sd->edje = edje_object_add(evas_object_evas_get(obj));
   edje_object_file_set(sd->edje, enna_config_theme_get(),
			"enna/module/playlist");

   if (!sd->edje)
      dbg("NOT EDJE\n");

   sd->switcher = edje_object_add(evas_object_evas_get(obj));
   edje_object_file_set(sd->switcher, enna_config_theme_get(), "enna/switcher");
   evas_object_smart_member_add(sd->switcher, obj);
   evas_object_show(sd->switcher);
   edje_object_part_swallow(sd->edje, "enna.switcher.swallow", sd->switcher);

   evas_object_smart_member_add(sd->edje, obj);
   evas_object_show(sd->edje);
   evas_object_propagate_events_set(obj, 0);

   sd->menu = enna_list_add(evas_object_evas_get(obj));
   _pl_update(sd);
   enna_list_selected_set(sd->menu, 0);
   evas_object_show(sd->menu);
}

static void
_e_smart_del(Evas_Object * obj)
{
   INTERNAL_ENTRY;
   evas_object_del(sd->edje);
   evas_object_del(sd->switcher);
   evas_object_del(sd->menu);
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
   evas_object_show(sd->edje);
   //_pl_update(sd);
}

static void
_e_smart_hide(Evas_Object * obj)
{
   INTERNAL_ENTRY;
   evas_object_hide(sd->edje);
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
