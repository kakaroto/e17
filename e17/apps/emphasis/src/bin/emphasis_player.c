#include "emphasis.h"
#include "emphasis_player.h"

/* XXX XXX XXX XXX XXX XXX XXX XXX XXX XXX XXX XXX XXX XXX XXX XXX XXX XXX */
#ifndef EMPHASIS_LOCAL_PATH

# if   defined(__lok__)
#   define EMPHASIS_LOCAL_PATH "/home/lok/projects/"
# elif defined(__any__)
#   define EMPHASIS_LOCAL_PATH "./"
# elif defined(__aji__)
#   define EMPHASIS_LOCAL_PATH "/home/aji/src/"
# else
#   define EMPHASIS_LOCAL_PATH "./"
# endif

#endif

#undef ICON_TEST
#define ICON_TEST "emphasis/src/pict/haricot_eap-test3.png"
/* XXX XXX XXX XXX XXX XXX XXX XXX XXX XXX XXX XXX XXX XXX XXX XXX XXX XXX */


#undef  EN_VAR_GET
#define EN_VAR_GET(en,root,id) \
 (root)->small.id=ENHANCE_VAR_GET(en,"small_"#id);\
 (root)->full.id=ENHANCE_VAR_GET(en,"full_"#id)

#undef ENHANCE_VAR_GET
#define ENHANCE_VAR_GET(en,id) \
 enhance_var_get(en,id)

static void _emphasis_enhance_callbacks(Emphasis_Player_Gui *player);

/* TODO : documentation */
void
emphasis_init_player(Emphasis_Player_Gui *player)
{
  Enhance *en;
  
  en = enhance_new();
  
  player->en = en;
  if(!en)
    {
      fprintf(stderr, "Enhance new failed");
      exit(1);
    }

  player->state = EMPHASIS_SMALL; /* TODO : state toggle full */

  _emphasis_enhance_callbacks(player);
  enhance_file_load(en, "Emphasis", PACKAGE_DATA_DIR EMPHASIS_GLADE);

  /* TODO : check widget? */

  EN_VAR_GET(en, player, window);
  EN_VAR_GET(en, player, root);
  
  EN_VAR_GET(en, player, cover);
  
  EN_VAR_GET(en, player, stop);
  EN_VAR_GET(en, player, prev);
  EN_VAR_GET(en, player, play);
  EN_VAR_GET(en, player, next);

  EN_VAR_GET(en, player, info);

  EN_VAR_GET(en, player, progress);
  EN_VAR_GET(en, player, time);

  EN_VAR_GET(en, player, sound_slider);
  EN_VAR_GET(en, player, sound_low);
  EN_VAR_GET(en, player, sound_high);

  EN_VAR_GET(en, player, random);
  EN_VAR_GET(en, player, repeat);
  EN_VAR_GET(en, player, full);
  player->small.media  = enhance_var_get(en, "small_media");


  player->media.window   = enhance_var_get(en, "media_window");
  player->media.root     = enhance_var_get(en, "media_root");

  player->media.notebook = enhance_var_get(en, "media_notebook");

  player->media.pls      = enhance_var_get(en, "media_pls");

  player->media.artist   = enhance_var_get(en, "media_artist");
  player->media.album    = enhance_var_get(en, "media_album");
  player->media.track    = enhance_var_get(en, "media_track");

  player->media.pls_list    = enhance_var_get(en, "media_pls_list");
  player->media.pls_content = enhance_var_get(en, "media_pls_content");

  player->media.pls_entry_save = enhance_var_get(en, "media_pls_entry_save");

  /* Mediabox buttons init */
  player->media.toolbar          = enhance_var_get(en, "media_toolbar");
  player->media.button_lib       = enhance_var_get(en, "media_button_lib");
  player->media.button_search    = enhance_var_get(en, "media_button_search");
  player->media.button_playlists = enhance_var_get(en, "media_button_pls");
  player->media.button_stats     = enhance_var_get(en, "media_button_stats");

  /* TODO : finish the special theme  
  etk_widget_theme_file_set(ETK_WIDGET(player->media.button_lib),
                            PACKAGE_DATA_DIR "/widgets/media_tab_button.edj");
  etk_widget_theme_file_set(ETK_WIDGET(player->media.button_search),
                            PACKAGE_DATA_DIR "/widgets/media_tab_button.edj");
  etk_widget_theme_file_set(ETK_WIDGET(player->media.button_playlists),
                            PACKAGE_DATA_DIR "/widgets/media_tab_button.edj");
  etk_widget_theme_file_set(ETK_WIDGET(player->media.button_stats),
                            PACKAGE_DATA_DIR "/widgets/media_tab_button.edj");
  */
  /* enhance completion : col def */
  etk_tree_multiple_select_set(ETK_TREE(player->media.artist), ETK_TRUE);
  etk_tree_multiple_select_set(ETK_TREE(player->media.album) , ETK_TRUE);
  etk_tree_multiple_select_set(ETK_TREE(player->media.track) , ETK_TRUE);
  etk_tree_multiple_select_set(ETK_TREE(player->media.pls)   , ETK_TRUE);

  etk_tree_col_new(ETK_TREE(player->media.artist),
                   "Artist",
                   etk_tree_model_text_new(ETK_TREE(player->media.artist)),
                   60);
  etk_tree_col_new(ETK_TREE(player->media.album),
                   "Album",
                   etk_tree_model_text_new(ETK_TREE(player->media.album)),
                   60);
  etk_tree_col_new(ETK_TREE(player->media.track),
                   "Track",
                   etk_tree_model_text_new(ETK_TREE(player->media.track)),
                   60);
  /* pls */
  etk_tree_col_new(ETK_TREE(player->media.pls),
                   "Title",
                   etk_tree_model_icon_text_new(ETK_TREE(player->media.pls),
                                                ETK_TREE_FROM_FILE),
                   140);
  etk_tree_col_new(ETK_TREE(player->media.pls),
                   "Time",
                   etk_tree_model_text_new(ETK_TREE(player->media.pls)),
                   30);
  etk_tree_col_new(ETK_TREE(player->media.pls),
                   "Artist",
                   etk_tree_model_text_new(ETK_TREE(player->media.pls)),
                   120);
  etk_tree_col_new(ETK_TREE(player->media.pls),
                   "Album",
                   etk_tree_model_text_new(ETK_TREE(player->media.pls)),
                   120);
  /* playlists page */
  etk_tree_col_new(ETK_TREE(player->media.pls_list),
                   "Playlists",
                   etk_tree_model_text_new(ETK_TREE(player->media.pls_list)),
                   120);
  etk_tree_col_new(ETK_TREE(player->media.pls_content),
                   "Preview",
                   etk_tree_model_text_new(ETK_TREE(player->media.pls_content)),
                   120);
  /**/
 
  etk_object_data_set(ETK_OBJECT(player->media.artist), "title", "Artist");
  etk_object_data_set(ETK_OBJECT(player->media.album) , "title", "Album" );
  etk_object_data_set(ETK_OBJECT(player->media.track) , "title", "Track" );

  etk_object_data_set(ETK_OBJECT(player->media.artist),
                      "Emphasis_Type",
                      (void*)EMPHASIS_ARTIST);
  etk_object_data_set(ETK_OBJECT(player->media.album),
                      "Emphasis_Type",
                      (void*)EMPHASIS_ALBUM);
  etk_object_data_set(ETK_OBJECT(player->media.track),
                      "Emphasis_Type",
                      (void*)EMPHASIS_TRACK);

  /* enhance completion : dnd */
  etk_widget_dnd_source_set(ETK_WIDGET(player->media.artist), ETK_TRUE);
  etk_widget_dnd_source_set(ETK_WIDGET(player->media.album) , ETK_TRUE);
  etk_widget_dnd_source_set(ETK_WIDGET(player->media.track) , ETK_TRUE);
  etk_widget_dnd_dest_set(ETK_WIDGET(player->media.pls), ETK_TRUE);

  etk_signal_connect("row_clicked", ETK_OBJECT(player->media.artist),
                     ETK_CALLBACK(cb_tree_mlib_clicked),
                     player);
  etk_signal_connect("row_clicked", ETK_OBJECT(player->media.album ),
                     ETK_CALLBACK(cb_tree_mlib_clicked),
                     player);
  etk_signal_connect("row_clicked", ETK_OBJECT(player->media.track ),
                     ETK_CALLBACK(cb_tree_mlib_clicked),
                     player);
  etk_signal_connect("row_clicked", ETK_OBJECT(player->media.pls   ),
                     ETK_CALLBACK(cb_tree_pls_clicked),
                     NULL);

  /* Media pls init */
  etk_object_data_set(ETK_OBJECT(player->media.pls_content),
                      "Emphasis_Type",
                      (void*)EMPHASIS_TRACK);

  /* enhance complection : images */
  /* TODO : use cover_haricotmagique() */
  emphasis_player_cover_set(player, PACKAGE_DATA_DIR EMPHASIS_LOGO);

  emphasis_player_vol_image_set(player, 0, PACKAGE_DATA_DIR EMPHASIS_SOUNDL);
  emphasis_player_vol_image_set(player, 1, PACKAGE_DATA_DIR EMPHASIS_SOUNDR);

  player->small.cover_size_w = player->small.cover->requested_size.w;
  player->small.cover_size_h = player->small.cover->requested_size.h;
  player->full.cover_size_w  = player->full.cover->requested_size.w;
  player->full.cover_size_h  = player->full.cover->requested_size.h;

  /* TEMP configuration, need to add this to enhance */
  etk_window_wmclass_set(ETK_WINDOW(player->full.window),
                         "emphasis", "Emphasis");
  etk_window_wmclass_set(ETK_WINDOW(player->small.window),
                         "emphasis", "Emphasis");
  etk_window_wmclass_set(ETK_WINDOW(player->media.window),
                         "emphasis", "Emphasis");
  etk_container_border_width_set(ETK_CONTAINER(player->full.window), 5);
}

static void
_emphasis_enhance_callbacks(Emphasis_Player_Gui *player)
{
  Enhance *en;

  en = player->en;

  /* destroyed */
  enhance_callback_data_set(en, "cb_quit"      , player);
  enhance_callback_data_set(en, "cb_media_quit", player);

  /* clicked */
  enhance_callback_data_set(en, "cb_button_stop_clicked", NULL);
  enhance_callback_data_set(en, "cb_button_prev_clicked", NULL);
  enhance_callback_data_set(en, "cb_button_play_clicked", NULL);
  enhance_callback_data_set(en, "cb_button_next_clicked", NULL);

  /* value_changed */
  enhance_callback_data_set(en, "cb_vol_slider_value_changed", NULL);
  /* mouse_down */
  enhance_callback_data_set(en, "cb_vol_image_clicked", player);

  /* clicked */
  enhance_callback_data_set(en, "cb_toggle_random", NULL);
  enhance_callback_data_set(en, "cb_toggle_repeat", NULL);
  enhance_callback_data_set(en, "cb_toggle_full"  , player);
  enhance_callback_data_set(en, "cb_toggle_media" , player);

  /* mouse_down */
  enhance_callback_data_set(en, "cb_seek_time", NULL);

  /* drag_begin */
  enhance_callback_data_set(en, "cb_drag_artist", player);
  enhance_callback_data_set(en, "cb_drag_album" , player);
  enhance_callback_data_set(en, "cb_drag_track" , player);
  /* drag_drop */
  enhance_callback_data_set(en, "cb_drop_song"  , player);

  /* key_down */
  enhance_callback_data_set(en, "cb_mlib_bindings_key", player);
  enhance_callback_data_set(en, "cb_pls_bindings_key" , player);

  /* row_selected */
  enhance_callback_data_set(en, "cb_tree_artist_selected", player);
  enhance_callback_data_set(en, "cb_tree_album_selected" , player);

  /* media buttonbox */
  enhance_callback_data_set(en, "cb_media_button_lib_clicked"      , player);
  enhance_callback_data_set(en, "cb_media_button_search_clicked"   , player);
  enhance_callback_data_set(en, "cb_media_button_playlists_clicked", player);
  enhance_callback_data_set(en, "cb_media_button_stats_clicked"    , player);

  enhance_callback_data_set(en, "cb_media_pls_list_row_clicked"    , player);
  enhance_callback_data_set(en, "cb_media_pls_save_clicked"        , player);
  enhance_callback_data_set(en, "cb_media_pls_load_clicked"        , player);
  enhance_callback_data_set(en, "cb_media_pls_del_clicked"         , player);

  enhance_callback_data_set(en, "cb_media_pls_list_row_clicked"    , player);
  enhance_callback_data_set(en, "cb_tree_mlib_clicked"             , player);
  enhance_callback_data_set(en, "cb_media_pls_save_key_down"       , player);
}

/* TODO : documentation */
void
emphasis_player_cover_set(Emphasis_Player_Gui *player, const char *path)
{
  etk_image_set_from_file(ETK_IMAGE(player->full.cover) , path);
  etk_image_set_from_file(ETK_IMAGE(player->small.cover), path);
}

/* TODO : documentation */
void
emphasis_player_cover_size_set(Emphasis_Player_Gui *player, int w, int h)
{
  etk_widget_size_request_set(player->small.cover, w, h);
  etk_widget_size_request_set(player->full.cover , w, h);
}

/* TODO : documentation */
void
emphasis_player_cover_size_update(Emphasis_Player_Gui *player)
{
  etk_widget_size_request_set(player->small.cover,
                              player->small.cover_size_w,
                              player->small.cover_size_h);

  etk_widget_size_request_set(player->full.cover,
                              player->full.cover_size_w,
                              player->full.cover_size_h);
}

/* TODO : update doc */
/**
 * @brief Set the message in th textblock
 * @param player A Emphasis_Player_Gui
 * @param song The current playing song
 * @param msg Additional, if song != NULL or the message displayed
 */
void
emphasis_player_info_set(Emphasis_Player_Gui *player,
                         mpd_Song * song, char *msg)
{
  char *info_label;
  char *info_textblock;

  if (song)
    {
      char **table[] = {&(song->artist), &(song->title), &(song->album), NULL};
      emphasis_unknow_if_null(table);

      /* TEMP */
      if (!msg)
        {
          asprintf(&info_label,
                   "<left_margin=10>"
                   "<b><font_size=16>%s</font_size></b>"
                   "<br><br>"
                   "<font_size=11><i>by</i></font_size>  "
                   "<font_size=13>%s</font_size>  "
                   "<font_size=11><i>in</i></font_size>  "
                   "<font_size=13>%s</font_size>"
                   "</left_margin>",
                   song->title, song->artist, song->album);
           asprintf(&info_textblock,
                    "<b><font size=16>%s</font size></b>"
                    "\n\n"
                    "<font size=11><i>by</i></font size>  "
                    "<font size=13>%s</font size>\n"
                    "<font size=11><i>in</i></font size>  "
                    "<font size=13>%s</font size>",
                    song->title, song->artist, song->album);
        }
      else
        {
          asprintf(&info_label,
                   "<left_margin=10>"
                   "<b><font_size=16>%s</font_size></b>"
                   "<br><br>"
                   "<font_size=11><i>by</i></font_size>  "
                   "<font_size=13>%s</font_size>  "
                   "<font_size=11><i>in</i></font_size>  "
                   "<font_size=13>%s</font_size>"
                   "<font_size=11>   (%s)</font_size>"
                   "</left_margin>",
                   song->title, song->artist, song->album, msg);
          asprintf(&info_textblock,
                   "<b><font size=16>%s</font size></b>"
                   "\n\n"
                   "<font size=11><i>by</i></font size>  "
                   "<font size=13>%s</font size>\n"
                   "<font size=11><i>in</i></font size>  "
                   "<font size=13>%s</font size>"
                   "<font size=11>(%s)</font size>",
                   song->title, song->artist, song->album, msg);
        }
      
      etk_label_set(ETK_LABEL(player->full.info), info_label);
      etk_textblock_text_set(ETK_TEXT_VIEW(player->small.info)->textblock,
                             info_textblock,
                             ETK_TRUE);
      free(info_label);
      free(info_textblock);
    }
  else
    {
      if (msg)
        {
          asprintf(&info_label, "%s", msg);
          asprintf(&info_textblock, "%s", msg);

          etk_label_set(ETK_LABEL(player->full.info), info_label);
          etk_textblock_text_set(ETK_TEXT_VIEW(player->small.info)->textblock,
                                 info_textblock, ETK_TRUE);

          free(info_label);
          free(info_textblock);
          
        }
    }
  
  etk_textblock_object_cursor_visible_set
   (ETK_TEXT_VIEW(player->small.info)->textblock->evas_objects->data,
    ETK_FALSE);
}

/* TODO : update doc */
/**
 * @brief Set the progress bar to elapsed_time/total_time
 * @param elapsed_time The time elapsed
 * @param total_time The lenght of the song
 * @param gui A Emphasis_Gui
 */
void
emphasis_player_progress_set(Emphasis_Player_Gui *player,
                             float elapsed_time, int total_time)
{
  char *time;
  
  etk_progress_bar_fraction_set(ETK_PROGRESS_BAR(player->full.progress),
                                (elapsed_time / total_time));
  etk_progress_bar_fraction_set(ETK_PROGRESS_BAR(player->small.progress),
                                (elapsed_time / total_time));

  asprintf(&time, "%d:%02d / %d:%02d", (int) elapsed_time / 60,
                                       (int) elapsed_time % 60, 
                                       total_time / 60, 
                                       total_time % 60);

  etk_label_set(ETK_LABEL(player->full.time) , time);
  etk_label_set(ETK_LABEL(player->small.time), time);

  free(time);
}

/**
 * @brief Set the image of btn_play to play if mpd is playing or to pause if mpd is stopped. 
 * @param player A Emphasis_Player_Gui
 */
void
emphasis_player_toggle_play(Emphasis_Player_Gui *player)
{
  /* TODO : respect enhance file */
  if (mpc_assert_status(MPD_PLAYER_PLAY))
    {
      etk_button_set_from_stock(ETK_BUTTON(player->full.play),
                                ETK_STOCK_MEDIA_PLAYBACK_PAUSE);
      etk_button_label_set(ETK_BUTTON(player->full.play), NULL);

      etk_button_set_from_stock(ETK_BUTTON(player->small.play),
                                ETK_STOCK_MEDIA_PLAYBACK_PAUSE);
      etk_button_label_set(ETK_BUTTON(player->small.play), NULL);
    }
  else
    {
      if (mpc_assert_status(MPD_PLAYER_PAUSE)
          || mpc_assert_status(MPD_PLAYER_STOP))
        {
          etk_button_set_from_stock(ETK_BUTTON(player->full.play),
                                    ETK_STOCK_MEDIA_PLAYBACK_START);
          etk_button_label_set(ETK_BUTTON(player->full.play), NULL);

          etk_button_set_from_stock(ETK_BUTTON(player->small.play),
                                    ETK_STOCK_MEDIA_PLAYBACK_START);
          etk_button_label_set(ETK_BUTTON(player->small.play), NULL);
        }
    }
}

/* TODO : update doc */
/**
 * @brief Set the random checkbox state to value (1 is ticked)
 * @param value A boolean
 * @param gui A Emphasis_Gui
 */
void
emphasis_player_toggle_random(Emphasis_Player_Gui *player, int value)
{
  etk_toggle_button_active_set(ETK_TOGGLE_BUTTON(player->full.random) , value);
  etk_toggle_button_active_set(ETK_TOGGLE_BUTTON(player->small.random), value);
}

/* TODO : update doc */
/**
 * @brief Set the repeat checkbox state to value (1 is ticked)
 * @param value A boolean
 * @param gui A Emphasis_Gui
 */
void
emphasis_player_toggle_repeat(Emphasis_Player_Gui *player, int value)
{
  etk_toggle_button_active_set(ETK_TOGGLE_BUTTON(player->full.repeat) , value);
  etk_toggle_button_active_set(ETK_TOGGLE_BUTTON(player->small.repeat), value);
}

/* TODO : documentation */
void
emphasis_player_toggle_full(Emphasis_Player_Gui *player, int value)
{
  etk_toggle_button_active_set(ETK_TOGGLE_BUTTON(player->full.full) , value);
  etk_toggle_button_active_set(ETK_TOGGLE_BUTTON(player->small.full), value);
}

/* TODO : update doc */
/**
 * @brief Set the volume slider to the value position
 * @param value A vol value in percent
 * @param gui A Emphasis_Gui
 */
void
emphasis_player_vol_slider_set(Emphasis_Player_Gui *player, int value)
{
  etk_range_value_set(ETK_RANGE(player->full.sound_slider) , (double) value);
  etk_range_value_set(ETK_RANGE(player->small.sound_slider), (double) value);
}

/* TODO : doc */
void
emphasis_player_vol_image_set(Emphasis_Player_Gui *player,
                              int right,
                              const char *path)
{
  switch(right)
    {
     case 0:
       etk_image_set_from_file(ETK_IMAGE(player->small.sound_low), path);
       etk_image_set_from_file(ETK_IMAGE(player->full.sound_low) , path);
       break;
     default:
       etk_image_set_from_file(ETK_IMAGE(player->small.sound_high), path);
       etk_image_set_from_file(ETK_IMAGE(player->full.sound_high) , path);
       break;
    }
}

void
emphasis_player_mode_set(Emphasis_Player_Gui *player,
                         Emphasis_Mode state)
{
  switch (state)
    {
    case EMPHASIS_FULL:
      cb_switch_full(NULL, player);
      break;
    case EMPHASIS_SMALL:
      cb_switch_small(NULL, player);
      break;
    }
}

void
emphasis_player_force_mode_set(Emphasis_Player_Gui *player,
                               Emphasis_Mode state)
{
  player->state = (state == EMPHASIS_SMALL) ? EMPHASIS_FULL : EMPHASIS_SMALL;
  emphasis_player_mode_set(player, state);
}
