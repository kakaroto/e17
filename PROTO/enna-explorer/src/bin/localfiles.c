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
#include <sys/statfs.h>

#include <Ecore.h>
#include <Ecore_File.h>
#include <Eio.h>


#include "enna.h"
#include "enna_config.h"
#include "vfs.h"
#include "utils.h"
#include "buffer.h"
#include "exec.h"

#include "gettext.h"

typedef struct _Root_Directories
{
   const char *name;
   char *uri;
   char *label;
   char *icon;
} Root_Directories;

typedef struct _Module_Config
{
   Eina_List *root_directories;
} Module_Config;

typedef struct _Class_Private_Data
{
   const char *name;
   const char *uri;
   const char *prev_uri;
   Module_Config *config;
} Class_Private_Data;

typedef struct _Enna_Module_LocalFiles
{
   Evas *e;
   Class_Private_Data *all;

} Enna_Module_LocalFiles;

typedef struct localfiles_path_s {
   char *uri;
   char *label;
   char *icon;
} localfiles_path_t;

typedef struct localfiles_cfg_s {
   Eina_List *path_all;
   Eina_List *path_music;
   Eina_List *path_video;
   Eina_List *path_photo;
   Eina_Bool home;
} localfiles_cfg_t;

typedef struct _Enna_Localfiles_Priv
{
   Eio_File *file;
   ENNA_VFS_CAPS caps;
   const char *root;
   const char *relative_path;
   Enna_Browser *browser;
   /* Eio_Monitor *monitor; */
   /* Eina_List *monitor_handlers; */
   Evas_Object *dialog;
   Enna_File *file_dialog;
   const char *new_path;
} Enna_Localfiles_Priv;

static localfiles_cfg_t localfiles_cfg;
static Enna_Module_LocalFiles *mod;

static void
__class_init(const char *name, Class_Private_Data **priv,
             ENNA_VFS_CAPS caps, char *key)
{
   Class_Private_Data *data;
   Root_Directories *root;
   char buf[PATH_MAX + 7];
   Eina_List *path_list;
   Eina_List *l;

   data = calloc(1, sizeof(Class_Private_Data));
   *priv = data;

   data->prev_uri = NULL;
   data->name = eina_stringshare_add(name);
   data->config = calloc(1, sizeof(Module_Config));
   data->config->root_directories = NULL;

   switch (caps)
     {
      case ENNA_CAPS_ALL:
         path_list = localfiles_cfg.path_all;
         break;
      case ENNA_CAPS_MUSIC:
         path_list = localfiles_cfg.path_music;
         break;
      case ENNA_CAPS_VIDEO:
         path_list = localfiles_cfg.path_video;
         break;
      case ENNA_CAPS_PHOTO:
         path_list = localfiles_cfg.path_photo;
         break;
      default:
         return;
     }

   for (l = path_list; l; l = l->next)
     {
        localfiles_path_t *p;

        p = l->data;

        root        = calloc(1, sizeof(Root_Directories));
        root->name  = eina_stringshare_add(p->label);
        root->uri   = strdup(p->uri);
        root->label = strdup(p->label);
        root->icon  = strdup(p->icon);

        data->config->root_directories =
          eina_list_append(data->config->root_directories, root);
     }

   // add home directory entry
   root = ENNA_NEW(Root_Directories, 1);
   snprintf(buf, sizeof(buf), "file://%s", enna_util_user_home_get());
   root->name = eina_stringshare_add("Home");
   root->uri = strdup(buf);
   root->label = strdup("Home");
   root->icon = strdup("icon/home");

   data->config->root_directories =
       eina_list_append(data->config->root_directories, root);

   // add Root (/) directory entry
   root = ENNA_NEW(Root_Directories, 1);
   snprintf(buf, sizeof(buf), "file:///");
   root->name = eina_stringshare_add("Root");
   root->uri = strdup(buf);
   root->label = strdup("Root");
   root->icon = strdup("icon/root");

   data->config->root_directories =
       eina_list_append(data->config->root_directories, root);

}

static void *
_add(const char *uri, Enna_Browser *browser, ENNA_VFS_CAPS caps)
{

   Enna_Localfiles_Priv *priv;

   priv = calloc(1, sizeof(Enna_Localfiles_Priv));
   priv->caps = caps;
   priv->browser = browser;

   return priv;
}

const char*
make_human_readable_str(unsigned long long val,
                        unsigned long block_size, unsigned long display_unit)
{
   static const char unit_chars[]  = {
     '\0', 'K', 'M', 'G', 'T', 'P', 'E', 'Z', 'Y'
   };

   static char *str;

   unsigned frac; /* 0..9 - the fractional digit */
   const char *u;
   const char *fmt;

   if (val == 0)
     return "0";

   fmt = "%llu";
   if (block_size > 1)
     val *= block_size;
   frac = 0;
   u = unit_chars;

   if (display_unit) {
      val += display_unit/2;	/* Deal with rounding */
      val /= display_unit;	/* Don't combine with the line above! */
      /* will just print it as ulonglong (below) */
   } else {
      while ((val >= 1024)
             /* && (u < unit_chars + sizeof(unit_chars) - 1) - always true */
             ) {
         fmt = "%llu.%u%c";
         u++;
         frac = (((unsigned)val % 1024) * 10 + 1024/2) / 1024;
         val /= 1024;
      }
      if (frac >= 10) { /* we need to round up here */
         ++val;
         frac = 0;
      }
#if 1
      /* If block_size is 0, dont print fractional part */
      if (block_size == 0) {
         if (frac >= 5) {
            ++val;
         }
         fmt = "%llu%*c";
         frac = 1;
      }
#endif
   }

   if (!str) {
      /* sufficient for any width of val */
      str = malloc(sizeof(val)*3 + 2 + 3);
   }
   sprintf(str, fmt, val, frac, *u);
   return str;
}

/* static const char * */
/* _root_meta_get(void *data, Enna_File *file, const char *key) */
/* { */
/*    Root_Directories *root = data; */
/*    struct statfs st; */

/*    if (!root) */
/*      return NULL; */

/*    if (!strcmp(key, ENNA_META_KEY_SIZE)) */
/*      { */

/*         if (statfs(root->uri+7, &st) != 0) */
/*           { */
/*              return NULL; */
/*           } */

/*         return */
/*           eina_stringshare_printf("%s",  make_human_readable_str( */
/*                                                                  st.f_blocks, st.f_bsize, 0)); */
/*      } */
/*    else if (!strcmp(key, ENNA_META_KEY_USESPACE)) */
/*      { */
/*         if (statfs(root->uri+7, &st) != 0) */
/*           { */
/*              return NULL; */
/*           } */
/*         return */
/*           eina_stringshare_printf("%s",make_human_readable_str( */
/*                                                                (st.f_blocks - st.f_bfree), */
/*                                                                st.f_bsize, 0)); */
/*      } */
/*    else if (!strcmp(key, ENNA_META_KEY_FREESPACE)) */
/*      { */
/*         if (statfs(root->uri+7, &st) != 0) */
/*           { */
/*              return NULL; */
/*           } */
/*         return */
/*           eina_stringshare_printf("%s", make_human_readable_str(st.f_bavail, */
/*                                                                 st.f_bsize, 0)); */
/*      } */
/*    else if (!strcmp(key, ENNA_META_KEY_PERCENT_USED)) */
/*      { */
/*         unsigned long blocks_used; */
/*         unsigned blocks_percent_used; */

/*         if (statfs(root->uri+7, &st) != 0) */
/*           { */
/*              return NULL; */
/*           } */
/*         blocks_used = st.f_blocks - st.f_bfree; */
/*         blocks_percent_used = (blocks_used * 100ULL + (blocks_used + st.f_bavail)/2) / (blocks_used + st.f_bavail); */
/*         return */
/*           eina_stringshare_printf("%d", blocks_percent_used); */
/*      } */

/*    return NULL; */
/* } */

static Eina_Bool
_file_filter_cb(void *data, Eio_File *handler, const Eina_File_Direct_Info *info)
{
   /* Enna_Localfiles_Priv *priv = data; */

   if (*(info->path + info->name_start) == '.' )
     return EINA_FALSE;

   return EINA_TRUE;

   /* if ( info->type == EINA_FILE_DIR  || */
   /*      enna_util_uri_has_extension(info->path + info->name_start, priv->caps) ) */
   /*     return EINA_TRUE; */
   /* else */
   /*     return EINA_FALSE; */
}


static Eina_Bool
_dir_filter_cb(void *data, Eio_File *handler, const Eina_File_Direct_Info *info)
{
   return EINA_TRUE;
}

static void
_dir_progress_cb(void *data, Eio_File *handler, const Eio_Progress *info)
{
}

static void
_delete_done_cb(void *data, Eio_File *handler)
{
   Enna_Localfiles_Priv *priv = data;

   enna_browser_file_del(priv->browser, priv->file_dialog);
   enna_file_free(priv->file_dialog);
}

static void
_rename_done_cb(void *data, Eio_File *handler)
{
   Enna_Localfiles_Priv *priv = data;
   const char *new_path = priv->new_path;
   const char *new_uri;
   const char *new_mrl;


   new_uri = eina_stringshare_printf("%s/%s", ecore_file_dir_get(priv->file_dialog->uri), 
                                     ecore_file_file_get(new_path));

   new_mrl = eina_stringshare_printf("%s/%s", ecore_file_dir_get(priv->file_dialog->mrl), 
                                      ecore_file_file_get(new_path));

   priv->file_dialog->name = eina_stringshare_add(ecore_file_dir_get(new_path));
   priv->file_dialog->uri = new_uri;
   priv->file_dialog->label = eina_stringshare_add(ecore_file_file_get(new_path));
   priv->file_dialog->mrl = new_mrl;

   enna_browser_file_update(priv->browser, priv->file_dialog);
   eina_stringshare_del(new_path);
   priv->new_path = NULL;
}

static void
_error_cb(void *data, Eio_File *handler, int error)
{
}

static void
_dialog_delete_ok_clicked_cb(void *data, Evas_Object *btn, void *ev)
{
   Enna_Localfiles_Priv *priv = data;
   Enna_File *file = evas_object_data_get(priv->dialog, "file");

   if (priv->file_dialog)
     enna_file_free(priv->file_dialog);
   priv->file_dialog = enna_file_ref(file);


   if (file->type == ENNA_FILE_DIRECTORY)
     {
        eio_dir_unlink(file->mrl,
                       _dir_filter_cb,
                       _dir_progress_cb,
                       _delete_done_cb,
                       _error_cb,
                       priv);
     }
   else if (file->type == ENNA_FILE_FILE)
     {
        eio_file_unlink(file->mrl,
                        _delete_done_cb,
                        _error_cb,
                        priv);
     }

   evas_object_del(priv->dialog);
}

static void
_dialog_rename_ok_clicked_cb(void *data, Evas_Object *btn, void *ev)
{
   Enna_Localfiles_Priv *priv = data;
   Enna_File *file;
   Evas_Object *entry;
   const char *new_name;
   const char *new_path;


   file = evas_object_data_get(priv->dialog, "file");
   entry = evas_object_data_get(priv->dialog, "entry");
   new_name = elm_object_text_get(entry);
   new_path = eina_stringshare_printf("%s/%s", ecore_file_dir_get(file->mrl),
                                        new_name);

   priv->new_path = new_path;

   if (priv->file_dialog)
     enna_file_free(priv->file_dialog);
   priv->file_dialog = enna_file_ref(file);

   if (file->type == ENNA_FILE_DIRECTORY)
     {
        eio_dir_move(file->mrl,
                     new_path,
                       _dir_filter_cb,
                       _dir_progress_cb,
                       _rename_done_cb,
                       _error_cb,
                       priv);
     }
   else if (file->type == ENNA_FILE_FILE)
     {
        eio_file_move(file->mrl,
                      new_path,
                      _dir_progress_cb,
                      _rename_done_cb,
                      _error_cb,
                        priv);
     }

   evas_object_del(priv->dialog);
}

static void
_dialog_create_folder_ok_clicked_cb(void *data, Evas_Object *btn, void *ev)
{
   Enna_Localfiles_Priv *priv = data;
   Enna_File *file;
   Evas_Object *entry;
   const char *new_name;
   const char *new_path;
   char uri[4096];

   file = evas_object_data_get(priv->dialog, "file");
   entry = evas_object_data_get(priv->dialog, "entry");
   new_name = elm_object_text_get(entry);
   new_path = eina_stringshare_printf("%s/%s", ecore_file_dir_get(file->mrl),
                                        new_name);

   if (ecore_file_mkdir(new_name))
       {
           Enna_File *f;
           snprintf(uri, sizeof(uri), "file://%s", new_path);
           printf(" uri : %s\n", uri);
           f = enna_file_directory_add(new_name,
                                       uri,
                                       new_path,
                                       new_name,
                                       "icon/directory");
           enna_browser_file_add(priv->browser, f);
       }

   evas_object_del(priv->dialog);
}

static void
_dialog_cancel_clicked_cb(void *data, Evas_Object *obj, void *ev)
{
   Enna_Localfiles_Priv *priv = data;

   evas_object_del(priv->dialog);
}



static void
_action_rename_cb(void *data, Enna_File *file)
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
   Enna_Localfiles_Priv *priv = data;

   win = elm_win_add(enna->win, NULL, ELM_WIN_DIALOG_BASIC);
   elm_win_title_set(win, _("Rename"));
   elm_win_autodel_set(win, EINA_TRUE);
   evas_object_data_set(win, "file", file);

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
   elm_object_text_set(lb, _("Enter new name"));
   evas_object_size_hint_weight_set(lb, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   evas_object_size_hint_align_set(lb, 0.5, -1);
   evas_object_show(lb);
   elm_box_pack_end(bx, lb);

   en = elm_entry_add(win);
   elm_entry_single_line_set(en, EINA_TRUE);
   evas_object_size_hint_weight_set(en, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   evas_object_size_hint_align_set(en, -1, -1);
   elm_object_text_set(en, file->label);
   elm_box_pack_end(bx, en);
   evas_object_show(en);
   evas_object_data_set(win, "entry", en);

   btn_bx = elm_box_add(win);
   elm_box_horizontal_set(btn_bx, EINA_TRUE);
   evas_object_size_hint_weight_set(btn_bx, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   evas_object_size_hint_align_set(btn_bx, EVAS_HINT_FILL, 0.5);
   evas_object_show(btn_bx);
   elm_box_padding_set(btn_bx, 8, 2);

   btn_ok = elm_button_add(win);
   elm_object_text_set(btn_ok, _("Rename"));
   evas_object_show(btn_ok);
   evas_object_size_hint_weight_set(btn_ok, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   evas_object_size_hint_align_set(btn_ok, EVAS_HINT_FILL, 0.5);
   evas_object_smart_callback_add(btn_ok, "clicked",
                                  _dialog_rename_ok_clicked_cb, priv);
   elm_box_pack_end(btn_bx, btn_ok);

   btn_cancel = elm_button_add(win);
   elm_object_text_set(btn_cancel, _("Cancel"));
   evas_object_show(btn_cancel);
   evas_object_size_hint_weight_set(btn_cancel, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   evas_object_size_hint_align_set(btn_cancel, EVAS_HINT_FILL, EVAS_HINT_FILL);
   evas_object_smart_callback_add(btn_cancel, "clicked",
                                  _dialog_cancel_clicked_cb, priv);
   elm_box_pack_end(btn_bx, btn_cancel);

   elm_box_pack_end(bx, btn_bx);

   evas_object_show(win);
   priv->dialog = win;
}

static void
_action_create_folder_cb(void *data, Enna_File *file)
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
   Enna_Localfiles_Priv *priv = data;

   win = elm_win_add(enna->win, NULL, ELM_WIN_DIALOG_BASIC);
   elm_win_title_set(win, _("Create New Folder"));
   elm_win_autodel_set(win, EINA_TRUE);
   evas_object_data_set(win, "file", file);

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
   elm_object_text_set(lb, _("Enter folder name"));
   evas_object_size_hint_weight_set(lb, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   evas_object_size_hint_align_set(lb, 0.5, -1);
   evas_object_show(lb);
   elm_box_pack_end(bx, lb);

   en = elm_entry_add(win);
   elm_entry_single_line_set(en, EINA_TRUE);
   evas_object_size_hint_weight_set(en, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   evas_object_size_hint_align_set(en, -1, -1);
   elm_object_text_set(en, "New Folder");
   elm_box_pack_end(bx, en);
   evas_object_show(en);
   evas_object_data_set(win, "entry", en);

   btn_bx = elm_box_add(win);
   elm_box_horizontal_set(btn_bx, EINA_TRUE);
   evas_object_size_hint_weight_set(btn_bx, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   evas_object_size_hint_align_set(btn_bx, EVAS_HINT_FILL, 0.5);
   evas_object_show(btn_bx);
   elm_box_padding_set(btn_bx, 8, 2);

   btn_ok = elm_button_add(win);
   elm_object_text_set(btn_ok, _("Create"));
   evas_object_show(btn_ok);
   evas_object_size_hint_weight_set(btn_ok, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   evas_object_size_hint_align_set(btn_ok, EVAS_HINT_FILL, 0.5);
   evas_object_smart_callback_add(btn_ok, "clicked",
                                  _dialog_create_folder_ok_clicked_cb, priv);
   elm_box_pack_end(btn_bx, btn_ok);

   btn_cancel = elm_button_add(win);
   elm_object_text_set(btn_cancel, _("Cancel"));
   evas_object_show(btn_cancel);
   evas_object_size_hint_weight_set(btn_cancel, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   evas_object_size_hint_align_set(btn_cancel, EVAS_HINT_FILL, EVAS_HINT_FILL);
   evas_object_smart_callback_add(btn_cancel, "clicked",
                                  _dialog_cancel_clicked_cb, priv);
   elm_box_pack_end(btn_bx, btn_cancel);

   elm_box_pack_end(bx, btn_bx);

   evas_object_show(win);
   priv->dialog = win;
}


static void
_action_delete_cb(void *data, Enna_File *file)
{
   Evas_Object *win;
   Evas_Object *bg;
   Evas_Object *fr;
   Evas_Object *lb;
   Evas_Object *bx;
   Evas_Object *btn_bx;
   Evas_Object *btn_ok;
   Evas_Object *btn_cancel;
   Enna_Localfiles_Priv *priv = data;
   const char *label;

   ENNA_OBJECT_DEL(priv->dialog);

   win = elm_win_add(enna->win, NULL, ELM_WIN_DIALOG_BASIC);
   elm_win_title_set(win, _("Delete files"));
   elm_win_autodel_set(win, EINA_TRUE);
   evas_object_data_set(win, "file", file);

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
   elm_label_line_wrap_set(lb, ELM_WRAP_MIXED);
   elm_label_wrap_width_set(lb, 400);
   label = eina_stringshare_printf(_("Are you sure to delete <b>%s</b> ?"), file->label);
   elm_object_text_set(lb, label);
   eina_stringshare_del(label);
   evas_object_size_hint_weight_set(lb, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   evas_object_size_hint_align_set(lb, 0.5, -1);
   evas_object_show(lb);
   elm_box_pack_end(bx, lb);

   btn_bx = elm_box_add(win);
   elm_box_horizontal_set(btn_bx, EINA_TRUE);
   evas_object_size_hint_weight_set(btn_bx, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   evas_object_size_hint_align_set(btn_bx, EVAS_HINT_FILL, 0.5);
   evas_object_show(btn_bx);
   elm_box_padding_set(btn_bx, 8, 2);

   btn_ok = elm_button_add(win);
   elm_object_text_set(btn_ok, _("Delete"));
   evas_object_show(btn_ok);
   evas_object_size_hint_weight_set(btn_ok, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   evas_object_size_hint_align_set(btn_ok, EVAS_HINT_FILL, 0.5);
   elm_box_pack_end(btn_bx, btn_ok);
   evas_object_smart_callback_add(btn_ok, "clicked",
                                  _dialog_delete_ok_clicked_cb, priv);

   btn_cancel = elm_button_add(win);
   elm_object_text_set(btn_cancel, _("Cancel"));
   evas_object_show(btn_cancel);
   evas_object_size_hint_weight_set(btn_cancel, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   evas_object_size_hint_align_set(btn_cancel, EVAS_HINT_FILL, EVAS_HINT_FILL);
   elm_box_pack_end(btn_bx, btn_cancel);
   evas_object_smart_callback_add(btn_cancel, "clicked",
                                  _dialog_cancel_clicked_cb, priv);

   elm_box_pack_end(bx, btn_bx);

   evas_object_resize(win, 400, 128);

   evas_object_show(win);
   priv->dialog = win;
}

static void
_action_open_file_cb(void *data, Enna_File *file)
{
   enna_exec(file);
}

static void
_file_main_cb(void *data, Eio_File *handler, const Eina_File_Direct_Info *info)
{
   Enna_Localfiles_Priv *priv = data;
   Enna_File *f;
   const char *buf;
   Enna_File_Action *action;

   buf = eina_stringshare_printf("file://%s", info->path);

   if (info->type == EINA_FILE_DIR)
     {
        f = enna_file_directory_add(info->path + info->name_start, buf,
                                    info->path,
                                    info->path + info->name_start, "icon/directory");
     }
   else
     {
        if (priv->caps == ENNA_CAPS_ALL)
          f = enna_file_file_add(info->path + info->name_start, buf,
                                 info->path, info->path + info->name_start,
                                 NULL);

        else if (priv->caps == ENNA_CAPS_MUSIC)
          f = enna_file_track_add(info->path + info->name_start, buf,
                                  info->path, info->path + info->name_start,
                                  "icon/music");

        else if (priv->caps == ENNA_CAPS_VIDEO)
          f = enna_file_film_add(info->path + info->name_start, buf,
                                 info->path, info->path + info->name_start,
                                 "icon/video");
        else
          f = enna_file_file_add(info->path + info->name_start, buf,
                                 info->path, info->path + info->name_start,
                                 "icon/music");

     }

   if (!ENNA_FILE_IS_BROWSABLE(f))
     {
        action = enna_file_action_new(f, "open_file", "Open", "open-file", _action_open_file_cb, priv);
        enna_file_action_add(f, action);
     }
   action = enna_file_action_new(f, "copy", "Copy", "edit-copy", NULL, NULL);
   enna_file_action_add(f, action);
   action = enna_file_action_new(f, "move", "Move", "folder-move", NULL, NULL);
   enna_file_action_add(f, action);
   action = enna_file_action_new(f, "rename", "Rename", "gtk-edit", _action_rename_cb, priv);
   enna_file_action_add(f, action);
   action = enna_file_action_new(f, "delete", "Delete", "edit-delete", _action_delete_cb, priv);
   enna_file_action_add(f, action);
   action = enna_file_action_new(f, "details", "Details", "view-list-details", NULL, NULL);
   enna_file_action_add(f, action);
   action = enna_file_action_new(f, "create_folder", "Create new folder", "folder-new", _action_create_folder_cb, priv);
   enna_file_action_add(f, action);


   eina_stringshare_del(buf);
   enna_browser_file_add(priv->browser, f);
}

static void
_file_done_cb(void *data, Eio_File *handler)
{
   Enna_Localfiles_Priv *priv = data;
   priv->file = NULL;
}

static void
_file_error_cb(void *data, Eio_File *handler, int error)
{
}

/* static Eina_Bool */
/* _eio_event_cb(void *data, int type __UNUSED__, void *event) */
/* { */
/*    return EINA_TRUE; */
/* } */

static void
_children_get(void *priv, const char *uri, Enna_Browser *browser, ENNA_VFS_CAPS caps)
{
   Class_Private_Data *pmod = NULL;
   Enna_Localfiles_Priv *p = priv;
   switch(caps)
     {
      case  ENNA_CAPS_ALL:
         pmod = mod->all;
         break;
      default:
         break;
     }

   if (!pmod)
     return;


   p->file = eio_file_direct_ls(uri + 7,
                                _file_filter_cb,
                                _file_main_cb,
                                _file_done_cb,
                                _file_error_cb,
                                p);

   return;
}

static void
_del(void *priv)
{
   Enna_Localfiles_Priv *p = priv;
   /* Ecore_Event_Handler *handler; */

   if (p->file)
     eio_file_cancel(p->file);

   /* EINA_LIST_FREE(p->monitor_handlers, handler) */
   /*   { */
   /*      ecore_event_handler_del(handler); */
   /*   } */

   /* if (p->monitor) */
   /*   eio_monitor_del(p->monitor); */

   eina_stringshare_del(p->root);
   eina_stringshare_del(p->relative_path);
   ENNA_FREE(p);
}

static Enna_File *
_file_get(const char *uri)
{
   Enna_File *f;
   char *path;

   if (strlen(uri) < 7)
     {
        path = malloc(2 * sizeof(char));
        path[0] = '/';
        path[1] = '\0';
     }
   else
     {
        path = malloc((strlen(uri + 7) + 1) * sizeof(char));
        strncpy(path, uri + 7, strlen(uri + 7) + 1);
     }

   if (!ecore_file_exists(path))
     return NULL;

   if (ecore_file_is_dir(path))
     {
        const char *file;
        const char *label;
        file = ecore_file_file_get(path);
        if (!file || file[0] == '\0')
          {
             label = eina_stringshare_add("Root");
             f = enna_file_directory_add(label, "file:///",
                                         path, label,
                                         "icon/directory");
             eina_stringshare_del(label);
          }
        else
          {
             f = enna_file_directory_add(file, uri,
                                         path, file,
                                         "icon/directory");
          }
     }
   else
     f = enna_file_file_add(ecore_file_file_get(path), uri,
                            path, ecore_file_file_get(path),
                            NULL);
   free(path);
   return f;
}

static Enna_Vfs_Class class = {
  "localfiles",
  1,
  "Browse local devices",
  NULL,
  "icon/hd",
  {
    _add,
    _children_get,
    _del,
    _file_get,
  },
  NULL
};

/* Module interface */

void
enna_localfiles_init(void)
{
   int flags = 0;

   mod = calloc(1, sizeof(Enna_Module_LocalFiles));

   flags |= ENNA_CAPS_ALL;
   __class_init("explorer", &mod->all, ENNA_CAPS_ALL, "path_all");

   enna_vfs_register(&class, flags);
}

void
enna_localfiles_shutdown(void)
{

   free(mod->all);

   ENNA_FREE(mod);

   efreet_mime_shutdown();
   eio_shutdown();
}
