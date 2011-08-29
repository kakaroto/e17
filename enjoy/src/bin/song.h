#ifndef __SONG_H__
#define __SONG_H__

struct _Song
{
   const char *path;
   const char *title;
   const char *album;
   const char *artist;
   const char *genre;
   int64_t id;
   int64_t album_id;
   int64_t artist_id;
   int64_t genre_id;
   int size; /* file size in bytes */
   int trackno;
   int rating;
   int playcnt;
   int length;
   struct {
      unsigned int path;
      unsigned int title;
      unsigned int album;
      unsigned int artist;
      unsigned int genre;
   } len; /* strlen of string fields */
   struct { /* not from db, for runtime use */
      Eina_Bool fetched_album:1;
      Eina_Bool fetched_artist:1;
      Eina_Bool fetched_genre:1;
   } flags;
};

#endif /* __SONG_H__ */
