#ifndef _EX_CDDB_H
#define _EX_CDDB_H

#define MAX_INEXACT_MATCHES 16

/* Match values returned by a query */
#define MATCH_NOMATCH    0
#define MATCH_EXACT      1
#define MATCH_INEXACT    2

typedef struct _Ex_Cddb_Hello   Ex_Cddb_Hello;
typedef struct _Ex_Cddb_Entry   Ex_Cddb_Entry;
typedef struct _Ex_Cddb_Query   Ex_Cddb_Query;
typedef struct _Ex_Track_Data   Ex_Track_Data;
typedef struct _Ex_Disc_Data    Ex_Disc_Data;
typedef struct _Ex_Id3_Genre    Ex_Id3_Genre;

struct _Ex_Id3_Genre
{
  char *name;
  int num;
};

struct _Ex_Cddb_Hello
{
   char hello_program[256];    /* Program */
   char hello_version[256];    /* Program version */
   int  proto_version;
};

/* Track database structure */
struct _Ex_Track_Data
{
   char name[256];               /* Track name */
   char artist[256];             /* Track artist */
   char extended[4096];           /* Extended information */
   char number[256];
   char *wav_filename;
};

/* Disc database structure */
struct _Ex_Disc_Data
{
   unsigned int  id;                  /* CD id */
   char          title[256];          /* Disc title */
   char          artist[256];         /* Disc artist */
   char          extended[4096];      /* Extended information */
   int           genre;               /* Discdb genre */
   int           id3genre;            /* ID3 genre */
   int           year;                /* Disc year */
   char          playlist[256];       /* Playlist info */
   int           multi_artist;        /* Is CD multi-artist? */
   Ex_Track_Data track[MAX_TRACKS];   /* Track names FIXME this should be dinamyc */
   int           revision;            /* Database revision */
};


/* this are ok */
void	ex_cddb_match_find(Extrackt *ex);
void	ex_cddb_match_get(Extrackt *ex, char *genre, char *list_id);
void	ex_cddb_request_send(Extrackt *ex, char *header, char *request);
void	ex_cddb_response_find_parse(Extrackt *ex, char *response);
void	ex_cddb_response_get_parse(Extrackt *ex, char *response);
void	ex_cddb_default_set(Ex_Disc_Data *dd, int numtracks);

void            ex_cddb_lookup(Extrackt *ex);
unsigned int    ex_cddb_discid_get(Extrackt *ex);    
char           *ex_cddb_genre_to_text(int genre);
int             ex_cddb_genre_to_num(char *genre);
int             ex_cddb_query(Extrackt *ex, Ex_Config_Cddb_Server *server, Ex_Cddb_Hello *hello);
int             ex_cddb_read(Extrackt *ex, Ex_Config_Cddb_Server *server, Ex_Cddb_Hello *hello, char *genre, char *list_id);
void            ex_cddb_title_parse(char *buf, char *title, char *artist, char *sep);
int             ex_cddb_utf8_validate(const Extrackt *ex, const Ex_Disc_Data *data);
int             ex_cddb_disc_cached(Extrackt *ex);    
int             ex_cddb_data_cache_read(Extrackt *ex, Ex_Disc_Data *ddata, const char *encoding);
int		ex_cddb_cache_write(Extrackt *ex, Ex_Disc_Data *ddata,FILE *outfile, int gripext, int freedbext, char *encoding);              
void		ex_cddb_info_get(Extrackt *ex);
char*		ex_id3_genre_string(int num);

#endif
