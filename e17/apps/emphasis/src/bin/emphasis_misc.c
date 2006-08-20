#include "emphasis.h"
#include "emphasis_misc.h"

Evas_List *
evas_list_concatenate(Evas_List *head, Evas_List *tail)
{
  Evas_List *p;
  p = tail;

  while (p)
    {
      head = evas_list_append(head, evas_list_data(p));
      if (evas_list_alloc_error())
        {
          fprintf(stderr, "Error: Out of memory. List allocation failed.\n");
          exit(-1);
        }
      p = evas_list_next(p);
    }
  evas_list_free(tail);

  return head;
}

/**
 * @brief Convert a list of row in a Evas_List.
 *        And set the song->file element
 * @param rowlist An Evas_List of song's row
 * @return A list of song
 */
Evas_List *
convert_rowlist_in_playlist_with_file(Evas_List *rowlist)
{
  Evas_List *list = NULL, *first_rowlist;
  Etk_Tree_Row *row;
  Emphasis_Data *data = NULL;

  if (!rowlist)
    return NULL;

  first_rowlist = rowlist;

  while (rowlist)
    {
      row = evas_list_data(rowlist);

      data = malloc(sizeof(Emphasis_Data));
      data->type = MPD_DATA_TYPE_SONG;
      data->song = malloc(sizeof(Emphasis_Song));
      data->song->file = strdup(etk_tree_row_data_get(row));

      list = evas_list_append(list, data);
      rowlist = evas_list_next(rowlist);
    }

  rowlist = first_rowlist;
  return list;
}

/**
 * @brief Convert a list of row in a Evas_List.
 *        And set the song->id element
 * @param rowlist An Evas_List of song's row
 * @return A list of song
 */
Evas_List *
convert_rowlist_in_playlist_with_id(Evas_List *rowlist)
{
  Evas_List *list = NULL, *first_rowlist;
  Etk_Tree_Row *row;
  int id;
  Emphasis_Data *data = NULL;

  if (!rowlist)
    return NULL;

  first_rowlist = rowlist;

  while (rowlist)
    {
      row = evas_list_data(rowlist);
      id = (int) etk_tree_row_data_get(row);

      data = malloc(sizeof(Emphasis_Data));
      data->type = MPD_DATA_TYPE_SONG;
      data->song = malloc(sizeof(Emphasis_Song));
      data->song->id = id;

      list = evas_list_append(list, data);
      rowlist = evas_list_next(rowlist);
    }

  rowlist = first_rowlist;
  return list;
}

/*
 * @brief Get the rows selected on the tree_artist. 
 *        and add the corresponding songs to the playlist
 * @param gui A Emphasis_Gui
 */
void
emphasis_playlist_append_selected(Etk_Tree *tree, Emphasis_Type type)
{
  Etk_Tree_Row *row;
  Evas_List *rowlist, *list;
  Evas_List *playlist = NULL, *tmplist;
  char *artist, **album;

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
              playlist = evas_list_concatenate(playlist, tmplist);
            }
          rowlist = evas_list_next(rowlist);
        }
    }
  if (type == EMPHASIS_ALBUM)
    {
      while (rowlist)
        {
          album = etk_tree_row_data_get(evas_list_data(rowlist));
          if (album == NULL)
            {
              /* the first row is the All */
              row = etk_tree_first_row_get(tree);
              row = etk_tree_next_row_get(row, ETK_FALSE, ETK_FALSE);

              while (row)
                {
                  album = etk_tree_row_data_get(row);
                  playlist =
                    evas_list_concatenate(playlist,
                                          mpc_mlib_track_get(album[1],
                                                             album[0]));       
                  row = etk_tree_next_row_get(row, ETK_FALSE, ETK_FALSE);
                }
              break;
            }
          tmplist = mpc_mlib_track_get(album[1], album[0]);

          if (!playlist)
            {
              playlist = tmplist;
            }
          else
            {
              playlist = evas_list_concatenate(playlist, tmplist);
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
emphasis_playlist_search_and_delete(Etk_Tree *tree, char *str,
                                    Emphasis_Type type)
{
  Etk_Tree_Col *col;
  Etk_Tree_Row *row;
  int num = -1;
  char *row_str;
  Evas_List *rowlist = NULL;
  Evas_List *list;

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
        {
          etk_tree_row_fields_get(row, col, NULL, &row_str, NULL);
        }
      else
        {
          etk_tree_row_fields_get(row, col, &row_str, NULL);
        }

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
    {
      return NULL;
    }
  else
    {
      return strdup(str);
    }
}

void
strescape(char *str)
{
  int i = 0;

  if (str == NULL)
    {
      return;
    }

  while (str[i] != '\0')
    {
      if (str[i] == '/')
        {
          str[i] = '_';
        }
      i++;
    }
}

void 
pack_in_vbox(Etk_Widget *child, void *data)
{
  Etk_Widget *vbox;
  vbox = data;
  etk_box_append(ETK_BOX(vbox), child, ETK_BOX_START, ETK_BOX_EXPAND_FILL, 0);
}

void
etk_button_make_vertical(Etk_Widget *button)
{
  Etk_Widget *align;
  Etk_Widget *hbox;
  Etk_Widget *vbox;

  align = evas_list_data(etk_container_children_get(ETK_CONTAINER(button)));
  hbox  = evas_list_data(etk_container_children_get(ETK_CONTAINER(align)));

  etk_container_remove(ETK_CONTAINER(button), align);
  etk_container_remove(ETK_CONTAINER(align),  hbox);

  vbox = etk_vbox_new(ETK_FALSE, 0);
  etk_widget_show(vbox);
  etk_container_for_each_data(ETK_CONTAINER(hbox), pack_in_vbox, (void *)vbox);
  etk_container_add(ETK_CONTAINER(align),  vbox);
  etk_container_add(ETK_CONTAINER(button), align);

  etk_widget_pass_mouse_events_set(align, ETK_TRUE);
  etk_object_destroy(ETK_OBJECT(hbox));
}

/* Used for debug tiem to time */
void etk_container_inspect(Etk_Container *container, int lvl, int *to_trace)
{ 
#define PRINTF_TYPE_NAME(widget) printf("%s\n", \
                                          etk_type_name_get( \
                                            etk_object_object_type_get( \
                                              ETK_OBJECT(widget))));
  Evas_List *children;
  Etk_Widget *widget;
  int i = 0;

  if (lvl == 0)
    PRINTF_TYPE_NAME(ETK_WIDGET(container));

  children = etk_container_children_get(ETK_CONTAINER(container));
  
  while (children)
    {
      widget = evas_list_data(children);
      for (i=0; i<=lvl; i++)
        {
          if (to_trace[i])
            printf("|    ");
          else
            printf("     ");
        }
      printf("'");
      printf("----");
      PRINTF_TYPE_NAME(widget);

      if ((int) sizeof(to_trace) < lvl+3)
        to_trace = realloc(to_trace, sizeof(int) * (lvl+3));
      if (evas_list_next(children) && ETK_IS_CONTAINER(widget))
        to_trace[lvl+1] = 1;
      else
        to_trace[lvl+1] = 0;

      if (ETK_IS_CONTAINER(widget)) 
        {
            etk_container_inspect(ETK_CONTAINER(widget), lvl+1, to_trace);
        }
      children = evas_list_next(children);
    }
}

