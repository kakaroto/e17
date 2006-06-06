/** @file emphasis_misc.h */
#ifndef _MISC_H_
#define _MISC_H_

/**
 * @defgroup misc
 * @{
 */
#define evas_list_first(list) do{list=evas_list_prev(list);} while(evas_list_prev(list))

typedef enum _Emphasis_Data_Type {
	EMPHASIS_DATA_TYPE_NONE,
	EMPHASIS_DATA_TYPE_TAG,
	EMPHASIS_DATA_TYPE_DIRECTORY,
	EMPHASIS_DATA_TYPE_SONG,
	EMPHASIS_DATA_TYPE_PLAYLIST
} Emphasis_Data_Type;

typedef struct _Emphasis_Song {
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

typedef struct _Emphasis_Data {
	Emphasis_Data_Type type;
	union {
		char *tag;
		char *directory;
		char *playlist;
		Emphasis_Song *song;
		/* output_dev not supported */
	};
} Emphasis_Data;

MpdData *mpd_new_data_struct_append(MpdData  * const data);
MpdData* mpd_data_concatenate( MpdData  * const first, MpdData  * const second);
 
MpdData *convert_rowlist_in_playlist_with_file(Evas_List *rowlist);
MpdData *convert_rowlist_in_playlist_with_id(Evas_List *rowlist);
void mpd_data_full_free(MpdData *list);
void emphasis_playlist_append_selected(Etk_Tree *tree, Emphasis_Type type);
void emphasis_playlist_search_and_delete(Etk_Tree *tree, char *str, Emphasis_Type type);

Emphasis_Song *convert_mpd_song(mpd_Song *src);
Evas_List *convert_mpd_data(MpdData *src);
mpd_Song *convert_emphasis_song(Emphasis_Song *src);
MpdData *convert_evas_list(Evas_List *src);
/** @} */

#endif /*_MISC_H_*/
