/*
 * vim:ts=8:sw=3:sts=8:noexpandtab:cino=>5n-3f0^-2{2
 */

#include <Evry.h>
#include "md5.h"
#include "e_mod_main.h"

#define QUERY_ITEM(_q, _it) Query_Item *_q = (Query_Item *) _it;

typedef struct _Plugin Plugin;
typedef struct _Query_Item Query_Item;

struct _Plugin
{
  Evry_Plugin base;
  const char *input;
  
  char *condition;
  char *service;
  Eina_List *files;

  DBusPendingCall *pnd;

  Eina_Bool fetching;
  const char *match;
  const char *query;
  Eina_Bool category;  
};

struct _Query_Item
{
  Evry_Item base;
  Eina_Bool category;
  const char *query;
  const char *match;
};

static E_DBus_Connection *conn = NULL;
static Eina_List *plugins = NULL;
static int _prio = 5;
static int active = 0;
static const char *mime_dir = NULL;
static DBusPendingCall *pending_get_name_owner = NULL;
static E_DBus_Signal_Handler *cb_name_owner_changed = NULL;
static const char bus_name[] = "org.freedesktop.Tracker1";
static const char fdo_bus_name[] = "org.freedesktop.DBus";
static const char fdo_interface[] = "org.freedesktop.DBus";
static const char fdo_path[] = "/org/freedesktop/DBus";

static const char query_files[] =
  "SELECT ?s nie:url(?s) nfo:fileName(?s) ?m WHERE"
  "{ ?s fts:match \"%s\". ?s a nfo:FileDataObject;"
  "     tracker:available true;"
  "     nie:mimeType ?m"
  "}  order by desc (fn:starts-with(?m, 'inode/directory')) limit 100";

static const char query_artists[] =
  "SELECT ?a WHERE { ?a fts:match \"%s\"."
  "?a a nmm:Artist } LIMIT 50";

static const char query_albums[] =
  "SELECT ?a  nmm:artistName(?p) where {  ?a fts:match \"%s\"."
  " { SELECT DISTINCT ?a ?p WHERE { "
  "   ?s a nmm:MusicPiece; "
  "        nmm:musicAlbum ?a;"
  "        nmm:performer ?p }}} LIMIT 50";

static const char query_genres[] =
  "SELECT DISTINCT ?g WHERE { ?s a nmm:MusicPiece; nfo:genre ?g } LIMIT 200"

  "SELECT ?a WHERE { ?a fts:match \"%s\"."
  "?a a nmm:Artist } LIMIT 50";

static const char query_albums_for_artist[] =
  "SELECT ?match nmm:artistName(?p) WHERE { "
  " %s" 
  " { SELECT DISTINCT ?match ?p WHERE "
  "  { ?s a nmm:MusicPiece; nmm:musicAlbum ?match;"
  "         nmm:performer [ nmm:artistName \"%s\" ]; "
  "         nmm:performer ?p}}"
  "} LIMIT 50";

static const char query_tracks[] =
  "SELECT ?a ?url nfo:fileName(?a) nie:mimeType(?a) WHERE "
  "{ ?a fts:match \"%s*\" . "
  "  ?a a nmm:MusicPiece;  "
  "       nie:isStoredAs ?as ."
  "  ?as nie:url ?url . "
  "} LIMIT 50";

static const char query_tracks_for_album[] =
  "SELECT ?match ?url nfo:fileName(?match) nie:mimeType(?match) WHERE "
  "{ %s "
  "  ?match a nmm:MusicPiece;  "
  "       nie:isStoredAs ?as;"
  "       nmm:musicAlbum [ nmm:albumTitle \"%s\" ]."
  "  ?as nie:url ?url . "
  "} LIMIT 50";

static const char query_tracks_for_artist[] =
  "SELECT ?match ?url nfo:fileName(?match) nie:mimeType(?match) WHERE "
  "{ %s "
  " ?match a nmm:MusicPiece; "
  "          nmm:performer [ nmm:artistName \"%s\" ]; "
  "          nmm:musicAlbum ?album;"
  "          nie:isStoredAs ?as . "
  " ?as nie:url ?url . "
  "} GROUP BY (?album) LIMIT 50";

  
static const char fts_match[] = "?match fts:match \"%s*\".";

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
_item_free(Evry_Item *it)
{
  ITEM_FILE(file, it);
  if (file->path) eina_stringshare_del(file->path);
  if (file->url) eina_stringshare_del(file->url);
  if (file->mime) eina_stringshare_del(file->mime);

  E_FREE(file);
}

static void
_query_item_free(Evry_Item *item)
{
  QUERY_ITEM(it, item);

  if (it->match) eina_stringshare_del(it->match);
  E_FREE(it);
}

static Query_Item *
_query_item_new(Plugin *p, const char *label, const char *detail, const char *query, const char *match, Eina_Bool category)
{
  Query_Item *it = E_NEW(Query_Item, 1);
  evry_item_new(EVRY_ITEM(it), EVRY_PLUGIN(p), label, _query_item_free);
  it->query = query;
  if (match)
    it->match = eina_stringshare_add(match);
  it->category = category;
  EVRY_ITEM(it)->browseable = EINA_TRUE;
  if (detail)
    EVRY_ITEM(it)->detail = eina_stringshare_add(detail);;

  return it;
}

static Evry_Item_File *
_item_add(Plugin *p, char *id, char *url, char *label, char *mime, int prio)
{
  Evry_Item_File *file;

  char *path;
  const char *tmp;

  /* XXX use evry_file_url/path_get to do the conversion only when needed */
  if (!strncmp(url, "file://", 7))
    tmp = url + 7;
  else return NULL;
   
  if (!(path = evry_util_unescape(tmp, 0)))
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
  EVRY_ITEM(file)->context = eina_stringshare_ref(file->mime);
   
  if (file->mime == mime_dir)
    {
      EVRY_ITEM(file)->browseable = EINA_TRUE;
    }
  else
    EVRY_ITEM(file)->priority = 1;
  
  free(path);
   
  return file;
}



static void
_dbus_cb_reply(void *data, DBusMessage *msg, DBusError *error)
{
  DBusMessageIter array, iter, item;
  char *urn, *path, *name, *mime, *label, *detail;
  Eina_List *items = NULL, *l;
  Plugin *p = data;
  Query_Item *it;
  Evry_Item_File *file;
  
  p->pnd = NULL;

  if (!p->fetching)
    {
      ERR("cb after cleanup!\n");
      return;
    }

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
	      dbus_message_iter_get_basic(&iter, &urn);
	      
	      if (!urn)	goto next;

	      if (!strncmp(urn, "urn:uuid:", 9))
		{
		  dbus_message_iter_next(&iter);
		  if (dbus_message_iter_get_arg_type(&iter) != DBUS_TYPE_STRING) goto next;
		  dbus_message_iter_get_basic(&iter, &path);

		  dbus_message_iter_next(&iter);
		  if (dbus_message_iter_get_arg_type(&iter) != DBUS_TYPE_STRING) goto next;
		  dbus_message_iter_get_basic(&iter, &name);

		  dbus_message_iter_next(&iter);
		  if (dbus_message_iter_get_arg_type(&iter) != DBUS_TYPE_STRING) goto next;
		  dbus_message_iter_get_basic(&iter, &mime);
		  
		  if (path && name && mime)
		    file = _item_add(p, urn, path, name, mime, 0);

		  if (file)
		    {
		      evry_util_file_detail_set(file); 
		      items = eina_list_append(items, file);
		    }
		}
	      else if (!strncmp(urn, "urn:artist:", 11))
		{
		  label = evry_util_unescape(urn + 11, 0);
		  it = _query_item_new(p, label, NULL, query_albums_for_artist, label, EINA_TRUE); 
		  if (it) items = eina_list_append(items, it);
		}
	      else if (!strncmp(urn, "urn:album:", 10))
		{
		  label = evry_util_unescape(urn + 10, 0);

		  dbus_message_iter_next(&iter);
		  if (dbus_message_iter_get_arg_type(&iter) == DBUS_TYPE_STRING)
		    dbus_message_iter_get_basic(&iter, &detail);
		  else
		    detail = NULL;
		  
		  it = _query_item_new(p, label, detail, query_tracks_for_album, label, EINA_FALSE); 

		  if (it) items = eina_list_append(items, it);
		}
	    }
	next:
	  dbus_message_iter_next(&item);
	}
    }

  EVRY_PLUGIN_ITEMS_CLEAR(p);
  
  if (items)
    {
      EINA_LIST_FREE(p->files, it)
  	evry_item_free(EVRY_ITEM(it));

      if (p->query == query_albums_for_artist)
	{
	  it = _query_item_new(p, "All Tracks", NULL, query_tracks_for_artist, p->match, EINA_FALSE);
	  it->base.priority = 1;
	  p->files = eina_list_append(p->files, it); 
	}

      items = eina_list_sort(items, eina_list_count(items), _cb_sort);
      p->files = items;
  
      EINA_LIST_FOREACH(p->files, l, it)
  	EVRY_PLUGIN_ITEM_APPEND(p, it);
    }
  else if (p->files)
    {
      EINA_LIST_REVERSE_FOREACH(p->files, l, it)
  	if (evry_fuzzy_match(it->base.label, p->input))
  	  EVRY_PLUGIN_ITEM_APPEND(p, it);
    }

  evry_plugin_async_update(EVRY_PLUGIN(p), EVRY_ASYNC_UPDATE_ADD);
}

static DBusPendingCall *
_send_query(const char *query, const char *match, const char *match2, void *cb_data)
{
  char *_query = NULL;
  DBusMessage *msg;
  DBusPendingCall *pnd;

  if (match && match2)
    {
      _query = malloc(sizeof(char) * (strlen(match) + strlen(match2) + strlen(query)));
      sprintf(_query, query, match, match2);
    }
  else if (match)
    {
      _query = malloc(sizeof(char) * (strlen(match) + strlen(query)));
      sprintf(_query, query, match);
    }
  else
    {
      _query = strdup(query);
    }
  
  printf("send: %s\n", _query);

  msg = dbus_message_new_method_call(bus_name,
				     "/org/freedesktop/Tracker1/Resources",
				     "org.freedesktop.Tracker1.Resources",
				     "SparqlQuery");
  dbus_message_append_args(msg,
			   DBUS_TYPE_STRING,  &_query,
			   DBUS_TYPE_INVALID);

  pnd = e_dbus_message_send(conn, msg, _dbus_cb_reply, -1, cb_data);
  dbus_message_unref(msg);

  E_FREE(_query);

  return pnd;
}

/* item provides a query to find files */
static Evry_Plugin *
_begin(Evry_Plugin *plugin, const Evry_Item *item)
{
  Plugin *p;
  
  if (!item) return plugin;

  QUERY_ITEM(it, item);
  
  if (it->category) return NULL;

  p = E_NEW(Plugin, 1);
  p->base = *plugin;
  p->base.items = NULL;

  p->query = it->query;
  p->match = it->match;
  
  return EVRY_PLUGIN(p);
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

  p->fetching = EINA_FALSE;

  if (p->match)
    E_FREE(p);
}


static int
_fetch(Evry_Plugin *plugin, const char *input)
{
  if (active)
    {
      PLUGIN(p, plugin);

      if (p->pnd)
	dbus_pending_call_cancel(p->pnd);
      p->pnd = NULL;

      if (!p->match)
	{
	  if (input && (strlen(input) > 2))
	    {
	      if (p->input) eina_stringshare_del(p->input);
	      p->input = eina_stringshare_add(input); 
	    }
	  else
	    {
	      _cleanup(plugin);
	      return 0;
	    }
	}
      
      /* XXX just use p->pnd*/
      p->fetching = EINA_TRUE;

      if (p->match)
	{
	  char buf[128];

	  if (input)
	    snprintf(buf, sizeof(buf), fts_match, input);
	  else
	    buf[0] = '\0';
	  
	  p->pnd = _send_query(p->query, buf, p->match, p);

	}
      else
	{
	  p->pnd = _send_query(p->query, input, NULL, p);
      	}
      
      if (p->files) return 1;
    }

  return 0;
}

static char thumb_buf[4096];
static const char hex[] = "0123456789abcdef";

static char *
_md5_sum(const char *str)
{
   MD5_CTX ctx;
  unsigned char hash[MD5_HASHBYTES];
  int n;
  char md5out[(2 * MD5_HASHBYTES) + 1];
  MD5Init (&ctx);      
  MD5Update (&ctx, (unsigned char const*)str,
	       (unsigned)strlen (str));
  MD5Final (hash, &ctx);

  for (n = 0; n < MD5_HASHBYTES; n++)
    {
      md5out[2 * n] = hex[hash[n] >> 4];
      md5out[2 * n + 1] = hex[hash[n] & 0x0f];
    }
  md5out[2 * n] = '\0';

  return strdup(md5out);
}

static Evas_Object *

_icon_get(Evry_Plugin *p __UNUSED__, const Evry_Item *item, Evas *e)
{
  ITEM_FILE(it, item);

  char *sum = _md5_sum(it->url);
  
  snprintf(thumb_buf, sizeof(thumb_buf), "%s/.thumbnails/normal/%s.png",
	   e_user_homedir_get(), sum);
  free(sum);
  
  if (ecore_file_exists(thumb_buf))
    {
      Evas_Object *o = e_icon_add(e);
      e_icon_file_set(o, thumb_buf);
      if (o) return o;
    }

  if (item->browseable)
    return evry_icon_theme_get("folder", e);
  else
    return evry_icon_mime_get(it->mime, e);

  DBG("load thumb: %s - %s\n", thumb_buf, it->path);
  return NULL;
}

static Evas_Object *

_icon_get_cat(Evry_Plugin *p __UNUSED__, const Evry_Item *item, Evas *e)
{
  QUERY_ITEM(it, item);
      
  if ((((Plugin *)item->plugin)->query == query_albums) ||
      (((Plugin *)item->plugin)->query == query_albums_for_artist))
    {
      if (!it->match || !item->detail) return NULL;

      char *suma, *sumb, *a;
      int i;

      a = strdup(item->detail);

      for(i = 0; a[i] != '\0'; i++)
      	a[i] = tolower(a[i]);

      suma = _md5_sum(a); 
      free(a);

      a = strdup(it->match);

      for(i = 0; a[i] != '\0'; i++)
      	a[i] = tolower(a[i]);

      sumb = _md5_sum(a); 
      free(a);
      
      snprintf(thumb_buf, sizeof(thumb_buf), "%s/.cache/media-art/album-%s-%s.jpeg",
      	       e_user_homedir_get(), suma, sumb );
      
      free(suma);
      free(sumb);
      
      if (ecore_file_exists(thumb_buf))
	{
	  Evas_Object *o = e_icon_add(e);
	  e_icon_file_set(o, thumb_buf);
	  if (o) return o;
	}

    }
  
  return NULL;
}

/*** Base Query List Plugin ***/
static int
_fetch_query(Evry_Plugin *plugin, const char *input)
{
  PLUGIN(p, plugin);

  DBusMessage *msg;
  char *query;
  Query_Item *it;

  if (!active) return 0;

  if (!plugin->items)
    {
      it = _query_item_new(p, "Artists", NULL, query_artists, NULL, EINA_TRUE);
      EVRY_PLUGIN_ITEM_APPEND(p, it);

      it = _query_item_new(p, "Albums", NULL, query_albums, NULL, EINA_TRUE);
      EVRY_PLUGIN_ITEM_APPEND(p, it);

      it = _query_item_new(p, "Tracks", NULL, query_tracks, NULL, EINA_FALSE);
      EVRY_PLUGIN_ITEM_APPEND(p, it);
    }  
  return 1;
}

static void
_cleanup_query(Evry_Plugin *plugin)
{
  PLUGIN(p, plugin);
  EVRY_PLUGIN_ITEMS_FREE(p);
}

/*** Category Query Plugin (Browser) ***/
static Evry_Plugin *
_begin_cat(Evry_Plugin *plugin, const Evry_Item *item)
{
  Plugin *p;
  
  if (!item) return NULL;

  QUERY_ITEM(it, item);
  
  if (!it->category) return NULL;
  
  p = E_NEW(Plugin, 1);
  p->base = *plugin;
  p->base.items = NULL;

  p->query = it->query;
  if (it->match)
    p->match = eina_stringshare_add(it->match);
  p->category = it->category;
  
  return EVRY_PLUGIN(p);
}

static int
_fetch_cat(Evry_Plugin *plugin, const char *input)
{
  PLUGIN(p, plugin);

  DBusMessage *msg;
  char *query;
  Query_Item *it;
  char match[128];
  
  if (!active) return 0;

  if (p->pnd)
    dbus_pending_call_cancel(p->pnd);
  p->pnd = NULL;

  if (input)
    {
      if (p->input) eina_stringshare_del(p->input);
      p->input = eina_stringshare_add(input); 
    }

  if (p->match)
    {
      char buf[128];

      if (input)
	snprintf(buf, sizeof(buf), fts_match, input);
      else
	buf[0] = '\0';

      p->fetching = EINA_TRUE;
      p->pnd = _send_query(p->query, buf, p->match, p);

      if (p->query == query_albums_for_artist)
      	{
      	  it = _query_item_new(p, "All Tracks", NULL,
			       query_tracks_for_artist,
			       p->match, EINA_FALSE);
	  
      	  p->files = eina_list_append(p->files, it); 
      	  EVRY_PLUGIN_ITEM_APPEND(p, it);
      	}
    }
  else
    {
      if (!input) return 0;

      snprintf(match, sizeof(match), "%s*", input);
      p->fetching = EINA_TRUE;
      p->pnd = _send_query(p->query, match, NULL, p);
    }
  
  if (!p->files)
    return 0;
    
  return 1;
}

static void
_cleanup_cat(Evry_Plugin *plugin)
{
  PLUGIN(p, plugin);
  if (p->match) eina_stringshare_del(p->match);
  if (p->input) eina_stringshare_del(p->input); 

  if (p->pnd)
    dbus_pending_call_cancel(p->pnd);

  p->fetching = EINA_FALSE;
  EVRY_PLUGIN_ITEMS_FREE(p);

  E_FREE(p);
}

static void
_plugin_free(Evry_Plugin *plugin)
{
  PLUGIN(p, plugin);

  if (p->pnd)
    dbus_pending_call_cancel(p->pnd);

  E_FREE(p);
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
module_init(void)
{
  Plugin *p;
  
  if (!evry_api_version_check(EVRY_API_VERSION))
    return EINA_FALSE;

  conn = e_dbus_bus_get(DBUS_BUS_SESSION);

  if (!conn) return EINA_FALSE;

  cb_name_owner_changed = e_dbus_signal_handler_add
    (conn, fdo_bus_name, fdo_path, fdo_interface, "NameOwnerChanged",
     _name_owner_changed, NULL);

  pending_get_name_owner = e_dbus_get_name_owner
    (conn, bus_name, _get_name_owner, NULL);

  p = E_NEW(Plugin, 1);
  EVRY_PLUGIN_NEW(p, "Tracker", type_subject, "TRACKER_QUERY", "FILE",
		  _begin, _cleanup, _fetch, _icon_get, _plugin_free);
  p->query = query_files;
  
  plugins = eina_list_append(plugins, p);
  evry_plugin_register(EVRY_PLUGIN(p), _prio++);

  p = E_NEW(Plugin, 1);
  EVRY_PLUGIN_NEW(p, "Tracker Queries", type_subject, NULL, "TRACKER_QUERY",
		  NULL, _cleanup_query, _fetch_query, NULL, NULL);
  EVRY_PLUGIN(p)->trigger = "#";
  
  plugins = eina_list_append(plugins, p);
  evry_plugin_register(EVRY_PLUGIN(p), _prio++);

  p = E_NEW(Plugin, 1);
  EVRY_PLUGIN_NEW(p, "Tracker Categories", type_subject,
		  "TRACKER_QUERY", "TRACKER_QUERY",
		  _begin_cat, _cleanup_cat, _fetch_cat, _icon_get_cat, NULL);

  plugins = eina_list_append(plugins, p);
  evry_plugin_register(EVRY_PLUGIN(p), _prio++);

  
  mime_dir = eina_stringshare_add("inode/directory");
   
  return EINA_TRUE;
}

static void
module_shutdown(void)
{
  Plugin *p;

  EINA_LIST_FREE(plugins, p)
    EVRY_PLUGIN_FREE(p);
}

/***************************************************************************/
/**/
/* actual module specifics */

static E_Module *module = NULL;
static Eina_Bool _active = EINA_FALSE;

/***************************************************************************/
/**/
/* module setup */
EAPI E_Module_Api e_modapi = 
  {
    E_MODULE_API_VERSION,
    PACKAGE
  };

EAPI void *
e_modapi_init(E_Module *m)
{
  char buf[4096];

  /* Location of message catalogs for localization */
  snprintf(buf, sizeof(buf), "%s/locale", e_module_dir_get(m));
  bindtextdomain(PACKAGE, buf);
  bind_textdomain_codeset(PACKAGE, "UTF-8");

  module = m;

  if (e_datastore_get("everything_loaded"))
    _active = module_init();
   
  e_module_delayed_set(m, 1); 

  return m;
}

EAPI int
e_modapi_shutdown(E_Module *m)
{
  Plugin *p;

  if (conn)
    {
      e_dbus_signal_handler_del(conn, cb_name_owner_changed);
      e_dbus_connection_close(conn);
    }

  eina_stringshare_del(mime_dir); 

  if (_active && e_datastore_get("everything_loaded"))
    module_shutdown();

  module = NULL;
   
  return 1;
}

EAPI int
e_modapi_save(E_Module *m)
{
  return 1;
}

/**/
/***************************************************************************/

