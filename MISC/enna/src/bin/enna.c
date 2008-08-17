/*
 * enna.c
 * Copyright (C) Nicolas Aguirre 2006,2007,2008 <aguirre.nicolas@gmail.com>
 *
 * enna.c is free software copyrighted by Nicolas Aguirre.
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
 * enna.c IS PROVIDED BY Nicolas Aguirre ``AS IS'' AND ANY EXPRESS
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
#include <math.h>
#include "config.h"
#include "enna_config.h"
#include "enna_mainmenu.h"
#include "enna_module.h"
#include "enna_module_music.h"
#include "enna_module_video.h"
#include "enna_module_photo.h"
#include "enna_module_playlist.h"
#include "enna_mediaplayer.h"
#include "enna_miniplayer.h"
#include "enna_lirc.h"
#include "enna_event.h"
#include "enna_volume_manager.h"
#include "enna_scanner.h"
#include "enna_util.h"
#if WITH_BLUETOOTH_SUPPORT
#include "enna_bluetooth.h"
#endif

/* Callbacks */
static void         _resize_viewport_cb(Ecore_Evas * ee);
static void         _tv_activate_cb(void *data, void *data2);
static void         _music_activate_cb(void *data, void *data2);
static void         _video_activate_cb(void *data, void *data2);
static void         _photo_activate_cb(void *data, void *data2);
static void         _playlist_activate_cb(void *data, void *data2);
static void         _mediaplayer_activate_cb (void *data, void *data2);

/* Functions */
static int          enna_init(Enna * enna, int run_gl);
static int          _create_mainmenu(Enna * enna);
static int          _event_quit(void *data, int ev_type, void *ev);

/* Calbacks */

static void
_resize_viewport_cb(Ecore_Evas * ee)
{
   Evas_Coord          w, h, x, y;
   Evas_Coord          min_w, min_h;
   Evas_Object        *edje;
   Evas               *evas;
   unsigned int        font_size = 8;

   if (!ee)
      return;

   edje = ecore_evas_data_get(ee, "enna_main_edje");
   evas = ecore_evas_get(ee);

   if (!evas || !edje)
      return;

   evas_output_viewport_get(evas, &x, &y, &w, &h);

   edje_object_size_min_get(edje, &min_w, &min_h);
   if (w < min_w)
      w = min_w;
   if (h < min_h)
      h = min_h;
   font_size = enna_util_calculate_font_size(w, h);

   edje_text_class_set("enna_infos_label", "Vera", font_size + 2);
   edje_text_class_set("enna_list_label", "Vera", font_size);

   evas_object_resize(edje, w, h);
   evas_object_move(edje, x, y);
   ecore_evas_resize(ee, w, h);
}

static void
_tv_activate_cb(void *data, void *data2)
{
   Enna               *enna;

   enna = (Enna *) data;

   if (!enna)
      return;
}

static void
_music_activate_cb(void *data, void *data2)
{
   Enna               *enna;

   enna = (Enna *) data;

   if (!enna)
      return;
   enna_module_selected_name_set(enna->modules, "Music Module");
}

static void
_video_activate_cb(void *data, void *data2)
{
   Enna               *enna;

   enna = (Enna *) data;

   if (!enna)
      return;
   enna_module_selected_name_set(enna->modules, "Video Module");
}

static void
_photo_activate_cb(void *data, void *data2)
{
   Enna               *enna;

   enna = (Enna *) data;

   if (!enna)
      return;
   enna_module_selected_name_set(enna->modules, "Photo Module");
}

static void
_playlist_activate_cb(void *data, void *data2)
{
   Enna               *enna;

   enna = (Enna *) data;

   if (!enna)
      return;
   enna_module_selected_name_set(enna->modules, "Playlist Module");
}

static void
  _mediaplayer_activate_cb (void *data, void *data2)
{
   Enna *enna;
   enna = (Enna *) data;

   if (!enna)
     return;
   enna_module_selected_name_set (enna->modules, "MediaPlayer Module");
   enna_miniplayer_select(enna->miniplayer);
   
}

static int
_device_add_event(void *data, int type, void *ev)
{
   Enna               *enna;

   //  Device *dev, *new_dev;
   enna = (Enna *) data;

   if (!enna)
      return 0;

   return 1;
}

static int
_device_remove_event(void *data, int type, void *ev)
{
   Enna               *enna;

   enna = (Enna *) data;

   if (!enna)
      return 0;

   return 1;
}

static int
_event_quit(void *data, int ev_type, void *ev)
{
   Ecore_Event_Signal_Exit *e;

   e = (Ecore_Event_Signal_Exit *) ev;

   if (e)
     {
	if (e->interrupt)
	   dbg("Exit: interrupt\n");
	if (e->quit)
	   dbg("Exit: quit\n");
	if (e->terminate)
	   dbg("Exit: terminate\n");
     }
   ecore_main_loop_quit();
   return 1;
}

/* Functions */

static int
enna_init(Enna * enna, int run_gl)
{
   Evas_Coord          w, h;
   char                tmp[PATH_MAX];
   int                 i;

   enna->home = enna_util_user_home_get();

   ecore_init();
   ecore_file_init();
   ecore_evas_init();
   edje_init();

   sprintf(tmp, "%s/.enna", enna->home);

   if (!ecore_file_exists(tmp))
      ecore_file_mkdir(tmp);

   sprintf(tmp, "%s/.enna/covers", enna->home);
   if (!ecore_file_exists(tmp))
      ecore_file_mkdir(tmp);

   enna_scanner_init(enna);
   
#if WITH_BLUETOOTH_SUPPORT
   enna_bluetooth_init();
#endif
   /* No object in fullscreen mode for the moment */
   enna->fs_obj = 0;

   if (run_gl)
      enna->ee = ecore_evas_gl_x11_new(NULL, 0, 0, 0, 64, 64);
   else
      enna->ee = ecore_evas_software_x11_new(NULL, 0, 0, 0, 64, 64);

   if (!enna->ee)
     {
	dbg("Can not Initialize Ecore Evas !\n");
	return 0;
     }

   for (i = 0; i < NBR_ELEMENTS(enna->events); i++)
      enna->events[i] = ecore_event_type_new();

   ecore_event_handler_add(enna->events[ENNA_EVENT_DEV_AUDIO_CD_ADD],
			   _device_add_event, enna);
   ecore_event_handler_add(enna->events[ENNA_EVENT_DEV_DATA_CD_ADD],
			   _device_add_event, enna);
   ecore_event_handler_add(enna->events[ENNA_EVENT_DEV_DATA_DVD_ADD],
			   _device_add_event, enna);
   ecore_event_handler_add(enna->events[ENNA_EVENT_DEV_USBDISK_ADD],
			   _device_add_event, enna);
   ecore_event_handler_add(enna->events[ENNA_EVENT_DEV_VIDEO_DVD_ADD],
			   _device_add_event, enna);
   ecore_event_handler_add(enna->events[ENNA_EVENT_DEV_CAMERA_ADD],
			   _device_add_event, enna);
   ecore_event_handler_add(enna->events[ENNA_EVENT_DEV_AUDIO_CD_REMOVE],
			   _device_remove_event, enna);
   ecore_event_handler_add(enna->events[ENNA_EVENT_DEV_DATA_CD_REMOVE],
			   _device_remove_event, enna);
   ecore_event_handler_add(enna->events[ENNA_EVENT_DEV_DATA_DVD_REMOVE],
			   _device_remove_event, enna);
   ecore_event_handler_add(enna->events[ENNA_EVENT_DEV_USBDISK_REMOVE],
			   _device_remove_event, enna);
   ecore_event_handler_add(enna->events[ENNA_EVENT_DEV_VIDEO_DVD_REMOVE],
			   _device_remove_event, enna);
   ecore_event_handler_add(enna->events[ENNA_EVENT_DEV_CAMERA_REMOVE],
			   _device_remove_event, enna);
   ecore_event_handler_add(ECORE_EVENT_SIGNAL_EXIT, _event_quit, NULL);
   enna_lirc_init(enna);
   enna_volume_manager_init(enna);

   ecore_evas_title_set(enna->ee, "enna HTPC");
   ecore_evas_name_class_set(enna->ee, "enna", "enna");
   ecore_evas_borderless_set(enna->ee, 0);
   ecore_evas_shaped_set(enna->ee, 1);
   enna->evas = ecore_evas_get(enna->ee);

   //ecore_evas_cursor_set(enna->ee, "/home/nico/test2.png", -1, 0, 0);

   enna->black_background = evas_object_rectangle_add(enna->evas);
   evas_object_resize(enna->black_background, 800, 480);
   evas_object_color_set(enna->black_background, 0, 0, 0, 255);
   evas_object_layer_set(enna->black_background, 0);
   evas_object_show(enna->black_background);
   evas_object_event_callback_add(enna->black_background,
				  EVAS_CALLBACK_KEY_DOWN,
				  enna_event_bg_key_down_cb, enna);
   evas_object_focus_set(enna->black_background, 1);
   enna->edje = edje_object_add(enna->evas);
   edje_object_file_set(enna->edje, enna_config_theme_get(), "enna/background");
   edje_object_size_min_get(enna->edje, &w, &h);
   evas_object_resize(enna->edje, w, h);
   evas_object_move(enna->edje, 0, 0);
   evas_object_show(enna->edje);
   ecore_evas_resize(enna->ee, w, h);
   edje_object_signal_emit(enna->edje, "enna,action,anim,logo", "enna");
   ecore_evas_data_set(enna->ee, "enna_main_edje", enna->edje);
   ecore_evas_callback_resize_set(enna->ee, _resize_viewport_cb);
   evas_data_attach_set(enna->evas, enna);
   ecore_evas_show(enna->ee);
   _create_mainmenu(enna);
   return 1;
}

static int
_create_mainmenu(Enna * enna)
{
   Evas_Object        *mainmenu;
   Evas_Object        *modules;
   Evas_Object        *module_mediaplayer;
   Evas_Object        *module_music;
   Evas_Object        *module_video;
   Evas_Object        *module_photo;
   Evas_Object        *module_playlist;
   Evas_Object        *icon;

   char               *config_module;

   if (!enna)
      return 0;

   mainmenu = enna_mainmenu_add(enna->evas);

   edje_object_part_swallow(enna->edje, "menubar.swallow", mainmenu);
   edje_object_signal_emit(enna->edje, "menubar,show", "enna");
   evas_object_show(mainmenu);

   /* Create modules */
   modules = enna_module_add(enna->evas);
   edje_object_part_swallow(enna->edje, "module.swallow", modules);
   edje_object_signal_emit(enna->edje, "module,show", "enna");

   /* Set focus to modules */
   enna_module_has_focus_set(modules, 1);
   evas_object_show(modules);
   enna->modules = modules;

   /* Create miniplayer menu */
   module_mediaplayer = enna_mediaplayer_add(enna->evas);
   enna->mediaplayer = module_mediaplayer;
   enna_module_append(modules, module_mediaplayer, "MediaPlayer Module", 
		      ENNA_MODULE_MEDIAPLAYER);

   enna_mainmenu_append (mainmenu, NULL, _("MediaPlayer"), 1,
			 _mediaplayer_activate_cb, NULL, enna, NULL);
   /* Create menus */
   config_module =
      enna_config_get_conf_value_or_default("tv_module", "used", "1");
   if (config_module[0] == '1')
     {
	/* Module TV doesn't exist yet ! */

	icon = edje_object_add(enna->evas);
	edje_object_file_set(icon, enna_config_theme_get(), "icon_tv");
	enna_mainmenu_append(mainmenu, icon, _("TV"), 0, _tv_activate_cb, NULL,
			     enna, NULL);
     }
   config_module =
      enna_config_get_conf_value_or_default("music_module", "used", "1");
   if (config_module[0] == '1')
     {
	module_music = enna_module_music_add(enna->evas);
	enna_module_append(modules, module_music, "Music Module",
			   ENNA_MODULE_MUSIC);

	icon = edje_object_add(enna->evas);
	edje_object_file_set(icon, enna_config_theme_get(), "icon_music");
	enna_mainmenu_append(mainmenu, icon, _("Music"), 0, _music_activate_cb,
			     NULL, enna, NULL);
     }
   config_module =
      enna_config_get_conf_value_or_default("video_module", "used", "1");
   if (config_module[0] == '1')
     {
	module_video = enna_module_video_add(enna->evas);
	enna_module_append(modules, module_video, "Video Module",
			   ENNA_MODULE_VIDEO);

	icon = edje_object_add(enna->evas);
	edje_object_file_set(icon, enna_config_theme_get(), "icon_video");;
	enna_mainmenu_append(mainmenu, icon, _("Video"), 0, _video_activate_cb,
			     NULL, enna, NULL);
     }
   config_module =
      enna_config_get_conf_value_or_default("photo_module", "used", "1");
   if (config_module[0] == '1')
     {
	module_photo = enna_module_photo_add(enna->evas);
	enna_module_append(modules, module_photo, "Photo Module",
			   ENNA_MODULE_PHOTO);

	icon = edje_object_add(enna->evas);
	edje_object_file_set(icon, enna_config_theme_get(), "icon_photo");
	enna_mainmenu_append(mainmenu, icon, _("Photo"), 0, _photo_activate_cb,
			     NULL, enna, NULL);
     }
   config_module =
      enna_config_get_conf_value_or_default("playlist_module", "used", "1");
   if (config_module[0] == '1')
     {
	module_playlist = enna_module_playlist_add(enna->evas);
	enna_module_append(modules, module_playlist, "Playlist Module",
			   ENNA_MODULE_PLAYLIST);

	icon = edje_object_add(enna->evas);
	edje_object_file_set(icon, enna_config_theme_get(), "icon_pl");;
	enna_mainmenu_append(mainmenu, icon, _("Playlist"), 0,
			     _playlist_activate_cb, NULL, enna, NULL);
     }

   /* Select 1st module */
   enna_module_selected_name_set(modules, "Music Module");
   enna_mainmenu_selected_set(mainmenu, 1);

   enna->mainmenu = mainmenu;
   enna_mainmenu_has_focus_set(mainmenu, 0);

   return 1;
}

static void
enna_deinit(Enna * enna)
{
   evas_object_del(enna->black_background);
   evas_object_del(enna->edje);
   evas_object_del(enna->mainmenu);
   evas_object_del(enna->modules);
   evas_object_del(enna->mediaplayer);
   ecore_list_destroy(enna->storage_devices);
   ecore_list_destroy(enna->volumes);
   edje_shutdown();
   ENNA_FREE(enna->home);
   ecore_file_shutdown();
   ecore_evas_shutdown();
   ecore_shutdown();
   evas_shutdown();
   enna_scanner_shutdown();
   ENNA_FREE(enna);
}

static void
usage(char *binname)
{
   dbg("Usage: %s [-c filename] [-fs] [-th theme_name]\n", binname);
   exit(0);
}

int
main(int arc, char **arv)
{
   Enna               *enna;
   char               *binname = arv[0];
   char               *conffile = NULL;
   char               *theme_name = NULL;
   int                 run_fullscreen = 0;
   int                 run_gl = 0;

   arv++;
   arc--;

   while (arc)
     {
	if (!strcmp("-fs", *arv))
	  {
	     run_fullscreen = 1;
	     arv++;
	     arc--;
	  }
	else if (!strcmp("-gl", *arv))
	  {
	     run_gl = 1;
	     arv++;
	     arc--;
	  }
	else if (!strcmp("-c", *arv))
	  {
	     arv++;
	     if (!--arc)
		usage(binname);
	     conffile = strdup(*arv);
	     arc--;
	     arv++;
	  }
	else if (!strcmp("-th", *arv))
	  {
	     arv++;
	     if (!--arc)
		usage(binname);
	     theme_name = strdup(*arv);
	     dbg("theme in use : %s\n", theme_name);
	     arc--;
	     arv++;
	  }
	else
	   usage(binname);
     }

   /* Must be called first */
   enna_config_init(conffile, theme_name);

   enna = (Enna *) malloc(sizeof(Enna));

   if (!enna_init(enna, run_gl))
      return 0;

   enna_event_init(enna);

   if (run_fullscreen)
      enna_togle_fullscreen(enna);

   ecore_main_loop_begin();

   enna_deinit(enna);
   dbg("Bye Bye !\n");

   return 0;
}
