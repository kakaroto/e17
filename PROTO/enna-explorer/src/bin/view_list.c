/*
 * GeeXboX Enna Media Center.
 * Copyright (C) 2005-2010 The Enna Project
 *
 * This file is part of Enna.
 *
 * Enna is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * Enna is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with Enna; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */

#include <Elementary.h>

#include "enna.h"
#include "enna_config.h"
#include "view_list.h"
#include "vfs.h"
#include "utils.h"

#define SMART_NAME "enna_list"

typedef struct _Smart_Data Smart_Data;
typedef struct _List_Item List_Item;

struct _List_Item
{
   Smart_Data *sd;
   Enna_File *file;
   void (*func_activated) (void *data);
   void *data;
   Elm_Genlist_Item *item;
   Eina_Bool checked;
};

struct _Smart_Data
{
   Evas_Object *obj;
   Eina_List *items;
   Eina_List *checked;
};

static Elm_Object_Item *
_item_new(Evas_Object *ctxpopup, const char * label, const char *icon)
{
   Evas_Object *ic = elm_icon_add(ctxpopup);
   elm_icon_standard_set(ic, icon);
   elm_icon_scale_set(ic, EINA_FALSE, EINA_FALSE);
   return elm_ctxpopup_item_append(ctxpopup, label, ic, NULL, NULL);
}

static void
_item_longpress_cb(void *data, Evas_Object *o __UNUSED__, void *event_info)
{
   Smart_Data *sd = data;
   List_Item *li;

   li = (List_Item*)elm_genlist_item_data_get(event_info);

   evas_object_smart_callback_call(sd->obj, "longpress", li->file);
}

static void
_item_activate(Elm_Genlist_Item *item)
{
   List_Item *li;

   li = (List_Item*)elm_genlist_item_data_get(item);
   if (li->func_activated)
     li->func_activated(li->data);
}

static void
_item_selected(void *data, Evas_Object *obj, void *event_info)
{
   _item_activate(event_info);
}


static void
_file_meta_update(void *data, Enna_File *file __UNUSED__)
{
   List_Item *it = data;

   if (!it || !it->item)
     return;

   elm_genlist_item_update(it->item);
}

static void
_item_remove(Evas_Object *obj, List_Item *item)
{
   Smart_Data *sd = evas_object_data_get(obj, "sd");

   if (!sd || !item) return;

   sd->items = eina_list_remove(sd->items, item);
   enna_file_meta_callback_del(item->file, _file_meta_update);
   enna_file_free(item->file);
   elm_genlist_item_del(item->item);
   free(item);

   return;
}

/* List View */
/* Default genlist items */
static char *
_list_item_default_label_get(void *data, Evas_Object *obj __UNUSED__, const char *part __UNUSED__)
{
   const List_Item *li = data;

   if (!li || !li->file) return NULL;

   return li->file->label ? strdup(li->file->label) : NULL;
}

static void
_list_item_default_check_changed(void *data, Evas_Object *obj, void *event_info __UNUSED__)
{
   List_Item *li = data;
   Smart_Data *sd = li->sd;

   li->checked = elm_check_state_get(obj);
   if (li->checked)
     {
       sd->checked = eina_list_append(sd->checked, li);
     }
   else
       sd->checked = eina_list_remove(sd->checked, li);

   if (eina_list_count(sd->checked))
     evas_object_smart_callback_call(sd->obj, "checked", NULL);
   else
     evas_object_smart_callback_call(sd->obj, "unchecked", NULL);

}

static Evas_Object *
_list_item_default_icon_get(void *data, Evas_Object *obj, const char *part)
{
   List_Item *li = (List_Item*) data;

   if (!li) return NULL;

   if (!strcmp(part, "elm.swallow.icon"))
     {
         Evas_Object *ic;

         ic = elm_icon_add(obj);

         if (ENNA_FILE_IS_BROWSABLE(li->file))
             {
                 elm_icon_file_set(ic, enna_config_theme_get(), li->file->icon);
             }
         else
             {
                 const char *mime;
                 const char *icon;

                 mime = efreet_mime_type_get(li->file->mrl);
                 icon = efreet_mime_type_icon_get(mime, getenv("E_ICON_THEME"), 48);
                 if (!icon)
                     icon = efreet_mime_type_icon_get("unknown", getenv("E_ICON_THEME"), 48);
                 elm_icon_file_set(ic, icon, NULL);
                 /* Don't generate thumb in list, it's too small */
                 /* elm_icon_thumb_set(ic, li->file->mrl, NULL); */
             }

         evas_object_size_hint_min_set(ic, 32, 32);
         evas_object_show(ic);
         return ic;
     }
   else if (!strcmp(part, "elm.swallow.end"))
     {
        Evas_Object *ic;

        ic = elm_check_add(obj);
        evas_object_propagate_events_set(ic, EINA_FALSE);
        elm_check_state_set(ic, li->checked);
        evas_object_smart_callback_add(ic, "changed", _list_item_default_check_changed, li);
        evas_object_show(ic);
        return ic;
     }

   return NULL;
}

static Eina_Bool
_list_item_default_state_get(void *data __UNUSED__, Evas_Object *obj __UNUSED__, const char *part __UNUSED__)
{
   return EINA_FALSE;
}

/* Tracks relative  genlist items */
static char *
_list_item_track_label_get(void *data, Evas_Object *obj __UNUSED__, const char *part)
{
   const List_Item *li = data;
   const char *title;
   const char *track;
   const char *duration;
   const char *artist;
   const char *album;
   char *tmp;

   if (!li || !li->file) return NULL;

   if (!strcmp(part, "elm.text.title"))
     {
        title = enna_file_meta_get(li->file, "title");
        if (!title || !title[0] || title[0] == ' ')
          return li->file->label ? strdup(li->file->label) : NULL;
        else
          {
             tmp = strdup(title);
             eina_stringshare_del(title);
             return tmp;
          }
     }
   if (!strcmp(part, "elm.text.artist"))
     {
        artist = enna_file_meta_get(li->file, "artist");
        album = enna_file_meta_get(li->file, "album");
        if (!artist && !album)
          return NULL;
        else
          {
             char buf[128];
             snprintf(buf, sizeof(buf), "%s - %s", artist, album);
             eina_stringshare_del(artist);
             eina_stringshare_del(album);
             return strdup(buf);
          }
     }
   else if (!strcmp(part, "elm.text.trackno"))
     {
        track = enna_file_meta_get(li->file, "track");
        if (!track)
          return NULL;
        else
          {
             track = eina_stringshare_printf("%02d", atoi(track));
             tmp = strdup(track);
             eina_stringshare_del(track);
             return tmp;
          }
     }
   else if (!strcmp(part, "elm.text.length"))
     {
        duration = enna_file_meta_get(li->file, "duration");
        if (!duration)
          return NULL;
        tmp = strdup(duration);
        eina_stringshare_del(duration);
        return tmp;
     }

   return NULL;
}

static Evas_Object *
_list_item_track_icon_get(void *data, Evas_Object *obj, const char *part)
{
   List_Item *li = (List_Item*) data;

   if (!li) return NULL;

   if (!strcmp(part, "elm.swallow.starred"))
     {
        Evas_Object *ic;
        const char *starred;
        if (!li->file || ENNA_FILE_IS_BROWSABLE(li->file))
          return NULL;

        starred = enna_file_meta_get(li->file, "starred");
        if (!starred)
          return NULL;
        ic = elm_icon_add(obj);
        elm_icon_file_set(ic, enna_config_theme_get(), "icon/favorite");
        evas_object_size_hint_min_set(ic, 24, 24);
        evas_object_show(ic);
        return ic;
     }
   else if (!strcmp(part, "elm.swallow.cover"))
     {
        Evas_Object *ic = NULL;
        const char *cover;

        if (!li->file)
          return NULL;

        ic = elm_icon_add(obj);

        cover = enna_file_meta_get(li->file, "cover");
        if (cover)
          {
             char cv[1024] = { 0 };

             if (*cover == '/')
               snprintf(cv, sizeof(cv), "%s", cover);
             else
               snprintf(cv, sizeof(cv), "%s/covers/%s",
                        enna_util_data_home_get(), cover);

             elm_icon_file_set(ic, cv, NULL);
          }
        else
          {
             elm_icon_file_set(ic,
                               enna_config_theme_get(), "cover/music/file");
          }
        evas_object_size_hint_min_set(ic, 50, 50);
        evas_object_show(ic);
        eina_stringshare_del(cover);
        return ic;
     }

   return NULL;
}

/* Tracks relative  genlist items */
static char *
_list_item_film_label_get(void *data, Evas_Object *obj __UNUSED__, const char *part)
{
   const List_Item *li = data;
   const char *title;
   const char *season;
   const char *episode;
   const char *str;
   char *tmp;

   if (!li || !li->file) return NULL;

   if (!strcmp(part, "elm.text.title"))
     {
        title = enna_file_meta_get(li->file, "title");
        if (!title || !title[0] || title[0] == ' ')
          return li->file->label ? strdup(li->file->label) : NULL;
        else
          {
             tmp = strdup(title);
             eina_stringshare_del(title);
             return strdup(li->file->label);
             return tmp;
          }
     }
   else if (!strcmp(part, "elm.text.episode"))
     {
        season = enna_file_meta_get(li->file, "season");
        episode = enna_file_meta_get(li->file, "episode");
        if (!season || !episode )
          return NULL;
        str = eina_stringshare_printf("Season %02d Episode %02d", atoi(season), atoi(episode));
        tmp = strdup(str);
        eina_stringshare_del(season);
        eina_stringshare_del(episode);
        eina_stringshare_del(str);
        return tmp;
     }

   return NULL;
}

static Evas_Object *
_list_item_film_icon_get(void *data, Evas_Object *obj, const char *part)
{
   List_Item *li = (List_Item*) data;

   if (!li) return NULL;

   if (!strcmp(part, "elm.swallow.cover"))
     {
        Evas_Object *ic = NULL;
        const char *cover;

        if (!li->file)
          return NULL;

        ic = elm_icon_add(obj);

        cover = enna_file_meta_get(li->file, "cover");
        if (cover)
          {
             char cv[1024] = { 0 };

             if (*cover == '/')
               snprintf(cv, sizeof(cv), "%s", cover);
             else
               snprintf(cv, sizeof(cv), "%s/covers/%s",
                        enna_util_data_home_get(), cover);

             elm_icon_file_set(ic, cv, NULL);
          }
        else
          {
             elm_icon_file_set(ic,
                               enna_config_theme_get(), "cover/music/file");
          }
        evas_object_show(ic);
        eina_stringshare_del(cover);
        return ic;
     }
   else if(!strcmp(part, "elm.swallow.played"))
     {

     }

   return NULL;
}

static Elm_Genlist_Item_Class itc_list_default = {
  "default",
  {
    _list_item_default_label_get,
    _list_item_default_icon_get,
    _list_item_default_state_get,
    NULL
  }
};

static Elm_Genlist_Item_Class itc_list_track = {
  "track",
  {
    _list_item_track_label_get,
    _list_item_track_icon_get,
    NULL,
    NULL
  }
};

static Elm_Genlist_Item_Class itc_list_film = {
  "film",
  {
    _list_item_film_label_get,
    _list_item_film_icon_get,
    NULL,
    NULL
  }
};

static void
_smart_select_item(Smart_Data *sd, int n)
{
   List_Item *it;

   it = eina_list_nth(sd->items, n);
   if (!it) return;

   elm_genlist_item_middle_bring_in(it->item);
   elm_genlist_item_selected_set(it->item, 1);
   evas_object_smart_callback_call(sd->obj, "hilight", it->data);
}

static void
_del_cb(void *data, Evas *e __UNUSED__, Evas_Object *obj, void *event_info __UNUSED__)
{
   Smart_Data *sd = data;

   if (!sd)
     return;

   enna_list_clear(obj);
   eina_list_free(sd->items);

   free(sd);
}

Evas_Object *
enna_list_add(Evas_Object *parent)
{
   Evas_Object *obj;
   Smart_Data *sd;

   sd = calloc(1, sizeof(Smart_Data));
   obj = elm_genlist_add(parent);
   evas_object_size_hint_weight_set(obj, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   elm_genlist_horizontal_set(obj, ELM_LIST_COMPRESS);
   evas_object_smart_callback_add(obj, "longpressed", _item_longpress_cb, sd);
   evas_object_show(obj);
   sd->obj = obj;
   evas_object_data_set(obj, "sd", sd);
   evas_object_event_callback_add(obj, EVAS_CALLBACK_DEL, _del_cb, sd);

   return obj;
}

static int
_item_insert_cmp_cb(const void *data1, const void *data2)
{
   List_Item *li1 = (List_Item*) data1;
   List_Item *li2 = (List_Item*) data2;

   if (ENNA_FILE_IS_BROWSABLE(li1->file) && !ENNA_FILE_IS_BROWSABLE(li2->file))
     {
        return -1;
     }
   else if (!ENNA_FILE_IS_BROWSABLE(li1->file) && ENNA_FILE_IS_BROWSABLE(li2->file))
     return 1;
   else
     return (strcasecmp(li1->file->label, li2->file->label));
}

void
enna_list_file_append(Evas_Object *obj, Enna_File *file,
                      void (*func_activated) (void *data),  void *data)
{
   Smart_Data *sd;
   List_Item *it;

   sd = evas_object_data_get(obj, "sd");

   it = ENNA_NEW(List_Item, 1);

   it->func_activated = func_activated;
   it->data = data;
   it->file = enna_file_ref(file);
   it->sd = sd;

   if (file->type == ENNA_FILE_TRACK)
     {
        it->item = elm_genlist_item_sorted_insert (obj, &itc_list_track, it,
                                                   NULL, ELM_GENLIST_ITEM_NONE,
                                                   _item_insert_cmp_cb,
                                                   _item_selected, it);

        enna_file_meta_callback_add(it->file, _file_meta_update, it);
     }
   else if (file->type == ENNA_FILE_FILM)
     {
        it->item = elm_genlist_item_sorted_insert (obj, &itc_list_film, it,
                                                   NULL, ELM_GENLIST_ITEM_NONE,
                                                   _item_insert_cmp_cb,
                                                   _item_selected, it);
        enna_file_meta_callback_add(it->file, _file_meta_update, it);
     }
   else if (file->type == ENNA_FILE_ALBUM)
     {
        it->item = elm_genlist_item_sorted_insert (obj, &itc_list_film, it,
                                                   NULL, ELM_GENLIST_ITEM_NONE,
                                                   _item_insert_cmp_cb,
                                                   _item_selected, it);
     }
   else
     {
        it->item = elm_genlist_item_sorted_insert (obj, &itc_list_default, it,
                                                   NULL, ELM_GENLIST_ITEM_NONE,
                                                   _item_insert_cmp_cb,
                                                   _item_selected, it);
     }
   sd->items = eina_list_append(sd->items, it);
}

void
enna_list_file_remove(Evas_Object *obj, Enna_File *file)
{
   Smart_Data *sd;
   List_Item *it;
   Eina_List *l;

   sd = evas_object_data_get(obj, "sd");

   EINA_LIST_FOREACH(sd->items, l, it)
     {
        if (it->file == file)
          {
             _item_remove(obj, it);
             break;
          }
     }
}

void
enna_list_file_update(Evas_Object *obj, Enna_File *file)
{
   Smart_Data *sd;
   List_Item *it;
   Eina_List *l;

   sd = evas_object_data_get(obj, "sd");

   EINA_LIST_FOREACH(sd->items, l, it)
     {
        if (it->file == file)
          {
             if (it->item)
               elm_genlist_item_update(it->item);
             break;
          }
     }
}

void
enna_list_select_nth(Evas_Object *obj, int nth)
{

}

Eina_List *
enna_list_files_get(Evas_Object* obj)
{
   Smart_Data *sd = evas_object_data_get(obj, "sd");
   Eina_List *files = NULL;
   Eina_List *l;
   List_Item *it;

   EINA_LIST_FOREACH(sd->items, l, it)
     files = eina_list_append(files, it->file);

   return files;
}

int
enna_list_jump_label(Evas_Object *obj, const char *label)
{
   List_Item *it = NULL;
   Eina_List *l;
   int i = 0;

   Smart_Data *sd = evas_object_data_get(obj, "sd");

   if (!sd || !label) return -1;

   EINA_LIST_FOREACH(sd->items, l, it)
     {
        if (it->file->label && !strcmp(it->file->label, label))
          {
             _smart_select_item(sd, i);
             return i;
          }
        i++;
     }

   return -1;
}

int
enna_list_selected_get(Evas_Object *obj)
{
   Eina_List *l;
   List_Item *it;
   int i = 0;
   Smart_Data *sd = evas_object_data_get(obj, "sd");

   if (!sd->items) return -1;
   EINA_LIST_FOREACH(sd->items,l, it)
     {
        if ( elm_genlist_item_selected_get (it->item))
          {
             return i;
          }
        i++;
     }
   return -1;
}

void *
enna_list_selected_data_get(Evas_Object *obj)
{
   Eina_List *l;
   List_Item *it;
   Smart_Data *sd = evas_object_data_get(obj, "sd");

   if (!sd->items) return NULL;

   EINA_LIST_FOREACH(sd->items,l, it)
     {
        if ( elm_genlist_item_selected_get (it->item))
          {
             return it->data;
          }
     }
   return NULL;
}

void
enna_list_jump_ascii(Evas_Object *obj, char k)
{
   List_Item *it;
   Eina_List *l;
   int i = 0;
   Smart_Data *sd = evas_object_data_get(obj, "sd");

   EINA_LIST_FOREACH(sd->items, l, it)
     {
        if (it->file->label[0] == k || it->file->label[0] == k - 32)
          {
             _smart_select_item(sd, i);
             return;
          }
        i++;
     }
}

void
enna_list_clear(Evas_Object *obj)
{
   Smart_Data *sd = evas_object_data_get(obj, "sd");
   List_Item *item;
   Eina_List *l, *l_next;

   eina_list_free(sd->checked);

   EINA_LIST_FOREACH_SAFE(sd->items, l, l_next, item)
     {
        _item_remove(obj, item);
     }
   

   elm_genlist_clear(obj);
}

Eina_List *
enna_list_selected_files_get(Evas_Object *obj)
{
   Eina_List *l;
   Eina_List *files = NULL;
   List_Item *item;
   Smart_Data *sd = evas_object_data_get(obj, "sd");

   EINA_LIST_FOREACH(sd->items, l, item)
     {
        if (item->checked)
          {
             printf("Selected file : %s\n", item->file->uri);
             files = eina_list_append(files, enna_file_ref(item->file));
          }
     }

   return files;
}
