/*
 * enna_scanner.c
 * Copyright (C) Nicolas Aguirre 2006,2007,2008 <aguirre.nicolas@gmail.com>
 *
 * enna_scanner.c is free software copyrighted by Nicolas Aguirre.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name ``Nicolas Aguirre'' nor the name of any other
 *    contributor may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * enna_scanner.c IS PROVIDED BY Nicolas Aguirre ``AS IS'' AND ANY EXPRESS
 * OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL Nicolas Aguirre OR ANY OTHER CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
 * BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <stdio.h>
#include <string.h>
#include <Ecore_File.h>
#include <Ecore_Str.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <Evas.h>
#include <unistd.h>
#include <pthread.h>
#include <lightmediascanner.h>
#include <sqlite3.h>
#include <musicbrainz/queries.h>
#include <musicbrainz/mb_c.h>

#include "enna_scanner.h"
#include "enna.h"
#include "enna_config.h"
#include "enna_util.h"

typedef struct _Enna_Scanner Enna_Scanner;

struct _Enna_Scanner
{
  unsigned int     commit_interval;
  unsigned int     slave_timeout;
  const char      *db_path;
  Ecore_List      *parsers;
  const char      *charset;
  const char      *scan_path;
  lms_t           *lms;
  sqlite3         *db;
  int             fd_ev_read;
  int             fd_ev_write;
  Ecore_Fd_Handler *fd_ev_handler;
  pthread_t        scanner_thread;
  
};


static Enna_Scanner *scanner;

/* Local functions */
static sqlite3 *_db_open(const char *filename);
static sqlite3_stmt * _db_compile_stmt(sqlite3 *db, const char *sql);
static int _db_finalize_stmt(sqlite3_stmt *stmt, const char *name);
static int _db_bind_blob(sqlite3_stmt *stmt, int col, const void *blob, int len);
static int _db_bind_text(sqlite3_stmt *stmt, int col, const char *text, int len);
static void _metadata_print(Enna_Metadata * metadata);

static sqlite3 * _db_open(const char *filename)
{
     
   sqlite3 *db;
   
    if (sqlite3_open(filename, &db) != SQLITE_OK) {
        dbg("ERROR: could not open DB \"%s\": %s\n",
                filename, sqlite3_errmsg(db));
	sqlite3_close(db);
	return NULL;
    }
    return db;
}

int
_db_bind_blob(sqlite3_stmt *stmt, int col, const void *blob, int len)
{
    int r;

    if (blob)
        r = sqlite3_bind_blob(stmt, col, blob, len, SQLITE_STATIC);
    else
        r = sqlite3_bind_null(stmt, col);

    if (r == SQLITE_OK)
        return 0;
    else {
        sqlite3 *db;
        const char *err;

        db = sqlite3_db_handle(stmt);
        err = sqlite3_errmsg(db);
        fprintf(stderr, "ERROR: could not bind SQL value %d: %s\n", col, err);
        return -col;
    }
}

int
_db_bind_text(sqlite3_stmt *stmt, int col, const char *text, int len)
{
    int r;

    if (text)
        r = sqlite3_bind_text(stmt, col, text, len, SQLITE_STATIC);
    else
        r = sqlite3_bind_null(stmt, col);

    if (r == SQLITE_OK)
        return 0;
    else {
        sqlite3 *db;
        const char *err;

        db = sqlite3_db_handle(stmt);
        err = sqlite3_errmsg(db);
        fprintf(stderr, "ERROR: could not bind SQL value %d: %s\n", col, err);
        return -col;
    }
}

sqlite3_stmt *
_db_compile_stmt(sqlite3 *db, const char *sql)
{
    sqlite3_stmt *stmt;

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK)
      dbg("ERROR: could not prepare \"%s\": %s\n", sql,
	  sqlite3_errmsg(db));

    return stmt;
}

int
_db_reset_stmt(sqlite3_stmt *stmt)
{
    int r, ret;

    ret = r = sqlite3_reset(stmt);
    if (r != SQLITE_OK)
        fprintf(stderr, "ERROR: could not reset SQL statement: #%d\n", r);

    r = sqlite3_clear_bindings(stmt);
    ret += r;
    if (r != SQLITE_OK)
        fprintf(stderr, "ERROR: could not clear SQL: #%d\n", r);

    return ret;
}

int
_db_finalize_stmt(sqlite3_stmt *stmt, const char *name)
{
    int r;

    r = sqlite3_finalize(stmt);
    if (r != SQLITE_OK) {
        dbg("ERROR: could not finalize %s statement: #%d\n",
                name, r);
        return -1;
    }

    return 0;
}


static int
_cover_download(const char *artist, const char *album)
{
   musicbrainz_t       o, o2;
   char                error[256], data[256], temp[256], *args[2];
   int                 ret;
   int                 numAlbums = 0;
   char                asin[256];
   char                cd_info[256];
   int                 i;

   o = mb_New();
   /* UTF-8 */
   mb_UseUTF8(o, 1);

   /* Set the proper server to use. Defaults to mm.musicbrainz.org:80 */
   if (getenv("MB_SERVER"))
      mb_SetServer(o, getenv("MB_SERVER"), 80);

   /* Check to see if the debug env var has been set */
   if (getenv("MB_DEBUG"))
      mb_SetDebug(o, atoi(getenv("MB_DEBUG")));

   /* Tell the server to only return 2 levels of data, unless the MB_DEPTH env var is set */
   if (getenv("MB_DEPTH"))
      mb_SetDepth(o, atoi(getenv("MB_DEPTH")));
   else
      mb_SetDepth(o, 4);

   args[0] = (char*)album;
   args[1] = NULL;
   /* Execute the MB_FindAlbumByName query */
   ret = mb_QueryWithArgs(o, MBQ_FindAlbumByName, args);

   if (!ret)
     {
	mb_GetQueryError(o, error, 256);
	dbg("Query failed: %s\n", error);
	mb_Delete(o);
	return 0;
     }

   /* Check to see how many items were returned from the server */
   numAlbums = mb_GetResultInt(o, MBE_GetNumAlbums);
   if (numAlbums < 1)
     {
	dbg("No albums found.\n");
	mb_Delete(o);
	return 0;
     }

   /*if ( numAlbums == 1)
    * //for(i = 1; i <= numAlbums; i++)
    * {
    * // Start at the top of the query and work our way down
    * mb_Select(o, MBS_Rewind);
    * 
    * // Select the ith album
    * mb_Select1(o, MBS_SelectAlbum, 1);
    * 
    * // Extract the album id from the ith track
    * mb_GetResultData(o, MBE_AlbumGetAlbumId, data, 256);
    * mb_GetIDFromURL(o, data, cd_info, 256);
    * printf("  AlbumId: '%s'\n", cd_info);
    * }
    * else */
   {
      for (i = 1; i <= numAlbums; i++)
	{
	   mb_Select(o, MBS_Rewind);
	   mb_Select1(o, MBS_SelectAlbum, i);
	   /* More than one album found */
	   /* Try to search the first album of our artist */
	   mb_GetResultData(o, MBE_AlbumGetAlbumId, data, 256);
	   mb_GetIDFromURL(o, data, cd_info, 256);

	   /* Extract the artist id from the ith track */

	   mb_GetResultData(o, MBE_AlbumGetAlbumArtistId, data, 256);
	   mb_GetIDFromURL(o, data, temp, 256);

	   o2 = mb_New();
	   // Tell the client library to return data in UTF-8
	   mb_UseUTF8(o2, 1);

	   args[0] = temp;
	   args[1] = NULL;

	   ret = mb_QueryWithArgs(o2, MBQ_GetArtistById, args);
	   if (!ret)
	     {
		mb_GetQueryError(o2, error, 256);
		dbg("Query failed: %s\n", error);
		mb_Delete(o);
		mb_Delete(o2);
		return 0;
	     }
	   mb_Select1(o2, MBS_SelectArtist, 1);
	   if (!mb_GetResultData(o2, MBE_ArtistGetArtistName, data, 256))
	     {
		dbg("Artist Name not found\n");
		mb_Delete(o);
		mb_Delete(o2);
		return 0;
	     }

	   if (!strcasecmp(data, artist))
	     {
		mb_GetResultData(o, MBE_AlbumGetAlbumId, data, 256);
		mb_GetIDFromURL(o, data, cd_info, 256);
		mb_Delete(o2);
		break;

	     }
	}
      //mb_Delete(o);
      //return 0;
   }
   /****************************************************************************/

   // Set up the args for the find album query
   args[0] = cd_info;
   args[1] = NULL;

   if (strlen(cd_info) != MB_CDINDEX_ID_LEN)
      // Execute the MB_GetAlbumById query
      ret = mb_QueryWithArgs(o, MBQ_GetAlbumById, args);
   else
      // Execute the MBQ_GetCDInfoFromCDIndexId
      ret = mb_QueryWithArgs(o, MBQ_GetCDInfoFromCDIndexId, args);

   if (!ret)
     {
	mb_GetQueryError(o, error, 256);
	dbg("Query failed: %s\n", error);
	mb_Delete(o);
	return 0;
     }

   //if(albumNum == 1)
   {
      // Select the first album
      if (!mb_Select1(o, MBS_SelectAlbum, 1))
	{
	   mb_Delete(o);
	   return 0;
	}
      // Pull back the album id to see if we got the album
      if (!mb_GetResultData(o, MBE_AlbumGetAlbumId, data, 256))
	{
	   mb_Delete(o);
	   dbg("Album not found.\n");
	   return 0;
	}

      mb_GetIDFromURL(o, data, temp, 256);

      // Extract the amazon asin, if any
      if (!mb_GetResultData(o, MBE_AlbumGetAmazonAsin, asin, 256))
	{
	   
	   mb_Delete(o);
	   return 0;
	}
      else
	dbg("asin found : %s\n", asin);
      
      mb_Select(o, MBS_Back);

   }
   /*else
    * {
    * return 0;
    * // and clean up the musicbrainz object
    * mb_Delete(o);
    * }
    */

   mb_Delete(o);

   sprintf(data, "http://images.amazon.com/images/P/%s.01.LZZZZZZZ.jpg", asin);
   dbg("data : %s\n", data);
   
   sprintf(temp, "%s/.enna/covers/%s - %s.jpg", enna_util_user_home_get(), artist,
	   album);

   ecore_file_download(data, temp, NULL, NULL, NULL);

}


static void 
* _scanner_thread(void *ptr)
{
   Enna_Scanner *scanner;
   void         *buf[1];
   int r, ret;
   sqlite3_stmt *stmt;

   scanner = (Enna_Scanner *) ptr;
 
   lms_check(scanner->lms, scanner->scan_path);  
   /*  Start Scan process */
   lms_process(scanner->lms, scanner->scan_path);
   /*  Scann process is done */
   /* Get albums/artists list and try to download covers */
   stmt = _db_compile_stmt(scanner->db, "SELECT audio_albums.name,audio_artists.name "
			                "FROM audio_albums,audio_artists "
			                "WHERE audio_albums.artist_id=audio_artists.id;");
   if (!stmt)
     goto done;
   
   while ( sqlite3_step ( stmt ) == SQLITE_ROW )
    { 
       char *artist, *album, cover_filename[PATH_MAX];
       artist = (char*)sqlite3_column_text(stmt,0);
       album  = (char*)sqlite3_column_text(stmt,1);
       sprintf(cover_filename, "%s/.enna/covers/%s - %s.jpg", enna_util_user_home_get(), artist, album);
       if (ecore_file_exists(cover_filename))
	 {
	    dbg("cover exists : %s\n", cover_filename);
	 }
       else
	 _cover_download(artist,album);
    }

 done:
   _db_reset_stmt(stmt);
   dbg("Scanner Thread Done\n");
   buf[0] = "done";
   write(scanner->fd_ev_write, buf, sizeof(buf));
   return NULL;
}

static int
_pipe_read_active(void *data, Ecore_Fd_Handler * fdh)
{
   Enna_Scanner *scanner;

   scanner = (Enna_Scanner *) data;
   pthread_join(scanner->scanner_thread, NULL);
   dbg("Scanner Thread ended\n");
   return 0;
}

EAPI void
enna_scanner_init(Enna * enna)
{
   char           tmp[PATH_MAX];
   char           db_filename[PATH_MAX];
   lms_plugin_t  *parser;
   int            fds[2];

   /* Create scanner for audio only */
   scanner = malloc(sizeof( Enna_Scanner));
   if (!scanner) return;
   /* Commit changes on db each 100 medias */
   scanner->commit_interval = 100;
   scanner->slave_timeout = 1000; /* 1sec */
   /* Set db filename */
   sprintf(tmp, "%s/.enna/%s", enna_util_user_home_get(), "enna_music.db");
   scanner->db_path = evas_stringshare_add(tmp) ;
   /* Set charset to UTF-8 */
   scanner->charset = evas_stringshare_add("iso-8859-1");
   /* Set base path */
   scanner->scan_path = enna_config_get_conf_value_or_default("music_module",
							      "base_path", "/");
   /* Create lms, set params and start scann process */
   scanner->lms = lms_new(scanner->db_path);
   if (!scanner->lms)
     goto error;

   lms_set_commit_interval(scanner->lms, scanner->commit_interval);
   lms_set_slave_timeout(scanner->lms, scanner->slave_timeout);
   
   lms_parser_find_and_add(scanner->lms, "id3");
   lms_parser_find_and_add(scanner->lms, "asf");
   lms_parser_find_and_add(scanner->lms, "rm");
   lms_parser_find_and_add(scanner->lms, "audio-dummy");
   lms_parser_find_and_add(scanner->lms, "ogg");
   
   if (lms_charset_add(scanner->lms, scanner->charset) != 0)
     goto error;

   if (pipe(fds) == 0)
     {
	scanner->fd_ev_read = fds[0];
	scanner->fd_ev_write = fds[1];
	fcntl(scanner->fd_ev_read, F_SETFL, O_NONBLOCK);
	scanner->fd_ev_handler = ecore_main_fd_handler_add(scanner->fd_ev_read,
							   ECORE_FD_READ,
							   _pipe_read_active,
							   scanner, NULL, NULL);
	ecore_main_fd_handler_active_set(scanner->fd_ev_handler, ECORE_FD_READ);
	pthread_create(&scanner->scanner_thread, NULL, _scanner_thread,
		       (void *)scanner);
     }

   sprintf(db_filename, "%s/.enna/%s", enna->home, "enna_music.db");
   scanner->db = _db_open(db_filename);

   /*enna_scanner_nb_medias_get(0);
   enna_scanner_audio_nb_albums_get();
   enna_scanner_audio_nb_artists_get();
   enna_scanner_audio_nb_genres_get();
   enna_scanner_audio_metadata_get("/media/serveur/Medias/Musique/Air/Moon Safari/01 - La femme d'argent.mp3");
   */
   return;
   

 error: 
   lms_free(scanner->lms);
   evas_stringshare_del(scanner->db_path);
   evas_stringshare_del(scanner->charset);
   ENNA_FREE(scanner);
}

static void
_metadata_print(Enna_Metadata * metadata)
{
#if 0
   dbg("---------\n");
   dbg("\n\tfile:\t%s\n\ttitle:\t%s\n\tartist:\t%s\n\talbum:\t%s\n\tgenre:\t%s\n"
       "\ttrack\t%d\n\tplay count:\t%d\n",
       metadata->uri ? metadata->uri : "Unknown",
       metadata->title[0] ? metadata->title : "Unknown Title",
       metadata->artist[0] ? metadata->artist : "Unknown Artist",
       metadata->album[0] ? metadata->album : "Unknown Album",
       metadata->genre[0] ? metadata->genre : "Unknown Genre",
       metadata->track_nb,
       metadata->play_count);
   dbg("----------\n");
#endif
}

EAPI void
enna_scanner_shutdown()
{
   lms_stop_processing(scanner->lms);
   //pthread_join(scanner->scanner_thread, NULL);   
   lms_free(scanner->lms);
   evas_stringshare_del(scanner->db_path);
   evas_stringshare_del(scanner->charset);
   ENNA_FREE(scanner);
}

EAPI int enna_scanner_nb_medias_get(int type)
{    
   int r, ret;
   sqlite3_stmt *stmt;
   int nb = 0;
   
   stmt = _db_compile_stmt(scanner->db, "SELECT COUNT(*) FROM files");
   if (!stmt)
     return -1;

   r = sqlite3_step(stmt);
   if (r == SQLITE_DONE) {
      ret = 1;
      goto done;
   }
   nb = sqlite3_column_int(stmt, 0);
   ret = 0;
   dbg("Nb of medias in files db : %d\n", nb);
 done:
   _db_reset_stmt(stmt);
   return ret;
}


EAPI char *enna_scanner_cover_get(const char *album, const char *artist)
{
   char cover_filename[PATH_MAX];

   if (!artist || !album)
     return;

   sprintf(cover_filename, "%s/.enna/covers/%s - %s.jpg", enna_util_user_home_get(), artist, album);
   if (ecore_file_exists(cover_filename))
     return strdup(cover_filename);
   else
     return NULL;
}

EAPI Enna_Metadata *enna_scanner_audio_metadata_get(const char *filename)
{

   int r, ret;
   sqlite3_stmt *stmt;
   int nb = 0;
   int id;
   Enna_Metadata *m;
   
   m = malloc(sizeof(Enna_Metadata));
   
   stmt = _db_compile_stmt(scanner->db, "SELECT files.id, "
                                       "files.path, "
                                       "audios.title, "
                                       "audio_albums.name, "
                                       "audio_genres.name, "
                                       "audio_artists.name, "
                                       "files.size, "
                                       "audios.trackno, "
                                       "audios.rating, "
                                       "audios.playcnt "
                                "FROM files, "
                                     "audios, "
                                     "audio_artists, "
                                     "audio_albums, "
                                     "audio_genres "
                                "WHERE files.id=audios.id AND "
                                      "audios.album_id=audio_albums.id AND "
                                      "audios.genre_id=audio_genres.id AND " 
                                      "audio_albums.artist_id=audio_artists.id AND "
                                      "path like ?");
   if (!stmt)
     return NULL;
   
   ret = _db_bind_blob(stmt, 1, filename, strlen(filename));
   if (ret != 0)
     goto done;

   r = sqlite3_step(stmt);
   if (r == SQLITE_DONE) {
      fprintf(stderr, "ERROR: could not get file info from table: %s\n",
	      sqlite3_errmsg(sqlite3_db_handle(stmt)));
      ret = 1;
      goto done;
   }
 
   id = sqlite3_column_int(stmt, 0);
   m->uri = (char*)sqlite3_column_blob(stmt, 1);
   m->title = (char*)sqlite3_column_blob(stmt, 2);
   m->album = (char*)sqlite3_column_blob(stmt, 3);
   m->genre = (char*)sqlite3_column_blob(stmt, 4);
   m->artist = (char*)sqlite3_column_blob(stmt, 5);
   m->size = sqlite3_column_int(stmt, 6);
   m->track_nb = sqlite3_column_int(stmt, 7);
   m->rating = sqlite3_column_int(stmt, 8);
   m->play_count = sqlite3_column_int(stmt, 9);
   _metadata_print(m);

   return m;
   

 done:
   _db_reset_stmt(stmt);
   return NULL;

}

EAPI int enna_scanner_audio_nb_albums_get()
{
   int r, ret;
   sqlite3_stmt *stmt;
   int nb = 0;
   
   stmt = _db_compile_stmt(scanner->db, "SELECT COUNT(*) FROM audio_albums");
   if (!stmt)
     return -1;

   r = sqlite3_step(stmt);
   if (r == SQLITE_DONE) {
      ret = 1;
      goto done;
   }
   nb = sqlite3_column_int(stmt, 0);
   ret = 0;
   dbg("Nb of medias in audio_albums db : %d\n", nb);
 done:
   _db_reset_stmt(stmt);
   return ret;
}

EAPI int enna_scanner_audio_nb_artists_get()
{
   int r, ret;
   sqlite3_stmt *stmt;
   int nb = 0;
   
   stmt = _db_compile_stmt(scanner->db, "SELECT COUNT(*) FROM audio_artists");
   if (!stmt)
     return -1;

   r = sqlite3_step(stmt);
   if (r == SQLITE_DONE) {
      ret = 1;
      goto done;
   }
   nb = sqlite3_column_int(stmt, 0);
   ret = 0;
   dbg("Nb of medias in audio_artists db : %d\n", nb);
 done:
   _db_reset_stmt(stmt);
   return ret;
}

EAPI int enna_scanner_audio_nb_genres_get()
{
   int r, ret;
   sqlite3_stmt *stmt;
   int nb = 0;
   
   stmt = _db_compile_stmt(scanner->db, "SELECT COUNT(*) FROM audio_genres");
   if (!stmt)
     return -1;

   r = sqlite3_step(stmt);
   if (r == SQLITE_DONE) {
      ret = 1;
      goto done;
   }
   nb = sqlite3_column_int(stmt, 0);
   ret = 0;
   dbg("Nb of medias in audio_genres db : %d\n", nb);
 done:
   _db_reset_stmt(stmt);
   return ret;
}

EAPI Ecore_List *enna_scanner_audio_artists_list_get()
{
   void         *buf[1];
   int r, ret;
   sqlite3_stmt *stmt;
   Ecore_List *albums;
   
 
   stmt = _db_compile_stmt(scanner->db, "SELECT name FROM audio_artists;");
   if (!stmt)
     goto done;
   
   albums = ecore_list_new();
   ecore_list_free_cb_set(albums, free);
   while ( sqlite3_step ( stmt ) == SQLITE_ROW )
    { 
       char  *album;
       album  = (char*)sqlite3_column_text(stmt,0);
       if (album)
	 ecore_list_append(albums, strdup(album));
    }
   return albums;

 done:
   _db_reset_stmt(stmt);
   return NULL;
}

EAPI Ecore_List *enna_scanner_audio_albums_list_get()
{
   
}

EAPI Ecore_List *enna_scanner_audio_genres_list_get()
{
   
}

EAPI Ecore_List *enna_scanner_audio_albums_of_artist_list_get(const char *artist)
{
  
   void         *buf[1];
   int r, ret;
   sqlite3_stmt *stmt;
   Ecore_List *albums;

   stmt = _db_compile_stmt(scanner->db, "SELECT audio_albums.name "
			                "FROM audio_albums,audio_artists "
                                        "WHERE audio_artists.name=? "
			                "AND audio_albums.artist_id=audio_artists.id;");
   if (!stmt)
     goto done;
   
   ret = _db_bind_text(stmt, 1, artist, strlen(artist));
   if (ret != 0)
     goto done;

   albums = ecore_list_new();
   while ( sqlite3_step ( stmt ) == SQLITE_ROW )
    { 
       char  *album;
       album  = (char*)sqlite3_column_text(stmt,0);
       if (album)
	 ecore_list_append(albums, strdup(album));
       
    }
   return albums;

 done:
   _db_reset_stmt(stmt);
   return NULL; 

}
EAPI Ecore_List *enna_scanner_audio_tracks_of_album_list_get(const char *artist, const char *album)
{
   void         *buf[1];
   int r, ret;
   sqlite3_stmt *stmt;
   Ecore_List *tracks;

   stmt = _db_compile_stmt(scanner->db, "SELECT audios.title,files.path "
			                "FROM audios,audio_artists,audio_albums,files "
                                        "WHERE audio_artists.name LIKE ? "
                                        "AND audio_albums.name LIKE ? "
                                        "AND audio_albums.artist_id=audio_artists.id "
                                        "AND audios.album_id=audio_albums.id "
			                "AND audios.id=files.id;");
   if (!stmt)
     goto done;
   
   ret = _db_bind_text(stmt, 1, artist, strlen(artist));
   if (ret != 0)
     goto done;
   ret = _db_bind_text(stmt, 2, album, strlen(album));
   if (ret != 0)
     goto done;

   tracks = ecore_list_new();
   while ( sqlite3_step ( stmt ) == SQLITE_ROW )
    { 
       Enna_Metadata *m;
       m = malloc(sizeof(Enna_Metadata));

       m->title  = strdup((char*)sqlite3_column_text(stmt,0));
       m->uri  = strdup((char*)sqlite3_column_text(stmt,1));
       m->track_nb =  sqlite3_column_int(stmt,2);
       if (m)
	 ecore_list_append(tracks, m);
       
    }
   return tracks;

 done:
   _db_reset_stmt(stmt);
   return NULL;
   
}


