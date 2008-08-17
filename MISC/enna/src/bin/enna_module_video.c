/*
 * enna_module_video.c
 * Copyright (C) Nicolas Aguirre 2006,2007,2008 <aguirre.nicolas@gmail.com>
 *
 * enna_module_video.c is free software copyrighted by Nicolas Aguirre.
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
 * enna_module_video.c IS PROVIDED BY Nicolas Aguirre ``AS IS'' AND ANY EXPRESS
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
#include "enna.h"
#include "enna_list.h"
#include "enna_fm.h"
#include "enna_config.h"
#include "enna_volume_manager.h"
#include "enna_mediaplayer.h"
#include "enna_util.h"

#define API_ENTRY E_Smart_Data *sd; sd = evas_object_smart_data_get(obj); if ((!obj) || (!sd) || (evas_object_type_get(obj) && strcmp(evas_object_type_get(obj), SMART_NAME)))
#define INTERNAL_ENTRY E_Smart_Data *sd; sd = evas_object_smart_data_get(obj); if (!sd) return;

#define SMART_NAME			"enna_module_video"
#define VIDEO_MODULE_STATE_MENU		0
#define VIDEO_MODULE_STATE_FM		1
#define VIDEO_MODULE_STATE_MPLAYER	2
#define VIDEO_MODULE_STATE_XINE		3

typedef struct _E_Smart_Data E_Smart_Data;

struct _E_Smart_Data
{
   Enna               *enna;
   Evas_Coord          coord_x, coord_y, coord_w, coord_h;
   Evas_Object        *edje;
   Evas_Object        *obj;
   Evas_List          *modules;
   Evas_Object        *menu;
   Evas_Object        *fm;
   unsigned char       state;
   unsigned char       old_state;
   char               *video_path;
   Evas_Object        *switcher;
};

/* local subsystem functions */
static void         _update_mainmenu(E_Smart_Data * sd, Enna * enna);

/* local subsystem globals */
static Evas_Smart  *_e_smart = NULL;

static int
_late_clear(void *data)
{
   Evas_Object        *obj;

   obj = (Evas_Object *) data;
   evas_object_hide(obj);
   evas_object_del(obj);

   return 0;
}

static void
_enna_module_video_init(E_Smart_Data * sd)
{
   sd->video_path =
      strdup(enna_config_get_conf_value_or_default
	     ("video_module", "base_path", "/"));
}

static void
_watch_dvd_cb(void *data1, void *data2)
{

   E_Smart_Data       *sd;
   Enna               *enna;

   if (!(sd = (E_Smart_Data *) data1))
      return;

   enna = evas_data_attach_get(evas_object_evas_get(sd->edje));
   enna_mediaplayer_play(enna->mediaplayer, "dvd://",
			 ENNA_MEDIA_TYPE_VIDEO_DVD, (Volume *) data2);
}

static void
_watch_file_cb(void *data1, void *data2, char *filename)
{

   E_Smart_Data       *sd;
   Enna               *enna;

   if (!(sd = (E_Smart_Data *) data1))
      return;

   enna = evas_data_attach_get(evas_object_evas_get(sd->edje));
   enna_mediaplayer_play(enna->mediaplayer, filename,
			 ENNA_MEDIA_TYPE_VIDEO_FILE, NULL);
}

EAPI void
enna_module_video_process_event(Evas_Object * obj, enna_event event)
{
   API_ENTRY           return;

   if (sd->state == VIDEO_MODULE_STATE_MENU)
      enna_list_process_event(sd->menu, event);
   else if (sd->state == VIDEO_MODULE_STATE_FM)
      enna_fm_process_event(sd->fm, event);

}

EAPI void
enna_module_video_focus_set(Evas_Object * obj, unsigned int focus)
{
   API_ENTRY           return;
}

static void
_exit_fm_cb(void *data1, void *data2)
{
   E_Smart_Data       *sd;
   Enna               *enna;

   sd = (E_Smart_Data *) data1;
   if (!sd)
      return;

   enna = evas_data_attach_get(evas_object_evas_get(sd->edje));
   enna_util_switch_objects(sd->switcher, sd->fm, NULL);
   ecore_timer_add(1.0, _late_clear, sd->fm);

   sd->state = VIDEO_MODULE_STATE_MENU;
   _update_mainmenu(sd, enna);

}

static void
_watch_videos_cb(void *data, void *data2)
{
   E_Smart_Data       *sd;
   char               *root_path;

   sd = (E_Smart_Data *) data;
   Enna               *enna;

   enna = evas_data_attach_get(evas_object_evas_get(sd->edje));
   if (!enna)
      return;

   root_path = (char *)data2;

   sd->fm = enna_fm_add(evas_object_evas_get(sd->edje));
   evas_object_show(sd->fm);
   evas_object_hide(sd->menu);
   enna_util_switch_objects(sd->switcher, sd->menu, sd->fm);
   ecore_timer_add(1.0, _late_clear, sd->menu);
   sd->state = VIDEO_MODULE_STATE_FM;
   enna_fm_filter_set(sd->fm, "video");
   enna_fm_root_set(sd->fm, root_path);
   enna_fm_exe_file_cb_set(sd->fm, _watch_file_cb, sd, NULL);
   enna_fm_exit_cb_set(sd->fm, _exit_fm_cb, sd, NULL);
}

static void
_e_smart_reconfigure(E_Smart_Data * sd)
{
   evas_object_move(sd->edje, sd->coord_x, sd->coord_y);
   evas_object_resize(sd->edje, sd->coord_w, sd->coord_h);
}

static void
_update_mainmenu(E_Smart_Data * sd, Enna * enna)
{
   Evas_Object        *icon;
   Volume             *v;

   if (!sd || sd->state != VIDEO_MODULE_STATE_MENU)
      return;

   evas_object_del(sd->menu);
   sd->menu = enna_list_add(evas_object_evas_get(sd->obj));
   v = ecore_list_first_goto(enna->volumes);
   while ((v = ecore_list_next(enna->volumes)))
     {
	switch (v->type)
	  {
	  case DEV_VIDEO_DVD:
	     {
		char                tmp[4096];

		sprintf(tmp, "Watch DVD (%s)", v->label);
		icon = edje_object_add(evas_object_evas_get(sd->obj));
		edje_object_file_set(icon, enna_config_theme_get(),
				     "icon_dvdvideo");
		enna_list_append(sd->menu, icon, tmp, 0, _watch_dvd_cb, NULL, sd, v);	//->device);
		break;
	     }
	  case DEV_DATA_CD:
	     {
		char                tmp[4096];

		sprintf(tmp, "Watch Videos on CDROM (%s)", v->label);
		icon = edje_object_add(evas_object_evas_get(sd->obj));
		edje_object_file_set(icon, enna_config_theme_get(),
				     "icon_cdrom");
		enna_list_append(sd->menu, icon, tmp,
				 0, _watch_videos_cb, NULL, sd, v->mount_point);
		break;
	     }
	  case DEV_DATA_DVD:
	     {
		char                tmp[4096];

		sprintf(tmp, "Watch Videos on DVDROM (%s)", v->label);
		icon = edje_object_add(evas_object_evas_get(sd->obj));
		edje_object_file_set(icon, enna_config_theme_get(),
				     "icon_dvdrom");
		enna_list_append(sd->menu, icon, tmp,
				 0, _watch_videos_cb, NULL, sd, v->mount_point);
		break;
	     }
	  case DEV_USBDISK:
	     {
		char                tmp[4096];

		sprintf(tmp, "Watch Videos on USBDisk (%s)", v->label);
		icon = edje_object_add(evas_object_evas_get(sd->obj));
		edje_object_file_set(icon, enna_config_theme_get(),
				     "icon_usbdisk");
		enna_list_append(sd->menu, icon, tmp,
				 0, _watch_videos_cb, NULL, sd, v->mount_point);
		break;

	     }
	  default:
	     break;
	  }
     }

   icon = edje_object_add(evas_object_evas_get(sd->obj));
   edje_object_file_set(icon, enna_config_theme_get(), "icon_hd");
   enna_list_append(sd->menu, icon, "Watch Videos", 0,
		    _watch_videos_cb, NULL, sd, sd->video_path);
   enna_list_selected_set(sd->menu, 0);
   enna_util_switch_objects(sd->switcher, NULL, sd->menu);

}

static int
_device_add_event(void *data, int type, void *ev)
{
   E_Smart_Data       *sd;
   Enna               *enna;

   sd = (E_Smart_Data *) data;
   enna = evas_data_attach_get(evas_object_evas_get(sd->edje));

   if (!enna)
      return 0;

   _update_mainmenu(sd, enna);
   return 1;
}

static void
_e_smart_add(Evas_Object * obj)
{
   E_Smart_Data       *sd;
   Enna               *enna;

   sd = calloc(1, sizeof(E_Smart_Data));
   if (!sd)
      return;

   _enna_module_video_init(sd);
   evas_object_smart_data_set(obj, sd);
   sd->obj = obj;
   sd->coord_x = 0;
   sd->coord_y = 0;
   sd->coord_w = 0;
   sd->coord_h = 0;
   sd->state = VIDEO_MODULE_STATE_MENU;
   sd->edje = edje_object_add(evas_object_evas_get(obj));
   edje_object_file_set(sd->edje, enna_config_theme_get(), "enna/module/video");
   if (!sd->edje)
      dbg("NOT EDJE\n");

   enna = evas_data_attach_get(evas_object_evas_get(sd->edje));
   sd->enna = enna;

   sd->switcher = edje_object_add(evas_object_evas_get(obj));
   edje_object_file_set(sd->switcher, enna_config_theme_get(), "enna/switcher");
   evas_object_smart_member_add(sd->switcher, obj);
   evas_object_show(sd->switcher);
   edje_object_part_swallow(sd->edje, "enna.switcher.swallow", sd->switcher);
   evas_object_smart_member_add(sd->edje, obj);
   evas_object_propagate_events_set(obj, 0);
   enna = evas_data_attach_get(evas_object_evas_get(sd->edje));
   if (!enna)
      return;
   ecore_event_handler_add(enna->events[ENNA_EVENT_DEV_VIDEO_DVD_ADD],
			   _device_add_event, sd);
   ecore_event_handler_add(enna->events[ENNA_EVENT_DEV_DATA_CD_ADD],
			   _device_add_event, sd);
   ecore_event_handler_add(enna->events[ENNA_EVENT_DEV_DATA_DVD_ADD],
			   _device_add_event, sd);
   ecore_event_handler_add(enna->events[ENNA_EVENT_DEV_USBDISK_ADD],
			   _device_add_event, sd);
   ecore_event_handler_add(enna->events[ENNA_EVENT_DEV_CAMERA_ADD],
			   _device_add_event, sd);
   ecore_event_handler_add(enna->events[ENNA_EVENT_DEV_VIDEO_DVD_REMOVE],
			   _device_add_event, sd);
   ecore_event_handler_add(enna->events[ENNA_EVENT_DEV_DATA_CD_REMOVE],
			   _device_add_event, sd);
   ecore_event_handler_add(enna->events[ENNA_EVENT_DEV_DATA_DVD_REMOVE],
			   _device_add_event, sd);
   ecore_event_handler_add(enna->events[ENNA_EVENT_DEV_USBDISK_REMOVE],
			   _device_add_event, sd);
   ecore_event_handler_add(enna->events[ENNA_EVENT_DEV_CAMERA_REMOVE],
			   _device_add_event, sd);
   _update_mainmenu(sd, enna);

   evas_object_show(sd->menu);
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
   if ((sd->coord_x == x) && (sd->coord_y == y))
      return;
   sd->coord_x = x;
   sd->coord_y = y;
   _e_smart_reconfigure(sd);
}

static void
_e_smart_resize(Evas_Object * obj, Evas_Coord w, Evas_Coord h)
{
   INTERNAL_ENTRY;

   if ((sd->coord_w == w) && (sd->coord_h == h))
      return;
   sd->coord_w = w;
   sd->coord_h = h;
   _e_smart_reconfigure(sd);

}

static void
_e_smart_show(Evas_Object * obj)
{
   INTERNAL_ENTRY;
   evas_object_show(sd->edje);
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

EAPI Evas_Object   *
enna_module_video_add(Evas * evas)
{
   _e_smart_init();
   return evas_object_smart_add(evas, _e_smart);
}
