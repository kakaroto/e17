#include "private.h"
#include <sqlite3.h>

struct _DB
{
   const char *path;
   sqlite3 *handle;
   struct {
      /* just pre-compile most used and complex operations, no need to do so
       * for simple update statements like "set song playcnt".
       */
      sqlite3_stmt *songs_get;
      sqlite3_stmt *album_songs_get;
      sqlite3_stmt *artist_songs_get;
      sqlite3_stmt *genre_songs_get;
      sqlite3_stmt *album_get;
      sqlite3_stmt *artist_get;
      sqlite3_stmt *genre_get;
      sqlite3_stmt *albums_get;
      sqlite3_stmt *artists_get;
      sqlite3_stmt *genres_get;
   } stmt;
};

static Eina_Bool
_db_stmt_bind_int64(sqlite3_stmt *stmt, int col, int64_t value)
{
   int r = sqlite3_bind_int64(stmt, col, value);
   if (r == SQLITE_OK)
     return EINA_TRUE;
   else
     {
        sqlite3 *db = sqlite3_db_handle(stmt);;
        const char *err = sqlite3_errmsg(db);
        ERR("could not bind SQL value %lld to column %d: %s",
            (long long)value, col, err);
        return EINA_FALSE;
     }
}

static Eina_Bool
_db_stmt_reset(sqlite3_stmt *stmt)
{
   Eina_Bool r, c;

   r = sqlite3_reset(stmt) == SQLITE_OK;
   if (!r)
     ERR("could not reset SQL statement");

   c = sqlite3_clear_bindings(stmt) == SQLITE_OK;
   if (!c)
     ERR("could not clear SQL");

   return r && c;
}

static Eina_Bool
_db_stmt_finalize(sqlite3_stmt *stmt, const char *name)
{
   int r = sqlite3_finalize(stmt);
   if (r != SQLITE_OK)
     ERR("could not finalize %s statement: #%d\n", name, r);
   return r == SQLITE_OK;
}

static sqlite3_stmt *
_db_stmt_compile(DB *db, const char *name, const char *sql)
{
   sqlite3_stmt *stmt = NULL;

   if (sqlite3_prepare_v2(db->handle, sql, -1, &stmt, NULL) != SQLITE_OK)
     ERR("could not prepare %s sql=\"%s\": %s",
         name, sql, sqlite3_errmsg(db->handle));

    return stmt;
}

static Eina_Bool
_db_stmts_compile(DB *db)
{
#define C(m, sql)                                               \
   do                                                           \
     {                                                          \
        db->stmt.m = _db_stmt_compile(db, stringify(m), sql);   \
        if (!db->stmt.m) return EINA_FALSE;                     \
     }                                                          \
   while (0)

   C(songs_get,
     "SELECT files.id, files.path, files.size, "
     " audios.title, audios.album_id, audios.artist_id, audios.genre_id, "
     " audios.trackno, audios.rating, audios.playcnt, audios.length "
     "FROM audios, files "
     "WHERE "
     " files.id = audios.id "
     "ORDER BY UPPER(audios.title)");

   C(album_songs_get,
     "SELECT files.id, files.path, files.size, "
     " audios.title, audios.album_id, audios.artist_id, audios.genre_id, "
     " audios.trackno, audios.rating, audios.playcnt, audios.length "
     "FROM audios, files "
     "WHERE "
     " files.id = audios.id AND "
     " audios.album_id = ? "
     "ORDER BY audios.trackno, UPPER(audios.title)");

   C(artist_songs_get,
     "SELECT files.id, files.path, files.size, "
     " audios.title, audios.album_id, audios.artist_id, audios.genre_id, "
     " audios.trackno, audios.rating, audios.playcnt, audios.length "
     "FROM audios, files "
     "WHERE "
     " files.id = audios.id AND "
     " audios.artist_id = ? "
     "ORDER BY UPPER(audios.title)");

   C(genre_songs_get,
     "SELECT files.id, files.path, files.size, "
     " audios.title, audios.album_id, audios.artist_id, audios.genre_id, "
     " audios.trackno, audios.rating, audios.playcnt, audios.length "
     "FROM audios, files "
     "WHERE "
     " files.id = audios.id AND "
     " audios.genre_id = ? "
     "ORDER BY UPPER(audios.title)");

   C(album_get, "SELECT name FROM audio_albums WHERE id = ?");
   C(artist_get, "SELECT name FROM audio_artists WHERE id = ?");
   C(genre_get, "SELECT name FROM audio_genres WHERE id = ?");

   C(albums_get, "SELECT id, name FROM audio_albums ORDER BY UPPER(name)");
   C(artists_get, "SELECT id, name FROM audio_artists ORDER BY UPPER(name)");
   C(genres_get, "SELECT id, name FROM audio_genres ORDER BY UPPER(name)");

#undef C
   return EINA_TRUE;
}

static Eina_Bool
_db_stmts_finalize(DB *db)
{
   Eina_Bool ret = EINA_TRUE;

#define F(m)                                            \
   ret &= _db_stmt_finalize(db->stmt.m, stringify(m));

   F(songs_get);
   F(album_songs_get);
   F(artist_songs_get);
   F(genre_songs_get);

   F(album_get);
   F(artist_get);
   F(genre_get);

   F(albums_get);
   F(artists_get);
   F(genres_get);

#undef F
   return ret;
}

DB *
db_open(const char *path)
{
   DB *db;

   EINA_SAFETY_ON_NULL_RETURN_VAL(path, NULL);
   db = calloc(1, sizeof(DB));
   EINA_SAFETY_ON_NULL_RETURN_VAL(db, NULL);

   db->path = eina_stringshare_add(path);

   if (sqlite3_open(path, &db->handle) != SQLITE_OK)
     {
        CRITICAL("Could not open database '%s'", db->path);
        goto error;
     }

   if (!_db_stmts_compile(db))
     {
        CRITICAL("Could not compile statements.");
        goto error;
     }

   return db;

 error:
   _db_stmts_finalize(db);
   sqlite3_close(db->handle);
   eina_stringshare_del(db->path);
   free(db);
   return NULL;
}

Eina_Bool
db_close(DB *db)
{
   EINA_SAFETY_ON_NULL_RETURN_VAL(db, EINA_FALSE);
   _db_stmts_finalize(db);
   sqlite3_close(db->handle);
   eina_stringshare_del(db->path);
   free(db);
   return EINA_TRUE;
}

Song *db_song_copy(const Song *orig)
{
   // TODO: move to mempool to avoid fragmentation!
   Song *copy;
   EINA_SAFETY_ON_NULL_RETURN_VAL(orig, NULL);
   copy = malloc(sizeof(Song));
   EINA_SAFETY_ON_NULL_RETURN_VAL(copy, NULL);

   /* Note: cannot use eina_stringshare_ref() as value may be from sqlite
    * during iterator walks.
    */
#define STR(m)                                  \
   copy->m = eina_stringshare_add(orig->m);     \
   copy->len.m = orig->len.m
   STR(path);
   STR(title);
   STR(album);
   STR(artist);
#undef STR

#define N(m) copy->m = orig->m
   N(id);
   N(album_id);
   N(artist_id);
   N(genre_id);
   N(size);
   N(trackno);
   N(rating);
   N(playcnt);
   N(length);
#undef N

   return copy;
}

void
db_song_free(Song *song)
{
   eina_stringshare_del(song->path);
   eina_stringshare_del(song->title);
   eina_stringshare_del(song->album);
   eina_stringshare_del(song->artist);
   free(song);
}

struct DB_Iterator
{
   Eina_Iterator base;
   DB *db;
   sqlite3_stmt *stmt;
};

struct DB_Iterator_Songs
{
   struct DB_Iterator base;
   Song song;
};

static void *
_db_iterator_container_get(Eina_Iterator *iterator)
{
   struct DB_Iterator *it = (struct DB_Iterator *)iterator;
   if (!EINA_MAGIC_CHECK(iterator, EINA_MAGIC_ITERATOR))
     {
        EINA_MAGIC_FAIL(iterator, EINA_MAGIC_ITERATOR);
        return NULL;
     }
   return it->db;
}

static void
_db_iterator_free(Eina_Iterator *iterator)
{
   struct DB_Iterator *it = (struct DB_Iterator *)iterator;
   if (!EINA_MAGIC_CHECK(iterator, EINA_MAGIC_ITERATOR))
     {
        EINA_MAGIC_FAIL(iterator, EINA_MAGIC_ITERATOR);
        return;
     }
   _db_stmt_reset(it->stmt);
   EINA_MAGIC_SET(&it->base, EINA_MAGIC_NONE);
   free(it);
}

static Eina_Bool
_db_iterator_songs_next(Eina_Iterator *iterator, void **data)
{
   struct DB_Iterator_Songs *it = (struct DB_Iterator_Songs *)iterator;
   Song **song = (Song **)data;
   int r;

   EINA_SAFETY_ON_NULL_RETURN_VAL(song, EINA_FALSE);
   *song = NULL;
   if (!EINA_MAGIC_CHECK(iterator, EINA_MAGIC_ITERATOR))
     {
        EINA_MAGIC_FAIL(iterator, EINA_MAGIC_ITERATOR);
        return EINA_FALSE;
     }

   r = sqlite3_step(it->base.stmt);
   if (r == SQLITE_DONE)
     return EINA_FALSE;
   if (r != SQLITE_ROW)
     {
        ERR("Error executing sql statement: %s",
            sqlite3_errmsg(it->base.db->handle));
        return EINA_FALSE;
     }

#define ID(m, c)                                        \
   it->song.m = sqlite3_column_int64(it->base.stmt, c);
#define INT(m, c)                                       \
   it->song.m = sqlite3_column_int(it->base.stmt, c);
#define STR(m, c)                                                       \
   it->song.m = (const char *)sqlite3_column_text(it->base.stmt, c);    \
   it->song.len.m = sqlite3_column_bytes(it->base.stmt, c)

   ID(id, 0);
   STR(path, 1);
   INT(size, 2);
   STR(title, 3);
   ID(album_id, 4);
   ID(artist_id, 5);
   ID(genre_id, 6);
   INT(trackno, 7);
   INT(rating, 8);
   INT(playcnt, 9);
   INT(length, 10);

#undef STR
#undef INT
#undef ID

   *song = &it->song;

   return EINA_TRUE;
}

Eina_Iterator *
db_songs_get(DB *db)
{
   struct DB_Iterator_Songs *it;
   EINA_SAFETY_ON_NULL_RETURN_VAL(db, NULL);
   it = calloc(1,  sizeof(*it));
   EINA_SAFETY_ON_NULL_RETURN_VAL(it, NULL);

   it->base.base.version = EINA_ITERATOR_VERSION;
   it->base.base.next = _db_iterator_songs_next;
   it->base.base.get_container = _db_iterator_container_get;
   it->base.base.free = _db_iterator_free;
   it->base.db = db;
   it->base.stmt = db->stmt.songs_get;

   EINA_MAGIC_SET(&it->base.base, EINA_MAGIC_ITERATOR);
   return &it->base.base;
}

Eina_Bool
db_song_rating_set(DB *db, Song *song, int rating)
{
   DBG("TODO");
   return EINA_TRUE;
}

Eina_Bool
db_song_length_set(DB *db, Song *song, int length)
{
   DBG("TODO");
   return EINA_TRUE;
}

Eina_Iterator *
db_album_songs_get(DB *db, int64_t album_id)
{
   struct DB_Iterator_Songs *it;
   EINA_SAFETY_ON_NULL_RETURN_VAL(db, NULL);
   it = calloc(1,  sizeof(*it));
   EINA_SAFETY_ON_NULL_RETURN_VAL(it, NULL);

   it->base.base.version = EINA_ITERATOR_VERSION;
   it->base.base.next = _db_iterator_songs_next;
   it->base.base.get_container = _db_iterator_container_get;
   it->base.base.free = _db_iterator_free;
   it->base.db = db;
   it->base.stmt = db->stmt.album_songs_get;

   if (!_db_stmt_bind_int64(it->base.stmt, 1, album_id))
     {
        free(it);
        return NULL;
     }

   EINA_MAGIC_SET(&it->base.base, EINA_MAGIC_ITERATOR);
   return &it->base.base;
}

Eina_Iterator *
db_artist_songs_get(DB *db, int64_t artist_id)
{
   struct DB_Iterator_Songs *it;
   EINA_SAFETY_ON_NULL_RETURN_VAL(db, NULL);
   it = calloc(1,  sizeof(*it));
   EINA_SAFETY_ON_NULL_RETURN_VAL(it, NULL);

   it->base.base.version = EINA_ITERATOR_VERSION;
   it->base.base.next = _db_iterator_songs_next;
   it->base.base.get_container = _db_iterator_container_get;
   it->base.base.free = _db_iterator_free;
   it->base.db = db;
   it->base.stmt = db->stmt.artist_songs_get;

   if (!_db_stmt_bind_int64(it->base.stmt, 1, artist_id))
     {
        free(it);
        return NULL;
     }

   EINA_MAGIC_SET(&it->base.base, EINA_MAGIC_ITERATOR);
   return &it->base.base;
}

Eina_Iterator *
db_genre_songs_get(DB *db, int64_t genre_id)
{
   struct DB_Iterator_Songs *it;
   EINA_SAFETY_ON_NULL_RETURN_VAL(db, NULL);
   it = calloc(1,  sizeof(*it));
   EINA_SAFETY_ON_NULL_RETURN_VAL(it, NULL);

   it->base.base.version = EINA_ITERATOR_VERSION;
   it->base.base.next = _db_iterator_songs_next;
   it->base.base.get_container = _db_iterator_container_get;
   it->base.base.free = _db_iterator_free;
   it->base.db = db;
   it->base.stmt = db->stmt.genre_songs_get;

   if (!_db_stmt_bind_int64(it->base.stmt, 1, genre_id))
     {
        free(it);
        return NULL;
     }

   EINA_MAGIC_SET(&it->base.base, EINA_MAGIC_ITERATOR);
   return &it->base.base;
}

Eina_Bool
db_song_album_fetch(DB *db, Song *song)
{
   sqlite3_stmt *stmt;
   Eina_Bool ret;
   int err;

   EINA_SAFETY_ON_NULL_RETURN_VAL(db, EINA_FALSE);
   EINA_SAFETY_ON_NULL_RETURN_VAL(song, EINA_FALSE);
   if (song->flags.fetched_album) return EINA_TRUE;

   stmt = db->stmt.album_get;
   if (!_db_stmt_bind_int64(stmt, 1, song->album_id))
     return EINA_FALSE;

   err = sqlite3_step(stmt);
   if (err == SQLITE_ROW)
     {
        eina_stringshare_replace
          (&song->album, (const char *)sqlite3_column_text(stmt, 0));
        song->len.album = sqlite3_column_bytes(stmt, 0);
        ret = EINA_TRUE;
     }
   else if (err == SQLITE_DONE)
     {
        DBG("no album with id=%lld", (long long)song->album_id);
        eina_stringshare_replace(&song->album, NULL);
        song->len.album = 0;
        ret = EINA_TRUE;
     }
   else
     {
        ERR("could not query album with id=%lld: %s",
            (long long)song->album_id, sqlite3_errmsg(db->handle));
        ret = EINA_FALSE;
     }

   _db_stmt_reset(stmt);
   song->flags.fetched_album = ret;
   return ret;
}

Eina_Bool
db_song_artist_fetch(DB *db, Song *song)
{
   sqlite3_stmt *stmt;
   Eina_Bool ret;
   int err;

   EINA_SAFETY_ON_NULL_RETURN_VAL(db, EINA_FALSE);
   EINA_SAFETY_ON_NULL_RETURN_VAL(song, EINA_FALSE);
   if (song->flags.fetched_artist) return EINA_TRUE;

   stmt = db->stmt.artist_get;
   if (!_db_stmt_bind_int64(stmt, 1, song->artist_id))
     return EINA_FALSE;

   err = sqlite3_step(stmt);
   if (err == SQLITE_ROW)
     {
        eina_stringshare_replace
          (&song->artist, (const char *)sqlite3_column_text(stmt, 0));
        song->len.artist = sqlite3_column_bytes(stmt, 0);
        ret = EINA_TRUE;
     }
   else if (err == SQLITE_DONE)
     {
        DBG("no artist with id=%lld", (long long)song->artist_id);
        eina_stringshare_replace(&song->artist, NULL);
        song->len.artist = 0;
        ret = EINA_TRUE;
     }
   else
     {
        ERR("could not query artist with id=%lld: %s",
            (long long)song->artist_id, sqlite3_errmsg(db->handle));
        ret = EINA_FALSE;
     }

   _db_stmt_reset(stmt);
   song->flags.fetched_artist = ret;
   return ret;
}

Eina_Bool
db_song_genre_fetch(DB *db, Song *song)
{
   sqlite3_stmt *stmt;
   Eina_Bool ret;
   int err;

   EINA_SAFETY_ON_NULL_RETURN_VAL(db, EINA_FALSE);
   EINA_SAFETY_ON_NULL_RETURN_VAL(song, EINA_FALSE);
   if (song->flags.fetched_genre) return EINA_TRUE;

   stmt = db->stmt.genre_get;
   if (!_db_stmt_bind_int64(stmt, 1, song->genre_id))
     return EINA_FALSE;

   err = sqlite3_step(stmt);
   if (err == SQLITE_ROW)
     {
        eina_stringshare_replace
          (&song->genre, (const char *)sqlite3_column_text(stmt, 0));
        song->len.genre = sqlite3_column_bytes(stmt, 0);
        ret = EINA_TRUE;
     }
   else if (err == SQLITE_DONE)
     {
        DBG("no genre with id=%lld", (long long)song->genre_id);
        eina_stringshare_replace(&song->genre, NULL);
        song->len.genre = 0;
        ret = EINA_TRUE;
     }
   else
     {
        ERR("could not query genre with id=%lld: %s",
            (long long)song->genre_id, sqlite3_errmsg(db->handle));
        ret = EINA_FALSE;
     }

   _db_stmt_reset(stmt);
   song->flags.fetched_genre = ret;
   return ret;
}

NameID *
db_nameid_copy(const NameID *orig)
{
   NameID *copy;
   EINA_SAFETY_ON_NULL_RETURN_VAL(orig, NULL);
   copy = malloc(sizeof(NameID));
   EINA_SAFETY_ON_NULL_RETURN_VAL(copy, NULL);
   copy->id = orig->id;
   copy->len = orig->len;
   copy->name = eina_stringshare_add(orig->name);
   return copy;
}

void
db_nameid_free(NameID *nameid)
{
   eina_stringshare_del(nameid->name);
   free(nameid);
}

struct DB_Iterator_NameID
{
   struct DB_Iterator base;
   NameID nameid;
};

static Eina_Bool
_db_iterator_nameid_next(Eina_Iterator *iterator, void **data)
{
   struct DB_Iterator_NameID *it = (struct DB_Iterator_NameID *)iterator;
   NameID **nameid = (NameID **)data;
   int r;

   EINA_SAFETY_ON_NULL_RETURN_VAL(nameid, EINA_FALSE);
   *nameid = NULL;
   if (!EINA_MAGIC_CHECK(iterator, EINA_MAGIC_ITERATOR))
     {
        EINA_MAGIC_FAIL(iterator, EINA_MAGIC_ITERATOR);
        return EINA_FALSE;
     }

   r = sqlite3_step(it->base.stmt);
   if (r == SQLITE_DONE)
     return EINA_FALSE;
   if (r != SQLITE_ROW)
     {
        ERR("Error executing sql statement: %s",
            sqlite3_errmsg(it->base.db->handle));
        return EINA_FALSE;
     }

   it->nameid.id = sqlite3_column_int64(it->base.stmt, 0);
   it->nameid.name = (const char *)sqlite3_column_text(it->base.stmt, 1);
   it->nameid.len = sqlite3_column_bytes(it->base.stmt, 1);

   *nameid = &it->nameid;

   return EINA_TRUE;
}

Eina_Iterator *
db_albums_get(DB *db)
{
   struct DB_Iterator_NameID *it;
   EINA_SAFETY_ON_NULL_RETURN_VAL(db, NULL);
   it = calloc(1,  sizeof(*it));
   EINA_SAFETY_ON_NULL_RETURN_VAL(it, NULL);

   it->base.base.version = EINA_ITERATOR_VERSION;
   it->base.base.next = _db_iterator_nameid_next;
   it->base.base.get_container = _db_iterator_container_get;
   it->base.base.free = _db_iterator_free;
   it->base.db = db;
   it->base.stmt = db->stmt.albums_get;

   EINA_MAGIC_SET(&it->base.base, EINA_MAGIC_ITERATOR);
   return &it->base.base;
}

Eina_Iterator *
db_artists_get(DB *db)
{
   struct DB_Iterator_NameID *it;
   EINA_SAFETY_ON_NULL_RETURN_VAL(db, NULL);
   it = calloc(1,  sizeof(*it));
   EINA_SAFETY_ON_NULL_RETURN_VAL(it, NULL);

   it->base.base.version = EINA_ITERATOR_VERSION;
   it->base.base.next = _db_iterator_nameid_next;
   it->base.base.get_container = _db_iterator_container_get;
   it->base.base.free = _db_iterator_free;
   it->base.db = db;
   it->base.stmt = db->stmt.artists_get;

   EINA_MAGIC_SET(&it->base.base, EINA_MAGIC_ITERATOR);
   return &it->base.base;
}

Eina_Iterator *
db_genres_get(DB *db)
{
   struct DB_Iterator_NameID *it;
   EINA_SAFETY_ON_NULL_RETURN_VAL(db, NULL);
   it = calloc(1,  sizeof(*it));
   EINA_SAFETY_ON_NULL_RETURN_VAL(it, NULL);

   it->base.base.version = EINA_ITERATOR_VERSION;
   it->base.base.next = _db_iterator_nameid_next;
   it->base.base.get_container = _db_iterator_container_get;
   it->base.base.free = _db_iterator_free;
   it->base.db = db;
   it->base.stmt = db->stmt.genres_get;

   EINA_MAGIC_SET(&it->base.base, EINA_MAGIC_ITERATOR);
   return &it->base.base;
}



/*

TODO: filters:

// filter: free form string from user
// keep all searches with filter, not used now
// in future split on whitespace and search all terms in all fields (ie: even when listing artists, if term is Metallica, artist name would match and just "heavy metal" term will be shown
// possibly we'll take specifiers, like artist:metallica
// filters are case insensitive!


Eina_Bool db_path_del(DB *db, const char *path); // delete from ... where url like '$path/%', used to remove directories from media collection

// iterators return temporary struct and values straight from sqlite (= zero copy), use functions to copy when required:
Song *song_copy(const Song *);
Album *album_copy(const Album *);
Artist *artist_copy(const Artist *);

// use mempool + stringshare
void song_free(Song *);
void album_free(Album *);
void artist_free(Artist *);

Song *db_song_get(DB *db, id); // ret song
Eina_Bool db_song_rating_set(DB *db, Song *song, int rating);

Eina_Iterator *db_songs_get(DB *db); // ret song
Eina_Iterator *db_albums_get(DB *db); // ret albums (name, id)
Eina_Iterator *db_artists_get(DB *db); // ret artist (name, id)
Eina_Iterator *db_genres_get(DB *db); // ret genres (name, id)

Eina_Iterator *db_album_songs_get(DB *db, id); // ret all songs from album id

Eina_Iterator *db_artist_songs_get(DB *db, id); // ret all songs from artist id
Eina_Iterator *db_artist_albums_get(DB *db, id); // ret all albums (name, id) from artist id
Eina_Iterator *db_artist_album_songs_get(DB *db, id_artist, id_album); // ret all songs (name, id) from album from artist

Eina_Iterator *db_genre_songs_get(DB *db, id); // ret all songs from genre id
Eina_Iterator *db_genre_artists_get(DB *db, id); // ret all artists (name, id) from genre id
Eina_Iterator *db_genre_artist_albums_get(DB *db, id_genre, id_artist); // ret all albums from artist from genre
Eina_Iterator *db_genre_artist_album_songs_get(DB *db, id_genre, id_artist, id_album); // ret all songs from albums from artist from genre

*/
