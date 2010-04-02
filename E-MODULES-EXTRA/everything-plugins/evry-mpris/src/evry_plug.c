#include "Evry.h"

typedef struct _Plugin Plugin;
typedef struct _Track Track;

/* #undef DBG
 * #define DBG(...) ERR(__VA_ARGS__) */

struct _Plugin
{
  Evry_Plugin base;
  int active;
  int current_track;
  int tracklist_cnt;
  int fetch_tracks;
  Eina_Bool clear_tracks;

  struct
  {
    int playing;
    int random;
    int loop;
    int repeat;
  } status;
};


struct _Track
{
  Evry_Item base;
  int id;
  char *title;
  char *artist;
  char *location;
};

static Plugin *plug;
static Eina_List *actions = NULL;
static Eina_List *players = NULL;

static E_DBus_Connection *conn = NULL;
static DBusPendingCall *pending_get_name_owner = NULL;
static E_DBus_Signal_Handler *cb_name_owner_changed = NULL;
static E_DBus_Signal_Handler *cb_tracklist_change = NULL;
static E_DBus_Signal_Handler *cb_player_track_change = NULL;
static E_DBus_Signal_Handler *cb_player_status_change = NULL;
static const char *bus_name = NULL;
static const char mpris_interface[] = "org.freedesktop.MediaPlayer";
static const char fdo_bus_name[] = "org.freedesktop.DBus";
static const char fdo_interface[] = "org.freedesktop.DBus";
static const char fdo_path[] = "/org/freedesktop/DBus";
static Eina_Bool active = EINA_FALSE;

#define ITEM_TRACK(_t, _it) Track *_t = (Track*) (_it);

static void _mpris_get_metadata(int tracks);
static char *unescape(const char *string, int length, int *olen);

static int
_dbus_check_msg(DBusMessage *reply, DBusError *error)
{
   if (error && dbus_error_is_set(error))
     {
	DBG("Error: %s - %s\n", error->name, error->message);
	return 0;
     }
   return (dbus_message_get_type(reply) == DBUS_MESSAGE_TYPE_METHOD_RETURN);
}

static void
_item_free(Evry_Item *it)
{
   Track *t = (Track *)it;

   if (t->location) eina_stringshare_del(t->location);
   if (t->artist) eina_stringshare_del(t->artist);
   if (t->title) eina_stringshare_del(t->title);

   E_FREE(t);
}

static void
_dbus_cb_tracklist_metadata(void *data, DBusMessage *reply, DBusError *error)
{
   DBusMessageIter array, iter, item, iter_val;
   Track *t = data;

   int type;
   char *key = NULL, *artist = NULL, *title = NULL, *location = NULL;

   plug->fetch_tracks--;

   if (plug->clear_tracks)
     {
	E_FREE(t);

	if (!plug->fetch_tracks)
	  _mpris_get_metadata(plug->tracklist_cnt);

	return;
     }

   if (!_dbus_check_msg(reply, error)) goto error;

   dbus_message_iter_init(reply, &array);
   if(dbus_message_iter_get_arg_type(&array) == DBUS_TYPE_ARRAY)
     {
	dbus_message_iter_recurse(&array, &item);

	while(dbus_message_iter_get_arg_type(&item) == DBUS_TYPE_DICT_ENTRY)
	  {
	     dbus_message_iter_recurse(&item, &iter);
	     if (dbus_message_iter_get_arg_type(&iter) == DBUS_TYPE_STRING)
	       {
		  dbus_message_iter_get_basic(&iter, &key);
	       }
	     else goto error;

	     dbus_message_iter_next(&iter);
	     type = dbus_message_iter_get_arg_type (&iter);
	     if (type != DBUS_TYPE_VARIANT)
	       goto error;

	     if (!strcmp(key, "artist"))
	       {
		  dbus_message_iter_recurse (&iter, &iter_val);
		  dbus_message_iter_get_basic (&iter_val, &artist);
	       }
	     else if (!strcmp(key, "title"))
	       {
		  dbus_message_iter_recurse (&iter, &iter_val);
		  dbus_message_iter_get_basic (&iter_val, &title);
	       }
	     else if (!strcmp(key, "location"))
	       {
		  dbus_message_iter_recurse (&iter, &iter_val);
		  dbus_message_iter_get_basic (&iter_val, &location);
	       }
	     dbus_message_iter_next(&item);
	  }

	if (artist && title && artist[0] && title[0])
	  {
	     char buf[128];
	     snprintf(buf, sizeof(buf), "%s - %s", artist, title);
	     t->base.label = eina_stringshare_add(buf);
	  }
	else if (title && title[0])
	  {
	     t->base.label = eina_stringshare_add(title);
	  }
	else if (location && location[0])
	  {
	     const char *file = ecore_file_file_get(location);
	     int blub;
	     char *tmp = unescape(file, 0, &blub);
	     if (tmp)
	       {
		  t->base.label = eina_stringshare_add(tmp);
		  free(tmp);
	       }
	     else goto error;
	  }
	else goto error;
     }

   if (t->base.label)
     {
	DBG("add %s, %d - %d\n", t->base.label, t->id, plug->current_track);

	EVRY_PLUGIN_ITEM_APPEND(plug, EVRY_ITEM(t));
	evry_plugin_async_update(EVRY_PLUGIN(plug), EVRY_ASYNC_UPDATE_ADD);

	if (plug->current_track == t->id)
	  {
	     evry_item_select(NULL, EVRY_ITEM(t));
	  }

	return;
     }

 error:
   E_FREE(t);
   return;
}

static void
_mpris_get_metadata(int tracks)
{
   DBusMessage *msg;
   int cnt;
   Track *it;

   plug->fetch_tracks = tracks;

   for (cnt = 0; cnt < plug->fetch_tracks; cnt++)
     {
	it = E_NEW(Track, 1);
	it->id = cnt;

	evry_item_new(EVRY_ITEM(it), EVRY_PLUGIN(plug), NULL, _item_free);

	msg = dbus_message_new_method_call(bus_name, "/TrackList",
					   mpris_interface,
					   "GetMetadata");

	dbus_message_append_args(msg,
				 DBUS_TYPE_INT32, &cnt,
				 DBUS_TYPE_INVALID);

	e_dbus_message_send(conn, msg, _dbus_cb_tracklist_metadata, -1, it);
	dbus_message_unref(msg);
     }
}


static void
_dbus_cb_tracklist_length(void *data, DBusMessage *reply, DBusError *error)
{
   DBusMessage *msg;
   Evry_Plugin *p = data;

   if (!_dbus_check_msg(reply, error)) return;

   dbus_message_get_args(reply, error,
			 DBUS_TYPE_INT32, (dbus_int32_t*) &(plug->tracklist_cnt),
			 DBUS_TYPE_INVALID);

   _mpris_get_metadata(plug->tracklist_cnt);
}

static void
_dbus_cb_current_track(void *data, DBusMessage *reply, DBusError *error)
{
   DBusMessage *msg;

   if (!_dbus_check_msg(reply, error)) return;

   dbus_message_get_args(reply, error,
			 DBUS_TYPE_INT32, (dbus_int32_t*) &(plug->current_track),
			 DBUS_TYPE_INVALID);

   if (data)
     evry_plugin_async_update(EVRY_PLUGIN(plug), EVRY_ASYNC_UPDATE_REFRESH);
}

static void
_dbus_cb_tracklist_change(void *data, DBusMessage *msg)
{
   DBG("tracklist change");
   EVRY_PLUGIN_ITEMS_FREE(plug);

   dbus_message_get_args(msg, NULL,
			 DBUS_TYPE_INT32, (dbus_int32_t*) &(plug->tracklist_cnt),
			 DBUS_TYPE_INVALID);

   /* XXX race - there could still come metadata from previous
      tracklist */
   if (!plug->fetch_tracks)
     _mpris_get_metadata(plug->tracklist_cnt);
}

static void
_dbus_cb_track_change(void *data, DBusMessage *msg)
{
   /* XXX just fsckin give the track nr. if I want metadata I would ask for it!*/

   /* dbus_message_get_args(msg, NULL,
    * 			 DBUS_TYPE_INT32, (dbus_int32_t*) &current_track,
    * 			 DBUS_TYPE_INVALID); */

   msg = dbus_message_new_method_call(bus_name, "/TrackList",
				      mpris_interface,
				      "GetCurrentTrack");

   e_dbus_message_send(conn, msg, _dbus_cb_current_track, -1, plug);
   dbus_message_unref(msg);
}

static void
_dbus_cb_status_change(void *data, DBusMessage *msg)
{
   DBusMessageIter iter, array;
   int value;
   int cnt;

   dbus_message_iter_init(msg, &array);
   dbus_message_iter_recurse(&array, &iter);
   dbus_message_iter_get_basic(&iter, &(plug->status.playing));
   dbus_message_iter_next(&iter);
   dbus_message_iter_get_basic(&iter, &(plug->status.random));
   dbus_message_iter_next(&iter);
   dbus_message_iter_get_basic(&iter, &(plug->status.repeat));
   dbus_message_iter_next(&iter);
   dbus_message_iter_get_basic(&iter, &(plug->status.loop));

   DBG("status %d", plug->status.playing);
}

static Evry_Plugin *
_begin(Evry_Plugin *p, const Evry_Item *item __UNUSED__)
{
   DBusMessage *msg;

   if (!conn || !active) return 0;

   msg = dbus_message_new_method_call(bus_name, "/TrackList",
				      mpris_interface,
				      "GetLength");

   e_dbus_message_send(conn, msg, _dbus_cb_tracklist_length, -1, p);
   dbus_message_unref(msg);

   msg = dbus_message_new_method_call(bus_name, "/TrackList",
				      mpris_interface,
				      "GetCurrentTrack");

   e_dbus_message_send(conn, msg, _dbus_cb_current_track, -1, NULL);
   dbus_message_unref(msg);


   cb_tracklist_change = e_dbus_signal_handler_add
     (conn, bus_name, "/TrackList", mpris_interface, "TrackListChange",
      _dbus_cb_tracklist_change, NULL);

   cb_player_track_change = e_dbus_signal_handler_add
     (conn, bus_name, "/Player", mpris_interface, "TrackChange",
      _dbus_cb_track_change, NULL);

   cb_player_status_change = e_dbus_signal_handler_add
     (conn, bus_name, "/Player", mpris_interface, "StatusChange",
      _dbus_cb_status_change, NULL);

   return p;
}

static void
_cleanup(Evry_Plugin *p)
{
   /* free instances */
   if (active)
     {
	e_dbus_signal_handler_del(conn, cb_tracklist_change);
	e_dbus_signal_handler_del(conn, cb_player_track_change);
	e_dbus_signal_handler_del(conn, cb_player_status_change);
     }

   EVRY_PLUGIN_ITEMS_FREE(p);
}


static int
_action(Evry_Plugin *p, const Evry_Item *it)
{
   return EVRY_ACTION_FINISHED;
}

static int
_fetch(Evry_Plugin *p, const char *input)
{

   return 1;
}

static Evas_Object *
_icon_get(Evry_Plugin *p __UNUSED__, const Evry_Item *it, Evas *e)
{
   ITEM_TRACK(t, it);

   if (t->id == plug->current_track)
     {
	return evry_icon_theme_get("media-playback-start", e);
     }

   return NULL;
}

/** ACTIONS **/
static int
_mpris_play_track(Evry_Action *act)
{
   DBusMessage *msg;
   ITEM_TRACK(t, act->item1);

   /* XXX FIX the spec. or call next/prev to skip to the track...*/
   if (!strcmp(bus_name, "org.mpris.amarok"))
     {
	msg = dbus_message_new_method_call(bus_name, "/TrackList",
					   mpris_interface,
					   "PlayTrack");
     }
   else if (!strcmp(bus_name, "org.mpris.corn"))
     {
	msg = dbus_message_new_method_call(bus_name, "/Corn",
					   "org.corn.CornPlayer",
					   "PlayTrack");
     }
   else if (!strcmp(bus_name, "org.mpris.audacious"))
     {
	msg = dbus_message_new_method_call(bus_name, "/org/atheme/audacious",
					   "org.atheme.audacious",
					   "Jump");
     }
   else return 1;

   dbus_message_append_args(msg,
			    DBUS_TYPE_INT32, &(t->id),
			    DBUS_TYPE_INVALID);

   e_dbus_message_send(conn, msg, NULL, -1, NULL);
   dbus_message_unref(msg);

   return 1;
}

static int
_mpris_tracklist_remove_track(Evry_Action *act)
{
   DBusMessage *msg;
   ITEM_TRACK(t, act->item1);

   msg = dbus_message_new_method_call(bus_name, "/TrackList",
				      mpris_interface,
				      "DelTrack");

   dbus_message_append_args(msg,
			    DBUS_TYPE_INT32, &(t->id),
			    DBUS_TYPE_INVALID);

   e_dbus_message_send(conn, msg, NULL, -1, NULL);
   dbus_message_unref(msg);

   return 1;
}

static int
_mpris_player_action(Evry_Action *act)
{
   DBusMessage *msg;

   msg = dbus_message_new_method_call(bus_name, "/Player",
				      mpris_interface,
				      (char *)act->data);

   e_dbus_message_send(conn, msg, NULL, -1, NULL);
   dbus_message_unref(msg);

   return 1;
}

static int
_mpris_play_file(Evry_Action *act)
{
   ITEM_FILE(file, act->item1);

   DBusMessage *msg;
   int play = (strcmp((char *) act->data, "e"));
   char *buf;

   if (strncmp(file->uri, "file://", 7))
     {
	buf = malloc(sizeof(char) * (strlen(file->uri) + 8));
	sprintf(buf, "file://%s", file->uri);
     }
   else
     {
	buf = malloc(sizeof(char) * (strlen(file->uri) + 1));
	sprintf(buf, "%s", file->uri);
     }

   DBG("play %s", buf);

   msg = dbus_message_new_method_call(bus_name, "/TrackList",
				      mpris_interface,
				      "AddTrack");
   dbus_message_append_args(msg,
			    DBUS_TYPE_STRING, &(buf),
			    DBUS_TYPE_BOOLEAN,  &(play),
			    DBUS_TYPE_INVALID);

   e_dbus_message_send(conn, msg, NULL, -1, NULL);
   dbus_message_unref(msg);

   free(buf);

   return 1;
}

static int
_mpris_check_file(Evry_Action *act __UNUSED__, const Evry_Item *it)
{
   ITEM_FILE(file, it);

   return (!strncmp(file->mime, "audio/", 6));
}

static void
_dbus_cb_name_owner_changed(void *data __UNUSED__, DBusMessage *msg)
{
   DBusError err;
   Eina_List *l;
   const char *tmp;
   const char *name, *from, *to;

   if (!conn) return;

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

   if (strncmp(name, "org.mpris.", 10) != 0)
     return;

   DBG("NameOwnerChanged from=[%s] to=[%s]\n", from, to);

   tmp = eina_stringshare_add(name);

   if (to[0] == '\0')
     {
	players = eina_list_remove(players, tmp);

	/* vanished player was current? */
	if (tmp == bus_name)
	  {
	     /* make another player current */
	     if (eina_list_count(players) > 0)
	       {
		  bus_name = players->data;
		  DBG("use::%s", bus_name);
		  active = EINA_TRUE;
	       }
	     else
	       {
		  active = EINA_FALSE;
	       }
	  }

	eina_stringshare_del(tmp);
     }
   else
     {
	/* new player appeared? */
	if (!eina_list_data_find(players, tmp))
	  {
	     eina_stringshare_ref(tmp);
	     players = eina_list_append(players, tmp);
	  }

	/* no active player - make player current */
	if (!active)
	  {
	     bus_name = tmp;
	     active = EINA_TRUE;
	  }
     }

   eina_stringshare_del(tmp);
}

static void
_dbus_cb_list_names(void *data __UNUSED__, DBusMessage *msg, DBusError *err)
{
   DBusMessageIter array, iter, item, iter_val;
   char *name;

   if (!_dbus_check_msg(msg, err)) return;

   dbus_message_iter_init(msg, &array);
   if(dbus_message_iter_get_arg_type(&array) == DBUS_TYPE_ARRAY)
     {
	dbus_message_iter_recurse(&array, &item);

	while(dbus_message_iter_get_arg_type(&item) == DBUS_TYPE_STRING)
	  {
	     dbus_message_iter_get_basic(&item, &name);
	     if (strncmp(name, "org.mpris.", 10) == 0)
	       {
		  DBG("found %s", name);

		  players = eina_list_append(players, eina_stringshare_add(name));
	       }

	     dbus_message_iter_next(&item);
	  }
     }

   if (eina_list_count(players) > 0)
     {
	bus_name = players->data;
	DBG("use::%s", bus_name);
	active = EINA_TRUE;
     }
}


static Eina_Bool
_init(void)
{
   Evry_Action *act;

   conn = e_dbus_bus_get(DBUS_BUS_SESSION);

   if (!conn) return EINA_FALSE;

   cb_name_owner_changed = e_dbus_signal_handler_add
     (conn, fdo_bus_name, fdo_path, fdo_interface, "NameOwnerChanged",
      _dbus_cb_name_owner_changed, NULL);

   e_dbus_list_names(conn, _dbus_cb_list_names, NULL);

   plug = E_NEW(Plugin, 1);
   evry_plugin_new(EVRY_PLUGIN(plug), "Playlist", type_subject, NULL, "MPRIS_TRACK",
		   0, "emblem-sound", NULL,
		   _begin, _cleanup, _fetch, _action, _icon_get, NULL, NULL);

   EVRY_PLUGIN(plug)->aggregate = EINA_FALSE;
   evry_plugin_register(EVRY_PLUGIN(plug), 0);

   act = evry_action_new("Play Track", "MPRIS_TRACK", NULL, NULL, "media-playback-start",
			 _mpris_play_track, NULL, NULL, NULL,NULL);
   evry_action_register(act,  0);
   actions = eina_list_append(actions, act);

   act = evry_action_new("Remove Track", "MPRIS_TRACK", NULL, NULL, "list-remove",
			 _mpris_tracklist_remove_track, NULL, NULL, NULL,NULL);
   evry_action_register(act,  0);
   actions = eina_list_append(actions, act);

   act = evry_action_new("Play", "MPRIS_TRACK", NULL, NULL, "media-playback-start",
			 _mpris_player_action, NULL, NULL, NULL,NULL);
   act->data = "Play";
   evry_action_register(act,  0);
   actions = eina_list_append(actions, act);

   act = evry_action_new("Pause", "MPRIS_TRACK", NULL, NULL, "media-playback-pause",
			 _mpris_player_action, NULL, NULL, NULL,NULL);
   act->data = "Pause";
   evry_action_register(act,  0);
   actions = eina_list_append(actions, act);

   act = evry_action_new("Stop", "MPRIS_TRACK", NULL, NULL, "media-playback-stop",
			 _mpris_player_action, NULL, NULL, NULL,NULL);
   act->data = "Stop";
   evry_action_register(act,  0);
   actions = eina_list_append(actions, act);

   act = evry_action_new("Enqueue File", "FILE", NULL, NULL, "list-add",
			 _mpris_play_file, _mpris_check_file, NULL, NULL,NULL);
   act->data = "e";
   evry_action_register(act,  0);
   actions = eina_list_append(actions, act);

   act = evry_action_new("Play File", "FILE", NULL, NULL, "media-playback-start",
			 _mpris_play_file, _mpris_check_file, NULL, NULL,NULL);
   act->data = "p";
   evry_action_register(act,  1);
   actions = eina_list_append(actions, act);

   /* act = evry_action_new("Enqueue Files", "MPRIS_TRACK", "FILE", NULL, "list-add",
    * 			 _mpris_enqueue_files, _mpris_check_files, NULL, NULL,NULL);
    * act->data = "e";
    * evry_action_register(act,  0);
    * actions = eina_list_append(actions, act); */

   return EINA_TRUE;
}

static void
_shutdown(void)
{
   Evry_Action *act;
   char *player;

   EVRY_PLUGIN_FREE(plug);

   EINA_LIST_FREE(actions, act)
     evry_action_free(act);

   if (conn)
     {
	e_dbus_signal_handler_del(conn, cb_name_owner_changed);
	e_dbus_connection_close(conn);
     }

   EINA_LIST_FREE(players, player)
     eina_stringshare_del(player);
}


EINA_MODULE_INIT(_init);
EINA_MODULE_SHUTDOWN(_shutdown);

/* taken from curl:
 *
 * Copyright (C) 1998 - 2010, Daniel Stenberg, <daniel@haxx.se>, et
 * al.
 *
 * Unescapes the given URL escaped string of given length. Returns a
 * pointer to a malloced string with length given in *olen.
 * If length == 0, the length is assumed to be strlen(string).
 * If olen == NULL, no output length is stored.
 */
#define ISXDIGIT(x) (isxdigit((int) ((unsigned char)x)))

static char *unescape(const char *string, int length, int *olen)
{
   int alloc = (length?length:(int)strlen(string))+1;
   char *ns = malloc(alloc);
   unsigned char in;
   int strindex=0;
   unsigned long hex;

   if( !ns )
     return NULL;

   while(--alloc > 0) {
      in = *string;
      if(('%' == in) && ISXDIGIT(string[1]) && ISXDIGIT(string[2])) {
	 /* this is two hexadecimal digits following a '%' */
	 char hexstr[3];
	 char *ptr;
	 hexstr[0] = string[1];
	 hexstr[1] = string[2];
	 hexstr[2] = 0;

	 hex = strtoul(hexstr, &ptr, 16);
	 in = (unsigned char)(hex & (unsigned long) 0xFF);
	 // in = ultouc(hex); /* this long is never bigger than 255 anyway */

	 string+=2;
	 alloc-=2;
      }

      ns[strindex++] = in;
      string++;
   }
   ns[strindex]=0; /* terminate it */

   if(olen)
     /* store output size */
     *olen = strindex;
   return ns;
}

#undef ISXDIGIT
