#include "private.h"
#include <Emotion.h>

#define MSG_VOLUME 1
#define MSG_POSITION 2
#define MSG_RATING 3

typedef struct Win
{
   Evas_Object *win;
   Evas_Object *layout;
   Evas_Object *edje;
   Evas_Object *emotion;
   Evas_Object *list;
   Evas_Object *nowplaying;
   const char *db_path;
   DB *db;
   Song *song;
   struct {
      double position, length;
      double volume;
      Eina_Bool playing:1;
      Eina_Bool playing_last:1;
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

   /*
    * Efreet is initialized here so it doesn't take longer to open the window.
    */
   efreet_mime_init();

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
_win_scan_job(void *data)
{
   Win *w = data;
   w->job.scan = NULL;
   DBG("TODO");
   printf("SCAN IS NOT IMPLEMENTED!\n"
          "Meanwhile please run "
          "(./test = binary from lightmediascanner/src/bin):\n");

   Eina_List *l;
   const char *path;
   EINA_LIST_FOREACH(w->scan.add, l, path)
     printf("   ./test -p id3 -i 5000 -s %s %s\n",
            path, w->db_path);
   EINA_LIST_FOREACH(w->scan.del, l, path)
     printf("   sqlite3 %s \"delete from files where path like '%s%%'\"\n",
            w->db_path, path);

   // notify win (should stop lists from updating)
   // create lms
   // w->thread.scan = ecore_thread_...: create thread
   // emit delete as sqlite statements
   // start lms process + check from thread
   // finish thread -> unmark it from Win
   // notify win (should reload lists)

   if (!w->job.populate)
     w->job.populate = ecore_job_add(_win_populate_job, w);
}

static void
_win_toolbar_eval(Win *w)
{
   if (list_prev_exists(w->list))
     edje_object_signal_emit(w->edje, "ejy,prev,enable", "ejy");
   else
     edje_object_signal_emit(w->edje, "ejy,prev,disable", "ejy");

   if (list_next_exists(w->list))
     edje_object_signal_emit(w->edje, "ejy,next,enable", "ejy");
   else
     edje_object_signal_emit(w->edje, "ejy,next,disable", "ejy");

   if (w->song)
     {
        edje_object_signal_emit(w->edje, "ejy,action,play,enable", "ejy");
        edje_object_signal_emit(w->edje, "ejy,action,pause,enable", "ejy");
        edje_object_signal_emit(w->edje, "ejy,mode,list,enable", "ejy");
        edje_object_signal_emit(w->edje, "ejy,mode,nowplaying,enable", "ejy");
     }
   else
     {
        edje_object_signal_emit(w->edje, "ejy,action,play,disable", "ejy");
        edje_object_signal_emit(w->edje, "ejy,action,pause,disable", "ejy");
        edje_object_signal_emit(w->edje, "ejy,mode,list,disable", "ejy");
        edje_object_signal_emit(w->edje, "ejy,mode,nowplaying,disable", "ejy");
     }
}

static void
_win_play_pause_toggle(Win *w)
{
   if (w->play.playing)
     {
        edje_object_signal_emit(w->edje, "ejy,action,play,hide", "ejy");
        edje_object_signal_emit(w->edje, "ejy,action,pause,show", "ejy");
     }
   else
     {
        edje_object_signal_emit(w->edje, "ejy,action,pause,hide", "ejy");
        edje_object_signal_emit(w->edje, "ejy,action,play,show", "ejy");
     }

}
static void
_win_play_eval(Win *w)
{
   Edje_Message_Float_Set *mf;

   w->play.length = emotion_object_play_length_get(w->emotion);
   if ((w->song) && (w->song->length != (int)w->play.length))
     {
        db_song_length_set(w->db, w->song, w->play.length);
        list_song_updated(w->list);
     }

   w->play.position = emotion_object_position_get(w->emotion);

   mf = alloca(sizeof(Edje_Message_Float_Set) + sizeof(double));
   mf->count = 2;
   mf->val[0] = w->play.position;
   mf->val[1] = w->play.length;
   edje_object_message_send(elm_layout_edje_get(w->nowplaying), EDJE_MESSAGE_FLOAT_SET, MSG_POSITION, mf);

   if (w->play.playing_last == w->play.playing) return;
   w->play.playing_last = !w->play.playing;
   _win_play_pause_toggle(w);
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
   char *artist_title, *s1, *s2;
   cover = cover_album_fetch_by_id(w->win, w->db,w->song->album_id, 256); // TODO: size!
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
   edje_object_message_send(w->edje, EDJE_MESSAGE_INT, MSG_RATING, &mi);

   emotion_object_file_set(w->emotion, s->path);
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
_win_play_end(void *data, Evas_Object *o __UNUSED__, void *event_info __UNUSED__)
{
   Win *w = data;
   Song *s = list_next_go(w->list);
   _win_song_set(w, s);
}

static void
_win_list_selected(void *data, Evas_Object *list __UNUSED__, void *event_info)
{
   Win *w = data;
   Song *s = event_info;
   _win_song_set(w, s);
}

static void
_win_list_changed(void *data, Evas_Object *list __UNUSED__, void *event_info __UNUSED__)
{
   Win *w = data;
   _win_song_set(w, list_selected_get(w->list));
   if (list_songs_exists(w->list))
     edje_object_signal_emit(w->edje, "ejy,songs,show", "ejy");
   else
     edje_object_signal_emit(w->edje, "ejy,songs,hide", "ejy");
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
_win_prev(void *data, Evas_Object *o __UNUSED__, const char *emission __UNUSED__, const char *source __UNUSED__)
{
   Win *w = data;
   Song *s = list_prev_go(w->list);
   INF("prev song=%p (%s)", s, s ? s->path : NULL);
   if (s) _win_song_set(w, s);
}

static void
_win_next(void *data, Evas_Object *o __UNUSED__, const char *emission __UNUSED__, const char *source __UNUSED__)
{
   Win *w = data;
   Song *s = list_next_go(w->list);
   INF("next song=%p (%s)", s, s ? s->path : NULL);
   if (s) _win_song_set(w, s);
}

static void
_win_action_play(void *data, Evas_Object *o __UNUSED__, const char *emission __UNUSED__, const char *source __UNUSED__)
{
   Win *w = data;
   INF("play song=%p (%s)", w->song, w->song ? w->song->path : NULL);
   w->play.playing = EINA_TRUE;
   emotion_object_play_set(w->emotion, EINA_TRUE);
   _win_play_pause_toggle(w);
   _win_play_eval(w);
}

static void
_win_action_pause(void *data, Evas_Object *o __UNUSED__, const char *emission __UNUSED__, const char *source __UNUSED__)
{
   Win *w = data;
   INF("pause song=%p (%s)", w->song, w->song ? w->song->path : NULL);
   w->play.playing = EINA_FALSE;
   emotion_object_play_set(w->emotion, EINA_FALSE);
   _win_play_pause_toggle(w);
   _win_play_eval(w);
}

static void
_win_mode_list(void *data, Evas_Object *o __UNUSED__, const char *emission __UNUSED__, const char *source __UNUSED__)
{
   Win *w = data;
   edje_object_signal_emit(w->edje, "ejy,mode,list,hide", "ejy");
   edje_object_signal_emit(w->edje, "ejy,mode,nowplaying,show", "ejy");
   edje_object_signal_emit(w->edje, "ejy,screen,nowplaying,hide", "ejy");
}

static void
_win_mode_nowplaying(void *data, Evas_Object *o __UNUSED__, const char *emission __UNUSED__, const char *source __UNUSED__)
{
   Win *w = data;
   edje_object_signal_emit(w->edje, "ejy,mode,nowplaying,hide", "ejy");
   edje_object_signal_emit(w->edje, "ejy,mode,list,show", "ejy");
   edje_object_signal_emit(w->edje, "ejy,screen,nowplaying,show", "ejy");
}

static void
_win_more(void *data, Evas_Object *o __UNUSED__, const char *emission __UNUSED__, const char *source __UNUSED__)
{
   Win *w = data;
   DBG("todo");
}

static void
_win_songs(void *data __UNUSED__, Evas_Object *o __UNUSED__, const char *emission __UNUSED__, const char *source __UNUSED__)
{
   Win *w = data;
   if (!list_songs_show(w->list)) return;
   edje_object_signal_emit(w->edje, "ejy,mode,nowplaying,hide", "ejy");
   edje_object_signal_emit(w->edje, "ejy,mode,list,show", "ejy");
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
                db_song_rating_set(w->db, w->song, m->val);
           }
        break;

      default:
         ERR("unknown edje message id: %d of type: %d", id, type);
     }
}


Evas_Object *
win_new(App *app)
{
   Win *w = &_win;
   const char *s;
   const char **e;
   Evas_Coord iw = 320, ih = 240;
   char path[PATH_MAX];

   memset(w, 0, sizeof(*w));

   w->play.volume = 0.8;

   w->win = elm_win_add(NULL, PACKAGE_NAME, ELM_WIN_BASIC);
   if (!w->win) return NULL;
   evas_object_data_set(w->win, "_enjoy", &w);
   evas_object_event_callback_add(w->win, EVAS_CALLBACK_DEL, _win_del, w);

   elm_win_autodel_set(w->win, 1); // TODO
   elm_policy_set(ELM_POLICY_QUIT, ELM_POLICY_QUIT_LAST_WINDOW_CLOSED);

   snprintf(path, sizeof(path), "%s/media.db", app->configdir);
   w->db_path = eina_stringshare_add(path);

   w->emotion = emotion_object_add(evas_object_evas_get(w->win));
   if (!emotion_object_init(w->emotion, NULL))
     {
        CRITICAL("could not create emotion engine");
        goto error;
     }
   emotion_object_video_mute_set(w->emotion, EINA_TRUE);
   evas_object_show(w->emotion); // req?
   evas_object_resize(w->emotion, 10, 10); // req?

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

   if (!elm_layout_file_set(w->layout, PACKAGE_DATA_DIR "/default.edj", "win"))
     {
        CRITICAL("no theme for 'win' at %s", PACKAGE_DATA_DIR "/default.edj");
        goto error;
     }

   w->list = list_add(w->layout);
   if (!w->list)
     {
        CRITICAL("cannot create list");
        goto error;
     }
   elm_layout_content_set(w->layout, "ejy.swallow.list", w->list);
   evas_object_smart_callback_add(w->list, "selected", _win_list_selected, w);
   evas_object_smart_callback_add(w->list, "changed", _win_list_changed, w);

   w->nowplaying = nowplaying_add(w->layout);
   edje_object_message_handler_set(elm_layout_edje_get(w->nowplaying), _win_edje_msg, w);
   elm_layout_content_set(w->layout, "ejy.swallow.nowplaying", w->nowplaying);
   w->edje = elm_layout_edje_get(w->layout);
   edje_object_size_min_get(w->edje, &(w->min.w), &(w->min.h));
   edje_object_size_min_restricted_calc
     (w->edje, &(w->min.w), &(w->min.h), w->min.w, w->min.h);

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

   edje_object_signal_callback_add
     (w->edje, "ejy,prev,clicked", "ejy", _win_prev, w);
   edje_object_signal_callback_add
     (w->edje, "ejy,next,clicked", "ejy", _win_next, w);
   edje_object_signal_callback_add
     (w->edje, "ejy,action,play,clicked", "ejy", _win_action_play, w);
   edje_object_signal_callback_add
     (w->edje, "ejy,action,pause,clicked", "ejy", _win_action_pause, w);
   edje_object_signal_callback_add
     (w->edje, "ejy,mode,list,clicked", "ejy", _win_mode_list, w);
   edje_object_signal_callback_add
     (w->edje, "ejy,mode,nowplaying,clicked", "ejy", _win_mode_nowplaying, w);
   edje_object_signal_callback_add
     (w->edje, "ejy,more,clicked", "ejy", _win_more, w);
   edje_object_signal_callback_add
     (w->edje, "ejy,songs,clicked", "ejy", _win_songs, w);
   edje_object_message_handler_set(w->edje, _win_edje_msg, w);

   edje_object_signal_emit(w->edje, "ejy,prev,disable", "ejy");
   edje_object_signal_emit(w->edje, "ejy,next,disable", "ejy");
   edje_object_signal_emit(w->edje, "ejy,action,pause,hide", "ejy");
   edje_object_signal_emit(w->edje, "ejy,action,play,disable", "ejy");
   edje_object_signal_emit(w->edje, "ejy,mode,list,hide", "ejy");
   edje_object_signal_emit(w->edje, "ejy,mode,nowplaying,disable", "ejy");
   edje_object_signal_emit(w->edje, "ejy,more,disable", "ejy");

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

   return w->win;

 error:
   evas_object_del(w->win); /* should delete everything */
   return NULL;
}
