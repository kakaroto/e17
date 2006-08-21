#include "emphasis.h"
#include "emphasis_callbacks.h"

/* TODO : update doc */
/**
 * @brief Quit the main loop
 */
Etk_Bool
cb_quit(Etk_Object *object, void *data)
{
  /* TODO :
   * config option : stop music on exit?
   */
  UNUSED(object);

  Emphasis_Player_Gui *player;
  Emphasis_Config     *config;

  player = data;

  config = config_load();

  /* TODO: maybe check emphasis' mode */
  etk_widget_geometry_get(ETK_WIDGET(player->full.window),
                          &(config->geometry.x), &(config->geometry.y),
                          &(config->geometry.w), &(config->geometry.h));

  config->colwidth.title  = 
    etk_tree_col_width_get(ETK_TREE_COL_GET(player->media.pls, 0));
  config->colwidth.time   =
    etk_tree_col_width_get(ETK_TREE_COL_GET(player->media.pls, 1));
  config->colwidth.artist =
    etk_tree_col_width_get(ETK_TREE_COL_GET(player->media.pls, 2));
  config->colwidth.album  =
    etk_tree_col_width_get(ETK_TREE_COL_GET(player->media.pls, 3));

  config->mode = player->state;

  config_save(config);
  config_free(config);
  
  mpc_disconnect();
  ecore_config_shutdown();
  etk_main_quit();

  return ETK_TRUE;
}

/* TODO : doc */
Etk_Bool
cb_media_quit(Etk_Object *object, void *data)
{
  Emphasis_Player_Gui *player;
  player = data;

  etk_toggle_button_toggle(ETK_TOGGLE_BUTTON(player->small.media));

  etk_widget_hide(ETK_WIDGET(object));
  return ETK_TRUE;
}

/**
 * @brief Stop the playing
 */
void
cb_button_stop_clicked(Etk_Object *object, void *data)
{
  UNUSED_CLICKED_PARAM
  mpc_stop();
}

/**
 * @brief Re-play the song from the start or play the previous song
 */
void
cb_button_prev_clicked(Etk_Object *object, void *data)
{
  UNUSED_CLICKED_PARAM
  mpc_prev();
}

/**
 * @brief Toggle play/pause the current song
 */
void
cb_button_play_clicked(Etk_Object *object, void *data)
{
  UNUSED_CLICKED_PARAM
  mpc_toggle_play_pause();
}

/**
 * @brief Play the next song
 */
void
cb_button_next_clicked(Etk_Object *object, void *data)
{
  UNUSED_CLICKED_PARAM
  mpc_next();
}

/**
 * @brief Toggle the random mode
 */
void
cb_toggle_random(Etk_Object *object, void *data)
{
  UNUSED_CLICKED_PARAM
  mpc_toggle_random();
}

/**
 * @brief Toggle the repeat mode
 */
void
cb_toggle_repeat(Etk_Object *object, void *data)
{
  UNUSED_CLICKED_PARAM
  mpc_toggle_repeat();
}

/* TODO : doc */
void
cb_toggle_full(Etk_Object *object, void *data)
{ 
  Emphasis_Player_Gui *player;
  player = data;

  if (player->state == EMPHASIS_FULL)
    {
      cb_switch_small(object, data);
    }
  else
    {
      cb_switch_full(object, data);
    }
}

/* TODO : doc */
void
cb_toggle_media(Etk_Object *object, void *data)
{
  /* Okay, now I have an ETK_FALSE. */
  Emphasis_Player_Gui *player;
  Etk_Bool checked;
  player = data;

  checked =
   etk_toggle_button_active_get(ETK_TOGGLE_BUTTON(object));
  
  if(checked == ETK_FALSE)
    {
      etk_widget_show(player->media.window);
    }
  else
    {
      etk_widget_hide(player->media.window);
    }
}

/**
 * @brief Seek in the song the same percent position than the progress bar click
 */
void
cb_seek_time(Etk_Object *object, Etk_Event_Mouse_Up_Down *event, void *data)
{
  UNUSED(data)
  Etk_Widget *progress;
  int x_click, widget_width;

  progress = ETK_WIDGET(object);
  x_click = event->widget.x;

  etk_widget_geometry_get(progress, NULL, NULL, &widget_width, NULL);

  mpc_seek((float) x_click / widget_width);
}

void
cb_vol_image_clicked(Etk_Object *object,
                     Etk_Event_Mouse_Up_Down *event,
                     void *data)
{
  UNUSED(event);
  Emphasis_Player_Gui *player;

  player = data;
  if (ETK_WIDGET(object) == player->full.sound_low ||
      ETK_WIDGET(object) == player->small.sound_low)
    {
      mpc_change_vol(mpc_get_vol() - 5);
    }
  else if (ETK_WIDGET(object) == player->full.sound_high ||
           ETK_WIDGET(object) == player->small.sound_high)
    {
      mpc_change_vol(mpc_get_vol() + 5);
    }
}

/**
 * @brief Changed the mpd output volume level
 */
void
cb_vol_slider_value_changed(Etk_Object *object, double value, void *data)
{
  UNUSED(object)
  UNUSED(data)
  
  mpc_change_vol(value);
}

/**
 * @brief Get the name of the selected artists, search for his albums.
 *        And set the album tree with.
 */
void
cb_tree_artist_selected(Etk_Object *object, Etk_Tree_Row *row, void *data)
{
  UNUSED(object)
  UNUSED(row)
  Emphasis_Player_Gui *player;
  Evas_List *artist_sel;
  char *artist;

  player = data;

  artist_sel = etk_tree_selected_rows_get(ETK_TREE(player->media.artist));
  artist     = etk_tree_row_data_get(evas_list_data(artist_sel));

  if (artist == NULL)
    {
      emphasis_tree_mlib_set(ETK_TREE(player->media.album),
                             mpc_mlib_album_get(NULL),
                             MPD_DATA_TYPE_TAG, NULL);
      etk_tree_row_select
       (etk_tree_first_row_get(ETK_TREE(player->media.album)));

      return;
    }
  emphasis_tree_mlib_set(ETK_TREE(player->media.album), NULL,
                         MPD_DATA_TYPE_TAG, NULL);
  while (artist_sel)
    {
      artist = etk_tree_row_data_get(evas_list_data(artist_sel));
      emphasis_tree_mlib_append(ETK_TREE(player->media.album),
                                mpc_mlib_album_get(artist),
                                MPD_DATA_TYPE_TAG, artist);
      artist_sel = evas_list_next(artist_sel);
    }

  etk_tree_row_select(etk_tree_first_row_get(ETK_TREE(player->media.album)));
}

/**
 * @brief Get the selected albums and search for songs wich matches with these albums.
 *        Then set the tree track with that stuff.
 */
void
cb_tree_album_selected(Etk_Object *object, Etk_Tree_Row *row, void *data)
{
  UNUSED(object)
  UNUSED(row)
  Emphasis_Player_Gui *player;
  Evas_List *artist_sel, *album_sel;
  char **album_tag;
  char *artist_tag;

  player = data;


  album_sel = etk_tree_selected_rows_get(ETK_TREE(player->media.album));
  album_tag = etk_tree_row_data_get(evas_list_data(album_sel));

  if (album_tag == NULL)
    {
      etk_tree_clear(ETK_TREE(player->media.track));
      artist_sel = etk_tree_selected_rows_get(ETK_TREE(player->media.artist));
      while (artist_sel)
        {
          artist_tag = etk_tree_row_data_get(evas_list_data(artist_sel));
          emphasis_tree_mlib_append(ETK_TREE(player->media.track),
                                    mpc_mlib_track_get(artist_tag, NULL),
                                    MPD_DATA_TYPE_SONG, NULL);
          artist_sel = evas_list_next(artist_sel);
        }
      return;
    }

  etk_tree_clear(ETK_TREE(player->media.track));
  while (album_sel)
    {
      album_tag = etk_tree_row_data_get(evas_list_data(album_sel));
      emphasis_tree_mlib_append(ETK_TREE(player->media.track),
                                mpc_mlib_track_get(album_tag[1], album_tag[0]),
                                MPD_DATA_TYPE_SONG, NULL);
      album_sel = evas_list_next(album_sel);
    }
}

/**
 * @brief Set a drag widget from a list of artist selected
 * @param data A common drag widget
 */
void
cb_drag_artist(Etk_Object *object, void *data)
{
  Etk_Widget *drag_menu, *menu_item;
  Etk_Drag *drag;
  Etk_Tree *tree;
  Evas_List *rowlist, *next;
  char *artist;
  const char **types;
  unsigned int num_types;
  Evas_List *playlist = NULL, *tmplist;

  tree = ETK_TREE(object);
  drag = ETK_DRAG((ETK_WIDGET(tree))->drag);
  rowlist = etk_tree_selected_rows_get(tree);

  drag_menu = etk_menu_new();

  while(rowlist)
    {
      artist = etk_tree_row_data_get(evas_list_data(rowlist));
      tmplist = mpc_mlib_track_get(artist, NULL);
      
      if(artist)
        {
          menu_item = etk_menu_item_new_with_label(artist);
        }
      else
        {
          menu_item = etk_menu_item_new_with_label("All");
        }

      etk_menu_shell_append(ETK_MENU_SHELL(drag_menu),
                            ETK_MENU_ITEM(menu_item));

      if(!playlist)
        {
          playlist = tmplist;
        }
      else
        {
          playlist = evas_list_concatenate(playlist, tmplist);
        }
      
      next = evas_list_next(rowlist);
      rowlist = next;
    }
  evas_list_free(rowlist);

  types = calloc(1, sizeof(char));
  num_types = 1;
  types[0] = strdup("Emphasis_Playlist");

  etk_drag_types_set(drag, types, num_types);
  etk_drag_data_set(drag, playlist, 1);

  etk_container_add(ETK_CONTAINER(drag), drag_menu);
  ((Emphasis_Player_Gui*)data)->media.drag = ETK_WIDGET(drag);
}

/**
 * @brief Set a drag widget from a list of albums selected
 * @param data An Emphasis_Gui
 */
void
cb_drag_album(Etk_Object *object, void *data)
{
  Etk_Widget *drag_menu, *menu_item;
  Etk_Drag *drag;
  Etk_Tree *tree;
  Evas_List *rowlist;
  char **album;
  const char **types;
  unsigned int num_types;
  Evas_List *playlist = NULL, *tmplist;

  tree = ETK_TREE(object);
  drag = ETK_DRAG((ETK_WIDGET(object))->drag);
  rowlist = etk_tree_selected_rows_get(tree);

  drag_menu = etk_menu_new();

  while (rowlist)
    {
      album = etk_tree_row_data_get(evas_list_data(rowlist));
      tmplist = mpc_mlib_track_get(album[1], album[0]);

      if (album[0])
        {
          menu_item = etk_menu_item_new_with_label(album[0]);
        }
      else
        {
          menu_item = etk_menu_item_new_with_label("All");
        }

      etk_menu_shell_append(ETK_MENU_SHELL(drag_menu),
                            ETK_MENU_ITEM(menu_item));
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

  types = calloc(1, sizeof(char));
  num_types = 1;
  types[0] = strdup("Emphasis_Playlist");

  etk_drag_types_set(drag, types, num_types);
  etk_drag_data_set(drag, playlist, 1);

  etk_container_add(ETK_CONTAINER(drag), drag_menu);

  ((Emphasis_Player_Gui*)data)->media.drag = ETK_WIDGET(drag);
}

/**
 * @brief Set a drag widget from a list of selected song's row
 * @param data A drag widget
 * @note this callback will set the data's drag and the object's drag
 */
void
cb_drag_track(Etk_Object *object, void *data)
{
  Etk_Widget *drag_menu, *menu_item;
  Etk_Drag *drag;
  Etk_Tree *tree;
  Evas_List *rowlist;
  char *title;
  const char **types;
  unsigned int num_types;
  Evas_List *playlist;

  tree = ETK_TREE(object);
  drag = ETK_DRAG((ETK_WIDGET(object))->drag);

  drag_menu = etk_menu_new();

  rowlist = etk_tree_selected_rows_get(tree);
  playlist = convert_rowlist_in_playlist_with_file(rowlist);
  while (rowlist)
    {
      etk_tree_row_fields_get(evas_list_data(rowlist),
                              etk_tree_nth_col_get(tree, 0), &title, NULL);

      menu_item = etk_menu_item_new_with_label(title);
      etk_menu_shell_append(ETK_MENU_SHELL(drag_menu),
                            ETK_MENU_ITEM(menu_item));

      rowlist = evas_list_next(rowlist);
    }

  types = calloc(1, sizeof(char));
  num_types = 1;
  types[0] = strdup("Emphasis_Playlist");

  etk_drag_types_set(drag, types, num_types);
  etk_drag_data_set(drag, playlist, 1);

  etk_tree_row_fields_get(evas_list_data(rowlist),
                          etk_tree_nth_col_get(tree, 0), &title, NULL);

  etk_container_add(ETK_CONTAINER(drag), drag_menu);

  ((Emphasis_Player_Gui*)data)->media.drag = ETK_WIDGET(drag);
}

/**
 * @brief Get the playlist draged. Add it to the current playlist.
 *        And set the playlist tree up to date.
 * @param data A drag widget with a playlist in his data
 */
void
cb_drop_song(Etk_Object *object, void *event, void *data)
{
  UNUSED(event)
  Etk_Tree *tree;
  Etk_Drag *drag;
  Evas_List *list;

  tree = ETK_TREE(object);

  drag = ETK_DRAG(((Emphasis_Player_Gui*)data)->media.drag);
  if (drag == NULL ||
      drag->types == NULL ||
      drag->types[0] == NULL || 
      strcmp("Emphasis_Playlist", drag->types[0]))
    {
      return;
    }
  
  list = drag->data;
  mpc_playlist_add(list);

  free(drag->types[0]);
  free(drag->types);

  /*
     Etk_Widget *wid;
     Evas_List *evaslist;
     evaslist = etk_container_children_get(ETK_CONTAINER(drag));
     while (evaslist)
     {
     etk_container_remove(ETK_CONTAINER(drag), evas_list_data(evaslist));
     etk_object_destroy(ETK_OBJECT(evas_list_data(evaslist)));
     evaslist = evas_list_next(evaslist);
     }
   */
}

/**
 * @brief Double-click on a row add the artist's song to the playlist
 */
void
cb_tree_mlib_clicked(Etk_Object *object, Etk_Tree_Row *row,
                     Etk_Event_Mouse_Up_Down *event, void *data)
{
  Emphasis_Player_Gui *player;
  Emphasis_Type et;
  char *str;

  player = data;
  et = (Emphasis_Type) etk_object_data_get(object, "Emphasis_Type");
  if ((event->button == 1) && (event->flags & EVAS_BUTTON_DOUBLE_CLICK))
    {
      emphasis_playlist_append_selected(ETK_TREE(object), et);
      mpc_play_if_stopped();
    }
  if (event->button == 2)
    {
      etk_tree_row_fields_get(row, etk_tree_nth_col_get(ETK_TREE(object), 0),
                              &str, NULL);

      emphasis_playlist_search_and_delete(ETK_TREE(player->media.pls), str, et);
    }
}

/**
 * @brief Play the song double-clicked
 */
void
cb_tree_pls_clicked(Etk_Object *object, Etk_Tree_Row *row,
                    Etk_Event_Mouse_Up_Down *event, void *data)
{
  UNUSED(data)
  int id;

  if ((event->button == 1) && (event->flags & EVAS_BUTTON_DOUBLE_CLICK))
    {
      id = (int) etk_tree_row_data_get(row);
      mpc_play_id(id);
    }
  if (event->button == 1)
    {
      ETK_TREE(object)->last_selected = row;
    }
}

void
cb_emphasis_bindings_key(Etk_Object *object, Etk_Event_Key_Up_Down *event,
                         void *data)
{
  UNUSED(data)
  UNUSED(object)
  if (!strcmp(event->key, "z"))
    {
      cb_toggle_random(NULL, NULL);
      return;
    }
  if (!strcmp(event->key, "r"))
    {
      cb_toggle_repeat(NULL, NULL);
      return;
    }
  if (!strcmp(event->key, "s"))
    {
      cb_button_stop_clicked(NULL, NULL);
      return;
    }
  if (!strcmp(event->key, "p"))
    {
      cb_button_play_clicked(NULL, NULL);
      return;
    }
  if (!strcmp(event->key, "greater"))
    {
      cb_button_next_clicked(NULL, NULL);
      return;
    }
  if (!strcmp(event->key, "less"))
    {
      cb_button_prev_clicked(NULL, NULL);
      return;
    }
}

/**
 * @brief Fast find in mlib tree
 */
void
cb_mlib_bindings_key(Etk_Object *object, Etk_Event_Key_Up_Down *event,
                     void *data)
{
  Etk_Tree *tree;
  Etk_Tree_Row *row, *row_next;
  Etk_Tree_Col *col;
  Emphasis_Player_Gui *player;
  char *str;

  if (!event->string)
    return;

  tree = ETK_TREE(object);
  player = data;

  if (!strcmp(event->key, "a")
      && evas_key_modifier_is_set(event->modifiers, "Control"))
    {
      etk_tree_select_all(tree);

      if (object == ETK_OBJECT(player->media.track))
        return;

      row = etk_tree_first_row_get(tree);
      etk_tree_row_unselect(row);
      if (object == ETK_OBJECT(player->media.artist))
        {
          cb_tree_artist_selected(object, row, data);
        }
      if (object == ETK_OBJECT(player->media.album))
        {
          cb_tree_album_selected(object, row, data);
        }

      return;
    }

  if (!strcmp(event->key, "Tab")
      || evas_key_modifier_is_set(event->modifiers, "Control"))
    return;

  col = etk_tree_nth_col_get(tree, 0);

/**/ 
  const char *cur_title, *base_title;
  char *title, *filter = NULL;
  int length;
  Emphasis_Type type;

  /*get the base title of the tree */
  base_title = etk_object_data_get(ETK_OBJECT(tree), "title");    
  /*get the current title of tree */
  cur_title = etk_tree_col_title_get(col);      
  type = (Emphasis_Type) etk_object_data_get(ETK_OBJECT(tree), "Emphasis_Type");

  if (!strcmp(base_title, cur_title))
    {
      if (!strcmp("BackSpace", event->keyname) ||
          !strcmp("Escape", event->keyname)    ||
          !strcmp("Return", event->keyname))
        return;

      asprintf(&title, "%s starting with : %s", base_title, event->string);
    }
  else
    {
      if (!strcmp("BackSpace", event->keyname))
        {
          title = strdup(cur_title);
          title[strlen(title) - 1] = '\0';
          emphasis_tree_mlib_init(player, type);
          if (strlen(title) == strlen(base_title) + 17)
            {
              etk_tree_col_title_set(col, base_title);
              return;
            }
        }
      else if (!strcmp("Escape", event->keyname))
        {
          emphasis_tree_mlib_init(player, type);
          etk_tree_col_title_set(col, base_title);
          return;
        }
      else if (!strcmp("Return", event->keyname))
        {
          etk_tree_col_title_set(col, base_title);
          return;
        }
      else
        asprintf(&title, "%s%s", cur_title, event->string);
    }

  etk_tree_col_title_set(etk_tree_nth_col_get(tree, 0), title);
  length = strlen(base_title) + 17;     
  filter = strdup(&title[length]);
  length = strlen(filter);
/**/ 

  row = etk_tree_first_row_get(tree);
  while (row)
    {
      row_next = etk_tree_next_row_get(row, ETK_FALSE, ETK_FALSE);
      etk_tree_row_fields_get(row, col, &str, NULL);
      if (!strncasecmp("The ", str, 4))
        {
          str = str + 4;
        }
      if (!strcmp("All", str) || (strncasecmp(filter, str, length) != 0))
        etk_tree_row_del(row);
      row = row_next;
    }

  etk_tree_row_select(etk_tree_first_row_get(tree));
  free(title);
  free(filter);
}

/**
 * @brief If right-clicked: popdown the contextual menu on the playlist
 */
void
cb_pls_contextual_menu(Etk_Object *object, Etk_Event_Mouse_Up_Down *event,
                       void *data)
{
  UNUSED(object)
  Emphasis_Gui *gui;

  gui = data;
  if (event->button == 3)
    {
      etk_menu_popup(ETK_MENU(gui->menu));
      etk_widget_show_all(gui->menu);
    }
}

/**
 * @brief Clear the playlist
 */
void
cb_playlist_clear(Etk_Object *object, Etk_Event_Mouse_Up_Down *event,
                  void *data)
{
  UNUSED(object)
  UNUSED(event)
  UNUSED(data)

  mpc_playlist_clear();
}

/**
 * @brief Callback function of bindings key on the playlist
 */
void
cb_pls_bindings_key(Etk_Object *object, Etk_Event_Key_Up_Down *event, 
                    void *data)
{
  UNUSED(object)
  Emphasis_Player_Gui *player;
  Evas_List *rowlist;
  Evas_List *list;

  player = data;

  if (!strcmp(event->key, "d") || !strcmp(event->key, "Delete"))
    {
      rowlist = etk_tree_selected_rows_get(ETK_TREE(player->media.pls));
      if (rowlist)
        {
          list = convert_rowlist_in_playlist_with_id(rowlist);
          mpc_playlist_delete(list);
        }
    }
  etk_widget_redraw_queue(ETK_WIDGET(player->media.pls));
}

/**
 * @brief Delete all the selected item from the playlist
 */
void
cb_playlist_delete(Etk_Object *object, void *data)
{
  UNUSED(object)
  Emphasis_Player_Gui *player;
  Evas_List *rowlist;
  Evas_List *list;

  player = data;

  rowlist = etk_tree_selected_rows_get(ETK_TREE(player->media.pls));
  list = convert_rowlist_in_playlist_with_id(rowlist);

  mpc_playlist_delete(list);
}

/**
 * @brief Request an database update
 */
void
cb_database_update(Etk_Object *object, void *data)
{
  UNUSED(object)
  Emphasis_Player_Gui *player;
  player = data;

  mpc_database_update();
  emphasis_tree_mlib_init(player, EMPHASIS_ARTIST);

  /* Clear old search */
  etk_tree_col_title_set(ETK_TREE_COL_GET(player->media.artist, 0), "Artist");
  etk_tree_col_title_set(ETK_TREE_COL_GET(player->media.album , 0), "Album");
  etk_tree_col_title_set(ETK_TREE_COL_GET(player->media.track , 0), "Track");

  /* TODO redraw */
}

void
cb_switch_full(Etk_Object *object, void *data)
{
  /* TODO : this function ;) */
  UNUSED(object);
//  int x, y, w, h;

  Emphasis_Player_Gui *player;
  player = data;

  if(player->state == EMPHASIS_FULL)
    {
      return;
    }
  player->state = EMPHASIS_FULL;

//  etk_window_geometry_get(ETK_WINDOW(player->small.window), &x, &y, &w, &h);
  etk_widget_hide(player->small.window);
  etk_widget_hide(player->media.window);
  
  etk_container_remove(ETK_CONTAINER(player->media.window), player->media.root);
  etk_box_append(ETK_BOX(player->full.root),
                     player->media.root,
                     ETK_BOX_START,
                     ETK_BOX_EXPAND_FILL, 0);
 
  emphasis_player_toggle_full(player, ETK_TRUE);

  etk_widget_show(player->full.window);
//  etk_window_move(ETK_WINDOW(player->full.window), x, y);
}

void
cb_switch_small(Etk_Object *object, void *data)
{
  /* TODO : this function ;) */
  UNUSED(object);
//  int x, y, w, h;

  Emphasis_Player_Gui *player;
  player = data;

  if(player->state == EMPHASIS_SMALL)
    {
      return;
    }
  player->state = EMPHASIS_SMALL;

//  etk_window_geometry_get(ETK_WINDOW(player->full.window), &x, &y, &w, &h);
  etk_widget_hide(player->full.window);

  etk_container_add(ETK_CONTAINER(player->media.window), player->media.root);
  emphasis_player_toggle_full(player, ETK_FALSE);

  if(etk_toggle_button_active_get(ETK_TOGGLE_BUTTON(player->small.media)))
    {
      etk_widget_show(player->media.window);
    }
  etk_widget_show(player->small.window);
//  etk_window_move(ETK_WINDOW(player->small.window), x, y);
}

void
cb_media_button_lib_clicked(Etk_Object *object, 
                            void *data)
{
  UNUSED(object);

  Emphasis_Player_Gui *player;
  player = data;

  etk_notebook_current_page_set(ETK_NOTEBOOK(player->media.notebook), 0);
}

void
cb_media_button_search_clicked(Etk_Object *object, 
                               void *data)
{
  UNUSED(object);

  Emphasis_Player_Gui *player;
  player = data;

  etk_notebook_current_page_set(ETK_NOTEBOOK(player->media.notebook), 1);
}

void
cb_media_button_playlists_clicked(Etk_Object *object, 
                                  void *data)
{
  UNUSED(object);

  Emphasis_Player_Gui *player;
  player = data;

  etk_notebook_current_page_set(ETK_NOTEBOOK(player->media.notebook), 2);
  emphasis_pls_list_init(player);
}

void
cb_media_button_stats_clicked(Etk_Object *object, 
                              void *data)
{
  UNUSED(object);

  Emphasis_Player_Gui *player;
  player = data;

  etk_notebook_current_page_set(ETK_NOTEBOOK(player->media.notebook), 3);
}

void
cb_config_show(Etk_Object *object, void *data)
{
  Emphasis_Gui *gui;
  Emphasis_Config_Gui *configgui;
  UNUSED(object);

  gui = data;

  if(gui->config_gui == NULL)
    {
      gui->config_gui = malloc(sizeof(Emphasis_Config_Gui));
      emphasis_init_configgui(gui->config_gui);
      gui->config_gui->data = data;
    }
  configgui = gui->config_gui;

  emphasis_configgui_autoset(configgui);
  etk_widget_show_all(configgui->window);
}

Etk_Bool
cb_config_hide(Etk_Object *object, void *data)
{
  Emphasis_Config_Gui *configgui;
  UNUSED(object);

  configgui = data;

  etk_widget_hide_all(configgui->window);
  return ETK_FALSE;
}

void
cb_config_ok(Etk_Object *object, void *data)
{
  Emphasis_Config_Gui *configgui;

  configgui = data;

  emphasis_configgui_save(configgui);
  emphasis_try_connect(configgui->data);

  cb_config_hide(object, data);
}

void
cb_media_pls_list_row_clicked(Etk_Object *object, Etk_Tree_Row *row, 
                              Etk_Event_Mouse_Up_Down *event, void *data)
{
  Emphasis_Player_Gui *player;
  Etk_Tree *tree;
  Etk_Tree_Col *col;
  char *playlist_name;

  player = data;
  if (event->button == 1)
    {
      if (event->flags == EVAS_BUTTON_NONE)
        {
          tree = ETK_TREE(object);
          col = etk_tree_nth_col_get(tree, 0);

          etk_tree_clear(ETK_TREE(player->media.pls_content));
          etk_tree_row_fields_get(row, col, &playlist_name, NULL);
          emphasis_tree_mlib_append(ETK_TREE(player->media.pls_content), 
                                    (mpc_get_playlist_content(playlist_name)),
                                    MPD_DATA_TYPE_SONG,
                                    NULL);
        }
      else if (event->flags == EVAS_BUTTON_DOUBLE_CLICK)
        {
          cb_media_pls_load_clicked(NULL, data);
        }
    }
}

void
cb_media_pls_save_clicked(Etk_Object *object, void *data)
{
  UNUSED(object);
#if defined(LIBMPD_0_12_4)
  Emphasis_Player_Gui *player;
  Etk_Widget *entry;
  
  player = data;
  entry = player->media.pls_entry_save;
  if (etk_widget_is_visible(entry)) 
    {
      if (etk_entry_text_get(ETK_ENTRY(entry)))
        {
          mpc_save_playlist((char *) etk_entry_text_get(ETK_ENTRY(entry)));
        }
      etk_entry_text_set(ETK_ENTRY(entry), "");
      etk_widget_hide(entry);
    }
  else
    {
      etk_widget_show(entry);
      etk_widget_focus(entry);
    }
  emphasis_pls_list_init(player);
#else
  UNUSED(data);
#endif
}

void
cb_media_pls_load_clicked(Etk_Object *object, void *data)
{
  UNUSED(object);
#if defined(LIBMPD_0_12_4)
  Emphasis_Player_Gui *player;
  Etk_Tree_Col *col;
  Etk_Tree_Row *row;
  char *playlist_name;

  player = data;
  col = etk_tree_nth_col_get(ETK_TREE(player->media.pls_list), 0);
  row = etk_tree_selected_row_get(ETK_TREE(player->media.pls_list));
  etk_tree_row_fields_get(row, col, &playlist_name, NULL);

  mpc_playlist_clear();
  etk_tree_multiple_select_set(ETK_TREE(player->media.pls_content), ETK_TRUE);
  etk_tree_select_all(ETK_TREE(player->media.pls_content));
  emphasis_playlist_append_selected(ETK_TREE(player->media.pls_content),
                                    EMPHASIS_TRACK);
  etk_tree_unselect_all(ETK_TREE(player->media.pls_content));
  mpc_play_if_stopped();
#else
  UNUSED(data);
#endif
}

void
cb_media_pls_del_clicked(Etk_Object *object, void *data)
{
  UNUSED(object);
#if defined(LIBMPD_0_12_4)
  Emphasis_Player_Gui *player;
  Etk_Tree_Col *col;
  Etk_Tree_Row *row;
  char *playlist_name;

  player = data;
  col = etk_tree_nth_col_get(ETK_TREE(player->media.pls_list), 0);
  row = etk_tree_selected_row_get(ETK_TREE(player->media.pls_list));
  etk_tree_row_fields_get(row, col, &playlist_name, NULL);

  mpc_delete_playlist(playlist_name);
  emphasis_pls_list_init(player);
#else
  UNUSED(data);
#endif
}

void
cb_media_pls_save_key_down(Etk_Object *object, Etk_Event_Key_Up_Down *event,
                           void *data)
{
  if (!strcmp(event->key, "Return"))
    {
      etk_widget_unfocus(ETK_WIDGET(object));
      cb_media_pls_save_clicked(NULL, data);
    }
}
