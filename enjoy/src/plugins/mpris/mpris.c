#include <Eina.h>
#include <E_DBus.h>
#include <Ecore.h>

#include "plugin.h"
#include "song.h"

typedef struct _MPRIS_Method MPRIS_Method;
typedef struct _MPRIS_Signal MPRIS_Signal;

static int _mpris_log_domain = -1;

#ifdef CRITICAL
#undef CRITICAL
#endif
#ifdef ERR
#undef ERR
#endif
#ifdef WRN
#undef WRN
#endif
#ifdef INF
#undef INF
#endif
#ifdef DBG
#undef DBG
#endif

#define CRITICAL(...) EINA_LOG_DOM_CRIT(_mpris_log_domain, __VA_ARGS__)
#define ERR(...)      EINA_LOG_DOM_ERR(_mpris_log_domain, __VA_ARGS__)
#define WRN(...)      EINA_LOG_DOM_WARN(_mpris_log_domain, __VA_ARGS__)
#define INF(...)      EINA_LOG_DOM_INFO(_mpris_log_domain, __VA_ARGS__)
#define DBG(...)      EINA_LOG_DOM_DBG(_mpris_log_domain, __VA_ARGS__)


/*
 * Capabilities and player status values conform to the MPRIS 1.0 standard:
 * http://www.mpris.org/1.0/spec.html
 */
typedef enum {
  MPRIS_CAPABILITY_CAN_GO_NEXT = 1 << 0,
  MPRIS_CAPABILITY_CAN_GO_PREV = 1 << 1,
  MPRIS_CAPABILITY_CAN_PAUSE = 1 << 2,
  MPRIS_CAPABILITY_CAN_PLAY = 1 << 3,
  MPRIS_CAPABILITY_CAN_SEEK = 1 << 4,
  MPRIS_CAPABILITY_CAN_PROVIDE_METADATA = 1 << 5,
  MPRIS_CAPABILITY_HAS_TRACKLIST = 1 << 6
} Mpris_Capabilities;


#define APPLICATION_NAME "org.mpris.enjoy"
#define PLAYER_INTERFACE_NAME "org.freedesktop.MediaPlayer"
#define ROOT_NAME "/Root" /* should really be "/", but this doesn't work correctly :( */
#define TRACKLIST_NAME "/TrackList"
#define PLAYER_NAME "/Player"


static void _mpris_signal_player_caps_change(int caps);
static void _mpris_signal_player_status_change(int playback, int shuffle, int repeat, int endless);
static void _mpris_signal_player_track_change(const Song *song);
static void _mpris_signal_tracklist_tracklist_change(int size);

static void _mpris_signal_emit(const char *root, const char *signal_name, int arg_type, void *arg_value);
static void _mpris_signals_add(const char *root, const MPRIS_Signal *signals);
static void _mpris_methods_add(const char *root, const MPRIS_Method *methods);
static void _mpris_append_dict_entry(DBusMessageIter *dict_iter, const char *key,
                                     int value_type, const void *value);
static void _mpris_signal_emit(const char *root, const char *signal_name, int arg_type,
                               void *arg_value);
static DBusMessage *_mpris_player_next(E_DBus_Object *obj __UNUSED__, DBusMessage *msg);
static DBusMessage *_mpris_player_previous(E_DBus_Object *obj __UNUSED__, DBusMessage *msg);
static DBusMessage *_mpris_player_pause(E_DBus_Object *obj __UNUSED__, DBusMessage *msg);
static DBusMessage *_mpris_player_stop(E_DBus_Object *obj __UNUSED__, DBusMessage *msg);
static DBusMessage *_mpris_player_play(E_DBus_Object *obj __UNUSED__, DBusMessage *msg);
static DBusMessage *_mpris_player_seek(E_DBus_Object *obj __UNUSED__, DBusMessage *msg);
static DBusMessage *_mpris_root_identity(E_DBus_Object *obj __UNUSED__, DBusMessage *msg);
static DBusMessage *_mpris_root_quit(E_DBus_Object *obj __UNUSED__, DBusMessage *msg);
static DBusMessage *_mpris_root_version(E_DBus_Object *obj __UNUSED__, DBusMessage *msg);
static DBusMessage *_mpris_player_caps_get(E_DBus_Object *obj __UNUSED__, DBusMessage *msg);
static DBusMessage *_mpris_player_volume_set(E_DBus_Object *obj __UNUSED__, DBusMessage *msg);
static DBusMessage *_mpris_player_volume_get(E_DBus_Object *obj __UNUSED__, DBusMessage *msg);
static DBusMessage *_mpris_player_repeat_set(E_DBus_Object *obj __UNUSED__, DBusMessage *msg);
static DBusMessage *_mpris_player_status_get(E_DBus_Object *obj __UNUSED__, DBusMessage *msg);
static DBusMessage *_mpris_player_position_set(E_DBus_Object *obj __UNUSED__, DBusMessage *msg);
static DBusMessage *_mpris_player_position_get(E_DBus_Object *obj __UNUSED__, DBusMessage *msg);
static DBusMessage *_mpris_song_metadata_reply(DBusMessage *msg, const Song *song);
static DBusMessage *_mpris_player_metadata_get(E_DBus_Object *obj __UNUSED__, DBusMessage *msg);
static DBusMessage *_mpris_tracklist_current_track_get(E_DBus_Object *obj __UNUSED__, DBusMessage *msg);
static DBusMessage *_mpris_tracklist_count(E_DBus_Object *obj __UNUSED__, DBusMessage *msg);
static DBusMessage *_mpris_tracklist_metadata_get(E_DBus_Object *obj __UNUSED__, DBusMessage *msg);
static DBusMessage *_mpris_tracklist_shuffle_set(E_DBus_Object *obj __UNUSED__, DBusMessage *msg);

static void _cb_dbus_request_name(void *data, DBusMessage *msg __UNUSED__, DBusError *err);

struct _MPRIS_Method {
  const char *name;
  const char *param_sig;
  const char *return_sig;
  void *callback;
};

struct _MPRIS_Signal {
  const char *name;
  const char *sig;
};


static E_DBus_Connection *conn = NULL;
static E_DBus_Object *bus_obj = NULL;
static Eina_Hash *interface_list = NULL;
static Eina_List *ev_handlers = NULL;


static const char *object_list[] = { ROOT_NAME, TRACKLIST_NAME, PLAYER_NAME, NULL };
static const MPRIS_Signal mpris_player_signals[] = {
  /* Emitted whenever a new song is played; gives the song metadata */
  { "TrackChange",  "a{sv}" },
  /* Emitted whenever player's status changes */
  { "StatusChange", "(iiii)" },
  /* Emitted whenever player's capabilities changes */
  { "CapsChange",   "i" },
  { NULL, NULL }
};
static const MPRIS_Signal mpris_tracklist_signals[] = {
  /* Emitted whenever the tracklist changes; gives the number of items */
  { "TrackListChange", "i" },
  { NULL, NULL }
};
static const MPRIS_Method mpris_root_methods[] = {
  /* Returns a string representing the player name */
  { "Identity",     "",    "s",    _mpris_root_identity },
  /* Quits the player */
  { "Quit",         "",    "",     _mpris_root_quit },
  /* Returns a tuple containing the version of MPRIS protocol implemented */
  { "MprisVersion", "",    "(qq)", _mpris_root_version },
  { NULL,           NULL,  NULL,   NULL }
};
static const MPRIS_Method mpris_player_methods[] = {
  /* Goes to the next song */
  { "Next",        "",  "",       _mpris_player_next },
  /* Goes to the previous song */
  { "Prev",        "",  "",       _mpris_player_previous },
  /* Pauses the song */
  { "Pause",       "",  "",       _mpris_player_pause },
  /* Stops the song */
  { "Stop",        "",  "",       _mpris_player_stop },
  /* If playing, rewind to the beginning of the current track; else, start playing */
  { "Play",        "",  "",       _mpris_player_play },
  /* Seek the current song by given miliseconds */
  { "Seek",        "x", "",       _mpris_player_seek },
  /* Toggle the current track repeat */
  { "Repeat",      "b", "",       _mpris_player_repeat_set },
  /* Return the status of the media player */
  { "GetStatus",   "",  "(iiii)", _mpris_player_status_get },
  /* Gets all the metadata for the currently played element */
  { "GetMetadata", "",  "a{sv}",  _mpris_player_metadata_get },
  /* Returns the media player's current capabilities */
  { "GetCaps",     "",  "i",      _mpris_player_caps_get },
  /* Sets the volume */
  { "VolumeSet",   "i", "",       _mpris_player_volume_set },
  /* Gets the current volume */
  { "VolumeGet",   "",  "i",      _mpris_player_volume_get },
  /* Sets the playing position (in ms) */
  { "PositionSet", "i", "",       _mpris_player_position_set },
  /* Gets the playing position (in ms) */
  { "PositionGet", "",  "i",      _mpris_player_position_get },
  { NULL,          NULL,  NULL,   NULL }
};
static const MPRIS_Method mpris_tracklist_methods[] = {
  /* Gives all the metadata available at the given position in the track list */
  { "GetMetadata",     "i",   "a{sv}", _mpris_tracklist_metadata_get },
  /* Returns the position of the current URI in the track list */
  { "GetCurrentTrack", "",    "i",     _mpris_tracklist_current_track_get },
  /* Returns the number of elements in the track list */
  { "GetLength",       "",    "i",     _mpris_tracklist_count },
  /* Appends an URI to the track list */
  { "AddTrack",        "sb",  "i",     NULL },
  /* Removes an URL from the track list */
  { "DelTrack",        "i",   "",      NULL },
  /* Toggle playlist loop */
  { "SetLoop",         "b",   "",      NULL },
  /* Toggle playlist shuffle/random */
  { "SetRandom",       "b",   "",      _mpris_tracklist_shuffle_set },
  { NULL,              NULL,  NULL,    NULL }
};

static int
_caps_to_mpris_bits(const Enjoy_Player_Caps caps)
{
   int bits = 0;
   if (caps.can_go_next) bits |= MPRIS_CAPABILITY_CAN_GO_NEXT;
   if (caps.can_go_prev) bits |= MPRIS_CAPABILITY_CAN_GO_PREV;
   if (caps.can_pause) bits |= MPRIS_CAPABILITY_CAN_PAUSE;
   if (caps.can_play) bits |= MPRIS_CAPABILITY_CAN_PLAY;
   if (caps.can_seek) bits |= MPRIS_CAPABILITY_CAN_SEEK;
   if (caps.can_provide_metadata) bits |= MPRIS_CAPABILITY_CAN_PROVIDE_METADATA;
   if (caps.has_tracklist) bits |= MPRIS_CAPABILITY_HAS_TRACKLIST;
   return bits;
}

static Eina_Bool
_cb_player_caps_change(void *data __UNUSED__, int type __UNUSED__, void *event __UNUSED__)
{
   Enjoy_Player_Caps caps = enjoy_player_caps_get();
   int bits = _caps_to_mpris_bits(caps);
   _mpris_signal_player_caps_change(bits);
   return ECORE_CALLBACK_PASS_ON;
}

static Eina_Bool
_cb_player_status_change(void *data __UNUSED__, int type __UNUSED__, void *event __UNUSED__)
{
   Enjoy_Player_Status status = enjoy_player_status_get();
   _mpris_signal_player_status_change
     (status.playback, status.shuffle, status.repeat, status.endless);
   return ECORE_CALLBACK_PASS_ON;
}

static Eina_Bool
_cb_player_track_change(void *data __UNUSED__, int type __UNUSED__, void *event __UNUSED__)
{
   _mpris_signal_player_track_change(enjoy_song_current_get());
   return ECORE_CALLBACK_PASS_ON;
}

static Eina_Bool
_cb_player_tracklist_change(void *data __UNUSED__, int type __UNUSED__, void *event __UNUSED__)
{
   _mpris_signal_tracklist_tracklist_change(enjoy_playlist_count());
   return ECORE_CALLBACK_PASS_ON;
}

static Eina_Bool
mpris_enable(Enjoy_Plugin *p __UNUSED__)
{
#define EV_HANDLER(ev, func, data)              \
   ev_handlers = eina_list_append               \
     (ev_handlers, ecore_event_handler_add(ev, func, data))

   EV_HANDLER(ENJOY_EVENT_PLAYER_CAPS_CHANGE, _cb_player_caps_change, NULL);
   EV_HANDLER(ENJOY_EVENT_PLAYER_STATUS_CHANGE, _cb_player_status_change, NULL);
   EV_HANDLER(ENJOY_EVENT_PLAYER_TRACK_CHANGE, _cb_player_track_change, NULL);
   EV_HANDLER(ENJOY_EVENT_TRACKLIST_TRACKLIST_CHANGE,
              _cb_player_tracklist_change, NULL);
#undef EV_HANDLER

   e_dbus_request_name(conn, APPLICATION_NAME, DBUS_NAME_FLAG_DO_NOT_QUEUE,
                       _cb_dbus_request_name, NULL);
   return EINA_TRUE;
}

static Eina_Bool
mpris_disable(Enjoy_Plugin *p __UNUSED__)
{
   Ecore_Event_Handler *eh;

   if (interface_list)
     {
        eina_hash_free(interface_list);
        interface_list = NULL;
     }
   if (bus_obj)
     {
        e_dbus_object_free(bus_obj);
        bus_obj = NULL;
     }

   EINA_LIST_FREE(ev_handlers, eh)
     ecore_event_handler_del(eh);

   return EINA_TRUE;
}

static const Enjoy_Plugin_Api api = {
  ENJOY_PLUGIN_API_VERSION,
  mpris_enable,
  mpris_disable
};

static Eina_Bool
mpris_init(void)
{
   if (_mpris_log_domain < 0)
     {
        _mpris_log_domain = eina_log_domain_register
          ("enjoy-mpris", EINA_COLOR_LIGHTCYAN);
        if (_mpris_log_domain < 0)
          {
             EINA_LOG_CRIT("Could not register log domain 'enjoy-mpris'");
             return EINA_FALSE;
          }
     }

   if (!ENJOY_ABI_CHECK())
     {
        ERR("ABI versions differ: enjoy=%u, mpris=%u",
            enjoy_abi_version(), ENJOY_ABI_VERSION);
        goto error;
     }

   if (conn) return EINA_TRUE;

   e_dbus_init();
   conn = e_dbus_bus_get(DBUS_BUS_SESSION);
   if (!conn)
     {
        ERR("Could not get DBus session bus");
        goto error;
     }

   enjoy_plugin_register("listener/mpris", &api, ENJOY_PLUGIN_PRIORITY_HIGH);

   return EINA_TRUE;

 error:
   eina_log_domain_unregister(_mpris_log_domain);
   _mpris_log_domain = -1;
   return EINA_FALSE;
}

void
mpris_shutdown(void)
{
   if (!conn) return;

   e_dbus_shutdown();
   conn = NULL;

   if (_mpris_log_domain >= 0)
     {
        eina_log_domain_unregister(_mpris_log_domain);
        _mpris_log_domain = -1;
     }
}

static void
_cb_dbus_request_name(void *data __UNUSED__, DBusMessage *msg, DBusError *err)
{
   DBusError new_err;
   dbus_uint32_t msgtype;
   int i;

   if (dbus_error_is_set(err))
     {
        ERR("Could not get DBus name: %s", err->message);
        return;
     }

   if (interface_list)
     {
        eina_hash_free(interface_list);
        interface_list = NULL;
     }

   if (bus_obj)
     {
        e_dbus_object_free(bus_obj);
        bus_obj = NULL;
     }

   dbus_error_init(&new_err);
   dbus_message_get_args
     (msg, &new_err, DBUS_TYPE_UINT32, &msgtype, DBUS_TYPE_INVALID);
   if (msgtype != DBUS_REQUEST_NAME_REPLY_PRIMARY_OWNER)
     {
        ERR("Could not get the DBus name: reply=%d", msgtype);
        return;
     }

   interface_list = eina_hash_string_small_new
     ((Eina_Free_Cb)e_dbus_interface_unref);
   for (i = 0; object_list[i]; i++)
     {
        bus_obj = e_dbus_object_add(conn, object_list[i], NULL);
        E_DBus_Interface *iface = e_dbus_interface_new(PLAYER_INTERFACE_NAME);
        e_dbus_object_interface_attach(bus_obj, iface);
        eina_hash_add(interface_list, object_list[i], iface);
     }

   _mpris_signals_add(PLAYER_NAME, mpris_player_signals);
   _mpris_signals_add(TRACKLIST_NAME, mpris_tracklist_signals);

   _mpris_methods_add(ROOT_NAME, mpris_root_methods);
   _mpris_methods_add(PLAYER_NAME, mpris_player_methods);
   _mpris_methods_add(TRACKLIST_NAME, mpris_tracklist_methods);
}

static void
_mpris_signals_add(const char *root, const MPRIS_Signal *signals)
{
   E_DBus_Interface *interface;
   int i;

   if (!conn) return;

   interface = eina_hash_find(interface_list, root);
   if (!interface) return;

   for (i = 0; signals[i].name; i++)
     e_dbus_interface_signal_add(interface, signals[i].name, signals[i].sig);
}

static void
_mpris_methods_add(const char *root, const MPRIS_Method *methods)
{
   E_DBus_Interface *interface;
   int i;

   if (!conn) return;

   interface = eina_hash_find(interface_list, root);
   if (!interface) return;

   for (i = 0; methods[i].name; i++)
     e_dbus_interface_method_add(interface, methods[i].name, methods[i].param_sig,
                                 methods[i].return_sig, methods[i].callback);
}

static void
_mpris_append_dict_entry(DBusMessageIter *dict_iter, const char *key,
                         int value_type, const void *value)
{
   DBusMessageIter entry_iter, value_iter;
   const char *signature;

   switch (value_type) {
   case DBUS_TYPE_BOOLEAN:
     signature = DBUS_TYPE_BOOLEAN_AS_STRING;
     break;
   case DBUS_TYPE_STRING:
     signature = DBUS_TYPE_STRING_AS_STRING;
     break;
   case DBUS_TYPE_BYTE:
     signature = DBUS_TYPE_BYTE_AS_STRING;
     break;
   case DBUS_TYPE_UINT16:
     signature = DBUS_TYPE_UINT16_AS_STRING;
     break;
   case DBUS_TYPE_UINT32:
     signature = DBUS_TYPE_UINT32_AS_STRING;
     break;
   case DBUS_TYPE_INT16:
     signature = DBUS_TYPE_INT16_AS_STRING;
     break;
   case DBUS_TYPE_INT32:
     signature = DBUS_TYPE_INT32_AS_STRING;
     break;
   case DBUS_TYPE_OBJECT_PATH:
     signature = DBUS_TYPE_OBJECT_PATH_AS_STRING;
     break;
   default:
     signature = DBUS_TYPE_VARIANT_AS_STRING;
   }

   dbus_message_iter_open_container(dict_iter, DBUS_TYPE_DICT_ENTRY, NULL, &entry_iter);

   dbus_message_iter_append_basic(&entry_iter, DBUS_TYPE_STRING, &key);

   dbus_message_iter_open_container(&entry_iter, DBUS_TYPE_VARIANT, signature, &value_iter);
   dbus_message_iter_append_basic(&value_iter, value_type, value);
   dbus_message_iter_close_container(&entry_iter, &value_iter);

   dbus_message_iter_close_container(dict_iter, &entry_iter);
}

static void
_mpris_signal_emit(const char *root, const char *signal_name, int arg_type, void *arg_value)
{
   DBusMessage *sig = dbus_message_new_signal(root, PLAYER_INTERFACE_NAME, signal_name);
   if (arg_type != DBUS_TYPE_INVALID)
     dbus_message_append_args(sig, arg_type, arg_value, DBUS_TYPE_INVALID);
   e_dbus_message_send(conn, sig, NULL, -1, NULL);
   dbus_message_unref(sig);
}

static void
_mpris_message_fill_song_metadata(DBusMessage *msg, const Song *song)
{
   DBusMessageIter iter, dict;

   if (!song) return;

   /*
     Other possible metadata:
     location s		time u
     mtime u		comment s
     rating u		year u
     date u		arturl s
     genre s		mpris:length u
     trackno s
   */

   dbus_message_iter_init_append(msg, &iter);
   dbus_message_iter_open_container(&iter, DBUS_TYPE_ARRAY,
             DBUS_DICT_ENTRY_BEGIN_CHAR_AS_STRING
             DBUS_TYPE_STRING_AS_STRING
             DBUS_TYPE_VARIANT_AS_STRING
             DBUS_DICT_ENTRY_END_CHAR_AS_STRING, &dict);

   if (song->title)
     _mpris_append_dict_entry(&dict, "title", DBUS_TYPE_STRING, &song->title);
   if (song->flags.fetched_album && song->album)
     _mpris_append_dict_entry(&dict, "album", DBUS_TYPE_STRING, &song->album);
   if (song->flags.fetched_artist && song->artist)
     _mpris_append_dict_entry(&dict, "artist", DBUS_TYPE_STRING, &song->artist);
   if (song->flags.fetched_genre && song->genre)
     _mpris_append_dict_entry(&dict, "genre", DBUS_TYPE_STRING, &song->genre);
   _mpris_append_dict_entry(&dict, "rating", DBUS_TYPE_UINT32, &song->rating);
   _mpris_append_dict_entry(&dict, "length", DBUS_TYPE_UINT32, &song->length);
   _mpris_append_dict_entry(&dict, "enjoy:playcount", DBUS_TYPE_INT32, &song->playcnt);
   _mpris_append_dict_entry(&dict, "enjoy:filesize", DBUS_TYPE_INT32, &song->size);

   dbus_message_iter_close_container(&iter, &dict);
}

void
_mpris_signal_player_caps_change(int caps)
{
   static int old_caps = 0;
   if (caps != old_caps)
     {
        _mpris_signal_emit(PLAYER_NAME, "CapsChange", DBUS_TYPE_INT32, &caps);
        old_caps = caps;
     }
}

static void
_mpris_signal_player_status_change(int playback, int shuffle, int repeat, int endless)
{
   DBusMessage *sig;
   DBusMessageIter iter, siter;
   static int old_playback = 0, old_shuffle = 0, old_repeat = 0, old_endless = 0;

   if (old_playback == playback && old_shuffle == shuffle &&
       old_repeat == repeat && old_endless == endless) return;
   old_playback = playback;
   old_shuffle = shuffle;
   old_repeat = repeat;
   old_endless = endless;

   sig = dbus_message_new_signal(PLAYER_NAME, PLAYER_INTERFACE_NAME, "StatusChange");
   if (!sig) return;
   dbus_message_iter_init_append(sig, &iter);
   dbus_message_iter_open_container(&iter, DBUS_TYPE_STRUCT, NULL, &siter);
   dbus_message_iter_append_basic(&siter, DBUS_TYPE_INT32, &playback);
   dbus_message_iter_append_basic(&siter, DBUS_TYPE_INT32, &shuffle);
   dbus_message_iter_append_basic(&siter, DBUS_TYPE_INT32, &repeat);
   dbus_message_iter_append_basic(&siter, DBUS_TYPE_INT32, &endless);
   dbus_message_iter_close_container(&iter, &siter);
   e_dbus_message_send(conn, sig, NULL, -1, NULL);
   dbus_message_unref(sig);
}

static void
_mpris_signal_player_track_change(const Song *song)
{
   static const void *old_song = NULL;
   if (old_song != song)
     {
        DBusMessage *sig = dbus_message_new_signal(PLAYER_NAME, PLAYER_INTERFACE_NAME, "TrackChange");
        if (!sig) return;
        _mpris_message_fill_song_metadata(sig, song);
        e_dbus_message_send(conn, sig, NULL, -1, NULL);
        dbus_message_unref(sig);
        old_song = song;
     }
}

static void
_mpris_signal_tracklist_tracklist_change(int size)
{
   _mpris_signal_emit(TRACKLIST_NAME, "TrackListChange", DBUS_TYPE_INT32, &size);
}

static DBusMessage *
_mpris_player_next(E_DBus_Object *obj __UNUSED__, DBusMessage *msg)
{
   enjoy_control_next();
   return dbus_message_new_method_return(msg);
}

static DBusMessage *
_mpris_player_previous(E_DBus_Object *obj __UNUSED__, DBusMessage *msg)
{
   enjoy_control_previous();
   return dbus_message_new_method_return(msg);
}

static DBusMessage *
_mpris_player_pause(E_DBus_Object *obj __UNUSED__, DBusMessage *msg)
{
   enjoy_control_pause();
   return dbus_message_new_method_return(msg);
}

static DBusMessage *
_mpris_player_stop(E_DBus_Object *obj __UNUSED__, DBusMessage *msg)
{
   enjoy_control_stop();
   return dbus_message_new_method_return(msg);
}

static DBusMessage *
_mpris_player_play(E_DBus_Object *obj __UNUSED__, DBusMessage *msg)
{
   Enjoy_Player_Status status = enjoy_player_status_get();
   if (!status.playback)
     enjoy_position_set(0);
   enjoy_control_play();
   return dbus_message_new_method_return(msg);
}

static DBusMessage *
_mpris_player_seek(E_DBus_Object *obj __UNUSED__, DBusMessage *msg)
{
   dbus_uint64_t position;
   dbus_message_get_args(msg, NULL, DBUS_TYPE_INT64, &position, DBUS_TYPE_INVALID);
   enjoy_control_seek(position);
   return dbus_message_new_method_return(msg);
}

static DBusMessage *
_mpris_root_identity(E_DBus_Object *obj __UNUSED__, DBusMessage *msg)
{
   const char *identity = PACKAGE_STRING;
   DBusMessageIter iter;
   DBusMessage *reply = dbus_message_new_method_return(msg);
   dbus_message_iter_init_append(reply, &iter);
   dbus_message_iter_append_basic(&iter, DBUS_TYPE_STRING, &identity);
   return reply;
}

static DBusMessage *
_mpris_root_quit(E_DBus_Object *obj __UNUSED__, DBusMessage *msg)
{
   enjoy_quit();
   return dbus_message_new_method_return(msg);
}

static DBusMessage *
_mpris_root_version(E_DBus_Object *obj __UNUSED__, DBusMessage *msg)
{
   DBusMessage *reply = dbus_message_new_method_return(msg);
   DBusMessageIter iter, siter;
   dbus_message_iter_init_append(reply, &iter);
   dbus_message_iter_open_container(&iter, DBUS_TYPE_STRUCT, NULL, &siter);

#define APPEND_UINT16(val)                                              \
   do {                                                                 \
      unsigned short _tmp_val = val;                                    \
      dbus_message_iter_append_basic(&siter, DBUS_TYPE_UINT16, &_tmp_val); \
   } while (0)
   APPEND_UINT16(1);
   APPEND_UINT16(0);
#undef APPEND_UINT16

   dbus_message_iter_close_container(&iter, &siter);
   return reply;
}

static DBusMessage *
_mpris_player_caps_get(E_DBus_Object *obj __UNUSED__, DBusMessage *msg)
{
   DBusMessage *reply = dbus_message_new_method_return(msg);
   DBusMessageIter iter;
   int bits = _caps_to_mpris_bits(enjoy_player_caps_get());
   dbus_message_iter_init_append(reply, &iter);
   dbus_message_iter_append_basic(&iter, DBUS_TYPE_INT32, &bits);
   return reply;
}

static DBusMessage *
_mpris_player_volume_set(E_DBus_Object *obj __UNUSED__, DBusMessage *msg)
{
   dbus_int32_t volume;

   dbus_message_get_args(msg, NULL, DBUS_TYPE_INT32, &volume, DBUS_TYPE_INVALID);
   if (volume > 100)
     volume = 100;
   else if (volume < 0)
     volume = 0;
   enjoy_volume_set(volume);

   return dbus_message_new_method_return(msg);
}

static DBusMessage *
_mpris_player_volume_get(E_DBus_Object *obj __UNUSED__, DBusMessage *msg)
{
   DBusMessage *reply = dbus_message_new_method_return(msg);
   DBusMessageIter iter;
   int vol = enjoy_volume_get();
   dbus_message_iter_init_append(reply, &iter);
   dbus_message_iter_append_basic(&iter, DBUS_TYPE_INT32, &vol);
   return reply;
}

static DBusMessage *
_mpris_player_repeat_set(E_DBus_Object *obj __UNUSED__, DBusMessage *msg)
{
   dbus_bool_t repeat;
   dbus_message_get_args(msg, NULL, DBUS_TYPE_BOOLEAN, &repeat, DBUS_TYPE_INVALID);
   enjoy_control_loop_set(repeat);
   return dbus_message_new_method_return(msg);
}

static DBusMessage *
_mpris_player_status_get(E_DBus_Object *obj __UNUSED__, DBusMessage *msg)
{
   DBusMessage *reply = dbus_message_new_method_return(msg);
   DBusMessageIter iter, siter;
   Enjoy_Player_Status status = enjoy_player_status_get();

   dbus_message_iter_init_append(reply, &iter);
   dbus_message_iter_open_container(&iter, DBUS_TYPE_STRUCT, NULL, &siter);

#define APPEND_INT(member)                                              \
   do {                                                                 \
      int _tmp_val = status.member;                                     \
      dbus_message_iter_append_basic(&siter, DBUS_TYPE_INT32, &(_tmp_val)); \
   } while (0)
   APPEND_INT(playback);
   APPEND_INT(shuffle);
   APPEND_INT(repeat);
   APPEND_INT(endless);
#undef APPEND_INT

   dbus_message_iter_close_container(&iter, &siter);
   return reply;
}

static DBusMessage *
_mpris_player_position_set(E_DBus_Object *obj __UNUSED__, DBusMessage *msg)
{
   dbus_int32_t position;
   dbus_message_get_args(msg, NULL, DBUS_TYPE_INT32, &position, DBUS_TYPE_INVALID);
   enjoy_position_set(position);
   return dbus_message_new_method_return(msg);
}

static DBusMessage *
_mpris_player_position_get(E_DBus_Object *obj __UNUSED__, DBusMessage *msg)
{
   DBusMessage *reply = dbus_message_new_method_return(msg);
   DBusMessageIter iter;
   int pos = enjoy_position_get();
   dbus_message_iter_init_append(reply, &iter);
   dbus_message_iter_append_basic(&iter, DBUS_TYPE_INT32, &pos);
   return reply;
}

static DBusMessage *
_mpris_song_metadata_reply(DBusMessage *msg, const Song *song)
{
   DBusMessage *reply = dbus_message_new_method_return(msg);
   _mpris_message_fill_song_metadata(reply, song);
   return reply;
}

static DBusMessage *
_mpris_player_metadata_get(E_DBus_Object *obj __UNUSED__, DBusMessage *msg)
{
   return _mpris_song_metadata_reply(msg, enjoy_song_current_get());
}

static DBusMessage *
_mpris_tracklist_current_track_get(E_DBus_Object *obj __UNUSED__, DBusMessage *msg)
{
   DBusMessage *reply = dbus_message_new_method_return(msg);
   DBusMessageIter iter;
   int pos = enjoy_playlist_current_position_get();
   dbus_message_iter_init_append(reply, &iter);
   dbus_message_iter_append_basic(&iter, DBUS_TYPE_INT32, &pos);
   return reply;
}

static DBusMessage *
_mpris_tracklist_count(E_DBus_Object *obj __UNUSED__, DBusMessage *msg)
{
   DBusMessage *reply = dbus_message_new_method_return(msg);
   DBusMessageIter iter;
   int count = enjoy_playlist_count();
   dbus_message_iter_init_append(reply, &iter);
   dbus_message_iter_append_basic(&iter, DBUS_TYPE_INT32, &count);
   return reply;
}

static DBusMessage *
_mpris_tracklist_metadata_get(E_DBus_Object *obj __UNUSED__, DBusMessage *msg)
{
   DBusMessage *reply;
   const Song *song;
   dbus_int32_t position;
   dbus_message_get_args(msg, NULL, DBUS_TYPE_INT32, &position, DBUS_TYPE_INVALID);
   song = enjoy_playlist_song_position_get(position);
   reply = _mpris_song_metadata_reply(msg, song);
   return reply;
}

static DBusMessage *
_mpris_tracklist_shuffle_set(E_DBus_Object *obj __UNUSED__, DBusMessage *msg)
{
   DBusMessage *reply = dbus_message_new_method_return(msg);
   dbus_bool_t param;
   dbus_message_get_args(msg, NULL, DBUS_TYPE_BOOLEAN, &param, DBUS_TYPE_INVALID);
   enjoy_control_shuffle_set(param);
   return reply;
}

EINA_MODULE_INIT(mpris_init);
EINA_MODULE_SHUTDOWN(mpris_shutdown);
