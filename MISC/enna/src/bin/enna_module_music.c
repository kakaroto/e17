
/*
 * enna_module_music.c
 * Copyright (C) Nicolas Aguirre 2006,2007,2008 <aguirre.nicolas@gmail.com>
 *
 * enna_module_music.c is free software copyrighted by Nicolas Aguirre.
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
 * enna_module_music.c IS PROVIDED BY Nicolas Aguirre ``AS IS'' AND ANY EXPRESS
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
#include "enna_fm.h"
#include "enna_fm_db.h"
#include "enna_mediaplayer.h"
#include "enna_config.h"
#include "enna_volume_manager.h"
#include "enna_cdda.h"
#include "enna_scanner.h"
#include "enna_util.h"
#include "enna_popup.h"
#include "enna_reflection.h"

#define SMART_NAME "enna_module_music"
#define API_ENTRY E_Smart_Data *sd; sd = evas_object_smart_data_get(obj); if ((!obj) || (!sd) || (evas_object_type_get(obj) && strcmp(evas_object_type_get(obj), SMART_NAME)))
#define INTERNAL_ENTRY E_Smart_Data *sd; sd = evas_object_smart_data_get(obj); if (!sd) return;

#define MUSIC_MODULE_STATE_MAINMENU  0
#define MUSIC_MODULE_STATE_MENUMUSIC 1
#define MUSIC_MODULE_STATE_CDDA      2
#define MUSIC_MODULE_STATE_FOLDER    3
#define MUSIC_MODULE_STATE_BROWSE_DB 4
#define MUSIC_MODULE_STATE_BROWSE_FOLDER 5
#define MUSIC_MODULE_STATE_WEBRADIO  6
#define MUSIC_MODULE_STATE_POPUP     7

typedef struct _E_Smart_Data E_Smart_Data;

struct _E_Smart_Data
{
   Enna               *enna;
   Evas_Coord          x, y, w, h;
   Evas_Object        *edje;
   Evas_Object        *obj;
   Evas_Object        *switcher;
   Evas_Object        *menu;
   Evas_Object        *menu_music;
   Evas_Object        *fm;
   Evas_Object        *cdda;
   Evas_Object        *cover;
   Evas_Object        *popup;
   char               *music_path;
   char               *radio_path;
   unsigned int        state;
   unsigned int        old_state;

};

/* local subsystem functions */
static int          _late_clear(void *data);
static void         _update_mainmenu(E_Smart_Data * sd);
static void         _update_menu_music(E_Smart_Data * sd);
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
enna_module_music_add(Evas * evas)
{
   _e_smart_init();
   return evas_object_smart_add(evas, _e_smart);
}

static void
_enna_module_music_init(E_Smart_Data * sd)
{

   sd->music_path = enna_config_get_conf_value_or_default("music_module",
							  "base_path", "/");
   sd->radio_path = enna_config_get_conf_value_or_default("radio_module",
							  "base_path", "/");

}

EAPI void
enna_module_music_process_event(Evas_Object * obj, enna_event event)
{
   API_ENTRY           return;

   if (sd->state == MUSIC_MODULE_STATE_MAINMENU)
     {
	enna_list_process_event(sd->menu, event);
     }
   else if (sd->state == MUSIC_MODULE_STATE_MENUMUSIC)
     {
	if (event == enna_event_escape)
	  {
	     sd->state = MUSIC_MODULE_STATE_MAINMENU;
	     _update_mainmenu(sd);
	  }
	else
	   enna_list_process_event(sd->menu_music, event);
     }
   else if ( sd->state == MUSIC_MODULE_STATE_BROWSE_FOLDER)
     {	
	if ((event == enna_event_1))
	  {
	     sd->popup = enna_popup_add(sd->enna->evas);
	     sd->old_state = sd->state;
	     sd->state = MUSIC_MODULE_STATE_POPUP;
	     evas_object_show(sd->popup);
	     enna_popup_file_set(sd->popup, enna_fm_selected_file_get(sd->fm));
	  }
	else if (event == enna_event_escape)
	   edje_object_signal_emit(sd->edje, "enna,state,infos,hide", "enna");

	enna_fm_process_event(sd->fm, event);
     }
   else if ( sd->state == MUSIC_MODULE_STATE_BROWSE_DB)
     {
	enna_fm_db_process_event(sd->fm, event);
     }
   else if (sd->state == MUSIC_MODULE_STATE_CDDA)
      enna_cdda_process_event(sd->cdda, event);
   else if (sd->state == MUSIC_MODULE_STATE_POPUP)
     {
	if (event == enna_event_1 || event == enna_event_escape)
	  {
	     evas_object_hide(sd->popup);
	     evas_object_del(sd->popup);
	     sd->state = sd->old_state;
	  }
	else
	  {
	     enna_popup_process_event(sd->popup, event);
	  }
     }
}

EAPI void
enna_module_music_focus_set(Evas_Object * obj, unsigned int focus)
{
   API_ENTRY           return;
}

static void
_exit_fm_cb(void *data1, void *data2)
{
   E_Smart_Data       *sd;

   sd = (E_Smart_Data *) data1;
   if (!sd)
      return;

   enna_util_switch_objects(sd->switcher, sd->fm, NULL);
   ecore_timer_add(1.0, _late_clear, sd->fm);
   if (sd->state == MUSIC_MODULE_STATE_BROWSE_FOLDER)
     {
	sd->state = MUSIC_MODULE_STATE_MAINMENU;
	_update_mainmenu(sd);
     }
   else if (sd->state == MUSIC_MODULE_STATE_BROWSE_DB)
     {
	sd->state = MUSIC_MODULE_STATE_MENUMUSIC;
	_update_menu_music(sd);
     }
}

static void
_fm_change_path_cb(void *data1, char *path)
{
   E_Smart_Data       *sd;

   sd = (E_Smart_Data *) data1;
   if (!sd)
      return;
   dbg("change path : %s\n", path);

   edje_object_signal_emit(sd->edje, "enna,state,path,hide", "enna");
   edje_object_part_text_set(sd->edje, "enna.text.path",
			     ecore_file_file_get(path));
   edje_object_signal_emit(sd->edje, "enna,state,path,show", "enna");

}

static void
_exit_cdda_cb(void *data1, void *data2)
{
   E_Smart_Data       *sd;

   sd = (E_Smart_Data *) data1;
   if (!sd)
      return;

   enna_util_switch_objects(sd->switcher, sd->cdda, NULL);
   ecore_timer_add(1.0, _late_clear, sd->cdda);
   sd->state = MUSIC_MODULE_STATE_MAINMENU;
   _update_mainmenu(sd);
}

static void
_play_file_cb(void *data1, void *data2, char *filename)
{
   E_Smart_Data       *sd;

   sd = (E_Smart_Data *) data1;

   if (!sd)
      return;

   enna_mediaplayer_play(sd->enna->mediaplayer, filename,
			 ENNA_MEDIA_TYPE_MUSIC_FILE, NULL);

}

static void
_hilight_file_cb(void *data1, void *data2, char *filename)
{
   E_Smart_Data       *sd;
   Enna               *enna;
   Enna_Metadata      *metadata;
   long                mb, kb;
   long                m, s;
   char                tmp[256];
   char                tmp2[256];
   int                 w, h;

   sd = (E_Smart_Data *) data1;

   if (!sd)
      return;

   if (!filename)
      edje_object_signal_emit(sd->edje, "enna,state,infos,hide", "enna");
   else
     {

	edje_object_signal_emit(sd->edje, "enna,state,infos,hide", "enna");
	enna = evas_data_attach_get(evas_object_evas_get(sd->edje));

	metadata = enna_scanner_audio_metadata_get(filename);
	if (metadata)
	  {
	     char *cover;
	     
	     edje_object_part_text_set(sd->edje, "text.album", metadata->album);
	     edje_object_part_text_set(sd->edje, "text.title", metadata->title);
	     edje_object_part_text_set(sd->edje, "text.artist", metadata->artist);
	     edje_object_part_text_set(sd->edje, "text.filename",
				       ecore_file_file_get(filename));

	     edje_object_part_text_set(sd->edje, "text.genre", metadata->genre);

	     mb = metadata->size / (1024 * 1024);
	     kb = metadata->size / 1024 - (mb * 1024);
	     snprintf(tmp, sizeof(tmp), "%li,%li MB", mb, kb);
	     edje_object_part_text_set(sd->edje, "text.size", tmp);
	     //m = metadata->duration / 60;
	     //s = metadata->duration - (m * 60);
	     //snprintf(tmp2, sizeof(tmp2), "%02li mn %02li sec", m, s);
	     //edje_object_part_text_set(sd->edje, "text.duration", tmp2);
	     cover = (char*)enna_scanner_cover_get(metadata->album, metadata->artist);
	     if (cover)
	       {
		  edje_object_part_unswallow(sd->edje, sd->cover);
		  evas_object_hide(sd->cover);
		  evas_object_del(sd->cover);
		  sd->cover = enna_reflection_add(evas_object_evas_get(sd->edje));
		  edje_object_part_swallow(sd->edje, "cover.swallow", sd->cover);
		  edje_object_part_geometry_get(sd->edje, "cover.swallow", NULL,
						NULL, &w, &h);
		  enna_reflection_file_set(sd->cover, cover);
		  evas_object_resize(sd->cover, w, h);
		  ENNA_FREE(cover);
	       }
	     else
	       {
	          edje_object_part_unswallow(sd->edje, sd->cover);
	          evas_object_hide(sd->cover);
	          evas_object_del(sd->cover);
	          sd->cover = edje_object_add(evas_object_evas_get(sd->obj));
		  edje_object_file_set(sd->edje, enna_config_theme_get(), "unknown_cover");
	          
	          edje_object_part_geometry_get(sd->edje, "enna.cover.swallow", NULL, NULL, &w, &h);
	          edje_extern_object_min_size_set(sd->cover, w, h);
		  edje_object_part_swallow(sd->edje, "enna.cover.swallow", sd->cover);
	          evas_object_resize(sd->cover, w, h);
	          evas_object_show(sd->cover); 
	       }
	     edje_object_signal_emit(sd->edje, "enna,state,infos,show", "enna");
	  }
	
     }

}

static void
_play_radio_cb(void *data1, void *data2, char *filename)
{
   E_Smart_Data       *sd;

   sd = (E_Smart_Data *) data1;

   if (!sd)
      return;
}

static void
_play_track_cb(void *data1, void *data2, int selected)
{
   E_Smart_Data       *sd;
   int                 nb_tracks = 0;
   char               *device = NULL;
   char                tmp[4096];

   sd = (E_Smart_Data *) data1;

   if (!sd)
      return;

   nb_tracks = enna_cdda_nbtracks_get(sd->cdda);
   device = enna_cdda_device_get(sd->cdda);

   sprintf(tmp, "cdda://%s#%d#%d", device, nb_tracks, selected + 1);
   dbg("track : %s\n", tmp);
   enna_mediaplayer_play(sd->enna->mediaplayer, tmp, ENNA_MEDIA_TYPE_MUSIC_CDDA,
			 NULL);

}
static void
_listen_to_webradio_cb(void *data, void *data2)
{
   E_Smart_Data       *sd;
   char               *root_path;

   sd = (E_Smart_Data *) data;

   root_path = (char *)data2;
   if (!root_path)
      return;

   sd->fm = enna_fm_add(evas_object_evas_get(sd->edje));
   enna_util_switch_objects(sd->switcher, sd->menu, sd->fm);
   ecore_timer_add(1.0, _late_clear, sd->menu);
   sd->state = MUSIC_MODULE_STATE_WEBRADIO;
   enna_fm_filter_set(sd->fm, "radio");
   enna_fm_root_set(sd->fm, root_path);
   edje_object_signal_emit(sd->edje, "enna,state,path,hide", "enna");
   edje_object_part_text_set(sd->edje, "enna.text.path", "Web Radios");
   edje_object_signal_emit(sd->edje, "enna,state,path,show", "enna");
   enna_fm_exe_file_cb_set(sd->fm, _play_radio_cb, sd, NULL);
   enna_fm_exit_cb_set(sd->fm, _exit_fm_cb, sd, NULL);

}

static void
_listen_to_cdda_cb(void *data, void *data2)
{
   E_Smart_Data       *sd;
   char               *device;

   sd = (E_Smart_Data *) data;

   device = (char *)data2;
   sd->cdda = enna_cdda_add(evas_object_evas_get(sd->edje));
   enna_util_switch_objects(sd->switcher, sd->menu, sd->cdda);
   ecore_timer_add(1.0, _late_clear, sd->menu);
   enna_cdda_device_set(sd->cdda, device);
   sd->state = MUSIC_MODULE_STATE_CDDA;
   enna_cdda_exe_track_cb_set(sd->cdda, _play_track_cb, sd, NULL);
   enna_cdda_exit_cb_set(sd->cdda, _exit_cdda_cb, sd, NULL);
}

static void
_listen_to_ipod_cb(void *data, void *data2)
{
   E_Smart_Data       *sd;
   char               *mount_point;

   dbg("Listen iPOD\n");

   sd = (E_Smart_Data *) data;
   if (!sd)
      return;

   mount_point = (char *)data2;
   dbg("Ipod Mount Point %s\n", mount_point);

}

static void
_listen_to_music_by_folder_cb(void *data, void *data2)
{
   E_Smart_Data       *sd;
   char               *root_path;

   sd = (E_Smart_Data *) data;

   root_path = (char *)data2;
   if (!root_path)
      return;

   sd->fm = enna_fm_add(evas_object_evas_get(sd->edje));

   if (sd->state == MUSIC_MODULE_STATE_MAINMENU)
     {
	enna_util_switch_objects(sd->switcher, sd->menu, sd->fm);
	ecore_timer_add(1.0, _late_clear, sd->menu);
     }
   else
     {
	enna_util_switch_objects(sd->switcher, sd->menu_music, sd->fm);
	ecore_timer_add(1.0, _late_clear, sd->menu_music);
     }
   sd->state = MUSIC_MODULE_STATE_BROWSE_FOLDER;
   enna_fm_filter_set(sd->fm, "music");
   enna_fm_root_set(sd->fm, root_path);
   edje_object_signal_emit(sd->edje, "enna,state,path,hide", "enna");
   edje_object_part_text_set(sd->edje, "enna.text.path", "Digital Corner");
   edje_object_signal_emit(sd->edje, "enna,state,path,show", "enna");
   enna_fm_exe_file_cb_set(sd->fm, _play_file_cb, sd, NULL);
   enna_fm_hilight_file_cb_set(sd->fm, _hilight_file_cb, sd, NULL);
   enna_fm_exit_cb_set(sd->fm, _exit_fm_cb, sd, NULL);
   enna_fm_change_path_cb_set(sd->fm, _fm_change_path_cb, sd);
}

static void
_listen_to_music_by_artist_cb(void *data, void *data2)
{

   E_Smart_Data       *sd;
   char               *root_path;

   sd = (E_Smart_Data *) data;

   root_path = (char *)data2;
   if (!root_path)
      return;

   dbg("Browse by artist : %s\n", (char *)data2);
   sd->fm = enna_fm_db_add(evas_object_evas_get(sd->edje));
   enna_util_switch_objects(sd->switcher, sd->menu_music, sd->fm);
   ecore_timer_add(1.0, _late_clear, sd->menu_music);
   sd->state = MUSIC_MODULE_STATE_BROWSE_DB;
   edje_object_signal_emit(sd->edje, "enna,state,path,hide", "enna");
   edje_object_part_text_set(sd->edje, "enna.text.path", "Digital Corner");
   edje_object_signal_emit(sd->edje, "enna,state,path,show", "enna");
   enna_fm_db_exe_file_cb_set(sd->fm, _play_file_cb, sd, NULL);
   //enna_fm_db_hilight_file_cb_set(sd->fm, _hilight_file_cb, sd, NULL);
   enna_fm_db_exit_cb_set(sd->fm, _exit_fm_cb, sd, NULL);
   //enna_fm_db_change_path_cb_set(sd->fm, _fm_change_path_cb, sd);

}

static void
_update_menu_music(E_Smart_Data * sd)
{
   sd->menu_music = enna_list_add(evas_object_evas_get(sd->obj));
   enna_list_append_with_icon_name(sd->menu_music, "icon_cdda",
				   "Browse by folder",
				   _listen_to_music_by_folder_cb, NULL, sd,
				   sd->music_path);
   enna_list_append_with_icon_name(sd->menu_music, "icon_cdda",
				   "Browse by Artist",
				   _listen_to_music_by_artist_cb, NULL, sd,
				   "artist");
   /* enna_list_append_with_icon_name(sd->menu_music, "icon_cdda",
				   "Browse by Album",
				   _listen_to_music_by_artist_cb, NULL, sd,
				   "album");
   enna_list_append_with_icon_name(sd->menu_music, "icon_cdda",
				   "Browse by Genre",
				   _listen_to_music_by_artist_cb, NULL, sd,
				   "genre");
   enna_list_append_with_icon_name(sd->menu_music, "icon_cdda", "Search Music",
				   _listen_to_music_by_folder_cb, NULL, sd,
				   NULL);
   */
   enna_list_selected_set(sd->menu_music, 0);
   enna_util_switch_objects(sd->switcher, sd->menu, sd->menu_music);
   ecore_timer_add(1.0, _late_clear, sd->menu);
}

static void
_listen_to_music_cb(void *data, void *data2)
{
   E_Smart_Data       *sd;
   char               *root_path;

   sd = (E_Smart_Data *) data;

   root_path = (char *)data2;
   if (!root_path)
      return;

   _update_menu_music(sd);
   sd->state = MUSIC_MODULE_STATE_MENUMUSIC;

}

static void
_e_smart_reconfigure(E_Smart_Data * sd)
{
   evas_object_move(sd->edje, sd->x, sd->y);
   evas_object_resize(sd->edje, sd->w, sd->h);
}

static void
_update_mainmenu(E_Smart_Data * sd)
{
   Volume             *v;

   edje_object_signal_emit(sd->edje, "enna,state,path,hide", "enna");
   edje_object_part_text_set(sd->edje, "enna.text.path", "Listen to Music");

   dbg("text : %s\n", edje_object_part_text_get(sd->edje, "enna.text.path"));

   edje_object_signal_emit(sd->edje, "enna,state,path,show", "enna");
   if (sd->state != MUSIC_MODULE_STATE_MAINMENU)
      return;

   evas_object_del(sd->menu);
   sd->menu = enna_list_add(evas_object_evas_get(sd->obj));

   v = ecore_list_first_goto(sd->enna->volumes);
   while ((v = ecore_list_next(sd->enna->volumes)))
     {
	switch (v->type)
	  {
	  case DEV_AUDIO_CD:
	     {
		enna_list_append_with_icon_name(sd->menu, "icon_cdda",
						"from Audio CD",
						_listen_to_cdda_cb, NULL, sd,
						v->device);
		break;
	     }
	  case DEV_DATA_CD:
	     {
		char                tmp[4096];

		sprintf(tmp, "from CDROM (%s)", v->label);
		enna_list_append_with_icon_name(sd->menu, "icon_cdrom", tmp,
						_listen_to_music_by_folder_cb,
						NULL, sd, v->mount_point);
		break;
	     }
	  case DEV_DATA_DVD:
	     {
		char                tmp[4096];

		sprintf(tmp, "from DVDROM (%s)", v->label);
		enna_list_append_with_icon_name(sd->menu, "icon_dvdrom", tmp,
						_listen_to_music_by_folder_cb,
						NULL, sd, v->mount_point);
		break;
	     }
	  case DEV_USBDISK:
	     {
		char                tmp[4096];

		sprintf(tmp, "from USBDisk (%s)", v->label);
		enna_list_append_with_icon_name(sd->menu, "icon_usbdisk", tmp,
						_listen_to_music_by_folder_cb,
						NULL, sd, v->mount_point);
		break;
	     }
	  case DEV_IPOD_DEVICE:
	     {
		enna_list_append_with_icon_name(sd->menu, "icon_ipod",
						_("Listen to iPod"),
						_listen_to_ipod_cb, NULL, sd,
						v->mount_point);
		break;
	     }
	  default:
	     break;
	  }
     }

   enna_list_append_with_icon_name(sd->menu, "icon_hd", "from Digital Corner",
				   _listen_to_music_cb, NULL, sd,
				   sd->music_path);
   enna_list_append_with_icon_name(sd->menu, "icon_radio", "from Web Radios",
				   _listen_to_webradio_cb, NULL, sd,
				   sd->radio_path);
   enna_list_selected_set(sd->menu, 0);
   enna_util_switch_objects(sd->switcher, sd->menu_music, sd->menu);

}

static int
_late_clear(void *data)
{
   Evas_Object        *obj;

   obj = (Evas_Object *) data;

   evas_object_hide(obj);
   evas_object_del(obj);

   return 0;
}

static int
_device_add_event(void *data, int type, void *ev)
{
   E_Smart_Data       *sd;

   sd = (E_Smart_Data *) data;

   _update_mainmenu(sd);
   return 1;

}
static void
_e_smart_event_mouse_down(void *data, Evas * e, Evas_Object * obj,
			  void *event_info)
{
   Evas_Event_Mouse_Down *ev;

   ev = event_info;
   E_Smart_Data       *sd;

   sd = (E_Smart_Data *) data;
   if (!sd)
      return;

   switch (sd->state)
     {
     case MUSIC_MODULE_STATE_MAINMENU:
	/* Do nothing */
	break;
     case MUSIC_MODULE_STATE_BROWSE_FOLDER:
	enna_fm_parent_go(sd->fm);
	break;
     case MUSIC_MODULE_STATE_CDDA:
	sd->state = MUSIC_MODULE_STATE_MAINMENU;
	_update_mainmenu(sd);
	break;
     case MUSIC_MODULE_STATE_MENUMUSIC:
	sd->state = MUSIC_MODULE_STATE_MAINMENU;
	_update_mainmenu(sd);
	break;
     case MUSIC_MODULE_STATE_BROWSE_DB:
	//enna_fm_db_parent_go(sd->fm);
	break;
     default:
	dbg("I'm a got, state unknown\n");
     }

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
   Enna               *enna;
   Evas_Object        *o_back;

   sd = calloc(1, sizeof(E_Smart_Data));
   if (!sd)
      return;
   evas_object_smart_data_set(obj, sd);
   _enna_module_music_init(sd);

   sd->obj = obj;
   sd->x = 0;
   sd->y = 0;
   sd->w = 0;
   sd->h = 0;
   sd->edje = edje_object_add(evas_object_evas_get(obj));
   edje_object_file_set(sd->edje, enna_config_theme_get(), "enna/module/music");
   if (!sd->edje)
      dbg("NOT EDJE\n");

   enna = evas_data_attach_get(evas_object_evas_get(sd->edje));
   sd->enna = enna;

   sd->switcher = edje_object_add(evas_object_evas_get(obj));
   edje_object_file_set(sd->switcher, enna_config_theme_get(), "enna/switcher");

   evas_object_smart_member_add(sd->switcher, obj);
   //evas_object_show (sd->switcher);
   edje_object_part_swallow(sd->edje, "enna.switcher.swallow", sd->switcher);

   evas_object_event_callback_add(obj, EVAS_CALLBACK_MOUSE_DOWN,
				  _e_smart_event_mouse_down, NULL);
   evas_object_event_callback_add(obj, EVAS_CALLBACK_MOUSE_UP,
				  _e_smart_event_mouse_up, sd);

   o_back = edje_object_part_object_get(sd->edje, "enna.back");
   evas_object_event_callback_add(obj, EVAS_CALLBACK_MOUSE_DOWN,
				  _e_smart_event_mouse_down, sd);

   evas_object_smart_member_add(sd->edje, obj);
   evas_object_show(sd->edje);
   evas_object_propagate_events_set(obj, 0);

   ecore_event_handler_add(sd->enna->events[ENNA_EVENT_DEV_AUDIO_CD_ADD],
			   _device_add_event, sd);
   ecore_event_handler_add(sd->enna->events[ENNA_EVENT_DEV_DATA_CD_ADD],
			   _device_add_event, sd);
   ecore_event_handler_add(sd->enna->events[ENNA_EVENT_DEV_DATA_DVD_ADD],
			   _device_add_event, sd);
   ecore_event_handler_add(sd->enna->events[ENNA_EVENT_DEV_USBDISK_ADD],
			   _device_add_event, sd);
   ecore_event_handler_add(sd->enna->events[ENNA_EVENT_DEV_CAMERA_ADD],
			   _device_add_event, sd);
   ecore_event_handler_add(sd->enna->events[ENNA_EVENT_DEV_IPOD_ADD],
			   _device_add_event, sd);
   ecore_event_handler_add(sd->enna->events[ENNA_EVENT_DEV_AUDIO_CD_REMOVE],
			   _device_add_event, sd);
   ecore_event_handler_add(sd->enna->events[ENNA_EVENT_DEV_DATA_CD_REMOVE],
			   _device_add_event, sd);
   ecore_event_handler_add(sd->enna->events[ENNA_EVENT_DEV_DATA_DVD_REMOVE],
			   _device_add_event, sd);
   ecore_event_handler_add(sd->enna->events[ENNA_EVENT_DEV_USBDISK_REMOVE],
			   _device_add_event, sd);
   ecore_event_handler_add(sd->enna->events[ENNA_EVENT_DEV_CAMERA_REMOVE],
			   _device_add_event, sd);
   sd->state = MUSIC_MODULE_STATE_MAINMENU;

   _update_mainmenu(sd);
   sd->cover = NULL;

   //evas_object_show (sd->menu);
}

static void
_e_smart_del(Evas_Object * obj)
{
   INTERNAL_ENTRY;
   evas_object_del(sd->edje);
   evas_object_del(sd->switcher);
   if (sd->music_path)
      free(sd->music_path);
   evas_object_del(sd->cdda);
   evas_object_del(sd->fm);
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

   //edje_object_signal_emit(sd->edje, "enna,object,show", "enna");
   evas_object_show(sd->edje);
}

static void
_e_smart_hide(Evas_Object * obj)
{
   INTERNAL_ENTRY;
   //edje_object_signal_emit(sd->edje, "enna,object,hide", "enna");
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
