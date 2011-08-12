#include "e.h"
#include "e_mod_main.h"
#include "evry_api.h"

#define ACT_PLAY		0
#define ACT_STOP		1
#define ACT_PAUSE		2
#define ACT_FORWARD		3
#define ACT_REWIND		4
#define ACT_CLEAR		5
#define ACT_PLAY_TRACK		6
#define ACT_REMOVE_TRACK	7
#define ACT_ADD_FILE		8
#define ACT_ENQUEUE_FILE	9
#define ACT_PLAY_FILE		10


#define MPRIS_CAN_GO_NEXT(caps)			(caps & (1 << 0))
#define MPRIS_CAN_GO_PREV(caps)			(caps & (1 << 1))
#define MPRIS_CAN_PAUSE(caps)			(caps & (1 << 2))
#define MPRIS_CAN_PLAY(caps)			(caps & (1 << 3))
#define MPRIS_CAN_SEEK(caps)			(caps & (1 << 4))
#define MPRIS_CAN_PROVIDE_METADATA(caps)	(caps & (1 << 5))
#define MPRIS_CAN_HAS_TRACKLIST(caps)		(caps & (1 << 6))

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
  Eina_List *tracks;
  Eina_List *fetch;

  int next_track;

  const char *input;

  struct
  {
    int playing;
    int random;
    int loop;
    int repeat;
  } status;

  struct
  {
    Eina_Bool play_track;
  } support;

  Track *empty;

  Ecore_Timer *update_timer;

  E_DBus_Signal_Handler *cb_tracklist_change;
  E_DBus_Signal_Handler *cb_player_track_change;
  E_DBus_Signal_Handler *cb_player_status_change;

  DBusPendingCall *pnd_tracklist;
  DBusPendingCall *pnd_status;
  DBusPendingCall *pnd_curtrack;
};


struct _Track
{
  Evry_Item_File base;
  int id;
  const char *title;
  const char *artist;
  const char *album;
  /* const char *location; */
  int length;

  DBusPendingCall *pnd;
};

static Evry_Module *evry_module = NULL;

static Plugin *_plug;
static Eina_List *actions = NULL;
static Eina_List *players = NULL;
static Ecore_Event_Handler *select_handler = NULL;
static Eina_Bool plugin_selected = EINA_FALSE;
static E_DBus_Connection *conn = NULL;
static E_DBus_Signal_Handler *cb_name_owner_changed = NULL;
static const char *bus_name = NULL;
static const char mpris_interface[] = "org.freedesktop.MediaPlayer";
static const char fdo_bus_name[] = "org.freedesktop.DBus";
static const char fdo_interface[] = "org.freedesktop.DBus";
static const char fdo_path[] = "/org/freedesktop/DBus";
static char *theme_file = NULL;
static Evry_Type MPRIS_TRACK;
static Evry_Type TRACKER_MUSIC;
static Evry_Type FILE_LIST;
static Eina_Bool dbus_active = EINA_FALSE;

#define GET_TRACK(_t, _it) Track *_t = (Track*) (_it);

static void _mpris_get_metadata(Plugin *p, Eina_Bool delay);


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

static DBusPendingCall *
_dbus_send_msg(const char *path, const char *method,
	       E_DBus_Method_Return_Cb _cb, void *data)
{
   DBusMessage *msg;
   DBusPendingCall *pnd;

   msg = dbus_message_new_method_call(bus_name, path,
				      mpris_interface,
				      method);

   pnd = e_dbus_message_send(conn, msg, _cb, -1, data);
   dbus_message_unref(msg);

   return pnd;
}

static DBusPendingCall *
_dbus_send_msg_int(const char *path, const char *method,
		   E_DBus_Method_Return_Cb _cb, void *data, int num)
{
   DBusMessage *msg;
   DBusPendingCall *pnd;

   msg = dbus_message_new_method_call(bus_name, path,
				      mpris_interface,
				      method);

   dbus_message_append_args(msg,
			    DBUS_TYPE_INT32, &num,
			    DBUS_TYPE_INVALID);

   pnd = e_dbus_message_send(conn, msg, _cb, -1, data);
   dbus_message_unref(msg);

   return pnd;
}


static void
_item_free(Evry_Item *it)
{
   GET_TRACK(t, it);
   GET_FILE(f, it);

   IF_RELEASE(f->path);
   IF_RELEASE(f->url);

   IF_RELEASE(t->artist);
   IF_RELEASE(t->album);
   IF_RELEASE(t->title);

   if (t->pnd) dbus_pending_call_cancel(t->pnd);

   E_FREE(t);
}

static void
_dbus_cb_current_track(void *data, DBusMessage *reply, DBusError *error)
{
   Plugin *p = data;
   Evry_Item *it;
   int num;

   p->pnd_curtrack = NULL;
   
   if (!_dbus_check_msg(reply, error))
     return;

   dbus_message_get_args(reply, error,
			 DBUS_TYPE_INT32, (dbus_int32_t*) &(num),
			 DBUS_TYPE_INVALID);

   if (num != p->current_track)
     {
	it = eina_list_nth(p->tracks, p->current_track);
	if (it) evry->item_changed(it, 1, 0);

	p->current_track = num;

	it = eina_list_nth(p->tracks, p->current_track);
	if (it) evry->item_changed(it, 1, plugin_selected);
     }
}

static void
_update_list(Plugin *p)
{
   Eina_List *l;
   Track *t, *t2;
   char buf[128];
   char *tmp;

   EVRY_PLUGIN_ITEMS_CLEAR(p);

   /* remove 'empty' item */
   if ((p->tracks && p->tracks->next) &&
       (p->tracks->data == p->empty))
     p->tracks = eina_list_remove(p->tracks, p->empty);

   l = p->tracks;
   p->tracks = NULL;

   /* merge old and new list, keep old when possible */
   EINA_LIST_FREE(p->fetch, t)
     {
	t2 = (l ? l->data : NULL);

	if (!EVRY_ITEM(t)->id)
	  {
	     EVRY_ITEM_FREE(t);
	     continue;
	  }

	if (t2 && (t->id == t2->id) && (EVRY_ITEM(t)->id == EVRY_ITEM(t2)->id))
	  {
	     EVRY_ITEM_FREE(t);
	     p->tracks = eina_list_append(p->tracks, t2);
	  }
	else /*** new track ***/
	  {
	     if (t2) EVRY_ITEM_FREE(t2);
	     p->tracks = eina_list_append(p->tracks, t);

	     GET_ITEM(it, t);
	     GET_FILE(file, t);

	     /* TODO fix xmms2 mpris bridge / audacious */
	     tmp = evry->util_url_unescape(it->id, 0);
	     if (!strncmp(tmp, "file://", 7))
	       file->path = eina_stringshare_add(tmp + 7);
	     else
	       file->path = eina_stringshare_add(tmp);
	     free(tmp);

	     /*** set label ***/
	     if (t->artist && t->title)
	       {
		  snprintf(buf, sizeof(buf), "%s - %s", t->artist, t->title);
		  it->label = eina_stringshare_add(buf);
	       }
	     else if (t->title)
	       {
		  it->label = eina_stringshare_add(t->title);
	       }
	     else if (file->path)
	       {
		  const char *label;
		  if ((label = ecore_file_file_get(file->path)))
		    it->label = eina_stringshare_add(label);
	       }
	     else
	       {
		  it->label = eina_stringshare_add(N_("No Title"));
	       }

	     if (t->album)
	       EVRY_ITEM(t)->detail = eina_stringshare_ref(t->album);
	  }

	if (l) l = eina_list_remove_list(l, l);
     }

   EINA_LIST_FREE(l, t)
     EVRY_ITEM_FREE(t);

   EINA_LIST_FOREACH(p->tracks, l, t)
     {
	if ((!p->input || evry->fuzzy_match(EVRY_ITEM(t)->label, p->input)))
	  EVRY_PLUGIN_ITEM_APPEND(p, t);
     }

   if (!p->pnd_curtrack)
     p->pnd_curtrack = _dbus_send_msg("/TrackList", "GetCurrentTrack",
				      _dbus_cb_current_track, p);

   EVRY_PLUGIN_UPDATE(p, EVRY_UPDATE_ADD);
}


static void
_dbus_cb_tracklist_metadata(void *data, DBusMessage *reply, DBusError *error)
{
   DBusMessageIter array, item, iter, iter_val;
   Track *t = data;
   char *key, *tmp;
   GET_PLUGIN(p, EVRY_ITEM(t)->plugin);

   t->pnd = NULL;

   p->fetch_tracks--;

   if (!_dbus_check_msg(reply, error))
     {
	ERR("dbus garbage!");
	goto error;
     }

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
	     else
	       {
		  ERR("not string");
		  goto error;
	       }

	     dbus_message_iter_next(&iter);
	     if (dbus_message_iter_get_arg_type(&iter) != DBUS_TYPE_VARIANT)
	       {
		  ERR("not variant");
		  goto error;
	       }

	     if (!strcmp(key, "artist"))
	       {
		  dbus_message_iter_recurse (&iter, &iter_val);
		  dbus_message_iter_get_basic (&iter_val, &tmp);
		  if (tmp && tmp[0])
		    t->artist = eina_stringshare_add(tmp);
	       }
	     else if (!strcmp(key, "title"))
	       {
		  dbus_message_iter_recurse (&iter, &iter_val);
		  dbus_message_iter_get_basic (&iter_val, &tmp);
		  if (tmp && tmp[0])
		    t->title = eina_stringshare_add(tmp);
	       }
	     else if (!strcmp(key, "location"))
	       {
		  dbus_message_iter_recurse (&iter, &iter_val);
		  dbus_message_iter_get_basic (&iter_val, &tmp);
		  if (tmp && tmp[0])
		    EVRY_ITEM(t)->id = eina_stringshare_add(tmp);
	       }
	     else if (!strcmp(key, "album"))
	       {
		  dbus_message_iter_recurse (&iter, &iter_val);
		  dbus_message_iter_get_basic (&iter_val, &tmp);
		  if (tmp && tmp[0])
		    t->album = eina_stringshare_add(tmp);
	       }
	     else if (!strcmp(key, "mtime"))
	       {
		  dbus_message_iter_recurse (&iter, &iter_val);
		  dbus_message_iter_get_basic (&iter_val, &(t->length));
	       }
	     dbus_message_iter_next(&item);
	  }
     }

   if (!p->fetch_tracks)
     _update_list(p);

   return;

 error:
   if (!p->fetch_tracks)
     {
	EVRY_PLUGIN_ITEMS_CLEAR(p);

	EINA_LIST_FREE(p->tracks, t)
	  EVRY_ITEM_FREE(t);

	p->tracks = p->fetch;

	EVRY_PLUGIN_UPDATE(p, EVRY_UPDATE_ADD);
     }

   p->fetch = eina_list_remove(p->fetch, t);
   EVRY_ITEM_FREE(t);

   return;
}

static Evas_Object *
_icon_get(Evry_Item *it, Evas *e)
{
   Evas_Object *o = NULL;

   if (CHECK_TYPE(it, MPRIS_TRACK))
     {
	GET_TRACK(t, it);
	GET_PLUGIN(p, it->plugin);

	if (t->id == p->current_track)
	  {
	     o = e_icon_add(e);
	     e_icon_preload_set(o, 1);
	     if (p->status.playing == 0)
	       e_icon_file_edje_set(o, theme_file, "media-playback-start");
	     else if (p->status.playing == 1)
	       e_icon_file_edje_set(o, theme_file, "media-playback-pause");
	     else if (p->status.playing == 2)
	       e_icon_file_edje_set(o, theme_file, "media-playback-stop");
	  }
     }
   else if (CHECK_TYPE(it, EVRY_TYPE_ACTION))
     {
	if (edje_file_group_exists(theme_file, it->icon))
	  {
	     o = e_icon_add(e);
	     e_icon_preload_set(o, 1);
	     if (e_icon_file_edje_set(o, theme_file, it->icon))
	       return o;

	     evas_object_del(o);
	     return NULL;
	  }
     }

   return o;
}

static Eina_Bool
_update_timer(void *data)
{
   Plugin *p = data;
   int cnt;
   Track *t;

   for (cnt = 0; cnt < p->fetch_tracks; cnt++)
     {
	t = EVRY_ITEM_NEW(Track, p, NULL, _icon_get, _item_free);
	t->id = cnt;
	EVRY_ITEM(t)->subtype = EVRY_TYPE_FILE;

	t->pnd = _dbus_send_msg_int("/TrackList", "GetMetadata",
				    _dbus_cb_tracklist_metadata, t, cnt);

	p->fetch = eina_list_append(p->fetch, t);
     }

   p->update_timer = NULL;
   return EINA_FALSE;
}

static void
_mpris_get_metadata(Plugin *p, Eina_Bool delay)
{

   Track *t;
   Eina_List *l, *ll;

   DBG("tracklist changed %d, %d", p->tracklist_cnt, p->fetch_tracks);
   p->fetch_tracks = p->tracklist_cnt;
   p->fetch = NULL;

   EINA_LIST_FOREACH_SAFE(p->tracks, l, ll, t)
     {
	if (t->pnd)
	  {
	     p->tracks = eina_list_remove_list(p->tracks, l);
	     EVRY_ITEM_FREE(t);
	  }
     }

   if (p->fetch_tracks)
     {
	EVRY_ITEM_LABEL_SET(p->empty, _("Loading Playlist"));
	evry->item_changed(EVRY_ITEM(p->empty), 0, 0);

	if (p->update_timer)
	  ecore_timer_del(p->update_timer);

	if (delay)
	  p->update_timer = ecore_timer_add(0.3, _update_timer, p);
	else
	  _update_timer(p);
     }
   else
     {
	EVRY_ITEM_LABEL_SET(p->empty, _("Empty Playlist"));
	evry->item_changed(EVRY_ITEM(p->empty), 0, 0);
     }

   if (!p->tracks)
     {
	EVRY_PLUGIN_ITEM_APPEND(p, p->empty);
	EVRY_PLUGIN_UPDATE(p, EVRY_UPDATE_ADD);
     }
}


static void
_dbus_cb_tracklist_length(void *data, DBusMessage *reply, DBusError *error)
{
   Plugin *p = data;

   p->pnd_tracklist = NULL;
   
   if (!_dbus_check_msg(reply, error)) return;

   dbus_message_get_args(reply, error,
			 DBUS_TYPE_INT32, (dbus_int32_t*) &(p->tracklist_cnt),
			 DBUS_TYPE_INVALID);

   _mpris_get_metadata(p, EINA_FALSE);
}

static void
_set_status(Plugin *p, DBusMessage *msg)
{
   DBusMessageIter iter, array;
   Evry_Item *it;

   dbus_message_iter_init(msg, &iter);

   if (dbus_message_iter_get_arg_type(&iter) != DBUS_TYPE_STRUCT)
     {
	ERR("no dbus struct");
	return;
     }

   dbus_message_iter_recurse(&iter, &array);
   dbus_message_iter_get_basic(&array, &(p->status.playing));
   dbus_message_iter_next(&array);
   dbus_message_iter_get_basic(&array, &(p->status.random));
   dbus_message_iter_next(&array);
   dbus_message_iter_get_basic(&array, &(p->status.repeat));
   dbus_message_iter_next(&array);
   dbus_message_iter_get_basic(&array, &(p->status.loop));
   DBG("status %d", p->status.playing);

   it = eina_list_nth(p->tracks, p->current_track);
   if (it) evry->item_changed(it, 1, 0);
}

static void
_dbus_cb_get_status(void *data, DBusMessage *reply, DBusError *error)
{
   GET_PLUGIN(p, data);

   p->pnd_status = NULL;
   
   if (!_dbus_check_msg(reply, error)) return;

   _set_status(data, reply);
}

static void
_dbus_cb_tracklist_change(void *data, DBusMessage *msg)
{
   GET_PLUGIN(p, data);

   p->next_track = 0;

   dbus_message_get_args(msg, NULL,
			 DBUS_TYPE_INT32, (dbus_int32_t*) &(p->tracklist_cnt),
			 DBUS_TYPE_INVALID);

   _mpris_get_metadata(p, EINA_TRUE);
}

static void
_dbus_cb_track_change(void *data, DBusMessage *msg)
{
   GET_PLUGIN(p, data);

   /* XXX just fsckin give the track nr. if I want metadata I would ask for it!*/

   if (p->pnd_curtrack)
     dbus_pending_call_cancel(p->pnd_curtrack);

   p->pnd_curtrack = _dbus_send_msg("/TrackList", "GetCurrentTrack", _dbus_cb_current_track, p);
}

static void
_dbus_cb_status_change(void *data, DBusMessage *msg)
{
   GET_PLUGIN(p, data);

   DBusMessageIter iter;

   dbus_message_iter_init(msg, &iter);

   if (dbus_message_iter_get_arg_type(&iter) == DBUS_TYPE_STRUCT)
     {
	_set_status(p, msg);
     }
   else if (dbus_message_iter_get_arg_type(&iter) == DBUS_TYPE_INT32)
     {
	/* XXX audacious.. seems to be fixed upstream, remove later */
	p->pnd_status = _dbus_send_msg("/Player", "GetStatus", _dbus_cb_get_status, p);
     }
}

static Evry_Plugin *
_begin(Evry_Plugin *plugin, const Evry_Item *item __UNUSED__)
{
   Plugin *p;

   if (!conn || !dbus_active) return NULL;

   EVRY_PLUGIN_INSTANCE(p, plugin);

   p->pnd_tracklist = _dbus_send_msg("/TrackList", "GetLength", _dbus_cb_tracklist_length, p);

   p->pnd_status = _dbus_send_msg("/Player", "GetStatus", _dbus_cb_get_status, p);

   p->cb_tracklist_change = e_dbus_signal_handler_add
     (conn, bus_name, "/TrackList", mpris_interface, "TrackListChange",
      _dbus_cb_tracklist_change, p);

   p->cb_player_track_change = e_dbus_signal_handler_add
     (conn, bus_name, "/Player", mpris_interface, "TrackChange",
      _dbus_cb_track_change, p);

   p->cb_player_status_change = e_dbus_signal_handler_add
     (conn, bus_name, "/Player", mpris_interface, "StatusChange",
      _dbus_cb_status_change, p);

   p->empty = EVRY_ITEM_NEW(Track, p, _("Loading Playlist"), NULL, _item_free);
   p->empty->id = -1;

   p->current_track = -2;

   return EVRY_PLUGIN(p);
}

static void
_finish(Evry_Plugin *plugin)
{
   Track *t;

   GET_PLUGIN(p, plugin);
   EVRY_PLUGIN_ITEMS_CLEAR(p);

   IF_RELEASE(p->input);

   if (p->cb_tracklist_change)
     e_dbus_signal_handler_del(conn, p->cb_tracklist_change);
   if (p->cb_player_track_change)
     e_dbus_signal_handler_del(conn, p->cb_player_track_change);
   if (p->cb_player_status_change)
     e_dbus_signal_handler_del(conn, p->cb_player_status_change);

   if (p->pnd_tracklist)
     dbus_pending_call_cancel(p->pnd_tracklist);
   if (p->pnd_status)
     dbus_pending_call_cancel(p->pnd_status);
   if (p->pnd_curtrack)
     dbus_pending_call_cancel(p->pnd_curtrack);

   EINA_LIST_FREE(p->fetch, t)
     EVRY_ITEM_FREE(t);

   EINA_LIST_FREE(p->tracks, t)
     EVRY_ITEM_FREE(t);

   EVRY_ITEM_FREE(p->empty);

   if (p->update_timer)
     ecore_timer_del(p->update_timer);

   E_FREE(p);
}

static int
_fetch(Evry_Plugin *plugin, const char *input)
{
   Eina_List *l;
   Track *t;

   GET_PLUGIN(p, plugin);

   IF_RELEASE(p->input);

   if (input)
     p->input = eina_stringshare_add(input);

   EVRY_PLUGIN_ITEMS_CLEAR(p);

   EINA_LIST_FOREACH(p->tracks, l, t)
     {
	if (!input || evry->fuzzy_match(EVRY_ITEM(t)->label, input))
	  EVRY_PLUGIN_ITEM_APPEND(p, t);
     }

   return 1;
}

/** ACTIONS **/
static int
_mpris_play_track(Evry_Action *act)
{
   DBusMessage *msg;

   GET_TRACK(t, act->it1.item);
   GET_PLUGIN(p, EVRY_ITEM(t)->plugin);

   if (!strcmp(bus_name, "org.mpris.amarok") ||
       !strcmp(bus_name, "org.mpris.xmms2"))
     {
	_dbus_send_msg_int("/TrackList", "PlayTrack", NULL, NULL, t->id);
	return 1;
     }
   else if (!strcmp(bus_name, "org.mpris.corn"))
     {
	msg = dbus_message_new_method_call(bus_name, "/Corn",
					   "org.corn.CornPlayer",
					   "PlayTrack");

	dbus_message_append_args(msg,
				 DBUS_TYPE_INT32, &(t->id),
				 DBUS_TYPE_INVALID);

	e_dbus_message_send(conn, msg, NULL, -1, NULL);
	dbus_message_unref(msg);

     }
   else if (!strcmp(bus_name, "org.mpris.audacious"))
     {
	msg = dbus_message_new_method_call(bus_name, "/org/atheme/audacious",
					   "org.atheme.audacious",
					   "Jump");
	dbus_message_append_args(msg,
				 DBUS_TYPE_UINT32, &(t->id),
				 DBUS_TYPE_INVALID);

	e_dbus_message_send(conn, msg, NULL, -1, NULL);
	dbus_message_unref(msg);
     }
   else if (!strcmp(bus_name, "org.mpris.vlc"))
     {
	/* the ones that want extra ugly treatment */
	/* p->next_track = t->id - p->current_track;
	 *
	 * _dbus_send_msg("/Player", "Stop", _mpris_play_track_hack, p);  */
     }
   else
     {
	_dbus_send_msg("/Player", "Stop", NULL, NULL);
	p->next_track = t->id - p->current_track;
	while (p->next_track)
	  {
	     if (p->next_track > 0)
	       {
		  msg = dbus_message_new_method_call(bus_name, "/Player",
						     mpris_interface,
						     "Next");
		  p->next_track--;
	       }
	     else
	       {
		  msg = dbus_message_new_method_call(bus_name, "/Player",
						     mpris_interface,
						     "Prev");
		  p->next_track++;
	       }

	     e_dbus_message_send(conn, msg, NULL, -1, NULL);

	     dbus_message_unref(msg);
	  }
	_dbus_send_msg("/Player", "Play", NULL, NULL);
     }

   return 1;
}

static int
_mpris_tracklist_remove_track(Evry_Action *act)
{
   if (!act->it1.items)
     {
	GET_TRACK(t, act->it1.item);
	_dbus_send_msg_int("/TrackList", "DelTrack", NULL, NULL, t->id);
     }
   else
     {
	Evry_Item *it;
	Eina_List *l;

	EINA_LIST_REVERSE_FOREACH(act->it1.items, l, it)
	  {
	     if (it->type == MPRIS_TRACK)
	       {
		  GET_TRACK(t, it);
		  _dbus_send_msg_int("/TrackList", "DelTrack", NULL, NULL, t->id);
	       }
	  }
     }

   return 1;
}

static int
_mpris_player_action(Evry_Action *act)
{
   int m = EVRY_ITEM_DATA_INT_GET(act);

   switch (m)
     {
      case ACT_PLAY:
	 _dbus_send_msg("/Player", "Play", NULL, NULL);
	 break;
      case ACT_STOP:
	 _dbus_send_msg("/Player", "Stop", NULL, NULL);
	 break;
      case ACT_PAUSE:
	 _dbus_send_msg("/Player", "Pause", NULL, NULL);
	 break;
     }
   return 1;
}

static int
_mpris_tracklist_action_clear(Evry_Action *act)
{
   DBusMessage *msg;

   if (!strcmp(bus_name, "org.mpris.xmms2"))
     {
	_dbus_send_msg("/TrackList", "Clear", NULL, NULL);
	return 1;
     }
   else if (!strcmp(bus_name, "org.mpris.corn"))
     {
	msg = dbus_message_new_method_call(bus_name, "/Corn",
					   "org.corn.CornPlayer",
					   "Clear");
     }
   else if (!strcmp(bus_name, "org.mpris.audacious"))
     {
	msg = dbus_message_new_method_call(bus_name, "/org/atheme/audacious",
					   "org.atheme.audacious",
					   "Clear");
     }
   else return 1;

   e_dbus_message_send(conn, msg, NULL, -1, NULL);
   dbus_message_unref(msg);

   return 1;
}

static void
_dbus_cb_position_get(void *data, DBusMessage *reply, DBusError *error)
{
   Evry_Action *act = data;
   int pos;

   GET_TRACK(t, act->it1.item);

   if (!_dbus_check_msg(reply, error)) return;

   dbus_message_get_args(reply, NULL,
			 DBUS_TYPE_INT32, (dbus_int32_t*) &(pos),
			 DBUS_TYPE_INVALID);

   if (EVRY_ITEM_DATA_INT_GET(act) == ACT_FORWARD)
     pos += 60000;
   else
     pos -= 60000;

   if (pos < 0)
     pos = 0;
   else if (pos > t->length)
     pos = t->length;

   _dbus_send_msg_int("/Player", "PositionSet", NULL, NULL, pos);
}

static int
_mpris_player_position(Evry_Action *act)
{
   _dbus_send_msg("/Player", "PositionGet", _dbus_cb_position_get, act);

   /* usually one wants to repeat this action more than one time */
   return 0;
}

static void
_add_file(const char *path, int play_now)
{
   DBusMessage *msg;
   char *buf;

   if (strncmp(path, "file://", 7))
     {
	buf = malloc(sizeof(char) * (strlen(path) + 8));
	sprintf(buf, "file://%s", path);
     }
   else
     {
	buf = evry->util_url_unescape(path, 0);
     }

   DBG("play %s", buf);

   msg = dbus_message_new_method_call(bus_name, "/TrackList",
				      mpris_interface,
				      "AddTrack");
   dbus_message_append_args(msg,
			    DBUS_TYPE_STRING, &(buf),
			    DBUS_TYPE_BOOLEAN,  &(play_now),
			    DBUS_TYPE_INVALID);

   e_dbus_message_send(conn, msg, NULL, -1, NULL);
   dbus_message_unref(msg);

   if (play_now && _plug->status.playing != 0)
     _dbus_send_msg("/Player", "Play", NULL, NULL);

   free(buf);
}

static void
_add_dir(const char *path)
{
   Eina_List *files = ecore_file_ls(path);
   char *f;
   char buf[PATH_MAX];
   const char *mime;

   EINA_LIST_FREE(files, f)
     {
	snprintf(buf, sizeof(buf), "%s/%s", path, f);
	mime = efreet_mime_type_get(buf);

	if (mime && strncmp(mime, "audio/", 6) == 0)
	  _add_file(buf, 0);

	free(f);
     }
}

static int
_add_files(Evry_Item_File *file)
{
   if (file->mime && strncmp(file->mime, "audio/", 6) == 0)
     {
	_add_file(file->path, 0);
	return 1;
     }
   else if (ecore_file_is_dir(file->path))
     {
	_add_dir(file->path);
	return 1;
     }

   return 0;
}

static int
_mpris_play_file(Evry_Action *act)
{
   Evry_Item_File *file;
   int play = EVRY_ITEM_DATA_INT_GET(act) == ACT_PLAY_FILE;

   if (CHECK_TYPE(act->it1.item, MPRIS_TRACK))
     {

	file = (Evry_Item_File *)act->it2.item;
	if (strncmp(file->mime, "audio/", 6) != 0)
	  return 0;
     }
   else
     {
	file = (Evry_Item_File *)act->it1.item;
     }

   _add_file(file->path, play);

   return 1;
}

static int
_mpris_add_files(Evry_Action *act)
{
   const Evry_Item *it = act->it2.item;

   /* if (!CHECK_TYPE(act->it1.item, MPRIS_TRACK))
    *   return 0; */

   if ((!CHECK_TYPE(it, EVRY_TYPE_FILE)) &&
       (!CHECK_TYPE(it, TRACKER_MUSIC)))
     return 0;

   if (CHECK_TYPE(it, TRACKER_MUSIC) &&
       CHECK_SUBTYPE(it, FILE_LIST) &&
       (act->it2.item->data))
     {
	char *file;
	Eina_List *l;

	EINA_LIST_REVERSE_FOREACH(act->it2.item->data, l, file)
	  _add_file(file, 0);
	return 1;
     }

   GET_FILE(file, act->it2.item);

   return _add_files(file);
}

static int
_mpris_enqueue_files(Evry_Action *act)
{
   GET_FILE(file, act->it1.item);

   return _add_files(file);
}

static int
_mpris_remove_dups(Evry_Action *act)
{
   Eina_List *l, *ll;
   Evry_Item_File *f, *f2;

   _mpris_tracklist_action_clear(NULL);

   EINA_LIST_FOREACH(_plug->tracks, l, f)
     {
	EINA_LIST_FOREACH(_plug->tracks, ll, f2)
	  {
	     if (f == f2)
	       {
		  _add_file(f->path, 0);
		  break;
	       }
	     else if (f->path == f2->path)
	       {
		  break;
	       }
	  }
     }
   return 1;
}

static int
_mpris_remove_missing(Evry_Action *act)
{
   Eina_List *l;
   Evry_Item_File *f;

   _mpris_tracklist_action_clear(NULL);

   EINA_LIST_FOREACH(_plug->tracks, l, f)
     {
	if (ecore_file_exists(f->path))
	  _add_file(f->path, 0);
     }

   return 1;
}

static int
_mpris_check_file(Evry_Action *act __UNUSED__, const Evry_Item *it)
{
   GET_FILE(file, it);

   return (file->mime && !strncmp(file->mime, "audio/", 6));
}

static int
_mpris_check_files(Evry_Action *act __UNUSED__, const Evry_Item *it)
{
   GET_FILE(file, it);

   return ((it->browseable) || (file->mime && !strncmp(file->mime, "audio/", 6)));
}

static int
_mpris_check_add_music(Evry_Action *act __UNUSED__, const Evry_Item *it __UNUSED__)
{
   if (e_datastore_get("everything_tracker"))
     return 1;

   return 0;
}

static int
_mpris_check_item(Evry_Action *act, const Evry_Item *it)
{
   GET_PLUGIN(p, it->plugin);

   if (it == EVRY_ITEM(p->empty))
     return 0;

   int action = EVRY_ITEM_DATA_INT_GET(act);


   if (action == ACT_PLAY_TRACK)
     {
	if (p->current_track == ((Track *)it)->id)
	  return 0;
     }
   else if (action == ACT_REMOVE_TRACK)
     {
	return 1;
     }
   else if (action == ACT_STOP)
     {
	if (p->status.playing == 2) return 0;
     }
   else if (action == ACT_PLAY)
     {
	if (p->status.playing == 0) return 0;
     }
   else if (action == ACT_PAUSE)
     {
	if (p->status.playing != 0) return 0;
     }
   else if (action == ACT_FORWARD)
     {
	if ((p->current_track != ((Track *)it)->id) ||
	    (p->status.playing != 0)) return 0;
     }
   else if (action == ACT_REWIND)
     {
	if ((p->current_track != ((Track *)it)->id) ||
	    (p->status.playing != 0)) return 0;
     }
   else if (action == ACT_CLEAR)
     {
	if (strcmp(bus_name, "org.mpris.xmms2") &&
	    strcmp(bus_name, "org.mpris.audacious") &&
	    strcmp(bus_name, "org.mpris.corn"))
	  return 0;
     }

   return 1;
}

static void
_dbus_cb_get_caps(void *data, DBusMessage *reply, DBusError *error)
{
   const char *player = data;
   int caps;

   if (!_dbus_check_msg(reply, error))
     {
	eina_stringshare_del(player);
	return;
     }

   dbus_message_get_args(reply, error,
			 DBUS_TYPE_INT32, (dbus_int32_t*) &(caps),
			 DBUS_TYPE_INVALID);

   DBG("player: %s\n go next:%d\n go prev %d\n pause %d\n play"
       "% d\n seek %d\n metadata %d\n tracklist %d\n",
	  player,
	  caps & (1 << 0),
	  caps & (1 << 1),
	  caps & (1 << 2),
	  caps & (1 << 3),
	  caps & (1 << 4),
	  caps & (1 << 5),
	  caps & (1 << 6));

   if (!MPRIS_CAN_HAS_TRACKLIST(caps))
     {
	eina_stringshare_del(player);
	return;
     }

   players = eina_list_append(players, player);

   if (!dbus_active)
     {
	bus_name = player;
	DBG("use::%s", bus_name);
	dbus_active = EINA_TRUE;
     }
}

static void
_dbus_cb_name_owner_changed(void *data __UNUSED__, DBusMessage *msg)
{
   DBusError err;
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
		  dbus_active = EINA_TRUE;
	       }
	     else
	       {
		  dbus_active = EINA_FALSE;
	       }
	  }

	eina_stringshare_del(tmp);
     }
   else
     {
	/* new player appeared? */
	if (!eina_list_data_find(players, tmp))
	  {
	     const char *bus_tmp = bus_name;
	     bus_name = tmp;
	     _dbus_send_msg("/Player", "GetCaps", _dbus_cb_get_caps,
			    (void *) bus_name);
	     bus_name = bus_tmp;;
	  }
     }
}

static void
_dbus_cb_list_names(void *data __UNUSED__, DBusMessage *msg, DBusError *err)
{
   DBusMessageIter array, item;
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
		  bus_name = eina_stringshare_add(name);
		  _dbus_send_msg("/Player", "GetCaps", _dbus_cb_get_caps,
				 (void *) bus_name);
		  bus_name = NULL;
	       }

	     dbus_message_iter_next(&item);
	  }
     }
}

static int
_cb_key_down(Evry_Plugin *plugin, const Ecore_Event_Key *ev)
{
   GET_PLUGIN(p, plugin);
   Track *t;

   if ((ev->modifiers & ECORE_EVENT_MODIFIER_SHIFT) &&
       ((!strcmp(ev->key, "Up")) ||
	(!strcmp(ev->key, "Down"))))
     {
	if ((t = eina_list_nth(p->tracks, p->current_track)))
	  {
	     if (!EVRY_ITEM(t)->selected)
	       {
		  evry->item_changed(EVRY_ITEM(t), 1, 1);
		  return 1;
	       }
	  }
     }
   else if ((ev->modifiers & ECORE_EVENT_MODIFIER_SHIFT) &&
	    (!strcmp(ev->key, "Right")))
     {
	if ((t = eina_list_nth(p->tracks, p->current_track + 1)))
	  {
	     if (!EVRY_ITEM(t)->selected)
	       {
		  evry->item_changed(EVRY_ITEM(t), 1, 1);
		  _dbus_send_msg("/Player", "Next", NULL, NULL);
	       }
	  }
	return 1;
     }
   else if ((ev->modifiers & ECORE_EVENT_MODIFIER_SHIFT) &&
	    (!strcmp(ev->key, "Left")))
     {
	if ((t = eina_list_nth(p->tracks, p->current_track - 1)))
	  {
	     if (!EVRY_ITEM(t)->selected)
	       {
		  evry->item_changed(EVRY_ITEM(t), 1, 1);
		  _dbus_send_msg("/Player", "Prev", NULL, NULL);
	       }
	  }
	return 1;
     }

   return 0;
}

static Eina_Bool
_cb_plugin_selected(void *data, int type, void *event)
{
   Evry_Event_Item_Selected *ev = event;
   Track *t;

   if (ev->item != EVRY_ITEM(_plug))
     {
	plugin_selected = EINA_FALSE;
	return EINA_TRUE;
     }

   GET_PLUGIN(p, _plug);

   if (!p)
     return EINA_TRUE;

   plugin_selected = EINA_TRUE;

   if ((t = eina_list_nth(p->tracks, p->current_track)))
     {
	if (!EVRY_ITEM(t)->selected)
	  evry->item_changed(EVRY_ITEM(t), 1, 1);
     }

   return EINA_TRUE;
}

static int
_plugins_init(void)
{
   Evry_Action *act;
   Evry_Plugin *p;
   int prio = 15;

   if (evry_module->active)
     return EINA_TRUE;

   if (!evry->api_version_check(EVRY_API_VERSION))
     return EINA_FALSE;

   conn = e_dbus_bus_get(DBUS_BUS_SESSION);

   if (!conn)
     return EINA_FALSE;

   cb_name_owner_changed = e_dbus_signal_handler_add
     (conn, fdo_bus_name, fdo_path, fdo_interface, "NameOwnerChanged",
      _dbus_cb_name_owner_changed, NULL);

   e_dbus_list_names(conn, _dbus_cb_list_names, NULL);

   MPRIS_TRACK   = evry->type_register("MPRIS_TRACK");
   TRACKER_MUSIC = evry->type_register("TRACKER_MUSIC");
   FILE_LIST     = evry->type_register("FILE_LIST");


   p = EVRY_PLUGIN_NEW(Plugin, N_("Playlist"), "emblem-sound", MPRIS_TRACK,
		       _begin, _finish, _fetch, NULL);
   p->history     = EINA_FALSE;
   p->async_fetch = EINA_TRUE;
   p->cb_key_down = &_cb_key_down;

   select_handler = evry->event_handler_add(EVRY_EVENT_PLUGIN_SELECTED,
					    _cb_plugin_selected, NULL);

   if (evry->plugin_register(p, EVRY_PLUGIN_SUBJECT, 0))
     {
	Plugin_Config *pc = p->config;
	pc->view_mode = VIEW_MODE_LIST;
	pc->aggregate = EINA_FALSE;
	pc->top_level = EINA_FALSE;
	pc->trigger = eina_stringshare_add("l ");
     }

   _plug = (Plugin *) p;

#define ACTION_NEW(_label, _meth, _type1, _type2, _icon, _act, _check)  \
   act = EVRY_ACTION_NEW(_label, _type1, _type2, _icon, _act, _check);	\
   EVRY_ITEM(act)->icon_get = &_icon_get;				\
   evry->action_register(act,  prio++);					\
   actions = eina_list_append(actions, act);				\
   EVRY_ITEM_DATA_INT_SET(act, _meth);

#define PLUG_ACTION_NEW(_label, _meth, _type1, _type2, _icon, _act, _check) \
   act = EVRY_ACTION_NEW(_label, EVRY_TYPE_PLUGIN, _type2, _icon, _act, _check); \
   EVRY_ITEM(act)->icon_get = &_icon_get;				\
   p->actions = eina_list_append(p->actions, act);			\
   EVRY_ITEM_DATA_INT_SET(act, _meth);

   ACTION_NEW(N_("Play Track"), ACT_PLAY_TRACK,
	      MPRIS_TRACK, 0,
	      "media-playback-start",
	      _mpris_play_track, _mpris_check_item);

   ACTION_NEW(N_("Play"), ACT_PLAY,
		   MPRIS_TRACK, 0, "media-playback-start",
		   _mpris_player_action, _mpris_check_item);

   ACTION_NEW(N_("Pause"), ACT_PAUSE,
		   MPRIS_TRACK, 0, "media-playback-pause",
		   _mpris_player_action, _mpris_check_item);

   ACTION_NEW(N_("Stop"), ACT_STOP,
		   MPRIS_TRACK, 0, "media-playback-stop",
		   _mpris_player_action, _mpris_check_item);

   ACTION_NEW(N_("Forward"), ACT_FORWARD,
	      MPRIS_TRACK, 0, "media-seek-forward",
	      _mpris_player_position, _mpris_check_item);

   ACTION_NEW(N_("Rewind"), ACT_REWIND,
	      MPRIS_TRACK, 0, "media-seek-backward",
	      _mpris_player_position, _mpris_check_item);

   ACTION_NEW(N_("Remove Track"), ACT_REMOVE_TRACK,
	      MPRIS_TRACK, 0, "list-remove",
	      _mpris_tracklist_remove_track, _mpris_check_item);
   act->it1.accept_list = EINA_TRUE;

   ACTION_NEW(N_("Clear Playlist"), ACT_CLEAR,
		   MPRIS_TRACK, 0, "list-remove",
		   _mpris_tracklist_action_clear, _mpris_check_item);

   ACTION_NEW(N_("Play File"), ACT_PLAY_FILE,
	      EVRY_TYPE_FILE, 0, "media-playback-start",
	      _mpris_play_file, _mpris_check_file);
   act->remember_context = EINA_TRUE;

   ACTION_NEW(N_("Add Files..."), ACT_ADD_FILE,
		   MPRIS_TRACK, EVRY_TYPE_FILE, "list-add",
		   _mpris_add_files, NULL);

   ACTION_NEW(N_("Enqueue in Playlist"), ACT_ADD_FILE,
	      EVRY_TYPE_FILE, 0, "list-add",
	      _mpris_enqueue_files, _mpris_check_files);
   act->remember_context = EINA_TRUE;

   ACTION_NEW(N_("Add Music..."), ACT_ADD_FILE,
		   MPRIS_TRACK, TRACKER_MUSIC, "list-add",
		   _mpris_add_files, _mpris_check_add_music);

   ACTION_NEW(N_("Remove Duplicates"), 0,
		   MPRIS_TRACK, 0, "list-remove",
		   _mpris_remove_dups, NULL);

   ACTION_NEW(N_("Remove Missing Files"), 0,
		   MPRIS_TRACK, 0, "list-remove",
		   _mpris_remove_missing, NULL);


#undef ACTION_NEW

   return EINA_TRUE;
}

static void
_plugins_shutdown(void)
{
   Evry_Action *act;
   char *player;

   if (!evry_module->active) return;

   GET_EVRY_PLUGIN(p, _plug);
   EINA_LIST_FREE(p->actions, act)
     EVRY_ACTION_FREE(act);

   EVRY_PLUGIN_FREE(_plug);

   EINA_LIST_FREE(actions, act)
     EVRY_ACTION_FREE(act);

   if (conn)
     {
	if (cb_name_owner_changed) e_dbus_signal_handler_del(conn, cb_name_owner_changed);
	e_dbus_connection_close(conn);
     }

   ecore_event_handler_del(select_handler);
   select_handler = NULL;

   EINA_LIST_FREE(players, player)
     eina_stringshare_del(player);

   evry_module->active = EINA_FALSE;
}

/***************************************************************************/

EAPI E_Module_Api e_modapi = { E_MODULE_API_VERSION, PACKAGE };

EAPI void *
e_modapi_init(E_Module *m)
{
   char buf[4096];

   /* Location of message catalogs for localization */
   snprintf(buf, sizeof(buf), "%s/locale", e_module_dir_get(m));
   bindtextdomain(PACKAGE, buf);
   bind_textdomain_codeset(PACKAGE, "UTF-8");

   snprintf(buf, sizeof(buf), "%s/e-module.edj", e_module_dir_get(m));
   theme_file = strdup(buf);

   EVRY_MODULE_NEW(evry_module, _plugins_init, _plugins_shutdown);

   e_module_delayed_set(m, 1);

   return m;
}

EAPI int
e_modapi_shutdown(E_Module *m)
{
   EVRY_MODULE_FREE(evry_module);

   E_FREE(theme_file);

   return 1;
}

EAPI int
e_modapi_save(E_Module *m)
{
   return 1;
}

/***************************************************************************/
