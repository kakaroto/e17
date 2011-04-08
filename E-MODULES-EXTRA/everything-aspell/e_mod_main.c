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
  Eina_Bool filter_result;  
  Eina_Bool parent;
};

struct _Query_Item
{
  Evry_Item base;

  const char *query;
  const char *match;
};

static E_DBus_Connection *conn = NULL;
static Eina_List *plugins = NULL;
static int _prio = 5;
static int active = 0;
static const char *mime_dir = NULL;
static const char *_file_list = NULL;
static DBusPendingCall *pending_get_name_owner = NULL;
static E_DBus_Signal_Handler *cb_name_owner_changed = NULL;
static const char bus_name[] = "org.freedesktop.Tracker1";
static const char fdo_bus_name[] = "org.freedesktop.DBus";
static const char fdo_interface[] = "org.freedesktop.DBus";
static const char fdo_path[] = "/org/freedesktop/DBus";

static Evas_Object *_icon_get(Evry_Item *item, Evas *e);


static const char query_files[] =
  "SELECT ?match  nie:url(?match) nfo:fileName(?match) ?m WHERE {"
  " ?match a nfo:FileDataObject;"
  "      tracker:available true;"
  "      nie:mimeType ?m"
  " %s"
  " %s"
  "} ORDER BY DESC (fn:starts-with(?m, 'inode/directory')) LIMIT 50";

static const char query_directory[] =
  "SELECT ?match  nie:url(?match) nfo:fileName(?match) ?m WHERE {"
  " ?match a nfo:FileDataObject;"
  "      nie:isPartOf [nie:url '%s'];"// propery
  "      tracker:available true;"
  "      nie:mimeType ?m"
  " %s"  // match
  "} ORDER BY DESC (fn:starts-with(?m, 'inode/directory')) LIMIT 50";

static const char query_albums[] =
  "SELECT ?match ?artist nie:url(?s) WHERE {"
  " { SELECT ?match WHERE {"
  "          ?match a nmm:MusicAlbum"
  "          %s" // property
  "          %s" // match
  " } LIMIT 20} . "
  " ?s a nmm:MusicPiece;"
  "      tracker:available true;"
  "      nmm:musicAlbum ?match;"
  "      nmm:performer[nmm:artistName ?artist]"
  "} ORDER BY (?match)";

static const char query_artists[] =
  "SELECT ?match nie:url(?s) WHERE {"
  " { SELECT ?match WHERE {"
  "          ?match a nmm:Artist"
  "          %s" // property
  "          %s" // match
  " } LIMIT 20} . "
  " ?s a nmm:MusicPiece;"
  "      nmm:performer ?match;"
  "      tracker:available true"
  "} ORDER BY (?match) LIMIT 10";

static const char query_tracks[] =
  "SELECT ?match nie:url(?match) nfo:fileName(?match) nie:mimeType(?match) WHERE {"
  "  ?match a nmm:MusicPiece;"
  "   tracker:available true"
  "   %s" // property
  "   %s" // match
  "} ORDER BY DESC nfo:fileLastModified(?match) LIMIT 30";

static const char query_tracks_for_album[] =
  "SELECT ?match nie:url(?match) nfo:fileName(?match) nie:mimeType(?match) WHERE{"
  "  ?match a nmm:MusicPiece;  "
  "   tracker:available true;"
  "   nmm:musicAlbum '%s'" // property
  "    %s"                 // match
  " OPTIONAL { ?match nmm:trackNumber ?nr}"
  "} ORDER BY ASC(?nr) LIMIT 100";

static const char query_tracks_no_album[] =
  "SELECT ?match nie:url(?match) nfo:fileName(?match) nie:mimeType(?match) WHERE{"
  "  ?match a nmm:MusicPiece;  "
  "   tracker:available true;"
  "   nmm:performer[nmm:artistName '%s']" // property
  "    %s "              // match
  "  . OPTIONAL { ?match nmm:musicAlbum ?b}"
  "  . FILTER(bound(?b) = false) "
  "} LIMIT 100";

static const char query_albums_for_artist[] =
  "SELECT tracker:coalesce(?match, 'urn:album:UnknownAlbum')"
  "       nmm:artistName(nmm:performer(?s)) nie:url(?s) WHERE {"
  " ?s a nmm:MusicPiece;"
  "      tracker:available true;"
  "      nmm:performer '%s'."// property
  "  OPTIONAL { ?s nmm:musicAlbum ?match}"
  "  %s"
  "} ORDER BY (?match) LIMIT 500";
  
static const char fts_match[] = ". ?match fts:match \"%s*\"";

static int
_cb_sort(const void *data1, const void *data2)
{
   const Evry_Item *it1, *it2;
  
   it1 = data1;
   it2 = data2;

   if (it1->priority - it2->priority)
     return (it1->priority - it2->priority);

   return !strcasecmp(it1->label, it2->label);
}


static void
_query_item_free(Evry_Item *item)
{
   QUERY_ITEM(it, item);
   const char *data;
  
   if (it->match) eina_stringshare_del(it->match);

   if (EVRY_ITEM(it)->data)
     {
	EINA_LIST_FREE(EVRY_ITEM(it)->data, data)
	  eina_stringshare_del(data); 
     }
  
   E_FREE(it);
}

static Query_Item *
_query_item_get(Plugin *p, const char *id,
		const char *label, const char *detail,
		const char *query, const char *match)
{
   Query_Item *it;
   Eina_List *l;
   const char *tmp;
  
   if (id)
     {
	tmp = eina_stringshare_add(id);

	EINA_LIST_FOREACH(p->files, l, it)
	  {
	     if (tmp == EVRY_ITEM(it)->id)
	       {
		  eina_stringshare_del(tmp);
		  evry_item_ref(EVRY_ITEM(it)); 

		  return it;
	       }
	  }
     }


   it = EVRY_ITEM_NEW(Query_Item, p, label, _icon_get, _query_item_free);
   EVRY_ITEM(it)->browseable = EINA_TRUE;
   EVRY_ITEM(it)->subtype = _file_list;
  
   it->query = query;

   if (match)
     it->match = eina_stringshare_add(match);
  
   if (detail)
     EVRY_ITEM(it)->detail = eina_stringshare_add(detail);;

   if (id)
     EVRY_ITEM(it)->id = eina_stringshare_ref(tmp);

   return it;
}

static void
_file_item_free(Evry_Item *it)
{
   GET_FILE(file, it);
   if (file->path) eina_stringshare_del(file->path);
   if (file->url) eina_stringshare_del(file->url);
   if (file->mime) eina_stringshare_del(file->mime);
   if (EVRY_ITEM(file)->data) eina_stringshare_del((const char *)EVRY_ITEM(file)->data);

   E_FREE(file);
}

static Evry_Item_File *
_file_item_get(Plugin *p, const char *urn, char *url, char *label, char *mime, int prio)
{
   Evry_Item_File *file;
   Evry_Item *it;
   Eina_List *l;
   char *path;
   const char *tmp;

   /* one could jus check the ref counts (= */
   const char *id = eina_stringshare_add(urn);

   EINA_LIST_FOREACH(p->files, l, file)
     {
	if (id == EVRY_ITEM(file)->data)
	  {
	     eina_stringshare_del(id);
	     evry_item_ref(EVRY_ITEM(file)); 
	     return file;
	  }
     }
  
   /* XXX use evry_file_url/path_get to do the conversion only when needed */
   if (!strncmp(url, "file://", 7))
     tmp = url + 7;
   else return NULL;
   
   if (!(path = evry_util_unescape(tmp, 0)))
     return NULL;

   file = EVRY_ITEM_NEW(Evry_Item_File, p, label, _icon_get, _file_item_free);
   EVRY_ITEM(file)->data = (void *)id;
   int match = evry_fuzzy_match(label, p->input);
   if (match)
     EVRY_ITEM(file)->fuzzy_match = match;
   else
     EVRY_ITEM(file)->fuzzy_match = 100;
   
   file->path = eina_stringshare_add(path);
   file->mime = eina_stringshare_add(mime);
   file->url = eina_stringshare_add(url);
   EVRY_ITEM(file)->context = eina_stringshare_ref(file->mime);
   EVRY_ITEM(file)->id = eina_stringshare_ref(file->path);
    
   if (file->mime == mime_dir)
     EVRY_ITEM(file)->browseable = EINA_TRUE;
   else
     EVRY_ITEM(file)->priority = 1;
  
   free(path);
   
   return file;
}

static void
_add_file(Plugin *p, const char *urn, DBusMessageIter *iter, Eina_List **items)
{
   Evry_Item_File *file;
   char *path, *name, *mime;

   dbus_message_iter_next(iter);
   if (dbus_message_iter_get_arg_type(iter) != DBUS_TYPE_STRING)
     return;
   dbus_message_iter_get_basic(iter, &path);

   dbus_message_iter_next(iter);
   if (dbus_message_iter_get_arg_type(iter) != DBUS_TYPE_STRING)
     return;
   dbus_message_iter_get_basic(iter, &name);

   dbus_message_iter_next(iter);
   if (dbus_message_iter_get_arg_type(iter) != DBUS_TYPE_STRING)
     return;
   dbus_message_iter_get_basic(iter, &mime);

   if (path && name && mime)
     file = _file_item_get(p, urn, path, name, mime, 0);

   if (file)
     {
	evry_util_file_detail_set(file); 
	*items = eina_list_append(*items, file);
     }
}

static Query_Item *
_add_album(Plugin *p, const char *urn, DBusMessageIter *iter, Query_Item *it, Eina_List **items)
{
   char *label, *path, *detail;
   Eina_List *l;
  
   if (!it || strcmp(urn, EVRY_ITEM(it)->id))
     {
	label = evry_util_unescape(urn + 10, 0);

	dbus_message_iter_next(iter);
	if (dbus_message_iter_get_arg_type(iter) == DBUS_TYPE_STRING)
	  dbus_message_iter_get_basic(iter, &detail);

	if (!strcmp(urn, "urn:album:UnknownAlbum"))
	  it = _query_item_get(p, urn, label, detail,
			       query_tracks_no_album, detail);
	else
	  it = _query_item_get(p, urn, label, detail,
			       query_tracks_for_album, urn);

	if (it && (!EVRY_ITEM(it)->data))
	  *items = eina_list_append(*items, it);
     }
   else
     {
	/* skip artist... */
	dbus_message_iter_next(iter);
     }
		  
   if (it)
     {
	dbus_message_iter_next(iter);
	if (dbus_message_iter_get_arg_type(iter) == DBUS_TYPE_STRING)
	  dbus_message_iter_get_basic(iter, &path);
		      
	if (path)
	  {
	     l = EVRY_ITEM(it)->data;
	     l = eina_list_append(l, eina_stringshare_add(path));
	     EVRY_ITEM(it)->data = l;
	  }
     }

   return it;
}

static Query_Item *
_add_artist(Plugin *p, const char *urn, DBusMessageIter *iter, Query_Item *it, Eina_List **items)
{
   char *label, *path;
   Eina_List *l;
  
   if (!it || strcmp(urn, EVRY_ITEM(it)->id))
     {
	label = evry_util_unescape(urn + 11, 0);

	it = _query_item_get(p, urn, label, NULL, query_albums_for_artist, urn);
		      
	if (it) *items = eina_list_append(*items, it);
     }
   if (it)
     {
	dbus_message_iter_next(iter);
	if (dbus_message_iter_get_arg_type(iter) == DBUS_TYPE_STRING)
	  dbus_message_iter_get_basic(iter, &path);
		      
	if (path)
	  {
	     l = EVRY_ITEM(it)->data;
	     l = eina_list_append(l, eina_stringshare_add(path));
	     EVRY_ITEM(it)->data = l;
	  }
     }
   return it;
}

static void
_dbus_cb_reply(void *data, DBusMessage *msg, DBusError *error)
{
   DBusMessageIter array, iter, item;
   char *urn, *path, *name, *mime, *label, *detail;
   Eina_List *items = NULL, *l;
   Plugin *p = data;
   Query_Item *it = NULL;
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
   if (dbus_message_iter_get_arg_type(&array) != DBUS_TYPE_ARRAY)
     {
	ERR("got trash!\n");
	goto end;
     }
  
   dbus_message_iter_recurse(&array, &item);
   while (dbus_message_iter_get_arg_type(&item) == DBUS_TYPE_ARRAY)
     {
	dbus_message_iter_recurse(&item, &iter);
	  
	if (dbus_message_iter_get_arg_type(&iter) != DBUS_TYPE_STRING)
	  goto next;
	  
	dbus_message_iter_get_basic(&iter, &urn);
	      
	if (!urn)	goto next;

	if (!strncmp(urn, "urn:uuid:", 9))
	  {		    
	     _add_file(p, urn, &iter, &items); 
	  }
	else if (!strncmp(urn, "urn:album:", 10))
	  {
	     it = _add_album(p, urn, &iter, it, &items); 
	  }
	else if (!strncmp(urn, "urn:artist:", 11))
	  {
	     it = _add_artist(p, urn, &iter, it, &items); 
	  }
     next:
	dbus_message_iter_next(&item);
     }

 end:     
   EVRY_PLUGIN_ITEMS_CLEAR(p);
  
   if (items)
     {
	int prio = 0;
	Evry_Item *it;
	EINA_LIST_FREE(p->files, it)
	  evry_item_free(it);

	p->files = items;
  
	EINA_LIST_FOREACH(p->files, l, it)
	  {
	     EVRY_PLUGIN_ITEM_APPEND(p, it);
	     it->priority  = prio++;
	  
	  }      
     }
   else if (p->files)
     {
	EINA_LIST_FOREACH(p->files, l, it)
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

   if (!query)
     {
	ERR("no query");
	return NULL;
     }
  
   if (match && match2)
     {
	_query = malloc(sizeof(char) * (strlen(match) + strlen(match2) + strlen(query)));
	sprintf(_query, query, match, match2);
     }
   else if (match)
     {
	_query = malloc(sizeof(char) * (strlen(match) + strlen(query)));
	sprintf(_query, query, match, "");
     }
   else
     {
	_query = strdup(query);
     }
  
   /* printf("send: %s\n", _query); */

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
   GET_PLUGIN(parent, plugin);
   Plugin *p;

   p = E_NEW(Plugin, 1);
   p->base = *plugin;
   p->base.items = NULL;

   if (item && evry_item_type_check(item, "TRACKER_QUERY", NULL) ||
       item && evry_item_type_check(item, "TRACKER_MUSIC", NULL))
     {
	QUERY_ITEM(it, item);
	p->parent = EINA_TRUE;      
	p->query = it->query;
	if (it->match)
	  p->match = eina_stringshare_add(it->match);
     }
   else if (item && evry_item_type_check(item, "FILE", NULL))
     {
	GET_FILE(it, item);
	p->parent = EINA_TRUE;
	p->query = query_directory;
	p->match = eina_stringshare_ref(it->url);
     }
   else
     {
	p->parent = EINA_TRUE;
	p->query = parent->query;
	if (parent->match)
	  p->match = eina_stringshare_ref(parent->match);
     }
  
   return EVRY_PLUGIN(p);
}

static void
_cleanup(Evry_Plugin *plugin)
{
   GET_PLUGIN(p, plugin);
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
     eina_stringshare_del(p->match);
  
   E_FREE(p);
}


static int
_fetch(Evry_Plugin *plugin, const char *input)
{
   if (active)
     {
	char buf[128];

	GET_PLUGIN(p, plugin);
      
	if (p->filter_result && p->files)
	  {
	     Evry_Item *it;
	     Eina_List *l;
	  
	     EVRY_PLUGIN_ITEMS_CLEAR(p);

	     EINA_LIST_FOREACH(p->files, l, it)
	       if (!input || evry_fuzzy_match(it->label, input))
		 EVRY_PLUGIN_ITEM_APPEND(p, it);

	     if (p->parent && p->base.items) return 1;
	  }

	if (p->pnd)
	  dbus_pending_call_cancel(p->pnd);
	p->pnd = NULL;

	if (input && (strlen(input) > 2) && isalnum(input[0]))
	  {
	     if (p->input) eina_stringshare_del(p->input);
	     p->input = eina_stringshare_add(input); 

	     snprintf(buf, sizeof(buf), fts_match, input);
	  
	     p->fetching = EINA_TRUE;

	     p->pnd = _send_query(p->query, (p->match ? p->match : ""), buf, p);
	  }
	else
	  {
	     if (/*evry_item_type_check(p, ) ||*/ p->match)
	       {
		  p->fetching = EINA_TRUE;
		  p->pnd = _send_query(p->query, (p->match ? p->match : ""), "", p);
	       }
	  }
      
	if (p->files) return 1;
     }

   return 0;
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
_icon_get(Evry_Item *item, Evas *e)
{
   if (evry_item_type_check(item, "TRACKER_MUSIC", NULL))
     {
	if (item->subtype && item->subtype == _file_list)
	  {
	     QUERY_ITEM(it, item);

	     if (!item->label || !item->detail)
	       return evry_icon_theme_get("folder", e);

	     char *suma, *sumb, *a;
	     int i;

	     /* Artist */
	     a = strdup(item->detail);

	     for(i = 0; a[i] != '\0'; i++)
	       a[i] = tolower(a[i]);

	     suma = _md5_sum(a); 
	     free(a);

	     /* Album */
	     a = strdup(item->label);

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
	     else
	       return evry_icon_theme_get("folder", e);
	  }
	else
	  {
	     GET_FILE(it, item);
	  
	     if (item->browseable)
	       return evry_icon_theme_get("folder", e);
	     else
	       return evry_icon_mime_get(it->mime, e);
	  }
     }
   else if (evry_item_type_check(item, "FILE", NULL))
     {
	GET_FILE(it, item);

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
     }
  
   /* DBG("load thumb: %s - %s\n", thumb_buf, it->path); */
   return NULL;
}

static Eina_Bool
_plugins_init(void)
{
   Evry_Plugin *p; 
  
   if (!evry_api_version_check(EVRY_API_VERSION))
     return EINA_FALSE;
  
   p = EVRY_PLUGIN_NEW(Plugin, N_("Tracker"), NULL, "FILE",
		       _begin, _cleanup, _fetch, NULL);
   GET_PLUGIN(p1, p);
   p1->query = query_files;
   plugins = eina_list_append(plugins, p);
   evry_plugin_register(p, EVRY_PLUGIN_SUBJECT,_prio++);
  
   p = EVRY_PLUGIN_NEW(Plugin, N_("Tracker"), NULL, "FILE",
		       _begin, _cleanup, _fetch, NULL);
   GET_PLUGIN(p2, p);
   p2->query = query_files;
   plugins = eina_list_append(plugins, p);
   evry_plugin_register(p, EVRY_PLUGIN_OBJECT,_prio++);
  
   p = EVRY_PLUGIN_NEW(Plugin, N_("Albums"), NULL, "TRACKER_MUSIC",
		       _begin, _cleanup, _fetch, NULL);
   p->history = EINA_FALSE;
   GET_PLUGIN(p3, p);
   p3->query = query_albums;
   p3->filter_result = EINA_TRUE;
   plugins = eina_list_append(plugins, p);
   evry_plugin_register(p, EVRY_PLUGIN_OBJECT,_prio++);

   p = EVRY_PLUGIN_NEW(Plugin, N_("Artists"), NULL, "TRACKER_MUSIC",
		       _begin, _cleanup, _fetch, NULL);
   p->history = EINA_FALSE;
   GET_PLUGIN(p4, p);
   p4->query = query_artists;
   p4->filter_result = EINA_TRUE;
   plugins = eina_list_append(plugins, p);
   evry_plugin_register(p, EVRY_PLUGIN_OBJECT, _prio++);
   
   return EINA_TRUE;
}

static void
_plugins_shutdown(void)
{
   Plugin *p;

   EINA_LIST_FREE(plugins, p)
     EVRY_PLUGIN_FREE(p);
}

/***************************************************************************/

static E_Module *module = NULL;
static Eina_Bool _active = EINA_FALSE;

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

   conn = e_dbus_bus_get(DBUS_BUS_SESSION);

   if (!conn)
     return NULL;

   cb_name_owner_changed = e_dbus_signal_handler_add
     (conn, fdo_bus_name, fdo_path, fdo_interface, "NameOwnerChanged",
      _name_owner_changed, NULL);

   pending_get_name_owner = e_dbus_get_name_owner
     (conn, bus_name, _get_name_owner, NULL);

   mime_dir = eina_stringshare_add("inode/directory");
   _file_list = eina_stringshare_add("FILE_LIST");

   if (e_datastore_get("everything_loaded"))
     _active = _plugins_init();
   
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

   if (mime_dir)
     eina_stringshare_del(mime_dir); 
   if (_file_list)
     eina_stringshare_del(_file_list); 

   if (_active && e_datastore_get("everything_loaded"))
     _plugins_shutdown();

   module = NULL;
   
   return 1;
}

EAPI int
e_modapi_save(E_Module *m)
{
   return 1;
}

/***************************************************************************/



