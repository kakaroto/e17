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
   Enna_Browser_Type type;
   Ecore_Event_Handler *ev_handler;
   Eina_List *tokens;
   Eina_List* files;
   Enna_Vfs_Class *vfs;
};

static void _browser_browse_root(Enna_Browser *browser);
static void _browser_browse_activity(Enna_Browser* browser);
static void _browser_browse_module(Enna_Browser* browser);

static Eina_Bool
_add_idler(void *data)
{
   Enna_Browser* b = data;
   switch (b->type)
     {
      case BROWSER_ROOT:
         _browser_browse_root(b);
         break;
      case BROWSER_ACTIVITY:
         _browser_browse_activity(b);
         break;
      case BROWSER_MODULE:
         _browser_browse_module(b);
         break;
      default:
         break;

     }
   b->queue_idler = NULL;
   return EINA_FALSE;

}

static Eina_Bool
_activities_changed_cb(void *data, int type __UNUSED__, void *event)
{
   Enna_Browser *browser = data;
   Enna_File *file = event;

   if (!file || !browser)
     return ECORE_CALLBACK_RENEW;

   if (browser->add)
     browser->add(browser->add_data, file);


   return ECORE_CALLBACK_RENEW;
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
   b->tokens = NULL;
   b->tokens = enna_util_tuple_get(uri, "/");

   if (!b->tokens || eina_list_count(b->tokens) == 0)
     b->type = BROWSER_ROOT;
   else if (eina_list_count(b->tokens) == 1)
     b->type = BROWSER_ACTIVITY;
   else if (eina_list_count(b->tokens) >= 2)
     b->type = BROWSER_MODULE;
   else
     return NULL;

   return b;
}

void
enna_browser_del(Enna_Browser *b)
{
   Enna_File *file;
   char *token;

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
   EINA_LIST_FREE(b->tokens, token)
     free(token);
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
   if (b->type == BROWSER_ROOT && !b->ev_handler)
     b->ev_handler = ecore_event_handler_add(ENNA_EVENT_BROWSER_CHANGED,
                                             _activities_changed_cb, b);
}

int enna_browser_level_get(Enna_Browser *b)
{
   if (b && b->tokens)
     return eina_list_count(b->tokens);

   return -1;
}

static void
_browser_browse_root(Enna_Browser *browser)
{
   Eina_List *l;
   Enna_Class_Activity *act;
   Enna_Buffer *buf;
   Enna_File *f;

   EINA_LIST_FOREACH(enna_activities_get(), l, act)
     {
        f = calloc(1, sizeof(Enna_File));

        buf = enna_buffer_new();
        enna_buffer_appendf(buf, "/%s", act->name);
        f->name = eina_stringshare_add(act->name);
        f->uri = eina_stringshare_add(buf->buf);
        enna_buffer_free(buf);
        f->label = eina_stringshare_add(act->label);
        f->icon = eina_stringshare_add(act->icon);
        f->icon_file = eina_stringshare_add(act->bg);
        f->type = ENNA_FILE_MENU;

        browser->files = eina_list_append(browser->files, f);
        if (browser->add)
          browser->add(browser->add_data, f);

     }
}

static void
_browser_browse_activity(Enna_Browser *browser)
{

   const char *act_name = eina_list_nth(browser->tokens, 0);
   Enna_Class_Activity *act = enna_activity_get(act_name);

   Enna_Vfs_Class *vfs;
   Eina_List *l;
   Enna_File *f;
   Enna_Buffer *buf;

   EINA_LIST_FOREACH(enna_vfs_get(act->caps), l, vfs)
     {

        f = calloc(1, sizeof(Enna_File));

        buf = enna_buffer_new();
        enna_buffer_appendf(buf, "/%s/%s", act_name, vfs->name);
        f->name = eina_stringshare_add(vfs->name);
        f->uri = eina_stringshare_add(buf->buf);
        enna_buffer_free(buf);
        f->label = eina_stringshare_add(vfs->label);
        f->icon = eina_stringshare_add(vfs->icon);
        f->type = ENNA_FILE_MENU;
        browser->files = eina_list_append(browser->files, f);
        if (browser->add)
          browser->add(browser->add_data, f);
     }
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
   Enna_Class_Activity *act;
   const char *act_name = (const char*)eina_list_nth(browser->tokens, 0);
   const char *name=  (const char*)eina_list_nth(browser->tokens, 1) ;

   printf("Activity : %s\n", act_name);
   act = enna_activity_get(act_name);

   if (!act)
     return;

   EINA_LIST_FOREACH(enna_vfs_get(act->caps), l, tmp)
     if (!strcmp(tmp->name, name))
       {
          vfs = tmp;
          break;
       }

   if (!vfs)
     return;

   browser->vfs = vfs;
   browser->priv_module =
     browser->vfs->func.add(browser->tokens, browser, act->caps);
   browser->vfs->func.get_children(browser->priv_module,
                                   browser->tokens, browser, act->caps);


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

