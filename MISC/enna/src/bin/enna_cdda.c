/*
 * enna_cdda.c
 * Copyright (C) Nicolas Aguirre 2006,2007,2008 <aguirre.nicolas@gmail.com>
 *
 * enna_cdda.c is free software copyrighted by Nicolas Aguirre.
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
 * enna_cdda.c IS PROVIDED BY Nicolas Aguirre ``AS IS'' AND ANY EXPRESS
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
#include <musicbrainz/queries.h>
#include <musicbrainz/mb_c.h>

#include "enna.h"
#include "enna_module.h"
#include "enna_list.h"
#include "enna_config.h"
#include "enna_mediaplayer.h"
#include "enna_util.h"
#include "enna_scanner.h"

#define SMART_NAME "enna_cdda"
#define API_ENTRY E_Smart_Data *sd; sd = evas_object_smart_data_get(obj); if ((!obj) || (!sd) || (evas_object_type_get(obj) && strcmp(evas_object_type_get(obj), SMART_NAME)))
#define INTERNAL_ENTRY E_Smart_Data *sd; sd = evas_object_smart_data_get(obj); if (!sd) return;
typedef struct _E_Smart_Data E_Smart_Data;

struct _E_Smart_Data
{
   Evas_Coord          x, y, w, h;
   Evas_Object        *edje;
   Evas_Object        *obj;
   Evas_Object        *switcher;
   Evas_Object        *o_files;
   Enna               *enna;
   void                (*exe_track_cb) (void *data1, void *data2, int selected);
   void               *data1;
   void               *data2;
   char               *device;
   int                 nb_tracks;
   void                (*exit_cb) (void *data1, void *data2);
   void               *exit_data1;
   void               *exit_data2;
};

/* local subsystem functions */

static void         _append_cdda_tracks(E_Smart_Data * sd, char *device);
static void         _track_selected_cb(void *data, void *data2);
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
enna_cdda_add(Evas * evas)
{
   _e_smart_init();
   return evas_object_smart_add(evas, _e_smart);
}

EAPI void
enna_cdda_process_event(Evas_Object * obj, enna_event event)
{
   API_ENTRY           return;

   if (event == enna_event_escape)
     {
	if (sd->exit_cb)
	   sd->exit_cb(sd->exit_data1, sd->exit_data2);
     }
   else
      enna_list_process_event(sd->o_files, event);
}

EAPI int
enna_cdda_exe_track_cb_set(Evas_Object * obj,
			   void (exe_track_cb) (void *data1, void *data2,
						int selected), void *data1,
			   void *data2)
{
   API_ENTRY           return 0;

   sd->exe_track_cb = exe_track_cb;
   sd->data1 = data1;
   sd->data2 = data2;
   return 1;
}

EAPI int
enna_cdda_exit_cb_set(Evas_Object * obj,
		      void (exit_cb) (void *data1, void *data2), void *data1,
		      void *data2)
{
   API_ENTRY           return 0;

   sd->exit_cb = exit_cb;
   sd->exit_data1 = data1;
   sd->exit_data2 = data2;
   return 1;
}

EAPI void
enna_cdda_device_set(Evas_Object * obj, char *device)
{
   API_ENTRY           return;

   sd->device = strdup(device);
   _append_cdda_tracks(sd, sd->device);
}

EAPI char          *
enna_cdda_device_get(Evas_Object * obj)
{
   API_ENTRY           return NULL;

   return sd->device;
}

EAPI int
enna_cdda_nbtracks_get(Evas_Object * obj)
{
   API_ENTRY           return 0;

   return sd->nb_tracks;
}

static void
_append_cdda_tracks(E_Smart_Data * sd, char *device)
{

   Evas_List          *metadatas, *l;
   char                temp[PATH_MAX];

   metadatas = NULL;//enna_db_cdda_metadata_get(sd->enna->db, device, NULL, NULL);
   if (metadatas)
     {

	sd->o_files = enna_list_add(evas_object_evas_get(sd->edje));

	for (l = metadatas; l; l = l->next)
	  {
	     int                *id;
	     Enna_Metadata      *metadata;

	     id = malloc(1);
	     *id = metadata->track_nb;

	     metadata = l->data;
	     sprintf(temp, "%d - %s", metadata->track_nb, metadata->title);
	     enna_list_append_with_icon_name(sd->o_files, "icon_cdda", temp,
					     _track_selected_cb, NULL, sd, id);
	     sd->nb_tracks = 0;
	  }
	enna_util_switch_objects(sd->switcher, NULL, sd->o_files);
	enna_list_selected_set(sd->o_files, 0);

     }
   else
     {
	dbg("Please wait for metadatas\n");
     }
}

static void
_track_selected_cb(void *data, void *data2)
{
   E_Smart_Data       *sd;
   int                *id;

   sd = (E_Smart_Data *) data;

   id = data2;

   dbg("ID = %d\n", *id);

   if (sd->exe_track_cb)
      sd->exe_track_cb(sd->data1, sd->data2, *id);
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
   edje_object_file_set(sd->edje, enna_config_theme_get(), "enna/cdda");
   evas_object_smart_member_add(sd->edje, obj);
   evas_object_show(sd->edje);

   sd->enna = evas_data_attach_get(evas_object_evas_get(sd->edje));

   sd->switcher = edje_object_add(evas_object_evas_get(obj));
   edje_object_file_set(sd->switcher, enna_config_theme_get(), "enna/switcher");

   evas_object_smart_member_add(sd->switcher, obj);
   evas_object_smart_member_add(sd->edje, obj);
   evas_object_smart_member_add(sd->o_files, obj);

   evas_object_show(sd->switcher);
   edje_object_part_swallow(sd->edje, "enna.switcher.swallow", sd->switcher);
   sd->o_files = NULL;
   sd->exe_track_cb = NULL;
   sd->data1 = NULL;
   sd->data2 = NULL;
   sd->device = NULL;
   sd->nb_tracks = 0;
   evas_object_propagate_events_set(obj, 0);
}

static void
_e_smart_del(Evas_Object * obj)
{
   INTERNAL_ENTRY;
   if (sd->device)
      free(sd->device);
   evas_object_del(sd->o_files);
   evas_object_del(sd->switcher);
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
