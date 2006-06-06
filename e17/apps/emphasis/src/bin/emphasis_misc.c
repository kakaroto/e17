#include "emphasis.h"
#include "emphasis_misc.h"

/**
 * @brief Convert a list of row in a MpdData list.
 *        And set the song->file element
 * @param rowlist An Evas_List of song's row
 * @return A list of song
 */
MpdData *
convert_rowlist_in_playlist_with_file(Evas_List *rowlist)
{
	Evas_List *list;
	Etk_Tree_Row *row;
	char *file;
	MpdData *data=NULL;

	if (!rowlist)
		return NULL;
		
	list = rowlist;
	
	while (rowlist)
	{	
		row = evas_list_data(rowlist);
		file = etk_tree_row_data_get(row);
		
		data = mpd_new_data_struct_append(data);
		data->type = MPD_DATA_TYPE_SONG;
		data->song = mpd_newSong();
		data->song->file = file;
		
		rowlist = evas_list_next(rowlist);
	}
	
	rowlist = list;
	data = mpd_data_get_first(data);
	return data;
}

/**
 * @brief Convert a list of row in a MpdData.
 *        And set the song->id element
 * @param rowlist An Evas_List of song's row
 * @return A list of song
 */
MpdData *
convert_rowlist_in_playlist_with_id(Evas_List *rowlist)
{
	Evas_List *list;
	Etk_Tree_Row *row;
	int id;
	MpdData *data=NULL;

	if (!rowlist)
		return NULL;
	
	list = rowlist;

	while (rowlist)
	{	
		row = evas_list_data(rowlist);
		id = (int)etk_tree_row_data_get(row);
		
		data = mpd_new_data_struct_append(data);
		data->type = MPD_DATA_TYPE_SONG;
		data->song = mpd_newSong();
		data->song->id = id;
		
		rowlist = evas_list_next(rowlist);
	}
	
	rowlist = list;
	data = mpd_data_get_first(data);
	return data;
}

/**
 * @brief Free a full list of MpdData
 * @param list The list to free
 */
void
mpd_data_full_free(MpdData *list)
{
	MpdData *next;
	
//	list = mpd_data_get_first(list);
	while (list != NULL)
	{
		next = mpd_data_get_next(list);
		mpd_data_free(list);
		printf("un free de plus youhou \\o/\n");
		list = next;
	}
}

/*
 * @brief Get the rows selected on the tree_artist and add the corresponding songs to the playlist
 * @param gui A Emphasis_Gui
 */
void
emphasis_playlist_append_selected(Etk_Tree *tree, Emphasis_Type type)
{
	Etk_Tree_Row *row;
	Evas_List *rowlist, *list;
	MpdData *playlist=NULL, *tmplist;
	char *artist, *album;

	rowlist = etk_tree_selected_rows_get(tree);
	list = rowlist;
	
	if (type == EMPHASIS_ARTIST)
	{
		while (rowlist)
		{
			artist = etk_tree_row_data_get(evas_list_data(rowlist));
			tmplist = mpc_mlib_track_get(artist, NULL);
			
			if (!playlist)
			{
				playlist = tmplist;
			}
			else
			{
				playlist = mpd_data_concatenate(playlist, tmplist);
			}
			rowlist = evas_list_next(rowlist);		
		}
	}
	if (type == EMPHASIS_ALBUM)
	{
		artist = etk_object_data_get(ETK_OBJECT(tree), "artist");
		
		while (rowlist)
		{
			album = etk_tree_row_data_get(evas_list_data(rowlist));
			tmplist = mpc_mlib_track_get(artist, album);
			
			if (!playlist)
			{
				playlist = tmplist;
			}
			else
			{
				playlist = mpd_data_concatenate(playlist, tmplist);
			}
			rowlist = evas_list_next(rowlist);
		}
	}
	if (type == EMPHASIS_TRACK)
	{
		playlist = convert_rowlist_in_playlist_with_file(rowlist);
	}
	evas_list_free(list);
	mpc_playlist_add(playlist);	
}

void
emphasis_playlist_search_and_delete(Etk_Tree *tree, char *str, Emphasis_Type type)
{
	Etk_Tree_Col *col;
	Etk_Tree_Row *row;
	int num=-1;
	char *row_str;
	Evas_List *rowlist=NULL;
	MpdData *list;
	
	switch (type)
	{
		case EMPHASIS_ARTIST:
			num = 2;
			break;
		case EMPHASIS_ALBUM:
			num = 3;
			break;
		case EMPHASIS_TRACK:
			num = 0;
			break;
	}
	
	col = etk_tree_nth_col_get(tree, num);
	row = etk_tree_first_row_get(tree);
	while (row)
	{
		if (type == EMPHASIS_TRACK)
			etk_tree_row_fields_get(row, col, NULL, &row_str, NULL);
		else
			etk_tree_row_fields_get(row, col, &row_str, NULL);
			
		if (row_str && !(strcmp(row_str, str)))
		{
			rowlist = evas_list_append(rowlist, row);
		}
		row = etk_tree_next_row_get(row, ETK_FALSE, ETK_FALSE);
	}
	list = convert_rowlist_in_playlist_with_id(rowlist);
	mpc_playlist_delete(list);
}

char *
strdupnull(char *str)
{
	if (!str)
		return NULL;
	else
		return strdup(str);
}

Emphasis_Song *
convert_mpd_song(mpd_Song *src)
{
	Emphasis_Song *dest;

	if (!src)
		return NULL;

	dest = malloc(sizeof(Emphasis_Song));

	dest->file = strdupnull(src->file);
	dest->artist = strdupnull(src->artist);
	dest->title = strdupnull(src->title);
	dest->album = strdupnull(src->album);
	dest->track = strdupnull(src->track);
	dest->name = strdupnull(src->name);
	dest->date = strdupnull(src->date);
	dest->genre = strdupnull(src->genre);
	dest->composer = strdupnull(src->composer);
	dest->time = src->time;
	dest->pos = src->pos;
	dest->id = src->id;

	return dest;
}

Evas_List *
convert_mpd_data(MpdData *src)
{
	int loop=1;
	Evas_List *dest=NULL;

	if (!src)
		return NULL;

	while (loop)
	{
		Emphasis_Data *data;

		data = malloc(sizeof(Emphasis_Data));
		switch (src->type)
		{
			case MPD_DATA_TYPE_NONE:
				/** @todo TODO an emphasis_data_init */
				break;
			case MPD_DATA_TYPE_TAG:
				data->type = EMPHASIS_DATA_TYPE_TAG;
				data->tag = strdupnull(src->tag);
				break;
			case MPD_DATA_TYPE_DIRECTORY:
				data->type = EMPHASIS_DATA_TYPE_DIRECTORY;
				data->directory = strdupnull(src->directory);
				break;
			case MPD_DATA_TYPE_SONG:
				data->type = EMPHASIS_DATA_TYPE_SONG;
				data->song = convert_mpd_song(src->song);
				break;
			case MPD_DATA_TYPE_PLAYLIST:
				data->type = EMPHASIS_DATA_TYPE_PLAYLIST;
				data->playlist = strdupnull(src->playlist);
				break;
			case MPD_DATA_TYPE_OUTPUT_DEV:
				break;
		}
		dest = evas_list_append(dest, data);
		if (mpd_data_is_last(src))
			loop = 0;
		else
			src = mpd_data_get_next(src);
	}
	src = mpd_data_get_first(src);
	return dest;
}

mpd_Song *
convert_emphasis_song(Emphasis_Song *src)
{
	mpd_Song *dest;
	dest = malloc(sizeof(dest));

	dest = mpd_newSong();
	dest->file = strdupnull(src->file);
	dest->artist = strdupnull(src->artist);
	dest->title = strdupnull(src->title);
	dest->album = strdupnull(src->album);
	dest->track = strdupnull(src->track);
	dest->name = strdupnull(src->name);
	dest->date = strdupnull(src->date);
	dest->genre = strdupnull(src->genre);
	dest->composer = strdupnull(src->composer);
	dest->time = src->time;
	dest->pos = src->pos;
	dest->id = src->id;

	return dest;
}

MpdData *
convert_evas_list(Evas_List *src)
{
	Evas_List *list;
	MpdData *dest=NULL;
	Emphasis_Data *data=NULL;

	if (!src)
		return NULL;
		
	list = src;
	while (src)
	{	
		dest = mpd_new_data_struct_append(dest);
		data = evas_list_data(src);
		switch (data->type)
		{
			case EMPHASIS_DATA_TYPE_NONE :
				/** @todo TODO action */
				break;
			case EMPHASIS_DATA_TYPE_TAG :
				/** @todo TODO handle mpd type tag */
				dest->tag = strdupnull(data->tag);
				/* dest->tag_type = not handled */
				break;
			case EMPHASIS_DATA_TYPE_DIRECTORY:
				dest->type = MPD_DATA_TYPE_DIRECTORY;
				dest->directory = strdupnull(data->directory);
				break;
			case EMPHASIS_DATA_TYPE_SONG:
				dest->type = MPD_DATA_TYPE_SONG;
				dest->song = mpd_newSong();
				dest->song = convert_emphasis_song(data->song);
			case EMPHASIS_DATA_TYPE_PLAYLIST:
				dest->type = MPD_DATA_TYPE_PLAYLIST;
				dest->playlist = strdupnull(data->playlist);
				break;
		}
		src = evas_list_next(src);
	}
	
	src = list;
	dest = mpd_data_get_first(dest);
	return dest;
}

