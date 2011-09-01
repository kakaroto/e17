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

#include <string.h>

#include <Ecore.h>
#include <Ecore_File.h>
#include <Edje.h>
#include <Elementary.h>

#include "enna.h"
#include "enna_config.h"
#include "view_grid.h"
#include "vfs.h"


#define SMART_NAME "enna_grid"

typedef struct _Smart_Data Smart_Data;
typedef struct _Grid_Item Grid_Item;

struct _Grid_Item
{
   Enna_File *file;
   void (*func_activated) (void *data);
   void *data;
   Elm_Gengrid_Item *item;
   Smart_Data *sd;
   Eina_Bool checked;

};

struct _Smart_Data
{
   Evas_Object *o_grid;
   Eina_List *items;
   Eina_List *checked;
};

static char *
_grid_item_label_get(void *data, Evas_Object *obj __UNUSED__, const char *part __UNUSED__)
{
   const Grid_Item *gi = data;

   if (!gi || !gi->file) return NULL;

   return gi->file->label ? strdup(gi->file->label) : NULL;
}

static void
_grid_item_default_check_changed(void *data, Evas_Object *obj, void *event_info __UNUSED__)
{
   Grid_Item *li = data;
   Smart_Data *sd = li->sd;

   li->checked = elm_check_state_get(obj);
   if (li->checked)
     {
       sd->checked = eina_list_append(sd->checked, li);
     }
   else
       sd->checked = eina_list_remove(sd->checked, li);

   if (eina_list_count(sd->checked))
     evas_object_smart_callback_call(sd->o_grid, "checked", NULL);
   else
     evas_object_smart_callback_call(sd->o_grid, "unchecked", NULL);

}

static Evas_Object *
_grid_item_icon_get(void *data, Evas_Object *obj, const char *part)
{

   Grid_Item *gi = (Grid_Item*) data;

   if (!gi) return NULL;

   if (!strcmp(part, "elm.swallow.icon"))
     {
        Evas_Object *ic;

        if (ENNA_FILE_IS_BROWSABLE(gi->file))
          {
              ic = elm_icon_add(obj);
              elm_icon_file_set(ic, enna_config_theme_get(), gi->file->icon);
              evas_object_size_hint_min_set(ic, 96, 96);
          }
        else
          {
             const char *mime;
             const char *icon;

             mime = efreet_mime_type_get(gi->file->mrl);
             icon = efreet_mime_type_icon_get(mime, getenv("E_ICON_THEME"), 96);
             if (!icon)
               icon = efreet_mime_type_icon_get("unknown", getenv("E_ICON_THEME"), 96);
             if (mime && strstr(mime, "image/"))
               {
                  ic = elm_photo_add(obj);
                  evas_object_size_hint_weight_set(ic, EVAS_HINT_EXPAND,
                                                   EVAS_HINT_EXPAND);
                  evas_object_size_hint_align_set(ic, EVAS_HINT_FILL,
                                                  EVAS_HINT_FILL);
                  evas_object_size_hint_aspect_set(ic, EVAS_ASPECT_CONTROL_BOTH, 4, 3);

                  elm_photo_size_set(ic, 114);
                  elm_photo_thumb_set(ic, gi->file->mrl, NULL);
               }
             else
               {
                  ic = elm_icon_add(obj);
                  elm_icon_file_set(ic, icon, NULL);
                  evas_object_size_hint_min_set(ic, 96, 96);
               }
          }

        evas_object_show(ic);
        return ic;
     }
   else if (!strcmp(part, "elm.swallow.end"))
     {
        Evas_Object *ic;

        ic = elm_check_add(obj);
        evas_object_propagate_events_set(ic, EINA_FALSE);
        elm_check_state_set(ic, gi->checked);
        evas_object_smart_callback_add(ic, "changed", _grid_item_default_check_changed, gi);
        evas_object_show(ic);
        return ic;
     }

   return NULL;

}

static Eina_Bool
_grid_item_state_get(void *data __UNUSED__, Evas_Object *obj __UNUSED__, const char *part __UNUSED__)
{
   return EINA_FALSE;
}

static void
_grid_item_del(void *data __UNUSED__, Evas_Object *obj __UNUSED__)
{

}

static void
_item_remove(Evas_Object *obj, Grid_Item *item)
{
   Smart_Data *sd = evas_object_data_get(obj, "sd");

   if (!sd || !item) return;

   elm_gengrid_item_del(item->item);
   sd->items = eina_list_remove(sd->items, item);
   ENNA_FREE(item);

   return;
}

static Elm_Gengrid_Item_Class gic = {
  "default",
  {
    _grid_item_label_get,
    _grid_item_icon_get,
    _grid_item_state_get,
    _grid_item_del
  }
};

static void
_del_cb(void *data, Evas *e __UNUSED__, Evas_Object *obj __UNUSED__, void *event_info __UNUSED__)
{
   Smart_Data *sd = data;
   Grid_Item *gi;
   if (!sd)
     return;

   elm_gengrid_clear(sd->o_grid);

   EINA_LIST_FREE(sd->items, gi)
     free(gi);

   free(sd);
}



static void
_resize_cb(void *data, Evas *e __UNUSED__, Evas_Object *obj __UNUSED__, void *event_info __UNUSED__)
{
   Evas_Coord h;
   Smart_Data *sd = data;

   evas_object_geometry_get(sd->o_grid, NULL, NULL, NULL, &h);
   elm_gengrid_item_size_set(sd->o_grid, 168, 168);
}

static void
_item_longpress_cb(void *data, Evas_Object *o __UNUSED__, void *event_info)
{
   Smart_Data *sd = data;
   Grid_Item *gi;

   gi = (Grid_Item*)elm_gengrid_item_data_get(event_info);

   evas_object_smart_callback_call(sd->o_grid, "longpress", gi->file);
}

static void
_item_click_cb(void *data, Evas *e __UNUSED__, Evas_Object *obj __UNUSED__, void *event_info)
{
   Evas_Event_Mouse_Up *ev = event_info;
   Grid_Item *gi = elm_gengrid_item_data_get(data);

   /* Right click */
   if (ev->button != 3)
     return;

   evas_object_smart_callback_call(gi->sd->o_grid, "longpress", gi->file);
}

static void
_item_realized_cb(void *data, Evas_Object *o __UNUSED__, void *event_info)
{
   Evas_Object *o_item = elm_gengrid_item_object_get(event_info);
   evas_object_event_callback_add(o_item, EVAS_CALLBACK_MOUSE_UP,_item_click_cb, event_info);
}

static void
_item_selected(void *data __UNUSED__, Evas_Object *obj __UNUSED__, void *event_info)
{
   Grid_Item *li;

   li = (Grid_Item*)elm_gengrid_item_data_get(event_info);
   if (li->func_activated)
     li->func_activated(li->data);
}

/* externally accessible functions */

Evas_Object *
enna_grid_add(Evas_Object * parent)
{
   Smart_Data *sd;

   sd = calloc(1, sizeof(Smart_Data));

   sd->o_grid = elm_gengrid_add(parent);
   //elm_gengrid_horizontal_set(sd->o_grid, EINA_TRUE);
   elm_gengrid_multi_select_set(sd->o_grid, EINA_FALSE);
   elm_gengrid_align_set(sd->o_grid, 0, 0);
   elm_gengrid_bounce_set(sd->o_grid, EINA_FALSE, EINA_TRUE);

   evas_object_data_set(sd->o_grid, "sd", sd);
   evas_object_smart_callback_add(sd->o_grid, "realized", _item_realized_cb, sd);
   evas_object_smart_callback_add(sd->o_grid, "longpressed", _item_longpress_cb, sd);
   evas_object_event_callback_add(sd->o_grid, EVAS_CALLBACK_DEL, _del_cb, sd);
   evas_object_event_callback_add(sd->o_grid, EVAS_CALLBACK_RESIZE, _resize_cb, sd);

   return sd->o_grid;
}

void
enna_grid_clear(Evas_Object *obj)
{
   Smart_Data *sd = evas_object_data_get(obj, "sd");
   Grid_Item *item;

   elm_gengrid_clear(obj);
   EINA_LIST_FREE(sd->items, item)
     _item_remove(obj, item);
}


static int
_item_insert_cmp_cb(const void *data1, const void *data2)
{
   Grid_Item *li1 = (Grid_Item*) data1;
   Grid_Item *li2 = (Grid_Item*) data2;

   if (ENNA_FILE_IS_BROWSABLE(li1->file) && !ENNA_FILE_IS_BROWSABLE(li2->file))
     return -1;
   else if (!ENNA_FILE_IS_BROWSABLE(li1->file) && ENNA_FILE_IS_BROWSABLE(li2->file))
     return 1;
   else
     return (strcasecmp(li1->file->label, li2->file->label));
}

void
enna_grid_file_append(Evas_Object *obj, Enna_File *file,
                      void (*func_activated) (void *data), void *data )
{
   Smart_Data *sd;
   Grid_Item *gi;

   sd = evas_object_data_get(obj, "sd");

   gi = ENNA_NEW(Grid_Item, 1);

   gi->func_activated = func_activated;
   gi->data = data;
   gi->file = file;
   gi->sd = sd;

   gi->item = elm_gengrid_item_sorted_insert (obj, &gic, gi,
                                              _item_insert_cmp_cb,
                                              _item_selected, gi);

   sd->items = eina_list_append(sd->items, gi);
}

void
enna_grid_file_remove(Evas_Object *obj, Enna_File *file)
{
   Eina_List *l;
   Grid_Item *gi;
   Smart_Data *sd;

   sd = evas_object_data_get(obj, "sd");

   EINA_LIST_FOREACH(sd->items, l, gi)
     {
        if (file == gi->file)
	  {
	     _item_remove(obj, gi);
	     break;
	  }
     }
}

void
enna_grid_file_update(Evas_Object *obj, Enna_File *file)
{
   Smart_Data *sd;
   Grid_Item *gi;
   Eina_List *l;

   sd = evas_object_data_get(obj, "sd");

   EINA_LIST_FOREACH(sd->items, l, gi)
     {
        if (gi->file == file)
          {
             if (gi->item)
               elm_gengrid_item_update(gi->item);
             break;
          }
     }
}

void
enna_grid_select_nth(Evas_Object *obj, int nth)
{

}

Eina_List *
enna_grid_files_get(Evas_Object* obj)
{
   Smart_Data *sd = evas_object_data_get(obj, "sd");
   Eina_List *files = NULL;
   Eina_List *l;
   Grid_Item *gi;

   EINA_LIST_FOREACH(sd->items, l, gi)
     files = eina_list_append(files, gi->file);

   return files;
}

int
enna_grid_jump_label(Evas_Object *obj, const char *label)
{
   return -1;
}
int
enna_grid_selected_get(Evas_Object *obj)
{
   return -1;
}

Eina_List *
enna_grid_selected_files_get(Evas_Object *obj)
{
   Eina_List *l;
   Eina_List *files = NULL;
   Grid_Item *gi;
   Smart_Data *sd = evas_object_data_get(obj, "sd");

   EINA_LIST_FOREACH(sd->items, l, gi)
     {
        if (gi->checked)
          {
             files = eina_list_append(files, enna_file_ref(gi->file));
          }
     }

   return files;
}

void *
enna_grid_selected_data_get(Evas_Object *obj)
{
   Eina_List *l;
   Grid_Item *gi;
   Smart_Data *sd = evas_object_data_get(obj, "sd");

   if (!sd->items) return NULL;

   EINA_LIST_FOREACH(sd->items,l, gi)
     {
        if ( elm_gengrid_item_selected_get (gi->item))
          {
             return gi->data;
          }
     }
   return NULL;
}

void
enna_grid_jump_ascii(Evas_Object *obj, char k)
{

}
