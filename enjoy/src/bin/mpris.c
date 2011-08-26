#include "private.h"
#include "mpris.h"

typedef struct _MPRIS_Method MPRIS_Method;
typedef struct _MPRIS_Signal MPRIS_Signal;


#define APPLICATION_NAME "org.mpris.enjoy"
#define PLAYER_INTERFACE_NAME "org.freedesktop.MediaPlayer"
#define ROOT_NAME "/Root" /* should really be "/", but this doesn't work correctly :( */
#define TRACKLIST_NAME "/TrackList"
#define PLAYER_NAME "/Player"


static void _mpris_signals_add(const char *root, const MPRIS_Signal *signals);
static void _mpris_methods_add(const char *root, const MPRIS_Method *methods);
static void _mpris_append_dict_entry(DBusMessageIter *dict_iter, const char *key,
                                     int value_type, void *value);
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
static DBusMessage *_mpris_song_metadata_reply(DBusMessage *msg, Song *song);
static DBusMessage *_mpris_player_metadata_get(E_DBus_Object *obj __UNUSED__, DBusMessage *msg);
static DBusMessage *_mpris_tracklist_current_track_get(E_DBus_Object *obj __UNUSED__, DBusMessage *msg);
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


E_DBus_Connection *conn = NULL;
Eina_Hash *interface_list = NULL;


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
  { "GetLength",       "",    "i",     NULL },
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



void
mpris_init(void)
{
   if (conn) return;
   e_dbus_init();
   conn = e_dbus_bus_get(DBUS_BUS_SESSION);
   if (conn)
     e_dbus_request_name(conn, APPLICATION_NAME, 0, _cb_dbus_request_name, NULL);
}

void
mpris_shutdown(void)
{
   if (!conn) return;
   eina_hash_free(interface_list);
   e_dbus_shutdown();
   conn = NULL;
   interface_list = NULL;
}

static void
_cb_dbus_request_name(void *data __UNUSED__, DBusMessage *msg, DBusError *err)
{
   DBusError new_err;
   dbus_uint32_t msgtype;
  
   if (dbus_error_is_set(err))
     {
        dbus_error_free(err);
        return;
     }
  
   dbus_error_init(&new_err);
   dbus_message_get_args(msg, &new_err, DBUS_TYPE_UINT32, &msgtype, DBUS_TYPE_INVALID);
  
   if (msgtype == DBUS_REQUEST_NAME_REPLY_PRIMARY_OWNER ||
       msgtype == DBUS_REQUEST_NAME_REPLY_ALREADY_OWNER)
     {
        int i;

        interface_list = eina_hash_string_small_new((Eina_Free_Cb)e_dbus_interface_unref);
       
        for (i = 0; object_list[i]; i++)
          {
             E_DBus_Object *object = e_dbus_object_add(conn, object_list[i], NULL);
             E_DBus_Interface *interface = e_dbus_interface_new(PLAYER_INTERFACE_NAME);
             e_dbus_object_interface_attach(object, interface);
            
             eina_hash_add(interface_list, object_list[i], interface);
          }

        _mpris_signals_add(PLAYER_NAME, mpris_player_signals);
        _mpris_signals_add(TRACKLIST_NAME, mpris_tracklist_signals);

        _mpris_methods_add(ROOT_NAME, mpris_root_methods);
        _mpris_methods_add(PLAYER_NAME, mpris_player_methods);
        _mpris_methods_add(TRACKLIST_NAME, mpris_tracklist_methods);
     }
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
                         int value_type, void *value)
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
_mpris_message_fill_song_metadata(DBusMessage *msg, Song *song)
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
mpris_signal_player_caps_change(int caps)
{
   static int old_caps = 0;
   if (caps != old_caps)
     {
        _mpris_signal_emit(PLAYER_NAME, "CapsChange", DBUS_TYPE_INT32, &caps);
        old_caps = caps;
     }
}

void
mpris_signal_player_status_change(int playback, int shuffle, int repeat, int endless)
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
   dbus_message_iter_append_basic(&siter, DBUS_TYPE_UINT32, &playback);
   dbus_message_iter_append_basic(&siter, DBUS_TYPE_UINT32, &shuffle);
   dbus_message_iter_append_basic(&siter, DBUS_TYPE_UINT32, &repeat);
   dbus_message_iter_append_basic(&siter, DBUS_TYPE_UINT32, &endless);
   dbus_message_iter_close_container(&iter, &siter);
   e_dbus_message_send(conn, sig, NULL, -1, NULL);
   dbus_message_unref(sig);
}

void
mpris_signal_player_track_change(Song *song)
{
   static Song *old_song = NULL;
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

void
mpris_signal_tracklist_tracklist_change(int size)
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
   int playback;
   enjoy_status_get(&playback, NULL, NULL, NULL);
   if (!playback)
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
   dbus_message_iter_append_basic(&siter, DBUS_TYPE_UINT16, (int[]) { 1 });
   dbus_message_iter_append_basic(&siter, DBUS_TYPE_UINT16, (int[]) { 0 });
   dbus_message_iter_close_container(&iter, &siter);
   return reply;
}

static DBusMessage *
_mpris_player_caps_get(E_DBus_Object *obj __UNUSED__, DBusMessage *msg)
{
   DBusMessage *reply = dbus_message_new_method_return(msg);
   DBusMessageIter iter;
   dbus_message_iter_init_append(reply, &iter);
   dbus_message_iter_append_basic(&iter, DBUS_TYPE_INT32, (int[]) { enjoy_caps_get() });
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
   dbus_message_iter_init_append(reply, &iter);
   dbus_message_iter_append_basic(&iter, DBUS_TYPE_INT32, (int[]) { enjoy_volume_get() });
   return reply;
}

static DBusMessage *
_mpris_player_repeat_set(E_DBus_Object *obj __UNUSED__, DBusMessage *msg)
{
   dbus_bool_t repeat;
   dbus_message_get_args(msg, NULL, DBUS_TYPE_UINT32, &repeat, DBUS_TYPE_INVALID);
   enjoy_repeat_set(repeat);
   return dbus_message_new_method_return(msg);
}

static DBusMessage *
_mpris_player_status_get(E_DBus_Object *obj __UNUSED__, DBusMessage *msg)
{
   DBusMessage *reply = dbus_message_new_method_return(msg);
   DBusMessageIter iter, siter;
   int playback, shuffle, repeat, endless;

   enjoy_status_get(&playback, &shuffle, &repeat, &endless);

   dbus_message_iter_init_append(reply, &iter);
   dbus_message_iter_open_container(&iter, DBUS_TYPE_STRUCT, NULL, &siter);
   dbus_message_iter_append_basic(&siter, DBUS_TYPE_UINT32, &playback);
   dbus_message_iter_append_basic(&siter, DBUS_TYPE_UINT32, &shuffle);
   dbus_message_iter_append_basic(&siter, DBUS_TYPE_UINT32, &repeat);
   dbus_message_iter_append_basic(&siter, DBUS_TYPE_UINT32, &endless);
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
   dbus_message_iter_init_append(reply, &iter);
   dbus_message_iter_append_basic(&iter, DBUS_TYPE_INT32, (int[]) { enjoy_position_get() });
   return reply;
}

static DBusMessage *
_mpris_song_metadata_reply(DBusMessage *msg, Song *song)
{
   DBusMessage *reply = dbus_message_new_method_return(msg);
   _mpris_message_fill_song_metadata(reply, song);
   return reply;
}

static DBusMessage *
_mpris_player_metadata_get(E_DBus_Object *obj __UNUSED__, DBusMessage *msg)
{
   Song *song = enjoy_song_current_get();
   return _mpris_song_metadata_reply(msg, song);
}

static DBusMessage *
_mpris_tracklist_current_track_get(E_DBus_Object *obj __UNUSED__, DBusMessage *msg)
{
   DBusMessage *reply = dbus_message_new_method_return(msg);
   DBusMessageIter iter;
   dbus_message_iter_init_append(reply, &iter);
   dbus_message_iter_append_basic(&iter, DBUS_TYPE_INT32, (int[]) { enjoy_playlist_current_position_get() });
   return reply;
}

static DBusMessage *
_mpris_tracklist_metadata_get(E_DBus_Object *obj __UNUSED__, DBusMessage *msg)
{
   DBusMessage *reply;
   Song *song;
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
