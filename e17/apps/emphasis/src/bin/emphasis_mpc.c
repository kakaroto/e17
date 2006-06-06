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

	mo = mpd_new((char*)hostname, port, (char*)password);
	if (!mpd_connect(mo))
	{
		mpd_send_password(mo);
		printf("MPD daemon found at %s:%d\n", hostname, port);
		timer = ecore_timer_add(0.2, mpc_update, NULL);
	}
	else
	{
		printf("MPD daemon not found at %s:%d \n", hostname, port);
		timer = NULL;
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
	mpd_signal_connect_status_changed(mo, (StatusChangedCallback)status_changed_callback, data);
	mpd_signal_connect_connection_changed(mo, (ConnectionChangedCallback)mpc_connection_changed, data);
}

/**
 * @brief Update the mpd connection
 * @param data the callback data
 * @return 1 if no error occur during the update
 */
int
mpc_update(void *data)
{
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
status_changed_callback(MpdObj *mo, ChangedStatusType what, void *data)
{
	mpd_Song *song = NULL;
	MpdState state;
	int elapsed_time, total_time, vol_value;
	MpdData *playlist;
	
	if (what&MPD_CST_VOLUME)
	{
		vol_value = mpd_status_get_volume(mo);
		emphasis_vol_slider_set(vol_value, data);
	}
	if (what&MPD_CST_ELAPSED_TIME)
	{
		elapsed_time = mpd_status_get_elapsed_song_time(mo);
		total_time = mpd_status_get_total_song_time(mo);
		emphasis_player_progress_set((float)elapsed_time, total_time, data);
	}
	if (what&MPD_CST_UPDATING)
	{
		emphasis_player_info_set(NULL, "update", data);
	}
	if (what&MPD_CST_RANDOM)
	{
		if (mpd_player_get_random(mo))
		{
			emphasis_toggle_random(TRUE, data);
		}
		else
		{
			emphasis_toggle_random(FALSE, data);
		}
	}
	if (what&MPD_CST_REPEAT)
	{	
		if (mpd_player_get_repeat(mo))
		{
			emphasis_toggle_repeat(TRUE, data);
		}
		else
		{
			emphasis_toggle_repeat(FALSE, data);
		}
	}
	if (what&MPD_CST_STATE)
	{
		state = mpd_player_get_state(mo);
		switch (state)
		{
			case MPD_STATUS_STATE_STOP:
				emphasis_player_info_set(NULL, "Music Stoped", data);
				emphasis_toggle_play(data);
				emphasis_pls_mark_current(ETK_TREE(((Emphasis_Gui *)data)->tree_pls), -1);
				break;
			case MPD_STATUS_STATE_PAUSE:
				song = mpd_playlist_get_current_song(mo);
				emphasis_toggle_play(data);
				emphasis_player_info_set(song, "paused", data);
				break;
			case MPD_STATUS_STATE_PLAY:
				song = mpd_playlist_get_current_song(mo);
				emphasis_toggle_play(data);
				emphasis_player_info_set(song, NULL, data);
				emphasis_pls_mark_current(ETK_TREE(((Emphasis_Gui *)data)->tree_pls), song->id);
				break;
			case MPD_STATUS_STATE_UNKNOWN:
				emphasis_player_info_set(NULL, "wtf is that ?", data);
				break;
		}
	}
	if (what&MPD_CST_PLAYLIST)
	{
		playlist = mpd_playlist_get_changes(mo, -1);
		emphasis_tree_pls_set(ETK_TREE(((Emphasis_Gui *)data)->tree_pls), playlist);
	}
	if (what&MPD_CST_SONGID)
	{
		song = mpd_playlist_get_current_song(mo);
		if (song)
		{
			emphasis_player_info_set(song, NULL, data);
			emphasis_pls_mark_current(ETK_TREE(((Emphasis_Gui *)data)->tree_pls), song->id);
		}
	}
}	

void
mpc_connection_changed(MpdObj *mo, int connect, void *data)
{
	if (!connect)
	{
		emphasis_player_info_set(NULL, "Not connected to MPD daemon", data);
	//	emphasis_clear(data);
		ecore_timer_del(((Emphasis_Gui*)data)->timer);
		((Emphasis_Gui*)data)->timer = ecore_timer_add(0.2, emphasis_try_connect, data);
	}
	if (connect)
	{
		printf("platypus\n");
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
MpdData *
mpc_mlib_artist_get(void)
{
	MpdData *data;
	data = mpd_database_get_artists(mo);
	return data;
}

/**
 * @brief Get an albums list of artist from mpd database
 * @param artist An artist name
 * @return A list of albums
 */
MpdData *
mpc_mlib_album_get(char *artist)
{
	MpdData *data;
	data = mpd_database_get_albums(mo, artist);
	return data;
}

/**
 * @brief Get a list of song matching artist and album
 * @param artist An artist name
 * @param album An album name
 * @return A list of song matchin artist and album
 */
MpdData *
mpc_mlib_track_get(char *artist, char *album)
{
	MpdData *data;
	
	if ((album != NULL) || (artist != NULL))
	{
		data = mpd_database_find_adv(mo, 1,MPD_TAG_ITEM_ARTIST, artist,  MPD_TAG_ITEM_ALBUM, album, -1);
	}
	else
	{
		data = mpd_database_get_complete(mo);
	}
	
	return data;
}

/**
 * @brief Add to the current playlist a list of songs
 * @param list A list of songs
 * @return The list of songs added with their id
 */
void
mpc_playlist_add(MpdData *list)
{
	long long id;
	MpdData *next;
	
	id = mpd_playlist_get_playlist_id(mo);
	
	while (list)
	{
		mpd_playlist_queue_add(mo, list->song->file);
		list = mpd_data_get_next(list);
	}
	
	mpd_playlist_queue_commit(mo);
}

/**
 * @brief Delete the id of the mpd playlist
 * @param id 
 */
void 
mpc_playlist_delete(MpdData *list)
{
	while (list)
	{
		mpd_playlist_queue_delete_id(mo, list->song->id);
		list = mpd_data_get_next(list);		
	}
	mpd_playlist_queue_commit(mo);
	mpd_data_free(list);
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
		mpd_player_seek(mo, (int)(percent*song->time));
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
		if (mpc_assert_status(MPD_PLAYER_PAUSE) || mpc_assert_status(MPD_PLAYER_STOP))
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
		song = mpd_playlist_get_changes(mo, mpd_playlist_get_old_playlist_id(mo));
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
 * @brief Changed mpd volume level
 */
void
mpc_change_vol(int value)
{
	mpd_status_set_volume(mo, value);
}

void
mpc_database_update(char *path)
{
	int rep;
	rep = mpd_database_update_dir(mo, path);
	if (rep == 0)
		printf("cette fonction est indisponible... coin coin\n");
}
