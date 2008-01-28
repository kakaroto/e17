/*
 * enna_emotion.c
 * Copyright (C) Nicolas Aguirre 2006,2007,2008 <aguirre.nicolas@gmail.com>
 *
 * enna_emotion.c is free software copyrighted by Nicolas Aguirre.
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
 * enna_emotion.c IS PROVIDED BY Nicolas Aguirre ``AS IS'' AND ANY EXPRESS
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
#include <Emotion.h>

#include "enna_event.h"
#include "enna_volume_manager.h"
#include "enna_emotion.h"
#include "enna_miniplayer.h"
#include "enna_config.h"
#include "enna_scanner.h"
#include "enna_event.h"
#include "enna_util.h"

#define ENNA_EMOTION_STATE_PLAYING  0
#define ENNA_EMOTION_STATE_PAUSE    1
#define ENNA_EMOTION_STATE_STOP     2

#define SMART_NAME "enna_emotion"

typedef struct _E_Smart_Data E_Smart_Data;

struct _E_Smart_Data
{
   Evas_Coord          x, y, w, h;
   Evas_Object        *obj;
   Evas_Object        *edje;
   int                 has_focus;
   int                 state;
   Evas_Object        *emotion;
   Ecore_Timer        *pos_timer;
   unsigned int        fullscreen;
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
enna_emotion_add(Evas * evas)
{
   _e_smart_init();
   return evas_object_smart_add(evas, _e_smart);
}

EAPI void
enna_emotion_process_event(Evas_Object * obj, enna_event event)
{
   E_Smart_Data       *sd;

   sd = evas_object_smart_data_get(obj);
   if (!sd)
      return;

   switch (event)
     {
     case enna_event_enter:
	if (!sd->fullscreen)
	   enna_emotion_fullscreen_toggle(obj);
	else
	   emotion_object_event_simple_send(sd->emotion, EMOTION_EVENT_SELECT);
	break;
     case enna_event_escape:
	if (sd->fullscreen)
	   enna_emotion_fullscreen_toggle(obj);
	break;
     case enna_event_play:
	enna_emotion_play(obj);
	break;
     case enna_event_pause:
	enna_emotion_pause(obj);
	break;
     case enna_event_stop:
	enna_emotion_stop(obj);
	break;
     case enna_event_next:
	enna_emotion_next(obj);
	break;
     case enna_event_prev:
	enna_emotion_prev(obj);
	break;
     case enna_event_left:
	emotion_object_event_simple_send(sd->emotion, EMOTION_EVENT_LEFT);
	break;
     case enna_event_right:
	emotion_object_event_simple_send(sd->emotion, EMOTION_EVENT_RIGHT);
	break;
     case enna_event_down:
	emotion_object_event_simple_send(sd->emotion, EMOTION_EVENT_DOWN);
	break;
     case enna_event_up:
	emotion_object_event_simple_send(sd->emotion, EMOTION_EVENT_UP);
	break;
     case enna_event_rewind:
	enna_emotion_rewind(obj);
	break;
     case enna_event_fastforward:
	enna_emotion_fastforward(obj);
	break;
     default:
	break;
     }
}

EAPI void
enna_emotion_focus_set(Evas_Object * obj, unsigned int focus)
{
   Enna               *enna;
   E_Smart_Data       *sd;

   sd = evas_object_smart_data_get(obj);
   if (!sd)
      return;
   enna = evas_data_attach_get(evas_object_evas_get(sd->edje));
   if (focus)
     {
	emotion_object_video_mute_set(sd->emotion, 0);
	edje_object_signal_emit(enna->edje, "video,show", "enna");
     }
   else
     {
	emotion_object_video_mute_set(sd->emotion, 1);
	edje_object_signal_emit(enna->edje, "video,hide", "enna");
     }

}

EAPI int
enna_emotion_load_dvd(Evas_Object * obj, char *filename, Volume * v)
{
   E_Smart_Data       *sd;
   Evas_Coord          w, h, iw, ih;
   double              ratio = 0.0;
   Enna               *enna;

   sd = evas_object_smart_data_get(obj);
   if (!sd || !filename)
      return 0;
   edje_object_signal_emit(sd->edje, "enna,scanning,start", "enna");

   enna = evas_data_attach_get(evas_object_evas_get(sd->edje));
   emotion_object_file_set(sd->emotion, filename);
   emotion_object_play_set(sd->emotion, 1);
   emotion_object_smooth_scale_set(sd->emotion, 1);
   evas_object_move(sd->emotion, 0, 0);
   evas_object_resize(sd->emotion, 320, 240);
   evas_object_show(sd->emotion);

   emotion_object_size_get(obj, &iw, &ih);
   ratio = emotion_object_ratio_get(obj);
   if (ratio > 0.0)
      iw = (ih * ratio) + 0.5;
   edje_object_part_text_set(sd->edje, "enna.text.title", v->label);
   edje_extern_object_min_size_set(sd->emotion, iw, ih);
   edje_object_part_swallow(enna->edje, "video.swallow", sd->emotion);
   edje_object_size_min_calc(sd->edje, &w, &h);
   evas_object_resize(sd->edje, w, h);
   edje_extern_object_min_size_set(sd->emotion, 0, 0);
   edje_object_part_swallow(enna->edje, "video.swallow", sd->emotion);

   return 1;
}

EAPI int
enna_emotion_load(Evas_Object * obj, char *filename)
{
   E_Smart_Data       *sd;
   Evas_Coord          w, h, iw, ih;
   double              ratio = 0.0;
   Enna               *enna;
   char                filenamebuf[4096];
   char               *dotpos;

   sd = evas_object_smart_data_get(obj);

   if (!sd || !filename || !ecore_file_exists(filename))
      return 0;

   enna = evas_data_attach_get(evas_object_evas_get(sd->edje));

   emotion_object_file_set(sd->emotion, filename);
   emotion_object_play_set(sd->emotion, 1);

   emotion_object_smooth_scale_set(sd->emotion, 0);
   evas_object_move(sd->emotion, 0, 0);
   evas_object_resize(sd->emotion, 320, 240);
   evas_object_show(sd->emotion);

   /* Copy path into a buffer, get basename, remove extension and update edje */
   snprintf(filenamebuf, sizeof(filenamebuf), "%s",
	    ecore_file_file_get(filename));
   dotpos = strrchr(filenamebuf, '.');
   if (dotpos)
      *dotpos = '\0';
   edje_object_part_text_set(sd->edje, "enna.text.title", filenamebuf);

   emotion_object_size_get(obj, &iw, &ih);
   ratio = emotion_object_ratio_get(obj);
   if (ratio > 0.0)
      iw = (ih * ratio) + 0.5;
   edje_extern_object_min_size_set(sd->emotion, iw, ih);
   edje_object_part_swallow(enna->edje, "video.swallow", sd->emotion);
   edje_object_size_min_calc(sd->edje, &w, &h);
   evas_object_resize(sd->edje, w, h);
   edje_extern_object_min_size_set(sd->emotion, 0, 0);
   edje_object_part_swallow(enna->edje, "video.swallow", sd->emotion);

   return 1;
}

EAPI void
enna_emotion_fullscreen_toggle(Evas_Object * obj)
{
   E_Smart_Data       *sd;
   Enna               *enna;

   sd = evas_object_smart_data_get(obj);

   if (!sd)
      return;

   enna = evas_data_attach_get(evas_object_evas_get(sd->edje));
   sd->fullscreen = ~sd->fullscreen;
   if (!sd->fullscreen)
     {
	edje_object_signal_emit(enna->edje, "fullscreen,off",
				"enna");
	edje_object_signal_emit(sd->edje, "enna,state,infos,show", "enna");
	enna->fs_obj = 0;
     }
   else
     {
	edje_object_signal_emit(enna->edje, "fullscreen,on", "enna");
	edje_object_signal_emit(sd->edje, "enna,state,infos,hide", "enna");
	enna->fs_obj = 1;
     }

}

EAPI int
enna_emotion_play(Evas_Object * obj)
{

   E_Smart_Data       *sd;

   sd = evas_object_smart_data_get(obj);

   if (!sd)
      return 0;

   if (sd->state == ENNA_EMOTION_STATE_STOP
       || sd->state == ENNA_EMOTION_STATE_PAUSE)
     {
	sd->state = ENNA_EMOTION_STATE_PLAYING;
	edje_object_signal_emit(sd->edje, "enna,state,play", "enna");
	edje_object_signal_emit(sd->edje, "enna,state,infos,show", "enna");
	emotion_object_play_set(sd->emotion, 1);
     }
   else
     {
	sd->state = ENNA_EMOTION_STATE_PAUSE;
	edje_object_signal_emit(sd->edje, "enna,state,pause", "enna");
	emotion_object_play_set(sd->emotion, 0);
     }
   return 1;

}

EAPI int
enna_emotion_pause(Evas_Object * obj)
{
   E_Smart_Data       *sd;

   sd = evas_object_smart_data_get(obj);

   if (!sd)
      return 0;
   if (sd->state == ENNA_EMOTION_STATE_PLAYING)
     {
	sd->state = ENNA_EMOTION_STATE_PAUSE;
	edje_object_signal_emit(sd->edje, "enna,state,pause", "enna");
	emotion_object_play_set(sd->emotion, 0);
     }
   else if (sd->state == ENNA_EMOTION_STATE_PAUSE
	    || sd->state == ENNA_EMOTION_STATE_STOP)
     {
	sd->state = ENNA_EMOTION_STATE_PLAYING;
	edje_object_signal_emit(sd->edje, "enna,state,play", "enna");
	emotion_object_play_set(sd->emotion, 1);
     }

   return 1;
}

EAPI int
enna_emotion_stop(Evas_Object * obj)
{
   E_Smart_Data       *sd;

   sd = evas_object_smart_data_get(obj);

   if (!sd)
      return 0;

   if (sd->state == ENNA_EMOTION_STATE_PAUSE
       || sd->state == ENNA_EMOTION_STATE_PLAYING)
     {
	edje_object_signal_emit(sd->edje, "enna,state,stop", "enna");
	emotion_object_position_set(sd->emotion, 0.0);
	emotion_object_play_set(sd->emotion, 1);
     }

   return 1;
}

EAPI int
enna_emotion_next(Evas_Object * obj)
{
   E_Smart_Data       *sd;

   sd = evas_object_smart_data_get(obj);

   if (!sd)
      return 0;
   edje_object_signal_emit(sd->edje, "enna,state,next", "enna");
   emotion_object_position_set(sd->emotion, 0.0);
   emotion_object_play_set(sd->emotion, 1);
   return 1;
}

EAPI int
enna_emotion_prev(Evas_Object * obj)
{
   E_Smart_Data       *sd;

   sd = evas_object_smart_data_get(obj);

   if (!sd)
      return 0;

   return 1;
}

EAPI int
enna_emotion_rewind(Evas_Object * obj)
{
   E_Smart_Data       *sd;
   double              len;

   sd = evas_object_smart_data_get(obj);

   if (!sd)
      return 0;

   len = emotion_object_position_get(sd->emotion);

   len -= 10;
   if (len < 0)
      len = 0.0;

   emotion_object_position_set(sd->emotion, len);
   return 1;
}

EAPI int
enna_emotion_fastforward(Evas_Object * obj)
{
   E_Smart_Data       *sd;
   double              len;
   double              tot_len;

   sd = evas_object_smart_data_get(obj);

   if (!sd)
      return 0;

   len = emotion_object_position_get(sd->emotion);

   len += 100;
   tot_len = emotion_object_play_length_get(sd->emotion);

   if (len > tot_len)
      len = tot_len;

   emotion_object_position_set(sd->emotion, len);
   return 1;
}

static void
_frame_resize_change_cb(void *data, Evas_Object * obj, void *event_info)
{

   Evas_Coord          w, h;
   int                 iw, ih;
   double              ratio;
   Enna               *enna;
   E_Smart_Data       *sd;

   sd = data;

   enna = evas_data_attach_get(evas_object_evas_get(sd->edje));

   emotion_object_size_get(sd->emotion, &iw, &ih);
   if ((iw == 0) || (ih == 0))
      return;
   ratio = emotion_object_ratio_get(sd->emotion);
   if (ratio > 0.0)
      iw = (ih * ratio) + 0.5;
   else
      ratio = (double)iw / (double)ih;
   w = 10240 * ratio;
   h = 10240;

   edje_extern_object_aspect_set(sd->emotion, EDJE_ASPECT_CONTROL_BOTH, w, h);
   edje_object_part_swallow(enna->edje, "video.swallow", sd->emotion);
   edje_object_signal_emit(sd->edje, "enna,scanning,stop", "enna");
   edje_object_signal_emit(enna->edje, "video,show", "enna");

}

static int
_position_timer(void *data)
{
   double              pos, len;
   double              fraction;
   long                ph, pm, ps, lh, lm, ls;
   char                buf[256];
   char                buf2[256];
   E_Smart_Data       *sd;

   sd = data;

   pos = emotion_object_position_get(sd->emotion);
   len = emotion_object_play_length_get(sd->emotion);
   lh = len / 3600;
   lm = len / 60 - (lh * 60);
   ls = len - (lh * 3600) - (lm * 60);
   ph = pos / 3600;
   pm = pos / 60 - (ph * 60);
   ps = pos - (ph * 3600) - (pm * 60);
   snprintf(buf, sizeof(buf), "%li:%02li:%02li", ph, pm, ps);
   snprintf(buf2, sizeof(buf2), "%li:%02li:%02li", lh, lm, ls);
   if (len)
      fraction = pos / len;
   else
      fraction = 0.0;

   edje_object_part_text_set(sd->edje, "enna.text.length", buf2);
   edje_object_part_text_set(sd->edje, "enna.text.position", buf);
   edje_object_part_drag_value_set(sd->edje, "enna.drag.progress", fraction,
				   0.0);

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
   sd->x = 0;
   sd->y = 0;
   sd->w = 0;
   sd->h = 0;
   sd->fullscreen = 0;
   sd->state = ENNA_EMOTION_STATE_STOP;
   sd->edje = edje_object_add(evas_object_evas_get(obj));
   edje_object_file_set(sd->edje, enna_config_theme_get(),
			"enna/emotionplayer");

   sd->emotion = emotion_object_add(evas_object_evas_get(sd->edje));
   emotion_object_init(sd->emotion, "xine");

   sd->pos_timer = ecore_timer_add(0.5, _position_timer, sd);
   evas_object_smart_callback_add(sd->emotion, "frame_resize",
				  _frame_resize_change_cb, sd);
   evas_object_smart_data_set(obj, sd);
   evas_object_smart_member_add(sd->edje, obj);
   evas_object_smart_member_add(sd->emotion, obj);
}

static void
_e_smart_del(Evas_Object * obj)
{
   E_Smart_Data       *sd;

   sd = evas_object_smart_data_get(obj);
   if (!sd)
      return;
   evas_object_del(sd->edje);
   evas_object_del(sd->emotion);
   ecore_timer_del(sd->pos_timer);
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
