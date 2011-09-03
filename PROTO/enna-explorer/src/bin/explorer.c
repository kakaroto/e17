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

#include <Edje.h>
#include <Elementary.h>
#include <Eio.h>

#include "enna.h"
#include "enna_config.h"
#include "activity.h"
#include "vfs.h"
#include "mainmenu.h"
#include "view_list.h"
#include "browser.h"
#include "browser_obj.h"
#include "volumes.h"
#include "module.h"
#include "browser.h"
#include "shortcut.h"

#define ENNA_MODULE_NAME "explorer"

static void _create_menu();
static void _create_gui();

static void _browser_selected_cb(void *data,
                                 Evas_Object *obj, void *event_info);

typedef struct _Enna_Module_Explorer Enna_Module_Explorer;
typedef enum _EXPLORER_STATE EXPLORER_STATE;
typedef struct _Explorer_Item_Class_Data Explorer_Item_Class_Data;

struct _Explorer_Item_Class_Data
{
   const char *icon;
   const char *label;
};

enum _EXPLORER_STATE
  {
    BROWSER_VIEW,
    MEDIAPLAYER_VIEW
  };

struct _Enna_Module_Explorer
{
   Evas_Object *o_layout;
   Evas_Object *o_pager;
   Evas_Object *o_browser;
   Evas_Object *o_infos;
   Evas_Object *o_popup;
   Enna_Module *em;
   EXPLORER_STATE state;
   int infos_displayed;
};

static Enna_Module_Explorer *mod;


static void*
_app_exec_cb(void *data __UNUSED__, Efreet_Desktop *desktop, char *command, int remaining __UNUSED__)
{

   ecore_exe_run(command, NULL);
   return NULL;
}

static void
_browser_checked_cb(void *data __UNUSED__, Evas_Object *obj __UNUSED__, void *event_info)
{
   edje_object_signal_emit(elm_layout_edje_get(mod->o_layout),
                           "menu,show",
                           "enna");
}

static void
_browser_unchecked_cb(void *data __UNUSED__, Evas_Object *obj __UNUSED__, void *event_info)
{
   edje_object_signal_emit(elm_layout_edje_get(mod->o_layout),
                           "menu,hide",
                           "enna");
}
static void
_popup_item_clicked_cb(void *data __UNUSED__, Evas_Object *obj __UNUSED__, void *event_info)
{
   Enna_File_Action *action = data;

   printf("clicked action %s\n", action->label);

   enna_file_action_run(action);
   ENNA_OBJECT_DEL(mod->o_popup);
}


static Elm_Object_Item *
_item_new(Evas_Object *ctxpopup, const char * label, const char *icon,
          void (*func)(void *data, Evas_Object *obj, void *event_info),
          void *data)
{
   Evas_Object *ic = elm_icon_add(ctxpopup);
   elm_icon_order_lookup_set(ic, ELM_ICON_LOOKUP_THEME_FDO);
   elm_icon_standard_set(ic, icon);
   elm_icon_scale_set(ic, EINA_FALSE, EINA_FALSE);
   return elm_ctxpopup_item_append(ctxpopup, label, ic, func, data);
}

static void
_browser_longpress_cb(void *data __UNUSED__, Evas_Object *obj __UNUSED__, void *event_info)
{
   Eina_List *actions, *l;
   Enna_File_Action *action;
   Enna_File *file = event_info;
   Evas_Coord x, y;

   ENNA_OBJECT_DEL(mod->o_popup);

   mod->o_popup = elm_ctxpopup_add(mod->o_layout);
   evas_object_data_set(mod->o_popup, "file", file);

   actions = enna_file_actions_get(file);
   EINA_LIST_FOREACH(actions, l, action)
     {
        _item_new(mod->o_popup, action->label, action->icon, _popup_item_clicked_cb, action);
     }

   evas_pointer_canvas_xy_get(evas_object_evas_get(mod->o_layout), &x, &y);
   evas_object_move(mod->o_popup, x, y);
   evas_object_show(mod->o_popup);

}

static void
_browser_selected_cb(void *data __UNUSED__, Evas_Object *obj __UNUSED__, void *event_info)
{
   Enna_File *file = event_info;

   if (!file)
     return;

   if (ENNA_FILE_IS_BROWSABLE(file))
     {
        printf("Directory Selected %s\n", file->uri);
     }
   else
     {
        const char *mime;
        Eina_List *apps, *list;
        Efreet_Desktop *desk = NULL;

        mime = efreet_mime_type_get(file->uri);
        apps = efreet_util_desktop_mime_list(mime);

        EINA_LIST_FOREACH(apps, list, desk)
          {
             Eina_List *files = NULL;
             files = eina_list_append(files, file->mrl);

             efreet_desktop_command_get(desk, files,
                                        _app_exec_cb, NULL);
             break;
          }

     }
}

static void
_create_menu()
{
   const char *view_type;
   const char *start;
   /* Set default state */
   mod->state = BROWSER_VIEW;

   /* Create List */
   ENNA_OBJECT_DEL(mod->o_browser);

   mod->o_browser = enna_browser_obj_add(mod->o_layout, NULL);

   view_type = elm_layout_data_get(mod->o_layout, "view");

   if (view_type && !strcmp(view_type, "grid"))
     enna_browser_obj_view_type_set(mod->o_browser, ENNA_BROWSER_VIEW_GRID);
   else
     enna_browser_obj_view_type_set(mod->o_browser, ENNA_BROWSER_VIEW_LIST);

   enna_browser_obj_root_set(mod->o_browser, "/explorer/localfiles/");
   start = eina_stringshare_printf("/explorer/localfiles/Root/%s", enna->start_path);
   printf("start %s\n", start);
   enna_browser_obj_uri_set(mod->o_browser, start);
   eina_stringshare_del(start);

   evas_object_smart_callback_add(mod->o_browser, "selected",
                                  _browser_selected_cb, NULL);
   evas_object_smart_callback_add(mod->o_browser, "checked",
                                  _browser_checked_cb, NULL);
   evas_object_smart_callback_add(mod->o_browser, "unchecked",
                                  _browser_unchecked_cb, NULL);
   evas_object_smart_callback_add(mod->o_browser, "longpress",
                                  _browser_longpress_cb, NULL);

   elm_layout_content_set(mod->o_layout, "browser.swallow", mod->o_browser);
}

/* static Eina_Bool */
/* _delete_dir_filter_cb(void *data, Eio_File *handler, const Eina_File_Direct_Info *info) */
/* { */
/*    return EINA_TRUE; */
/* } */

/* static void */
/* _delete_dir_progress_cb(void *data, Eio_File *handler, const Eio_Progress *info) */
/* { */
/* } */

/* static void */
/* _delete_done_cb(void *data, Eio_File *handler) */
/* { */
/* } */

/* static void */
/* _delete_error_cb(void *data, Eio_File *handler, int error) */
/* { */
/* } */




/* static void */
/* _toolbar_delete_cb(void *data, Evas_Object *obj, void *event_info __UNUSED__) */
/* { */

/*    Evas_Object *win; */
/*    Evas_Object *bg; */
/*    Evas_Object *fr; */
/*    Evas_Object *lb; */
/*    Evas_Object *bx; */
/*    Evas_Object *btn_bx; */
/*    Evas_Object *btn_ok; */
/*    Evas_Object *btn_cancel; */

/*    win = elm_win_add(enna->win, NULL, ELM_WIN_DIALOG_BASIC); */
/*    elm_win_title_set(win, "Delete files"); */
/*    elm_win_autodel_set(win, EINA_TRUE); */

/*    bg = elm_bg_add(win); */
/*    evas_object_size_hint_weight_set(bg, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND); */
/*    elm_win_resize_object_add(win, bg); */
/*    evas_object_show(bg); */
/*    evas_object_size_hint_min_set(bg, 400, 64); */

/*    fr = elm_frame_add(win); */
/*    elm_object_style_set(fr, "pad_medium"); */
/*    evas_object_show(fr); */
/*    elm_win_resize_object_add(win, fr); */

/*    bx = elm_box_add(win); */
/*    evas_object_size_hint_weight_set(bx, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND); */
/*    evas_object_size_hint_align_set(bx, -1, -1); */
/*    evas_object_show(bx); */
/*    elm_frame_content_set(fr, bx); */
/*    elm_box_padding_set(bx, 4, 4); */

/*    lb = elm_label_add(win); */
/*    elm_object_text_set(lb, "Comfirm Delete ?"); */
/*    evas_object_size_hint_weight_set(lb, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND); */
/*    evas_object_size_hint_align_set(lb, 0.5, -1); */
/*    evas_object_show(lb); */
/*    elm_box_pack_end(bx, lb); */

/*    btn_bx = elm_box_add(win); */
/*    elm_box_horizontal_set(btn_bx, EINA_TRUE); */
/*    evas_object_size_hint_weight_set(btn_bx, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND); */
/*    evas_object_size_hint_align_set(btn_bx, EVAS_HINT_FILL, 0.5); */
/*    evas_object_show(btn_bx); */
/*    elm_box_padding_set(btn_bx, 8, 2); */

/*    btn_ok = elm_button_add(win); */
/*    elm_object_text_set(btn_ok, "Delete"); */
/*    evas_object_show(btn_ok); */
/*    evas_object_size_hint_weight_set(btn_ok, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND); */
/*    evas_object_size_hint_align_set(btn_ok, EVAS_HINT_FILL, 0.5); */
/*    elm_box_pack_end(btn_bx, btn_ok); */
/*    evas_object_smart_callback_add(btn_ok, "clicked", */
/*                                   _dialog_delete_ok_clicked_cb, win); */

/*    btn_cancel = elm_button_add(win); */
/*    elm_object_text_set(btn_cancel, "Cancel"); */
/*    evas_object_show(btn_cancel); */
/*    evas_object_size_hint_weight_set(btn_cancel, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND); */
/*    evas_object_size_hint_align_set(btn_cancel, EVAS_HINT_FILL, EVAS_HINT_FILL); */
/*    elm_box_pack_end(btn_bx, btn_cancel); */
/*    evas_object_smart_callback_add(btn_cancel, "clicked", */
/*                                   _dialog_delete_cancel_clicked_cb, win); */

/*    elm_box_pack_end(bx, btn_bx); */

/*    evas_object_resize(win, 400, 128); */

/*    evas_object_show(win); */

/* } */

static void
_toolbar_create_folder_cb(void *data, Evas_Object *obj, void *event_info __UNUSED__)
{
   Evas_Object *win;
   Evas_Object *bg;
   Evas_Object *fr;
   Evas_Object *en;
   Evas_Object *lb;
   Evas_Object *bx;
   Evas_Object *btn_bx;
   Evas_Object *btn_ok;
   Evas_Object *btn_cancel;

   win = elm_win_add(enna->win, NULL, ELM_WIN_DIALOG_BASIC);
   elm_win_title_set(win, "New Folder");
   elm_win_autodel_set(win, EINA_TRUE);

   bg = elm_bg_add(win);
   evas_object_size_hint_weight_set(bg, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   elm_win_resize_object_add(win, bg);
   evas_object_show(bg);
   evas_object_size_hint_min_set(bg, 400, 64);

   fr = elm_frame_add(win);
   elm_object_style_set(fr, "pad_medium");
   evas_object_show(fr);
   elm_win_resize_object_add(win, fr);

   bx = elm_box_add(win);
   evas_object_size_hint_weight_set(bx, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   evas_object_size_hint_align_set(bx, -1, -1);
   evas_object_show(bx);
   elm_frame_content_set(fr, bx);
   elm_box_padding_set(bx, 4, 4);

   lb = elm_label_add(win);
   elm_object_text_set(lb, "Enter folder name");
   evas_object_size_hint_weight_set(lb, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   evas_object_size_hint_align_set(lb, 0.5, -1);
   evas_object_show(lb);
   elm_box_pack_end(bx, lb);

   en = elm_entry_add(win);
   elm_entry_single_line_set(en, EINA_TRUE);
   evas_object_size_hint_weight_set(en, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   evas_object_size_hint_align_set(en, -1, -1);
   elm_box_pack_end(bx, en);
   evas_object_show(en);

   btn_bx = elm_box_add(win);
   elm_box_horizontal_set(btn_bx, EINA_TRUE);
   evas_object_size_hint_weight_set(btn_bx, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   evas_object_size_hint_align_set(btn_bx, EVAS_HINT_FILL, 0.5);
   evas_object_show(btn_bx);
   elm_box_padding_set(btn_bx, 8, 2);

   btn_ok = elm_button_add(win);
   elm_object_text_set(btn_ok, "OK");
   evas_object_show(btn_ok);
   evas_object_size_hint_weight_set(btn_ok, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   evas_object_size_hint_align_set(btn_ok, EVAS_HINT_FILL, 0.5);
   elm_box_pack_end(btn_bx, btn_ok);

   btn_cancel = elm_button_add(win);
   elm_object_text_set(btn_cancel, "Cancel");
   evas_object_show(btn_cancel);
   evas_object_size_hint_weight_set(btn_cancel, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   evas_object_size_hint_align_set(btn_cancel, EVAS_HINT_FILL, EVAS_HINT_FILL);
   elm_box_pack_end(btn_bx, btn_cancel);

   elm_box_pack_end(bx, btn_bx);

   evas_object_show(win);
}

static void
_create_gui()
{
   Evas_Object *tb;
   Evas_Object *shortcut;

   /* Set default state */
   mod->state = BROWSER_VIEW;

   /* Create main edje object */
   mod->o_layout = elm_layout_add(enna->layout);
   elm_layout_file_set(mod->o_layout, enna_config_theme_get(), "activity/explorer");
   elm_object_text_part_set(mod->o_layout, "title", "Explorer");

   tb = elm_toolbar_add(mod->o_layout);
   elm_object_style_set(tb, "transparent");
   elm_toolbar_homogeneous_set(tb, EINA_TRUE);
   elm_toolbar_mode_shrink_set(tb, ELM_TOOLBAR_SHRINK_SCROLL);
   evas_object_size_hint_weight_set(tb, 0.0, 0.0);
   evas_object_size_hint_align_set(tb, EVAS_HINT_FILL, 0.0);
   evas_object_show(tb);
   /* elm_toolbar_item_append(tb, "folder-new", "New", _toolbar_create_folder_cb, NULL); */
   elm_toolbar_item_append(tb, "edit-copy", "Copy", NULL, NULL);
   elm_toolbar_item_append(tb, "edit-cut", "Cut", NULL, NULL);
   elm_toolbar_item_append(tb, "edit-paste", "Paste", NULL, NULL);
   /* elm_toolbar_item_append(tb, "edit-delete", "Delete", _toolbar_delete_cb, NULL); */
   elm_layout_content_set(mod->o_layout, "enna.menu.swallow", tb);
   _create_menu();

   shortcut = enna_shortcut_add(mod->o_layout);
   evas_object_show(shortcut);
   elm_layout_content_set(mod->o_layout, "panel.swallow", shortcut);

}



/****************************************************************************/
/*                         Public Module API                                */
/****************************************************************************/
static Enna_Class_Activity class =
  {
    ENNA_MODULE_NAME,
    1,
    "Explorer",
    NULL,
    "icon/music",
    "background/music",
    ENNA_CAPS_ALL,
    {
      NULL,
      NULL,
      NULL,
      NULL,
      NULL
    }
  };

void
enna_explorer_init(void)
{
   Evas_Object *o_edje;

   enna_activity_register(&class);

   mod  = calloc(1, sizeof(Enna_Module_Explorer));
   _create_gui();
   o_edje = elm_layout_edje_get(mod->o_layout);


   edje_object_signal_emit(o_edje, "module,show", "enna");
   edje_object_signal_emit(o_edje, "content,show", "enna");

   o_edje = elm_layout_edje_get(enna->layout);

   elm_layout_content_set(enna->layout, "enna.content.swallow", mod->o_layout);
}

void
enna_explorer_shutdown(void)
{
   evas_object_smart_callback_del(mod->o_browser,
                                  "selected", _browser_selected_cb);
   evas_object_smart_callback_del(mod->o_browser,
                                  "checked", _browser_checked_cb);
   evas_object_smart_callback_del(mod->o_browser,
                                  "unchecked", _browser_unchecked_cb);
   evas_object_smart_callback_del(mod->o_browser,
                                  "longpress", _browser_longpress_cb);

   ENNA_OBJECT_DEL(mod->o_pager);
   ENNA_OBJECT_DEL(mod->o_layout);
   free(mod);
}


