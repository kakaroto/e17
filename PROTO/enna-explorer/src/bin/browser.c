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
#define _GNU_SOURCE
#include <string.h>

#include <Ecore.h>
#include <Ecore_File.h>

#include "browser.h"
#include "enna.h"
#include "activity.h"
#include "buffer.h"
#include "utils.h"
#include "vfs.h"

typedef enum _Enna_Browser_Type
  {
    BROWSER_ROOT,
    BROWSER_ACTIVITY,
    BROWSER_MODULE,
  }Enna_Browser_Type;

struct _Enna_Browser
{
   Ecore_Idler *queue_idler;
   void (*add)(void *data, Enna_File *file);
   void (*del)(void *data, Enna_File *file);
   void (*update)(void *data, Enna_File *file);
   void *add_data;
   void *del_data;
   void *update_data;
   void *priv_module;
   const char *uri;
   Ecore_Event_Handler *ev_handler;
   Eina_List* files;
   Enna_Vfs_Class *vfs;
};

static void _browser_browse_module(Enna_Browser* browser);

static Eina_Bool
_add_idler(void *data)
{
   Enna_Browser* b = data;

   _browser_browse_module(b);

   b->queue_idler = NULL;
   return EINA_FALSE;

}

Enna_Browser *
enna_browser_add(void (*add)(void *data, Enna_File *file), void *add_data,
                 void (*del)(void *data, Enna_File *file), void *del_data,
                 void (*update)(void *data, Enna_File  *file), void *update_data,
                 const char *uri)
{
   Enna_Browser *b;

   b = calloc(1, sizeof(Enna_Browser));
   b->add = add;
   b->del = del;
   b->update = update;
   b->add_data = add_data;
   b->del_data = del_data;
   b->update_data = update_data;
   b->queue_idler = NULL;
   b->uri = eina_stringshare_add(uri);

   return b;
}

void
enna_browser_del(Enna_Browser *b)
{
   Enna_File *file;

   if (!b)
     return;

   if (b->queue_idler)
     ecore_idler_del(b->queue_idler);
   b->queue_idler = NULL;
   eina_stringshare_del(b->uri);
   if (b->ev_handler)
     ecore_event_handler_del(b->ev_handler);
   EINA_LIST_FREE(b->files, file)
     enna_file_free(file);

   if (b->vfs)
     b->vfs->func.del(b->priv_module);
   free(b);
}

void
enna_browser_browse(Enna_Browser *b)
{
   if (!b)
     return;

   if (!b->queue_idler)
     b->queue_idler = ecore_idler_add(_add_idler, b);
}

void
enna_browser_file_add(Enna_Browser *b, Enna_File *file)
{
   if (!b)
     return;

   if (!file)
     {
        Enna_File *nofile;
        nofile = calloc(1, sizeof(Enna_File));
        nofile->icon =  eina_stringshare_add("icon/nofile");
        nofile->label = eina_stringshare_add("No media found!");
        nofile->type = ENNA_FILE_MENU;
        nofile->uri = eina_stringshare_add(ecore_file_dir_get(b->uri));
        b->files = eina_list_append(b->files, nofile);
        b->add(b->add_data, nofile);
        return;
     }

   b->files = eina_list_append(b->files, file);
   b->add(b->add_data, file);
}

Enna_File *
enna_browser_file_update(Enna_Browser *b, Enna_File *file)
{
   Enna_File *f;
   Eina_List *l;

   if (!b || !file)
     return NULL;

   if (eina_list_data_find(b->files, file))
     {
        b->update(b->update_data, file);
        return file;
     }
   else
     {
        EINA_LIST_FOREACH(b->files, l, f)
          {
             if(!strcmp(f->uri, file->uri))
               {
                  eina_stringshare_replace(&f->name, file->name);
                  eina_stringshare_replace(&f->label, file->label);
                  eina_stringshare_replace(&f->icon, file->icon);
                  eina_stringshare_replace(&f->icon_file, file->icon_file);
                  eina_stringshare_replace(&f->mrl, file->mrl);
                  f->type = file->type;
                  f->meta_class = file->meta_class;
                  f->meta_data = file->meta_data;
                  b->update(b->update_data, f);
                  enna_file_free(file);
                  return f;
               }
          }
        enna_browser_file_add(b, file);
        return file;
     }
}

void
enna_browser_file_del(Enna_Browser *b, Enna_File *file)
{
   if (!b || !file)
     return;

   b->files = eina_list_remove(b->files, file);
   b->del(b->del_data, file);
}

static void
_browser_browse_module(Enna_Browser *browser)
{
   Enna_Vfs_Class *vfs = NULL, *tmp = NULL;
   Eina_List *l;

   if (!browser || !browser->uri || !eina_str_has_prefix(browser->uri, "file://"))
     return;

   EINA_LIST_FOREACH(enna_vfs_get(ENNA_CAPS_ALL), l, tmp)
     {
        if (!strcmp(tmp->name, "localfiles"))
          {
             vfs = tmp;
             break;
          }
     }

   if (!vfs)
     return;

   browser->vfs = vfs;
   browser->priv_module =
     browser->vfs->func.add(browser->uri, browser, ENNA_CAPS_ALL);
   browser->vfs->func.get_children(browser->priv_module,
                                   browser->uri, browser, ENNA_CAPS_ALL);
}


Eina_List *
enna_browser_files_get(Enna_Browser *b)
{
   return b ? b->files : NULL;
}

const char *
enna_browser_uri_get(Enna_Browser *b)
{
   return b ? b->uri : NULL;
}

void
enna_browser_filter(Enna_Browser *b, const char *filter)
{
   Eina_List *l;
   Enna_File *f;

   if (!b || !filter)
     return;

   /* First remove all files from view */
   EINA_LIST_FOREACH(b->files, l, f)
     {
        if (b->del)
          b->del(b->del_data, f);
     }
   /* Then add files that contain filter string */
   EINA_LIST_FOREACH(b->files, l, f)
     {
        if (strcasestr(f->label, filter))
          {
             if(b->add)
               b->add(b->add_data, f);
          }

     }
}

