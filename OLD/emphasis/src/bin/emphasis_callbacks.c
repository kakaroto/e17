#include "emphasis.h"
#include "emphasis_callbacks.h"
#include <ctype.h>
#define xrand(min, max) (rand() % (max - min)) + min + 1

/* TODO : update doc */
/**
 * @brief Quit the main loop
 */
Etk_Bool
cb_quit(Etk_Object *object, void *data)
{
  UNUSED(object);

  Emphasis_Player_Gui *player;
  Emphasis_Config     *config;

  player = data;

  /* Save the current config settings */
  config = config_load();
  if(config)
    {
      if(player)
        {
          etk_widget_geometry_get(ETK_WIDGET(player->full.window),
                                  &(config->geometry.x), &(config->geometry.y),
                                  &(config->geometry.w), &(config->geometry.h));
            
          etk_widget_geometry_get(ETK_WIDGET(player->media.window),
                                  NULL, NULL,
                                  &(config->geometry.media_w),
                                  &(config->geometry.media_h));

          config->colwidth.title  = 
            etk_tree_col_width_get(ETK_TREE_COL_GET(player->media.pls, 0));
          config->colwidth.time   =
            etk_tree_col_width_get(ETK_TREE_COL_GET(player->media.pls, 1));
          config->colwidth.artist =
            etk_tree_col_width_get(ETK_TREE_COL_GET(player->media.pls, 2));
          config->colwidth.album  =
            etk_tree_col_width_get(ETK_TREE_COL_GET(player->media.pls, 3));
          config->media_paned     =
           etk_paned_position_get(ETK_PANED(player->media.paned));

         config->mode = player->state;
         config->pls_show =
          etk_toggle_button_active_get(ETK_TOGGLE_BUTTON(player->small.media));
        } /* if player */
      config->crossfade = mpc_get_crossfade();

      if(config->stop_on_exit)
        {
          cb_button_stop_clicked(NULL, NULL);
        }

      config_save(config);
      config_free(config);
  } /* if config */
  
  mpc_disconnect();
  ecore_config_shutdown();
  etk_main_quit();

  return ETK_FALSE; /* don't set to TRUE */
}

/* TODO : doc */
Etk_Bool
cb_media_quit(Etk_Object *object, void *data)
{
  Emphasis_Player_Gui *player;
  player = data;

  if(data) etk_toggle_button_toggle(ETK_TOGGLE_BUTTON(player->small.media));

  etk_widget_hide(ETK_WIDGET(object));
  return ETK_FALSE; /* don't quit, hide */
}
Etk_Bool
cb_pack_quit(Etk_Object *object, void *data)
{
  Emphasis_Player_Gui *player;
  player = data;

  if(data && player->small.packed == ETK_FALSE)
    {
      /* We are in the tiny mode */
      Etk_Widget *win = NULL;

      win = (object == (Etk_Object*)player->small.cov.window) ?
            player->small.ctr.window : player->small.cov.window;
      if (etk_widget_is_visible(win) == ETK_TRUE)
        /* The other window is visible, don't quit, hide this window */
        { return cb_window_hide(object, data); }

      etk_widget_hide(player->small.cov.window);
      etk_widget_hide(player->small.ctr.window);

      etk_container_remove(player->small.cov.root);
      etk_container_remove(player->small.ctr.root);

      etk_box_append(ETK_BOX(player->small.root), player->small.cov.root,
                     ETK_BOX_START, ETK_BOX_FILL, 0);
      etk_box_append(ETK_BOX(player->small.root), player->small.ctr.root,
                     ETK_BOX_START, ETK_BOX_EXPAND_FILL, 0);

      if(player->state == EMPHASIS_SMALL)
        { cb_quit(NULL, player); }
    }

  return ETK_TRUE; /* else, quit */
}

Etk_Bool
cb_window_hide(Etk_Object *object, void *data)
{
  UNUSED(data);
  etk_widget_hide(ETK_WIDGET(object));
  return ETK_FALSE; /* don't quit, hide */
}

/**
 * @brief Stop the playing
 */
Etk_Bool
cb_button_stop_clicked(Etk_Object *object, void *data)
{
  UNUSED_CLICKED_PARAM
  mpc_stop();

  return ETK_TRUE;
}

/**
 * @brief Re-play the song from the start or play the previous song
 */
Etk_Bool
cb_button_prev_clicked(Etk_Object *object, void *data)
{
  UNUSED_CLICKED_PARAM
  mpc_prev();
  return ETK_TRUE;
}

/**
 * @brief Toggle play/pause the current song
 */
Etk_Bool
cb_button_play_clicked(Etk_Object *object, void *data)
{
  UNUSED_CLICKED_PARAM
  mpc_toggle_play_pause();
  return ETK_TRUE;
}

/**
 * @brief Play the next song
 */
Etk_Bool
cb_button_next_clicked(Etk_Object *object, void *data)
{
  UNUSED_CLICKED_PARAM
  mpc_next();
  return ETK_TRUE;
}

/**
 * @brief Toggle the random mode
 */
Etk_Bool
cb_toggle_random(Etk_Object *object, void *data)
{
  UNUSED_CLICKED_PARAM
  mpc_toggle_random();
  return ETK_TRUE;
}

/**
 * @brief Toggle the repeat mode
 */
Etk_Bool
cb_toggle_repeat(Etk_Object *object, void *data)
{
  UNUSED_CLICKED_PARAM
  mpc_toggle_repeat();
  return ETK_TRUE;
}

/* TODO : doc */
Etk_Bool
cb_toggle_full(Etk_Object *object, void *data)
{ 
  Emphasis_Player_Gui *player;
  player = data;

  if(!data) return ETK_TRUE;

  if (player->state == EMPHASIS_FULL)
    {
      cb_switch_small(object, data);
    }
  else
    {
      cb_switch_full(object, data);
    }
  return ETK_TRUE;
}

/* TODO : doc */
Etk_Bool /* why? */
cb_toggle_media(Etk_Object *object, void *data)
{
  /* Okay, now I have an ETK_FALSE. */
  Emphasis_Player_Gui *player;
  player = data;

  if(!data) return ETK_TRUE;

  if(etk_widget_is_visible(player->media.window))
    {
      etk_widget_hide(player->media.window);
    }
  else
    {
      etk_widget_show(player->media.window);
    }

  return ETK_TRUE; /* why? */
}

/**
 * @brief Seek in the song the same percent position than the progress bar click
 */
Etk_Bool
cb_seek_time(Etk_Object *object, Etk_Event_Mouse_Down *event, void *data)
{
  UNUSED(data)
  Etk_Widget *progress;
  int x_click, widget_width;

  if(event->button != 1) { return ETK_TRUE; }

  progress = ETK_WIDGET(object);
  x_click = event->widget.x;

  etk_widget_geometry_get(progress, NULL, NULL, &widget_width, NULL);

  mpc_seek((float) x_click / widget_width);
  return ETK_TRUE;
}

Etk_Bool
cb_vol_image_clicked(Etk_Object *object,
                     Etk_Event_Mouse_Down *event,
                     void *data)
{
  UNUSED(event);
  Emphasis_Player_Gui *player;

  if(event->button != 1) { return ETK_TRUE; }

  player = data;
  if(!data) return ETK_TRUE;

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
  return ETK_TRUE;
}

/**
 * @brief Changed the mpd output volume level
 */
Etk_Bool
cb_vol_slider_value_changed(Etk_Object *object, double value, void *data)
{
  UNUSED(object)
  UNUSED(data)
  
  mpc_change_vol(value);
  return ETK_TRUE;
}

/**
 * @brief Get the name of the selected artists, search for his albums.
 *        And set the album tree with.
 */
Etk_Bool
cb_tree_artist_selected(Etk_Object *object, Etk_Tree_Row *row, void *data)
{
  UNUSED(object)
  UNUSED(row)
  Emphasis_Player_Gui *player;
  Eina_List *artist_sel;
  char *artist;

  player = data;
  if(!data) return ETK_TRUE;

  artist_sel = etk_tree_selected_rows_get(ETK_TREE(player->media.artist));
  artist     = etk_tree_row_data_get(eina_list_data_get(artist_sel));

  if (artist == NULL)
    {
      emphasis_tree_mlib_set(ETK_TREE(player->media.album),
                             mpc_mlib_album_get(NULL),
                             MPD_DATA_TYPE_TAG, NULL);
      etk_tree_row_select
       (etk_tree_first_row_get(ETK_TREE(player->media.album)));

      return ETK_TRUE;
    }
  emphasis_tree_mlib_set(ETK_TREE(player->media.album), NULL,
                         MPD_DATA_TYPE_TAG, NULL);
  while (artist_sel)
    {
      artist = etk_tree_row_data_get(eina_list_data_get(artist_sel));
      emphasis_tree_mlib_append(ETK_TREE(player->media.album),
                                mpc_mlib_album_get(artist),
                                MPD_DATA_TYPE_TAG, artist);
      artist_sel = eina_list_next(artist_sel);
    }

  etk_tree_row_select(etk_tree_first_row_get(ETK_TREE(player->media.album)));
  return ETK_TRUE;
}

/**
 * @brief Get the selected albums and search for songs wich matches with these albums.
 *        Then set the tree track with that stuff.
 */
Etk_Bool
cb_tree_album_selected(Etk_Object *object, Etk_Tree_Row *row, void *data)
{
  UNUSED(object)
  UNUSED(row)
  Emphasis_Player_Gui *player;
  Eina_List *artist_sel, *album_sel;
  char **album_tag;
  char *artist_tag;

  player = data;
  if(!data) return ETK_TRUE;

  album_sel = etk_tree_selected_rows_get(ETK_TREE(player->media.album));
  album_tag = etk_tree_row_data_get(eina_list_data_get(album_sel));

  if (album_tag == NULL)
    {
      etk_tree_clear(ETK_TREE(player->media.track));
      artist_sel = etk_tree_selected_rows_get(ETK_TREE(player->media.artist));
      while (artist_sel)
        {
          artist_tag = etk_tree_row_data_get(eina_list_data_get(artist_sel));
          emphasis_tree_mlib_append(ETK_TREE(player->media.track),
                                    mpc_mlib_track_get(artist_tag, NULL),
                                    MPD_DATA_TYPE_SONG, NULL);
          artist_sel = eina_list_next(artist_sel);
        }
      return ETK_TRUE;
    }

  etk_tree_clear(ETK_TREE(player->media.track));
  while (album_sel)
    {
      album_tag = etk_tree_row_data_get(eina_list_data_get(album_sel));
      emphasis_tree_mlib_append(ETK_TREE(player->media.track),
                                mpc_mlib_track_get(album_tag[1], album_tag[0]),
                                MPD_DATA_TYPE_SONG, NULL);
      album_sel = eina_list_next(album_sel);
    }
  return ETK_TRUE;
}

/* FIXME wait for tree2 to support dnd */
/**
 * @brief Set a drag widget from a list of artist selected
 * @param data A common drag widget
 */
Etk_Bool
cb_drag_artist(Etk_Object *object, void *data)
{
#if 0
  Etk_Widget *drag_menu, *menu_item;
  Etk_Drag *drag;
  Etk_Tree *tree;
  Eina_List *rowlist, *next;
  char *artist;
  const char **types;
  unsigned int num_types;
  Eina_List *playlist = NULL, *tmplist;

  if(!data) return ETK_TRUE;

  tree = ETK_TREE(object);
  drag = ETK_DRAG((ETK_WIDGET(tree))->drag);
  rowlist = etk_tree_selected_rows_get(tree);

  drag_menu = etk_menu_new();

  while(rowlist)
    {
      artist = etk_tree_row_data_get(eina_list_data_get(rowlist));
      tmplist = mpc_mlib_track_get(artist, NULL);
      
      if(artist)
        {
          menu_item = etk_menu_item_new_with_label(artist);
        }
      else
        {
          menu_item = etk_menu_item_new_with_label(LABEL_MLIB_ALL);
        }

      etk_menu_shell_append(ETK_MENU_SHELL(drag_menu),
                            ETK_MENU_ITEM(menu_item));

      if(!playlist)
        {
          playlist = tmplist;
        }
      else
        {
          playlist = eina_list_merge(playlist, tmplist);
        }
      
      next = eina_list_next(rowlist);
      rowlist = next;
    }
  eina_list_free(rowlist);

  types = calloc(1, sizeof(char));
  if(!types) return ETK_TRUE;
  num_types = 1;
  types[0] = strdup("Emphasis_Playlist");

  etk_drag_types_set(drag, types, num_types);
  etk_drag_data_set(drag, playlist, 1);

  etk_container_add(ETK_CONTAINER(drag), drag_menu);
  ((Emphasis_Player_Gui*)data)->media.drag = ETK_WIDGET(drag);
#endif
  return ETK_TRUE;
}

/**
 * @brief Set a drag widget from a list of albums selected
 * @param data An Emphasis_Gui
 */
Etk_Bool
cb_drag_album(Etk_Object *object, void *data)
{
#if 0
  Etk_Widget *drag_menu, *menu_item;
  Etk_Drag *drag;
  Etk_Tree *tree;
  Eina_List *rowlist;
  char **album;
  const char **types;
  unsigned int num_types;
  Eina_List *playlist = NULL, *tmplist;

  if(!data) return ETK_TRUE;

  tree = ETK_TREE(object);
  drag = ETK_DRAG((ETK_WIDGET(object))->drag);
  rowlist = etk_tree_selected_rows_get(tree);

  drag_menu = etk_menu_new();

  while (rowlist)
    {
      album = etk_tree_row_data_get(eina_list_data_get(rowlist));
      if (album != NULL)
        {
          tmplist = mpc_mlib_track_get(album[1], album[0]);

          if (album[0])
            {
              menu_item = etk_menu_item_new_with_label(album[0]);
            }
          else
            {
              menu_item = etk_menu_item_new_with_label(LABEL_MLIB_ALL);
            }

          etk_menu_shell_append(ETK_MENU_SHELL(drag_menu),
                                ETK_MENU_ITEM(menu_item));
          if (!playlist)
            {
              playlist = tmplist;
            }
          else
            {
              playlist = eina_list_merge(playlist, tmplist);
            }
          rowlist = eina_list_next(rowlist);
        }
      else
        {
          Etk_Tree_Row *row;

          row = etk_tree_first_row_get(tree);
          row = etk_tree_row_next_get(row);
          rowlist = eina_list_free(rowlist);
          rowlist = NULL;
          emphasis_list_free(playlist);
          playlist = NULL;
          while (row)
            {
              rowlist = eina_list_append(rowlist, row);
              row = etk_tree_row_next_get(row);
            }
        }
    }

  types = calloc(1, sizeof(char));
  if(!types) return ETK_TRUE;
  num_types = 1;
  types[0] = strdup("Emphasis_Playlist");

  etk_drag_types_set(drag, types, num_types);
  etk_drag_data_set(drag, playlist, 1);

  etk_container_add(ETK_CONTAINER(drag), drag_menu);

  ((Emphasis_Player_Gui*)data)->media.drag = ETK_WIDGET(drag);
#endif
  return ETK_TRUE;
}

/**
 * @brief Set a drag widget from a list of selected song's row
 * @param data A drag widget
 * @note this callback will set the data's drag and the object's drag
 */
Etk_Bool
cb_drag_track(Etk_Object *object, void *data)
{
#if 0
  Etk_Widget *drag_menu, *menu_item;
  Etk_Drag *drag;
  Etk_Tree *tree;
  Eina_List *rowlist;
  char *title;
  const char **types;
  unsigned int num_types;
  Eina_List *playlist;

  if(!data) return ETK_TRUE;

  tree = ETK_TREE(object);
  drag = ETK_DRAG((ETK_WIDGET(object))->drag);

  drag_menu = etk_menu_new();

  rowlist = etk_tree_selected_rows_get(tree);
  playlist = convert_rowlist_in_playlist_with_file(rowlist);
  while (rowlist)
    {
      etk_tree_row_fields_get(eina_list_data_get(rowlist),
                              etk_tree_nth_col_get(tree, 0), &title, NULL);

      menu_item = etk_menu_item_new_with_label(title);
      etk_menu_shell_append(ETK_MENU_SHELL(drag_menu),
                            ETK_MENU_ITEM(menu_item));

      rowlist = eina_list_next(rowlist);
    }

  types = calloc(1, sizeof(char));
  if(!types) return ETK_TRUE;
  num_types = 1;
  types[0] = strdup("Emphasis_Playlist");

  etk_drag_types_set(drag, types, num_types);
  etk_drag_data_set(drag, playlist, 1);

  etk_tree_row_fields_get(eina_list_data_get(rowlist),
                          etk_tree_nth_col_get(tree, 0), &title, NULL);

  etk_container_add(ETK_CONTAINER(drag), drag_menu);

  ((Emphasis_Player_Gui*)data)->media.drag = ETK_WIDGET(drag);
#endif
  return ETK_TRUE;
}

/**
 * @brief Get the playlist draged. Add it to the current playlist.
 *        And set the playlist tree up to date.
 * @param data A drag widget with a playlist in his data
 */
Etk_Bool
cb_drop_song(Etk_Object *object, void *event, void *data)
{
#if 0
  UNUSED(event)
  Etk_Tree *tree;
  Etk_Drag *drag;
  Eina_List *list;

  if(!data) return ETK_TRUE;

  tree = ETK_TREE(object);

  drag = ETK_DRAG(((Emphasis_Player_Gui*)data)->media.drag);
  if (drag == NULL ||
      drag->types == NULL ||
      drag->types[0] == NULL || 
      strcmp("Emphasis_Playlist", drag->types[0]))
    {
      return ETK_TRUE;
    }
  
  list = drag->data;
  mpc_playlist_add(list);

  free(drag->types[0]);
  free(drag->types);

  /*
     Etk_Widget *wid;
     Eina_List *evaslist;
     evaslist = etk_container_children_get(ETK_CONTAINER(drag));
     while (evaslist)
     {
     etk_container_remove(ETK_CONTAINER(drag), eina_list_data_get(evaslist));
     etk_object_destroy(ETK_OBJECT(eina_list_data_get(evaslist)));
     evaslist = eina_list_next(evaslist);
     }
   */
#endif
  return ETK_TRUE;
}

/**
 * @brief Double-click on a row add the artist's song to the playlist
 */
Etk_Bool
cb_tree_mlib_clicked(Etk_Object *object, Etk_Tree_Row *row,
                     Etk_Event_Mouse_Down *event, void *data)
{
  Emphasis_Player_Gui *player;
  Emphasis_Type et;
  char *str;

  if(!data) return ETK_TRUE;

  player = data;
  et = (Emphasis_Type) etk_object_data_get(object, "Emphasis_Type");
  if ((event->button == 1) && (event->flags == ETK_MOUSE_DOUBLE_CLICK))
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
  return ETK_TRUE;
}

/**
 * @brief Play the song double-clicked
 */
Etk_Bool
cb_tree_pls_clicked(Etk_Object *object, Etk_Tree_Row *row,
                    Etk_Event_Mouse_Down *event, void *data)
{
  UNUSED(data)
  UNUSED(object)
  int id;

  if ((event->button == 1) && (event->flags == ETK_MOUSE_DOUBLE_CLICK))
    {
      id = (int) etk_tree_row_data_get(row);
      mpc_play_id(id);
    }
  return ETK_TRUE;
}

Etk_Bool
cb_emphasis_bindings_key(Etk_Object *object, Etk_Event_Key_Down *event,
                         void *data)
{
  UNUSED(data)
  UNUSED(object)
  if (!strcmp(event->key, "z"))
    {
      cb_toggle_random(NULL, NULL);
      return ETK_TRUE;
    }
  if (!strcmp(event->key, "r"))
    {
      cb_toggle_repeat(NULL, NULL);
      return ETK_TRUE;
    }
  if (!strcmp(event->key, "s"))
    {
      cb_button_stop_clicked(NULL, NULL);
      return ETK_TRUE;
    }
  if (!strcmp(event->key, "p"))
    {
      cb_button_play_clicked(NULL, NULL);
      return ETK_TRUE;
    }
  if (!strcmp(event->key, "greater"))
    {
      cb_button_next_clicked(NULL, NULL);
      return ETK_TRUE;
    }
  if (!strcmp(event->key, "less"))
    {
      cb_button_prev_clicked(NULL, NULL);
      return ETK_TRUE;
    }
  return ETK_TRUE;
}

/**
 * @brief Fast find in mlib tree
 */
Etk_Bool
cb_mlib_bindings_key(Etk_Object *object, Etk_Event_Key_Down *event,
                     void *data)
{
  Etk_Tree *tree;
  Etk_Tree_Row *row, *row_next;
  Etk_Tree_Col *col;
  Emphasis_Player_Gui *player;
  char *str;

  if (!event->string || !data)
    return ETK_TRUE;

  tree = ETK_TREE(object);
  player = data;

  if (!strcmp(event->key, "a")
      && event->modifiers == ETK_MODIFIER_CTRL)
    {
      /* CTRL+a select all rows  */
      etk_tree_select_all(tree);

      if (object == ETK_OBJECT(player->media.track))
        return ETK_TRUE;

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

      return ETK_TRUE;
    }
  if (!strcmp(event->key, "r")
      && event->modifiers == ETK_MODIFIER_CTRL)
    {
      int r;
      /* selected randomly a row */

      /* We need an accessor here */
      r = xrand(0, ETK_TREE(tree)->total_rows);
      row = etk_tree_nth_row_get(tree, r);
      etk_tree_row_select(row);
      etk_tree_row_scroll_to(row, ETK_TRUE);
      return ETK_TRUE;
    }

  if (!strcmp(event->key, "Tab")
      || event->modifiers == ETK_MODIFIER_CTRL)
    return ETK_TRUE;

  col = etk_tree_nth_col_get(tree, 0);

/**/
  /* fast search section */
  const char *cur_title, *base_title;
  char *title, *filter = NULL;
  int length;
  Emphasis_Type type;

  /*get the base title of the tree */
  base_title = etk_object_data_get(ETK_OBJECT(tree), "title");    
  /*get the current title of tree */
  cur_title = etk_tree_col_title_get(col);      
  type = (Emphasis_Type) etk_object_data_get(ETK_OBJECT(tree), "Emphasis_Type");

  if (!strcmp("Return", event->keyname))
    {
      /* add selected songs to pls */
      Emphasis_Type et;

      et = (Emphasis_Type)etk_object_data_get(object, "Emphasis_Type");
      if(!et) { return ETK_TRUE; }
      emphasis_playlist_append_selected(tree, et);
      mpc_play_if_stopped();
      emphasis_tree_mlib_init(player, type);
      if (strcmp(base_title, cur_title))
        { etk_tree_col_title_set(col, base_title); }
      return ETK_TRUE;
    }

  /* FIXME & > < */
  /* Fast Research */
  if (!strcmp(base_title, cur_title))
    {
      /* FIXME, works with kenjins-like? */
      /* Don't print extendted keys (shift, ctrl, etc.) */
      if(!islower(event->keyname[0]))
        return ETK_TRUE;

      asprintf(&title, "%s starting with : %s", base_title, event->string);
    }
  else
    {
      if (!strcmp("BackSpace", event->keyname))
        {
          int utf8_p;
          utf8_p = evas_string_char_prev_get(cur_title, strlen(cur_title),NULL);
          title = strndup(cur_title, utf8_p);
          
          emphasis_tree_mlib_init(player, type);
          if (strlen(title) == strlen(base_title) + 17)
            {
              etk_tree_col_title_set(col, base_title);
              return ETK_TRUE;
            }
        }
      else if (!strcmp("Escape", event->keyname))
        {
          /* quit fast search */
          emphasis_tree_mlib_init(player, type);
          etk_tree_col_title_set(col, base_title);
          return ETK_TRUE;
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
      /* show corresponding rows */
      row_next = etk_tree_row_next_get(row);
      etk_tree_row_fields_get(row, col, &str, NULL);
      if (!strncasecmp("The ", str, 4))
        {
          str = str + 4;
        }
      if (!strcmp(LABEL_MLIB_ALL, str) || (strncasecmp(filter, str, length) != 0))
        etk_tree_row_delete(row);
      row = row_next;
    }

  etk_tree_row_select(etk_tree_first_row_get(tree));
  free(title);
  free(filter);
  return ETK_TRUE;
}

/**
 * @brief If right-clicked: popdown the contextual menu on the playlist
 */
Etk_Bool
cb_pls_contextual_menu(Etk_Object *object, Etk_Event_Mouse_Down *event,
                       void *data)
{
  UNUSED(object);
  Emphasis_Gui *gui;

  if(!data) return ETK_TRUE;

  gui = data;
  if (event->button == 3)
    {
      etk_menu_popup(ETK_MENU(gui->menu));
      etk_widget_show_all(gui->menu);
    }
  return ETK_TRUE;
}

/**
 * @brief Clear the playlist
 */
Etk_Bool
cb_playlist_clear(Etk_Object *object, Etk_Event_Mouse_Down *event,
                  void *data)
{
  UNUSED(object);
  UNUSED(event);
  UNUSED(data);

  mpc_playlist_clear();
  return ETK_TRUE;
}

/**
 * * @brief Shuffle the playlist
 */
Etk_Bool
cb_playlist_shuffle(Etk_Object *object, void *data)
{
  UNUSED_CLICKED_PARAM;
  
  mpc_playlist_shuffle();
  return ETK_TRUE;
}

/**
 * @brief Callback function of bindings key on the playlist
 */
Etk_Bool
cb_pls_bindings_key(Etk_Object *object, Etk_Event_Key_Down *event, 
                    void *data)
{
  UNUSED(object);
  Emphasis_Player_Gui *player;
  Eina_List *rowlist;
  Eina_List *list;

  if(!data) return ETK_TRUE;
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
  else if(!strcmp(event->key, "c"))
    {
      cb_playlist_clear(NULL,NULL,NULL);
    }
  else if(!strcmp(event->key, "s"))
    {
      cb_playlist_shuffle(NULL,NULL);
    }
  else if(!strcmp(event->key, "u"))
    {
      cb_database_update(NULL, data);
    }
  return ETK_TRUE;
}

/**
 * @brief Delete all the selected item from the playlist
 */
Etk_Bool
cb_playlist_delete(Etk_Object *object, void *data)
{
  UNUSED(object);
  Emphasis_Player_Gui *player;
  Eina_List *rowlist;
  Eina_List *list;

  if(!data) return ETK_TRUE;
  player = data;

  rowlist = etk_tree_selected_rows_get(ETK_TREE(player->media.pls));
  list = convert_rowlist_in_playlist_with_id(rowlist);

  mpc_playlist_delete(list);
  return ETK_TRUE;
}

/**
 * @brief Request an database update
 */
Etk_Bool
cb_database_update(Etk_Object *object, void *data)
{
  UNUSED(object);
  Emphasis_Player_Gui *player;

  if(!data) return ETK_TRUE;
  player = data;

  mpc_database_update();
  emphasis_tree_mlib_init(player, EMPHASIS_ARTIST);

  /* Clear old search */
  etk_tree_col_title_set(ETK_TREE_COL_GET(player->media.artist, 0), "Artist");
  etk_tree_col_title_set(ETK_TREE_COL_GET(player->media.album , 0), "Album");
  etk_tree_col_title_set(ETK_TREE_COL_GET(player->media.track , 0), "Track");

  /* TODO redraw */
  return ETK_TRUE;
}

Etk_Bool
cb_small_resize(Etk_Object *object, void *data)
{
  Emphasis_Player_Gui *player;
  int w = 0;

  UNUSED(object);
  if(!data) return ETK_TRUE;
  player = data;

  etk_window_geometry_get(ETK_WINDOW(player->small.window),
                          NULL, NULL,
                          &w  , NULL);
  player->small.cover_size_h = w;
  etk_widget_size_request_set(player->small.cover,
                              player->small.cover_size_w,
                              player->small.cover_size_h);
  return ETK_TRUE;
}

Etk_Bool
cb_switch_full(Etk_Object *object, void *data)
{
  UNUSED(object);

  Emphasis_Player_Gui *player;
  if(!data) return ETK_TRUE;
  player = data;

  if(player->state == EMPHASIS_FULL)
    {
      return ETK_TRUE;
    }
  player->state = EMPHASIS_FULL;

  etk_widget_hide(player->small.window);
  etk_widget_hide(player->media.window);
  etk_widget_hide(player->small.cov.window);
  etk_widget_hide(player->small.ctr.window);
  
  etk_container_remove(player->media.root);
  etk_box_append(ETK_BOX(player->full.root),
                     player->media.root,
                     ETK_BOX_START,
                     ETK_BOX_EXPAND_FILL, 0);
 
  emphasis_player_toggle_full(player, ETK_TRUE);

  etk_widget_show(player->full.window);
  return ETK_TRUE;
}

Etk_Bool
cb_switch_small(Etk_Object *object, void *data)
{
  UNUSED(object);

  Emphasis_Player_Gui *player;
  if(!data) return ETK_TRUE;
  player = data;

  if(player->state == EMPHASIS_SMALL)
    {
      return ETK_TRUE;
    }
  player->state = EMPHASIS_SMALL;

  etk_widget_hide(player->full.window);

  etk_container_add(ETK_CONTAINER(player->media.window), player->media.root);
  emphasis_player_toggle_full(player, ETK_FALSE);

  if(etk_toggle_button_active_get(ETK_TOGGLE_BUTTON(player->small.media)))
    {
      etk_widget_show(player->media.window);
    }
  if(player->small.packed)
    { etk_widget_show(player->small.window); }
  else
    {
      etk_widget_show(player->small.cov.window);
      etk_widget_show(player->small.ctr.window);
    }
  return ETK_TRUE;
}

Etk_Bool
cb_media_button_lib_clicked(Etk_Object *object, 
                            void *data)
{
  UNUSED(object);

  Emphasis_Player_Gui *player;
  player = data;
  if(!data) return ETK_TRUE;

  etk_notebook_current_page_set(ETK_NOTEBOOK(player->media.notebook), 0);
  return ETK_TRUE;
}

Etk_Bool
cb_media_button_search_clicked(Etk_Object *object, 
                               void *data)
{
  UNUSED(object);

  Emphasis_Player_Gui *player;
  player = data;
  if(!data) return ETK_TRUE;

  etk_notebook_current_page_set(ETK_NOTEBOOK(player->media.notebook), 1);
  return ETK_TRUE;
}

Etk_Bool
cb_media_button_playlists_clicked(Etk_Object *object, 
                                  void *data)
{
  UNUSED(object);

  Emphasis_Player_Gui *player;
  player = data;
  if(!data) return ETK_TRUE;

  etk_notebook_current_page_set(ETK_NOTEBOOK(player->media.notebook), 2);
  emphasis_pls_list_init(player);
  return ETK_TRUE;
}

Etk_Bool
cb_media_button_stats_clicked(Etk_Object *object, 
                              void *data)
{
  UNUSED(object);

  Emphasis_Player_Gui *player;
  player = data;
  if(!data) return ETK_TRUE;

  etk_notebook_current_page_set(ETK_NOTEBOOK(player->media.notebook), 3);
  return ETK_TRUE;
}

Etk_Bool
cb_config_show(Etk_Object *object, void *data)
{
  Emphasis_Gui *gui;
  Emphasis_Config_Gui *configgui;
  UNUSED(object);

  if(!data) return ETK_TRUE;
  gui = data;

  if(gui->config_gui == NULL)
    {
      gui->config_gui = malloc(sizeof(Emphasis_Config_Gui));
      if(!gui->config_gui) return ETK_TRUE;
      emphasis_init_configgui(gui->config_gui);
      gui->config_gui->data = data;
    }
  configgui = gui->config_gui;

  emphasis_configgui_autoset(configgui);
  etk_widget_show_all(configgui->window);
  return ETK_TRUE;
}

Etk_Bool
cb_config_hide(Etk_Object *object, void *data)
{
  Emphasis_Config_Gui *configgui;
  UNUSED(object);

  if(!data) return ETK_TRUE;
  configgui = data;

  etk_widget_hide_all(configgui->window);
  return ETK_FALSE;
}

Etk_Bool
cb_config_ok(Etk_Object *object, void *data)
{
  Emphasis_Config_Gui *configgui;

  if(!data) return ETK_TRUE;
  configgui = data;

  emphasis_configgui_save(configgui);

  cb_config_hide(object, data);
  return ETK_TRUE;
}

Etk_Bool
cb_media_pls_list_row_clicked(Etk_Object *object, Etk_Tree_Row *row, 
                              Etk_Event_Mouse_Down *event, void *data)
{
  Emphasis_Player_Gui *player;
  Etk_Tree *tree;
  Etk_Tree_Col *col;
  char *playlist_name;

  if(!data) return ETK_TRUE;
  player = data;
  if (event->button == 1)
    {
      if (event->flags == ETK_MOUSE_NONE)
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
      else if (event->flags == ETK_MOUSE_DOUBLE_CLICK)
        {
          cb_media_pls_load_clicked(NULL, data);
        }
    }
  return ETK_TRUE;
}

Etk_Bool
cb_media_pls_save_clicked(Etk_Object *object, void *data)
{
  UNUSED(object);
#if defined(LIBMPD_0_12_4)
  Emphasis_Player_Gui *player;
  Etk_Widget *entry;
 
  if(!data) return ETK_TRUE; 
  player = data;
  entry = player->media.pls_entry_save;
  if(!entry) return ETK_TRUE;
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
  return ETK_TRUE;
}

Etk_Bool
cb_media_pls_load_clicked(Etk_Object *object, void *data)
{
  UNUSED(object);
#if defined(LIBMPD_0_12_4)
  Emphasis_Player_Gui *player;
  Etk_Tree_Col *col;
  Etk_Tree_Row *row;
  char *playlist_name;

  if(!data) return ETK_TRUE;
  player = data;
  col = etk_tree_nth_col_get(ETK_TREE(player->media.pls_list), 0);
  row = etk_tree_selected_row_get(ETK_TREE(player->media.pls_list));
  if(!row) return ETK_TRUE;
  etk_tree_row_fields_get(row, ETK_FALSE, col, &playlist_name, NULL);

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
  return ETK_TRUE;
}

Etk_Bool
cb_media_pls_del_clicked(Etk_Object *object, void *data)
{
  UNUSED(object);
#if defined(LIBMPD_0_12_4)
  Emphasis_Player_Gui *player;
  Etk_Tree_Col *col;
  Etk_Tree_Row *row;
  char *playlist_name;

  if(!data) return ETK_TRUE;
  player = data;
  col = etk_tree_nth_col_get(ETK_TREE(player->media.pls_list), 0);
  row = etk_tree_selected_row_get(ETK_TREE(player->media.pls_list));
  etk_tree_row_fields_get(row, col, &playlist_name, NULL);

  mpc_delete_playlist(playlist_name);
  emphasis_pls_list_init(player);
#else
  UNUSED(data);
#endif
  return ETK_TRUE;
}

Etk_Bool
cb_media_pls_save_key_down(Etk_Object *object, Etk_Event_Key_Down *event,
                           void *data)
{
  if (!strcmp(event->key, "Return"))
    {
      etk_widget_unfocus(ETK_WIDGET(object));
      cb_media_pls_save_clicked(NULL, data);
    }
  return ETK_TRUE;
}

Etk_Bool
cb_media_search_btn_add_clicked(Etk_Object *object, void *data)
{
  Emphasis_Player_Gui *player;
  UNUSED(object);
  Eina_List *row;
  Eina_List *list;
  Etk_Tree_Col *col;
  const char *file;

  if(!data) return ETK_TRUE;
  player = data;

  row = etk_tree_selected_rows_get(ETK_TREE(player->media.search_tree));
  if (!row)
    row = etk_tree_unselected_rows_get(ETK_TREE(player->media.search_tree));
  list = row;

  col = etk_tree_nth_col_get(ETK_TREE(player->media.search_tree), 3);
  while (row)
    {
      etk_tree_row_fields_get(eina_list_data_get(row), col, &file, NULL);
      mpc_playlist_add_song(file, 0);
      row = eina_list_next(row);
    }
  eina_list_free(list);
  mpc_playlist_commit();
  return ETK_TRUE;
}

Etk_Bool
cb_media_search_btn_replace_clicked(Etk_Object *object, void *data)
{
  UNUSED(object);
  Emphasis_Player_Gui *player;

  if(!data) return ETK_TRUE;
  player = data;
  cb_playlist_clear(NULL,NULL,NULL);
  cb_media_search_btn_add_clicked(object, data);
  return ETK_TRUE;
}

Etk_Bool
cb_media_search_btn_add_search_clicked(Etk_Object *object, void *data)
{
  UNUSED(object);
  Emphasis_Player_Gui *player;

  if(!data) return ETK_TRUE;
  player = data;
  emphasis_search_row_add(player);
  return ETK_TRUE;
}

Etk_Bool
cb_media_search_btn_remove_search_clicked(Etk_Object *object, void *data)
{
  Etk_Widget *search_root;
  Etk_Widget *search_query_row;
  Etk_Widget *button;

  if(!data) return ETK_TRUE;
  search_root = data;
  button = ETK_WIDGET(object);
  search_query_row  = etk_widget_parent_get(button);

  etk_widget_unfocus(button);

  etk_container_remove(search_query_row);

  /* TODO : check for an etk function to replace this */
  Eina_List *children;
  Eina_List *child;
  children = etk_container_children_get(ETK_CONTAINER(search_query_row));
  child = children;
  while (child)
    {
      etk_object_destroy(ETK_OBJECT(eina_list_data_get(child)));
      child = eina_list_next(child);
    }
  eina_list_free(children);
  etk_object_destroy(ETK_OBJECT(search_query_row));
  return ETK_TRUE;
}

Etk_Bool
cb_media_search_entry_text_changed(Etk_Object *object,
                                   Etk_Event_Key_Down *event,
                                   void *data)
{
  if (data && event && event->keyname && !strcmp(event->keyname, "Return"))
    {
      cb_media_search_btn_search_clicked(object, data);
    }
  return ETK_TRUE;
}

Etk_Bool
cb_media_search_btn_search_clicked(Etk_Object *object, void *data)
{
  /* FIXME read this */
  UNUSED(object);
  Emphasis_Player_Gui *player;
  Eina_List *children;
  Eina_List *child;
  Eina_List *sub_child;
  Eina_List *list;
  Eina_List *query = NULL;
  Etk_Widget *widget;
  int index;

  if(!data) return ETK_TRUE;
  player   = data;
  children = etk_container_children_get(ETK_CONTAINER(player->media.search_root));

  child = children;
  while (child)
    {
      widget = eina_list_data_get(child);

      if (ETK_IS_BOX(widget))
        {
          /* Get the search row content */
          sub_child = etk_container_children_get(ETK_CONTAINER(eina_list_data_get(child)));
          list = sub_child;

          /* Get the id of the tag to search */
          index = etk_combobox_active_item_num_get(ETK_COMBOBOX(eina_list_data_get(sub_child)));
          query = eina_list_append(query, (void *)index);

          /* Get the search pattern */
          sub_child = eina_list_next(sub_child);
          query = eina_list_append(query, etk_entry_text_get(ETK_ENTRY(eina_list_data_get(sub_child))));

          eina_list_free(list);
        }

      if (ETK_IS_TREE(widget)) { break; }

      child = eina_list_next(child);
    } 
  eina_list_free(children);

  /* XXX */
  Eina_List *results;
  results = mpc_find(query, 0);

  emphasis_search_tree_fill(player, results);
  /* ^ freed results */

  eina_list_free(query);
  return ETK_TRUE;
}

Etk_Bool
cb_small_pack(Etk_Object *object, Etk_Event_Mouse_Down *event, void *data)
{
  Emphasis_Player_Gui *player;
  UNUSED(object);

  if(!data) { return ETK_TRUE; }

  player = data;

  if(event->button == 1) /* left-click, */
    {
      /* show/hide concurrent window */
      Etk_Widget *win = NULL;
      
      if(player->small.packed == ETK_TRUE) { return ETK_TRUE; }

      if(object == (Etk_Object*)player->small.cover)
        {
          int w, h;
          etk_widget_inner_geometry_get
           (player->small.cov.window, NULL, NULL, &w, &h);

          if( event->widget.y >= h - 30)
            {
              /* special sensible cover zone (bottom) */
              if(event->widget.x <= w * 0.33)
                {
                  /* left zone */
                  mpc_prev();
                }
              else if(event->widget.x >= ( w - w * 0.33 ))
                {
                  /* middle zone */
                  mpc_next();
                }
              else
                {
                  /* right zone */
                  mpc_toggle_play_pause();
                }
              return ETK_TRUE; /* no show/hide */
            }
          else if( event->widget.y <= 7 )
            {
              /* special sensible cover zone (top) */
              double seek;
              seek = (double) event->widget.x / (double) w;
              mpc_seek(seek);
              return ETK_TRUE; /* no show/hide */
            }
          win = player->small.ctr.window;
        }
      else
      if(object == (Etk_Object*)player->small.info)
        {
          win = player->small.cov.window;
        }
      else
        { return ETK_TRUE; }


      if(etk_widget_is_visible(win) == ETK_TRUE)
        { etk_widget_hide(win); }
      else
        { etk_widget_show(win); }
    }
  else if(event->button == 2) /* middle-click */
    {
      /* show/hide mlib window */
      Etk_Widget *toggle = player->small.media;

      cb_toggle_media(ETK_OBJECT(toggle), player);
      etk_toggle_button_active_set(ETK_TOGGLE_BUTTON(toggle),
                                   !etk_toggle_button_active_get
                                   (ETK_TOGGLE_BUTTON(toggle)));
    }
  else if(event->button == 3) /* right-click */
    {
      /* switch small<->tiny */
      if(player->small.packed == ETK_TRUE)
        {
          /* unpacking */
          etk_widget_hide(player->small.window);

          etk_container_add(ETK_CONTAINER(player->small.cov.window),
                            player->small.cov.root);
          etk_container_add(ETK_CONTAINER(player->small.ctr.window),
                            player->small.ctr.root);

          etk_widget_size_request_set(player->small.cover,
                                      100, 100);
                                      //player->small.cover_size_w,
                                      //player->small.cover_size_w);

          etk_widget_show(player->small.cov.window);
          etk_widget_show(player->small.ctr.window);
        }
      else
        {
          /* packing */
          etk_widget_hide(player->small.cov.window);
          etk_widget_hide(player->small.ctr.window);

          etk_container_remove(player->small.cov.root);
          etk_container_remove(player->small.ctr.root);

          etk_box_append(ETK_BOX(player->small.root), player->small.cov.root,
                         ETK_BOX_START, ETK_BOX_FILL, 0);
          etk_box_append(ETK_BOX(player->small.root), player->small.ctr.root,
                         ETK_BOX_START, ETK_BOX_EXPAND_FILL, 0);

          etk_widget_size_request_set(player->small.cover,
                                      player->small.cover_size_w,
                                      player->small.cover_size_h);
          
          etk_widget_show(player->small.window);
        }

      etk_textblock_cursor_visible_set
       (etk_text_view_textblock_get(ETK_TEXT_VIEW(player->small.info)),
        ETK_FALSE);
      player->small.packed = !player->small.packed;
    }
  return ETK_TRUE;
}


Etk_Bool
cb_tiny_cover_wheel(Etk_Object *object, Etk_Event_Mouse_Wheel *event, void *data)
{
  Emphasis_Player_Gui *player;
  Etk_Event_Mouse_Down event_md;
  UNUSED(object);

  if(!data) { return ETK_TRUE; }
  player = data;

  if(player->small.packed) { return ETK_TRUE; }

  event_md.button = 1;
  if(event->z < 0)
    {
      cb_vol_image_clicked(ETK_OBJECT(player->full.sound_high),
                           &event_md, player);
    }
  else
    {
      cb_vol_image_clicked(ETK_OBJECT(player->full.sound_low),
                           &event_md, player);
    }
  return ETK_TRUE;
}
