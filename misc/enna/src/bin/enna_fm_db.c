/*
 * enna_fm_db.c
 * Copyright (C) Nicolas Aguirre 2006,2007,2008 <aguirre.nicolas@gmail.com>
 *
 * enna_fm_db.c is free software copyrighted by Nicolas Aguirre.
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
 * enna_fm_db.c IS PROVIDED BY Nicolas Aguirre ``AS IS'' AND ANY EXPRESS
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
#include <fcntl.h>
#include <unistd.h>
#include <Evas.h>
#include <Ecore.h>
#include <Ecore_File.h>
#include <Ecore_Str.h>
#include <Ecore_Evas.h>
#include <Edje.h>
#include <pthread.h>

#include "enna.h"
#include "enna_module.h"
#include "enna_list.h"
#include "enna_config.h"
#include "enna_mediaplayer.h"
#include "enna_db.h"
#include "enna_util.h"

#define SMART_NAME "enna_fm_db"
#define API_ENTRY E_Smart_Data *sd; sd = evas_object_smart_data_get(obj); if ((!obj) || (!sd) || (evas_object_type_get(obj) && strcmp(evas_object_type_get(obj), SMART_NAME)))
#define INTERNAL_ENTRY E_Smart_Data *sd; sd = evas_object_smart_data_get(obj); if (!sd) return;

#define ENNA_FM_DB_STATE_UNDEF    0
#define ENNA_FM_DB_STATE_ARTISTS  1
#define ENNA_FM_DB_STATE_ALBUM    2
#define ENNA_FM_DB_STATE_ALBUMS   3
#define ENNA_FM_DB_STATE_SONGS    4
#define ENNA_FM_DB_STATE_GENRE    5

typedef struct _E_Smart_Data E_Smart_Data;
typedef struct _Thread_Infos Thread_Infos;
typedef struct _Db_List_Element Db_List_Element;

struct _E_Smart_Data
{
   Enna               *enna;
   Evas_Coord          x, y, w, h;
   Evas_Object        *edje;
   Evas_Object        *obj;
   Evas_Object        *switcher;
   Evas_Object        *o_files;
   unsigned int        state;
   unsigned int        root;
   char               *path;
   char               *filter;
   void                (*exe_file_cb) (void *data1, void *data2,
				       char *filename);
   void               *data1;
   void               *data2;
   void                (*hilight_file_cb) (void *data1, void *data2,
					   char *filename);
   void               *hilight_data1;
   void               *hilight_data2;
   void                (*exit_cb) (void *data1, void *data2);
   void               *exit_data1;
   void               *exit_data2;
   void                (*change_path_cb) (void *data, char *path);
   void               *change_path_data;
   Evas_List          *list_data;
};

struct _Thread_Infos
{
   char               *path;
   char               *oldpath;
   E_Smart_Data       *sd;
};

struct _Db_List_Element
{
   char               *filename;
   char               *item_name;
   unsigned int        type;
   unsigned int        is_selected;
};

/* local subsystem functions */
static void         _scan_db(E_Smart_Data * sd, unsigned int state,
			     unsigned int old_state);
static void         _parent_go(E_Smart_Data * sd, char *path);
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
enna_fm_db_add(Evas * evas)
{
   _e_smart_init();
   return evas_object_smart_add(evas, _e_smart);
}

EAPI void
enna_fm_db_process_event(Evas_Object * obj, enna_event event)
{
   API_ENTRY           return;

   if (event == enna_event_escape)
     {
	//dbg("enna_fm_db: go_parent ev %d %s\n", event, sd->path);
	_parent_go(sd, sd->path);
     }
   else
      enna_list_process_event(sd->o_files, event);
}

/* root_path should be artist, album, genre */
EAPI int
enna_fm_db_root_set(Evas_Object * obj, char *root)
{
   API_ENTRY           return 0;

   if (!root)
      return 0;

   if (!strcmp(root, "artist"))
      sd->root = ENNA_FM_DB_STATE_ARTISTS;
   else if (!strcmp(root, "album"))
      sd->root = ENNA_FM_DB_STATE_ALBUMS;
   else if (!strcmp(root, "genre"))
      sd->root = ENNA_FM_DB_STATE_GENRE;

   if (sd->o_files)
      evas_object_del(sd->o_files);

   _scan_db(sd, sd->root, 0);
   return 1;
}

EAPI int
enna_fm_db_filter_set(Evas_Object * obj, char *filter)
{
   API_ENTRY           return 0;

   if (!filter)
      return 0;
   sd->filter = strdup(filter);
   return 1;
}

EAPI int
enna_fm_db_exe_file_cb_set(Evas_Object * obj,
			   void (exe_file_cb) (void *data1, void *data2,
					       char *filename), void *data1,
			   void *data2)
{
   API_ENTRY           return 0;

   sd->exe_file_cb = exe_file_cb;
   sd->data1 = data1;
   sd->data2 = data2;
   return 1;
}

EAPI int
enna_fm_db_hilight_file_cb_set(Evas_Object * obj,
			       void (hilight_file_cb) (void *data1, void *data2,
						       char *filename),
			       void *data1, void *data2)
{
   API_ENTRY           return 0;

   sd->hilight_file_cb = hilight_file_cb;
   sd->hilight_data1 = data1;
   sd->hilight_data2 = data2;
   return 1;
}

EAPI int
enna_fm_db_exit_cb_set(Evas_Object * obj,
		       void (exit_cb) (void *data1, void *data2), void *data1,
		       void *data2)
{
   API_ENTRY           return 0;

   sd->exit_cb = exit_cb;
   sd->exit_data1 = data1;
   sd->exit_data2 = data2;
   return 1;
}

EAPI int
enna_fm_db_change_path_cb_set(Evas_Object * obj,
			      void (change_path_cb) (void *data, char *path),
			      void *data)
{
   API_ENTRY           return 0;

   sd->change_path_cb = change_path_cb;
   sd->change_path_data = data;
   return 1;
}

EAPI void
enna_fm_db_parent_go(Evas_Object * obj)
{
   API_ENTRY           return;

   _parent_go(sd, sd->path);
}

/*
  static void
  _hilight_file_cb (void *data, void *data2)
  {
  E_Smart_Data *sd;
  char *path;

  sd = (E_Smart_Data *) data;
  path = (char *) data2;

  if (sd->hilight_file_cb)
  sd->hilight_file_cb (sd->hilight_data1, sd->hilight_data2, path);

  }

  static void
  _file_selected_cb (void *data, void *data2)
  {
  E_Smart_Data *sd;
  char *path;
   
  sd = (E_Smart_Data *) data;
  path = (char *) data2;

  dbg("Item selected %s\n", path);
   
  if (sd->exe_file_cb)
  sd->exe_file_cb (sd->data1, sd->data2, path);
  }
*/

static void
_scan_db(E_Smart_Data * sd, unsigned int state, unsigned int old_state)
{

}

static void
_parent_go(E_Smart_Data * sd, char *path)
{
   if (!sd || !path || !sd->root)
      return;
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
   sd->state = ENNA_FM_DB_STATE_UNDEF;
   sd->edje = edje_object_add(evas_object_evas_get(obj));
   edje_object_file_set(sd->edje, enna_config_theme_get(), "enna/filemanager");
   evas_object_smart_member_add(sd->edje, obj);
   evas_object_show(sd->edje);

   sd->enna = evas_data_attach_get(evas_object_evas_get(sd->edje));

   sd->switcher = edje_object_add(evas_object_evas_get(obj));
   edje_object_file_set(sd->switcher, enna_config_theme_get(), "enna/switcher");
   evas_object_smart_member_add(sd->switcher, obj);
   evas_object_show(sd->switcher);
   edje_object_part_swallow(sd->edje, "enna.switcher.swallow", sd->switcher);
   sd->o_files = NULL;
   sd->path = NULL;
   sd->filter = NULL;
   sd->exe_file_cb = NULL;
   sd->data1 = NULL;
   sd->data2 = NULL;

   evas_object_propagate_events_set(obj, 0);
}

static void
_e_smart_del(Evas_Object * obj)
{
   INTERNAL_ENTRY;
   evas_object_del(sd->o_files);
   evas_object_del(sd->switcher);
   evas_object_del(sd->edje);
   ENNA_FREE(sd->path);
   ENNA_FREE(sd->filter);
   ENNA_FREE(sd);
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
