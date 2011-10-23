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
#include <Eio.h>

#include "vfs.h"
#include "view_list.h"
#include "view_grid.h"
#include "enna_config.h"
#include "browser.h"
#include "browser_obj.h"
#include "browser_bar.h"

typedef struct _Smart_Data Smart_Data;
typedef struct _Activated_Cb_Data Activated_Cb_Data;


struct _Activated_Cb_Data
{
   Smart_Data *sd;
   Enna_File *file;
};

struct _Smart_Data
{
   Evas_Object *o_layout;
   Evas_Object *o_pager;
   Evas_Object *o_view;
   Evas_Object *o_popup;
   Evas_Object *o_dialog;
   Enna_Browser_View_Type view_type;
   Ecore_Timer *hilight_timer;
   Enna_Browser *browser;
   Evas_Object *o_header;
   Enna_File *root;
   Enna_File *file;
   const char *root_uri;
   Eina_List *visited;
   const char *style;
   struct
   {
      Evas_Object *(*view_add)(Smart_Data *sd);
      void (*view_append)(Evas_Object *view,
                          Enna_File *file,
                          void (*func_activated)(void *data),
                          void *data);
      void (*view_remove)(Evas_Object *view,
                          Enna_File *file);
      void (*view_update)(Evas_Object *view,
                          Enna_File *file);
      void *(*view_selected_data_get)(Evas_Object *view);
      int (*view_jump_label)(Evas_Object *view, const char *label);
      void (*view_select_nth)(Evas_Object *obj, int nth);
      Eina_List *(*view_files_get)(Evas_Object *obj);
      void (*view_jump_ascii)(Evas_Object *obj, char k);
      Eina_List *(*view_selected_files_get)(Evas_Object *obj);
   } view_funcs;
};

static void _browse_back(Smart_Data *sd);
static void _browse(Smart_Data *sd, Enna_File *file, Eina_Bool back);
static void _del_cb(void *data, Enna_File *file);

static Eina_Bool
_view_delay_hilight_cb(void *data)
{
   Smart_Data *sd = data;
   Activated_Cb_Data *cb_data = sd->view_funcs.view_selected_data_get(sd->o_view);

   if (cb_data)
     evas_object_smart_callback_call (sd->o_layout, "delay,hilight", cb_data->file);

   sd->hilight_timer = NULL;
   return 0;
}

static void
_view_hilight_cb (void *data, Evas_Object *obj __UNUSED__, void *event_info __UNUSED__)
{
   Smart_Data *sd = data;

   evas_object_smart_callback_call (sd->o_layout, "hilight", NULL);
   ENNA_TIMER_DEL(sd->hilight_timer);
   sd->hilight_timer = ecore_timer_add(0.4, _view_delay_hilight_cb, sd);
}

static void
_view_checked_cb (void *data, Evas_Object *obj __UNUSED__, void *event_info)
{
   Smart_Data *sd = data;
   printf("browser obj Checked\n");
   evas_object_smart_callback_call (sd->o_layout, "checked", event_info);
}

static void
_view_unchecked_cb (void *data, Evas_Object *obj __UNUSED__, void *event_info)
{
   Smart_Data *sd = data;
   printf("browser obj unChecked\n");
   evas_object_smart_callback_call (sd->o_layout, "unchecked", event_info);
}



static void
_view_longpressed_cb (void *data, Evas_Object *obj __UNUSED__, void *event_info)
{
   Enna_File *file = event_info;
   Smart_Data *sd = data;

   evas_object_smart_callback_call (sd->o_layout, "longpress", file);
}

static Evas_Object *
_browser_view_list_add(Smart_Data *sd)
{
   Evas_Object *view;

   if (!sd) return NULL;

   view = enna_list_add(sd->o_layout);

   elm_pager_content_push(sd->o_pager, view);
   evas_object_smart_callback_add(view, "hilight", _view_hilight_cb, sd);
   evas_object_smart_callback_add(view, "checked", _view_checked_cb, sd);
   evas_object_smart_callback_add(view, "unchecked", _view_unchecked_cb, sd);
   evas_object_smart_callback_add(view, "longpress", _view_longpressed_cb, sd);
   /* View */
   edje_object_signal_emit(view, "list,right,now", "enna");
   return view;
}


static Evas_Object *
_browser_view_grid_add(Smart_Data *sd)
{
   Evas_Object *view;

   if (!sd) return NULL;

   view = enna_grid_add(sd->o_layout);

   elm_pager_content_push(sd->o_pager, view);
   evas_object_smart_callback_add(view, "hilight", _view_hilight_cb, sd);
   evas_object_smart_callback_add(view, "checked", _view_checked_cb, sd);
   evas_object_smart_callback_add(view, "unchecked", _view_unchecked_cb, sd);
   evas_object_smart_callback_add(view, "longpress", _view_longpressed_cb, sd);
   /* View */
   edje_object_signal_emit(view, "list,right,now", "enna");
   return view;
}



static void
_change_view(Smart_Data *sd, Enna_Browser_View_Type view_type)
{
   sd->view_type = view_type;
   switch(sd->view_type)
     {
      case ENNA_BROWSER_VIEW_LIST:
         sd->view_funcs.view_add                 = _browser_view_list_add;
         sd->view_funcs.view_append              = enna_list_file_append;
         sd->view_funcs.view_remove              = enna_list_file_remove;
         sd->view_funcs.view_update              = enna_list_file_update;
         sd->view_funcs.view_selected_data_get   = enna_list_selected_data_get;
         sd->view_funcs.view_jump_label          = enna_list_jump_label;
         sd->view_funcs.view_select_nth          = enna_list_select_nth;
         sd->view_funcs.view_files_get           = enna_list_files_get;
         sd->view_funcs.view_jump_ascii          = enna_list_jump_ascii;
         sd->view_funcs.view_selected_files_get  = enna_list_selected_files_get;
         break;
      case ENNA_BROWSER_VIEW_GRID:
         sd->view_funcs.view_add                 = _browser_view_grid_add;
         sd->view_funcs.view_append              = enna_grid_file_append;
         sd->view_funcs.view_remove              = enna_grid_file_remove;
         sd->view_funcs.view_update              = enna_grid_file_update;
         sd->view_funcs.view_selected_data_get   = enna_grid_selected_data_get;
         sd->view_funcs.view_jump_label          = enna_grid_jump_label;
         sd->view_funcs.view_select_nth          = enna_grid_select_nth;
         sd->view_funcs.view_files_get           = enna_grid_files_get;
         sd->view_funcs.view_jump_ascii          = enna_grid_jump_ascii;
         sd->view_funcs.view_selected_files_get  = enna_grid_selected_files_get;
         break;
      default:
         break;
     }
}

static void
_activated_cb(void *data)
{
   Activated_Cb_Data *cb_data = data;

   _browse(cb_data->sd, cb_data->file, EINA_FALSE);
}

static void
_add_cb(void *data, Enna_File *file)
{
   Smart_Data *sd = data;
   Activated_Cb_Data *cb_data;

   if (!sd->o_view)
     sd->o_view = sd->view_funcs.view_add(sd);

   if (!file)
     {
        /* No File detected */
     }

   cb_data = malloc(sizeof(Activated_Cb_Data));
   cb_data->sd = sd;
   cb_data->file = enna_file_ref(file); /* FIXME this reference is definitively LOST !!!! */
   sd->view_funcs.view_append(sd->o_view, file, _activated_cb, cb_data);

}

static void
_del_cb(void *data, Enna_File *file)
{
   Smart_Data *sd = data;

   if (file && sd && sd->o_view && sd->view_funcs.view_remove)
     sd->view_funcs.view_remove(sd->o_view, file);
}

static void
_update_cb(void *data, Enna_File *file)
{
   Smart_Data *sd = data;

   if (file && sd && sd->o_view && sd->view_funcs.view_update)
     sd->view_funcs.view_update(sd->o_view, file);
}

static void
_back_btn_clicked_cb(void *data, Evas_Object *obj __UNUSED__, void *event_info __UNUSED__)
{
   Smart_Data *sd = data;

   if (!sd)
     return;

   _browse_back(sd);
}

static void
_add_header(Smart_Data *sd, Enna_File *file)
{
   Evas_Object *o_layout;
   Evas_Object *o_edje;
   Evas_Object *o_back_btn;
   Evas_Object *o_ic;

   ENNA_OBJECT_DEL(sd->o_header);

   o_layout = elm_layout_add(sd->o_layout);
   elm_layout_file_set(o_layout, enna_config_theme_get(), "enna/browser/header");

   o_back_btn = elm_button_add(o_layout);
   o_ic = elm_icon_add(sd->o_layout);
   elm_icon_file_set(o_ic, enna_config_theme_get(), "icon/arrow_left");
   elm_button_icon_set(o_back_btn, o_ic);
   evas_object_size_hint_min_set(o_ic, 32, 32);
   evas_object_show(o_ic);

   evas_object_smart_callback_add(o_back_btn, "clicked", _back_btn_clicked_cb, sd);

   evas_object_size_hint_align_set(o_back_btn, EVAS_HINT_FILL, EVAS_HINT_FILL);
   evas_object_size_hint_weight_set(o_back_btn, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);

   elm_layout_content_set(o_layout, "enna.swallow.back", o_back_btn);

   o_ic = elm_icon_add(o_layout);
   if (!file)
     elm_icon_file_set(o_ic, enna_config_theme_get(), "icon/home");
   else
     elm_icon_file_set(o_ic, enna_config_theme_get(), file->icon);
   elm_layout_content_set(o_layout, "enna.swallow.icon", o_ic);

   o_edje = elm_layout_edje_get(o_layout);
   if (file)
     edje_object_part_text_set(o_edje, "enna.text.current", file->label);
   else
     edje_object_part_text_set(o_edje, "enna.text.current", "Main Menu");

   evas_object_size_hint_align_set(o_layout, EVAS_HINT_FILL, EVAS_HINT_FILL);
   evas_object_size_hint_weight_set(o_layout, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);

   elm_layout_content_set(sd->o_layout, "enna.swallow.header", o_layout);

   sd->o_header = o_layout;
}


static void
_view_mode_changed_cb(void *data, Evas_Object *obj, void *event_info)
{
    Smart_Data *sd = data;

    printf("Mode change : %s\n", (char*)event_info);

    if (!strcmp(event_info, "grid"))
        _change_view(sd, ENNA_BROWSER_VIEW_GRID);
    else
        _change_view(sd, ENNA_BROWSER_VIEW_LIST);

    _browse(sd, sd->file, EINA_FALSE);
}

static void
_up_clicked_cb(void *data, Evas_Object *obj, void *event_info)
{
    Smart_Data *sd = data;

    _browse_back(sd);
}

static void
_next_clicked_cb(void *data, Evas_Object *obj, void *event_info)
{
    Smart_Data *sd = data;

    _browse_back(sd);
}

static void
_previous_clicked_cb(void *data, Evas_Object *obj, void *event_info)
{
    Smart_Data *sd = data;

    _browse_back(sd);
}

static void
_path_changed_cb(void *data, Evas_Object *obj, void *event_info)
{
    Smart_Data *sd = data;
    const char *path = event_info;
    char uri[4096];
    Enna_File *file;

    if (!path)
        return;

    if (strlen(path) >= 2 && path[0] == '~')
        snprintf(uri, sizeof(uri), "file://%s/%s", getenv("HOME"), path + 2);
    else if (strlen(path) == 1 && path[0] == '~')
        snprintf(uri, sizeof(uri), "file://%s/%s", getenv("HOME"), path + 1);
    else
        snprintf(uri, sizeof(uri), "file://%s", path);
    file = enna_file_from_uri_new(uri);

    if (file)
       _browse(sd, file, EINA_TRUE);
}


static void
_browse(Smart_Data *sd, Enna_File *file, Eina_Bool back)
{
   if (!sd)
     return;

   if (!ENNA_FILE_IS_BROWSABLE(file))
     {
        evas_object_smart_callback_call (sd->o_layout, "selected", file);
        return;
     }

   if (file != sd->file)
     {
        enna_file_free(sd->file);
        sd->file = enna_file_ref(file);
     }

   if (file && !back)
     sd->visited = eina_list_append(sd->visited, enna_file_ref(file));
   enna_browser_del(sd->browser);

   sd->browser = enna_browser_add(_add_cb, sd, _del_cb, sd, _update_cb, sd, file->uri);

   if (!back)
     elm_object_style_set(sd->o_pager, "slide_invisible");
   else
     elm_object_style_set(sd->o_pager, "slide_invisible_back");
   elm_pager_content_pop(sd->o_pager);

   evas_object_smart_callback_del(sd->o_view, "hilight", _view_hilight_cb);

   if (sd->hilight_timer)
     ecore_timer_del(sd->hilight_timer);
   sd->hilight_timer = NULL;
   sd->o_view = NULL;

   enna_browser_bar_file_set(sd->o_header, sd->file);
   enna_browser_browse(sd->browser);
}

static void
_browse_back(Smart_Data *sd)
{
   Enna_File *cur;
   Enna_File *prev;

   prev = enna_file_from_uri_new(ecore_file_dir_get(sd->file->uri));
   if (prev)
       _browse(sd, prev, EINA_TRUE);
   else
       evas_object_smart_callback_call (sd->o_layout, "root", NULL);

}

Eina_List *
enna_browser_obj_files_get(Evas_Object *obj)
{
   Smart_Data *sd = evas_object_data_get(obj, "sd");

   if (!sd->browser)
     return NULL;
   else return
          enna_browser_files_get(sd->browser);
}

Eina_List *
enna_browser_obj_selected_files_get(Evas_Object *obj)
{
   Smart_Data *sd = evas_object_data_get(obj, "sd");

   if (!sd->browser)
     return NULL;
   else
     return sd->view_funcs.view_selected_files_get(sd->o_view);
}

void
enna_browser_obj_view_type_set(Evas_Object *obj,
                               Enna_Browser_View_Type view_type)
{
   Smart_Data *sd = evas_object_data_get(obj, "sd");

   _change_view(sd, view_type);
}

void
enna_browser_obj_root_set(Evas_Object *obj, const char *uri)
{
   Smart_Data *sd = evas_object_data_get(obj, "sd");
   if (sd->root)
     enna_file_free(sd->root);
   sd->root_uri = eina_stringshare_add(uri);
   sd->root = enna_file_menu_add("main_menu", uri, "Main Menu", "icon/home");
}

void
enna_browser_obj_uri_set(Evas_Object *obj, const char *uri)
{
   Smart_Data *sd = evas_object_data_get(obj, "sd");
   Enna_File *f;

   f =  enna_file_from_uri_new(uri);
   _browse(sd, f, EINA_FALSE);
}


static void
_browser_del_cb(void *data, Evas *e __UNUSED__, Evas_Object *o __UNUSED__, void *event_info __UNUSED__)
{
   Smart_Data *sd = data;
   Enna_File *f;

   ENNA_OBJECT_DEL(sd->o_view);
   if (sd->hilight_timer)
     ecore_timer_del(sd->hilight_timer);
   enna_browser_del(sd->browser);
   ENNA_OBJECT_DEL(sd->o_header);
   enna_file_free(sd->root);
   enna_file_free(sd->file);
   EINA_LIST_FREE(sd->visited, f)
     enna_file_free(f);
   eina_stringshare_del(sd->style);

   ENNA_FREE(sd);
}

Evas_Object *
enna_browser_obj_add(Evas_Object *parent, const char *style)
{
   Smart_Data *sd;

   sd = calloc(1, sizeof(Smart_Data));

   sd->style = eina_stringshare_add(style);

   sd->o_layout = elm_layout_add(parent);
   evas_object_size_hint_weight_set(sd->o_layout, -1.0, -1.0);
   evas_object_size_hint_align_set(sd->o_layout, 1.0, 1.0);
   elm_layout_file_set(sd->o_layout, enna_config_theme_get(), "enna/browser");


   sd->o_header = enna_browser_bar_add(sd->o_layout, sd->file);
   evas_object_show(sd->o_header);
   elm_layout_content_set(sd->o_layout, "enna.swallow.header", sd->o_header);
   evas_object_smart_callback_add(sd->o_header, "mode,changed", _view_mode_changed_cb, sd);
   evas_object_smart_callback_add(sd->o_header, "up,clicked", _up_clicked_cb, sd);
   evas_object_smart_callback_add(sd->o_header, "next,clicked", _next_clicked_cb, sd);
   evas_object_smart_callback_add(sd->o_header, "previous,clicked", _previous_clicked_cb, sd);
   evas_object_smart_callback_add(sd->o_header, "path,changed", _path_changed_cb, sd);

   sd->o_pager = elm_pager_add(sd->o_layout);
   evas_object_show(sd->o_pager);
   elm_object_style_set(sd->o_pager, "slide_invisible");
   elm_layout_content_set(sd->o_layout, "enna.swallow.content", sd->o_pager);

   evas_object_data_set(sd->o_layout, "sd", sd);
   evas_object_event_callback_add(sd->o_layout, EVAS_CALLBACK_DEL, _browser_del_cb, sd);
   enna_browser_obj_view_type_set(sd->o_layout, ENNA_BROWSER_VIEW_LIST);

   return sd->o_layout;
}


