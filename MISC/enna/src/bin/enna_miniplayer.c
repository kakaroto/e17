/*
 * enna_miniplayer.c
 * Copyright (C) Nicolas Aguirre 2006,2007,2008 <aguirre.nicolas@gmail.com>
 *
 * enna_miniplayer.c is free software copyrighted by Nicolas Aguirre.
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
 * enna_miniplayer.c IS PROVIDED BY Nicolas Aguirre ``AS IS'' AND ANY EXPRESS
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
#include "enna_config.h"
#include "enna_miniplayer.h"
#include "enna_reflection.h"

#define SMART_NAME "enna_miniplayer"
#define API_ENTRY E_Smart_Data *sd; sd = evas_object_smart_data_get(obj); if ((!obj) || (!sd) || (evas_object_type_get(obj) && strcmp(evas_object_type_get(obj), SMART_NAME)))
#define INTERNAL_ENTRY E_Smart_Data *sd; sd = evas_object_smart_data_get(obj); if (!sd) return;

#define ENNA_MINIPLAYER_STATE_STOP 0
#define ENNA_MINIPLAYER_STATE_PAUSE 1
#define ENNA_MINIPLAYER_STATE_PLAY 2

typedef struct _E_Smart_Data E_Smart_Data;

struct _E_Smart_Data
{
   Evas_Coord          x, y, w, h;
   Evas_Object        *obj;
   Evas_Object        *edje;
   Evas_Object        *cover;
   char               *title;
   char               *album;
   char               *artist;
};

static void         _e_smart_init(void);
static void         _e_smart_add(Evas_Object * obj);
static void         _e_smart_del(Evas_Object * obj);
static void         _e_smart_move(Evas_Object * obj, Evas_Coord x,
				  Evas_Coord y);
static void         _e_smart_resize(Evas_Object * obj, Evas_Coord w,
				    Evas_Coord h);
static void         _e_smart_show(Evas_Object * obj);
static void         _e_smart_hide(Evas_Object * obj);
static void         _e_smart_color_set(Evas_Object * obj, int r, int g,
				       int b, int a);
static void         _e_smart_clip_set(Evas_Object * obj, Evas_Object * clip);
static void         _e_smart_clip_unset(Evas_Object * obj);

/* local subsystem globals */
static Evas_Smart  *_e_smart = NULL;

/* externally accessible functions */
EAPI Evas_Object   *
enna_miniplayer_add(Evas * evas)
{
   _e_smart_init();
   return evas_object_smart_add(evas, _e_smart);
}

EAPI void
enna_miniplayer_play(Evas_Object * obj)
{
   API_ENTRY           return;

   edje_object_signal_emit(sd->edje, "enna,state,play", "enna");
}

EAPI void
enna_miniplayer_stop(Evas_Object * obj)
{
   API_ENTRY           return;

   edje_object_part_text_set(sd->edje, "text.title", "");
   edje_object_part_text_set(sd->edje, "text.album", "");
   edje_object_part_text_set(sd->edje, "text.artist", "Not Playing");
   edje_object_signal_emit(sd->edje, "enna,state,stop", "enna");
}

EAPI void
enna_miniplayer_pause(Evas_Object * obj)
{
   API_ENTRY           return;

   edje_object_signal_emit(sd->edje, "enna,state,pause", "enna");
}

EAPI void
enna_miniplayer_infos_set(Evas_Object * obj, char *title, char *album,
			  char *artist, char *cover)
{
   Evas_Coord          w, h;

   API_ENTRY           return;

   dbg("%s %s %s\n", title, album, artist);

   if (title)
      sd->title = strdup(title);
   else
      sd->title = strdup(_("Unknown"));
   if (album)
      sd->album = strdup(album);
   else
      sd->album = strdup(_("Unknown"));
   if (artist)
      sd->artist = strdup(artist);
   else
      sd->artist = strdup(_("Unknown"));

   edje_object_part_text_set(sd->edje, "text.title", sd->title);
   edje_object_part_text_set(sd->edje, "text.artist", sd->artist);
   edje_object_part_text_set(sd->edje, "text.album", sd->album);

   if (cover)
     {
	edje_object_signal_emit(sd->edje, "enna,cover,show", "enna");
	edje_object_part_swallow(sd->edje, "enna.swallow.cover", sd->cover);
	edje_object_part_geometry_get(sd->edje, "enna.swallow.cover", NULL,
				      NULL, &w, &h);
	enna_image_file_set(sd->cover, cover, NULL);
	evas_object_resize(sd->cover, w, h);
	edje_object_part_swallow(sd->edje, "swallow.cover", sd->cover);
     }
   else
      edje_object_signal_emit(sd->edje, "enna,cover,hide", "enna");

   edje_object_signal_emit(sd->edje, "enna,miniplayer,update", "enna");
   enna_miniplayer_play(sd->obj);
}

EAPI void           enna_miniplayer_select(Evas_Object *obj)
{
   API_ENTRY           return;
   edje_object_signal_emit(sd->edje, "miniplayer,select", "enna");
}

EAPI void           enna_miniplayer_unselect(Evas_Object *obj)
{
   API_ENTRY           return;
   edje_object_signal_emit(sd->edje, "miniplayer,unselect", "enna");
}


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
   edje_object_file_set(sd->edje, enna_config_theme_get(), "widget/miniplayer");
   sd->cover = enna_image_add(evas_object_evas_get(obj));
   edje_object_part_swallow(sd->edje, "swallow.cover", sd->cover);


   enna_miniplayer_stop(sd->obj);
   evas_object_smart_member_add(sd->edje, obj);
   evas_object_propagate_events_set(obj, 0);
}

static void
_e_smart_del(Evas_Object * obj)
{
   E_Smart_Data       *sd;

   sd = evas_object_smart_data_get(obj);
   if (!sd)
      return;

   ENNA_FREE(sd->title);
   ENNA_FREE(sd->album);
   ENNA_FREE(sd->artist);
   evas_object_del(sd->cover);
   evas_object_del(sd->edje);
   free(sd);
}

static void
_e_smart_move(Evas_Object * obj, Evas_Coord x, Evas_Coord y)
{
   E_Smart_Data       *sd;

   sd = evas_object_smart_data_get(obj);
   if (!sd)
      return;
   if ((x == sd->x) && (y == sd->y))
      return;
   {
      Evas_Coord          dx, dy;
      Evas_Coord          ox, oy;

      dx = x - sd->x;
      dy = y - sd->y;
      if (sd->edje)
	{
	   evas_object_geometry_get(sd->edje, &ox, &oy, NULL, NULL);
	   evas_object_move(sd->edje, ox + dx, oy + dy);
	}
   }
   sd->x = x;
   sd->y = y;
}

static void
_e_smart_resize(Evas_Object * obj, Evas_Coord w, Evas_Coord h)
{
   E_Smart_Data       *sd;
   Evas_Coord          cw, ch;

   sd = evas_object_smart_data_get(obj);
   if (!sd)
      return;
   if ((w == sd->w) && (h == sd->h))
      return;
   sd->w = w;
   sd->h = h;
   if (sd->edje)
      evas_object_resize(sd->edje, w, h);

   if (sd->cover)
     {
	edje_object_part_geometry_get(sd->edje, "enna.swallow.cover", NULL,
				      NULL, &cw, &ch);
	evas_object_resize(sd->cover, cw, ch);
     }
   //_enna_miniplayer_smart_reconfigure(sd);
}

static void
_e_smart_show(Evas_Object * obj)
{
   E_Smart_Data       *sd;

   sd = evas_object_smart_data_get(obj);
   if (!sd)
      return;

   evas_object_show(sd->edje);

}

static void
_e_smart_hide(Evas_Object * obj)
{
   E_Smart_Data       *sd;

   sd = evas_object_smart_data_get(obj);
   if (!sd)
      return;
   evas_object_hide(sd->edje);
}

static void
_e_smart_color_set(Evas_Object * obj, int r, int g, int b, int a)
{
   E_Smart_Data       *sd;

   sd = evas_object_smart_data_get(obj);
   if (!sd)
      return;
   evas_object_color_set(sd->edje, r, g, b, a);
}

static void
_e_smart_clip_set(Evas_Object * obj, Evas_Object * clip)
{
   E_Smart_Data       *sd;

   sd = evas_object_smart_data_get(obj);
   if (!sd)
      return;
   evas_object_clip_set(sd->edje, clip);
}

static void
_e_smart_clip_unset(Evas_Object * obj)
{
   E_Smart_Data       *sd;

   sd = evas_object_smart_data_get(obj);
   if (!sd)
      return;
   evas_object_clip_unset(sd->edje);
}
