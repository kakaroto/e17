#include "private.h"
#include <sqlite3.h>

struct _DB
{
   const char *path;
   sqlite3 *handle;
   struct {
      /* just pre-compile most used, no need to do so for rarely used
       * update statements like "set song playcnt".
       *
       * DO NOT pre-compile statements that are used from iterators to
       * populate huge lists, as the operations are asynchronous we
       * might have user asking to load one while the other is still
       * loading :-/
       */
      sqlite3_stmt *album_get;
      sqlite3_stmt *artist_get;
      sqlite3_stmt *genre_get;
   } stmt;
   Ecore_Timer *cleanup_timer;
};

static Eina_Bool
_db_stmt_bind_int64(sqlite3_stmt *stmt, int col, int64_t value)
{
   int r = sqlite3_bind_int64(stmt, col, value);
   if (r == SQLITE_OK)
     return EINA_TRUE;
   else
     {
        sqlite3 *db = sqlite3_db_handle(stmt);
        const char *err = sqlite3_errmsg(db);
        ERR("could not bind SQL value %lld to column %d: %s",
            (long long)value, col, err);
        return EINA_FALSE;
     }
}

static Eina_Bool
_db_stmt_bind_int(sqlite3_stmt *stmt, int col, int value)
{
   int r = sqlite3_bind_int(stmt, col, value);
   if (r == SQLITE_OK)
     return EINA_TRUE;
   else
     {
        sqlite3 *db = sqlite3_db_handle(stmt);
        const char *err = sqlite3_errmsg(db);
        ERR("could not bind SQL value %d to column %d: %s",
            value, col, err);
        return EINA_FALSE;
     }
}

static Eina_Bool
_db_stmt_bind_string(sqlite3_stmt *stmt, int col, char *value)
{
   int r = sqlite3_bind_text(stmt, col, value, -1, NULL);
   if (r == SQLITE_OK)
     return EINA_TRUE;
   else
     {
        sqlite3 *db = sqlite3_db_handle(stmt);
        const char *err = sqlite3_errmsg(db);
        ERR("could not bind SQL value %s to column %d: %s",
            value, col, err);
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

   C(album_get, "SELECT name FROM audio_albums WHERE id = ?");
   C(artist_get, "SELECT name FROM audio_artists WHERE id = ?");
   C(genre_get, "SELECT name FROM audio_genres WHERE id = ?");

#undef C
   return EINA_TRUE;
}

static Eina_Bool
_db_stmts_finalize(DB *db)
{
   Eina_Bool ret = EINA_TRUE;

#define F(m)                                            \
   ret &= _db_stmt_finalize(db->stmt.m, stringify(m));

   F(album_get);
   F(artist_get);
   F(genre_get);

#undef F
   return ret;
}

static void
_db_cover_table_ensure_exists(DB *db)
{
   static Eina_Bool created = EINA_FALSE;
   static const char *sql_create_table = "CREATE TABLE IF NOT EXISTS covers " \
                                         "(album_id integer, file_path text, " \
                                         "origin integer, width integer, " \
                                         "height integer, primary key" \
                                         "(album_id, file_path))";
   static const char *sql_create_trigger = "CREATE TRIGGER IF NOT EXISTS " \
                                           "delete_cover_on_album_deleted " \
                                           "DELETE ON audio_albums " \
                                           "BEGIN " \
                                           "DELETE FROM covers WHERE " \
                                           "album_id=OLD.id; " \
                                           "END";
   static const char *sql_create_index_path = "CREATE INDEX IF NOT EXISTS " \
                                              "cover_file_path_idx ON " \
                                              "covers (file_path)";
   static const char *sql_create_index_album_id = "CREATE INDEX IF NOT EXISTS " \
                                                  "cover_album_id_idx ON " \
                                                  "covers (album_id)";
   static const char *sql_add_version = "INSERT INTO lms_internal " \
                                         "(tab, version) " \
                                         "VALUES ('covers', 1)";
   if (created) return;
   sqlite3_exec(db->handle, sql_create_table, NULL, NULL, NULL);
   sqlite3_exec(db->handle, sql_create_trigger, NULL, NULL, NULL);
   sqlite3_exec(db->handle, sql_create_index_path, NULL, NULL, NULL);
   sqlite3_exec(db->handle, sql_create_index_album_id, NULL, NULL, NULL);
   sqlite3_exec(db->handle, sql_add_version, NULL, NULL, NULL);
   created = EINA_TRUE;
}

static void
_db_files_cleanup(DB *db)
{
   EINA_SAFETY_ON_NULL_RETURN(db);
   static const char *delete_old_files = "DELETE FROM files " \
                                         "WHERE"
                                         " dtime > 0 AND "
                                         " dtime < (strftime('%s', 'now') - (86400 * 7))";
   char *errmsg;
   int r;

   r = sqlite3_exec(db->handle, delete_old_files, NULL, NULL, &errmsg);
   if (r != SQLITE_OK)
     {
        ERR("Could not execute SQL %s: %s", delete_old_files, errmsg);
        sqlite3_free(errmsg);
     }
}

static void
_db_album_covers_cleanup(DB *db)
{
   EINA_SAFETY_ON_NULL_RETURN(db);
   static const char *create_temp_table = "CREATE TEMPORARY TABLE cover_cleanup " \
                                          "(file_path TEXT)";
   static const char *clean_temp_table = "DROP TABLE cover_cleanup";
   Eina_Iterator *files;
   Eina_File_Direct_Info *fi;
   sqlite3_stmt *stmt;
   char *errmsg;
   char *cache_dir;
   int r, cache_dir_len;

   cache_dir = enjoy_cache_dir_get();
   if (!cache_dir)
     {
        ERR("Could not get cache directory");
        return;
     }
   cache_dir_len = strlen(cache_dir);

   r = sqlite3_exec(db->handle, create_temp_table, NULL, NULL, &errmsg);
   if (r != SQLITE_OK)
     {
        ERR("Could not execute SQL %s: %s", create_temp_table, errmsg);
        sqlite3_free(errmsg);
        return;
     }

   stmt = _db_stmt_compile(db, "insert_files",
      "INSERT INTO cover_cleanup (file_path) VALUES (?)");
   if (!stmt) return;

   files = eina_file_direct_ls(enjoy_cache_dir_get());
   if (!files)
     {
        _db_stmt_finalize(stmt, "insert_files");
        ERR("Could not open cache directory");
        return;
     }

   EINA_ITERATOR_FOREACH(files, fi)
     {
        if (!_db_stmt_bind_string(stmt, 1, fi->path)) continue;
        sqlite3_step(stmt);
        _db_stmt_reset(stmt);
     }

   _db_stmt_finalize(stmt, "insert_files");

   stmt = _db_stmt_compile(db, "join_files",
      "SELECT cover_cleanup.file_path FROM covers LEFT OUTER JOIN cover_cleanup " \
      "ON covers.file_path = cover_cleanup.file_path " \
      "WHERE covers.file_path IS NULL");
   if (!stmt) goto end;

   while (sqlite3_step(stmt) == SQLITE_ROW)
     {
       char *path = (char *)sqlite3_column_text(stmt, 0);
       if (!strncmp(path, cache_dir, cache_dir_len)) ecore_file_remove(path);
     }
   _db_stmt_finalize(stmt, "join_files");

   r = sqlite3_exec(db->handle, clean_temp_table, NULL, NULL, &errmsg);
   if (r != SQLITE_OK)
     {
        ERR("Could not execute SQL %s: %s", clean_temp_table, errmsg);
        sqlite3_free(errmsg);
     }
end:
   eina_iterator_free(files);
}

static Eina_Bool
_db_cleanup_timer_cb(void *data)
{
   DB *db = data;
   _db_files_cleanup(db);
   _db_album_covers_cleanup(db);
   return ECORE_CALLBACK_RENEW;
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

   _db_cover_table_ensure_exists(db);

   db->cleanup_timer = ecore_timer_add(3600 * 4, _db_cleanup_timer_cb, db);
   if (!db->cleanup_timer)
     {
        ERR("could not create cleanup timer");
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
   ecore_timer_del(db->cleanup_timer);
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

   copy->flags = orig->flags;

   return copy;
}

void
db_song_free(Song *song)
{
   if (!song) return;
   eina_stringshare_del(song->path);
   eina_stringshare_del(song->title);
   eina_stringshare_del(song->album);
   eina_stringshare_del(song->artist);
   free(song);
}

Song *
db_song_get(DB *db, int64_t id)
{
   EINA_SAFETY_ON_NULL_RETURN_VAL(db, NULL);
   sqlite3_stmt *song_get;
   Song *song;

   song_get = _db_stmt_compile(db, "song_get",
      "SELECT files.id, files.path, files.size, "
      " audios.title, audios.album_id, audios.artist_id, audios.genre_id, "
      " audios.trackno, audios.rating, audios.playcnt, audios.length "
      "FROM audios, files "
      "WHERE "
      " files.id = audios.id "
      "AND audios.id = ?");
   if (!song_get) return NULL;
   if (!_db_stmt_bind_int64(song_get, 1, id)) goto cleanup;
   if (sqlite3_step(song_get) != SQLITE_ROW) goto cleanup;
   song = calloc(1, sizeof(*song));
   if (!song) goto cleanup;

#define ID(m, c)                                                        \
   song->m = sqlite3_column_int64(song_get, c);
#define INT(m, c)                                                       \
   song->m = sqlite3_column_int(song_get, c);
#define STR(m, c)                                                       \
   song->m = (const char *)sqlite3_column_text(song_get, c);            \
   song->m = eina_stringshare_add(song->m);                             \
   song->len.m = sqlite3_column_bytes(song_get, c)

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

   return song;
cleanup:
  _db_stmt_finalize(song_get, "song_get");
  return NULL;
}

struct DB_Iterator
{
   Eina_Iterator base;
   DB *db;
   const char *stmt_name;
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
   _db_stmt_finalize(it->stmt, it->stmt_name);
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
   it->base.stmt_name = "songs_get";
   it->base.stmt = _db_stmt_compile
     (db, it->base.stmt_name,
      "SELECT files.id, files.path, files.size, "
      " audios.title, audios.album_id, audios.artist_id, audios.genre_id, "
      " audios.trackno, audios.rating, audios.playcnt, audios.length "
      "FROM audios, files "
      "WHERE "
      " files.id = audios.id AND "
      " files.dtime = 0 "
      "ORDER BY UPPER(audios.title)");
   if (!it->base.stmt)
     {
        free(it);
        return NULL;
     }

   EINA_MAGIC_SET(&it->base.base, EINA_MAGIC_ITERATOR);
   return &it->base.base;
}

Eina_Bool
db_song_rating_set(DB *db, Song *song, int rating)
{
   char sql[128], *errmsg = NULL;
   int r;

   EINA_SAFETY_ON_NULL_RETURN_VAL(db, EINA_FALSE);
   EINA_SAFETY_ON_NULL_RETURN_VAL(song, EINA_FALSE);

   sqlite3_snprintf(sizeof(sql), sql,
                    "UPDATE audios SET rating = %d WHERE id = %lld",
                    rating, song->id);

   r = sqlite3_exec(db->handle, sql, NULL, NULL, &errmsg);
   if (r != SQLITE_OK)
     {
        ERR("Could not execute SQL %s: %s", sql, errmsg);
        sqlite3_free(errmsg);
        return EINA_FALSE;
     }
   song->rating = rating;
   return EINA_TRUE;
}

Eina_Bool
db_song_length_set(DB *db, Song *song, int length)
{
   char sql[128], *errmsg = NULL;
   int r;

   EINA_SAFETY_ON_NULL_RETURN_VAL(db, EINA_FALSE);
   EINA_SAFETY_ON_NULL_RETURN_VAL(song, EINA_FALSE);

   sqlite3_snprintf(sizeof(sql), sql,
                    "UPDATE audios SET length = %d WHERE id = %lld",
                    length, song->id);

   r = sqlite3_exec(db->handle, sql, NULL, NULL, &errmsg);
   if (r != SQLITE_OK)
     {
        ERR("Could not execute SQL %s: %s", sql, errmsg);
        sqlite3_free(errmsg);
        return EINA_FALSE;
     }
   song->length = length;
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
   it->base.stmt_name = "album_songs_get";
   it->base.stmt = _db_stmt_compile
     (db, it->base.stmt_name,
      "SELECT files.id, files.path, files.size, "
      " audios.title, audios.album_id, audios.artist_id, audios.genre_id, "
      " audios.trackno, audios.rating, audios.playcnt, audios.length "
      "FROM audios, files "
      "WHERE "
      " files.id = audios.id AND "
      " audios.album_id = ? AND "
      " files.dtime = 0 "
      "ORDER BY audios.trackno, UPPER(audios.title)");
   if (!it->base.stmt)
     {
        free(it);
        return NULL;
     }

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
   it->base.stmt_name = "artist_songs_get";
   it->base.stmt = _db_stmt_compile
     (db, it->base.stmt_name,
      "SELECT files.id, files.path, files.size, "
      " audios.title, audios.album_id, audios.artist_id, audios.genre_id, "
      " audios.trackno, audios.rating, audios.playcnt, audios.length "
      "FROM audios, files "
      "WHERE "
      " files.id = audios.id AND "
      " audios.artist_id = ? AND "
      " files.dtime = 0 "
      "ORDER BY UPPER(audios.title)");
   if (!it->base.stmt)
     {
        free(it);
        return NULL;
     }

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
   it->base.stmt_name = "genre_songs_get";
   it->base.stmt = _db_stmt_compile
     (db, it->base.stmt_name,
      "SELECT files.id, files.path, files.size, "
      " audios.title, audios.album_id, audios.artist_id, audios.genre_id, "
      " audios.trackno, audios.rating, audios.playcnt, audios.length "
      "FROM audios, files "
      "WHERE "
      " files.id = audios.id AND "
      " audios.genre_id = ? AND "
      " files.dtime = 0 "
      "ORDER BY UPPER(audios.title)");
   if (!it->base.stmt)
     {
        free(it);
        return NULL;
     }

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

Album *
db_album_copy(const Album *orig)
{
   Album *copy;
   const Album_Cover *orig_cover;

   EINA_SAFETY_ON_NULL_RETURN_VAL(orig, NULL);
   copy = malloc(sizeof(Album));
   EINA_SAFETY_ON_NULL_RETURN_VAL(copy, NULL);
   copy->id = orig->id;
   copy->artist_id = orig->artist_id;
   copy->name = eina_stringshare_add(orig->name);
   copy->artist = eina_stringshare_add(orig->artist);
   copy->len.name = orig->len.name;
   copy->len.artist = orig->len.artist;
   copy->flags = orig->flags;

   copy->covers = NULL;
   EINA_INLIST_FOREACH(orig->covers, orig_cover)
     {
        Album_Cover *copy_cover;

        copy_cover = malloc(sizeof(Album_Cover) + orig_cover->path_len + 1);
        if (!copy_cover) break;
        copy_cover->w = orig_cover->w;
        copy_cover->h = orig_cover->h;
        copy_cover->path_len = orig_cover->path_len;
        memcpy(copy_cover->path, orig_cover->path, orig_cover->path_len + 1);
        copy->covers = eina_inlist_append
          (copy->covers, EINA_INLIST_GET(copy_cover));
     }

   return copy;
}

void
db_album_free(Album *album)
{
   while (album->covers)
     {
        void *n = album->covers;
        album->covers = eina_inlist_remove(album->covers, album->covers);
        free(n);
     }

   eina_stringshare_del(album->name);
   eina_stringshare_del(album->artist);
   free(album);
}

Eina_Bool
db_album_artist_fetch(DB *db, Album *album)
{
   sqlite3_stmt *stmt;
   Eina_Bool ret;
   int err;

   EINA_SAFETY_ON_NULL_RETURN_VAL(db, EINA_FALSE);
   EINA_SAFETY_ON_NULL_RETURN_VAL(album, EINA_FALSE);
   if (album->flags.fetched_artist) return EINA_TRUE;

   stmt = db->stmt.artist_get;
   if (!_db_stmt_bind_int64(stmt, 1, album->artist_id))
     return EINA_FALSE;

   err = sqlite3_step(stmt);
   if (err == SQLITE_ROW)
     {
        eina_stringshare_replace
          (&album->artist, (const char *)sqlite3_column_text(stmt, 0));
        album->len.artist = sqlite3_column_bytes(stmt, 0);
        ret = EINA_TRUE;
     }
   else if (err == SQLITE_DONE)
     {
        DBG("no artist with id=%lld", (long long)album->artist_id);
        eina_stringshare_replace(&album->artist, NULL);
        album->len.artist = 0;
        ret = EINA_TRUE;
     }
   else
     {
        ERR("could not query artist with id=%lld: %s",
            (long long)album->artist_id, sqlite3_errmsg(db->handle));
        ret = EINA_FALSE;
     }

   _db_stmt_reset(stmt);
   album->flags.fetched_artist = ret;
   return ret;
}

Eina_Bool
db_album_covers_fetch(DB *db, Album *album)
{
   EINA_SAFETY_ON_NULL_RETURN_VAL(db, EINA_FALSE);
   EINA_SAFETY_ON_NULL_RETURN_VAL(album, EINA_FALSE);
   sqlite3_stmt *covers_get;

   if (album->flags.fetched_covers) return EINA_TRUE;

   covers_get = _db_stmt_compile(db, "covers_get",
      "SELECT file_path, origin, width, height, album_id "
      "FROM covers WHERE album_id = ?");
   if (!covers_get) return EINA_FALSE;
   if (!_db_stmt_bind_int64(covers_get, 1, album->id)) goto cleanup;

   while (sqlite3_step(covers_get) == SQLITE_ROW)
     {
        char *path;
        int path_len;
        Album_Cover *cover;

        path_len = sqlite3_column_bytes(covers_get, 0);
        if (path_len <= 0) continue;
        cover = malloc(sizeof(*cover) + path_len + 1);
        if (!cover) continue;

        cover->origin = sqlite3_column_int(covers_get, 1);
        cover->w = sqlite3_column_int(covers_get, 2);
        cover->h = sqlite3_column_int(covers_get, 3);
        cover->path_len = path_len;
        path = (char *)sqlite3_column_text(covers_get, 0);
        memcpy(cover->path, path, path_len + 1);

        album->covers = eina_inlist_append(album->covers,
                                           EINA_INLIST_GET(cover));
     }

cleanup:
   _db_stmt_finalize(covers_get, "covers_get");
   album->flags.fetched_covers = !!eina_inlist_count(album->covers);
   return album->flags.fetched_covers;
}

Eina_Bool
db_album_covers_update(DB *db, const Album *album)
{
   EINA_SAFETY_ON_NULL_RETURN_VAL(db, EINA_FALSE);
   EINA_SAFETY_ON_NULL_RETURN_VAL(album, EINA_FALSE);
   Album_Cover *cover;
   Eina_Bool retval = EINA_FALSE;
   sqlite3_stmt *stmt;

   if (EINA_UNLIKELY(!album->covers))
     {
        stmt = _db_stmt_compile(db, "covers_get",
           "SELECT file_path FROM covers WHERE album_id = ?");
        if (!stmt) return EINA_FALSE;
        if (!_db_stmt_bind_int64(stmt, 1, album->id))
          {
             _db_stmt_finalize(stmt, "covers_get");
             return EINA_FALSE;
          }
        while (sqlite3_step(stmt) == SQLITE_ROW)
          ecore_file_remove((char *)sqlite3_column_text(stmt, 0));
        _db_stmt_finalize(stmt, "covers_get");

        stmt = _db_stmt_compile(db, "covers_delete",
           "DELETE FROM covers WHERE album_id = ?");
        if (!stmt) return EINA_FALSE;
        if (!_db_stmt_bind_int64(stmt, 1, album->id))
          {
             _db_stmt_finalize(stmt, "covers_delete");
             return EINA_FALSE;
          }
        retval = sqlite3_step(stmt) == SQLITE_DONE;
        _db_stmt_finalize(stmt, "covers_delete");
        return retval;
     }

   stmt = _db_stmt_compile(db, "covers_update",
            "INSERT OR REPLACE INTO covers "
            "(album_id, file_path, origin, width, height) "
            "VALUES (?, ?, ?, ?, ?)");
   if (!stmt) return EINA_FALSE;

   EINA_INLIST_FOREACH(album->covers, cover)
     {
        if (!_db_stmt_bind_int64(stmt, 1, album->id)) goto reset;
        if (!_db_stmt_bind_string(stmt, 2, cover->path)) goto reset;
        if (!_db_stmt_bind_int(stmt, 3, cover->origin)) goto reset;
        if (!_db_stmt_bind_int(stmt, 4, cover->w)) goto reset;
        if (!_db_stmt_bind_int(stmt, 5, cover->h)) goto reset;
        if (sqlite3_step(stmt) != SQLITE_ERROR) retval = EINA_TRUE;
   reset:
        sqlite3_reset(stmt);
        sqlite3_clear_bindings(stmt);
     }

   _db_stmt_finalize(stmt, "covers_update");
   return retval;
}


struct DB_Iterator_Album
{
   struct DB_Iterator base;
   Album album;
};

static Eina_Bool
_db_iterator_album_next(Eina_Iterator *iterator, void **data)
{
   struct DB_Iterator_Album *it = (struct DB_Iterator_Album *)iterator;
   Album **album = (Album **)data;
   int r;

   EINA_SAFETY_ON_NULL_RETURN_VAL(album, EINA_FALSE);
   *album = NULL;
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

   it->album.id = sqlite3_column_int64(it->base.stmt, 0);
   it->album.artist_id = sqlite3_column_int64(it->base.stmt, 1);
   it->album.name = (const char *)sqlite3_column_text(it->base.stmt, 2);
   it->album.len.name = sqlite3_column_bytes(it->base.stmt, 2);

   *album = &it->album;

   return EINA_TRUE;
}

Eina_Iterator *
db_albums_get(DB *db)
{
   struct DB_Iterator_Album *it;
   EINA_SAFETY_ON_NULL_RETURN_VAL(db, NULL);
   it = calloc(1,  sizeof(*it));
   EINA_SAFETY_ON_NULL_RETURN_VAL(it, NULL);

   it->base.base.version = EINA_ITERATOR_VERSION;
   it->base.base.next = _db_iterator_album_next;
   it->base.base.get_container = _db_iterator_container_get;
   it->base.base.free = _db_iterator_free;
   it->base.db = db;
   it->base.stmt_name = "albums_get";
   it->base.stmt = _db_stmt_compile
     (db, it->base.stmt_name,
      "SELECT audio_albums.id, audio_albums.artist_id, audio_albums.name "
      "FROM audio_albums, files, audios "
      "WHERE "
      " audios.id = files.id AND "
      " audios.album_id = audio_albums.id AND "
      " files.dtime = 0 "
      "GROUP BY audio_albums.id "
      "ORDER BY UPPER(name)");
   if (!it->base.stmt)
     {
        free(it);
        return NULL;
     }

   EINA_MAGIC_SET(&it->base.base, EINA_MAGIC_ITERATOR);
   return &it->base.base;
}

Eina_Iterator *
db_artist_albums_get(DB *db, int64_t artist_id)
{
   struct DB_Iterator_Album *it;
   EINA_SAFETY_ON_NULL_RETURN_VAL(db, NULL);
   it = calloc(1,  sizeof(*it));
   EINA_SAFETY_ON_NULL_RETURN_VAL(it, NULL);

   it->base.base.version = EINA_ITERATOR_VERSION;
   it->base.base.next = _db_iterator_album_next;
   it->base.base.get_container = _db_iterator_container_get;
   it->base.base.free = _db_iterator_free;
   it->base.db = db;
   it->base.stmt_name = "artist_albums_get";
   it->base.stmt = _db_stmt_compile
     (db, it->base.stmt_name,
      "SELECT id, artist_id, name FROM audio_albums "
      "WHERE artist_id = ? ORDER BY UPPER(name)");
   if (!it->base.stmt)
     {
        free(it);
        return NULL;
     }

   if (!_db_stmt_bind_int64(it->base.stmt, 1, artist_id))
     {
        free(it);
        return NULL;
     }

   EINA_MAGIC_SET(&it->base.base, EINA_MAGIC_ITERATOR);
   return &it->base.base;
}

Eina_Iterator *
db_genre_albums_get(DB *db, int64_t genre_id)
{
   struct DB_Iterator_Album *it;
   EINA_SAFETY_ON_NULL_RETURN_VAL(db, NULL);
   it = calloc(1,  sizeof(*it));
   EINA_SAFETY_ON_NULL_RETURN_VAL(it, NULL);

   it->base.base.version = EINA_ITERATOR_VERSION;
   it->base.base.next = _db_iterator_album_next;
   it->base.base.get_container = _db_iterator_container_get;
   it->base.base.free = _db_iterator_free;
   it->base.db = db;
   it->base.stmt_name = "genre_albums_get";
   it->base.stmt = _db_stmt_compile
     (db, it->base.stmt_name,
      "SELECT DISTINCT "
      " audio_albums.id, audio_albums.artist_id, audio_albums.name "
      "FROM audios, audio_albums "
      "WHERE audios.album_id = audio_albums.id "
      "AND audios.genre_id = ? "
      "ORDER BY UPPER(audio_albums.name)");
   if (!it->base.stmt)
     {
        free(it);
        return NULL;
     }

   if (!_db_stmt_bind_int64(it->base.stmt, 1, genre_id))
     {
        free(it);
        return NULL;
     }

   EINA_MAGIC_SET(&it->base.base, EINA_MAGIC_ITERATOR);
   return &it->base.base;
}

NameID *
db_nameid_copy(const NameID *orig)
{
   NameID *copy;
   EINA_SAFETY_ON_NULL_RETURN_VAL(orig, NULL);
   copy = calloc(1, sizeof(NameID));
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
   it->base.stmt_name = "artists_get";
   it->base.stmt = _db_stmt_compile
     (db, it->base.stmt_name,
      "SELECT id, name FROM audio_artists ORDER BY UPPER(name)");
   if (!it->base.stmt)
     {
        free(it);
        return NULL;
     }

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
   it->base.stmt_name = "genres_get";
   it->base.stmt = _db_stmt_compile
     (db, it->base.stmt_name,
      "SELECT id, name FROM audio_genres ORDER BY UPPER(name)");
   if (!it->base.stmt)
     {
        free(it);
        return NULL;
     }

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
