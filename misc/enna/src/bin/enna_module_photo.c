
/*
 * enna_module_photo.c
 * Copyright (C) Nicolas Aguirre 2006,2007,2008 <aguirre.nicolas@gmail.com>
 *
 * enna_module_photo.c is free software copyrighted by Nicolas Aguirre.
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
 * enna_module_photo.c IS PROVIDED BY Nicolas Aguirre ``AS IS'' AND ANY EXPRESS
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
#include "enna_module_photo.h"
#include "enna_slideshow.h"
#include "enna_util.h"

#define API_ENTRY E_Smart_Data *sd; sd = evas_object_smart_data_get(obj); if ((!obj) || (!sd) || (evas_object_type_get(obj) && strcmp(evas_object_type_get(obj), SMART_NAME)))
#define INTERNAL_ENTRY E_Smart_Data *sd; sd = evas_object_smart_data_get(obj); if (!sd) return;

#define SMART_NAME			"enna_module_photo"
#define PHOTO_MODULE_STATE_MENU			0
#define PHOTO_MODULE_STATE_FM			1
#define PHOTO_MODULE_STATE_SLIDESHOW    2

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
   char               *photo_path;
   Evas_Object        *switcher;
   Evas_Object        *switcher_img;
   Evas_Object        *image;
   Evas_Object        *slideshow;
};

/* local subsystem functions */
static void         _update_mainmenu(E_Smart_Data * sd, Enna * enna);

/* local subsystem globals */
static Evas_Smart  *_e_smart = NULL;

static void
_switch_images(E_Smart_Data * sd, Evas_Object * obj1, Evas_Object * obj2)
{
   if (!sd || (!obj1 && !obj2))
      return;

   if ((obj1 && obj2))
     {
	edje_object_part_unswallow(sd->switcher_img, obj1);
	edje_object_part_swallow(sd->switcher_img, "enna.switcher.swallow2",
				 obj1);
	edje_object_signal_emit(sd->switcher_img, "enna,swallow2,default,now",
				"enna");
	edje_object_signal_emit(sd->switcher_img, "enna,swallow1,state1,now",
				"enna");
	edje_object_part_swallow(sd->switcher_img, "enna.switcher.swallow1",
				 obj2);
	edje_object_signal_emit(sd->switcher_img, "enna,swallow1,default",
				"enna");
	edje_object_signal_emit(sd->switcher_img, "enna,swallow2,state2",
				"enna");
	evas_object_hide(obj1);
     }
   else if (!obj2)
     {
	edje_object_part_unswallow(sd->switcher_img, obj1);
	edje_object_part_swallow(sd->switcher_img, "enna.switcher.swallow2",
				 obj1);
	edje_object_signal_emit(sd->switcher_img, "enna,swallow2,default,now",
				"enna");
	edje_object_signal_emit(sd->switcher_img, "enna,swallow2,state2",
				"enna");
     }
   else if (!obj1)
     {
	edje_object_signal_emit(sd->switcher_img, "enna,swallow1,state1,now",
				"enna");
	edje_object_part_swallow(sd->switcher_img, "enna.switcher.swallow1",
				 obj2);
	edje_object_signal_emit(sd->switcher_img, "enna,swallow1,default",
				"enna");
	evas_object_show(obj2);
     }
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

static void
_enna_module_photo_init(E_Smart_Data * sd)
{
   sd->photo_path =
      strdup(enna_config_get_conf_value_or_default
	     ("photo_module", "base_path", "/"));
}

EAPI void
enna_module_photo_process_event(Evas_Object * obj, enna_event event)
{
   API_ENTRY           return;

   if (sd->state == PHOTO_MODULE_STATE_MENU)
      enna_list_process_event(sd->menu, event);
   else if (sd->state == PHOTO_MODULE_STATE_FM)
     {
	if (event == enna_event_escape)
	   _switch_images(sd, sd->image, NULL);
	enna_fm_process_event(sd->fm, event);
     }
   else if (sd->state == PHOTO_MODULE_STATE_SLIDESHOW)
     {
	enna_slideshow_process_event(sd->slideshow, event);
     }
}

EAPI void
enna_module_photo_focus_set(Evas_Object * obj, unsigned int focus)
{
   API_ENTRY           return;
}

static void
_hilight_file_cb(void *data1, void *data2, char *filename)
{
   E_Smart_Data       *sd;
   Evas_Object        *o;

   sd = (E_Smart_Data *) data1;

   if (!sd || !filename)
      return;

   o = sd->image;

   sd->image = enna_image_add(evas_object_evas_get(sd->edje));
   enna_image_file_set(sd->image, filename);
   evas_object_show(sd->image);

   _switch_images(sd, o, sd->image);
   ecore_timer_add(1.0, _late_clear, o);
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
   ecore_timer_add(0.1, _late_clear, sd->fm);
   sd->state = PHOTO_MODULE_STATE_MENU;
   _update_mainmenu(sd, enna);
}

static void
_exit_slideshow(void *data1, void *data2)
{

   E_Smart_Data       *sd;
   Enna               *enna;

   sd = (E_Smart_Data *) data1;

   if (!sd)
      return;
   enna = evas_data_attach_get(evas_object_evas_get(sd->edje));
   sd->state = PHOTO_MODULE_STATE_FM;
   edje_object_signal_emit(enna->edje, "enna,state,slideshow,hide", "enna");
   enna->fs_obj = 0;
   edje_object_part_unswallow(enna->edje, sd->slideshow);
   enna_util_switch_objects(sd->switcher, sd->slideshow, sd->fm);
   ecore_timer_add(1.0, _late_clear, sd->slideshow);
   return;
}

static void
_play_file_cb(void *data1, void *data2, char *filename)
{
   E_Smart_Data       *sd;
   Enna               *enna;

   sd = (E_Smart_Data *) data1;

   if (!sd)
      return;

   enna = evas_data_attach_get(evas_object_evas_get(sd->edje));
   if (!enna)
      return;

   sd->state = PHOTO_MODULE_STATE_SLIDESHOW;
   enna->fs_obj = 1;
   sd->slideshow = enna_slideshow_add(evas_object_evas_get(sd->edje));
   evas_object_show(sd->slideshow);
   edje_object_signal_emit(enna->edje, "enna,state,slideshow,show", "enna");
   enna_slideshow_exit_cb_set(sd->slideshow, _exit_slideshow, sd, NULL);
   enna_slideshow_set_filename(sd->slideshow, filename);
   enna_slideshow_play(sd->slideshow);
   edje_object_part_swallow(enna->edje, "enna.slideshow.swallow",
			    sd->slideshow);

}

static void
_watch_photos_cb(void *data, void *data2)
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
   sd->state = PHOTO_MODULE_STATE_FM;
   enna_fm_filter_set(sd->fm, "photo");
   enna_fm_root_set(sd->fm, root_path);
   enna_fm_exe_file_cb_set(sd->fm, _play_file_cb, sd, NULL);
   enna_fm_exit_cb_set(sd->fm, _exit_fm_cb, sd, NULL);
   enna_fm_hilight_file_cb_set(sd->fm, _hilight_file_cb, sd, NULL);
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

   if (!sd || sd->state != PHOTO_MODULE_STATE_MENU)
      return;

   evas_object_del(sd->menu);
   sd->menu = enna_list_add(evas_object_evas_get(sd->obj));
   v = ecore_list_first_goto(enna->volumes);
   while ((v = ecore_list_next(enna->volumes)))
     {
	switch (v->type)
	  {
	  case DEV_DATA_CD:
	     {
		char                tmp[4096];

		sprintf(tmp, "Watch Photos on CDROM (%s)", v->label);
		icon = edje_object_add(evas_object_evas_get(sd->obj));
		edje_object_file_set(icon, enna_config_theme_get(),
				     "icon_cdrom");
		enna_list_append(sd->menu, icon, tmp,
				 0, _watch_photos_cb, NULL, sd, v->mount_point);
		break;
	     }
	  case DEV_DATA_DVD:
	     {
		char                tmp[4096];

		sprintf(tmp, "Watch Photos on DVDROM (%s)", v->label);
		icon = edje_object_add(evas_object_evas_get(sd->obj));
		edje_object_file_set(icon, enna_config_theme_get(),
				     "icon_dvdrom");
		enna_list_append(sd->menu, icon, tmp,
				 0, _watch_photos_cb, NULL, sd, v->mount_point);
		break;
	     }
	  case DEV_USBDISK:
	     {
		char                tmp[4096];

		sprintf(tmp, "Watch Photos on USBDisk (%s)", v->label);
		icon = edje_object_add(evas_object_evas_get(sd->obj));
		edje_object_file_set(icon, enna_config_theme_get(),
				     "icon_usbdisk");
		enna_list_append(sd->menu, icon, tmp,
				 0, _watch_photos_cb, NULL, sd, v->mount_point);
		break;
	     }
	  default:
	     break;
	  }
     }

   icon = edje_object_add(evas_object_evas_get(sd->obj));
   edje_object_file_set(icon, enna_config_theme_get(), "icon_hd");
   enna_list_append(sd->menu, icon, "Watch Photos", 0,
		    _watch_photos_cb, NULL, sd, sd->photo_path);
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

   _enna_module_photo_init(sd);

   evas_object_smart_data_set(obj, sd);

   sd->obj = obj;
   sd->coord_x = 0;
   sd->coord_y = 0;
   sd->coord_w = 0;
   sd->coord_h = 0;

   sd->state = PHOTO_MODULE_STATE_MENU;
   sd->edje = edje_object_add(evas_object_evas_get(obj));
   edje_object_file_set(sd->edje, enna_config_theme_get(), "enna/module/photo");

   if (!sd->edje)
      dbg("NOT EDJE\n");

   enna = evas_data_attach_get(evas_object_evas_get(sd->edje));
   sd->enna = enna;

   sd->switcher_img = edje_object_add(evas_object_evas_get(sd->edje));
   edje_object_file_set(sd->switcher_img, enna_config_theme_get(),
			"enna/dream");
   edje_object_part_swallow(sd->edje, "enna.blooper.swallow", sd->switcher_img);
   evas_object_show(sd->switcher_img);

   sd->switcher = edje_object_add(evas_object_evas_get(sd->edje));
   edje_object_file_set(sd->switcher, enna_config_theme_get(), "enna/dream");
   evas_object_show(sd->switcher);
   edje_object_part_swallow(sd->edje, "enna.switcher.swallow", sd->switcher);

   sd->image = NULL;
   evas_object_smart_member_add(sd->edje, obj);
   evas_object_propagate_events_set(obj, 0);

   enna = evas_data_attach_get(evas_object_evas_get(sd->edje));
   if (!enna)
      return;

   ecore_event_handler_add(enna->events[ENNA_EVENT_DEV_DATA_CD_ADD],
			   _device_add_event, sd);
   ecore_event_handler_add(enna->events[ENNA_EVENT_DEV_DATA_DVD_ADD],
			   _device_add_event, sd);
   ecore_event_handler_add(enna->events[ENNA_EVENT_DEV_USBDISK_ADD],
			   _device_add_event, sd);
   ecore_event_handler_add(enna->events[ENNA_EVENT_DEV_CAMERA_ADD],
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

/* externally accessible functions */
EAPI Evas_Object   *
enna_module_photo_add(Evas * evas)
{
   _e_smart_init();
   return evas_object_smart_add(evas, _e_smart);
}
