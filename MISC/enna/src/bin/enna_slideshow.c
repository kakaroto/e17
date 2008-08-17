/*
 * enna_slideshow.c
 * Copyright (C) Nicolas Aguirre 2006,2007,2008 <aguirre.nicolas@gmail.com>
 *
 * enna_slideshow.c is free software copyrighted by Nicolas Aguirre.
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
 * enna_slideshow.c IS PROVIDED BY Nicolas Aguirre ``AS IS'' AND ANY EXPRESS
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
#include <fnmatch.h>
#include <Evas.h>
#include <Ecore.h>
#include <Ecore_File.h>
#include <Ecore_Str.h>
#include <Ecore_Evas.h>
#include <Edje.h>

#include "enna_slideshow.h"
#include "enna.h"
#include "enna_module.h"
#include "enna_list.h"
#include "enna_config.h"
#include "enna_mediaplayer.h"
#include "enna_image.h"
#include "enna_event.h"

#define SMART_NAME "enna_slideshow"
#define API_ENTRY E_Smart_Data *sd; sd = evas_object_smart_data_get(obj); if ((!obj) || (!sd) || (evas_object_type_get(obj) && strcmp(evas_object_type_get(obj), SMART_NAME)))
#define INTERNAL_ENTRY E_Smart_Data *sd; sd = evas_object_smart_data_get(obj); if (!sd) return;
typedef struct _E_Smart_Data E_Smart_Data;

struct _E_Smart_Data
{
   Evas_Coord          x, y, w, h;
   Evas_Object        *edje;
   Evas_Object        *obj;
   Ecore_List         *playlist;
   unsigned int        playlist_id;
   Evas_Object        *image;
   Ecore_Timer        *timer;
   double              time;	//in seconds
   void                (*exit_cb) (void *data1, void *data2);
   void               *exit_data1;
   void               *exit_data2;
};

/* local subsystem functions */

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
enna_slideshow_add(Evas * evas)
{
   _e_smart_init();
   return evas_object_smart_add(evas, _e_smart);
}

static void
_switch_images(E_Smart_Data * sd, Evas_Object * obj1, Evas_Object * obj2)
{
   if (!sd || (!obj1 && !obj2))
      return;

   if ((obj1 && obj2))
     {
	edje_object_part_unswallow(sd->edje, obj1);
	edje_object_part_swallow(sd->edje, "enna.switcher.swallow2", obj1);
	edje_object_signal_emit(sd->edje, "enna,swallow2,default,now", "enna");
	edje_object_signal_emit(sd->edje, "enna,swallow1,state1,now", "enna");
	edje_object_part_swallow(sd->edje, "enna.switcher.swallow1", obj2);
	edje_object_signal_emit(sd->edje, "enna,swallow1,default", "enna");
	edje_object_signal_emit(sd->edje, "enna,swallow2,state2", "enna");
	evas_object_hide(obj1);
     }
   else if (!obj2)
     {
	edje_object_part_unswallow(sd->edje, obj1);
	edje_object_part_swallow(sd->edje, "enna.switcher.swallow2", obj1);
	edje_object_signal_emit(sd->edje, "enna,swallow2,default,now", "enna");
	edje_object_signal_emit(sd->edje, "enna,swallow2,state2", "enna");
     }
   else if (!obj1)
     {
	edje_object_signal_emit(sd->edje, "enna,swallow1,state1,now", "enna");
	edje_object_part_swallow(sd->edje, "enna.switcher.swallow1", obj2);
	edje_object_signal_emit(sd->edje, "enna,swallow1,default", "enna");
	evas_object_show(obj2);
     }
}

static int
_next_photo(void *data)
{
   E_Smart_Data       *sd;

   sd = data;
   if (!sd)
      return 0;

   return enna_slideshow_next(sd->obj);
}

static int
_has_suffix(char *str, Evas_List * patterns)
{
   Evas_List          *l;
   int                 result = 0;

   if (!patterns || !str || !str[0])
      return 0;
   for (l = patterns; l; l = evas_list_next(l))
     {
	result |= ecore_str_has_suffix(str, (char *)l->data);
     }
   return result;
}

EAPI int
enna_slideshow_exit_cb_set(Evas_Object * obj,
			   void (exit_cb) (void *data1, void *data2),
			   void *data1, void *data2)
{
   API_ENTRY           return 0;

   sd->exit_cb = exit_cb;
   sd->exit_data1 = data1;
   sd->exit_data2 = data2;
   return 1;
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

EAPI int
enna_slideshow_next(Evas_Object * obj)
{
   char               *filename;
   Evas_Object        *o;

   API_ENTRY           return 0;

   sd->playlist_id++;
   filename = ecore_list_index_goto(sd->playlist, sd->playlist_id);
   dbg("filename : %s\n", filename);
   if (filename)
     {
	dbg("new image : %s - %d\n", filename, sd->playlist_id);
	o = sd->image;
	sd->image = enna_image_add(evas_object_evas_get(sd->edje));
	enna_image_file_set(sd->image, filename);
	evas_object_show(sd->image);
	_switch_images(sd, o, sd->image);
	ecore_timer_add(1.0, _late_clear, o);
	return 1;
     }
   else
     {
	sd->playlist_id--;
	enna_slideshow_stop(sd->obj);
	return 0;
     }
   return 0;
}

EAPI int
enna_slideshow_prev(Evas_Object * obj)
{
   char               *filename;
   Evas_Object        *o;

   API_ENTRY           return 0;

   sd->playlist_id--;
   filename = (char *)ecore_list_index_goto(sd->playlist, sd->playlist_id);
   dbg("filename : %s\n", filename);
   if (filename)
     {
	dbg("new image : %s - %d\n", filename, sd->playlist_id);
	o = sd->image;
	sd->image = enna_image_add(evas_object_evas_get(sd->edje));
	enna_image_file_set(sd->image, filename);
	evas_object_show(sd->image);
	_switch_images(sd, o, sd->image);
	ecore_timer_add(1.0, _late_clear, o);
	return 1;
     }
   else
     {
	sd->playlist_id++;
	return 0;
     }
}

EAPI void
enna_slideshow_stop(Evas_Object * obj)
{
   API_ENTRY           return;

   if (sd->exit_cb)
      sd->exit_cb(sd->exit_data1, sd->exit_data2);
}

EAPI void
enna_slideshow_play(Evas_Object * obj)
{
   Evas_Object        *o;
   char               *filename;

   API_ENTRY           return;

   if (!sd->timer)
     {
	// Play
	o = sd->image;
	filename = (char *)ecore_list_index_goto(sd->playlist, sd->playlist_id);
	if (filename)
	  {

	     sd->image = enna_image_add(evas_object_evas_get(sd->edje));
	     enna_image_file_set(sd->image, filename);
	     evas_object_show(sd->image);
	     _switch_images(sd, o, sd->image);
	     ecore_timer_add(1.0, _late_clear, o);

	     sd->timer = ecore_timer_add(sd->time, _next_photo, sd);
	  }
	else
	  {
	     ecore_timer_del(sd->timer);
	     sd->timer = NULL;

	  }
     }
   else
     {
	//Pause
	ecore_timer_del(sd->timer);
	sd->timer = NULL;
	dbg("Pause\n");
	//dbg("%p\n", sd->edje);
	edje_object_signal_emit(sd->edje, "enna,slideshow,state,pause", "enna");
     }

}

EAPI void
enna_slideshow_decrease_time(Evas_Object * obj)
{
   API_ENTRY           return;

   sd->time -= sd->time;
   if (sd->time <= 0.0)
      sd->time = 1.0;

   ecore_timer_del(sd->timer);
   sd->timer = ecore_timer_add(sd->time, _next_photo, sd);

}

EAPI void
enna_slideshow_increase_time(Evas_Object * obj)
{
   API_ENTRY           return;

   sd->time += 1.0;

   ecore_timer_del(sd->timer);
   sd->timer = ecore_timer_add(sd->time, _next_photo, sd);
}

EAPI void
enna_slideshow_set_filename(Evas_Object * obj, char *filename)
{
   Ecore_List         *files;
   char               *file;
   int                 id = 0, i = 0;
   char               *directory;

   API_ENTRY           return;

   directory = ecore_file_dir_get(filename);
   dbg("path = %s\n", directory);
   files = ecore_file_ls(directory);
   file = ecore_list_first_goto(files);
   while ((file = (char *)ecore_list_next(files)) != NULL)
     {
	if (_has_suffix(file, enna_config_extensions_get("photo")))
	  {
	     char                full_filename[4096];

	     sprintf(full_filename, "%s/%s", directory, (char *)file);
	     //dbg("add : %s\n", strdup(full_filename));
	     ecore_list_append(sd->playlist, strdup(full_filename));
	     if (!strcmp(full_filename, filename))
		id = i;
	     i++;
	  }
     }

   sd->playlist_id = id;
}

EAPI void
enna_slideshow_process_event(Evas_Object * obj, enna_event event)
{
   API_ENTRY           return;

   dbg("enna_slideshow: ev %d\n", event);

   switch (event)
     {
     case enna_event_play:
     case enna_event_pause:
	enna_slideshow_play(obj);
	break;
     case enna_event_next:
	enna_slideshow_next(obj);
	break;
     case enna_event_prev:
	enna_slideshow_prev(obj);
	break;
     case enna_event_rewind:
	enna_slideshow_decrease_time(obj);
	break;
     case enna_event_fastforward:
	enna_slideshow_increase_time(obj);
	break;
     case enna_event_enter:
     case enna_event_exit:
     case enna_event_stop:
	enna_slideshow_stop(obj);
     default:
	break;
     }

}

static void
_e_smart_reconfigure(E_Smart_Data * sd)
{
   evas_object_move(sd->edje, sd->x, sd->y);
   evas_object_resize(sd->edje, sd->w, sd->h);
}

static void
_free_playlist(void *data)
{
   dbg("free data\n");
   free(data);
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
   sd->time = 4.0;
   sd->playlist_id = 0;
   sd->edje = edje_object_add(evas_object_evas_get(obj));
   edje_object_file_set(sd->edje, enna_config_theme_get(), "enna/slideshow");
   evas_object_smart_member_add(sd->edje, obj);
   evas_object_show(sd->edje);
   sd->playlist = ecore_list_new();
   ecore_list_init(sd->playlist);
   ecore_list_free_cb_set(sd->playlist, _free_playlist);

   evas_object_propagate_events_set(obj, 0);
}

static void
_e_smart_del(Evas_Object * obj)
{

   INTERNAL_ENTRY;
   ecore_list_destroy(sd->playlist);
   evas_object_del(sd->image);
   ecore_timer_del(sd->timer);
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
