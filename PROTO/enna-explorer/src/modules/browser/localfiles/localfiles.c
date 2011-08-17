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
#include "module.h"
#include "vfs.h"
#include "volumes.h"
#include "utils.h"
#include "buffer.h"

#define ENNA_MODULE_NAME "localfiles"

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
   Enna_Volumes_Listener *vl;
} Class_Private_Data;

typedef struct _Enna_Module_LocalFiles
{
   Evas *e;
   Enna_Module *em;
#ifdef BUILD_ACTIVITY_EXPLORER
   Class_Private_Data *all;
#endif
#ifdef BUILD_ACTIVITY_MUSIC
   Class_Private_Data *music;
#endif
#ifdef BUILD_ACTIVITY_VIDEO
   Class_Private_Data *video;
#endif
#ifdef BUILD_ACTIVITY_PHOTO
   Class_Private_Data *photo;
#endif
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
   Enna_Volumes_Listener *vl;
   Eio_File *file;
   ENNA_VFS_CAPS caps;
   const char *root;
   const char *relative_path;
   Enna_Browser *browser;
   Eio_Monitor *monitor;
   Eina_List *monitor_handlers;
} Enna_Localfiles_Priv;

static localfiles_cfg_t localfiles_cfg;
static Enna_Module_LocalFiles *mod;


static void
_add_volumes_cb(void *data, Enna_Volume *v)
{
   Class_Private_Data *priv = data;
   Root_Directories *root;

   if (!strstr(v->mount_point, "file://"))
     return;

   root = calloc(1, sizeof(Root_Directories));
   root->name = eina_stringshare_add(v->label);

   root->uri = strdup( v->mount_point);
   root->label = strdup(v->label);
   root->icon = strdup(enna_volumes_icon_from_type(v));
   priv->config->root_directories =
     eina_list_append(priv->config->root_directories, root);
}

static void
_remove_volumes_cb(void *data, Enna_Volume *v)
{
   Class_Private_Data *priv = data;
   Root_Directories *root;
   Eina_List *l;

   EINA_LIST_FOREACH(priv->config->root_directories, l, root)
     {
        if (!strcmp(root->label, v->label))
          {
             priv->config->root_directories =
               eina_list_remove(priv->config->root_directories, root);
             ENNA_FREE(root->uri);
             ENNA_FREE(root->label);
             ENNA_FREE(root->icon);
             ENNA_FREE(root);
          }
     }
}

static void
__class_init(const char *name, Class_Private_Data **priv,
             ENNA_VFS_CAPS caps, char *key)
{
   Class_Private_Data *data;
   Root_Directories *root;
   char buf[PATH_MAX + 7];
   Eina_List *path_list;
   Eina_List *l;
   Enna_Volume *v;

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

   /* Add All detected volumes */
   EINA_LIST_FOREACH(enna_volumes_get(), l, v)
     {
        root = calloc(1, sizeof(Root_Directories));
        snprintf(buf, sizeof(buf), "file://%s", v->mount_point);
        root->name = eina_stringshare_add(v->device_name);
        root->uri = strdup(buf);
        root->label = strdup(v->label);

        root->icon = strdup(enna_volumes_icon_from_type(v));
        data->config->root_directories =
          eina_list_append(data->config->root_directories, root);
     }

   if (localfiles_cfg.home)
     {
        // add home directory entry
        root = ENNA_NEW(Root_Directories, 1);
        snprintf(buf, sizeof(buf), "file://%s", enna_util_user_home_get());
        root->name = eina_stringshare_add("Home");
        root->uri = strdup(buf);
        root->label = strdup("Home");
        root->icon = strdup("icon/home");

        data->config->root_directories =
          eina_list_append(data->config->root_directories, root);
     }

   /* add localfiles to the list of volumes listener */
   data->vl = enna_volumes_listener_add("localfiles", _add_volumes_cb,
                                        _remove_volumes_cb, data);
}

static void
_add_child_volume_cb(void *data, Enna_Volume *v)
{
   Enna_Browser *b = data;
   Enna_File *f;

   Enna_Buffer *buf;

   buf = enna_buffer_new();
   enna_buffer_appendf(buf, "/%s/localfiles/%s", "music", v->label);
   f = enna_file_menu_add(v->label, buf->buf,
                          v->label, "icon/hd");
   enna_buffer_free(buf);
   enna_browser_file_add(b, f);
}

static void
_remove_child_volume_cb(void *data, Enna_Volume *v)
{
   Enna_Browser *b = data;
   Eina_List *files, *l;
   Enna_File *file;

   files = enna_browser_files_get(b);
   EINA_LIST_FOREACH(files, l, file)
     {
        if (file->name == v->label)
          {
             enna_browser_file_del(b, file);
          }
     }
}

static void *
_add(Eina_List *tokens, Enna_Browser *browser, ENNA_VFS_CAPS caps)
{

   Enna_Localfiles_Priv *priv;

   priv = calloc(1, sizeof(Enna_Localfiles_Priv));
   priv->caps = caps;
   priv->browser = browser;
   if (eina_list_count(tokens) == 2 )
     {
        priv->vl = enna_volumes_listener_add("localfiles_refresh", _add_child_volume_cb,
                                             _remove_child_volume_cb, browser);
     }
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

static const char *
_root_meta_get(void *data, Enna_File *file, const char *key)
{
   Root_Directories *root = data;
   struct statfs st;

   if (!root)
     return NULL;

   if (!strcmp(key, ENNA_META_KEY_SIZE))
     {

        if (statfs(root->uri+7, &st) != 0)
          {
             return NULL;
          }

        return
          eina_stringshare_printf("%s",  make_human_readable_str(
                                                                 st.f_blocks, st.f_bsize, 0));
     }
   else if (!strcmp(key, ENNA_META_KEY_USESPACE))
     {
        if (statfs(root->uri+7, &st) != 0)
          {
             return NULL;
          }
        return
          eina_stringshare_printf("%s",make_human_readable_str(
                                                               (st.f_blocks - st.f_bfree),
                                                               st.f_bsize, 0));
     }
   else if (!strcmp(key, ENNA_META_KEY_FREESPACE))
     {
        if (statfs(root->uri+7, &st) != 0)
          {
             return NULL;
          }
        return
          eina_stringshare_printf("%s", make_human_readable_str(st.f_bavail,
                                                                st.f_bsize, 0));
     }
   else if (!strcmp(key, ENNA_META_KEY_PERCENT_USED))
     {
        unsigned long blocks_used;
        unsigned blocks_percent_used;

        if (statfs(root->uri+7, &st) != 0)
          {
             return NULL;
          }
        blocks_used = st.f_blocks - st.f_bfree;
        blocks_percent_used = (blocks_used * 100ULL + (blocks_used + st.f_bavail)/2) / (blocks_used + st.f_bavail);
        return
          eina_stringshare_printf("%d", blocks_percent_used);
     }

   return NULL;
}

static Enna_File_Meta_Class root_meta_class = {
  _root_meta_get,
  NULL,
  NULL
};

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

static void
_file_main_cb(void *data, Eio_File *handler, const Eina_File_Direct_Info *info)
{
   Enna_Localfiles_Priv *priv = data;
   Enna_File *f;
   const char *buf;


   if (priv->relative_path)
     buf = eina_stringshare_printf("/%s/%s%s", priv->root, priv->relative_path, info->path + info->name_start);
   else
     buf = eina_stringshare_printf("/%s/%s", priv->root, info->path + info->name_start);

   if (info->type == EINA_FILE_DIR)
     {
        f = enna_file_directory_add(info->path + info->name_start, buf,
                                    info->path,
                                    info->path + info->name_start, "icon/directory");
     }
   else
     {
        if (priv->caps == ENNA_CAPS_ALL)
          {
             const char *mime;
             const char *icon;
             mime = efreet_mime_type_get(info->path);
             icon = efreet_mime_type_icon_get(mime, getenv("E_ICON_THEME"), 48);
             if (!icon)
               icon = efreet_mime_type_icon_get("unknown", getenv("E_ICON_THEME"), 48);
             f = enna_file_file_add(info->path + info->name_start, buf,
                                    info->path, info->path + info->name_start,
                                    icon);
          }
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

static Eina_Bool
_eio_event_cb(void *data, int type __UNUSED__, void *event)
{
   printf("EIO event\n");
   return EINA_TRUE;
}

static void
_get_children(void *priv, Eina_List *tokens, Enna_Browser *browser, ENNA_VFS_CAPS caps)
{
   Eina_List *l;
   Class_Private_Data *pmod = NULL;
   Enna_Localfiles_Priv *p = priv;
   switch(caps)
     {
      case  ENNA_CAPS_ALL:
#ifdef BUILD_ACTIVITY_EXPLORER
         pmod = mod->all;
#endif
         break;
      case  ENNA_CAPS_MUSIC:
#ifdef BUILD_ACTIVITY_MUSIC
         pmod = mod->music;
#endif
         break;
      case ENNA_CAPS_VIDEO:
#ifdef BUILD_ACTIVITY_VIDEO
         pmod = mod->video;
#endif
         break;
      case ENNA_CAPS_PHOTO:
#ifdef BUILD_ACTIVITY_PHOTO
         pmod = mod->photo;
#endif
         break;
      default:
         break;
     }

   if (!pmod)
     return;

   if (eina_list_count(tokens) == 2 )
     {
        //DBG("Browse Root\n");
        for (l = pmod->config->root_directories; l; l = l->next)
          {
             Enna_File *f;
             Root_Directories *root;
             Enna_Buffer *buf;

             root = l->data;

             buf = enna_buffer_new();
             enna_buffer_appendf(buf, "/%s/localfiles/%s", pmod->name, root->name);
             f = enna_file_volume_add(root->name, buf->buf,
                                      root->label, root->icon);

             enna_file_meta_add(f, &root_meta_class, root);
             enna_buffer_free(buf);
             enna_browser_file_add(browser, f);
             /* add localfiles to the list of volumes listener */
          }


     }
   else
     {
        const char *root_name = eina_list_nth(tokens, 2);
        Root_Directories *root = NULL;

        EINA_LIST_FOREACH(pmod->config->root_directories, l, root)
          {
             if (!strcmp(root->name, root_name))
               {

                  Eina_List *l;


                  Enna_Buffer *path;
                  Enna_Buffer *relative_path;
                  char *tmp;

                  Eina_List *l_tmp;
                  Ecore_Event_Handler *handler;

                  path = enna_buffer_new();
                  relative_path = enna_buffer_new();
                  enna_buffer_appendf(path, "%s", root->uri + 7);

                  l_tmp = eina_list_nth_list(tokens, 3);
                  EINA_LIST_FOREACH(l_tmp, l, tmp)
                    {
                       enna_buffer_appendf(path, "/%s", tmp);
                       enna_buffer_appendf(relative_path, "%s/", tmp);
                    }

                  p->relative_path = eina_stringshare_add(relative_path->buf);
                  p->root = eina_stringshare_printf("/%s/localfiles/%s",
                                                    pmod->name, root->name);

                  p->monitor = eio_monitor_stringshared_add(eina_stringshare_add(path->buf));
                  printf("Add monitor to %s\n", path->buf);

                  handler = ecore_event_handler_add(EIO_MONITOR_FILE_CREATED,
                                                    _eio_event_cb, p);
                  p->monitor_handlers  = eina_list_append(p->monitor_handlers, handler);
                  handler = ecore_event_handler_add(EIO_MONITOR_FILE_DELETED,
                                                    _eio_event_cb, p);
                  p->monitor_handlers  = eina_list_append(p->monitor_handlers, handler);
                  handler = ecore_event_handler_add(EIO_MONITOR_DIRECTORY_CREATED,
                                                    _eio_event_cb, p);
                  p->monitor_handlers  = eina_list_append(p->monitor_handlers, handler);
                  handler = ecore_event_handler_add(EIO_MONITOR_DIRECTORY_DELETED,
                                                    _eio_event_cb, p);
                  p->monitor_handlers  = eina_list_append(p->monitor_handlers, handler);
                  handler = ecore_event_handler_add(EIO_MONITOR_ERROR,
                                                    _eio_event_cb, p);
                  p->monitor_handlers  = eina_list_append(p->monitor_handlers, handler);

                  p->file = eio_file_direct_ls(path->buf,
                                               _file_filter_cb,
                                               _file_main_cb,
                                               _file_done_cb,
                                               _file_error_cb,
                                               p);
               }
          }
     }
   return;
}

static void
_del(void *priv)
{
   Enna_Localfiles_Priv *p = priv;
   Ecore_Event_Handler *handler;
   if (p && p->vl)
     enna_volumes_listener_del(p->vl);
   if (p->file)
     eio_file_cancel(p->file);

   EINA_LIST_FREE(p->monitor_handlers, handler)
     {
        ecore_event_handler_del(handler);
     }

   if (p->monitor)
     eio_monitor_del(p->monitor);

   eina_stringshare_del(p->root);
   eina_stringshare_del(p->relative_path);
   ENNA_FREE(p);
}

static Enna_Vfs_Class class = {
  "localfiles",
  1,
  "Browse local devices",
  NULL,
  "icon/hd",
  {
    _add,
    _get_children,
    _del
  },
  NULL
};

/* Module interface */

#ifdef USE_STATIC_MODULES
#undef MOD_PREFIX
#define MOD_PREFIX enna_mod_browser_localfiles
#endif /* USE_STATIC_MODULES */

static void
module_init(Enna_Module *em)
{
   int flags = 0;

   if (!em)
     return;

   efreet_mime_init();
   eio_init();
   mod = calloc(1, sizeof(Enna_Module_LocalFiles));
   mod->em = em;
   em->mod = mod;


#ifdef BUILD_ACTIVITY_EXPLORER
   flags |= ENNA_CAPS_ALL;
   __class_init("explorer", &mod->all, ENNA_CAPS_ALL, "path_all");
#endif
#ifdef BUILD_ACTIVITY_MUSIC
   flags |= ENNA_CAPS_MUSIC;
   __class_init("music", &mod->music, ENNA_CAPS_MUSIC, "path_music");
#endif
#ifdef BUILD_ACTIVITY_VIDEO
   flags |= ENNA_CAPS_VIDEO;
   __class_init("video", &mod->video, ENNA_CAPS_VIDEO, "path_video");
#endif
#ifdef BUILD_ACTIVITY_PHOTO
   flags |= ENNA_CAPS_PHOTO;
   __class_init("photo", &mod->photo, ENNA_CAPS_PHOTO, "path_photo");
#endif
   enna_vfs_register(&class, flags);
}

static void
module_shutdown(Enna_Module *em)
{
   Enna_Module_LocalFiles *mod;

   mod = em->mod;
#ifdef BUILD_ACTIVITY_EXPLORER
   enna_volumes_listener_del(mod->all->vl);
   free(mod->all);
#endif
#ifdef BUILD_ACTIVITY_MUSIC
   enna_volumes_listener_del(mod->music->vl);
   free(mod->music);
#endif
#ifdef BUILD_ACTIVITY_VIDEO
   enna_volumes_listener_del(mod->video->vl);
   free(mod->video);
#endif
#ifdef BUILD_ACTIVITY_PHOTO
   enna_volumes_listener_del(mod->photo->vl);
   free(mod->photo);
#endif
   ENNA_FREE(mod);

   efreet_mime_shutdown();
   eio_shutdown();
}

Enna_Module_Api ENNA_MODULE_API =
  {
    ENNA_MODULE_VERSION,
    "browser_localfiles",
    "Browse local files",
    "icon/hd",
    "Browse files in your local file systems",
    "bla bla bla<br><b>bla bla bla</b><br><br>bla.",
    {
      module_init,
      module_shutdown
    }
  };

