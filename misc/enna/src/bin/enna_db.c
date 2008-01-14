/*
 * enna_db.c
 * Copyright (C) Nicolas Aguirre 2006,2007,2008 <aguirre.nicolas@gmail.com>
 *
 * enna_db.c is free software copyrighted by Nicolas Aguirre.
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
 * enna_db.c IS PROVIDED BY Nicolas Aguirre ``AS IS'' AND ANY EXPRESS
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
#include <sqlite3.h>
#include <string.h>
#include <Ecore_File.h>
#include <Ecore_Str.h>
#include <tag_c.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <Evas.h>
#include <unistd.h>
#include <pthread.h>
#include <musicbrainz/queries.h>
#include <musicbrainz/mb_c.h>
#include <curl/curl.h>

#include "enna_db.h"
#include "enna.h"
#include "enna_config.h"
#include "enna_util.h"

#if WITH_IPOD_SUPPORT
#include <gpod/itdb.h>
#endif

typedef struct _Cdda_Thread Cdda_Thread;

struct _Cdda_Thread
{
   sqlite3            *db;
   char               *device;
   int                 fd_cdda_read;
   int                 fd_cdda_write;
   Ecore_Fd_Handler   *fd_cdda_handler;
   pthread_t           thread_cdda;
   void                (*cdda_func_cb) (void *data, Evas_List * metadatas);
   void               *cdda_data;

};

static void        *_scan_cover_thread(void *ptr);
static int          _add_cdda_metadata_to_db(sqlite3 * db, char *cdindex,
					     Enna_Metadata * metadata);
static int          _pipe_read_active(void *data, Ecore_Fd_Handler * fdh);
static void         _fill_info(Enna_Db * db, char *path);
static void         _create_table(sqlite3 * db, char *table_name,
				  char *sql_args);
static sqlite3     *_create_all_tables(char *filename);
static Enna_Db     *_create_db(Enna * enna, char *filename, char *path,
			       void (*func) (Enna_Db * db, char *path));
static Enna_Metadata* _process_file(Enna_Db * db, char *filename);
static int          _metadata_id3tag_get(char *filename,
					 Enna_Metadata * metadata);
static int          _metadata_fs_get(char *filename, Enna_Metadata * metadata);
static void         _metadata_print(Enna_Metadata * metadata);
static Enna_Metadata *_metadata_new();
static void         _metadata_delete(Enna_Metadata * m);

static int
_add_cdda_metadata_to_db(sqlite3 * db, char *cdindex, Enna_Metadata * metadata)
{
   sqlite3_stmt       *stmt;
   int                 result;
   char               *query;

   query =
      sqlite3_mprintf("REPLACE INTO music_cdda_table(cdindex, title, artist, "
		      "album, track_nb,nb_tracks,year, "
		      "play_count, duration, cover) "
		      "VALUES('%q', '%q', '%q', '%q', %d, %d, %d, %d, %d, '%q');",
		      cdindex ? cdindex : "Unknown",
		      metadata->title[0] ? metadata->title : "Unknown Title",
		      metadata->artist[0] ? metadata->artist : "Unknown Artist",
		      metadata->album[0] ? metadata->album : "Unknown Album",
		      metadata->track_nb, metadata->nb_tracks, metadata->year,
		      metadata->play_count, metadata->duration,
		      metadata->cover ? metadata->cover : "");

   result = sqlite3_prepare(db, query, -1, &stmt, NULL);
   if (result != SQLITE_OK && stmt != NULL)
     {
	dbg("Database: Unable to insert data into \"music_table\" table\n");
	sqlite3_free(query);
	sqlite3_finalize(stmt);
	return 0;
     }
   result = sqlite3_step(stmt);

   sqlite3_free(query);
   sqlite3_finalize(stmt);
   return 1;
}

static Evas_List   *
_get_metadata_from_device(sqlite3 * db, char *device)
{
   musicbrainz_t       mb;
   char                data[4096];
   int                 nb_tracks = 0;
   char               *cdindex;
   int                 i;
   Evas_List          *metadatas = NULL;

   mb = mb_New();
   mb_SetDevice(mb, device);	/* Set CDDA device */
   mb_UseUTF8(mb, TRUE);	/* Use UTF-8 for String */
   if (!mb_Query(mb, MBQ_GetCDTOC))
     {
	char                message[255];

	mb_GetQueryError(mb, message, 255);
     }
   if (!mb_GetResultData(mb, MBE_TOCGetCDIndexId, data, sizeof(data)))
     {
	mb_GetQueryError(mb, data, sizeof(data));
	mb_Delete(mb);
	return NULL;
     }

   cdindex = strdup(data);
   nb_tracks = mb_GetResultInt(mb, MBE_TOCGetLastTrack);

   if (!mb_Query(mb, MBQ_GetCDInfo))
     {
	mb_GetQueryError(mb, data, sizeof(data));
	dbg("This CD could not be queried: %s\n", data);
	for (i = 0; i < nb_tracks; i++)
	  {
	     Enna_Metadata      *metadata = NULL;

	     metadata = _metadata_new();
	     metadata->uri = strdup("");
	     metadata->title = strdup("Unknown Title");
	     metadata->album = strdup("Unknown Album");
	     metadata->artist = strdup("Unknown Aritst");
	     metadata->genre = strdup("Unknown Genre");
	     metadata->size = 0;	//nb of bytes
	     metadata->duration = 0;	//in seconds
	     metadata->track_nb = i + 1;
	     metadata->nb_tracks = nb_tracks;
	     metadata->year = 0;
	     metadata->play_count = 0;
	     metadata->cover = strdup("");
	     metadatas = evas_list_append(metadatas, metadata);
	     _add_cdda_metadata_to_db(db, cdindex, metadata);
	  }
	ENNA_FREE(cdindex);
	mb_Delete(mb);
	return metadatas;

     }
   else
     {
	mb_Select1(mb, MBS_SelectAlbum, 1);
	if (mb_GetResultData(mb, MBE_AlbumGetAlbumId, data, sizeof(data)))
	   mb_GetIDFromURL(mb, data, data, sizeof(data));
	if (mb_GetResultData(mb, MBE_AlbumGetAlbumArtistId, data, sizeof(data)))
	  {
	     mb_GetIDFromURL(mb, data, data, sizeof(data));
	     if (strncasecmp(MBI_VARIOUS_ARTIST_ID, data, 64) == 0)
	       {
		  dbg("Various Artist\n");
	       }
	     else
	       {
		  nb_tracks = mb_GetResultInt(mb, MBE_AlbumGetNumTracks);
		  for (i = 1; i <= nb_tracks; i++)
		    {
		       Enna_Metadata      *metadata = _metadata_new();

		       if (mb_GetResultData1
			   (mb, MBE_AlbumGetTrackName, data, sizeof(data), i))
			 {
			    metadata->title = strdup(data);
			 }
		       else
			 {
			    metadata->title = strdup("Unknown Track");
			 }

		       if (mb_GetResultData
			   (mb, MBE_AlbumGetAlbumName, data, sizeof(data)))
			 {
			    metadata->album = strdup(data);
			 }
		       else
			 {
			    metadata->album = strdup("Unknown Album");
			 }
		       if (mb_GetResultData
			   (mb, MBE_TrackGetArtistName, data, sizeof(data)))
			 {
			    metadata->artist = strdup(data);
			 }
		       else
			 {
			    metadata->artist = strdup("Unknown Album");
			 }
		       metadata->uri = strdup("");
		       metadata->genre = strdup("");
		       metadata->size = 0;	//nb of bytes
		       metadata->duration = 0;	//in seconds
		       metadata->track_nb = i;
		       metadata->nb_tracks = nb_tracks;
		       metadata->year = 0;
		       metadata->play_count = 0;
		       metadata->cover = strdup("");
		       metadatas = evas_list_append(metadatas, metadata);
		       _metadata_print(metadata);
		       _add_cdda_metadata_to_db(db, cdindex, metadata);
		    }
		  mb_Delete(mb);
		  ENNA_FREE(cdindex);
		  return metadatas;
	       }
	  }
     }
   return NULL;
}

static void        *
_get_cdda_metadata_thread(void *ptr)
{
   Cdda_Thread        *p;
   void               *buf[1];

   Evas_List          *metadatas;

   p = (Cdda_Thread *) ptr;

   metadatas = _get_metadata_from_device(p->db, p->device);

   buf[0] = metadatas;
   write(p->fd_cdda_write, buf, sizeof(buf));
   return NULL;
}

static int
_cdda_pipe_read_cb(void *data, Ecore_Fd_Handler * fdh)
{
   Cdda_Thread        *cdda;
   int                 fd;
   void               *buf[2];
   Evas_List          *metadatas;
   int                 len;

   cdda = (Cdda_Thread *) data;

   pthread_join(cdda->thread_cdda, NULL);
   fd = ecore_main_fd_handler_fd_get(fdh);
   while ((len = read(fd, buf, sizeof(buf))) > 0)
     {
	if (len == sizeof(buf))
	  {
	     metadatas = buf[0];
	  }
     }

   ENNA_FREE(cdda->device);
   if (cdda->cdda_func_cb)
      cdda->cdda_func_cb(cdda->cdda_data, metadatas);

   return 0;
}

EAPI Evas_List     *
enna_db_cdda_metadata_get(Enna_Db * db, char *device,
			  void (*cb_func) (void *data, Evas_List * metadata),
			  void *data)
{
   musicbrainz_t       mb;
   char                temp[4096];
   char               *cdindex;
   char               *error_msg;
   char               *query;
   char              **table_result;
   int                 nrows, ncols;
   int                 result;
   int                 i;
   Evas_List          *metadatas = NULL;
   int                 fds[2];

   /*   
    * Get CDIndex from musicbrainz 
    */
   mb = mb_New();
   mb_SetDevice(mb, device);	/* Set CDDA device */
   mb_UseUTF8(mb, TRUE);	/* Use UTF-8 for String */
   if (!mb_Query(mb, MBQ_GetCDTOC))
     {
	char                message[255];

	mb_GetQueryError(mb, message, 255);
	dbg("Cannot read CD: %s\n", message);
     }
   if (!mb_GetResultData(mb, MBE_TOCGetCDIndexId, temp, sizeof(temp)))
     {
	mb_GetQueryError(mb, temp, sizeof(temp));
	dbg("This CD could not be queried: %s\n", temp);
	return NULL;
     }
   cdindex = strdup(temp);
   /*
    * Search CDIndex in DB
    */
   query =
      sqlite3_mprintf
      ("SELECT DISTINCT cdindex,title,album,artist,track_nb,nb_tracks,play_count,cover FROM music_cdda_table;");
   result =
      sqlite3_get_table(db->db, query, &table_result, &nrows, &ncols,
			&error_msg);
   if (result != SQLITE_OK)
     {
	/* Can not find cdindex in database : error */
	dbg("Unable to select mb_cdindex from music_table: %s\n", error_msg);
	sqlite3_free(error_msg);
	sqlite3_free(query);
	return NULL;

     }
   sqlite3_free(query);
   if (!nrows)
     {
	Cdda_Thread        *cdda =
	   (Cdda_Thread *) calloc(1, sizeof(Cdda_Thread));
	cdda->db = NULL;
	/* This CDIndex is not found in DB : create new thread to create infos */
	/* and call back when info have been retrieved */
	if (pipe(fds) == 0)
	  {
	     cdda->device = strdup(device);
	     cdda->cdda_func_cb = cb_func;
	     cdda->cdda_data = data;
	     cdda->fd_cdda_read = fds[0];
	     cdda->fd_cdda_write = fds[1];
	     cdda->db = db->db;
	     fcntl(cdda->fd_cdda_read, F_SETFL, O_NONBLOCK);
	     cdda->fd_cdda_handler =
		ecore_main_fd_handler_add(cdda->fd_cdda_read, ECORE_FD_READ,
					  _cdda_pipe_read_cb, cdda, NULL, NULL);
	     ecore_main_fd_handler_active_set(cdda->fd_cdda_handler,
					      ECORE_FD_READ);
	     pthread_create(&cdda->thread_cdda, NULL, _get_cdda_metadata_thread,
			    (void *)cdda);
	     return NULL;
	  }
     }
   else
     {
	/* CD index found : Create Evas_list with metadata */

	for (i = 1; i <= nrows; i++)
	  {
	     Enna_Metadata      *metadata = _metadata_new();

	     metadata->title = strdup(table_result[i * ncols + 1]);
	     metadata->album = strdup(table_result[i * ncols + 2]);
	     metadata->artist = strdup(table_result[i * ncols + 3]);
	     metadata->uri = NULL;
	     metadata->genre = NULL;
	     metadata->size = 0;	//nb of bytes
	     metadata->duration = 0;	//in seconds
	     metadata->track_nb = atoi(table_result[i * ncols + 4]);
	     metadata->nb_tracks = atoi(table_result[i * ncols + 5]);
	     metadata->year = 0;
	     metadata->play_count = atoi(table_result[i * ncols + 6]);
	     metadata->cover = strdup(table_result[i * ncols + 7]);
	     metadatas = evas_list_append(metadatas, metadata);
	  }

	return metadatas;
     }
   return NULL;
}

static int
_download_http_file(char *file, char *url)
{
   CURL               *curl;
   CURLcode            res;

   FILE               *fp = fopen(file, "w");

   /* FIXME : Get permissions on file, if cannot write in directory, try to write in $HOME/.enna/cache */
   if (!fp)
      return 1;

   curl = curl_easy_init();
   if (curl)
     {
	curl_easy_setopt(curl, CURLOPT_URL, url);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);
	res = curl_easy_perform(curl);

	if (res)
	   dbg("Unable to download file.");

	curl_easy_cleanup(curl);
	fclose(fp);
	return res;
     }
   else
      return 1;
}

static int
_download_cover(Enna_Db * db, char *album, char *artist)
{
   musicbrainz_t       o, o2;
   char                error[256], data[256], temp[256], *args[2];
   int                 ret;
   int                 numAlbums = 0;
   char                asin[256];
   char                cd_info[256];
   int                 result;
   sqlite3_stmt       *stmt;
   char               *query;
   int                 i;

   // Create the musicbrainz object, which will be needed for subsequent calls
   o = mb_New();

   // Tell the client library to return data in ISO8859-1 and not UTF-8
   mb_UseUTF8(o, 1);

   // Set the proper server to use. Defaults to mm.musicbrainz.org:80
   if (getenv("MB_SERVER"))
      mb_SetServer(o, getenv("MB_SERVER"), 80);

   // Check to see if the debug env var has been set
   if (getenv("MB_DEBUG"))
      mb_SetDebug(o, atoi(getenv("MB_DEBUG")));

   // Tell the server to only return 2 levels of data, unless the MB_DEPTH env var is set
   if (getenv("MB_DEPTH"))
      mb_SetDepth(o, atoi(getenv("MB_DEPTH")));
   else
      mb_SetDepth(o, 4);

   /****************************************************************************/
   args[0] = album;
   args[1] = NULL;

   // Execute the MB_FindAlbumByName query
   ret = mb_QueryWithArgs(o, MBQ_FindAlbumByName, args);

   if (!ret)
     {
	mb_GetQueryError(o, error, 256);
	dbg("Query failed: %s\n", error);
	mb_Delete(o);
	return 0;
     }

   // Check to see how many items were returned from the server
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
      if (mb_GetResultData(o, MBE_AlbumGetAmazonAsin, asin, 256))
	{
	   sprintf(data, "%s/.enna/covers/%s.jpg", enna_util_user_home_get(),
		   asin);
	   if (ecore_file_exists(data))
	     {
		mb_Delete(o);
		query =
		   sqlite3_mprintf
		   ("UPDATE music_table SET cover='%q' WHERE album='%q';", data,
		    album);
		result = sqlite3_prepare(db->db, query, -1, &stmt, NULL);
		if (result != SQLITE_OK)
		  {
		     dbg("Database: Unable to insert data into \"music_table\" table\n");
		     sqlite3_free(query);
		     return 0;
		  }
		sqlite3_step(stmt);
		sqlite3_free(query);
		sqlite3_finalize(stmt);
	     }
	   return 0;
	}

      else
	{
	   mb_Delete(o);
	   return 0;
	}
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
   sprintf(temp, "%s/.enna/covers/%s-%s.jpg", enna_util_user_home_get(), artist,
	   album);
   if (!_download_http_file(temp, data))
     {
	query =
	   sqlite3_mprintf
	   ("UPDATE music_table SET cover='%q' WHERE album='%q';", temp, album);
	result = sqlite3_prepare(db->db, query, -1, &stmt, NULL);
	if (result != SQLITE_OK)
	  {
	     dbg("Database: Unable to insert data into \"music_table\" table\n");
	     sqlite3_free(query);
	     return 0;
	  }
	sqlite3_step(stmt);
	sqlite3_free(query);
	sqlite3_finalize(stmt);

     }
   else
      return 0;

   return 1;

}

static void        *
_scan_cover_thread(void *ptr)
{
   Enna_Db            *db;
   void               *buf[1];
   int                 result;
   char               *error_msg;
   char               *query;
   char              **table_result;
   int                 nrows, ncols;
   int                 i;

   db = (Enna_Db *) ptr;
   char               *cover_path;

   /*char cover_path2[PATH_MAX];
    * char cover_path3[PATH_MAX]; */

   query = sqlite3_mprintf("SELECT DISTINCT album from music_table;");
   result =
      sqlite3_get_table(db->db, query, &table_result, &nrows, &ncols,
			&error_msg);
   if (result != SQLITE_OK)
     {
	fprintf(stderr, "Unable to select track from music_table: %s\n",
		error_msg);
	sqlite3_free(error_msg);
	sqlite3_free(query);
	return NULL;
     }
   sqlite3_free(query);

   for (i = 0; i <= nrows; i++)
     {
	cover_path = table_result[i * ncols + 2];
	/* FIXME : test if cover is present in base directory and copy it in enna cover dir if exists */
	if (!cover_path || !cover_path[0] == '\0')
	   _download_cover(db, table_result[i * ncols],
			   table_result[i * ncols + 1]);
     }
   dbg("Cover Thread Done\n");
   buf[0] = "done";
   write(db->fd_ev_write, buf, sizeof(buf));
   return NULL;
}

static int
_pipe_read_active(void *data, Ecore_Fd_Handler * fdh)
{
   Enna_Db            *db;

   db = (Enna_Db *) data;
   pthread_join(db->thread_scan_cover, NULL);
   dbg("Cover Thread ended\n");
   return 0;
}

static Enna_Metadata *
_metadata_new()
{
   Enna_Metadata      *metadata;

   metadata = calloc(1, sizeof(Enna_Metadata));

   return metadata;
}

static void
_metadata_delete(Enna_Metadata * m)
{
   ENNA_FREE(m->uri);
   ENNA_FREE(m->title);
   ENNA_FREE(m->album);
   ENNA_FREE(m->artist);
   ENNA_FREE(m->genre);
   ENNA_FREE(m->cover);
   ENNA_FREE(m);
}

static int
_metadata_fs_get(char *filename, Enna_Metadata * metadata)
{
   struct stat         infos;

   stat(filename, &infos);
   metadata->uri = strdup(filename);
   metadata->size = infos.st_size;
   return 1;
}

static int
_metadata_id3tag_get(char *filename, Enna_Metadata * metadata)
{
   TagLib_File        *tag_file = NULL;
   TagLib_Tag         *tag = NULL;
   TagLib_AudioProperties *prop = NULL;

   if (!filename)
      return 0;

   tag_file = taglib_file_new(filename);
   if (!tag_file)
      return 0;

   tag = taglib_file_tag(tag_file);
   if (tag)
     {
	metadata->title = taglib_tag_title(tag);
	metadata->album = taglib_tag_album(tag);
	metadata->artist = taglib_tag_artist(tag);
	metadata->genre = taglib_tag_genre(tag);
	metadata->year = taglib_tag_year(tag);
	metadata->track_nb = taglib_tag_track(tag);
     }
   else
     {
	dbg("!!!!!!!!!!!!!!!!!!!!!NO TAG!!!!!!!!!!!!!!!!!!!!\n");
	metadata->title = strdup("Unknown Title");
	metadata->album = strdup("Unknown Album");
	metadata->artist = strdup("Unknown Artist");
	metadata->genre = strdup("Unknown Genre");
	metadata->year = 0;
	metadata->track_nb = 0;
     }

   prop = (TagLib_AudioProperties *) taglib_file_audioproperties(tag_file);
   if (prop)
     {
	metadata->duration = taglib_audioproperties_length(prop);
     }
   else
     {
	metadata->duration = 0;
     }

   taglib_file_free(tag_file);
   return 1;
}

static void
_metadata_insert_in_db(Enna_Db * db, Enna_Metadata * metadata)
{
   sqlite3_stmt       *stmt;
   int                 result;
   char               *query;

   query = sqlite3_mprintf("REPLACE INTO music_table(uri, title, artist, "
			   "album, genre, track_nb, year, "
			   "play_count, size, cover) "
			   "VALUES('%q', '%q', '%q', '%q', '%q', %d, %d, %d, %d, '%q');",
			   metadata->uri ? metadata->uri : "Unknown",
			   metadata->title[0] ? metadata->
			   title : "Unknown Title",
			   metadata->artist[0] ? metadata->
			   artist : "Unknown Artist",
			   metadata->album[0] ? metadata->
			   album : "Unknown Album",
			   metadata->genre[0] ? metadata->
			   genre : "Unknown Genre", metadata->track_nb,
			   metadata->year, metadata->play_count,
			   metadata->duration,
			   metadata->cover ? metadata->cover : "");
   result = sqlite3_prepare(db->db, query, -1, &stmt, NULL);
   if (result != SQLITE_OK && stmt != NULL)
     {
	dbg("Database: Unable to insert data into \"music_table\" table\n");
	sqlite3_free(query);
	_metadata_delete(metadata);
	sqlite3_finalize(stmt);
	return;
     }

   result = sqlite3_step(stmt);
   if (result != SQLITE_OK)
     printf("nok\n");
   else
     printf("ok");
   sqlite3_free(query);
   sqlite3_finalize(stmt);
}

static void
_metadata_print(Enna_Metadata * metadata)
{
#if 1
   dbg("---------\n");
   dbg("file:\t%s\ntitle:\t%s\nartist:\t%s\nalbum:\t%s\ngenre:\t%s\n"
       "track\t%d\nyear:\t%d\nplay count:\t%d\n"
       "duration:\t%d\ncover:\t%s\n",
       metadata->uri ? metadata->uri : "Unknown",
       metadata->title[0] ? metadata->title : "Unknown Title",
       metadata->artist[0] ? metadata->artist : "Unknown Artist",
       metadata->album[0] ? metadata->album : "Unknown Album",
       metadata->genre[0] ? metadata->genre : "Unknown Genre",
       metadata->track_nb,
       metadata->year,
       metadata->play_count,
       metadata->duration, metadata->cover ? metadata->cover : "");
   dbg("----------\n");
#endif
}

static Enna_Metadata *
_process_file(Enna_Db * db, char *filename)
{
   Enna_Metadata      *metadata;

   metadata = _metadata_new();
   _metadata_id3tag_get(filename, metadata);
   _metadata_fs_get(filename, metadata);
   _metadata_print(metadata);
   _metadata_insert_in_db(db, metadata);
   ENNA_FREE(filename);
   return metadata;
}

static void
_fill_info(Enna_Db * db, char *path)
{
   Ecore_List         *files = NULL;
   char               *filename = NULL;
   char                dir[PATH_MAX];

   files = ecore_file_ls(path);
   if (!files)
      return;
   filename = ecore_list_first_goto(files);

   while ((filename = (char *)ecore_list_next(files)) != NULL)
     {
	sprintf(dir, "%s/%s", path, filename);
	dbg("%s\n", dir);
	if (filename[0] == '.')
	   continue;
	else if (ecore_file_is_dir(dir))
	  {
	     _fill_info(db, strdup(dir));
	  }
	else if (ecore_str_has_suffix(filename, ".mp3"))
	  {
	    Enna_Metadata *metadata;
	    metadata = _process_file(db, strdup(dir));
	    _metadata_delete(metadata);
	     
	  }
     }
}

static void
_create_table(sqlite3 * db, char *table_name, char *sql_args)
{

   int                 res = 0;
   char               *query;
   sqlite3_stmt       *stmt;

   query = sqlite3_mprintf("CREATE TABLE %q(%q);", table_name, sql_args);

   res = sqlite3_prepare(db, query, -1, &stmt, NULL);
   if (res != SQLITE_OK)
     {
	dbg("Database: Unable to create \"%s\" table\n", table_name);
	sqlite3_free(query);
	sqlite3_close(db);
	sqlite3_finalize(stmt);
	return;
     }
   sqlite3_step(stmt);
   sqlite3_finalize(stmt);

}

static sqlite3     *
_create_all_tables(char *filename)
{
   int                 res = 0;
   char               *query;
   sqlite3            *db;
   sqlite3_stmt       *stmt;

   if (sqlite3_open(filename, &db) != SQLITE_OK)
     {
	dbg("Error Opening sqlite db : %s\n", sqlite3_errmsg(db));
	sqlite3_close(db);
	/* Im not god so if this fucking file could not been created, return NULL */
	return NULL;
     }

   /* New tables could be created */

   /*
    * 4 tables :
    * music_table : Contains informations about songs (Implemented)
    * music_db_version : contains version of db for compatibility with next versions of Enne (Implemented)
    * music_direcory : contains direcotries that are present in db (TODO!!!)
    * music_cdda_table : Contains informations about CDDA
    */

   _create_table(db, "music_db_version", "version INTEGER");
   _create_table(db, "music_table", "idx INTEGER PRIMARY KEY,"
		 "uri TEXT,"
		 "title TEXT,"
		 "artist TEXT,"
		 "album TEXT,"
		 "genre TEXT,"
		 "track_nb INTEGER,"
		 "year INTEGER,"
		 "size INTEGER,"
		 "duration INTEGER," "play_count INTEGER," "cover TEXT");
   _create_table(db, "music_cdda_table", "idx INTEGER PRIMARY KEY," "cdindex TEXT,"	/* Index Musicbrainz */
		 "title TEXT,"
		 "artist TEXT,"
		 "album TEXT,"
		 "genre TEXT,"
		 "track_nb INTEGER,"
		 "nb_tracks INTEGER,"
		 "year INTEGER,"
		 "duration INTEGER," "play_count INTEGER," "cover TEXT");

   query = sqlite3_mprintf("REPLACE INTO music_db_version(version) "
			   "VALUES(%d);", ENNA_DB_VERSION);
   res = sqlite3_prepare(db, query, -1, &stmt, NULL);
   if (res != SQLITE_OK)
     {
	dbg("Database: Unable to insert data into \"music_db_version\" table\n");
	sqlite3_free(query);
	sqlite3_finalize(stmt);
	sqlite3_close(db);
	return NULL;
     }
   sqlite3_step(stmt);
   sqlite3_free(query);
   sqlite3_finalize(stmt);
   return db;
}

static Enna_Db     *
_create_db(Enna * enna, char *filename, char *path,
	   void (*func) (Enna_Db * db, char *path))
{
   Enna_Db            *enna_db;
   sqlite3            *db;

   //char filename[PATH_MAX];
   int                 fds[2];

   enna_db = (Enna_Db *) malloc(sizeof(Enna_Db));
   if (!enna_db)
     {
	dbg("Can't allcocate Memory for Enna_DB struct\n");
	return NULL;
     }

   db = _create_all_tables(filename);
   enna_db->db = db;
   enna_db->version = ENNA_DB_VERSION;
   enna_db->path = strdup(path);
   /* Fill database with file infos */
   if (func)
      func(enna_db, path);

   if (pipe(fds) == 0)
     {
	enna_db->fd_ev_read = fds[0];
	enna_db->fd_ev_write = fds[1];
	fcntl(enna_db->fd_ev_read, F_SETFL, O_NONBLOCK);
	enna_db->fd_ev_handler = ecore_main_fd_handler_add(enna_db->fd_ev_read,
							   ECORE_FD_READ,
							   _pipe_read_active,
							   enna_db, NULL, NULL);
	ecore_main_fd_handler_active_set(enna_db->fd_ev_handler, ECORE_FD_READ);
	pthread_create(&enna_db->thread_scan_cover, NULL, _scan_cover_thread,
		       (void *)enna_db);
     }
   return enna_db;
}

EAPI Enna_Db       *
enna_db_init(Enna * enna)
{
   char                tmp[PATH_MAX];
   char               *filename;
   int                 res = 0;
   char               *query;
   char              **table_result;
   int                 nrows, ncols;
   Enna_Db            *enna_db;
   static sqlite3     *db;
   int                 fds[2];
   char               *error_msg;

   sprintf(tmp, "%s/.enna/%s", enna->home, "enna_music.db");
   filename = strdup(tmp);
   /* Db file exists on disk, opening db and return sqlite3 pointer */
   if (ecore_file_exists(filename))
     {
	if (sqlite3_open(filename, &db) != SQLITE_OK)
	  {
	     dbg("Error Opening sqlite db : %s\n", sqlite3_errmsg(db));
	     sqlite3_close(db);
	     ecore_file_recursive_rm(filename);
	     /* Can't open sqlite db => remove this file and try again */
	     return _create_db(enna, filename,
			       enna_config_get_conf_value_or_default
			       ("music_module", "base_path", "/"), _fill_info);
	  }
	query = sqlite3_mprintf("SELECT version FROM music_db_version;");
	res =
	   sqlite3_get_table(db, query, &table_result, &nrows, &ncols,
			     &error_msg);
	if (res != SQLITE_OK)
	  {
	     /* table db version with version seems to not exist */
	     /* remove db file and create new tables */
	     dbg("Unable to select version from music_db_version: %s\n",
		 error_msg);
	     sqlite3_free(error_msg);
	     sqlite3_free(query);
	     sqlite3_close(db);
	     /* remove file and create new db */
	     ecore_file_recursive_rm(filename);
	     return _create_db(enna, filename,
			       enna_config_get_conf_value_or_default
			       ("music_module", "base_path", "/"), _fill_info);
	  }
	/* Version Found */
	else
	  {

	     if (atoi(table_result[1]) != ENNA_DB_VERSION)
	       {
		  /* Bad DB Version remove db file and create new one */
		  dbg("Bad DB Version !\n");
		  sqlite3_free(error_msg);
		  sqlite3_free(query);
		  sqlite3_close(db);
		  /* remove file and create new db */
		  dbg("remove filename : %s\n", filename);
		  ecore_file_recursive_rm(filename);
		  return _create_db(enna, filename,
				    enna_config_get_conf_value_or_default
				    ("music_module", "base_path", "/"),
				    _fill_info);
	       }
	     enna_db = (Enna_Db *) malloc(sizeof(Enna_Db));
	     if (!enna_db)
	       {
		  dbg("Can't allcocate Memory for Enna_DB struct\n");
		  return NULL;
	       }

	     enna_db->db = db;
	     enna_db->version = atoi(table_result[1]);
	     /* DB Version is found and OK */
	     if (pipe(fds) == 0)
	       {
		  enna_db->fd_ev_read = fds[0];
		  enna_db->fd_ev_write = fds[1];
		  fcntl(enna_db->fd_ev_read, F_SETFL, O_NONBLOCK);
		  enna_db->fd_ev_handler =
		     ecore_main_fd_handler_add(enna_db->fd_ev_read,
					       ECORE_FD_READ, _pipe_read_active,
					       enna_db, NULL, NULL);
		  ecore_main_fd_handler_active_set(enna_db->fd_ev_handler,
						   ECORE_FD_READ);
		  pthread_create(&enna_db->thread_scan_cover, NULL,
				 _scan_cover_thread, (void *)enna_db);
	       }
	     return enna_db;
	  }
     }
   /* DB file doens't exist, open db, create tables, and return pointer */
   else
     {
	dbg("DB file doesn't exist\n");
	return _create_db(enna, filename,
			  enna_config_get_conf_value_or_default("music_module",
								"base_path",
								"/"),
			  _fill_info);
     }

   return NULL;
}

#if WITH_IPOD_SUPPORT

EAPI void
enna_db_add_song_to_ipod_db(Enna_Db * db, char *filename)
{
   Itdb_Track         *itdb_track;

   Enna_Metadata      *metadata;
   GError             *err = NULL;

   printf("add ipod song\n");
   itdb_track = itdb_track_new();
   metadata = _metadata_new();
   _metadata_fs_get(filename, metadata);
   _metadata_id3tag_get(filename, metadata);
   _metadata_print(metadata);
   itdb_track->title = metadata->title;
   itdb_track->album = metadata->album;
   itdb_track->artist = metadata->artist;
   itdb_track->itdb = db->itdb;
   itdb_track_add(db->itdb, itdb_track, -1);

   itdb_write(db->itdb, &err);
   if (err)
     {
	printf("err : %s\n", err->message);
     }

}

static void
_add_ipod_song_to_db(Enna_Db * db, Itdb_Track * song)
{
   char               *tmp;
   char                filename[PATH_MAX];
   sqlite3_stmt       *stmt;
   int                 result;
   char               *query;

   if (!song->ipod_path)
      return;

   tmp = strdup(song->ipod_path);
   itdb_filename_ipod2fs(tmp);
   sprintf(filename, "%s/%s", db->path, tmp);
   ENNA_FREE(tmp);

   query =
      sqlite3_mprintf
      ("REPLACE INTO music_table(uri, title, artist, album, genre, track_nb, year, play_count, size, cover) VALUES('%q', '%q', '%q', '%q', '%q', %d, %d, %d, %d, '%q');",
       filename ? filename : "Unknown",
       song->title ? song->title : "Unknown Title",
       song->artist ? song->artist : "Unknown Artist",
       song->album ? song->album : "Unknown Album",
       song->genre ? song->genre : "Unknown Genre", song->track_nr, song->year,
       song->playcount, song->tracklen / 1000, "");
   result = sqlite3_prepare(db->db, query, -1, &stmt, NULL);
   if (result != SQLITE_OK && stmt != NULL)
     {
	dbg("Database: Unable to insert data into \"music_table\" table\n");
	sqlite3_free(query);
	sqlite3_finalize(stmt);
	return;
     }

   result = sqlite3_step(stmt);

   sqlite3_free(query);
   sqlite3_finalize(stmt);
}

static void
_fill_info_ipod(Enna_Db * db, char *mount_point)
{
   Itdb_Playlist      *mpl;
   GList              *l;

   db->itdb = itdb_parse(mount_point, NULL);

   if (!db->itdb)
      return;

   mpl = itdb_playlist_mpl(db->itdb);
   if (mpl == NULL)
      dbg("Couldn't find iPod master playlist");
   else
      dbg("Ipod Name : %s\n", mpl->name);

   for (l = db->itdb->tracks; l != NULL; l = l->next)
     {
	Itdb_Track         *track;

	track = l->data;
	if (track)
	  {
	     printf("%s\n", track->title);
	     _add_ipod_song_to_db(db, track);
	  }

     }

}

EAPI Enna_Db       *
enna_db_ipod_init(Enna * enna, char *mount_point,
		  void (*func_progress_cb) (Enna_Db * db),
		  void (*func_finished_cb) (Enna_Db * db))
{
   Enna_Db            *db;

   char                tmp[PATH_MAX];
   char               *filename;

   sprintf(tmp, "%s/.enna/%s", enna->home, "enna_music_ipod.db");
   filename = strdup(tmp);
   if (ecore_file_exists(filename))
      ecore_file_recursive_rm(filename);

   db = _create_db(enna, filename, mount_point, _fill_info_ipod);

   return db;
}
EAPI void
enna_db_ipod_shutdown(Enna_Db * db)
{
   itdb_free(db->itdb);
   ENNA_FREE(db->path);
   ENNA_FREE(db);
}

#endif

EAPI void
enna_db_shutdown(Enna_Db * db)
{
   sqlite3_close(db->db);
   ENNA_FREE(db->path);
   ENNA_FREE(db);
}

EAPI Enna_Metadata *
enna_metadata_get(Enna_Db * db, char *filename)
{
   int                 result;
   char               *error_msg;
   char               *query;
   char              **table_result;
   int                 nrows, ncols;
   Enna_Metadata      *metadata;

   metadata = _metadata_new();

   query =
      sqlite3_mprintf("select * from music_table where uri='%q';", filename);
   result =
      sqlite3_get_table(db->db, query, &table_result, &nrows, &ncols,
			&error_msg);
   if (result != SQLITE_OK)
     {
	fprintf(stderr, "Unable to select track from music_table: %s\n",
		error_msg);
	sqlite3_free(error_msg);
	sqlite3_free(query);
	return NULL;
     }
   sqlite3_free(query);

   if (ncols == 0)
     {
       metadata = _process_file(db, strdup(filename));
       return metadata;
     }
   else
     {
	metadata->uri = table_result[ncols + 1];
	metadata->title = table_result[ncols + 2];
	metadata->artist = table_result[ncols + 2];
	metadata->album = table_result[ncols + 4];
	metadata->genre = table_result[ncols + 5];
	metadata->track_nb = atoi(table_result[ncols + 6]);
	metadata->year = atoi(table_result[ncols + 7]);
	metadata->duration = atoi(table_result[ncols + 8]);
	metadata->play_count = atoi(table_result[ncols + 10]);
	metadata->cover = table_result[ncols + 11];

     }
   return metadata;
}

EAPI Evas_List     *
enna_db_get_albums_list(Enna_Db * db)
{
   int                 result;
   char               *error_msg;
   char               *query;
   char              **table_result;
   int                 nrows, ncols;
   Evas_List          *albums_list = NULL;
   int                 i;

   query =
      sqlite3_mprintf("SELECT DISTINCT album FROM music_table ORDER BY album");
   result =
      sqlite3_get_table(db->db, query, &table_result, &nrows, &ncols,
			&error_msg);
   if (result != SQLITE_OK)
     {
	fprintf(stderr,
		"Unable to select distinct albums from music_table: %s\n",
		error_msg);
	sqlite3_free(error_msg);
	sqlite3_free(query);
	return NULL;
     }
   sqlite3_free(query);

   for (i = 1; i <= nrows; i++)
     {
	albums_list = evas_list_append(albums_list, table_result[i * ncols]);
     }
   return albums_list;
}

/**
 * Get the list of album for an artist
 * @param db Dtabase to search
 * @param artist Searched artist
 * @return Evas_List with album's name of artist searched, NULL otherwise
 */
EAPI Evas_List     *
enna_db_get_albums_from_artist(Enna_Db * db, char *artist)
{
   int                 result;
   char               *error_msg;
   char               *query;
   char              **table_result;
   int                 nrows, ncols;
   Evas_List          *albums_list = NULL;
   int                 i;

   query =
      sqlite3_mprintf
      ("SELECT DISTINCT album FROM music_table WHERE artist LIKE '%q' ORDER BY album;",
       artist);
   result =
      sqlite3_get_table(db->db, query, &table_result, &nrows, &ncols,
			&error_msg);
   if (result != SQLITE_OK)
     {
	fprintf(stderr,
		"Unable to select distinct album from music_table where artist = %s: %s\n",
		error_msg, artist);
	sqlite3_free(error_msg);
	sqlite3_free(query);
	return NULL;
     }
   sqlite3_free(query);

   for (i = 1; i <= nrows; i++)
     {
	albums_list = evas_list_append(albums_list, table_result[i * ncols]);
     }
   return albums_list;
}

/**
 * Get the list of album's title
 * @param db Database to search
 * @param album album's name searched
 * @param artist's name searched
 * @return Evas_List containing titles name
 */

EAPI Evas_List     *
enna_db_get_titles_from_album(Enna_Db * db, char *album, char *artist)
{
   int                 result;
   char               *error_msg;
   char               *query;
   char              **table_result;
   int                 nrows, ncols;
   Evas_List          *titles_list = NULL;
   int                 i;

   query = sqlite3_mprintf("select distinct title from music_table "
			   "where album='%q' and artist='%q' "
			   "order by track_nb", album, artist);
   result = sqlite3_get_table(db->db, query, &table_result,
			      &nrows, &ncols, &error_msg);
   if (result != SQLITE_OK)
     {
	fprintf(stderr, "Unable to select distinct titles from music_table "
		"where artist = %s and album = %s : %s\n",
		error_msg, artist, album);
	sqlite3_free(error_msg);
	sqlite3_free(query);
	return NULL;
     }
   sqlite3_free(query);

   for (i = 1; i <= nrows; i++)
     {
	titles_list = evas_list_append(titles_list, table_result[i * ncols]);
     }
   return titles_list;
}

/**
 * Get the list of db's artists
 * @param db Database to search
 * @return Evas_List with artist's name
 */

EAPI Evas_List     *
enna_db_get_artists_list(Enna_Db * db)
{
   int                 result;
   char               *error_msg;
   char               *query;
   char              **table_result;
   int                 nrows, ncols;
   Evas_List          *artists_list = NULL;
   int                 i;

   query = sqlite3_mprintf("select distinct artist from music_table "
			   "order by artist");
   result = sqlite3_get_table(db->db, query, &table_result,
			      &nrows, &ncols, &error_msg);
   if (result != SQLITE_OK)
     {
	fprintf(stderr, "Unable to select distinct artist "
		"from music_table: %s\n", error_msg);
	sqlite3_free(error_msg);
	sqlite3_free(query);
	return NULL;
     }
   sqlite3_free(query);

   for (i = 1; i <= nrows; i++)
     {
	artists_list = evas_list_append(artists_list, table_result[i * ncols]);
     }
   return artists_list;
}
