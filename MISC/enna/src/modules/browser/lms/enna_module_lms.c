/* Interface */

#include "enna.h"
#include <lightmediascanner.h>
#include <sqlite3.h>


//static int            _db_bind_blob(sqlite3_stmt *stmt, int col, const void *blob, int len);
static int            _db_bind_text(sqlite3_stmt *stmt, int col, const char *text, int len);
static sqlite3_stmt  *_db_compile_stmt(sqlite3 *db, const char *sql);
static int            _db_reset_stmt(sqlite3_stmt *stmt);
//static int            _db_finalize_stmt(sqlite3_stmt *stmt, const char *name);
static sqlite3       *_db_open(const char *filename);
static void          *_scanner_thread(void *ptr);
static int            _pipe_read_active(void *data, Ecore_Fd_Handler * fdh);
//static void           _metadata_print(Enna_Metadata * metadata);
//static int            _nb_medias_get(int type);
//static char          *_cover_get(const char *album, const char *artist);
//static Enna_Metadata *_audio_metadata_get(const char *filename);
//static int            _audio_nb_albums_get();
//static int            _audio_nb_artists_get();
//static int            _audio_nb_genres_get();
static Evas_List     *_audio_artists_list_get();
static Evas_List     *_audio_albums_list_get();
static Evas_List     *_audio_genres_list_get();
static Evas_List     *_audio_albums_of_artist_list_get(const char *artist);
static Evas_List     *_audio_tracks_of_album_list_get(const char *artist, const char *album);

static Evas_List     *_class_browse_up(const char *path);
static Evas_List     *_class_browse_down();
static Enna_Vfs_File *_class_vfs_get(void);
static void           _vfs_free(Enna_Vfs_File *vfs);
static Enna_Vfs_File *_vfs_set(char *uri, char *label,char *icon_file, unsigned char is_directory, char *icon);

static int            em_init(Enna_Module *em);
static int            em_shutdown(Enna_Module *em);

typedef struct _Enna_Module_Lms Enna_Module_Lms;
typedef struct _Enna_Scanner Enna_Scanner;

enum {
  ROOT,
  ARTISTS_ROOT,
  ALBUMS_ROOT,
  GENRES_ROOT,
  ARTISTS_ALBUMS,
  GENRES_ALBUMS,
  ARTISTS_ALBUMS_TRACKS,
};

struct _Enna_Scanner
{
   unsigned int     commit_interval;
   unsigned int     slave_timeout;
   const char      *db_path;
   Evas_List       *parsers;
   const char      *charset;
   Evas_List       *scan_path;
   lms_t           *lms;
   sqlite3         *db;
   int             fd_ev_read;
   int             fd_ev_write;
   Ecore_Fd_Handler *fd_ev_handler;
   pthread_t        scanner_thread;

};


struct _Enna_Module_Lms
{
   Evas *e;
   Enna_Module *em;
   Enna_Scanner *scanner;
   int state;
   Enna_Vfs_File *vfs;
};

static Enna_Module_Lms *mod;

EAPI Enna_Module_Api module_api =
  {
    ENNA_MODULE_VERSION,
    "lms"
  };

static Enna_Class_Vfs class =
  {
    "lms",
    1,
    "Browse Library",
    NULL,
    "icon/library",
    {
      NULL,
      NULL,
      _class_browse_up,
      _class_browse_down,
      _class_vfs_get,
    },
  };

#if 0
static int
_db_bind_blob(sqlite3_stmt *stmt, int col, const void *blob, int len)
{
   int r;

   if (blob)
     r = sqlite3_bind_blob(stmt, col, blob, len, SQLITE_STATIC);
   else
     r = sqlite3_bind_null(stmt, col);

   if (r == SQLITE_OK)
     return 0;
   else
     {
	sqlite3 *db;
	const char *err;

	db = sqlite3_db_handle(stmt);
	err = sqlite3_errmsg(db);
	fprintf(stderr, "ERROR: could not bind SQL value %d: %s\n", col, err);
	return -col;
   }
}
#endif

static int
_db_bind_text(sqlite3_stmt *stmt, int col, const char *text, int len)
{
   int r;

   if (text)
     r = sqlite3_bind_text(stmt, col, text, len, SQLITE_STATIC);
   else
     r = sqlite3_bind_null(stmt, col);

   if (r == SQLITE_OK)
     return 0;
   else
     {
	sqlite3 *db;
	const char *err;

	db = sqlite3_db_handle(stmt);
	err = sqlite3_errmsg(db);
	fprintf(stderr, "ERROR: could not bind SQL value %d: %s\n", col, err);
	return -col;
     }
}

static sqlite3_stmt *
_db_compile_stmt(sqlite3 *db, const char *sql)
{
   sqlite3_stmt *stmt;

   if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK)
     dbg("ERROR: could not prepare \"%s\": %s\n", sql,
	 sqlite3_errmsg(db));

   return stmt;
}

static int
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

#if 0
static int
_db_finalize_stmt(sqlite3_stmt *stmt, const char *name)
{
   int r;

   r = sqlite3_finalize(stmt);
   if (r != SQLITE_OK)
     {
	dbg("ERROR: could not finalize %s statement: #%d\n", name, r);
	return -1;
     }

   return 0;
}
#endif


static sqlite3 *
_db_open(const char *filename)
{

   sqlite3 *db;

   if (sqlite3_open(filename, &db) != SQLITE_OK)
     {
	dbg("ERROR: could not open DB \"%s\": %s\n", filename, sqlite3_errmsg(db));
	sqlite3_close(db);
	return NULL;
     }
   return db;
}

static void *
_scanner_thread(void *ptr)
{
   Enna_Scanner *scanner;
   void         *buf[1];
   Evas_List *l;
   scanner = (Enna_Scanner *) ptr;

   for (l = scanner->scan_path; l; l = l->next)
     {
	printf("scan : %s\n", (char*)l->data);
	lms_check(scanner->lms, (char*)l->data);
	/*  Start Scan process */
	lms_process(scanner->lms, (char*)l->data);
	/*  Scann process is done */
     }

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


#if 0
static void
_metadata_print(Enna_Metadata * metadata)
{
#if 0
   dbg("---------\n");
   dbg("\n\tfile:\t%s\n\ttitle:\t%s\n\tartist:\t%s\n\talbum:\t%s\n\tgenre:\t%s\n"
       "\ttrack\t%s\n\tplay count:\t%d\n",
       metadata->uri ? metadata->uri : "Unknown",
       metadata->title[0] ? metadata->title : "Unknown Title",
       metadata->artist[0] ? metadata->artist : "Unknown Artist",
       metadata->album[0] ? metadata->album : "Unknown Album",
       metadata->genre[0] ? metadata->genre : "Unknown Genre",
       metadata->track,
       metadata->play_count);
   dbg("----------\n");
#endif
}

static int
_nb_medias_get(int type)
{
   int r, ret;
   sqlite3_stmt *stmt;
   int nb = 0;

   stmt = _db_compile_stmt(mod->scanner->db, "SELECT COUNT(*) FROM files");
   if (!stmt)
     return -1;

   r = sqlite3_step(stmt);
   if (r == SQLITE_DONE)
     {
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


static char *
_cover_get(const char *album, const char *artist)
{
   char cover_filename[PATH_MAX];

   if (!artist || !album)
     return NULL;

   sprintf(cover_filename, "%s/.enna/covers/%s - %s.jpg", enna_util_user_home_get(), artist, album);
   if (ecore_file_exists(cover_filename))
     return strdup(cover_filename);
   else
     return NULL;
}

static Enna_Metadata *
_audio_metadata_get(const char *filename)
{

   int r, ret;
   sqlite3_stmt *stmt;
   //int nb = 0;
   int id;
   Enna_Metadata *m;

   m = malloc(sizeof(Enna_Metadata));

   stmt = _db_compile_stmt(mod->scanner->db,
			   "SELECT files.id, "
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
			   "path=?");
   if (!stmt)
     return NULL;

   ret = _db_bind_blob(stmt, 1, filename, strlen(filename));
   if (ret != 0)
     goto done;

   r = sqlite3_step(stmt);
   if (r == SQLITE_DONE)
     {
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
   /*m->track = sqlite3_column_int(stmt, 7);*/
   m->rating = sqlite3_column_int(stmt, 8);
   m->play_count = sqlite3_column_int(stmt, 9);
   _metadata_print(m);

   return m;


 done:
   _db_reset_stmt(stmt);
   return NULL;

}

static int
_audio_nb_albums_get()
{
   int r, ret;
   sqlite3_stmt *stmt;
   int nb = 0;

   stmt = _db_compile_stmt(mod->scanner->db, "SELECT COUNT(*) FROM audio_albums");
   if (!stmt)
     return -1;

   r = sqlite3_step(stmt);
   if (r == SQLITE_DONE)
     {
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

static int
_audio_nb_artists_get()
{
   int r, ret;
   sqlite3_stmt *stmt;
   int nb = 0;

   stmt = _db_compile_stmt(mod->scanner->db, "SELECT COUNT(*) FROM audio_artists");
   if (!stmt)
     return -1;

   r = sqlite3_step(stmt);
   if (r == SQLITE_DONE)
     {
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

static int
_audio_nb_genres_get()
{
   int r, ret;
   sqlite3_stmt *stmt;
   int nb = 0;

   stmt = _db_compile_stmt(mod->scanner->db, "SELECT COUNT(*) FROM audio_genres");
   if (!stmt)
     return -1;

   r = sqlite3_step(stmt);
   if (r == SQLITE_DONE)
     {
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
#endif

static Evas_List *
_audio_artists_list_get()
{
   sqlite3_stmt *stmt;
   Evas_List *artists = NULL;

   stmt = _db_compile_stmt(mod->scanner->db, "SELECT name FROM audio_artists;");
   if (!stmt)
     goto done;

   while ( sqlite3_step ( stmt ) == SQLITE_ROW )
     {
	char  *artist;
	artist  = (char*)sqlite3_column_text(stmt,0);
	if (artist)
	  artists = evas_list_append(artists, strdup(artist));
     }
   return artists;

 done:
   _db_reset_stmt(stmt);
   return NULL;
}

static Evas_List *
_audio_albums_list_get()
{
   sqlite3_stmt *stmt;
   Evas_List *albums = NULL;

   stmt = _db_compile_stmt(mod->scanner->db,
			   "SELECT DISTINCT name FROM audio_albums");
   if (!stmt)
     goto done;

   while ( sqlite3_step ( stmt ) == SQLITE_ROW )
     {
	char  *album;
	album  = (char*)sqlite3_column_text(stmt,0);
	if (album)
	  albums = evas_list_append(albums, strdup(album));

     }
   return albums;

 done:
   _db_reset_stmt(stmt);
   return NULL;
}

static Evas_List *
_audio_genres_list_get()
{
   sqlite3_stmt *stmt;
   Evas_List *genres = NULL;

   stmt = _db_compile_stmt(mod->scanner->db,
			   "SELECT DISTINCT name FROM audio_genres");
   if (!stmt)
     goto done;

   while ( sqlite3_step ( stmt ) == SQLITE_ROW )
     {
	char  *genre;
	genre  = (char*)sqlite3_column_text(stmt,0);
	if (genre)
	  genres = evas_list_append(genres, strdup(genre));

     }
   return genres;

 done:
   _db_reset_stmt(stmt);
   return NULL;
}

static Evas_List *
_audio_albums_of_artist_list_get(const char *artist)
{
   int ret;
   sqlite3_stmt *stmt;
   Evas_List *albums = NULL;

   stmt = _db_compile_stmt(mod->scanner->db,
			   "SELECT audio_albums.name "
			   "FROM audio_albums,audio_artists "
			   "WHERE audio_artists.name=? "
			   "AND audio_albums.artist_id=audio_artists.id;");
   if (!stmt)
     goto done;

   ret = _db_bind_text(stmt, 1, artist, strlen(artist));
   if (ret != 0)
     goto done;

   while ( sqlite3_step ( stmt ) == SQLITE_ROW )
     {
	char  *album;
	album  = (char*)sqlite3_column_text(stmt,0);
	if (album)
	  albums = evas_list_append(albums, strdup(album));

     }
   return albums;

 done:
   _db_reset_stmt(stmt);
   return NULL;

}

static Evas_List *
_audio_tracks_of_album_list_get(const char *artist, const char *album)
{
   int ret;
   sqlite3_stmt *stmt;
   Evas_List *tracks = NULL;

   stmt = _db_compile_stmt(mod->scanner->db,
			   "SELECT audios.title,files.path,audios.trackno "
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


   while ( sqlite3_step ( stmt ) == SQLITE_ROW )
     {
	Enna_Metadata *m;
	m = malloc(sizeof(Enna_Metadata));

	m->title  = strdup((char*)sqlite3_column_text(stmt,0));
	m->uri    = strdup((char*)sqlite3_column_text(stmt,1));
	m->track  = sqlite3_column_int(stmt,2);
	if (m)
	  tracks = evas_list_append(tracks, m);

     }
   return tracks;

 done:
   _db_reset_stmt(stmt);
   return NULL;

}

static Enna_Vfs_File *
_vfs_set(char *uri, char *label,
	 char *icon_file, unsigned char is_directory,
	 char *icon)
{
   return enna_vfs_create_directory (uri, label, icon, icon_file);
}

static void
_vfs_free(Enna_Vfs_File *file)
{
   enna_vfs_remove (file);
}

static int
_sort_cb(void *d1, void *d2)
{
   Enna_Vfs_File *f1 = d1;
   Enna_Vfs_File *f2 = d2;

   if(!f1->label) return(1);
   if(!f2->label) return(-1);

   return(strcasecmp((const char*)f1->label, (const char*)f2->label));
}

static Evas_List *
_browse_root()
{
   Evas_List *entries = NULL;
   Enna_Vfs_File *file;
   mod->state = ROOT;
   mod->vfs = NULL;

   file = enna_vfs_create_directory ("artists://", "Artists",
                                     "icon/artist", NULL);
   entries = evas_list_append(entries, file);

   file = enna_vfs_create_directory ("albums://", "Albums",
                                     "icon/album", NULL);
   entries = evas_list_append(entries, file);

   file = enna_vfs_create_directory ("genres://", "Genres",
                                     "icon/genre", NULL);
   entries = evas_list_append(entries, file);

   return entries;
}

static Evas_List *
_browse_artists_root()
{
   Evas_List *l;
   Evas_List *entries = NULL;

   mod->state = ARTISTS_ROOT;
   _vfs_free(mod->vfs);
   mod->vfs = _vfs_set(strdup("artists://"), strdup("Artist"), NULL, 1, NULL);
   for( l = _audio_artists_list_get(); l; l = l->next)
     {
	Enna_Vfs_File *file;
	char uri[4096];

	snprintf(uri, sizeof(uri), "artists://%s", (char*)l->data);
        file = enna_vfs_create_directory (uri, l->data,
                                          "icon/artist", NULL);
	entries = evas_list_append(entries, file);

     }
   return evas_list_sort(entries, evas_list_count(entries), _sort_cb);
}

static Evas_List *_class_browse_up(const char *path)
{

   /* Display LMS ROOT menu*/
   if (!path)
     {
	return _browse_root();
     }
   else if (!strcmp(path, "artists://"))
     {
	return _browse_artists_root();
     }
   else if (!strcmp(path, "albums://"))
     {
	Evas_List *l;
	Evas_List *entries = NULL;

	mod->state = ALBUMS_ROOT;
	_vfs_free(mod->vfs);
	mod->vfs = _vfs_set(strdup("albums://"), strdup("Album"), NULL, 1, NULL);
	for( l = _audio_albums_list_get(); l; l = l->next)
	  {
	     Enna_Vfs_File *file;
	     char uri[4096];

	     snprintf(uri, sizeof(uri), "albums://%s", (char*)l->data);
	     /* FIXME Set Cover filename here */
             file = enna_vfs_create_directory (uri, l->data, NULL, NULL);
	     entries = evas_list_append(entries, file);

	  }
	return evas_list_sort(entries, evas_list_count(entries), _sort_cb);
     }
   else if (!strcmp(path, "genres://"))
     {
	Evas_List *l;
	Evas_List *entries = NULL;

	mod->state = GENRES_ROOT;
	_vfs_free(mod->vfs);
	mod->vfs = _vfs_set(strdup("genres://"), strdup("Genres"), NULL, 1, NULL);
	for( l = _audio_genres_list_get(); l; l = l->next)
	  {
	     Enna_Vfs_File *file;
	     char uri[4096];

	     snprintf(uri, sizeof(uri), "genres://%s", (char*)l->data);
	     /* FIXME Set Cover filename here */
             file = enna_vfs_create_directory (uri, l->data, NULL, NULL);
	     entries = evas_list_append(entries, file);

	  }
	return evas_list_sort(entries, evas_list_count(entries), _sort_cb);

     }
   else if (!strncmp(path, "artists://", 10))
     {
	if ( mod->state == ARTISTS_ROOT)
	  {
	     Evas_List *l;
	     Evas_List *entries = NULL;
	     char uri[4096];
	     const char *artist;
	     const char *album;

	     mod->state = ARTISTS_ALBUMS;

	     _vfs_free(mod->vfs);
	     mod->vfs = _vfs_set(strdup(path), strdup(path+10), NULL, 1, NULL);
	     artist = path + 10;
	     for( l = _audio_albums_of_artist_list_get(path+10); l; l = l->next)
	       {
		  Enna_Vfs_File *file;
		  Evas_List *l2 = NULL;
		  Enna_Vfs_File *filename;
                  char *icon_file = NULL;
                  
		  snprintf(uri, sizeof(uri), "%s/%s", path, (char*)l->data);

		  album = uri;
		  /* FIXME Set Cover filename here */
		  /* */
		  printf("album : %s, artist : %s\n", album, artist);
		  l2 = _audio_tracks_of_album_list_get(artist, album);
		  if (evas_list_count(l2))
		    {
		       filename = evas_list_nth(l2, 0);
		       printf("uri : %s\n", filename->uri);
		       icon_file =enna_cover_album_get( artist, album, filename->uri);
		    }

                  file = enna_vfs_create_directory (uri, l->data,
                                                    "icon/album", icon_file);
		  entries = evas_list_append(entries, file);

	       }
	     return evas_list_sort(entries, evas_list_count(entries), _sort_cb);
	  }
	else
	  {

	     Evas_List *l;
	     Evas_List *entries = NULL;
	     char uri[4096];
	     const char *album;
	     const char *artist;

	     album = ecore_file_file_get(path);
	     artist =  ecore_file_file_get(ecore_file_dir_get(path));

	     mod->state = ARTISTS_ALBUMS_TRACKS;
	     _vfs_free(mod->vfs);
	     mod->vfs = _vfs_set(strdup(path), strdup(album), NULL, 1, NULL);

	     for( l = _audio_tracks_of_album_list_get(artist, album); l; l = l->next)
	       {
		  Enna_Vfs_File *file;
		  Enna_Metadata *m;
		  char tmp[4096];

		  m = l->data;
		  snprintf(uri, sizeof(uri), "file://%s", m->uri);
		  snprintf(tmp, sizeof(tmp), "%02d - %s", m->track, m->title);
                  file = enna_vfs_create_file (uri, tmp,
                                               "icon/song", NULL);
		  entries = evas_list_append(entries, file);
		  free(m);

	       }
	     return evas_list_sort(entries, evas_list_count(entries), _sort_cb);
	  }
     }
   return NULL;
}



static Evas_List *_class_browse_down()
{
   switch (mod->state)
     {
      case ROOT:
	 return NULL;
      case ARTISTS_ROOT:
      case ALBUMS_ROOT:
      case GENRES_ROOT:
	 return _browse_root();
      case ARTISTS_ALBUMS:
	 return _browse_artists_root();
      case ARTISTS_ALBUMS_TRACKS:
	{
	   Evas_List *files;
	   char *uri;
	   mod->state = ARTISTS_ROOT;
	   printf("mod->vfs->uri : %s\n", ecore_file_dir_get(mod->vfs->uri));
	   uri = strdup(ecore_file_dir_get(mod->vfs->uri));
	   files = _class_browse_up(uri);
	   free(uri);
	   return files;
	}
      default:
	 return NULL;
     }


   return NULL;
}


static Enna_Vfs_File *_class_vfs_get()
{
   return mod->vfs;
}

/* Module interface */

static int
em_init(Enna_Module *em)
{
   Enna_Config_Data *cfgdata = NULL;
   Evas_List *l = NULL;
   char           tmp[PATH_MAX];
   int            fds[2];

   mod = calloc(1, sizeof(Enna_Module_Lms));
   mod->em = em;
   em->mod = mod;
   mod->vfs = NULL;
   enna_vfs_append("lms", ENNA_CAPS_MUSIC, &class);

   /* Create scanner for audio only */
   mod->scanner = malloc(sizeof( Enna_Scanner));
   if (!mod->scanner) return -1;
   /* Commit changes on db each 100 medias */


   /* Set db filename */
   sprintf(tmp, "%s/.enna/%s", enna_util_user_home_get(), "enna_music.db");
   mod->scanner->db_path = evas_stringshare_add(tmp) ;
   /* Set charset to UTF-8 */
   mod->scanner->charset = evas_stringshare_add("iso-8859-1");
   /* Set base path */

   /* Create lms, set params and start scann process */
   mod->scanner->lms = lms_new(mod->scanner->db_path);
   if (!mod->scanner->lms)
     goto error;

   mod->state = ROOT;

   /* Config */
   /* Default value */
   mod->scanner->commit_interval = 100;
   mod->scanner->slave_timeout = 1000;
   mod->scanner->scan_path = NULL;
   mod->scanner->parsers = NULL;
   /* Load Config file values */
   cfgdata = enna_config_module_pair_get("lms");
   mod->scanner->scan_path = NULL;
   if (cfgdata)
     {
	Evas_List *parser = NULL;
	for (l = cfgdata->pair; l; l = l->next)
	  {
	     char *path;
	     Config_Pair *pair = l->data;
	     if (!strcmp("path", pair->key))
	       {
		  enna_config_value_store(&path, "path", ENNA_CONFIG_STRING, pair);
		  mod->scanner->scan_path = evas_list_append(mod->scanner->scan_path, path+7);
	       }
	     enna_config_value_store(&parser, "parser", ENNA_CONFIG_STRING_LIST, pair);
	     enna_config_value_store(&mod->scanner->slave_timeout, "slave_timeout", ENNA_CONFIG_INT, pair);
	     enna_config_value_store(&mod->scanner->commit_interval, "commit_interval", ENNA_CONFIG_INT, pair);
	  }
	for (l = parser; l; l = l->next)
	  {
	     char *p = l->data;
	     printf("parser : %s\n", p);
	     if(!strcmp(p, "ogg"))
	       mod->scanner->parsers = evas_list_append(mod->scanner->parsers, lms_parser_find_and_add(mod->scanner->lms, "ogg"));
	     if(!strcmp(p, "mp3"))
	       mod->scanner->parsers = evas_list_append(mod->scanner->parsers, lms_parser_find_and_add(mod->scanner->lms, "id3"));
	     if(!strcmp(p, "flac"))
	       mod->scanner->parsers = evas_list_append(mod->scanner->parsers, lms_parser_find_and_add(mod->scanner->lms, "flac"));
	     if(!strcmp(p, "dummy"))
	        mod->scanner->parsers = evas_list_append(mod->scanner->parsers, lms_parser_find_and_add(mod->scanner->lms, "audio-dummy"));
	     if(!strcmp(p, "rm"))
	       mod->scanner->parsers = evas_list_append(mod->scanner->parsers, lms_parser_find_and_add(mod->scanner->lms, "rm"));
	  }


     }
   for (l = mod->scanner->scan_path; l; l = l->next)
     printf("Scanner is going to scan : %s\n", (char*)l->data);
   printf("Slave Timeout : %d\n",mod->scanner->slave_timeout);
   printf("Commit Interval : %d\n", mod->scanner->commit_interval);

   lms_set_commit_interval(mod->scanner->lms, mod->scanner->commit_interval);
   lms_set_slave_timeout(mod->scanner->lms, mod->scanner->slave_timeout);
   lms_parser_find_and_add(mod->scanner->lms, "audio-dummy");


   if (lms_charset_add(mod->scanner->lms, mod->scanner->charset) != 0)
     goto error;

   if (pipe(fds) == 0)
     {
	mod->scanner->fd_ev_read = fds[0];
	mod->scanner->fd_ev_write = fds[1];
	fcntl(mod->scanner->fd_ev_read, F_SETFL, O_NONBLOCK);
	mod->scanner->fd_ev_handler = ecore_main_fd_handler_add(mod->scanner->fd_ev_read,
								ECORE_FD_READ,
								_pipe_read_active,
								mod->scanner, NULL, NULL);
	ecore_main_fd_handler_active_set(mod->scanner->fd_ev_handler, ECORE_FD_READ);
	pthread_create(&mod->scanner->scanner_thread, NULL, _scanner_thread,
		       (void *)mod->scanner);
     }

   mod->scanner->db = _db_open(mod->scanner->db_path);

   /*   */
   return 0;


 error:
   dbg("Error during lms module initialisation\n");
   lms_free(mod->scanner->lms);
   evas_stringshare_del(mod->scanner->db_path);
   evas_stringshare_del(mod->scanner->charset);
   ENNA_FREE(mod->scanner);
   return -1;
}


static int
em_shutdown(Enna_Module *em)
{
   Evas_List *l;
   Enna_Module_Lms *mod;
   mod = em->mod;

   lms_stop_processing(mod->scanner->lms);
   for (l = mod->scanner->parsers; l; l = l->next)
     lms_parser_del(mod->scanner->lms, (lms_plugin_t*)l->data);
   lms_free(mod->scanner->lms);
   sqlite3_close(mod->scanner->db);
   free(mod->scanner);
   free(mod);
   return 1;
}

EAPI void
module_init(Enna_Module *em)
{
   if (!em)
     return;

   if (!em_init(em))
     return;
}

EAPI void
module_shutdown(Enna_Module *em)
{
   em_shutdown(em);
}
