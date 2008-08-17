/*
 * enna_mediaplayer.c
 * Copyright (C) Nicolas Aguirre 2006,2007,2008 <aguirre.nicolas@gmail.com>
 *
 * enna_mediaplayer.c is free software copyrighted by Nicolas Aguirre.
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
 * enna_mediaplayer.c IS PROVIDED BY Nicolas Aguirre ``AS IS'' AND ANY EXPRESS
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

#include "enna_event.h"
#include "enna_volume_manager.h"
#include "enna_mediaplayer.h"
#include "enna_emusic.h"
#include "enna_emotion.h"
#include "enna_miniplayer.h"
#include "enna_config.h"
#include "enna_scanner.h"
#include "enna_event.h"
#include "enna_module.h"
#include "enna_mainmenu.h"

#define ENNA_MEDIAPLAYER_STATE_EMUSIC  0
#define ENNA_MEDIAPLAYER_STATE_EMOTION 1
#define ENNA_MEDIAPLAYER_STATE_MPLAYER 2
#define ENNA_MEDIAPLAYER_STATE_XINE    3
#define ENNA_MEDIAPLAYER_STATE_PICTURE 4
#define ENNA_MEDIAPLAYER_STATE_NONE    5
#define SMART_NAME "enna_mediaplayer"
typedef struct _E_Smart_Data E_Smart_Data;

struct _E_Smart_Data
{
   Evas_Coord          x, y, w, h;
   Evas_Object        *obj;
   Evas_Object        *edje;
   Evas_Object        *emusic;
   Evas_Object        *emotion;
   Evas_Object        *miniplayer;
   Enna               *enna;
   int                 has_focus;
   int                 state;
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
static void         _e_smart_color_set(Evas_Object * obj, int r,
				       int g, int b, int a);
static void         _e_smart_clip_set(Evas_Object * obj, Evas_Object * clip);
static void         _e_smart_clip_unset(Evas_Object * obj);

/* local subsystem globals */
static Evas_Smart  *_e_smart = NULL;

//static int first_played = 0;
/* externally accessible functions */
EAPI Evas_Object   *
enna_mediaplayer_add(Evas * evas)
{
   _e_smart_init();
   return evas_object_smart_add(evas, _e_smart);
}

EAPI Evas_Object   *
enna_mediaplayer_menu_get(Evas_Object * obj)
{

   E_Smart_Data       *sd;

   sd = evas_object_smart_data_get(obj);
   if (!sd)
      return NULL;

   return sd->miniplayer;

}
EAPI void
enna_mediaplayer_process_event(Evas_Object * obj, enna_event event)
{
   E_Smart_Data       *sd;

   sd = evas_object_smart_data_get(obj);
   if (!sd)
      return;

   switch (sd->state)
     {
     case ENNA_MEDIAPLAYER_STATE_NONE:
	break;
     case ENNA_MEDIAPLAYER_STATE_EMUSIC:
	enna_emusic_process_event(sd->emusic, event);
	break;
     case ENNA_MEDIAPLAYER_STATE_EMOTION:
	enna_emotion_process_event(sd->emotion, event);
	break;
     default:
	dbg("I'm a goat!\n");
	break;
     }
}

EAPI void
enna_mediaplayer_focus_set(Evas_Object * obj, unsigned int focus)
{
   E_Smart_Data       *sd;

   sd = evas_object_smart_data_get(obj);
   if (!sd)
      return;

   switch (sd->state)
     {
     case ENNA_MEDIAPLAYER_STATE_NONE:
	break;
     case ENNA_MEDIAPLAYER_STATE_EMUSIC:
	break;
     case ENNA_MEDIAPLAYER_STATE_EMOTION:
	enna_emotion_focus_set(sd->emotion, focus);
	break;
     default:
	dbg("I'm a goat!\n");
	break;
     }
}

EAPI int
enna_mediaplayer_has_focus_get(Evas_Object * obj)
{
   E_Smart_Data       *sd;

   sd = evas_object_smart_data_get(obj);
   if (!sd)
      return 0;

   return sd->has_focus;
}

EAPI int
enna_mediaplayer_play(Evas_Object * obj, char *filename, int type, Volume * v)
{
   char               *d;
   E_Smart_Data       *sd;
   Enna               *enna;

   sd = evas_object_smart_data_get(obj);
   enna = evas_data_attach_get(evas_object_evas_get(sd->edje));
   if (!sd || !enna || !filename)
      return 0;

   d = ecore_file_dir_get(filename);

   switch (sd->state)
     {
     case ENNA_MEDIAPLAYER_STATE_NONE:
	break;
     case ENNA_MEDIAPLAYER_STATE_EMUSIC:
	edje_object_part_unswallow(sd->edje, sd->emusic);
	evas_object_del(sd->emusic);
	sd->emusic = NULL;
	break;
     case ENNA_MEDIAPLAYER_STATE_EMOTION:
	edje_object_part_unswallow(sd->edje, sd->emotion);
	evas_object_del(sd->emotion);
	sd->emotion = NULL;
	break;
     default:
	dbg("I'm a goat\n");
	break;
     }

   switch (type)
     {
     case ENNA_MEDIA_TYPE_MUSIC_FILE:
	sd->emusic = enna_emusic_add(evas_object_evas_get(sd->edje));
	enna_emusic_load(sd->emusic, filename);
	enna_emusic_play(sd->emusic);
	edje_object_part_swallow(sd->edje, "enna.player.swallow", sd->emusic);
	sd->state = ENNA_MEDIAPLAYER_STATE_EMUSIC;
	enna_mainmenu_active_set(enna->mainmenu, 0);
	break;
     case ENNA_MEDIA_TYPE_MUSIC_CDDA:
	{
	   char               *device = NULL;
	   unsigned int        selected;
	   unsigned int        nb_tracks;

	   if (!strstr(filename, "cdda://"))
	      break;

	   device = filename + strlen("cdda://");
	   if (device[0] == '/')
	     {
		char               *tmp;

		if ((tmp = strchr(device, (const char)'#')))
		  {
		     sscanf(tmp + 1, "%d#%d", &nb_tracks, &selected);
		     tmp[0] = 0;
		  }
	     }
	   else
	     {
		device = strdup("/dev/cdrom");
		sscanf(filename, "cdda://%d#%d", &nb_tracks, &selected);
	     }

	   sd->emusic = enna_emusic_add(evas_object_evas_get(sd->edje));
	   enna_emusic_cdda_load(sd->emusic, selected, nb_tracks, device);
	   enna_emusic_play(sd->emusic);
	   edje_object_part_swallow(sd->edje, "enna.player.swallow",
				    sd->emusic);
	   sd->state = ENNA_MEDIAPLAYER_STATE_EMUSIC;
	   enna_mainmenu_active_set(enna->mainmenu, 0);
	   break;
	}
     case ENNA_MEDIA_TYPE_VIDEO_FILE:
	sd->emotion = enna_emotion_add(evas_object_evas_get(sd->edje));
	enna_emotion_load(sd->emotion, filename);
	enna_emotion_play(sd->emotion);
	edje_object_part_swallow(sd->edje, "enna.player.swallow", sd->emotion);
	sd->state = ENNA_MEDIAPLAYER_STATE_EMOTION;
	enna_mainmenu_active_set(enna->mainmenu, 0);

	break;
     case ENNA_MEDIA_TYPE_VIDEO_DVD:
	sd->emotion = enna_emotion_add(evas_object_evas_get(sd->edje));
	enna_emotion_load_dvd(sd->emotion, filename, v);
	enna_emotion_play(sd->emotion);
	edje_object_part_swallow(sd->edje, "enna.player.swallow", sd->emotion);
	sd->state = ENNA_MEDIAPLAYER_STATE_EMOTION;
	enna_mainmenu_active_set(enna->mainmenu, 0);

	break;
     default:
	dbg("I'm a goat\n");
	break;
     }
   return 1;

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
   sd->obj = obj;
   sd->enna = evas_data_attach_get(evas_object_evas_get(obj));
   sd->x = 0;
   sd->y = 0;
   sd->w = 0;
   sd->h = 0;
   sd->has_focus = 0;
   sd->state = ENNA_MEDIAPLAYER_STATE_NONE;
   sd->edje = edje_object_add(evas_object_evas_get(obj));
   edje_object_file_set(sd->edje, enna_config_theme_get(), "enna/mediaplayer");

   /* Create miniplayer */
   sd->miniplayer = enna_miniplayer_add(evas_object_evas_get(obj));
   evas_object_show(sd->miniplayer);
   edje_object_part_swallow(sd->enna->edje, "miniplayer.swallow",
			    sd->miniplayer);
   edje_object_signal_emit(sd->enna->edje, "miniplayer,show", "enna");
   sd->enna->miniplayer = sd->miniplayer;
   evas_object_show(sd->miniplayer);
   evas_object_propagate_events_set(obj, 1);
   evas_object_smart_data_set(obj, sd);
   evas_object_smart_member_add(sd->edje, obj);
   evas_object_smart_member_add(sd->emusic, obj);
   evas_object_smart_member_add(sd->emotion, obj);
   evas_object_smart_member_add(sd->miniplayer, obj);

}

static void
_e_smart_del(Evas_Object * obj)
{
   E_Smart_Data       *sd;

   sd = evas_object_smart_data_get(obj);
   if (!sd)
      return;
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

   sd = evas_object_smart_data_get(obj);
   if (!sd)
      return;
   if ((w == sd->w) && (h == sd->h))
      return;
   sd->w = w;
   sd->h = h;
   if (sd->edje)
      evas_object_resize(sd->edje, w, h);

}

static void
_e_smart_show(Evas_Object * obj)
{
   E_Smart_Data       *sd;
   int                *nothing;

   nothing = malloc(sizeof(int));
   *nothing = 0x55;

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
