#include "emphasis.h"
#include "emphasis_media.h"


/* TODO : documentation */
void
emphasis_tree_mlib_init(Emphasis_Player_Gui *player, Emphasis_Type type)
{
  Etk_Tree_Row *row;
  Eina_List *list;

  switch (type)
    {
      case EMPHASIS_ARTIST:
        list = mpc_mlib_artist_get();
        emphasis_tree_mlib_set(ETK_TREE(player->media.artist), list,
                               MPD_DATA_TYPE_TAG, NULL);
        break;
      case EMPHASIS_ALBUM:
        row = etk_tree_selected_row_get(ETK_TREE(player->media.artist));
       cb_tree_artist_selected(ETK_OBJECT(player->media.artist), row, player);
       break;
      case EMPHASIS_TRACK:
       row = etk_tree_selected_row_get(ETK_TREE(player->media.album));
       cb_tree_album_selected(ETK_OBJECT(player->media.album), row, player);
       break;
    }
}

/* TODO : documentation */
void
emphasis_tree_mlib_set(Etk_Tree *tree, Eina_List *list,
                       MpdDataType mpd_type, char *tag)
{
  Etk_Tree_Row *row;
  char **album_tag = NULL;

  etk_tree_clear(tree);
  if (mpd_type == MPD_DATA_TYPE_TAG)
    {
      if (tag != NULL)
        {
          album_tag = malloc(sizeof(char *) * 2);
          if(!album_tag) return;
          album_tag[0] = NULL;
          album_tag[1] = tag;
        }
      row = etk_tree_row_append(tree, NULL, etk_tree_nth_col_get(tree, 0), LABEL_MLIB_ALL, NULL);
      etk_tree_row_data_set(row, album_tag);
    }
  if (list)
    {
      emphasis_tree_mlib_append(tree, list, mpd_type, tag);
    }
}

/**
 * @brief Set a list to one of the medialib's tree
 * @param tree One of the medialib's tree
 * @param list The list to set in the treeview
 * @param mpd_type The type of the list
 */
void
emphasis_tree_mlib_append(Etk_Tree *tree, Eina_List *list,
                          MpdDataType mpd_type, char *tag)
{
  Etk_Tree_Col *col;
  Etk_Tree_Row *row;
  Emphasis_Data *data;
  Eina_List *first_list;
  char **album_tag;
  Emphasis_Type et;

  etk_tree_freeze(tree);
  col = etk_tree_nth_col_get(tree, 0);

  first_list = list;

  etk_tree_freeze(tree);
  if (mpd_type == MPD_DATA_TYPE_TAG)
    {
      et = (Emphasis_Type) etk_object_data_get(ETK_OBJECT(tree),
                                               "Emphasis_Type");
      while (list)
        {
          data = eina_list_data_get(list);
          row = etk_tree_row_append(tree, NULL, col, data->tag, NULL);
          if (et == EMPHASIS_ALBUM)
            {
              album_tag = malloc(sizeof(char *) * 2);
              if(!album_tag) return;
              album_tag[0] = strdupnull(data->tag);
              album_tag[1] = strdupnull(tag);
              etk_tree_row_data_set(row, album_tag);
            }
          else
            {
              etk_tree_row_data_set(row, strdup(data->tag));
            }
          list = eina_list_next(list);
        }
    }
  else
    {
      if (mpd_type == MPD_DATA_TYPE_SONG)
        {
          while (list)
            {
              data = eina_list_data_get(list);
              if (data->song->title)
                row = etk_tree_row_append(tree, NULL, col, data->song->title, NULL);
              else
                row = etk_tree_row_append(tree, NULL, col, data->song->file, NULL);

              etk_tree_row_data_set(row, strdup(data->song->file));
              list = eina_list_next(list);
            }
        }
      else
        {
          if (mpd_type == MPD_DATA_TYPE_PLAYLIST)
            {
              while (list)
                {
                  data = eina_list_data_get(list);
                  etk_tree_row_append(tree, NULL, col, data->playlist, NULL);
                  list = eina_list_next(list);
                }
            }
        }
    }

  etk_tree_thaw(tree);
  emphasis_list_free(first_list);
}

/**
 * @brief Add rows to the playlist treeview
 * @param tree A playlist tree
 * @param playlist The full playlist used by mpd
 */
void
emphasis_tree_pls_set(Etk_Tree *tree, Eina_List *playlist)
{
  Etk_Tree_Col *col_title, *col_time, *col_artist, *col_album;
  Etk_Tree_Row *row, *row_next;
  int id;
  char *title, *song_time;
  Emphasis_Data *data;
  Eina_List *list;

  list = playlist;
  col_title  = etk_tree_nth_col_get(tree, 0);
  col_time   = etk_tree_nth_col_get(tree, 1);
  col_artist = etk_tree_nth_col_get(tree, 2);
  col_album  = etk_tree_nth_col_get(tree, 3);

  etk_tree_freeze(tree);
  row = etk_tree_first_row_get(tree);

  /* LEAK : playlist and on NULL */
  while (row && playlist)
    {
      data = eina_list_data_get(playlist);
      row_next = etk_tree_row_next_get(row);
      id = (int) etk_tree_row_data_get(row);
      if (data->song->id != id)
        {
          etk_tree_row_delete(row);
        }
      else
        {
          playlist = eina_list_next(playlist);
        }
      row = row_next;
    }

  while (row)
    {
      row_next = etk_tree_row_next_get(row);
      etk_tree_row_delete(row);
      row = row_next;
    }

  while (playlist)
    {
      data = eina_list_data_get(playlist);
      if (!data->song->title)
        title = data->song->file;
      else
        title = data->song->title;
      asprintf(&song_time, "%d:%02d", (data->song->time) / 60,
               (data->song->time) % 60);
      row = etk_tree_row_append(tree, NULL, 
                                 col_title, NULL, NULL, title, 
                                 col_time, song_time,
                                 col_artist, data->song->artist, 
                                 col_album, data->song->album, NULL);

      etk_tree_row_data_set(row, (int *) data->song->id);
      free(song_time);
      playlist = eina_list_next(playlist);
    }

  emphasis_list_free(list);
  etk_tree_thaw(tree);
}

/**
 * @brief browse the tree_pls and set the playing image to the @e id
 * @note can be improve by stoping the while loop when the to cond has been validated
 */
void
emphasis_pls_mark_current(Etk_Tree *tree, int id)
{
  Etk_Tree_Col *col_current;
  Etk_Tree_Row *row;
  int row_id;
  char *image, *title;

  col_current = etk_tree_nth_col_get(tree, 0);

  etk_tree_freeze(tree);
  row = etk_tree_first_row_get(tree);

  while (row)
    {
      row_id = (int) etk_tree_row_data_get(row);
      etk_tree_row_fields_get(row, col_current, &image, NULL, &title, NULL);
      title = strdup(title);
      if (image)
        {
          etk_tree_row_fields_set(row, ETK_FALSE, col_current, NULL, NULL, title, NULL);
        }
      if (row_id == id)
        {
          etk_tree_row_fields_set(row, ETK_FALSE, col_current,
                                   PACKAGE_DATA_DIR "/images/note.png", NULL, title,
                                   NULL);
          etk_tree_row_scroll_to(row, ETK_FALSE);
        }
      free(title);
      /* TODO Don't read the whole tree stop after one mark/unmark */
      row = etk_tree_row_next_get(row);
    }

  etk_tree_thaw(tree);
}


/* Pane 3 : Playlists */
void
emphasis_pls_list_init(Emphasis_Player_Gui *player)
{
  Etk_Tree *pls_list;

  pls_list = ETK_TREE(player->media.pls_list);
  etk_tree_clear(pls_list);
#if defined(LIBMPD_0_12_4)
  emphasis_tree_mlib_append(pls_list, 
                            (mpc_list_playlists()),
                            MPD_DATA_TYPE_PLAYLIST,
                            NULL);
#else
  etk_tree_row_append(pls_list, NULL, etk_tree_nth_col_get(pls_list, 0),
                      "You need libmpd 0.12.4 order to use mpd playlists", NULL);
#endif
}

