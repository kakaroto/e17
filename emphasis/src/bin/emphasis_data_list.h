#ifndef EMPHASIS_DATA_LIST_H_
#define EMPHASIS_DATA_LIST_H_

typedef enum _Emphasis_Data_Type
{
  EMPHASIS_DATA_TYPE_NONE,
  EMPHASIS_DATA_TYPE_TAG,
  EMPHASIS_DATA_TYPE_DIRECTORY,
  EMPHASIS_DATA_TYPE_SONG,
  EMPHASIS_DATA_TYPE_PLAYLIST
} Emphasis_Data_Type;

typedef struct _Emphasis_Song
{
  char *file;
  char *artist;
  char *title;
  char *album;
  char *track;
  char *name;
  char *date;

  char *genre;
  char *composer;

  int time;
  int pos;
  int id;
} Emphasis_Song;

typedef struct _Emphasis_Data
{
  Emphasis_Data_Type type;

  char *tag;
  char *directory;
  char *playlist;
  Emphasis_Song *song;
  /* output_dev not supported */

} Emphasis_Data;


Emphasis_Data *emphasis_data_new(void);
void emphasis_data_free(Emphasis_Data *data);
void emphasis_song_free(Emphasis_Song *song);
void print_eina_list_stats(Eina_List *list);
void emphasis_list_free(Eina_List *list);
Eina_List *emphasis_list_concatenate(Eina_List *head, Eina_List *tail);

#endif /* EMPHASIS_DATA_LIST_H_ */
