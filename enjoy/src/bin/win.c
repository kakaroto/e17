#include "private.h"
#include <Emotion.h>

typedef struct Win
{
   Evas_Object *win;
   Evas_Object *bg;
   Evas_Object *layout;
   Evas_Object *toolbar;
   Evas_Object *edje;
   Evas_Object *emotion;
   Evas_Object *list;
   Evas_Object *nowplaying;
   Evas_Object *nowplaying_pager;
   const char *db_path;
   DB *db;
   Libmgr *mgr;
   Song *song;
   struct {
        Elm_Toolbar_Item *play;
        Elm_Toolbar_Item_State *pause;
        Elm_Toolbar_Item *next;
        Elm_Toolbar_Item *prev;
        Elm_Toolbar_Item *nowplaying;
        Elm_Toolbar_Item_State *playlist;
   } action;
   struct {
      double position, length;
      double volume;
      Eina_Bool mute:1;
      Eina_Bool playing:1;
      Eina_Bool playing_last:1;
      Eina_Bool repeat:1;
      Eina_Bool shuffle:1;
   } play;
   struct {
      Evas_Coord w, h;
   } min;
   struct {
      Eina_List *add, *del;
   } scan;
   struct {
      Ecore_Timer *play_eval;
   } timer;
   struct {
      Ecore_Job *scan;
      Ecore_Job *populate;
   } job;
   struct {
      Ecore_Thread *scan;
   } thread;
} Win;

static Win _win;

static void
_win_populate_job(void *data)
{
   Win *w = data;
   w->job.populate = NULL;

   if (w->db) db_close(w->db);
   w->db = db_open(w->db_path);
   if (!w->db)
     {
        CRITICAL("no database at %s!", w->db_path);
        // TODO: remove me! create library manager and start it from here
        printf("SCAN and LIBRARY MANAGER are not implemeted yet!\n"
               "Meanwhile please run "
               "(./test = binary from lightmediascanner/src/bin):\n"
               "   ./test -p id3 -i 5000 -s %s/Music %s\n"
               "sorry about the inconvenience!\n",
               getenv("HOME"), w->db_path);
        exit(-1);
     }
   list_populate(w->list, w->db);
}

static void
_win_scan_job_finished(void *data, Eina_Bool success __UNUSED__)
{
   Win *w = data;
   list_thaw(w->list);
   w->job.populate = ecore_job_add(_win_populate_job, w);
}

static void
_win_scan_job(void *data)
{
   Win *w = data;
   w->job.scan = NULL;

   Eina_List *l;
   const char *path;
   EINA_LIST_FOREACH(w->scan.add, l, path)
       libmgr_scanpath_add(w->mgr, path);
   EINA_LIST_FOREACH(w->scan.del, l, path)
     printf("   sqlite3 %s \"delete from files where path like '%s%%'\"\n",
            w->db_path, path);

   if (w->job.populate)
     {
        ecore_job_del(w->job.populate);
        w->job.populate = NULL;
     }
   list_freeze(w->list);

   libmgr_scan_start(w->mgr, _win_scan_job_finished, w);

   // TODO
   // emit delete as sqlite statements
   // finish thread -> unmark it from Win
   // notify win (should reload lists)

   // if (!w->job.populate)
   //   w->job.populate = ecore_job_add(_win_populate_job, w);
}

static void
_win_toolbar_eval(Win *w)
{

   if ((w->play.shuffle) || (list_prev_exists(w->list)))
      elm_toolbar_item_disabled_set(w->action.prev, EINA_FALSE);
   else
      elm_toolbar_item_disabled_set(w->action.prev, EINA_TRUE);

   if ((w->play.shuffle) || (list_next_exists(w->list)))
      elm_toolbar_item_disabled_set(w->action.next, EINA_FALSE);
   else
      elm_toolbar_item_disabled_set(w->action.next, EINA_TRUE);

   if (w->song)
     {
        elm_toolbar_item_disabled_set(w->action.play, EINA_FALSE);
        elm_toolbar_item_disabled_set(w->action.nowplaying, EINA_FALSE);
     }
   else
     {
        elm_toolbar_item_disabled_set(w->action.play, EINA_TRUE);
        elm_toolbar_item_disabled_set(w->action.nowplaying, EINA_TRUE);
     }

   ecore_event_add(ENJOY_EVENT_PLAYER_CAPS_CHANGE, NULL, NULL, NULL);
}

static void
_win_play_pause_toggle(Win *w)
{
   ecore_event_add(ENJOY_EVENT_PLAYER_STATUS_CHANGE, NULL, NULL, NULL);
   ecore_event_add(ENJOY_EVENT_PLAYER_CAPS_CHANGE, NULL, NULL, NULL);

   if (w->play.playing)
      elm_toolbar_item_state_set(w->action.play, w->action.pause);
   else
      elm_toolbar_item_state_unset(w->action.play);
}
static void
_win_play_eval(Win *w)
{
   Edje_Message_Float_Set *mf;

   w->play.position = emotion_object_position_get(w->emotion);
   w->play.length = emotion_object_play_length_get(w->emotion);

   if ((w->song) && (w->song->length != (int)w->play.length))
      db_song_length_set(w->db, w->song, w->play.length);

   mf = alloca(sizeof(Edje_Message_Float_Set) + sizeof(double));
   mf->count = 2;
   mf->val[0] = w->play.position;
   mf->val[1] = w->play.length;
   edje_object_message_send(elm_layout_edje_get(w->nowplaying), EDJE_MESSAGE_FLOAT_SET, MSG_POSITION, mf);

   if (w->play.playing_last == w->play.playing) return;
   w->play.playing_last = !w->play.playing;
   _win_play_pause_toggle(w);

   ecore_event_add(ENJOY_EVENT_PLAYER_CAPS_CHANGE, NULL, NULL, NULL);
}

static Eina_Bool
_win_play_eval_timer(void *data)
{
   _win_play_eval(data);
   return EINA_TRUE;
}

static void
_win_nowplaying_update(Win *w)
{
   Evas_Object *cover;
   int label_size;
   char *artist_title;
   const char *s1, *s2;
   cover = cover_album_fetch_by_id(w->win, w->db,w->song->album_id, 480, NULL, NULL); // TODO: size!
   elm_layout_content_set(w->nowplaying, "ejy.swallow.cover", cover);

   db_song_artist_fetch(w->db, w->song);
   s1 = w->song->title;
   s2 = w->song->artist;
   if (!s1) s1 = "";
   if (!s2) s2 = "";
   label_size = strlen(s1) + strlen(s2) + 4;
   artist_title = malloc(label_size);
   if (!artist_title)
      return;

   if (snprintf(artist_title, label_size, "%s - %s", s1, s2) >= label_size)
     {
        CRITICAL("could not set nowplaying title");
        goto nowplaying_error;
     }
   edje_object_part_text_set(elm_layout_edje_get(w->nowplaying), "ejy.text.title", eina_stringshare_add(artist_title));

nowplaying_error:
   free(artist_title);
}

static void
_win_song_set(Win *w, Song *s)
{
   Edje_Message_Int mi;
   char str[32];

   w->play.position = 0.0;
   w->play.length = 0.0;
   w->song = s;
   if (!s) goto end;

   if (s->trackno > 0)
     snprintf(str, sizeof(str), "%d", s->trackno);
   else
     str[0] = '\0';

   edje_object_part_text_set(w->edje, "ejy.text.trackno", str);
   edje_object_part_text_set(w->edje, "ejy.text.title", s->title);
   edje_object_part_text_set(w->edje, "ejy.text.album", s->album);
   edje_object_part_text_set(w->edje, "ejy.text.artist", s->artist);
   edje_object_part_text_set(w->edje, "ejy.text.genre", s->genre);

   mi.val = s->rating;
   edje_object_message_send(elm_layout_edje_get(w->nowplaying), EDJE_MESSAGE_INT, MSG_RATING, &mi);

   emotion_object_file_set(w->emotion, s->path);
   emotion_object_position_set(w->emotion, w->play.position);
   w->play.playing = EINA_TRUE;
   w->play.playing_last = EINA_FALSE;
   emotion_object_play_set(w->emotion, EINA_TRUE);
   emotion_object_audio_volume_set(w->emotion, w->play.volume);

end:
   if ((!w->play.playing) && (w->timer.play_eval))
     {
        ecore_timer_del(w->timer.play_eval);
        w->timer.play_eval = NULL;
     }
   else if ((w->play.playing) && (!w->timer.play_eval))
     w->timer.play_eval = ecore_timer_loop_add
       (0.1, _win_play_eval_timer, w);

   _win_nowplaying_update(w);
   _win_play_eval(w);
   _win_toolbar_eval(w);

   ecore_event_add(ENJOY_EVENT_PLAYER_CAPS_CHANGE, NULL, NULL, NULL);
   ecore_event_add(ENJOY_EVENT_PLAYER_TRACK_CHANGE, NULL, NULL, NULL);
}

static void
_win_play_pos_update(void *data, Evas_Object *o __UNUSED__, void *event_info __UNUSED__)
{
   Win *w = data;
   _win_play_eval(w);
}

static void
_win_play_begin(void *data, Evas_Object *o __UNUSED__, void *event_info __UNUSED__)
{
   Win *w = data;
   _win_play_eval(w);
}

static void
_win_list_selected(void *data, Evas_Object *list __UNUSED__, void *event_info)
{
   Win *w = data;
   Song *s = event_info;
   _win_song_set(w, s);
}

static void
_win_del(void *data, Evas *e __UNUSED__, Evas_Object *o __UNUSED__, void *event_info __UNUSED__)
{
   Win *w = data;
   if (w->emotion) evas_object_del(w->emotion);
   if (w->job.scan) ecore_job_del(w->job.scan);
   if (w->job.populate) ecore_job_del(w->job.populate);
   if (w->timer.play_eval) ecore_timer_del(w->timer.play_eval);
   if (w->thread.scan) ecore_thread_cancel(w->thread.scan);
   if (w->db_path) eina_stringshare_del(w->db_path);
}

static void
_win_del_request(void *data __UNUSED__, Evas_Object *o, void *event_info __UNUSED__)
{
   evas_object_hide(o);
   enjoy_quit();
}

static void
_win_prev(void *data, Evas_Object *obj __UNUSED__, void *event_info __UNUSED__)
{
   Win *w = data;
   Song *s;
   if (w->play.shuffle)
      s = list_shuffle_prev_go(w->list);
   else
      s = list_prev_go(w->list);
   INF("prev song=%p (%s)", s, s ? s->path : NULL);
   if (s) _win_song_set(w, s);
}

static void
_win_next(void *data, Evas_Object *obj __UNUSED__, void *event_info __UNUSED__)
{
   Win *w = data;
   Song *s;
   if (w->play.shuffle)
      s = list_shuffle_next_go(w->list);
   else
      s = list_next_go(w->list);
   INF("next song=%p (%s)", s, s ? s->path : NULL);
   if (s) _win_song_set(w, s);
}

static void
_win_play_end(void *data, Evas_Object *obj __UNUSED__, void *event_info __UNUSED__)
{
   Win *w = data;
   Song *s;

   if (w->play.repeat)
     {
        s = w->song;
        _win_song_set(w, s);
     }
   else
     _win_next(data, NULL, NULL);
}

static void
_win_action_play(void *data, Evas_Object *obj __UNUSED__, void *event_info __UNUSED__)
{
   Win *w = data;
   INF("play song=%p (%s)", w->song, w->song ? w->song->path : NULL);
   w->play.playing = EINA_TRUE;
   emotion_object_play_set(w->emotion, EINA_TRUE);
   _win_play_pause_toggle(w);
   _win_play_eval(w);

   ecore_event_add(ENJOY_EVENT_PLAYER_CAPS_CHANGE, NULL, NULL, NULL);
}

static void
_win_action_pause(void *data, Evas_Object *obj __UNUSED__, void *event_info __UNUSED__)
{
   Win *w = data;
   INF("pause song=%p (%s)", w->song, w->song ? w->song->path : NULL);
   w->play.playing = EINA_FALSE;
   emotion_object_play_set(w->emotion, EINA_FALSE);
   _win_play_pause_toggle(w);
   _win_play_eval(w);

   ecore_event_add(ENJOY_EVENT_PLAYER_CAPS_CHANGE, NULL, NULL, NULL);
}

static void
_win_mode_list(void *data, Evas_Object *obj __UNUSED__, void *event_info __UNUSED__)
{
   Win *w = data;
   elm_toolbar_item_state_unset(w->action.nowplaying);
   list_promote_current(w->list);
   edje_object_signal_emit(w->edje, "elm,title,show", "elm");
}

static void
_win_mode_nowplaying(void *data, Evas_Object *obj __UNUSED__, void *event_info __UNUSED__)
{
   Win *w = data;
   edje_object_signal_emit(w->edje, "elm,title,hide", "elm");
   elm_toolbar_item_state_set(w->action.nowplaying, w->action.playlist);
   elm_pager_content_promote(w->list, w->nowplaying);
}

static void
_win_repeat_on(void *data __UNUSED__, Evas_Object *o __UNUSED__, const char *emission __UNUSED__, const char *source __UNUSED__)
{
   Win *w = data;
   w->play.repeat = EINA_TRUE;
}

static void
_win_repeat_off(void *data __UNUSED__, Evas_Object *o __UNUSED__, const char *emission __UNUSED__, const char *source __UNUSED__)
{
   Win *w = data;
   w->play.repeat = EINA_FALSE;
}

static void
_win_shuffle_on(void *data __UNUSED__, Evas_Object *o __UNUSED__, const char *emission __UNUSED__, const char *source __UNUSED__)
{
   Win *w = data;
   w->play.shuffle = EINA_TRUE;
   list_shuffle_reset(w->list);
   _win_toolbar_eval(w);
}

static void
_win_shuffle_off(void *data __UNUSED__, Evas_Object *o __UNUSED__, const char *emission __UNUSED__, const char *source __UNUSED__)
{
   Win *w = data;
   w->play.shuffle = EINA_FALSE;
   _win_toolbar_eval(w);
}

//#define EDJE_SIGNAL_DEBUG 1
#ifdef EDJE_SIGNAL_DEBUG
static void
_edje_signal_debug(void *data __UNUSED__, Evas_Object *o __UNUSED__, const char *emission, const char *source)
{
   DBG("emission=%s, source=%s", emission, source);
}
#endif

static void
_win_event_song_done(void *data __UNUSED__, void *song)
{
   db_song_free(song);
}

static void
_win_event_song(int event, Song *song)
{
   ecore_event_add(event, db_song_copy(song), _win_event_song_done, NULL);
}

static void
_win_edje_msg(void *data, Evas_Object *o __UNUSED__, Edje_Message_Type type, int id, void *msg)
{
   Win *w = data;

   switch (id)
     {
      case MSG_VOLUME:
         if (type != EDJE_MESSAGE_FLOAT)
           ERR("message for volume got type %d instead of %d",
               type, EDJE_MESSAGE_FLOAT);
         else
           {
              Edje_Message_Float *m = msg;
              w->play.volume = m->val;
              emotion_object_audio_volume_set(w->emotion, w->play.volume);
              w->play.mute = EINA_FALSE;
              emotion_object_audio_mute_set(w->emotion, w->play.mute);
              ecore_event_add(ENJOY_EVENT_VOLUME_CHANGE, NULL, NULL, NULL);
           }
        break;

      case MSG_MUTE:
         if (type != EDJE_MESSAGE_INT)
           ERR("message for volume got type %d instead of %d",
               type, EDJE_MESSAGE_INT);
         else
           {
              Edje_Message_Int *m = msg;
              w->play.mute = m->val;
              emotion_object_audio_mute_set(w->emotion, w->play.mute);
              if (w->play.mute)
                 emotion_object_audio_volume_set(w->emotion, 0);
              else
                 emotion_object_audio_volume_set(w->emotion, w->play.volume);
              ecore_event_add(ENJOY_EVENT_VOLUME_CHANGE, NULL, NULL, NULL);
           }
        break;

      case MSG_POSITION:
         if (type != EDJE_MESSAGE_FLOAT)
           ERR("message for position/seek got type %d instead of %d",
               type, EDJE_MESSAGE_FLOAT);
         else
           {
              Edje_Message_Float *m = msg;
              w->play.position = m->val;
              emotion_object_position_set(w->emotion, w->play.position);
              ecore_event_add(ENJOY_EVENT_POSITION_CHANGE, NULL, NULL, NULL);
           }
        break;

      case MSG_RATING:
         if (type != EDJE_MESSAGE_INT)
           ERR("message for rating got type %d instead of %d",
               type, EDJE_MESSAGE_INT);
         else
           {
              Edje_Message_Int *m = msg;
              if (!w->song)
                ERR("setting rating without song?");
              else
                {
                   db_song_rating_set(w->db, w->song, m->val);
                   _win_event_song(ENJOY_EVENT_RATING_CHANGE, w->song);
                }
           }
        break;

      default:
         ERR("unknown edje message id: %d of type: %d", id, type);
     }
}

EAPI void
enjoy_control_next(void)
{
   Win *w = &_win;
   _win_next(w, NULL, NULL);
}

EAPI void
enjoy_control_previous(void)
{
   Win *w = &_win;
   _win_prev(w, NULL, NULL);
}

EAPI void
enjoy_control_pause(void)
{
   Win *w = &_win;
   _win_action_pause(w, NULL, NULL);
}

EAPI void
enjoy_control_stop(void)
{
   Win *w = &_win;
   _win_action_pause(&_win, NULL, NULL);
   w->play.position = 0.0;
   emotion_object_position_set(w->emotion, w->play.position);
}

EAPI void
enjoy_control_play(void)
{
   Win *w = &_win;
   _win_action_play(w, NULL, NULL);
}

EAPI void
enjoy_control_seek(uint64_t position)
{
   Win *w = &_win;
   double seek_to;

   seek_to = w->play.position + w->play.length / ((double)position / 1e6);
   if (seek_to <= 0.0)
     seek_to = 0.0;
   else if (seek_to >= 1.0)
     seek_to = 1.0;

   w->play.position = seek_to;
   emotion_object_position_set(w->emotion, w->play.position);
   ecore_event_add(ENJOY_EVENT_POSITION_CHANGE, NULL, NULL, NULL);
}

EAPI Enjoy_Player_Caps
enjoy_player_caps_get(void)
{
   Win *w = &_win;
   Enjoy_Player_Caps caps;

   memset(&caps, 0, sizeof(caps));

   caps.can_go_prev = !!list_prev_exists(w->list);
   caps.can_go_next = !!((w->play.shuffle) || (list_next_exists(w->list)));
   if (w->song)
     {
        caps.can_pause = EINA_TRUE;
        caps.can_play = EINA_TRUE;
        caps.can_seek = emotion_object_seekable_get(w->emotion);
        caps.can_provide_metadata = EINA_TRUE;
        caps.has_tracklist = EINA_TRUE;
     }

   return caps;
}

EAPI Enjoy_Player_Status
enjoy_player_status_get(void)
{
  Win *w = &_win;
  Enjoy_Player_Status status;

  memset(&status, 0, sizeof(status));

  if (w->play.playing)
    status.playback = ENJOY_PLAYBACK_PLAYING;
  else if (w->play.position == 0.0)
    status.playback = ENJOY_PLAYBACK_STOPPED;
  else
    status.playback = ENJOY_PLAYBACK_PAUSED;

  status.shuffle = !!w->play.shuffle;
  status.repeat = !!w->play.repeat;
  status.endless = EINA_FALSE;

  return status;
}

EAPI void
enjoy_repeat_set(Eina_Bool repeat)
{
  Win *w = &_win;
  w->play.repeat = !!repeat;
}

EAPI Eina_Bool
enjoy_repeat_get(void)
{
  Win *w = &_win;
  return w->play.repeat;
}

EAPI void
enjoy_position_set(int32_t position)
{
   Win *w = &_win;

   w->play.position = w->play.length / ((double)position / 1e6);
   if (w->play.position < 0.0)
     w->play.position = 0.0;
   else if (w->play.position > 1.0)
     w->play.position = 1.0;

   emotion_object_position_set(w->emotion, w->play.position);
   ecore_event_add(ENJOY_EVENT_POSITION_CHANGE, NULL, NULL, NULL);
}

EAPI int32_t
enjoy_position_get(void)
{
   Win *w = &_win;
   return w->play.position * w->play.length;
}

EAPI int32_t
enjoy_volume_get(void)
{
   Win *w = &_win;
   return w->play.volume * 100;
}

EAPI Eina_Bool
enjoy_mute_get(void)
{
   Win *w = &_win;
   return w->play.mute;
}

EAPI void
enjoy_volume_set(int32_t volume)
{
   Win *w = &_win;
   w->play.volume = (double)volume / 100.0;
   emotion_object_audio_volume_set(w->emotion, w->play.volume);
   w->play.mute = EINA_FALSE;
   emotion_object_audio_mute_set(w->emotion, w->play.mute);
   ecore_event_add(ENJOY_EVENT_VOLUME_CHANGE, NULL, NULL, NULL);
   edje_object_message_send(elm_layout_edje_get(w->nowplaying),
                            EDJE_MESSAGE_FLOAT, MSG_VOLUME, &(w->play.volume));
}

EAPI void
enjoy_mute_set(Eina_Bool mute)
{
   Win *w = &_win;
   int val = !!mute;
   w->play.mute = !!mute;
   emotion_object_audio_mute_set(w->emotion, w->play.mute);
   if (w->play.mute)
     emotion_object_audio_volume_set(w->emotion, 0);
   else
     emotion_object_audio_volume_set(w->emotion, w->play.volume);
   ecore_event_add(ENJOY_EVENT_VOLUME_CHANGE, NULL, NULL, NULL);
   edje_object_message_send(elm_layout_edje_get(w->nowplaying),
                            EDJE_MESSAGE_INT, MSG_MUTE, &val);
}

EAPI const Song *
enjoy_song_current_get(void)
{
   Win *w = &_win;
   return w->song;
}

EAPI const Song *
enjoy_song_position_get(int32_t position)
{
   Win *w = &_win;
   return db_song_get(w->db, position);
}

EAPI void
enjoy_control_loop_set(Eina_Bool param)
{
   Win *w = &_win;
   int val = !!param;
   w->play.repeat = !!param;
   edje_object_message_send(elm_layout_edje_get(w->nowplaying),
                            EDJE_MESSAGE_INT, MSG_LOOP, &val);
   ecore_event_add(ENJOY_EVENT_PLAYER_CAPS_CHANGE, NULL, NULL, NULL);
}

EAPI void
enjoy_control_shuffle_set(Eina_Bool param)
{
   Win *w = &_win;
   int val = !!param;
   w->play.shuffle = !!param;
   edje_object_message_send(elm_layout_edje_get(w->nowplaying),
                            EDJE_MESSAGE_INT, MSG_SHUFFLE, &val);
   if (val) list_shuffle_reset(w->list);
   _win_toolbar_eval(w);
}

EAPI int32_t
enjoy_playlist_current_position_get(void)
{
   Win *w = &_win;
   if (!w->list) return 0;
   return list_song_selected_n_get(w->list);
}

EAPI int32_t
enjoy_playlist_count(void)
{
   Win *w = &_win;
   if (!w->list) return 0;
   return list_song_count(w->list);
}

EAPI const Song *
enjoy_playlist_song_position_get(int32_t position)
{
   Win *w = &_win;
   if (!w->list) return NULL;
   return list_song_nth_get(w->list, position);
}

static Elm_Toolbar_Item *
_toolbar_item_add(Win *w, const char *icon, const char *label, int priority, Evas_Smart_Cb cb)
{
   Elm_Toolbar_Item *item = elm_toolbar_item_append(w->toolbar, icon, label,
                                                    cb, w);
   elm_toolbar_item_priority_set(item, priority);
   return item;
}

Evas_Object *
win_new(App *app)
{
   Win *w = &_win;
   const char *s;
   Evas_Coord iw = 320, ih = 240;
   char path[PATH_MAX];
   Evas_Object *nowplaying_edje;

   memset(w, 0, sizeof(*w));

   w->play.volume = 0.8;

   w->win = elm_win_add(NULL, PACKAGE_NAME, ELM_WIN_BASIC);
   if (!w->win) return NULL;
   evas_object_data_set(w->win, "_enjoy", &w);
   evas_object_event_callback_add(w->win, EVAS_CALLBACK_DEL, _win_del, w);

   w->bg = elm_bg_add(w->win);
   evas_object_size_hint_weight_set
     (w->bg, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   evas_object_size_hint_fill_set(w->bg, EVAS_HINT_FILL, EVAS_HINT_FILL);
   elm_win_resize_object_add(w->win, w->bg);
   evas_object_show(w->bg);

   elm_win_autodel_set(w->win, EINA_FALSE);
   evas_object_smart_callback_add
     (w->win, "delete,request", _win_del_request, w);

   snprintf(path, sizeof(path), "%s/media.db", app->configdir);
   w->db_path = eina_stringshare_add(path);

   w->mgr = libmgr_new(w->db_path);

   w->emotion = emotion_object_add(evas_object_evas_get(w->win));
   if (!emotion_object_init(w->emotion, NULL))
     {
        CRITICAL("could not create emotion engine");
        goto error;
     }
   emotion_object_video_mute_set(w->emotion, EINA_TRUE);

   evas_object_smart_callback_add
     (w->emotion, "position_update", _win_play_pos_update, w);
   evas_object_smart_callback_add
     (w->emotion, "length_change", _win_play_pos_update, w);
   evas_object_smart_callback_add
     (w->emotion, "frame_decode", _win_play_pos_update, w);
   evas_object_smart_callback_add
     (w->emotion, "playback_started", _win_play_begin, w);
   evas_object_smart_callback_add
     (w->emotion, "playback_finished", _win_play_end, w);

   w->layout = elm_layout_add(w->win);
   if (!w->layout) goto error;
   evas_object_size_hint_weight_set
     (w->layout, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   evas_object_size_hint_align_set
     (w->layout, EVAS_HINT_FILL, EVAS_HINT_FILL);
   elm_win_resize_object_add(w->win, w->layout);

   if (!elm_layout_theme_set
       (w->layout, "layout", "application", "toolbar-content"))
     {
        ERR("could not load style 'toolbar-content' from theme");
        goto error;
     }

   w->edje = elm_layout_edje_get(w->layout);
   w->toolbar = edje_object_part_external_object_get
     (w->edje, "elm.external.toolbar");
   if (!w->toolbar)
     {
        ERR("no toolbar in layout!");
        goto error;
     }

   elm_toolbar_no_select_mode_set(w->toolbar, EINA_TRUE);
   elm_toolbar_menu_parent_set(w->toolbar, w->win);
   w->action.prev = _toolbar_item_add
      (w, "media-seek-backward", "Previous", 130, _win_prev);
   w->action.next = _toolbar_item_add(w, "media-seek-forward", "Next", 140, _win_next);
   w->action.play = _toolbar_item_add
      (w, "media-playback-start", "Play", 150, _win_action_play);
   w->action.pause = elm_toolbar_item_state_add
      (w->action.play, "media-playback-pause", "Pause", _win_action_pause, w);
   w->action.nowplaying = _toolbar_item_add
      (w, "multimedia-player", "Playing", 120, _win_mode_nowplaying);
   w->action.playlist = elm_toolbar_item_state_add
      (w->action.nowplaying, "system-file-manager", "Library",
       _win_mode_list, w);

   elm_toolbar_item_disabled_set(w->action.prev, EINA_TRUE);
   elm_toolbar_item_disabled_set(w->action.next, EINA_TRUE);
   elm_toolbar_item_disabled_set(w->action.play, EINA_TRUE);
   elm_toolbar_item_disabled_set(w->action.nowplaying, EINA_TRUE);

   w->list = list_add(w->layout);
   if (!w->list)
     {
        CRITICAL("cannot create list");
        goto error;
     }
   elm_layout_content_set(w->layout, "elm.swallow.content", w->list);
   evas_object_smart_callback_add(w->list, "selected", _win_list_selected, w);

   w->nowplaying = nowplaying_add(w->layout);
   nowplaying_edje = elm_layout_edje_get(w->nowplaying);
   edje_object_message_handler_set(nowplaying_edje, _win_edje_msg, w);
   edje_object_signal_callback_add
     (nowplaying_edje, "ejy,repeat,on", "ejy", _win_repeat_on, w);
   edje_object_signal_callback_add
     (nowplaying_edje, "ejy,repeat,off", "ejy", _win_repeat_off, w);
   edje_object_signal_callback_add
     (nowplaying_edje, "ejy,shuffle,on", "ejy", _win_shuffle_on, w);
   edje_object_signal_callback_add
     (nowplaying_edje, "ejy,shuffle,off", "ejy", _win_shuffle_off, w);
   elm_layout_content_set(w->layout, "ejy.swallow.nowplaying", w->nowplaying);
   edje_object_size_min_get(w->edje, &(w->min.w), &(w->min.h));
   edje_object_size_min_restricted_calc
     (w->edje, &(w->min.w), &(w->min.h), w->min.w, w->min.h);
   elm_pager_content_push(w->list, w->nowplaying);

   s = edje_object_data_get(w->edje, "initial_size");
   if (!s)
     WRN("no initial size specified.");
   else
     {
        if (sscanf(s, "%d %d", &iw, &ih) != 2)
          {
             ERR("invalid initial_size format %s.", s);
             iw = 320;
             ih = 240;
          }
     }
   s = edje_object_data_get(w->edje, "alpha");
   if (s) elm_win_alpha_set(w->win, !!atoi(s));
   s = edje_object_data_get(w->edje, "borderless");
   if (s) elm_win_borderless_set(w->win, !!atoi(s));

#ifdef EDJE_SIGNAL_DEBUG
   edje_object_signal_callback_add(w->edje, "*", "*", _edje_signal_debug, w);
#endif
   edje_object_message_handler_set(w->edje, _win_edje_msg, w);

   evas_object_show(w->layout);

   evas_object_resize(w->win, iw, ih);
   evas_object_size_hint_min_set(w->win, w->min.w, w->min.h);
   elm_win_title_set(w->win, PACKAGE_STRING);
   evas_object_show(w->win);

   if ((app->add_dirs) || (app->del_dirs))
     {
        w->scan.add = app->add_dirs;
        w->scan.del = app->del_dirs;
        w->job.scan = ecore_job_add(_win_scan_job, w);
     }
   else
     w->job.populate = ecore_job_add(_win_populate_job, w);

   srand(ecore_time_unix_get());

   return w->win;

 error:
   evas_object_del(w->win); /* should delete everything */
   return NULL;
}
