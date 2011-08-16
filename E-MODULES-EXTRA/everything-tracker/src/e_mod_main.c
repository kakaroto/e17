#include "e.h"
#include "e_mod_main.h"
#include "evry_api.h"

/* #undef DBG
 * #define DBG(...) ERR(__VA_ARGS__) */

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

static const Evry_API *evry = NULL;
static Evry_Module *evry_module = NULL;

static E_DBus_Connection *conn = NULL;
static Eina_List *plugins = NULL;
static int _prio = 5;
static int dbus_active = 0;
static Ecore_Event_Handler *action_handler = NULL;
static const char *mime_dir = NULL;
static DBusPendingCall *pending_get_name_owner = NULL;
static E_DBus_Signal_Handler *cb_name_owner_changed = NULL;
static const char bus_name[] = "org.freedesktop.Tracker1";
static const char tracker_path[] = "/org/freedesktop/Tracker1/Resources";
static const char tracker_interface[] = "org.freedesktop.Tracker1.Resources";
static const char fdo_bus_name[] = "org.freedesktop.DBus";
static const char fdo_interface[] = "org.freedesktop.DBus";
static const char fdo_path[] = "/org/freedesktop/DBus";
static const char _module_icon[] = "find";

static Evas_Object *_icon_get(Evry_Item *item, Evas *e);

static Evry_Type TRACKER_MUSIC;
static Evry_Type TRACKER_QUERY;
static Evry_Type FILE_LIST;
static Evry_Type MPRIS_TRACK;

static const char query_files[] =
  "SELECT ?match  nie:url(?match) nfo:fileName(?match) ?m WHERE {"
  " ?match a nfo:FileDataObject;"
  "      tracker:available true;"
  "      nie:mimeType ?m"
  " %s"
  " %s"
  "} ORDER BY DESC nfo:fileLastModified(?match) LIMIT 50";

static const char query_remove_file[] =
  "DELETE { ?urn a rdfs:Resource } WHERE {?urn a nfo:FileDataObject;  nie:url \"%s\"} %s";

static const char query_directory[] =
  "SELECT ?match  nie:url(?match) nfo:fileName(?match) ?m WHERE {"
  " ?match a nfo:FileDataObject;"
  "      nie:isPartOf [nie:url \"%s\"];"// propery
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
  // "      tracker:available true;"
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
  "      nmm:performer ?match"
  // "      tracker:available true"
  "} ORDER BY (?match)";

static const char query_tracks[] =
  "SELECT ?match nie:url(?match) nfo:fileName(?match) nie:mimeType(?match) WHERE {"
  "  ?match a nmm:MusicPiece;"
  // "   tracker:available true"
  "   %s" // property
  "   %s" // match
  "} ORDER BY DESC nfo:fileLastModified(?match) LIMIT 30";

static const char query_tracks_for_album[] =
  "SELECT ?match nie:url(?match) nfo:fileName(?match) nie:mimeType(?match) WHERE{"
  "  ?match a nmm:MusicPiece;  "
  // "   tracker:available true;"
  "   nmm:musicAlbum \"%s\"" // property
  "    %s"                 // match
  " OPTIONAL { ?match nmm:trackNumber ?nr}"
  "} ORDER BY ASC(?nr) LIMIT 100";

static const char query_tracks_no_album[] =
  "SELECT ?match nie:url(?match) nfo:fileName(?match) nie:mimeType(?match) WHERE{"
  "  ?match a nmm:MusicPiece;  "
  // "   tracker:available true;"
  "   nmm:performer[nmm:artistName \"%s\"]" // property
  "    %s "              // match
  "  . OPTIONAL { ?match nmm:musicAlbum ?b}"
  "  . FILTER(bound(?b) = false) "
  "} LIMIT 100";

static const char query_albums_for_artist[] =
  "SELECT tracker:coalesce(?match, 'urn:album:UnknownAlbum')"
  "       nmm:artistName(nmm:performer(?s)) nie:url(?s) WHERE {"
  " ?s a nmm:MusicPiece;"
  // "      tracker:available true;"
  "      nmm:performer \"%s\"."// property
  "  OPTIONAL { ?s nmm:musicAlbum ?match}"
  "  %s"
  "} ORDER BY (?match) LIMIT 500";

static const char query_usage_count[] =
  "SELECT nie:url(?urn) nie:usageCounter(?urn) WHERE {?urn a nmm:MusicPiece; nie:url \"%s\" }";

static const char update_usage_count[] =
  "DELETE {"
  "  ?unknown nie:usageCounter ?count"
  "} WHERE {"
  "  ?unknown nie:usageCounter ?count ;"
  "  nie:url \"%s\" . "
  "}"
  "INSERT {"
  "  ?unknown nie:usageCounter %d"
  "} WHERE {"
  "  ?unknown nie:url \"%s\" ."
  "}";

static const char query_most_played[] =
  "SELECT  ?match nie:url(?match) nfo:fileName(?match) nie:mimeType(?match)"
  "WHERE {"
  "  ?match a nmm:MusicPiece;"
  "          nie:usageCounter ?count"
  "   %s" // property
  "   %s" // match
  "} ORDER BY DESC(?count) LIMIT 50";

static const char fts_match[] = ". ?match fts:match \"%s*\"";


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
	     if (tmp != EVRY_ITEM(it)->id)
	       continue;

	     eina_stringshare_del(tmp);
	     evry->item_ref(EVRY_ITEM(it));

	     return it;
	  }
     }

   it = EVRY_ITEM_NEW(Query_Item, p, label, _icon_get, _query_item_free);
   EVRY_ITEM(it)->browseable = EINA_TRUE;
   EVRY_ITEM(it)->subtype = FILE_LIST;

   it->query = query;

   if (match)
     it->match = eina_stringshare_add(match);

   if (detail)
     EVRY_ITEM(it)->detail = eina_stringshare_add(detail);;

   if (id)
     EVRY_ITEM(it)->id = tmp;

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
   Eina_List *l;
   const char *id;

   id = eina_stringshare_add(urn);
   EINA_LIST_FOREACH(p->files, l, file)
     {
	if (id == EVRY_ITEM(file)->data)
	  {
	     eina_stringshare_del(id);
	     evry->item_ref(EVRY_ITEM(file));
	     return file;
	  }
     }

   file = EVRY_ITEM_NEW(Evry_Item_File, p, label, _icon_get, _file_item_free);
   EVRY_ITEM(file)->type = EVRY_TYPE_FILE;
   EVRY_ITEM(file)->data = (void *)id;
   int match = evry->fuzzy_match(label, p->input);
   if (match)
     EVRY_ITEM(file)->fuzzy_match = match;
   else
     EVRY_ITEM(file)->fuzzy_match = 100;

   file->mime = eina_stringshare_add(mime);
   file->url = eina_stringshare_add(url);

   EVRY_ITEM(file)->context = eina_stringshare_ref(file->mime);

   if (evry->file_path_get(file))
     EVRY_ITEM(file)->id = eina_stringshare_ref(file->path);

   if (file->mime == mime_dir)
     EVRY_ITEM(file)->browseable = EINA_TRUE;
   else
     EVRY_ITEM(file)->priority = 1;

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
	evry->util_file_detail_set(file);
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
	label = evry->util_url_unescape(urn + 10, 0);

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
	label = evry->util_url_unescape(urn + 11, 0);

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

static int
_dbus_message_open(DBusMessage *msg, DBusError *error, DBusMessageIter *iter)
{
   DBusMessageIter array;

   if (dbus_error_is_set(error))
     {
	ERR("%s - %s\n", error->name, error->message);
	return 0;
     }

   dbus_message_iter_init(msg, &array);
   if (dbus_message_iter_get_arg_type(&array) != DBUS_TYPE_ARRAY)
     {
	ERR("got trash!\n");
	return 0;
     }

   dbus_message_iter_recurse(&array, iter);

   return 1;
}

static void
_dbus_cb_reply(void *data, DBusMessage *msg, DBusError *error)
{
   DBusMessageIter iter, item;
   char *urn;
   Eina_List *items = NULL, *l;
   Plugin *p = data;
   Query_Item *it = NULL;

   p->pnd = NULL;

   if (!p->fetching)
     {
	ERR("cb after cleanup!\n");
	return;
     }

   if (!_dbus_message_open(msg, error, &item))
     return;

   while (dbus_message_iter_get_arg_type(&item) == DBUS_TYPE_ARRAY)
     {
	dbus_message_iter_recurse(&item, &iter);

	if (dbus_message_iter_get_arg_type(&iter) != DBUS_TYPE_STRING)
	  goto next;

	dbus_message_iter_get_basic(&iter, &urn);

	if (!urn)
	  goto next;

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

   EVRY_PLUGIN_ITEMS_CLEAR(p);

   if (items)
     {
	int prio = 0;
	Evry_Item *it;
	EINA_LIST_FREE(p->files, it)
	  evry->item_free(it);

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
	  if (evry->fuzzy_match(it->base.label, p->input))
	    EVRY_PLUGIN_ITEM_APPEND(p, it);
     }

   EVRY_PLUGIN_UPDATE(p, EVRY_UPDATE_ADD);
}

static DBusPendingCall *
_send_query(const char *query, const char *match, const char *match2, int update,
	    void (*cb_reply)(void *data, DBusMessage *msg, DBusError *error),
	    void *cb_data)
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

   DBG("%s\n", _query);

   msg = dbus_message_new_method_call(bus_name, tracker_path, tracker_interface,
				      (update ? "SparqlUpdate" : "SparqlQuery"));

   dbus_message_append_args(msg, DBUS_TYPE_STRING, &_query, DBUS_TYPE_INVALID);

   pnd = e_dbus_message_send(conn, msg, cb_reply, -1, cb_data);

   dbus_message_unref(msg);

   E_FREE(_query);

   return pnd;
}

static Evry_Plugin *
_browse(Evry_Plugin *plugin, const Evry_Item *item)
{
   Plugin *p;

   if (!CHECK_TYPE(item, TRACKER_QUERY) &&
       !CHECK_TYPE(item, TRACKER_MUSIC))
     return NULL;

   GET_PLUGIN(parent, plugin);
   QUERY_ITEM(it, item);

   if (plugin != item->plugin)
     return NULL;

   EVRY_PLUGIN_INSTANCE(p, plugin);

   p->filter_result = parent->filter_result;
   p->parent = EINA_TRUE;
   p->query = it->query;

   if (it->match)
     p->match = eina_stringshare_add(it->match);

   return EVRY_PLUGIN(p);
}

static Evry_Plugin *
_begin(Evry_Plugin *plugin, const Evry_Item *item)
{
   Plugin *p;

   GET_PLUGIN(parent, plugin);

   EVRY_PLUGIN_INSTANCE(p, plugin);
   p->filter_result = parent->filter_result;
   p->parent = EINA_TRUE;
   p->query = parent->query;

   if (parent->match)
     p->match = eina_stringshare_ref(parent->match);

   return EVRY_PLUGIN(p);
}

static void
_finish(Evry_Plugin *plugin)
{
   GET_PLUGIN(p, plugin);
   Evry_Item_File *file;

   EVRY_PLUGIN_ITEMS_CLEAR(p);
   
   EINA_LIST_FREE(p->files, file)
     EVRY_ITEM_FREE(file);

   if (p->input)
     eina_stringshare_del(p->input);

   if (p->pnd)
     dbus_pending_call_cancel(p->pnd);

   p->fetching = EINA_FALSE;

   if (p->match)
     eina_stringshare_del(p->match);

   E_FREE(p);
}


static int
_fetch(Evry_Plugin *plugin, const char *input)
{
   GET_PLUGIN(p, plugin);

   DBG("%s %s", plugin->name, input);

   char buf[128];
   int len = 0;
   Evry_Item *it;

   IF_RELEASE(p->input);
   if (input)
     {
	len = strlen(input);
	p->input = eina_stringshare_add(input);
     }

   if (!dbus_active)
     {
	EVRY_PLUGIN_ITEMS_CLEAR(p);

	EINA_LIST_FREE(p->files, it)
	  evry->item_free(it);

	return 0;
     }

   if (input && p->filter_result && p->files)
     {
	Eina_List *l;
	EVRY_PLUGIN_ITEMS_CLEAR(p);

	EINA_LIST_FOREACH(p->files, l, it)
	  if (!input || evry->fuzzy_match(it->label, input))
	    EVRY_PLUGIN_ITEM_APPEND(p, it);

	if (p->parent && EVRY_PLUGIN(p)->items) return 1;
     }

   if (p->pnd)
     dbus_pending_call_cancel(p->pnd);
   p->pnd = NULL;

   if ((len > 0 && len >= plugin->config->min_query) && isalnum(input[0]))
     {
	p->fetching = EINA_TRUE;

	snprintf(buf, sizeof(buf), fts_match, input);
	p->pnd = _send_query(p->query, (p->match ? p->match : ""), buf,
				  0, _dbus_cb_reply, p);
     }
   else if (len == 0 && plugin->config->min_query == 0)
     {
	p->fetching = EINA_TRUE;

	p->pnd = _send_query(p->query, (p->match ? p->match : ""), "",
			     0, _dbus_cb_reply, p);
     }
   else if (p->match)
     {
	p->fetching = EINA_TRUE;
	p->pnd = _send_query(p->query, p->match, "",
			     0, _dbus_cb_reply, p);
	return 1;
     }
   else if (len < plugin->config->min_query)
     {
       EVRY_PLUGIN_ITEMS_CLEAR(p);

       EINA_LIST_FREE(p->files, it)
	 evry->item_free(it);
     }

   return !!(p->base.items);
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
     dbus_active = EINA_FALSE;
   else
     dbus_active = EINA_TRUE;
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
     dbus_active = EINA_TRUE;

   return;
}

static char thumb_buf[4096];

static Evas_Object *
_icon_get(Evry_Item *item, Evas *e)
{
   if (CHECK_TYPE(item, TRACKER_MUSIC))
     {
       if (CHECK_SUBTYPE(item, FILE_LIST))
	  {
	     if (!item->label || !item->detail)
	       return NULL;

	     char *suma, *sumb, *a;
	     int i;

	     /* Artist */
	     a = strdup(item->detail);

	     for(i = 0; a[i] != '\0'; i++)
	       a[i] = tolower(a[i]);

	     suma = evry->util_md5_sum(a);
	     free(a);

	     /* Album */
	     a = strdup(item->label);

	     for(i = 0; a[i] != '\0'; i++)
	       a[i] = tolower(a[i]);

	     sumb = evry->util_md5_sum(a);
	     free(a);

	     snprintf(thumb_buf, sizeof(thumb_buf),
		      "%s/.cache/media-art/album-%s-%s.jpeg",
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
     }

   return NULL;
}

static void
_dbus_cb_track_count(void *data, DBusMessage *reply, DBusError *error)
{
   DBusMessageIter item, iter;
   DBusMessage *msg;
   char *urn = NULL;
   char *count = NULL;
   char query[1024];
   char *tmp;

   if (!_dbus_message_open(reply, error, &item))
     return;

   while (dbus_message_iter_get_arg_type(&item) == DBUS_TYPE_ARRAY)
     {
	dbus_message_iter_recurse(&item, &iter);

	if (dbus_message_iter_get_arg_type(&iter) == DBUS_TYPE_STRING)
	  {
	     dbus_message_iter_get_basic(&iter, &urn);
	     if (!urn) continue;
	  }
	dbus_message_iter_next(&iter);

	if (dbus_message_iter_get_arg_type(&iter) == DBUS_TYPE_STRING)
	  {
	     dbus_message_iter_get_basic(&iter, &count);
	     if (!count) continue;
	  }
	break;
     }

   if (!urn) return;

   printf("update count %s %s\n", urn, count);

   snprintf(query, sizeof(query), update_usage_count, urn,
	    (count ? (atoi(count) + 1) : 1), urn);

   msg = dbus_message_new_method_call(bus_name, tracker_path, tracker_interface,
				      "SparqlUpdate");
   tmp = strdup(query);
   dbus_message_append_args(msg, DBUS_TYPE_STRING,  &tmp, DBUS_TYPE_INVALID);
   free(tmp);

   e_dbus_message_send(conn, msg, NULL, -1, NULL);
   dbus_message_unref(msg);
}

/* TODO remove folders recursively */
static Eina_Bool
_cb_action_performed(void *data, int type, void *event)
{
   Evry_Event_Action_Performed *ev = event;
   const char *url;

   if (!dbus_active)
     return 1;

   if (CHECK_TYPE(ev->it1, EVRY_TYPE_FILE) &&
       (ev->action && !strcmp(ev->action, "Move to Trash")) &&
       (url = evry->file_url_get(EVRY_FILE(ev->it1))))
     {
	printf("file deleted %s\n", url);
	_send_query(query_remove_file, url, NULL, 1,
		    NULL, NULL);
     }
   else if (CHECK_TYPE(ev->it1, MPRIS_TRACK) &&
       (ev->action && !strcmp(ev->action, "Play Track")) &&
       (url = evry->file_url_get(EVRY_FILE(ev->it1))))
     {
   	_send_query(query_usage_count, url, NULL, 0,
		    _dbus_cb_track_count, NULL);
     }
   else if (CHECK_TYPE(ev->it1, EVRY_TYPE_FILE) &&
	    (ev->action && !strcmp(ev->action, "Play File")) &&
	    (url = evry->file_url_get(EVRY_FILE(ev->it1))))
     {
       _send_query(query_usage_count, url, NULL, 0,
		   _dbus_cb_track_count, NULL);
     }

   return 1;
}

static int
_plugins_init(const Evry_API *_api)
{
   Evry_Plugin *p;

   evry = _api;

   if (!evry->api_version_check(EVRY_API_VERSION))
     return EINA_FALSE;

   conn = e_dbus_bus_get(DBUS_BUS_SESSION);

   if (!conn)
     return EINA_FALSE;

   cb_name_owner_changed = e_dbus_signal_handler_add
     (conn, fdo_bus_name, fdo_path, fdo_interface, "NameOwnerChanged",
      _name_owner_changed, NULL);

   pending_get_name_owner = e_dbus_get_name_owner
     (conn, bus_name, _get_name_owner, NULL);

   mime_dir = eina_stringshare_add("inode/directory");

   e_datastore_set("everything_tracker", "");

   TRACKER_QUERY = evry->type_register("TRACKER_QUERY");
   TRACKER_MUSIC = evry->type_register("TRACKER_MUSIC");
   FILE_LIST     = evry->type_register("FILE_LIST");
   MPRIS_TRACK   = evry->type_register("MPRIS_TRACK");

   action_handler = evry->event_handler_add(EVRY_EVENT_ACTION_PERFORMED,
					    _cb_action_performed, NULL);

#define FILE_PLUGIN_NEW(_name, _plug_type, _icon, _type, _begin, _finish, _fetch, _query) { \
     p = EVRY_PLUGIN_NEW(Plugin, _name, _icon, _type, _begin, _finish, _fetch, NULL); \
     GET_PLUGIN(p1, p);							\
     p1->query = _query;						\
     if (evry->plugin_register(p, _plug_type, _prio++)) {		\
	p->config->min_query = 4;					\
	p->config->top_level = 1; }					\
     plugins = eina_list_append(plugins, p); }


   FILE_PLUGIN_NEW("Tracker", EVRY_PLUGIN_SUBJECT, "find", EVRY_TYPE_FILE,
		   _begin, _finish, _fetch, query_files);

   FILE_PLUGIN_NEW("Tracker", EVRY_PLUGIN_OBJECT, "find", EVRY_TYPE_FILE,
		   _begin, _finish, _fetch, query_files);


#define QUERY_PLUGIN_NEW(_name, _plug_type, _icon, _type, _begin, _finish, _fetch, _query, _min_query) { \
      p = EVRY_PLUGIN_NEW(Plugin, _name, _icon, _type, _begin, _finish, _fetch, NULL); \
      p->browse = &_browse;						\
      p->history = EINA_FALSE;						\
      GET_PLUGIN(p1, p);						\
      p1->query = _query;						\
      p1->filter_result = EINA_TRUE;					\
      if (evry->plugin_register(p, _plug_type, _prio++)) {		\
	 p->config->top_level = 1; 					\
	 p->config->min_query = _min_query; }				\
      plugins = eina_list_append(plugins, p); }

   QUERY_PLUGIN_NEW("Albums", EVRY_PLUGIN_OBJECT, "emblem-sound", TRACKER_MUSIC,
		    _begin, _finish, _fetch, query_albums, 4);

   QUERY_PLUGIN_NEW("Artist", EVRY_PLUGIN_OBJECT, "emblem-sound", TRACKER_MUSIC,
		    _begin, _finish, _fetch, query_artists, 4);

   QUERY_PLUGIN_NEW("Most Listened", EVRY_PLUGIN_OBJECT, "emblem-sound", TRACKER_MUSIC,
		    _begin, _finish, _fetch, query_most_played, 0);

   QUERY_PLUGIN_NEW("Albums", EVRY_PLUGIN_SUBJECT, "emblem-sound", TRACKER_MUSIC,
		    _begin, _finish, _fetch, query_albums, 4);

   QUERY_PLUGIN_NEW("Artist", EVRY_PLUGIN_SUBJECT, "emblem-sound", TRACKER_MUSIC,
		    _begin, _finish, _fetch, query_artists, 4);

   /* QUERY_PLUGIN_NEW(N_("Most Listened"), EVRY_PLUGIN_SUBJECT, "emblem-sound", TRACKER_MUSIC,
    * 		    _begin, _finish, _fetch, query_most_played, 0); */

   return EINA_TRUE;
}

static void
_plugins_shutdown(void)
{
   Plugin *p;

   if (conn)
     {
       e_dbus_signal_handler_del(conn, cb_name_owner_changed);
       e_dbus_connection_close(conn);
       conn = NULL;
     }

   IF_RELEASE(mime_dir);

   ecore_event_handler_del(action_handler);
   action_handler = NULL;

   EINA_LIST_FREE(plugins, p)
     EVRY_PLUGIN_FREE(p);
}

/***************************************************************************/

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

   EVRY_MODULE_NEW(evry_module, evry, _plugins_init, _plugins_shutdown);
   
   e_module_delayed_set(m, 1);

   return m;
}

EAPI int
e_modapi_shutdown(E_Module *m)
{
   EVRY_MODULE_FREE(evry_module);
   
   return 1;
}

EAPI int
e_modapi_save(E_Module *m)
{
   return 1;
}

/***************************************************************************/
