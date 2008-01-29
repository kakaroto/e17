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



#include "enna.h"
#include "enna_module.h"
#include "enna_list.h"
#include "enna_config.h"
#include "enna_mediaplayer.h"
#include "enna_scanner.h"
#include "enna_util.h"
#include "enna_fm_db.h"

#define SMART_NAME "enna_fm_db"
#define API_ENTRY E_Smart_Data *sd; sd = evas_object_smart_data_get(obj); if ((!obj) || (!sd) || (evas_object_type_get(obj) && strcmp(evas_object_type_get(obj), SMART_NAME)))
#define INTERNAL_ENTRY E_Smart_Data *sd; sd = evas_object_smart_data_get(obj); if (!sd) return;

#define ENNA_FM_DB_STATE_UNDEF    0
#define ENNA_FM_DB_ARTISTS_VIEW   1
#define ENNA_FM_DB_ALBUMS_VIEW    2
#define ENNA_FM_DB_TRACKS_VIEW    3


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
  Evas_Object        *o_view;
  char               *artist;
  char               *album;
  char               *track;
  unsigned int        state;
  void                (*exe_file_cb) (void *data1, void *data2,
				      char *filename);
  void               *data1;
  void               *data2;
  void                (*exit_cb) (void *data1, void *data2);
  void               *exit_data1;
  void               *exit_data2;
};

/* local subsystem functions */

static int          _late_clear(void *data);
static void         _select_album_cb(void *data, void *data2);
static void         _select_artist_cb(void *data, void *data2);
static void         _select_track_cb(void *data, void *data2);
static void         _display_artists_view(E_Smart_Data * sd);
static void         _display_albums_view(E_Smart_Data * sd);
static void         _display_tracks_view(E_Smart_Data * sd);

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
     enna_fm_db_parent_go(obj);
   else
     enna_list_process_event(sd->o_view, event);
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

EAPI void
enna_fm_db_parent_go(Evas_Object * obj)
{
   API_ENTRY           return;
   
   switch (sd->state)
     {
      case ENNA_FM_DB_ARTISTS_VIEW:
	 /* exit */
	 if (sd->exit_cb)
	   sd->exit_cb(sd->exit_data1, sd->exit_data2);
	 break;
      case ENNA_FM_DB_ALBUMS_VIEW:
	 sd->state = ENNA_FM_DB_ARTISTS_VIEW;
	 _display_artists_view(sd);
	 break;
      case ENNA_FM_DB_TRACKS_VIEW:
	 sd->state = ENNA_FM_DB_ALBUMS_VIEW;
	 _display_albums_view(sd);
	 break;
   
     }
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
_select_track_cb(void *data, void *data2)
{
   E_Smart_Data       *sd;
   Enna_Metadata      *m;

   sd = (E_Smart_Data *) data;
   m = (Enna_Metadata *)data2;

   if (!sd || !m)
     return;

   if (sd->exe_file_cb)
      sd->exe_file_cb(sd->data1, sd->data2, m->uri);

}

static void
_select_album_cb(void *data, void *data2)
{
   E_Smart_Data       *sd;
   char               *album;

   sd = (E_Smart_Data *) data;
   album = (char *)data2;

   if (!sd || !album)
     return;

   sd->album = strdup(album);
   sd->state = ENNA_FM_DB_TRACKS_VIEW;
   _display_tracks_view(sd);

}

static void
_select_artist_cb(void *data, void *data2)
{
   E_Smart_Data       *sd;
   char               *artist;

   sd = (E_Smart_Data *) data;
   artist = (char *)data2;

   if (!sd || !artist)
     return;

   sd->artist = strdup(artist);
   sd->state = ENNA_FM_DB_ALBUMS_VIEW;
   _display_albums_view(sd);

}

static void 
_display_tracks_view(E_Smart_Data * sd)
{
   Evas_Object *o;
   Enna_Metadata *item;
   Ecore_List *list;
   
   
   list = enna_scanner_audio_tracks_of_album_list_get(sd->artist, sd->album);
   /* FIXME : Sort list by track nb */
   /* ecore_list_sort(list, ECORE_COMPARE_CB(strcasecmp), ECORE_SORT_MIN);*/
   ecore_list_first_goto(list);
   o = sd->o_view;
   sd->o_view = enna_list_add(evas_object_evas_get(sd->edje));
   while ((item = (Enna_Metadata *)ecore_list_next(list)) != NULL)
     {
	char item_name[PATH_MAX];
	if (item->track_nb)
	  sprintf(item_name, "%d - %s",item->track_nb, item->title);
	else
	  sprintf(item_name, "%s",item->title);
	
	enna_list_append_with_icon_name(sd->o_view, "icon_music", item_name,
			 _select_track_cb, NULL, sd,
			 item);
     }
   enna_util_switch_objects(sd->switcher, o, sd->o_view);
   ecore_timer_add(1.0, _late_clear, o);
   enna_list_selected_set(sd->o_view, 0);
}

static void 
_display_albums_view(E_Smart_Data * sd)
{
   Evas_Object *o;
   char *item_name;
   Ecore_List *list;
   int i = 0, to_select = 0;
   
   list = enna_scanner_audio_albums_of_artist_list_get(sd->artist);
   ecore_list_sort(list, ECORE_COMPARE_CB(strcasecmp), ECORE_SORT_MIN);
   ecore_list_first_goto(list);
   o = sd->o_view;
   sd->o_view = enna_list_add(evas_object_evas_get(sd->edje));
   while ((item_name = (char *)ecore_list_next(list)) != NULL)
     {
	
	enna_list_append_with_icon_name(sd->o_view, "icon_album", item_name,
					_select_album_cb, NULL, sd,
					item_name);
	if (sd->album && !strcmp(sd->album, item_name))
	  {
	     to_select = i;
	     ENNA_FREE(sd->album);
	  }
	i++;

     }
   enna_util_switch_objects(sd->switcher, o, sd->o_view);
   ecore_timer_add(1.0, _late_clear, o);
   enna_list_selected_set(sd->o_view, to_select);
}

static void 
_display_artists_view(E_Smart_Data * sd)
{
   Evas_Object *o;
   char *item_name;
   Ecore_List *list;
   int i = 0, to_select = 0;
   
   list = enna_scanner_audio_artists_list_get();
   ecore_list_sort(list, ECORE_COMPARE_CB(strcasecmp), ECORE_SORT_MIN);
   ecore_list_first_goto(list);
   o = sd->o_view;
   sd->o_view = enna_list_add(evas_object_evas_get(sd->edje));
   while ((item_name = (char *)ecore_list_next(list)) != NULL)
     {
	enna_list_append_with_icon_name(sd->o_view, "icon_artist", item_name,
			 _select_artist_cb, NULL, sd,
			 item_name);
	if (sd->artist && !strcmp(sd->artist, item_name))
	  {
	     to_select = i;
	     ENNA_FREE(sd->artist);
	  }
	i++;
     }
   enna_util_switch_objects(sd->switcher, o, sd->o_view);
   ecore_timer_add(1.0, _late_clear, o);
   enna_list_selected_set(sd->o_view, to_select);
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
   edje_object_file_set(sd->edje, enna_config_theme_get(), "enna/filemanager");
   evas_object_smart_member_add(sd->edje, obj);
   evas_object_show(sd->edje);

   sd->enna = evas_data_attach_get(evas_object_evas_get(sd->edje));
   sd->state = ENNA_FM_DB_ARTISTS_VIEW;
   sd->switcher = edje_object_add(evas_object_evas_get(obj));
   edje_object_file_set(sd->switcher, enna_config_theme_get(), "enna/switcher");
   evas_object_smart_member_add(sd->switcher, obj);
   evas_object_show(sd->switcher);
   edje_object_part_swallow(sd->edje, "enna.switcher.swallow", sd->switcher);
   sd->o_view = NULL;
   _display_artists_view(sd);
   evas_object_propagate_events_set(obj, 0);
}

static void
_e_smart_del(Evas_Object * obj)
{
   INTERNAL_ENTRY;
   evas_object_del(sd->o_view);
   evas_object_del(sd->switcher);
   evas_object_del(sd->edje);
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
