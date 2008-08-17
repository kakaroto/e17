/*
 * enna_emusic.c
 * Copyright (C) Nicolas Aguirre 2006,2007,2008 <aguirre.nicolas@gmail.com>
 *
 * enna_emusic.c is free software copyrighted by Nicolas Aguirre.
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
 * enna_emusic.c IS PROVIDED BY Nicolas Aguirre ``AS IS'' AND ANY EXPRESS
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
#include "enna_emusic.h"
#include "enna_miniplayer.h"
#include "enna_config.h"
#include "enna_scanner.h"
#include "enna_util.h"

#define ENNA_EMUSIC_STATE_PLAYING  0
#define ENNA_EMUSIC_STATE_PAUSE    1
#define ENNA_EMUSIC_STATE_STOP     2

#define ENNA_EMUSIC_TYPE_AUDIO     0
#define ENNA_EMUSIC_TYPE_CDDA      1

#define SMART_NAME "enna_emusic"

typedef struct _E_Smart_Data E_Smart_Data;

struct _E_Smart_Data
{
   Evas_Coord          x, y, w, h;
   Evas_Object        *obj;
   Evas_Object        *edje;
   int                 has_focus;
   int                 state;
   Evas_Object        *cover;
   Ecore_Timer        *pos_timer;
   Evas_Object        *emotion;
   Ecore_List         *playlist;
   unsigned int        playlist_id;
   int                 music_type;
};

static int          _update_metadata(E_Smart_Data * sd);
static void         _eos_cb(void *data, Evas_Object * obj, void *event_info);
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
enna_emusic_add(Evas * evas)
{
   _e_smart_init();
   return evas_object_smart_add(evas, _e_smart);
}

EAPI void
enna_emusic_process_event(Evas_Object * obj, enna_event event)
{
   E_Smart_Data       *sd;

   sd = evas_object_smart_data_get(obj);
   if (!sd)
      return;

   switch (event)
     {
     case enna_event_play:
	enna_emusic_play(obj);
	break;
     case enna_event_pause:
	enna_emusic_pause(obj);
	break;
     case enna_event_stop:
	enna_emusic_stop(obj);
	break;
     case enna_event_next:
	enna_emusic_next(obj);
	break;
     case enna_event_prev:
	enna_emusic_prev(obj);
	break;
     case enna_event_fastforward:
	enna_emusic_fastforward(obj);
	break;
     case enna_event_rewind:
	enna_emusic_rewind(obj);
	break;
     default:
	break;
     }
}

EAPI int
enna_emusic_load(Evas_Object * obj, char *filename)
{
   char               *d = NULL;
   E_Smart_Data       *sd;
   Ecore_List         *files;
   char               *file;
   int                 i = 0;

   sd = evas_object_smart_data_get(obj);

   if (!sd || !filename || !ecore_file_exists(filename))
      return 0;

   enna_emusic_stop(sd->obj);

   d = ecore_file_dir_get(filename);
   
   files = ecore_file_ls(ecore_file_dir_get(filename));
   file = ecore_list_first_goto(files);
   while ((file = (char *)ecore_list_next(files)) != NULL)
     {
	if (enna_util_has_suffix(file, enna_config_extensions_get("music")))
	  {
	     char                tmp[4096];
	     sprintf(tmp, "file://%s/%s", d, (char *)file);
	     if (!strcmp(file, ecore_file_file_get(filename)))
		sd->playlist_id = i;
	     i++;
	     ecore_list_append(sd->playlist, strdup(tmp));
	  }
     }
   
   ecore_list_index_goto(sd->playlist, sd->playlist_id);
   sd->music_type = ENNA_EMUSIC_TYPE_AUDIO;
   return 1;
}

EAPI int
enna_emusic_cdda_load(Evas_Object * obj, unsigned int selected,
		      unsigned int nb_tracks, char *device)
{
   E_Smart_Data       *sd;
   int                 i = 0;

   sd = evas_object_smart_data_get(obj);
   if (!sd || !device)
      return 0;

   //dbg("selecte : %d, nb_tracks : %d, device : %s\n",

   for (i = 1; i <= nb_tracks; i++)
     {
	char                tmp[4096];

	sprintf(tmp, "cdda://%s#%d", device, i);
	ecore_list_append(sd->playlist, strdup(tmp));
     }
   sd->playlist_id = selected - 1;
   ecore_list_index_goto(sd->playlist, selected - 1);
   sd->music_type = ENNA_EMUSIC_TYPE_CDDA;
   return 1;
}

EAPI int
enna_emusic_play(Evas_Object * obj)
{

   E_Smart_Data       *sd;
   Enna               *enna;

   sd = evas_object_smart_data_get(obj);

   if (!sd)
      return 0;

   enna = evas_data_attach_get(evas_object_evas_get(sd->edje));
   dbg("PLAY\n");
   

   if (sd->state == ENNA_EMUSIC_STATE_STOP)
     {
	char *filename;
	
	sd->state = ENNA_EMUSIC_STATE_PLAYING;
	edje_object_signal_emit(sd->edje, "enna,state,play", "enna");
	edje_object_signal_emit(sd->edje, "enna,state,play", "enna");
	filename = ecore_list_index_goto(sd->playlist, sd->playlist_id);
	dbg("filename : %s\n", filename);
	
	emotion_object_file_set(sd->emotion, filename);
	emotion_object_play_set(sd->emotion, 1);
	enna_miniplayer_play(enna->miniplayer);
	_update_metadata(sd);
	return 1;

     }
   else if (sd->state == ENNA_EMUSIC_STATE_PAUSE)
     {
	sd->state = ENNA_EMUSIC_STATE_PLAYING;
	edje_object_signal_emit(sd->edje, "enna,state,play", "enna");
	emotion_object_play_set(sd->emotion, 1);
	enna_miniplayer_play(enna->miniplayer);
	_update_metadata(sd);
	return 1;
     }
   else
     {
	sd->state = ENNA_EMUSIC_STATE_PAUSE;
	edje_object_signal_emit(sd->edje, "enna,state,pause", "enna");
	emotion_object_play_set(sd->emotion, 0);
	enna_miniplayer_pause(enna->miniplayer);
	return 1;
     }
   return 1;

}

EAPI int
enna_emusic_pause(Evas_Object * obj)
{
   E_Smart_Data       *sd;
   Enna               *enna;

   sd = evas_object_smart_data_get(obj);

   if (!sd)
      return 0;

   enna = evas_data_attach_get(evas_object_evas_get(sd->edje));
   if (sd->state == ENNA_EMUSIC_STATE_PLAYING)
     {
	sd->state = ENNA_EMUSIC_STATE_PAUSE;
	edje_object_signal_emit(sd->edje, "enna,state,pause", "enna");
	emotion_object_play_set(sd->emotion, 0);
	enna_miniplayer_pause(enna->miniplayer);
	return 1;
     }
   else if (sd->state == ENNA_EMUSIC_STATE_PAUSE
	    || sd->state == ENNA_EMUSIC_STATE_STOP)
     {
	sd->state = ENNA_EMUSIC_STATE_PLAYING;
	edje_object_signal_emit(sd->edje, "enna,state,play", "enna");
	emotion_object_play_set(sd->emotion, 1);
	enna_miniplayer_play(enna->miniplayer);
     }

   return 1;
}

EAPI int
enna_emusic_stop(Evas_Object * obj)
{
   E_Smart_Data       *sd;
   Enna               *enna;

   sd = evas_object_smart_data_get(obj);

   if (!sd)
      return 0;

   enna = evas_data_attach_get(evas_object_evas_get(sd->edje));

   if (sd->state == ENNA_EMUSIC_STATE_PAUSE
       || sd->state == ENNA_EMUSIC_STATE_PLAYING)
     {
	sd->state = ENNA_EMUSIC_STATE_STOP;
	edje_object_signal_emit(sd->edje, "enna,state,stop", "enna");
	emotion_object_play_set(sd->emotion, 0);
	emotion_object_position_set(sd->emotion, 0.0);
	enna_miniplayer_stop(enna->miniplayer);
	return 1;
     }

   return 1;
}

EAPI int
enna_emusic_next(Evas_Object * obj)
{
   E_Smart_Data       *sd;

   sd = evas_object_smart_data_get(obj);

   if (!sd)
      return 0;

   sd->playlist_id++;
   if (ecore_list_index_goto(sd->playlist, sd->playlist_id))
     {
	//enna_emusic_stop(sd->obj);
	sd->state = ENNA_EMUSIC_STATE_STOP;
	enna_emusic_play(sd->obj);
	edje_object_signal_emit(sd->edje, "enna,state,next", "enna");
	return 1;
     }
   else
     {
	sd->playlist_id--;
	return 0;
     }
   return 1;
}

EAPI int
enna_emusic_prev(Evas_Object * obj)
{
   E_Smart_Data       *sd;

   sd = evas_object_smart_data_get(obj);

   if (!sd)
      return 0;

   sd->playlist_id--;
   if (ecore_list_index_goto(sd->playlist, sd->playlist_id))
     {
	//enna_emusic_stop(sd->obj);
	sd->state = ENNA_EMUSIC_STATE_STOP;
	enna_emusic_play(sd->obj);
	edje_object_signal_emit(sd->edje, "enna,state,prev", "enna");
	return 1;
     }
   else
     {
	sd->playlist_id++;
	return 0;
     }
   return 1;

}

EAPI int
enna_emusic_fastforward(Evas_Object * obj)
{
   E_Smart_Data       *sd;
   double              pos = 0.0, len = 0.0;

   sd = evas_object_smart_data_get(obj);
   if (!sd)
      return 0;

   pos = emotion_object_position_get(sd->emotion);
   pos += 1.0;
   len = emotion_object_play_length_get(sd->emotion);
   if (pos <= len)
     {
	emotion_object_position_set(sd->emotion, pos);
	return 1;
     }
   return 0;
}

EAPI int
enna_emusic_rewind(Evas_Object * obj)
{
   E_Smart_Data       *sd;
   double              pos = 0.0;

   sd = evas_object_smart_data_get(obj);

   if (!sd)
      return 0;

   pos = emotion_object_position_get(sd->emotion);
   pos -= 1.0;
   if (pos >= 0)
     {
	emotion_object_position_set(sd->emotion, pos);
	return 1;
     }
   return 0;
}

static int
_update_metadata(E_Smart_Data * sd)
{
   Enna_Metadata      *metadata = NULL;
   Evas_List          *l, *metadatas;
   char               *filename;
   Evas_Coord          w, h;
   Enna               *enna;

   filename = (char *)ecore_list_index_goto(sd->playlist, sd->playlist_id);
   if (!filename)
      return 0;

   if (ecore_str_has_prefix(filename, "file://"))
      filename = filename + 7;

   enna = evas_data_attach_get(evas_object_evas_get(sd->edje));
   dbg("type : %d\n", sd->music_type);
   if (sd->music_type == ENNA_EMUSIC_TYPE_AUDIO)
     metadata = enna_scanner_audio_metadata_get(filename);
   else if (sd->music_type == ENNA_EMUSIC_TYPE_CDDA)
     {
	/*metadatas =
	  enna_db_cdda_metadata_get(enna->db, "/dev/cdrom", NULL, NULL);*/
	for (l = metadatas; l; l = l->next)
	  {
	     metadata = l->data;
	     if (metadata->track_nb == sd->playlist_id + 1)
	       {
		  dbg("%s\n", metadata->title);
		  break;
	       }
	     else
		metadata = NULL;
	  }
     }
   if (metadata)
     {
   	char  tmp[4096];
	char *cover;
	
   	edje_object_part_text_set(sd->edje, "enna.text.album", metadata->album);
   	edje_object_part_text_set(sd->edje, "enna.text.title", metadata->title);
   	edje_object_part_text_set(sd->edje, "enna.text.artist",
   				  metadata->artist);

   	sprintf(tmp, "%d / %02d", sd->playlist_id + 1,
   		ecore_list_count(sd->playlist));

   	edje_object_part_text_set(sd->edje, "enna.text.current", tmp);

   	edje_object_signal_emit(sd->edje, "enna,cover,set", "enna");
   	edje_object_part_swallow(sd->edje, "enna.swallow.cover", sd->cover);
   	edje_object_part_geometry_get(sd->edje, "enna.swallow.cover", NULL,
   				      NULL, &w, &h);
	cover = enna_scanner_cover_get(metadata->album, metadata->artist);
   	if (cover)
	  evas_object_image_file_set(sd->cover, cover, NULL);
	
   	evas_object_resize(sd->cover, w, h);
   	evas_object_image_fill_set(sd->cover, 0, 0, w, h);
   	enna_miniplayer_infos_set(enna->miniplayer, metadata->title,
   				  metadata->album, metadata->artist,
   				  cover);
	ENNA_FREE(cover);
     }
   else
     {
   	edje_object_part_text_set(sd->edje, "enna.text.album", "");
   	edje_object_part_text_set(sd->edje, "enna.text.title", "No Metadata");
   	edje_object_part_text_set(sd->edje, "enna.text.artist", "");
   	edje_object_signal_emit(sd->edje, "enna,cover,unset", "enna");
   	enna = evas_data_attach_get(evas_object_evas_get(sd->edje));
   	enna_miniplayer_infos_set(enna->miniplayer, "", "", "No Metadata",
   				  NULL);
     }
   return 1;
}

static void
_eos_cb(void *data, Evas_Object * obj, void *event_info)
{
   Enna               *enna;
   E_Smart_Data       *sd;

   sd = data;

   if (!sd)
      return;

   enna = evas_data_attach_get(evas_object_evas_get(sd->edje));

   if (!enna_emusic_next(sd->obj))
      enna_miniplayer_stop(enna->miniplayer);

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

   if (!sd)
      return 0;

   pos = emotion_object_position_get(sd->emotion);
   len = emotion_object_play_length_get(sd->emotion);

   lh = len / 3600000;
   lm = len / 60 - (lh * 60);
   ls = len - (lm * 60);
   ph = pos / 3600;
   pm = pos / 60 - (ph * 60);
   ps = pos - (pm * 60);
   snprintf(buf, sizeof(buf), "%li:%02li:%02li", ph, pm, ps);
   snprintf(buf2, sizeof(buf2), "%li:%02li:%02li", lh, lm, ls);
   if (len)
      fraction = pos / len;
   else
      fraction = 0.0;
   //  dbg("[%s / %s]\n", buf2, buf);
   edje_object_part_text_set(sd->edje, "enna.text.length", buf2);
   edje_object_part_text_set(sd->edje, "enna.text.position", buf);
   edje_object_part_drag_value_set(sd->edje, "enna.drag.progress", fraction,
				   0.0);

   return 1;
}

static void
_play_event_mouse_up(void *data, Evas * e, Evas_Object * obj, void *event_info)
{
   Evas_Event_Mouse_Down *ev;
   E_Smart_Data       *sd;

   sd = data;
   ev = event_info;
   if (sd && (ev->button == 1) && (ev->flags == EVAS_BUTTON_NONE))
      enna_emusic_pause(sd->obj);
}

static void
_pause_event_mouse_up(void *data, Evas * e, Evas_Object * obj, void *event_info)
{
   Evas_Event_Mouse_Down *ev;
   E_Smart_Data       *sd;

   sd = data;
   ev = event_info;

   dbg("PAUSE MOUSE UP\n");

   dbg("PAUSE %d %d\n", ev->button, ev->flags);
   if (sd && (ev->button == 1) && (ev->flags == EVAS_BUTTON_NONE))
     {
	enna_emusic_play(sd->obj);
     }
}

static void
_stop_event_mouse_up(void *data, Evas * e, Evas_Object * obj, void *event_info)
{
   Evas_Event_Mouse_Down *ev;
   E_Smart_Data       *sd;

   sd = data;
   ev = event_info;

   if (sd && (ev->button == 1) && (ev->flags == EVAS_BUTTON_NONE))
     {
	enna_emusic_play(sd->obj);
     }

}

static void
_prev_event_mouse_up(void *data, Evas * e, Evas_Object * obj, void *event_info)
{
   Evas_Event_Mouse_Down *ev;
   E_Smart_Data       *sd;

   sd = data;
   ev = event_info;

   if (sd && (ev->button == 1) && (ev->flags == EVAS_BUTTON_NONE))
     {
	enna_emusic_prev(sd->obj);
     }

}

static void
_next_event_mouse_up(void *data, Evas * e, Evas_Object * obj, void *event_info)
{
   Evas_Event_Mouse_Down *ev;
   E_Smart_Data       *sd;

   sd = data;
   ev = event_info;

   if (sd && (ev->button == 1) && (ev->flags == EVAS_BUTTON_NONE))
     {
	enna_emusic_next(sd->obj);
     }

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
_free_playlist(void *data)
{
   free(data);
}

static void
_progress_bar_drag_cb(void *data, Evas_Object * edje_object,
		      const char *emission, const char *source)
{

   double              progress_rate, len;
   E_Smart_Data       *sd;

   sd = data;

   if (strcmp(emission, "drag,stop") == 0)
     {
	edje_object_part_drag_value_get(sd->edje, "enna.drag.progress",
					&progress_rate, NULL);
	dbg("progress : %3.3f\n", progress_rate);
	len = emotion_object_play_length_get(sd->emotion);
	emotion_object_position_set(sd->emotion, len * progress_rate);
     }
}

static void
_e_smart_add(Evas_Object * obj)
{
   E_Smart_Data       *sd;
   Evas_Object        *o_play, *o_stop, *o_pause, *o_next, *o_prev;

   sd = calloc(1, sizeof(E_Smart_Data));
   if (!sd)
      return;
   sd->obj = obj;
   sd->x = 0;
   sd->y = 0;
   sd->w = 0;
   sd->h = 0;
   sd->state = ENNA_EMUSIC_STATE_STOP;
   sd->edje = edje_object_add(evas_object_evas_get(obj));
   edje_object_file_set(sd->edje, enna_config_theme_get(), "enna/emusicplayer");
   sd->cover = evas_object_image_add(evas_object_evas_get(obj));
   edje_object_part_swallow(sd->edje, "enna.swallow.cover", sd->cover);
   sd->emotion = emotion_object_add(evas_object_evas_get(sd->edje));
   edje_object_signal_callback_add(sd->edje, "drag*", "enna.drag.progress",
				   _progress_bar_drag_cb, sd);
   /* FIXME !! Music engine should come from config ! */
   emotion_object_init(sd->emotion, "gstreamer");
   //emotion_object_init (sd->emotion, "emotion_decoder_xine.so");
   emotion_object_video_mute_set(sd->emotion, 0);
   //evas_object_hide(sd->emotion);
   //evas_object_resize(sd->emotion, 0, 0);
   evas_object_smart_callback_add(sd->emotion, "decode_stop", _eos_cb, sd);
   sd->pos_timer = ecore_timer_add(0.5, _position_timer, sd);

   sd->playlist = ecore_list_new();
   ecore_list_init(sd->playlist);
   ecore_list_free_cb_set(sd->playlist, _free_playlist);

   /* Add Play/Next/Prev Clic signal */
   o_play = edje_object_part_object_get(sd->edje, "enna.btn.play");
   evas_object_event_callback_add(o_play, EVAS_CALLBACK_MOUSE_DOWN,
				  _play_event_mouse_up, sd);
   o_pause = edje_object_part_object_get(sd->edje, "enna.btn.pause");
   evas_object_event_callback_add(o_pause, EVAS_CALLBACK_MOUSE_DOWN,
				  _pause_event_mouse_up, sd);
   o_stop = edje_object_part_object_get(sd->edje, "enna.btn.stop");
   evas_object_event_callback_add(o_stop, EVAS_CALLBACK_MOUSE_DOWN,
				  _stop_event_mouse_up, sd);
   o_next = edje_object_part_object_get(sd->edje, "enna.btn.next");
   evas_object_event_callback_add(o_next, EVAS_CALLBACK_MOUSE_DOWN,
				  _next_event_mouse_up, sd);
   o_prev = edje_object_part_object_get(sd->edje, "enna.btn.prev");
   evas_object_event_callback_add(o_prev, EVAS_CALLBACK_MOUSE_DOWN,
				  _prev_event_mouse_up, sd);

   evas_object_smart_data_set(obj, sd);
   evas_object_smart_member_add(sd->edje, obj);
}

static void
_e_smart_del(Evas_Object * obj)
{
   E_Smart_Data       *sd;

   sd = evas_object_smart_data_get(obj);
   if (!sd)
      return;
   ecore_timer_del(sd->pos_timer);
   evas_object_del(sd->edje);
   evas_object_del(sd->cover);
   evas_object_del(sd->emotion);
   ecore_list_destroy(sd->playlist);
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
	evas_object_image_fill_set(sd->cover, 0, 0, cw, ch);
     }
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

   int                *nothing;

   nothing = malloc(sizeof(int));
   *nothing = 0x55;

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
