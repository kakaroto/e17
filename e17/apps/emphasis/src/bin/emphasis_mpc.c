#include "emphasis.h"
#include "emphasis_mpc.h"

/**
 * @brief Init a connection to the mpd daemon
 * @param hostname the mpd daemon hostname
 * @param port the port it use
 * @param password the password to be used, NULL if it hasn't
 * @return The timer of the updates
 */
Ecore_Timer *
mpc_init(const char *hostname, int port, const char *password)
{
  Ecore_Timer *timer;

  mo = mpd_new((char *) hostname, port, (char *) password);
  if (!mpd_connect(mo))
    {
      mpd_send_password(mo);
      timer = ecore_timer_add(0.2, mpc_update, NULL);
    }
  else
    {
      timer = NULL;
    }

  if (!mpd_server_check_version(mo, 0, 12, 0))
    {
      fprintf(stderr, "Wrong server version. Emphasis require MPD 0.12.0\n");
      mpc_disconnect();
      exit(-1);
    }

  return timer;
}

/**
 * @brief Connect a Emphasis_Gui to mpd status signals
 * @param data A Emphasis_Gui
 */
void
mpc_signal_connect_status_changed(void *data)
{
  mpd_signal_connect_status_changed(mo,
                                    (StatusChangedCallback)
                                    status_changed_callback, data);
  mpd_signal_connect_connection_changed(mo,
                                        (ConnectionChangedCallback)
                                        mpc_connection_changed, data);
}

/**
 * @brief Update the mpd connection
 * @param data the callback data
 * @return 1 if no error occur during the update
 */
int
mpc_update(void *data)
{
  UNUSED(data)

  mpd_status_update(mo);
  return 1;
}

/**
 * @brief Global callback call when the mpd status change
 * @param mo The mpd connection
 * @param what Type of the changed status
 * @param data A Emphasis_Gui
 */
void
status_changed_callback(MpdObj * mo, ChangedStatusType what, void *data)
{
  Emphasis_Player_Gui *player;
  mpd_Song *song = NULL;
  MpdState state;
  int elapsed_time, total_time, vol_value;
  MpdData *playlist;
  
  player = ((Emphasis_Gui *)data)->player;

  if (what & MPD_CST_VOLUME)
    {
      vol_value = mpd_status_get_volume(mo);
      emphasis_player_vol_slider_set(player, vol_value);
    }
  if (what & MPD_CST_ELAPSED_TIME)
    {
      elapsed_time = mpd_status_get_elapsed_song_time(mo);
      total_time = mpd_status_get_total_song_time(mo);
      emphasis_player_progress_set(player, (float) elapsed_time, total_time);
    }
  if (what & MPD_CST_UPDATING)
    {
      emphasis_player_info_set(player, NULL, "update");
    }
  if (what & MPD_CST_RANDOM)
    {
      if (mpd_player_get_random(mo))
        {
          emphasis_player_toggle_random(player, TRUE);
        }
      else
        {
          emphasis_player_toggle_random(player, FALSE);
        }
    }
  if (what & MPD_CST_REPEAT)
    {
      if (mpd_player_get_repeat(mo))
        {
          emphasis_player_toggle_repeat(player, TRUE);
        }
      else
        {
          emphasis_player_toggle_repeat(player, FALSE);
        }
    }
  if (what & MPD_CST_STATE)
    {
      state = mpd_player_get_state(mo);
      switch (state)
        {
        case MPD_STATUS_STATE_STOP:
          emphasis_player_info_set(player, NULL, "Music Stoped");
          emphasis_player_toggle_play(player);
          emphasis_pls_mark_current(ETK_TREE(player->media.pls), -1);
          break;
        case MPD_STATUS_STATE_PAUSE:
          song = mpd_playlist_get_current_song(mo);
          emphasis_player_toggle_play(player);
          emphasis_player_info_set(player, song, "paused");
          break;
        case MPD_STATUS_STATE_PLAY:
          song = mpd_playlist_get_current_song(mo);
          emphasis_player_toggle_play(player);
          emphasis_player_info_set(player, song, NULL);
          emphasis_pls_mark_current(ETK_TREE(player->media.pls), song->id);
          break;
        case MPD_STATUS_STATE_UNKNOWN:
          emphasis_player_info_set(player, NULL, "wtf is that ?");
          break;
        }
    }
  if (what & MPD_CST_PLAYLIST)
    {
      playlist = mpd_playlist_get_changes(mo, -1);
      emphasis_tree_pls_set(ETK_TREE(player->media.pls),
                            convert_mpd_data(playlist));
      mpd_data_free(playlist);
    }
  if (what & MPD_CST_SONGID)
    {
      song = mpd_playlist_get_current_song(mo);
      if (song)
        {
          emphasis_player_info_set(player, song, NULL);
          emphasis_pls_mark_current(ETK_TREE(player->media.pls), song->id);
          emphasis_cover_change((Emphasis_Gui *) data, strdup(song->artist),
                                strdup(song->album));
        }
    }
}

void
mpc_connection_changed(MpdObj * mo, int connect, void *data)
{
  UNUSED(mo)
  Emphasis_Gui *gui;
  
  gui = data;

  if (!connect)
    {
      emphasis_player_info_set(gui->player, NULL, "Not connected to MPD");
      ecore_timer_del(gui->timer);
      gui->timer = ecore_timer_add(0.2, emphasis_try_connect, data);
    }
}

/**
 * @brief Check the current status of the mpd daemon
 * @param status Asserted daemon status
 * @return TRUE if the assertion is true
 */
int
mpc_assert_status(MpdState status)
{
  MpdState current_state;

  current_state = mpd_player_get_state(mo);
  if (current_state == status)
    {
      return TRUE;
    }
  else
    {
      return FALSE;
    }
}

/**
 * @brief Get the artists list from mpd database
 * @return A list of all artists
 */
Evas_List *
mpc_mlib_artist_get(void)
{
  MpdData *data;
  Evas_List *list;

  data = mpd_database_get_artists(mo);
  list = convert_mpd_data(data);

  mpd_data_free(data);
  return list;
}

/**
 * @brief Get an albums list of artist from mpd database
 * @param artist An artist name
 * @return A list of albums
 */
Evas_List *
mpc_mlib_album_get(char *artist)
{
  MpdData *data;
  Evas_List *list;

  data = mpd_database_get_albums(mo, artist);
  list = convert_mpd_data(data);

  mpd_data_free(data);
  return list;
}

/**
 * @brief Get a list of song matching artist and album
 * @param artist An artist name
 * @param album An album name
 * @return A list of song matchin artist and album
 */
Evas_List *
mpc_mlib_track_get(char *artist, char *album)
{
  MpdData *data;
  Evas_List *list;

  if ((album != NULL) || (artist != NULL))
    {
      data =
        mpd_database_find_adv(mo, 1, MPD_TAG_ITEM_ARTIST, artist,
                              MPD_TAG_ITEM_ALBUM, album, -1);
    }
  else
    {
      data = mpd_database_get_complete(mo);
    }
  list = convert_mpd_data(data);

  mpd_data_free(data);
  return list;
}

/**
 * @brief Add to the current playlist a list of songs
 * @param list A list of songs
 * @return The list of songs added with their id
 */
void
mpc_playlist_add(Evas_List *list)
{
  long long id;
  Emphasis_Data *data;

  id = mpd_playlist_get_playlist_id(mo);

  while (list)
    {
      data = evas_list_data(list);
      mpd_playlist_queue_add(mo, data->song->file);
      list = evas_list_next(list);
    }

  mpd_playlist_queue_commit(mo);
  emphasis_list_free(list);
}

/**
 * @brief Delete the id of the mpd playlist
 * @param id 
 */
void
mpc_playlist_delete(Evas_List *list)
{
  Emphasis_Data *data;

  while (list)
    {
      data = evas_list_data(list);
      mpd_playlist_queue_delete_id(mo, data->song->id);
      list = evas_list_next(list);
    }
  mpd_playlist_queue_commit(mo);
  emphasis_list_free(list);
}

/**
 * @brief Clear mpd playlist
 */
void
mpc_playlist_clear(void)
{
  mpd_playlist_clear(mo);
}

/**
 * @brief Play the song with id @e id
 * @param id An id of one of the songs in the playlist
 */
void
mpc_play_id(int id)
{
  mpd_player_play_id(mo, id);
}

/**
 * @brief Play the current song
 */
void
mpc_play(void)
{
  mpd_player_play(mo);
}

/**
 * @brief Pause the current song
 */
void
mpc_pause(void)
{
  mpd_player_pause(mo);
}

/**
 * @brief Stop the playing
 */
void
mpc_stop(void)
{
  mpd_player_stop(mo);
}

/**
 * @brief Play the previous song
 */
void
mpc_prev(void)
{
  mpd_player_prev(mo);
}

/**
 * @brief Play the next song
 */
void
mpc_next(void)
{
  mpd_player_next(mo);
}

/**
 * @brief Get mpd play the song at percent
 * @param percent The position is the song in percent to seek
 */
void
mpc_seek(double percent)
{
  mpd_Song *song;

  song = mpd_playlist_get_current_song(mo);
  if (song)
    {
      mpd_player_seek(mo, (int) (percent * song->time));
    }
}

/**
 * @brief Toggle play/pause status on mpd
 */
void
mpc_toggle_play_pause(void)
{
  if (mpc_assert_status(MPD_PLAYER_PLAY))
    {
      mpc_pause();
    }
  else
    {
      if (mpc_assert_status(MPD_PLAYER_PAUSE)
          || mpc_assert_status(MPD_PLAYER_STOP))
        {
          mpc_play();
        }
    }
}

void
mpc_play_if_stopped(void)
{
  MpdData *song;

  if (mpc_assert_status(MPD_PLAYER_STOP))
    {
      song =
        mpd_playlist_get_changes(mo, mpd_playlist_get_old_playlist_id(mo));
      mpc_play_id(song->song->id);
    }
}

/**
 * @brief Toggle the random mode on mpd
 */
void
mpc_toggle_random(void)
{
  if (mpd_player_get_random(mo))
    {
      mpd_player_set_random(mo, 0);
    }
  else
    {
      mpd_player_set_random(mo, 1);
    }
}

/**
 * @brief Toggle the repeat mode on mpd
 */
void
mpc_toggle_repeat(void)
{
  if (mpd_player_get_repeat(mo))
    {
      mpd_player_set_repeat(mo, 0);
    }
  else
    {
      mpd_player_set_repeat(mo, 1);
    }
}

/**
 * @brief Get mpd volume value
 */
int
mpc_get_vol(void)
{
  return mpd_status_get_volume(mo);
}

/**
 * @brief Changed mpd volume level
 */
void
mpc_change_vol(int value)
{
  mpd_status_set_volume(mo, value);
}

void
mpc_database_update(void)
{
  mpd_database_update_dir(mo, "/");
}

void
mpc_disconnect(void)
{
  mpd_free(mo);
}

Evas_List *
mpc_list_playlists(void)
{
#if defined(LIBMPD_0_12_4)
  MpdData *data;
  Evas_List *list;

  data = mpd_database_list_playlist(mo);
  list = convert_mpd_data(data);

  mpd_data_free(data);
  return list;
#else
  return NULL;
#endif
}

Evas_List *
mpc_get_playlist_content(char *playlist_name)
{
#if defined(LIBMPD_0_12_4)
  MpdData *data;
  Evas_List *list;
  
  data = mpd_database_get_playlist_content(mo, playlist_name);
  list = convert_mpd_data(data);

  mpd_data_free(data);
  return list;
#else
  UNUSED(playlist_name);
  return NULL;
#endif
}

void
mpc_save_playlist(char *playlist_name)
{
  int mpd_error;

  mpd_error = mpd_database_save_playlist(mo, playlist_name);
  if (mpd_error != MPD_OK)
    {
      if (mpd_error == MPD_DATABASE_PLAYLIST_EXIST)
        {
          fprintf(stderr, "A playlist with the same name already exist.\n");
        }
    }
}

void
mpc_delete_playlist(char *playlist_name)
{
#if defined(LIBMPD_0_12_4)
  int mpd_error;

  mpd_error = mpd_database_delete_playlist(mo, playlist_name);
  if (mpd_error != MPD_OK)
    {
      fprintf(stderr, "An error occur during the playlist deleting\n");
    }
#else
  UNUSED(playlist_name);
#endif
}

#if defined(LIBMPD_0_12_4)
MpdData *mpd_database_list_playlist(MpdObj *mi)
{
	MpdData *data = NULL;
	mpd_InfoEntity *ent = NULL;

	if(!mpd_check_connected(mi))
	{
		return NULL;
	}
	if(mpd_lock_conn(mi))
	{
		return NULL;
	}

	mpd_sendLsInfoCommand(mi->connection, "");

	while((ent = mpd_getNextInfoEntity(mi->connection))) 
	{
		if(ent->type == MPD_INFO_ENTITY_TYPE_PLAYLISTFILE) 
		{
			data = mpd_new_data_struct_append(data);
			data->type = MPD_DATA_TYPE_PLAYLIST;
			data->playlist = ent->info.playlistFile->path;
      ent->info.playlistFile->path=NULL;
		}
		mpd_freeInfoEntity(ent);
	}

	mpd_finishCommand(mi->connection);

	mpd_unlock_conn(mi);
	if(data == NULL)
	{
		return NULL;
	}
	return mpd_data_get_first(data);
}
#endif

