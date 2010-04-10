/*
 * vim:ts=8:sw=3:sts=8:noexpandtab:cino=>5n-3f0^-2{2
 */

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
  Eina_List *tracks;

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
};


struct _Track
{
  Evry_Item base;
  int id;
  const char *title;
  const char *artist;
  const char *location;
  int length;
  
  DBusPendingCall *pnd;
  Eina_Bool ready;
};

static Plugin *_plug;
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

static void _mpris_get_metadata(Plugin *p);


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
  Track *t = (Track *)it;

  if (t->location) eina_stringshare_del(t->location);
  if (t->artist) eina_stringshare_del(t->artist);
  if (t->title) eina_stringshare_del(t->title);

  if (!t->ready)
    dbus_pending_call_cancel(t->pnd);
   
  E_FREE(t);
}
static void
_cb_free_item_changed(void *data, void *event)
{
  Evry_Event_Item_Changed *ev = event;

  evry_item_free(ev->item);
  E_FREE(ev);
}

static void
_dbus_cb_current_track(void *data, DBusMessage *reply, DBusError *error)
{
  Plugin *p = data;
  DBusMessage *msg;
  Evry_Item *it;
  int num;
  Evry_Event_Item_Changed *ev;
   
  if (!_dbus_check_msg(reply, error)) return;

  dbus_message_get_args(reply, error,
			DBUS_TYPE_INT32, (dbus_int32_t*) &(num),
			DBUS_TYPE_INVALID);

  if (num != p->current_track)
    {
      it = eina_list_nth(p->tracks, p->current_track);
      if (it)
	{
	  ev = E_NEW(Evry_Event_Item_Changed, 1);
	  ev->item = it;
	  evry_item_ref(it);
	  ecore_event_add(EVRY_EVENT_ITEM_CHANGED, ev, _cb_free_item_changed, NULL); 
	}
   
      p->current_track = num;
    }
   
  it = eina_list_nth(p->tracks, p->current_track);
   
  if (it)
    {
      ev = E_NEW(Evry_Event_Item_Changed, 1);
      ev->item = it;
      evry_item_ref(it);
      ecore_event_add(EVRY_EVENT_ITEM_CHANGED, ev, _cb_free_item_changed, NULL); 
    }
}

static void
_dbus_cb_tracklist_metadata(void *data, DBusMessage *reply, DBusError *error)
{
  DBusMessageIter array, item, iter, iter_val;
  Track *t = data;
  int type;
  char *key, *tmp;
  PLUGIN(p, EVRY_ITEM(t)->plugin);
   
  p->fetch_tracks--;

  DBG("add %i", t->id);
   
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
	  if (dbus_message_iter_get_arg_type(&iter) != DBUS_TYPE_VARIANT)
	    goto error;

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
		t->location = eina_stringshare_add(tmp); 
	    }
	  else if (!strcmp(key, "mtime"))
	    {
	      dbus_message_iter_recurse (&iter, &iter_val);
	      dbus_message_iter_get_basic (&iter_val, &(t->length));
	    }
	  dbus_message_iter_next(&item);
	}

      if (t->artist && t->title)
	{
	  char buf[128];
	  snprintf(buf, sizeof(buf), "%s - %s", t->artist, t->title);
	  t->base.label = eina_stringshare_add(buf);
	}
      else if (t->title)
	{
	  t->base.label = eina_stringshare_add(t->title);
	}
      else if (t->location)
	{
	  const char *file = ecore_file_file_get(t->location);
	  char *tmp = evry_util_unescape(file, 0);
	  if (tmp)
	    {
	      t->base.label = eina_stringshare_add(tmp);
	      free(tmp);
	    }
	  else goto error;
	}
      else goto error;
    }

  DBG("add %s, %d", t->base.label, t->id);

  t->ready = EINA_TRUE;
   
  if (!p->input || evry_fuzzy_match(t->base.label, p->input))
    EVRY_PLUGIN_ITEM_APPEND(p, EVRY_ITEM(t));

  if (!p->fetch_tracks)
    {
      _dbus_send_msg("/TrackList", "GetCurrentTrack",
		     _dbus_cb_current_track, p); 
   	
      evry_plugin_async_update(EVRY_PLUGIN(p), EVRY_ASYNC_UPDATE_ADD);
    }
   
  return;

 error:
  if (!p->fetch_tracks)
    evry_plugin_async_update(EVRY_PLUGIN(p), EVRY_ASYNC_UPDATE_ADD);

  p->tracks = eina_list_remove(p->tracks, t);
  evry_item_free(EVRY_ITEM(t));
   
  return;
}

static void
_mpris_get_metadata(Plugin *p)
{
  int cnt;
  Track *t;

  DBG("tracklist changed %d, %d", p->tracklist_cnt, p->fetch_tracks);
  p->fetch_tracks = p->tracklist_cnt;
   
  EVRY_PLUGIN_ITEMS_CLEAR(p);
   
  EINA_LIST_FREE(p->tracks, t)
    evry_item_free(EVRY_ITEM(t)); 
   
  for (cnt = 0; cnt < p->fetch_tracks; cnt++)
    {
      t = E_NEW(Track, 1);
      t->id = cnt;
      EVRY_ITEM(t)->transient = EINA_TRUE;
      evry_item_new(EVRY_ITEM(t), EVRY_PLUGIN(p), NULL, _item_free);

      t->pnd = _dbus_send_msg_int("/TrackList", "GetMetadata",
				  _dbus_cb_tracklist_metadata, t, cnt); 

      p->tracks = eina_list_append(p->tracks, t);
    }
}


static void
_dbus_cb_tracklist_length(void *data, DBusMessage *reply, DBusError *error)
{
  DBusMessage *msg;
  Plugin *p = data;

  if (!_dbus_check_msg(reply, error)) return;

  dbus_message_get_args(reply, error,
			DBUS_TYPE_INT32, (dbus_int32_t*) &(p->tracklist_cnt),
			DBUS_TYPE_INVALID);

  _mpris_get_metadata(p);
}

static void
_set_status(Plugin *p, DBusMessage *msg)
{
  DBusMessageIter iter, array;
  Evry_Event_Item_Changed *ev;
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
  if (it)
    {
      ev = E_NEW(Evry_Event_Item_Changed, 1);
      ev->item = it;
      evry_item_ref(it);
      ecore_event_add(EVRY_EVENT_ITEM_CHANGED, ev, _cb_free_item_changed, NULL); 
    }
}

static void
_dbus_cb_get_status(void *data, DBusMessage *reply, DBusError *error)
{
  if (!_dbus_check_msg(reply, error)) return;

  _set_status(data, reply);
}

static void
_dbus_cb_tracklist_change(void *data, DBusMessage *msg)
{
  PLUGIN(p, data);
   
  DBG("tracklist change");

  /* FIXME will be needed in some other places.. */
  p->next_track = 0;
   
  dbus_message_get_args(msg, NULL,
			DBUS_TYPE_INT32, (dbus_int32_t*) &(p->tracklist_cnt),
			DBUS_TYPE_INVALID);

  _mpris_get_metadata(p);
}

static void
_dbus_cb_track_change(void *data, DBusMessage *msg)
{
  PLUGIN(p, data);

  /* XXX just fsckin give the track nr. if I want metadata I would ask for it!*/

  _dbus_send_msg("/TrackList", "GetCurrentTrack", _dbus_cb_current_track, p); 
}

static void
_dbus_cb_status_change(void *data, DBusMessage *msg)
{
  PLUGIN(p, data);
   
  DBusMessageIter iter, array;
   
  dbus_message_iter_init(msg, &iter);

  if (dbus_message_iter_get_arg_type(&iter) == DBUS_TYPE_STRUCT)
    {
      _set_status(p, msg);
    }
  else if (dbus_message_iter_get_arg_type(&iter) == DBUS_TYPE_INT32)
    {
      /* XXX audacious.. */
      _dbus_send_msg("/Player", "GetStatus", _dbus_cb_get_status, p); 
    }
  else
    {
      ERR("hooray!");
    }
}

static Evry_Plugin *
_begin(Evry_Plugin *plugin, const Evry_Item *item __UNUSED__)
{
  PLUGIN(p, plugin);
   
  if (!conn || !active) return 0;

  _dbus_send_msg("/TrackList", "GetLength", _dbus_cb_tracklist_length, p); 
   
  _dbus_send_msg("/Player", "GetStatus", _dbus_cb_get_status, p);
   
  cb_tracklist_change = e_dbus_signal_handler_add
    (conn, bus_name, "/TrackList", mpris_interface, "TrackListChange",
     _dbus_cb_tracklist_change, p);

  cb_player_track_change = e_dbus_signal_handler_add
    (conn, bus_name, "/Player", mpris_interface, "TrackChange",
     _dbus_cb_track_change, p);

  cb_player_status_change = e_dbus_signal_handler_add
    (conn, bus_name, "/Player", mpris_interface, "StatusChange",
     _dbus_cb_status_change, p);

  return EVRY_PLUGIN(p);
}

static void
_cleanup(Evry_Plugin *plugin)
{
  PLUGIN(p, plugin);
   
  Evry_Item *it;
   
  /* free instances */
  if (active)
    {
      e_dbus_signal_handler_del(conn, cb_tracklist_change);
      e_dbus_signal_handler_del(conn, cb_player_track_change);
      e_dbus_signal_handler_del(conn, cb_player_status_change);
    }

  if (p->input)
    eina_stringshare_del(p->input); 
  p->input = NULL;

  EINA_LIST_FREE(p->tracks, it)
    evry_item_free(it); 
   
  EVRY_PLUGIN_ITEMS_CLEAR(p);
}


static int
_action(Evry_Plugin *p, const Evry_Item *it)
{
  return EVRY_ACTION_FINISHED;
}

static int
_fetch(Evry_Plugin *plugin, const char *input)
{
  PLUGIN(p, plugin);

  Eina_List *l;
  Track *t;
   
  if (p->input)
    eina_stringshare_del(p->input); 

  /* cant filter item is play_track is not supported... */
  if (p->support.play_track && input)
    p->input = eina_stringshare_add(input); 
  else
    p->input = NULL;
   
  EVRY_PLUGIN_ITEMS_CLEAR(p);
   
  EINA_LIST_FOREACH(p->tracks, l, t)
    {
      if (t->ready && (!input || evry_fuzzy_match(t->base.label, input)))
	EVRY_PLUGIN_ITEM_APPEND(p, EVRY_ITEM(t));
    }
   
  return 1;
}

static Evas_Object *
_icon_get(Evry_Plugin *plugin, const Evry_Item *it, Evas *e)
{
  PLUGIN(p, plugin);
   
  ITEM_TRACK(t, it);

  if (t->id == p->current_track)
    {
      if (p->status.playing == 0)
	return evry_icon_theme_get("media-playback-start", e);
      else if (p->status.playing == 1)
	return evry_icon_theme_get("media-playback-pause", e);
      else if (p->status.playing == 2)
	return evry_icon_theme_get("media-playback-stop", e);

    }

  return NULL;
}

/* static void
 * _mpris_play_track_hack(void *data, DBusMessage *reply, DBusError *error)
 * {
 *    DBusMessage *msg;
 *    PLUGIN(p, data);
 * 
 *    if (!p->next_track)
 *      {
 * 	DBG("PLAY");
 * 	_dbus_send_msg("/Player", "Play", NULL, NULL);
 * 	return;
 *      }
 *    
 *    DBG("next %d", p->next_track);
 *    
 *    if (p->next_track > 0)
 *      {
 * 	DBG("NEXT");
 * 	msg = dbus_message_new_method_call(bus_name, "/Player",
 * 					   mpris_interface,
 * 					   "Next");
 * 	p->next_track--;
 *      }
 *    else
 *      {
 * 	DBG("PREV");
 * 	msg = dbus_message_new_method_call(bus_name, "/Player",
 * 					   mpris_interface,
 * 					   "Prev");
 * 	p->next_track++;
 *      }
 * 
 *    e_dbus_message_send(conn, msg, _mpris_play_track_hack, -1, p);
 *        
 *    dbus_message_unref(msg);
 * } */


/** ACTIONS **/
static int
_mpris_play_track(Evry_Action *act)
{
  DBusMessage *msg;
   
  ITEM_TRACK(t, act->item1);
  PLUGIN(p, t->base.plugin);
   
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
			       DBUS_TYPE_INT32, &(t->id),
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
  ITEM_TRACK(t, act->item1);

  _dbus_send_msg_int("/TrackList", "DelTrack", NULL, NULL, t->id); 
   
  return 1;
}

static int
_mpris_player_action(Evry_Action *act)
{
  char *method = (char *)act->data;
   
  _dbus_send_msg("/Player", method, NULL, NULL); 

  return 1;
}

/* static int
 * _mpris_tracklist_action(Evry_Action *act)
 * {
 *   char *method = (char *)act->data;
 *    
 *   _dbus_send_msg("/TrackList", method, NULL, NULL); 
 * 
 *   return 1;
 * } */

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
   
  ITEM_TRACK(t, act->item1);
   
  if (!_dbus_check_msg(reply, error)) return;
   
  dbus_message_get_args(reply, NULL,
			DBUS_TYPE_INT32, (dbus_int32_t*) &(pos),
			DBUS_TYPE_INVALID);

  if (!strcmp(act->data, "Forward"))
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
   
  return 1;
}

static int
_mpris_play_file(Evry_Action *act)
{
  Evry_Item_File *file;
  DBusMessage *msg;
  int play = (strcmp((char *) act->data, "e"));
  char *buf;

  if (!strcmp(act->type_in1, "MPRIS_TRACK"))
    {
	
      file = (Evry_Item_File *)act->item2;
      if (strncmp(file->mime, "audio/", 6) != 0)
	return 0;
    }
  else
    {
      file = (Evry_Item_File *)act->item1;
    }

  if (strncmp(file->path, "file://", 7))
    {
      buf = malloc(sizeof(char) * (strlen(file->path) + 8));
      sprintf(buf, "file://%s", file->path);
    }
  else
    {
      buf = malloc(sizeof(char) * (strlen(file->path) + 1));
      sprintf(buf, "%s", file->path);
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

static int
_mpris_check_item(Evry_Action *act, const Evry_Item *it)
{
  PLUGIN(p, it->plugin);
   
  if (!strcmp((char *)act->data, "PlayTrack"))
    {
      if (p->current_track == ((Track *)it)->id)
	return 0;
    }
  else if (!strcmp((char *)act->data, "Stop"))
    {
      if (p->status.playing == 2) return 0;
    }
  else if (!strcmp((char *)act->data, "Play"))
    {
      if (p->status.playing == 0) return 0;
    }
  else if (!strcmp((char *)act->data, "Pause"))
    {
      if (p->status.playing != 0) return 0;
    }
  else if (!strcmp((char *)act->data, "Forward"))
    {
      if (p->current_track != ((Track *)it)->id ||
	  p->status.playing != 0) return 0;
    }
  else if (!strcmp((char *)act->data, "Rewind"))
    {
      if (p->current_track != ((Track *)it)->id ||
	  p->status.playing != 0) return 0;
    }
  else if (!strcmp((char *)act->data, "Clear"))
    {
      if (strcmp(bus_name, "org.mpris.xmms2") &&
	  strcmp(bus_name, "org.mpris.audacious") &&
	  strcmp(bus_name, "org.mpris.corn"))
	return 0;
    }
   
  return 1;
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

static int
_cb_key_down(Evry_Plugin *plugin, const Ecore_Event_Key *ev)
{
  PLUGIN(p, plugin);

  if (p->support.play_track)
    return 0;

  if (!strcmp(ev->key, "Down"))
    {
      printf("play next\n");
      _dbus_send_msg("/Player", "Next", NULL, NULL); 
      return 1;
    }
  else if (!strcmp(ev->key, "Up"))
    {
      printf("play prev\n");
      _dbus_send_msg("/Player", "Prev", NULL, NULL); 
      return 1;
    }

  return 0;
}

static void
_plugin_free(Evry_Plugin *plugin)
{
  PLUGIN(p, plugin);

  Evry_Item *it;
  
  if (active)
    {
      e_dbus_signal_handler_del(conn, cb_tracklist_change);
      e_dbus_signal_handler_del(conn, cb_player_track_change);
      e_dbus_signal_handler_del(conn, cb_player_status_change);
    }

  if (p->input)
    eina_stringshare_del(p->input); 

  EINA_LIST_FREE(p->tracks, it)
    evry_item_free(it); 

  E_FREE(p);
}

static Eina_Bool
module_init(void)
{
  Evry_Action *act;

  if (!evry_api_version_check(EVRY_API_VERSION))
    return EINA_FALSE;

  conn = e_dbus_bus_get(DBUS_BUS_SESSION);

  if (!conn) return EINA_FALSE;

  cb_name_owner_changed = e_dbus_signal_handler_add
    (conn, fdo_bus_name, fdo_path, fdo_interface, "NameOwnerChanged",
     _dbus_cb_name_owner_changed, NULL);

  e_dbus_list_names(conn, _dbus_cb_list_names, NULL);

  _plug = E_NEW(Plugin, 1);
  evry_plugin_new(EVRY_PLUGIN(_plug), "Playlist", type_subject, NULL, "MPRIS_TRACK",
		  1, "emblem-sound", NULL,
		  _begin, _cleanup, _fetch, _action, _icon_get, _plugin_free);
  /* EVRY_PLUGIN(_plug)->cb_key_down = &_cb_key_down; */
  EVRY_PLUGIN(_plug)->aggregate = EINA_FALSE;
  evry_plugin_register(EVRY_PLUGIN(_plug), 0);

  act = evry_action_new("Play Track", "MPRIS_TRACK", NULL, NULL, "media-playback-start",
			_mpris_play_track, _mpris_check_item, NULL, NULL,NULL, NULL);
  act->data = "PlayTrack";
  evry_action_register(act,  0);
  actions = eina_list_append(actions, act);

  act = evry_action_new("Remove Track", "MPRIS_TRACK", NULL, NULL, "list-remove",
			_mpris_tracklist_remove_track, NULL, NULL, NULL,NULL, NULL);
  evry_action_register(act,  0);
  actions = eina_list_append(actions, act);

  act = evry_action_new("Play", "MPRIS_TRACK", NULL, NULL, "media-playback-start",
			_mpris_player_action, _mpris_check_item, NULL, NULL,NULL, NULL);
  act->data = "Play";
  evry_action_register(act,  0);
  actions = eina_list_append(actions, act);

  act = evry_action_new("Pause", "MPRIS_TRACK", NULL, NULL, "media-playback-pause",
			_mpris_player_action, _mpris_check_item, NULL, NULL,NULL, NULL);
  act->data = "Pause";
  evry_action_register(act,  0);
  actions = eina_list_append(actions, act);

  act = evry_action_new("Stop", "MPRIS_TRACK", NULL, NULL, "media-playback-stop",
			_mpris_player_action, _mpris_check_item, NULL, NULL,NULL, NULL);
  act->data = "Stop";
  evry_action_register(act,  0);
  actions = eina_list_append(actions, act);

  act = evry_action_new("Forward", "MPRIS_TRACK", NULL, NULL, "media-seek-forward",
			_mpris_player_position, _mpris_check_item, NULL, NULL,NULL, NULL);
  act->data = "Forward";
  evry_action_register(act,  0);
  actions = eina_list_append(actions, act);

  act = evry_action_new("Rewind", "MPRIS_TRACK", NULL, NULL, "media-seek-backward",
			_mpris_player_position, _mpris_check_item, NULL, NULL,NULL, NULL);
  act->data = "Rewind";
  evry_action_register(act,  0);
  actions = eina_list_append(actions, act);

  act = evry_action_new("Clear Playlist", "MPRIS_TRACK", NULL, NULL, "media-playlist-clear",
			_mpris_tracklist_action_clear , _mpris_check_item, NULL, NULL,NULL, NULL);
  act->data = "Clear";
  evry_action_register(act,  0);
  actions = eina_list_append(actions, act);

  act = evry_action_new("Enqueue File", "FILE", NULL, NULL, "list-add",
			_mpris_play_file, _mpris_check_file, NULL, NULL,NULL, NULL);
  act->data = "e";
  evry_action_register(act,  0);
  actions = eina_list_append(actions, act);

  act = evry_action_new("Play File", "FILE", NULL, NULL, "media-playback-start",
			_mpris_play_file, _mpris_check_file, NULL, NULL,NULL, NULL);
  act->data = "p";
  evry_action_register(act,  1);
  actions = eina_list_append(actions, act);

  /* act = evry_action_new("Enqueue Files", NULL, "FILE", NULL, "list-add",
   * 			 _mpris_play_file, NULL, NULL, NULL,NULL);
   * act->data = "e";
   * evry_action_register(act,  0);
   * actions = eina_list_append(actions, act); */

  return EINA_TRUE;
}

static void
module_shutdown(void)
{
  Evry_Action *act;

  EVRY_PLUGIN_FREE(_plug);

  EINA_LIST_FREE(actions, act)
    evry_action_free(act);
}


/***************************************************************************/
/**/
/* actual module specifics */

static E_Module *_module = NULL;
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
  _module = m;

  if (e_datastore_get("everything_loaded"))
    _active = module_init();
   
  e_module_delayed_set(m, 1); 

  return m;
}

EAPI int
e_modapi_shutdown(E_Module *m)
{
  char *player;

  if (_active && e_datastore_get("everything_loaded"))
    module_shutdown();

  if (conn)
    {
      e_dbus_signal_handler_del(conn, cb_name_owner_changed);
      e_dbus_connection_close(conn);
    }

  EINA_LIST_FREE(players, player)
    eina_stringshare_del(player);

  _module = NULL;
   
  return 1;
}

EAPI int
e_modapi_save(E_Module *m)
{
  return 1;
}

/**/
/***************************************************************************/

