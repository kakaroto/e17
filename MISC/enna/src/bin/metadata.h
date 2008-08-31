#ifndef __ENNA_METADATA_H__
#define __ENNA_METADATA_H__


typedef struct _Enna_Metadata Enna_Metadata;
typedef struct _Enna_Metadata_Video Enna_Metadata_Video;
typedef struct _Enna_Metadata_Music Enna_Metadata_Music;

struct _Enna_Metadata_Music
{

   char       *artist;
   char       *album;
   char       *year;
   char       *genre;
   char       *comment;
   char       *discid;
   int         track;
   int         rating;
   int         play_count;
   char       *codec;
   int         bitrate; /* in Bps */
   int         channels;
   int         samplerate;
};

struct _Enna_Metadata_Video
{
   char       *codec;
   int         width;
   int         height;
   float       aspect;
   int         channels;
   int         streams;
   float       framerate;
   int         bitrate; /* in Bps */
};


struct _Enna_Metadata
{
   char       *uri;
   char       *title;
   int         size;   /* in Bytes */
   int         type;
   int         length; /* in seconds */
   Enna_Metadata_Video *video;
   Enna_Metadata_Music *music;

};

Enna_Metadata *enna_metadata_new();
void           enna_metadata_free(Enna_Metadata *m);

#endif
