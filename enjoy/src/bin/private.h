#ifndef ENJOY_PRIVATE_H
#define ENJOY_PRIVATE_H

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "log.h"
#include "plugin.h"
#include "song.h"

#include <Elementary.h>
#include <limits.h>

#define stringify(X) #X

typedef struct _App                     App;
typedef struct _DB                      DB;
typedef struct _Libmgr                  Libmgr;
typedef struct _Song                    Song;
typedef struct _NameID                  NameID;
typedef struct _Album_Cover             Album_Cover;
typedef struct _Album                   Album;
typedef struct _NameID                  Artist;
typedef struct _NameID                  Genre;

#define MSG_VOLUME   1
#define MSG_POSITION 2
#define MSG_RATING   3
#define MSG_MUTE     4
#define MSG_LOOP     5
#define MSG_SHUFFLE  6

struct _App
{
   Eina_List   *add_dirs;
   Eina_List   *del_dirs;
   char         configdir[PATH_MAX];
   Evas_Object *win;
   Eina_Array  *modules;
   struct {
      struct {
         int caps_change;
         int status_change;
         int track_change;
      } player;
      struct {
         int tracklist_change;
      } tracklist;
   } event_id;
};

Evas_Object *win_new(App *app);

Libmgr      *libmgr_new(const char *dbpath);
Eina_Bool    libmgr_scanpath_add(Libmgr *mgr, const char *path);
Eina_Bool    libmgr_scan_start(Libmgr *mgr, void (*func_end)(void *, Eina_Bool), void *data);

Evas_Object *list_add(Evas_Object *parent);
void         list_promote_current(Evas_Object *list);
Eina_Bool    list_populate(Evas_Object *list, DB *db);
Eina_Bool    list_songs_exists(const Evas_Object *obj);
Song        *list_selected_get(const Evas_Object *list);
Eina_Bool    list_song_updated(Evas_Object *obj);
Eina_Bool    list_next_exists(const Evas_Object *list);
Song        *list_next_go(Evas_Object *list);
Song        *list_shuffle_next_go(Evas_Object *list);
Song        *list_shuffle_prev_go(Evas_Object *list);
void         list_shuffle_reset(Evas_Object *obj);
Eina_Bool    list_prev_exists(const Evas_Object *list);
Song        *list_prev_go(Evas_Object *list);
DB          *list_db_get(const Evas_Object *obj);
void         list_freeze(Evas_Object *obj);
void         list_thaw(Evas_Object *obj);
Song        *list_song_nth_get(Evas_Object *obj, int32_t n);
int32_t      list_song_selected_n_get(Evas_Object *obj);

const char  *page_title_get(const Evas_Object *obj);
void         page_songs_exists_changed(Evas_Object *obj, Eina_Bool exists);
void         page_back_show(Evas_Object *page);
void         page_back_hide(Evas_Object *page);
void         page_playing_show(Evas_Object *page);
void         page_playing_hide(Evas_Object *page);

Evas_Object *page_root_add(Evas_Object *parent);

Song        *page_songs_selected_get(const Evas_Object *obj);
Eina_Bool    page_songs_song_updated(Evas_Object *obj);
Eina_Bool    page_songs_next_exists(const Evas_Object *obj);
Song        *page_songs_next_go(Evas_Object *obj);
Song        *page_songs_shuffle_next_go(Evas_Object *obj);
Song        *page_songs_shuffle_prev_go(Evas_Object *obj);
void         page_songs_shuffle_reset(Evas_Object *obj);
Eina_Bool    page_songs_prev_exists(const Evas_Object *obj);
Song        *page_songs_prev_go(Evas_Object *obj);
Song        *page_songs_nth_get(Evas_Object *obj, int32_t n);
int32_t      page_songs_selected_n_get(Evas_Object *obj);

Evas_Object *cover_allsongs_fetch(Evas_Object *parent, unsigned short size);
Evas_Object *cover_album_fetch(Evas_Object *parent, DB *db, Album *album, unsigned short size, void (*cb)(void *data), void *data);
Evas_Object *cover_album_fetch_by_id(Evas_Object *parent, DB *db, int64_t album_id, unsigned short size, void (*cb)(void *data), void *data);
void         cover_init(void);
void         cover_shutdown(void);


DB        *db_open(const char *path);
Eina_Bool  db_close(DB *db);

Evas_Object *nowplaying_add(Evas_Object *parent);

typedef enum {
   ALBUM_COVER_ORIGIN_LOCAL,
   ALBUM_COVER_ORIGIN_LASTFM
} Album_Cover_Origin;

struct _Album_Cover
{
   EINA_INLIST;
   unsigned short w;
   unsigned short h;
   unsigned short path_len;
   Album_Cover_Origin origin;
   char path[];
};

struct _Album
{
   int64_t id;
   int64_t artist_id;
   const char *name;
   const char *artist;
   Eina_Inlist *covers;
   struct {
      unsigned int name;
      unsigned int artist;
   } len; /* strlen of string fields */
   struct { /* not from db, for runtime use */
      Eina_Bool fetched_artist:1;
      Eina_Bool fetched_covers:1;
   } flags;
};

struct _NameID
{
   int64_t id;
   const char *name;
   unsigned int len;
};

Eina_Iterator *db_songs_get(DB *db); /* walks over 'const Song*'  */
Song          *db_song_copy(const Song *orig);
void           db_song_free(Song *song);
Eina_Bool      db_song_rating_set(DB *db, Song *song, int rating);
Eina_Bool      db_song_length_set(DB *db, Song *song, int length);
Eina_Bool      db_song_album_fetch(DB *db, Song *song);
Eina_Bool      db_song_artist_fetch(DB *db, Song *song);
Eina_Bool      db_song_genre_fetch(DB *db, Song *song);

Song           *db_song_get(DB *db, int64_t id);

/* walks over 'const Song*'  */
Eina_Iterator *db_album_songs_get(DB *db, int64_t album_id);
Eina_Iterator *db_artist_songs_get(DB *db, int64_t artist_id);
Eina_Iterator *db_genre_songs_get(DB *db, int64_t genre_id);

/* walks over 'const Album*' */
Eina_Iterator *db_artist_albums_get(DB *db, int64_t artist_id);
Eina_Iterator *db_genre_albums_get(DB *db, int64_t genre_id);


Eina_Iterator *db_albums_get(DB *db); /* walks over 'const Album*' */
Eina_Iterator *db_artists_get(DB *db); /* walks over 'const Artist*' */
Eina_Iterator *db_genres_get(DB *db); /* walks over 'const Genre*' */


Album         *db_album_copy(const Album *album);
Eina_Bool      db_album_artist_fetch(DB *db, Album *album);
Eina_Bool      db_album_covers_fetch(DB *db, Album *album);
Eina_Bool      db_album_covers_update(DB *db, const Album *album);
void           db_album_free(Album *album);

NameID        *db_nameid_copy(const NameID *nameid);
#define        db_artist_copy(v) db_nameid_copy(v)
#define        db_genre_copy(v)  db_nameid_copy(v)

void           db_nameid_free(NameID *nameid);
#define        db_artist_free(v) db_nameid_free(v)
#define        db_genre_free(v)  db_nameid_free(v)

#endif
