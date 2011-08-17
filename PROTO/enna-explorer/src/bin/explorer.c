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
_browser_selected_cb(void *data __UNUSED__, Evas_Object *obj __UNUSED__, void *event_info)
{
   Enna_File *file = event_info;
   //Eina_List *files = enna_browser_obj_files_get(mod->o_browser);

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
   /* Set default state */
   mod->state = BROWSER_VIEW;

   /* Create List */
   ENNA_OBJECT_DEL(mod->o_browser);

   mod->o_browser = enna_browser_obj_add(mod->o_layout, NULL);
   enna_browser_obj_view_type_set(mod->o_browser, ENNA_BROWSER_VIEW_LIST);
   enna_browser_obj_root_set(mod->o_browser, "/explorer");

   evas_object_smart_callback_add(mod->o_browser, "selected",
                                  _browser_selected_cb, NULL);

   elm_layout_content_set(mod->o_layout, "browser.swallow", mod->o_browser);
}

static Eina_Bool
_delete_dir_filter_cb(void *data, Eio_File *handler, const Eina_File_Direct_Info *info)
{
   return EINA_TRUE;
}

static void
_delete_dir_progress_cb(void *data, Eio_File *handler, const Eio_Progress *info)
{
}

static void
_delete_done_cb(void *data, Eio_File *handler)
{
}

static void
_delete_error_cb(void *data, Eio_File *handler, int error)
{
}

static void
_toolbar_delete_cb(void *data, Evas_Object *obj, void *event_info __UNUSED__)
{
   Eina_List *files;
   Enna_File *file;

   files = enna_browser_obj_selected_files_get(mod->o_browser);

   EINA_LIST_FREE(files, file)
     {
        if (file->type == ENNA_FILE_DIRECTORY)
          {
             eio_dir_unlink(file->mrl,
                            _delete_dir_filter_cb,
                            _delete_dir_progress_cb,
                            _delete_done_cb,
                            _delete_error_cb,
                            NULL);
          }
        else if (file->type == ENNA_FILE_FILE)
          {
             eio_file_unlink(file->mrl,
                             _delete_done_cb,
                             _delete_error_cb,
                             NULL);
          }
     }
}

static void
_create_gui()
{
   Evas_Object *tb;

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
   elm_toolbar_item_append(tb, "folder-new", "New", NULL, NULL);
   elm_toolbar_item_append(tb, "edit-copy", "Copy", NULL, NULL);
   elm_toolbar_item_append(tb, "edit-cut", "Cut", NULL, NULL);
   elm_toolbar_item_append(tb, "edit-paste", "Paste", NULL, NULL);
   elm_toolbar_item_append(tb, "edit-delete", "Delete", _toolbar_delete_cb, NULL);
   elm_layout_content_set(enna->layout, "enna.mainmenu.swallow", tb);
   _create_menu();
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

   ENNA_OBJECT_DEL(mod->o_pager);
   ENNA_OBJECT_DEL(mod->o_layout);
   free(mod);
}


