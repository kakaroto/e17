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

#include <libgupnp/gupnp-control-point.h>
#include <libgupnp-av/gupnp-av.h>
#include <string.h>
#include <pthread.h>

#include "enna.h"
#include "enna_config.h"
#include "module.h"
#include "vfs.h"
#include "volumes.h"
#include "utils.h"
#include "buffer.h"


#define ENNA_MODULE_NAME  "upnp"

#define UPNP_MEDIA_SERVER   "urn:schemas-upnp-org:device:MediaServer:1"
#define UPNP_CONTENT_DIR    "urn:schemas-upnp-org:service:ContentDirectory"
#define UPNP_MAX_BROWSE     32
#define UPNP_DEFAULT_ROOT   "0"
#define ITEM_CLASS_IMAGE    "object.item.imageItem"
#define ITEM_CLASS_AUDIO    "object.item.audioItem"
#define ITEM_CLASS_VIDEO    "object.item.videoItem"
#define ITEM_CLASS_TEXT     "object.item.textItem"

typedef struct _Enna_Module_UPnP
{
   Evas *e;
   Enna_Module *em;
   pthread_mutex_t mutex;
   Eina_List *devices;
   GUPnPContextManager *ctxm;
   GUPnPContext *ctx;
   GUPnPControlPoint *cp;
   pthread_mutex_t mutex_id;
   char *current_id;
} Enna_Module_UPnP;

typedef struct upnp_media_server_s {
   GUPnPDeviceInfo *info;
   GUPnPServiceProxy *content_dir;
   char *type;
   char *location;
   char *udn;
   char *name;
   char *model;
} upnp_media_server_t;

static Enna_Module_UPnP *mod;

/* UPnP Internals */

static void
upnp_current_id_set(const char *id)
{
   pthread_mutex_lock(&mod->mutex_id);
   ENNA_FREE(mod->current_id);
   mod->current_id = id ? strdup(id) : NULL;
   pthread_mutex_unlock(&mod->mutex_id);
}

static void
upnp_media_server_free(upnp_media_server_t *srv)
{
   if (!srv)
     return;

   g_object_unref(srv->info);
   g_object_unref(srv->content_dir);

   ENNA_FREE(srv->type);
   ENNA_FREE(srv->location);
   ENNA_FREE(srv->udn);
   ENNA_FREE(srv->name);
   ENNA_FREE(srv->model);
   ENNA_FREE(srv);
}

typedef struct upnp_filter_arg_s {
   Enna_Browser *browser;
   ENNA_VFS_CAPS cap;
   const char *current_id;
   GUPnPServiceProxy *content_dir;
   const char *container_id;
   const char *udn;
   unsigned int starting_index;
   GUPnPServiceProxyAction *browse_action;
   GUPnPServiceProxyAction *browse_metadata_action;
} upnp_filter_arg_t;


static void upnp_browse(guint32 starting_index, guint32 requested_count, upnp_filter_arg_t *priv);

static const char *
_track_meta_get(void *data, const Enna_File *file, const char *key)
{
   GUPnPDIDLLiteObject *object = data;

   if (!strcmp(key, "title"))
     {
        const char *title = gupnp_didl_lite_object_get_title(object);
        return eina_stringshare_add(title);
     }
   else if (!strcmp(key, "artist"))
     {
        const char *artist = gupnp_didl_lite_object_get_artist(object);
        return eina_stringshare_add(artist);
     }
   else if (!strcmp(key, "author"))
     {
        const char *author = gupnp_didl_lite_object_get_author(object);
        return eina_stringshare_add(author);
     }
   else if (!strcmp(key, "album"))
     {
        const char *album = gupnp_didl_lite_object_get_album(object);
        return eina_stringshare_add(album);
     }
   else if (!strcmp(key, "cover"))
     {
        const char *cover = gupnp_didl_lite_object_get_album_art(object);
        return eina_stringshare_add(cover);
     }
   else if (!strcmp(key, "track"))
     {
        char tmp[16];
        int track = gupnp_didl_lite_object_get_track_number(object);
        snprintf(tmp, sizeof(tmp), "%02d", track);
        return eina_stringshare_add(tmp);
     }
   return NULL;
}

static Enna_File_Meta_Class track_meta = {
  _track_meta_get,
  NULL,
  NULL
};


static void
on_didl_item_available (GUPnPDIDLLiteParser *parser,
                        GUPnPDIDLLiteObject *object,
                        gpointer             user_data)
{
   GUPnPDIDLLiteResource *resource;
   const char *id, *title;
   const char *uri;
   GList *resources;
   upnp_filter_arg_t *priv = user_data;
   Enna_File *f;

   id = gupnp_didl_lite_object_get_id(object);
   if (!id)
     return;

   title = gupnp_didl_lite_object_get_title(object);
   if (!title)
     return;

   //class_name = gupnp_didl_lite_object_get_upnp_class(object);

   resources = gupnp_didl_lite_object_get_resources(object);
   resource = resources->data;

   uri = gupnp_didl_lite_resource_get_uri(resource);

   f = enna_file_track_add(title, uri, uri, title, "icon/music");
   g_object_ref(object);
   enna_file_meta_add(f, &track_meta, object);

   enna_browser_file_add(priv->browser, f);

}

static void
_upnp_browse_metadata_cb (GUPnPServiceProxy       *content_dir,
                          GUPnPServiceProxyAction *action,
                          gpointer                 user_data)
{
   char               *metadata;
   GError             *error;
   upnp_filter_arg_t *priv = user_data;
   GUPnPDIDLLiteParser   *parser;

   metadata = NULL;
   error = NULL;

   gupnp_service_proxy_end_action (content_dir,
                                   action,
                                   &error,
                                   /* OUT args */
                                   "Result",
                                   G_TYPE_STRING,
                                   &metadata,
                                   NULL);
   if (metadata)
     {
        parser = gupnp_didl_lite_parser_new ();

        g_signal_connect (parser,
                          "item-available",
                          G_CALLBACK (on_didl_item_available),
                          priv);

        gupnp_didl_lite_parser_parse_didl (parser, metadata, &error);

        g_free (metadata);
     } else if (error) {
      g_warning ("Failed to get metadata for '%s': %s",
                 priv->container_id,
                 error->message);

      g_error_free (error);
   }

   g_object_unref (content_dir);
}

static void
didl_parse_item(GUPnPDIDLLiteParser *parser,
                GUPnPDIDLLiteItem *item, gpointer user_data)
{
   GUPnPDIDLLiteObject *obj;
   const char *id, *title;
   const char *class_name;
   char *icon = NULL;

   upnp_filter_arg_t *priv;

   priv  = user_data;

   obj  = (GUPnPDIDLLiteObject *) item;

   id = gupnp_didl_lite_object_get_id(obj);
   if (!id)
     return;

   title = gupnp_didl_lite_object_get_title(obj);
   if (!title)
     return;

   class_name = gupnp_didl_lite_object_get_upnp_class(obj);
   if (!class_name)

     if (!icon)
       icon = "icon/hd";




   priv->browse_metadata_action =  gupnp_service_proxy_begin_action
     (g_object_ref (priv->content_dir),
      "Browse",
      _upnp_browse_metadata_cb,
      priv,
      /* IN args */
      "ObjectID",
      G_TYPE_STRING,
      id,
      "BrowseFlag",
      G_TYPE_STRING,
      "BrowseMetadata",
      "Filter",
      G_TYPE_STRING,
      "*",
      "StartingIndex",
      G_TYPE_UINT,
      0,
      "RequestedCount",
      G_TYPE_UINT, 0,
      "SortCriteria",
      G_TYPE_STRING,
      "",
      NULL);

   //err_res:
   //    g_list_free(resources);
}

static void
didl_parse_container(GUPnPDIDLLiteParser *parser,
                     GUPnPDIDLLiteContainer *container,
                     gpointer user_data)
{
   GUPnPDIDLLiteObject *obj;
   Enna_File *f = NULL;
   const char *id, *title;
   char uri[512];
   upnp_filter_arg_t *priv;

   obj = (GUPnPDIDLLiteObject *) container;
   priv = user_data;

   id = gupnp_didl_lite_object_get_id(obj);
   if (!id)
     return;
   title = gupnp_didl_lite_object_get_title(obj);
   if (!title)
     return;

   snprintf(uri, sizeof(uri), "/%s/upnp/udn:%s,id:%s", "explorer", priv->udn, id);

   f = enna_file_menu_add(title, uri, title, "icon/directory");
   enna_browser_file_add(priv->browser, f);

}

static void
didl_process(char *didl, const char *udn, upnp_filter_arg_t *priv)
{
   GUPnPDIDLLiteParser *parser;

   parser = gupnp_didl_lite_parser_new();

   g_signal_connect(parser, "container-available",
                    G_CALLBACK(didl_parse_container), priv);
   g_signal_connect(parser, "item-available",
                    G_CALLBACK(didl_parse_item), priv);

   gupnp_didl_lite_parser_parse_didl(parser, didl, NULL);

   g_object_unref(parser);
}

static void
_upnp_browse_cb (GUPnPServiceProxy       *content_dir,
                 GUPnPServiceProxyAction *action,
                 gpointer                 user_data)
{
   char       *didl_xml;
   guint32     number_returned;
   guint32     total_matches;
   GError     *error;
   upnp_filter_arg_t *priv = user_data;

   didl_xml = NULL;
   error = NULL;

   gupnp_service_proxy_end_action (content_dir,
                                   action,
                                   &error,
                                   /* OUT args */
                                   "Result",
                                   G_TYPE_STRING,
                                   &didl_xml,
                                   "NumberReturned",
                                   G_TYPE_UINT,
                                   &number_returned,
                                   "TotalMatches",
                                   G_TYPE_UINT,
                                   &total_matches,
                                   NULL);

   /* check action result */
   if (error)
     {
        g_error_free(error);
     }

   if (didl_xml)
     {
        guint32 remaining;

        didl_process(didl_xml, priv->udn, priv);
        g_free(didl_xml);

        /* See if we have more objects to get */
        priv->starting_index += number_returned;
        remaining = total_matches - priv->starting_index;

        /* Keep browsing till we get each and every object */
        if (remaining)
          {
             upnp_browse(priv->starting_index,
                         MIN(remaining, UPNP_MAX_BROWSE), priv);
          }
     }
}

static void
upnp_browse(guint32 starting_index, guint32 requested_count,
            upnp_filter_arg_t *priv)
{

   priv->browse_action = gupnp_service_proxy_begin_action(
                                                          priv->content_dir, "Browse", _upnp_browse_cb, priv,
                                                          /* IN args */
                                                          "ObjectID", G_TYPE_STRING, priv->container_id,
                                                          "BrowseFlag", G_TYPE_STRING, "BrowseDirectChildren",
                                                          "Filter", G_TYPE_STRING, "@childCount",
                                                          "StartingIndex", G_TYPE_UINT, starting_index,
                                                          "RequestedCount", G_TYPE_UINT, requested_count,
                                                          "SortCriteria",  G_TYPE_STRING, "",
                                                          NULL);
}


static void *
_add(Eina_List *tokens, Enna_Browser *browser, ENNA_VFS_CAPS caps)
{

   upnp_filter_arg_t *priv;
   upnp_media_server_t *srv;
   size_t res;
   char udn[512], id[512];
   Eina_List *l, *l_tmp = NULL;
   Enna_Buffer *path;
   const char *container_id = NULL;

   const char *tmp;
   int i = 0;

   path = enna_buffer_new();
   l_tmp = eina_list_nth_list(tokens, 2);
   EINA_LIST_FOREACH(l_tmp, l, tmp)
     {
        if (!i)
          enna_buffer_appendf(path, "%s", tmp);
        else
          enna_buffer_appendf(path, "/%s", tmp);
        i++;

     }

   if (!path->buf)
     {
        /* ROOT */
        container_id = eina_stringshare_add(UPNP_DEFAULT_ROOT);
        udn[0] = '\0';
        id[0] = '\0';
     }
   else
     {
        res = sscanf(path->buf, "udn:%[^,],id:%[^,]", udn, id);
        if (res != 2)
          {
             return NULL;
          }
        container_id = eina_stringshare_add(id);
     }



   EINA_LIST_FOREACH(mod->devices, l, srv)
     {
        if (!strcmp(srv->udn, udn))
          break;
     }


   /* no server to browse */
   if (!srv && eina_list_count(tokens) != 2)
     {
        return NULL;
     }

   priv = calloc(1, sizeof(upnp_filter_arg_t));
   priv->browser = browser;
   priv->cap = caps;
   if (srv)
     priv->content_dir = srv->content_dir;
   priv->container_id = eina_stringshare_add(container_id);//"/g/52/a/409/l/329/t");
   priv->starting_index = 0;
   priv->udn = eina_stringshare_add(udn);

   return priv;
}

static void
_get_children(void *priv, Eina_List *tokens, Enna_Browser *browser, ENNA_VFS_CAPS caps)
{
   upnp_filter_arg_t *p = priv;
   if (eina_list_count(tokens) == 2 )
     {
        Eina_List *l;
        upnp_media_server_t *srv;

        EINA_LIST_FOREACH(mod->devices, l, srv)
          {

             char name[256], uri[1024];
             Enna_File *f;

             snprintf(name, sizeof(name), "%s (%s)", srv->name, srv->model);
             snprintf(uri, sizeof(uri), "/%s/upnp/udn:%s,id:%s",
                      "explorer",
                      srv->udn, UPNP_DEFAULT_ROOT);
             f = enna_file_directory_add(name, uri, NULL, name, "icon/dev/nfs");
             enna_browser_file_add(browser, f);
          }
     }
   else
     {
        upnp_browse(0, UPNP_MAX_BROWSE, p);
     }
}

static void
_del(void *priv)
{
   upnp_filter_arg_t *p = priv;

   if (!p)
     return;

   if (p->current_id)
     eina_stringshare_del(p->current_id);

   if (p->container_id)
     eina_stringshare_del(p->container_id);
   if (p->udn)
     eina_stringshare_del(p->udn);

   gupnp_service_proxy_cancel_action(p->content_dir, p->browse_action);
   free(p);
}


static Enna_Vfs_Class class = {
  ENNA_MODULE_NAME,
  10,
  "UPnP/DLNA media servers",
  NULL,
  "icon/upnp",
  {
    _add,
    _get_children,
    _del
  },
  NULL
};


/* Device Callbacks */

static void
upnp_add_device(GUPnPControlPoint *cp, GUPnPDeviceProxy  *proxy)
{
   const char *type, *location, *udn;
   char *name, *model;
   upnp_media_server_t *srv;
   GUPnPServiceInfo *si;
   Eina_List *l;


   type = gupnp_device_info_get_device_type(GUPNP_DEVICE_INFO(proxy));
   if (!g_pattern_match_simple("urn:schemas-upnp-org:device:MediaServer:*", type))
     return;

   location = gupnp_device_info_get_location(GUPNP_DEVICE_INFO(proxy));
   udn      = gupnp_device_info_get_udn(GUPNP_DEVICE_INFO(proxy));
   name     = gupnp_device_info_get_friendly_name(GUPNP_DEVICE_INFO(proxy));
   model    = gupnp_device_info_get_model_name(GUPNP_DEVICE_INFO(proxy));

   pthread_mutex_lock(&mod->mutex);
   /* check if device is already known */
   EINA_LIST_FOREACH(mod->devices, l, srv)
     {
        if (location && !strcmp(srv->location, location))
          {
             return;
          }
     }

   srv              = calloc(1, sizeof(upnp_media_server_t));
   srv->info        = GUPNP_DEVICE_INFO(proxy);
   si               = gupnp_device_info_get_service(srv->info,
                                                    UPNP_CONTENT_DIR);
   srv->content_dir = GUPNP_SERVICE_PROXY(si);
   srv->type        = type     ? strdup(type)     : NULL;
   srv->location    = location ? strdup(location) : NULL;
   srv->udn         = udn      ? strdup(udn)      : NULL;
   srv->name        = name     ? strdup(name)     : NULL;
   srv->model       = model    ? strdup(model)    : NULL;

   mod->devices = eina_list_append(mod->devices, srv);
   pthread_mutex_unlock(&mod->mutex);
}

/* Module interface */

#ifdef USE_STATIC_MODULES
#undef MOD_PREFIX
#define MOD_PREFIX enna_mod_browser_upnp
#endif /* USE_STATIC_MODULES */
static void
_context_available_cb(GUPnPContextManager *context_manager,
                      GUPnPContext        *context,
                      gpointer             user_data)
{
   GUPnPControlPoint *dms_cp;


   dms_cp = gupnp_control_point_new (context, UPNP_MEDIA_SERVER);

   pthread_mutex_init(&mod->mutex, NULL);
   pthread_mutex_init(&mod->mutex_id, NULL);

   g_signal_connect (dms_cp,
                     "device-proxy-available",
                     G_CALLBACK (upnp_add_device),
                     NULL);

   gssdp_resource_browser_set_active (GSSDP_RESOURCE_BROWSER (dms_cp),
                                      TRUE);

   /* Let context manager take care of the control point life cycle */
   gupnp_context_manager_manage_control_point (context_manager, dms_cp);

   /* We don't need to keep our own references to the control points */
   g_object_unref (dms_cp);

}

static void
module_init(Enna_Module *em)
{
   if (!em)
     return;

   mod = calloc(1, sizeof(Enna_Module_UPnP));
   mod->em = em;
   em->mod = mod;

   if (!ecore_main_loop_glib_integrate())
     {
        return;
     }

   g_thread_init(NULL);
   g_type_init();

   mod->ctxm = gupnp_context_manager_new (NULL, 0);
   if (!mod->ctxm)
     return;

   g_signal_connect (mod->ctxm,
                     "context-available",
                     G_CALLBACK (_context_available_cb),
                     NULL);


   enna_vfs_register(&class, ENNA_CAPS_ALL);
}

static void
module_shutdown(Enna_Module *em)
{
   Enna_Module_UPnP *mod;
   upnp_media_server_t *srv;
   Eina_List *l;

   mod = em->mod;

   gssdp_resource_browser_set_active(GSSDP_RESOURCE_BROWSER(mod->cp), FALSE);

   EINA_LIST_FOREACH(mod->devices, l, srv)
     upnp_media_server_free(srv);
   g_object_unref(mod->cp);
   g_object_unref(mod->ctx);

   upnp_current_id_set(NULL);
   pthread_mutex_destroy(&mod->mutex);
   pthread_mutex_destroy(&mod->mutex_id);
}

Enna_Module_Api ENNA_MODULE_API =
  {
    ENNA_MODULE_VERSION,
    "browser_upnp",
    "UPnP/DLNA module",
    "icon/module",
    "This module allows Enna to browse contents from UPnP/DLNA media servers",
    "bla bla bla<br><b>bla bla bla</b><br><br>bla.",
    {
      module_init,
      module_shutdown
    }
  };


