#ifndef _ENNA_SCANNER_H_
#define _ENNA_SCANNER_H_

#include "enna.h"

typedef struct _Enna_Metadata Enna_Metadata;

struct _Enna_Metadata
{
  char               *uri;
  char               *title;
  char               *album;
  char               *artist;
  char               *genre;
  int                 size;
  int                 track_nb;
  int                 rating;
  int                 play_count;
};


EAPI void enna_scanner_init(void);
EAPI void enna_scanner_shutdown(void);

EAPI int enna_scanner_nb_medias_get(int type);
EAPI char *enna_scanner_cover_get(const char *album, const char *artist);
EAPI Enna_Metadata *enna_scanner_audio_metadata_get(const char *filename);
EAPI int enna_scanner_audio_nb_albums_get();
EAPI int enna_scanner_audio_nb_artists_get();
EAPI int enna_scanner_audio_nb_genres_get();
EAPI Ecore_List *enna_scanner_audio_artists_list_get();
EAPI Ecore_List *enna_scanner_audio_albums_list_get();
EAPI Ecore_List *enna_scanner_audio_genres_list_get();
EAPI Ecore_List *enna_scanner_audio_albums_of_artist_list_get(const char *artist);
EAPI Ecore_List *enna_scanner_audio_tracks_of_album_list_get(const char *artist, const char *album);

#endif
