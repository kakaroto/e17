#ifndef _ENNA_DB_H
# define _ENNA_DB_H

# include <sqlite3.h>
# include "enna.h"
# define ENNA_DB_VERSION 3

typedef struct _Enna_Metadata Enna_Metadata;

struct _Enna_Metadata
{
   char               *uri;
   char               *title;
   char               *album;
   char               *artist;
   char               *genre;
   int                 size;	//nb of bytes
   int                 duration;	//in seconds
   int                 track_nb;
   int                 nb_tracks;
   int                 year;
   int                 play_count;
   char               *cover;
};

EAPI Enna_Db       *enna_db_init();

#if WITH_IPOD_SUPPORT
EAPI Enna_Db       *enna_db_ipod_init();
EAPI void           enna_db_add_song_to_ipod_db(Enna_Db * db, char *filename);
#endif

EAPI void           enna_db_shutdown();
EAPI Enna_Metadata *enna_metadata_get(Enna_Db * db, char *filename);
EAPI Evas_List     *enna_db_cdda_metadata_get(Enna_Db * db, char *device,
					      void (*cb_func) (void *data,
							       Evas_List *
							       metadatas),
					      void *data);
EAPI Evas_List     *enna_db_get_albums_list(Enna_Db * db);
EAPI Evas_List     *enna_db_get_albums_from_artist(Enna_Db * db, char *artist);
EAPI Evas_List     *enna_db_get_titles_from_album(Enna_Db * db, char *album,
						  char *artist);
EAPI Evas_List     *enna_db_get_artists_list(Enna_Db * db);

#endif
