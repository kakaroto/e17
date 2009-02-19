/*
 * UPnP/DLNA browser module
 *  Benjamin Zores (C) 2009
 *
 * TODO:
 *  - Add device shutdown discovery (remove from list)
 *  - Fix potential segv if browsed too fast (mutex issue i'd say).
 *  - Fix browse down feature (probably bad parent_id).
 *
 */

#include "enna.h"

#include <libgupnp/gupnp-control-point.h>
#include <libgupnp-av/gupnp-av.h>
#include <string.h>
#include <pthread.h>

#define ENNA_MODULE_NAME  "upnp"

#define UPNP_MEDIA_SERVER   "urn:schemas-upnp-org:device:MediaServer:*"
#define UPNP_CONTENT_DIR    "urn:schemas-upnp-org:service:ContentDirectory"
#define UPNP_MAX_BROWSE     1024
#define UPNP_DEFAULT_ROOT   "0"
#define ITEM_CLASS_IMAGE    "object.item.imageItem"
#define ITEM_CLASS_AUDIO    "object.item.audioItem"
#define ITEM_CLASS_VIDEO    "object.item.videoItem"
#define ITEM_CLASS_TEXT     "object.item.textItem"

typedef struct Enna_Module_UPnP_s
{
    Evas *e;
    Enna_Module *em;
    pthread_mutex_t mutex;
    Ecore_List *devices;
    Ecore_Idler *idler;
    GMainContext *mctx;
    GUPnPContext *ctx;
    GUPnPControlPoint *cp;
    char *prev_id;
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
upnp_media_server_free (upnp_media_server_t *srv)
{
    if (!srv)
        return;

    g_object_unref (srv->info);
    g_object_unref (srv->content_dir);

    if (srv->type)
        free (srv->type);
    if (srv->location)
        free (srv->location);
    if (srv->udn)
        free (srv->udn);
    if (srv->name)
        free (srv->name);
    if (srv->model)
        free (srv->model);
    free (srv);
}

static gboolean
upnp_gloop_prepare (GSource *source, gint *timeout)
{
    return TRUE;
}

static gboolean
upnp_gloop_check (GSource *source)
{
    return TRUE;
}

static gboolean
upnp_gloop_dispatch (GSource *source,
                     GSourceFunc callback, gpointer user_data)
{
    ecore_main_loop_iterate ();
    return TRUE;
}

static GSourceFuncs
upnp_gloop_source_funcs = {
    upnp_gloop_prepare,
    upnp_gloop_check,
    upnp_gloop_dispatch,
    NULL,
    NULL,
    NULL
};

static void
upnp_gloop_main_begin (GMainContext *ctx)
{
    GSource *s = g_source_new (&upnp_gloop_source_funcs, sizeof (GSource));
    g_source_attach (s, ctx);
}

static int
upnp_gloop_idler (void *data)
{
    GMainContext *mctx = data;
    g_main_context_iteration (mctx, FALSE);
    return 1; /* keep going */
}

static xmlNode *
xml_util_get_element (xmlNode *node, ...)
{
    va_list var_args;

    va_start (var_args, node);

    while (1)
    {
        const char *arg;

        arg = va_arg (var_args, const char *);
        if (!arg)
            break;

        for (node = node->children; node; node = node->next)
        {
            if (!node->name)
                continue;

            if (!strcmp (arg, (char *) node->name))
                break;
        }

        if (!node)
            break;
    }

    va_end (var_args);

    return node;
}

static Enna_Vfs_File *
didl_process_object (xmlNode *e, char *udn)
{
    char *id, *parent_id, *title;
    gboolean is_container;
    Enna_Vfs_File *f = NULL;

    id = gupnp_didl_lite_object_get_id (e);
    if (!id)
        goto err_id;

    title = gupnp_didl_lite_object_get_title (e);
    if (!title)
        goto err_title;

    parent_id = gupnp_didl_lite_object_get_parent_id (e);
    if (!parent_id)
        goto err_parent;

    is_container = gupnp_didl_lite_object_is_container (e);


    if (is_container)
    {
        char uri[1024];

        memset (uri, '\0', sizeof (uri));
        snprintf (uri, sizeof (uri),
                  "udn:%s,id:%s,parent_id:%s", udn, id, parent_id);

        f = enna_vfs_create_directory (uri, title, "icon/directory", NULL);

        enna_log (ENNA_MSG_EVENT, ENNA_MODULE_NAME,
                  "DIDL container '%s' (id: %s, parent_id: %s, uri: %s)\n",
                  title, id, parent_id, uri);
    }
    else
    {
        char *class_name, *uri;
        char *icon = NULL;
        GList *resources;
        xmlNode *n;

        class_name = gupnp_didl_lite_object_get_upnp_class (e);
        if (!class_name)
            goto err_no_class;

        if (!strcmp (class_name, ITEM_CLASS_IMAGE))
            icon = "icon/photo";
        else if (!strcmp (class_name, ITEM_CLASS_AUDIO))
            icon = "icon/music";
        else if (!strcmp (class_name, ITEM_CLASS_VIDEO))
            icon = "icon/video";

        g_free (class_name);

    err_no_class:
        if (!icon)
            icon = "icon/hd";

        resources = gupnp_didl_lite_object_get_property (e, "res");
        if (!resources)
            goto err_resources;

        n = (xmlNode *) resources->data;
        if (!n)
            goto err_res_node;

        uri = gupnp_didl_lite_property_get_value (n);
        if (!uri)
            goto err_no_uri;

        f = enna_vfs_create_file (uri, title, icon, NULL);

        enna_log (ENNA_MSG_EVENT, ENNA_MODULE_NAME,
                  "DIDL item '%s' (id: %s, parent_id: %s, uri: %s)\n",
                  title, id, parent_id, uri);

    err_no_uri:
    err_res_node:
        g_list_free (resources);
    }

 err_resources:
 err_parent:
    g_free (title);
 err_title:
    g_free (id);
 err_id:
    return f;
}

static Eina_List *
didl_process (char *didl, char *udn)
{
    xmlNode *element;
    xmlDoc  *doc;
    Eina_List *list = NULL;

    doc = xmlParseMemory (didl, strlen (didl));
    if (!doc)
    {
        enna_log (ENNA_MSG_ERROR, ENNA_MODULE_NAME,
                  "Unable to parse DIDL-Lite XML:\n%s\n", didl);
        return NULL;
    }

    /* Get a pointer to root element */
    element = xml_util_get_element ((xmlNode *) doc, "DIDL-Lite", NULL);
    if (!element)
    {
        enna_log (ENNA_MSG_ERROR, ENNA_MODULE_NAME,
                  "No 'DIDL-Lite' node found.\n");
        xmlFreeDoc (doc);
        return NULL;
    }

    for (element = element->children; element; element = element->next)
    {
        const char *name = (const char *) element->name;

        if (!g_ascii_strcasecmp (name, "container") ||
            !g_ascii_strcasecmp (name, "item"))
        {
            Enna_Vfs_File *f;

            f = didl_process_object (element, udn);
            if (f)
                list = eina_list_append (list, f);
        }
    }

    xmlFreeDoc (doc);
    return list;
}

static Eina_List *
upnp_browse (upnp_media_server_t *srv, const char *container_id,
             guint32 starting_index, guint32 requested_count)
{
    guint32 number_returned, total_matches, si;
    Eina_List *results = NULL;
    char *didl_xml;
    GError *error;

    g_object_ref (srv->content_dir);
    didl_xml = NULL;
    error = NULL;

    si = starting_index;

    if (!srv)
        return NULL;

    /* UPnP Browse request */
    gupnp_service_proxy_send_action
        (srv->content_dir, "Browse", &error,
         /* IN args */
         "ObjectID",       G_TYPE_STRING, container_id,
         "BrowseFlag",     G_TYPE_STRING, "BrowseDirectChildren",
         "Filter",         G_TYPE_STRING, "*",
         "StartingIndex",  G_TYPE_UINT,   starting_index,
         "RequestedCount", G_TYPE_UINT,   requested_count,
         "SortCriteria",   G_TYPE_STRING, "",
         NULL,
         /* OUT args */
         "Result",         G_TYPE_STRING, &didl_xml,
         "NumberReturned", G_TYPE_UINT,   &number_returned,
         "TotalMatches",   G_TYPE_UINT,   &total_matches,
         NULL);

    /* check action result */
    if (error)
    {
        GUPnPServiceInfo *info;

        info = GUPNP_SERVICE_INFO (srv->content_dir);
        enna_log (ENNA_MSG_ERROR, ENNA_MODULE_NAME,
                  "Failed to browse '%s': %s\n",
                  gupnp_service_info_get_location (info), error->message);
        g_error_free (error);
        goto err;
    }

    if (didl_xml)
    {
        guint32 remaining;
        Eina_List *list;

        list = didl_process (didl_xml, srv->udn);
        results = eina_list_merge (results, list);
        g_free (didl_xml);

        /* See if we have more objects to get */
        si += number_returned;
        remaining = total_matches - si;

        /* Keep browsing till we get each and every object */
        if (remaining)
        {
            list = upnp_browse (srv, container_id, si,
                                MIN (remaining, UPNP_MAX_BROWSE));
            results = eina_list_merge (results, list);
        }
    }

 err:
    g_object_unref (srv->content_dir);
    return results;
}

static Eina_List *
browse_server_list (const char *uri, int parent)
{
    upnp_media_server_t *srv = NULL;
    char udn[512], id[256], parent_id[256];
    int i, res, count;

    memset (udn, '\0', sizeof (udn));
    memset (id, '\0', sizeof (id));

    if (!uri)
        return NULL;

    res = sscanf (uri, "udn:%[^,],id:%[^,],parent_id:%s", udn, id, parent_id);
    if (res != 3)
        return NULL;

    count = ecore_list_count (mod->devices);
    for (i = 0; i < count; i++)
    {
        srv = ecore_list_index_goto (mod->devices, i);
        if (!srv)
            continue;

        if (!strcmp (srv->udn, udn))
            break;

        srv = NULL;
    }

    /* no server to browse */
    if (!srv)
        return NULL;

    return upnp_browse (srv, parent ? parent_id : id, 0, UPNP_MAX_BROWSE);
}

static Eina_List *
upnp_list_mediaservers (void)
{
    Eina_List *servers = NULL;
    int i, count;

    count = ecore_list_count (mod->devices);
    for (i = 0; i < count; i++)
    {
        upnp_media_server_t *srv;
        char name[256], uri[1024];
        Enna_Vfs_File *f;

        srv = ecore_list_index_goto (mod->devices, i);
        if (!srv)
            continue;

        memset (name, '\0', sizeof (name));
        snprintf (name, sizeof (name), "%s (%s)", srv->name, srv->model);

        memset (uri, '\0', sizeof (uri));
        snprintf (uri, sizeof (uri),
                  "udn:%s,id:%s,parent_id:%s",
                  srv->udn, UPNP_DEFAULT_ROOT, UPNP_DEFAULT_ROOT);

        f = enna_vfs_create_directory (uri, name, "icon/dev/nfs", NULL);
        servers = eina_list_append (servers, f);
    }

    return servers;
}

static void
upnp_add_device (GUPnPControlPoint *cp, GUPnPDeviceProxy  *proxy)
{
    const char *type, *location, *udn;
    char *name, *model;
    upnp_media_server_t *srv;
    GUPnPServiceInfo *si;

    type = gupnp_device_info_get_device_type (GUPNP_DEVICE_INFO (proxy));
    if (!g_pattern_match_simple (UPNP_MEDIA_SERVER, type))
        return;

    location = gupnp_device_info_get_location (GUPNP_DEVICE_INFO (proxy));
    udn = gupnp_device_info_get_udn (GUPNP_DEVICE_INFO (proxy));
    name = gupnp_device_info_get_friendly_name (GUPNP_DEVICE_INFO (proxy));
    model = gupnp_device_info_get_model_name (GUPNP_DEVICE_INFO (proxy));

    /* check if device is already known */
    ecore_list_first_goto (mod->devices);
    while ((srv = ecore_list_next (mod->devices)))
        if (!strcmp (srv->location, location))
            return;

    srv = calloc (1, sizeof (upnp_media_server_t));
    srv->info = GUPNP_DEVICE_INFO (proxy);
    si = gupnp_device_info_get_service (srv->info, UPNP_CONTENT_DIR);
    srv->content_dir = GUPNP_SERVICE_PROXY (si);
    srv->type = strdup (type);
    srv->location = strdup (location);
    srv->udn = strdup (udn);
    srv->name = strdup (name);
    srv->model = strdup (model);

    pthread_mutex_lock (&mod->mutex);
    ecore_list_append (mod->devices, srv);
    pthread_mutex_unlock (&mod->mutex);

    enna_log (ENNA_MSG_EVENT, ENNA_MODULE_NAME,
              "Found news UPnP device '%s (%s)'\n", name, model);
}

static Eina_List *
_class_browse_up (const char *id, void *cookie)
{
    Eina_List *l;

    ENNA_FREE (mod->prev_id);
    if (!id)
    {
        /* list available UPnP media servers */
        l = upnp_list_mediaservers ();
        ENNA_FREE (mod->prev_id);
        mod->prev_id = NULL;
    }
    else
    {
        /* browse content from media server */
        l = browse_server_list (id, 0);
        mod->prev_id = strdup (id);
    }

    return l;
}

static Eina_List *
_class_browse_down (void *cookie)
{
    if (mod->prev_id)
        return browse_server_list (mod->prev_id, 1);

    return upnp_list_mediaservers ();
}

static Enna_Vfs_File *
_class_vfs_get (void *cookie)
{
    return enna_vfs_create_directory (NULL, NULL,
            evas_stringshare_add ("icon/upnp"), NULL);
}

static Enna_Class_Vfs class_upnp = {
    ENNA_MODULE_NAME,
    1,
    "UPnP/DLNA Media Servers",
    NULL,
    "icon/upnp",
    {
        NULL,
        NULL,
        _class_browse_up,
        _class_browse_down,
        _class_vfs_get,
    },
    NULL
};

/* Module interface */

Enna_Module_Api module_api =
{
    ENNA_MODULE_VERSION,
    ENNA_MODULE_BROWSER,
    "browser_upnp"
};

void module_init (Enna_Module *em)
{
    int flags = ENNA_CAPS_MUSIC | ENNA_CAPS_VIDEO | ENNA_CAPS_PHOTO;
    GError *error;

    if (!em)
        return;

    mod = calloc (1, sizeof (Enna_Module_UPnP));
    mod->em = em;
    em->mod = mod;

    g_thread_init (NULL);
    g_type_init ();

    pthread_mutex_init (&mod->mutex, NULL);
    mod->devices = ecore_list_new ();
    ecore_list_free_cb_set (mod->devices,
                            ECORE_FREE_CB (upnp_media_server_free));


    /* bind upnp context to ecore */
    mod->mctx = g_main_context_default ();
    upnp_gloop_main_begin (mod->mctx);
    mod->idler = ecore_idler_add (upnp_gloop_idler, mod->mctx);

    error = NULL;
    mod->ctx = gupnp_context_new (mod->mctx, NULL, 0, &error);
    if (error)
    {
        g_error_free (error);
        return;
    }

    mod->cp = gupnp_control_point_new (mod->ctx, GSSDP_ALL_RESOURCES);
    if (!mod->cp)
    {
        g_object_unref (mod->ctx);
        return;
    }

    g_signal_connect (mod->cp, "device-proxy-available",
                      G_CALLBACK (upnp_add_device), NULL);

    gssdp_resource_browser_set_active
        (GSSDP_RESOURCE_BROWSER (mod->cp), TRUE);

    enna_vfs_append (ENNA_MODULE_NAME, flags, &class_upnp);
}

void module_shutdown (Enna_Module *em)
{
    Enna_Module_UPnP *mod;

    mod = em->mod;

    gssdp_resource_browser_set_active
        (GSSDP_RESOURCE_BROWSER (mod->cp), FALSE);

    g_object_unref (mod->cp);
    g_object_unref (mod->ctx);

    ecore_idler_del (mod->idler);

    ENNA_FREE (mod->prev_id);
    pthread_mutex_destroy (&mod->mutex);
}
