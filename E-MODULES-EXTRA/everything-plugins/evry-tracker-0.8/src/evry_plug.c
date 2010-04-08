#include <Evry.h>
#include "md5.h"

typedef struct _Plugin Plugin;

struct _Plugin
{
  Evry_Plugin base;
  const char *input;
  
  char *condition;
  char *service;
  Eina_List *files;

  DBusPendingCall *pnd;
};

static E_DBus_Connection *conn = NULL;
static Eina_List *plugins = NULL;
static int _prio = 5;
static int active = 0;

static DBusPendingCall *pending_get_name_owner = NULL;
static E_DBus_Signal_Handler *cb_name_owner_changed = NULL;
static const char bus_name[] = "org.freedesktop.Tracker1";
static const char fdo_bus_name[] = "org.freedesktop.DBus";
static const char fdo_interface[] = "org.freedesktop.DBus";
static const char fdo_path[] = "/org/freedesktop/DBus";
static const char query_files[] =  "SELECT ?s nie:url(?s) nie:mimeType(?s) WHERE { ?s fts:match \"%s\". ?s a nfo:FileDataObject } limit 100";
static const char *mime_dir = NULL;

static Evry_Plugin *
_begin(Evry_Plugin *plugin, const Evry_Item *it)
{
   PLUGIN(p, plugin);
  
   return plugin;
}

static void
_item_free(Evry_Item *it)
{
   ITEM_FILE(file, it);
   if (file->path) eina_stringshare_del(file->path);
   if (file->url) eina_stringshare_del(file->url);
   if (file->mime) eina_stringshare_del(file->mime);

   E_FREE(file);
}

static Evry_Item_File *
_item_add(Plugin *p, char *id, char *url, char *mime, int prio)
{
   Evry_Item_File *file;

   char *path;
   const char *label, *tmp;

   if (!strncmp(url, "file://", 7))
     tmp = url + 7;
   else return NULL;
   
   if (!(path = evry_util_unescape(tmp, 0)))
     return NULL;

   if (!(label = ecore_file_file_get(path)))
     return NULL;
   
   if (!(file = E_NEW(Evry_Item_File, 1)))
     {
	free(path);
	return NULL;
     }

   evry_item_new(EVRY_ITEM(file), EVRY_PLUGIN(p), label, _item_free);
   EVRY_ITEM(file)->id = eina_stringshare_add(id);
   int match = evry_fuzzy_match(label, p->input);
   if (match)
     EVRY_ITEM(file)->fuzzy_match = match;
   else
     EVRY_ITEM(file)->fuzzy_match = 100;
   
   file->path = eina_stringshare_add(path);
   file->mime = eina_stringshare_add(mime);
   file->url = eina_stringshare_add(url);
   
   if (file->mime == mime_dir)
     {
	EVRY_ITEM(file)->browseable = EINA_TRUE;
	EVRY_ITEM(file)->priority = 1;
     }

   free(path);
   
   return file;
}

static void
_cleanup(Evry_Plugin *plugin)
{
   PLUGIN(p, plugin);
   Evry_Item_File *file;

   EINA_LIST_FREE(p->files, file)
     evry_item_free(EVRY_ITEM(file));

   if (p->input)
     eina_stringshare_del(p->input);
   p->input = NULL;

   if (p->pnd)
     dbus_pending_call_cancel(p->pnd);
   p->pnd = NULL;

   EVRY_PLUGIN_ITEMS_CLEAR(p);
}

static int
_cb_sort(const void *data1, const void *data2)
{
   const Evry_Item *it1, *it2;

   it1 = data1;
   it2 = data2;

   if (it1->priority - it2->priority)
     return (it1->priority - it2->priority);

   return strcasecmp(it1->label, it2->label);
}

static void
_dbus_cb_reply(void *data, DBusMessage *msg, DBusError *error)
{
   DBusMessageIter array, iter, item;
   char *path, *mime, *id;
   Evry_Item_File *file;
   Eina_List *files = NULL;
   Plugin *p = data;

   p->pnd = NULL;
   
   if (dbus_error_is_set(error))
     {
	ERR("%s - %s\n", error->name, error->message);
	return;
     }

   dbus_message_iter_init(msg, &array);
   if (dbus_message_iter_get_arg_type(&array) == DBUS_TYPE_ARRAY)
     {
	dbus_message_iter_recurse(&array, &item);
	while (dbus_message_iter_get_arg_type(&item) == DBUS_TYPE_ARRAY)
	  {
	     dbus_message_iter_recurse(&item, &iter);

	     if (dbus_message_iter_get_arg_type(&iter) == DBUS_TYPE_STRING)
	       {
		  dbus_message_iter_get_basic(&iter, &id);
		  /* printf("id %s\n", id); */

		  dbus_message_iter_next(&iter);
		  dbus_message_iter_get_basic(&iter, &path);
		  /* printf("path %s\n", path); */

		  dbus_message_iter_next(&iter);
		  dbus_message_iter_get_basic(&iter, &mime);
		  /* printf("mime %s\n", mime); */
		  
		  if (id && path && mime)
		    {
		       file = _item_add(p, id, path, mime, 0);
		       if (file) files = eina_list_append(files, file);
		    }
	       }
	     dbus_message_iter_next(&item);
	  }
     }

   EVRY_PLUGIN_ITEMS_CLEAR(p);

   if (files)
     {
	Eina_List *l;

	EINA_LIST_FREE(p->files, file)
	  evry_item_free(EVRY_ITEM(file));

	files = eina_list_sort(files, eina_list_count(files), _cb_sort);
	p->files = files;

	EINA_LIST_FOREACH(p->files, l, file)
	  EVRY_PLUGIN_ITEM_APPEND(p, file);
     }

   evry_plugin_async_update(EVRY_PLUGIN(p), EVRY_ASYNC_UPDATE_ADD);
}

static int
_fetch(Evry_Plugin *plugin, const char *input)
{
   if (active)
     {
	PLUGIN(p, plugin);

	DBusMessage *msg;
	char *query;

	if (p->pnd)
	  dbus_pending_call_cancel(p->pnd);
	p->pnd = NULL;
	
	if (input && (strlen(input) > 2))
	  {
	     query = malloc(sizeof(char) * (strlen(input) + strlen(query_files)));
	     sprintf(query, query_files, input);

	     if (p->input)
	       eina_stringshare_del(p->input);
   
	     p->input = eina_stringshare_add(input); 
	  }
	else
	  {
	     _cleanup(plugin);
	     return 0;
	  }

	msg = dbus_message_new_method_call(bus_name,
					   "/org/freedesktop/Tracker1/Resources",
					   "org.freedesktop.Tracker1.Resources",
					   "SparqlQuery");
	dbus_message_append_args(msg,
				 DBUS_TYPE_STRING,  &query,
				 DBUS_TYPE_INVALID);

	p->pnd = e_dbus_message_send(conn, msg, _dbus_cb_reply, -1, p);
	dbus_message_unref(msg);

	E_FREE(query);
	
	if (p->files) return 1;
     }

   return 0;
}

static Evas_Object *
_icon_get(Evry_Plugin *p __UNUSED__, const Evry_Item *it, Evas *e)
{
   ITEM_FILE(file, it);
   char buf[4096];
   MD5_CTX ctx;
   char md5out[(2 * MD5_HASHBYTES) + 1];
   unsigned char hash[MD5_HASHBYTES];
   int n;
   static const char hex[] = "0123456789abcdef";
   
   MD5Init (&ctx);
   MD5Update (&ctx, (unsigned char const*)file->url, (unsigned)strlen (file->url));
   MD5Final (hash, &ctx);

   for (n = 0; n < MD5_HASHBYTES; n++)
     {
	md5out[2 * n] = hex[hash[n] >> 4];
	md5out[2 * n + 1] = hex[hash[n] & 0x0f];
     }
   md5out[2 * n] = '\0';

   snprintf(buf, sizeof(buf), "%s/.thumbnails/normal/%s.png", e_user_homedir_get(), md5out);
   
   DBG("load thumb: %s - %s\n", buf, file->path);
   if (ecore_file_exists(buf))
     {
	Evas_Object *o = e_icon_add(e);
	e_icon_file_set(o, buf);
	if (o) return o;
     }

   if (it->browseable)
     return evry_icon_theme_get("folder", e);
   else
     return evry_icon_mime_get(file->mime, e);

   return NULL;
}

static void
_plugin_new(const char *name, int type, char *service, int begin)
{
   Plugin *p;

   p = E_NEW(Plugin, 1);
   p->condition = "";
   p->service = service;

   if (!begin)
     evry_plugin_new(EVRY_PLUGIN(p), name, type, "", "FILE", 1, NULL, NULL,
		     NULL, _cleanup, _fetch,
		     NULL, _icon_get, NULL, NULL);
   else if (type == type_object)
     evry_plugin_new(EVRY_PLUGIN(p), name, type, "APPLICATION", "FILE", 1, NULL, NULL,
		   _begin, _cleanup, _fetch,
		   NULL, _icon_get, NULL, NULL);

   plugins = eina_list_append(plugins, p);

   evry_plugin_register(EVRY_PLUGIN(p), _prio++);
}

static void
_name_owner_changed(void *data __UNUSED__, DBusMessage *msg)
{
   DBusError err;
   const char *name, *from, *to;

   dbus_error_init(&err);
   if (!dbus_message_get_args(msg, &err,
			      DBUS_TYPE_STRING, &name,
			      DBUS_TYPE_STRING, &from,
			      DBUS_TYPE_STRING, &to,
			      DBUS_TYPE_INVALID))
     {
	ERR("could not get NameOwnerChanged arguments: %s: %s",
	    err.name, err.message);
	dbus_error_free(&err);
	return;
     }

   if (strcmp(name, bus_name) != 0)
     return;

   DBG("NameOwnerChanged from=[%s] to=[%s]", from, to);

   if (to[0] == '\0')
     active = EINA_FALSE;
   else
     active = EINA_TRUE;
}

static void
_get_name_owner(void *data __UNUSED__, DBusMessage *msg, DBusError *err)
{
   DBusMessageIter itr;
   int t;
   const char *uid;

   pending_get_name_owner = NULL;

   if (dbus_error_is_set(err))
     {
        ERR("request name error: %s", err->message);
        return;
     }

   if (!dbus_message_iter_init(msg, &itr))
     return;

   t = dbus_message_iter_get_arg_type(&itr);
   if (t != DBUS_TYPE_STRING)
     return;

   dbus_message_iter_get_basic(&itr, &uid);

   if (uid)
     active = EINA_TRUE;

   return;
}

static Eina_Bool
_init(void)
{
  if (!evry_api_version_check(EVRY_API_VERSION))
    return EINA_FALSE;

  conn = e_dbus_bus_get(DBUS_BUS_SESSION);

   if (!conn) return EINA_FALSE;

   cb_name_owner_changed = e_dbus_signal_handler_add
     (conn, fdo_bus_name, fdo_path, fdo_interface, "NameOwnerChanged",
      _name_owner_changed, NULL);

   pending_get_name_owner = e_dbus_get_name_owner
     (conn, bus_name, _get_name_owner, NULL);

   _plugin_new("Tracker", type_subject,  "nfo:FileDataObject", 0);
   /* _plugin_new("Tracker", type_object,   "nfo:FileDataObject", 1); */
   
   mime_dir = eina_stringshare_add("inode/directory");
   
   return EINA_TRUE;
}

static void
_shutdown(void)
{
   Plugin *p;

   if (conn) e_dbus_connection_close(conn);

   EINA_LIST_FREE(plugins, p)
     {
	if (p->condition[0]) free(p->condition);

	EVRY_PLUGIN_FREE(p);
     }

   eina_stringshare_del(mime_dir); 
}


EINA_MODULE_INIT(_init);
EINA_MODULE_SHUTDOWN(_shutdown);


