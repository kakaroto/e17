#include <config.h>
#include <Edje.h>
#include <Esmart/container.h>
#include <assert.h>
#include "eplayer.h"
#include "track.h"
#include "interface.h"
#include "utils.h"
#include "callbacks.h"

typedef enum {
	PLAYBACK_STATE_STOPPED,
	PLAYBACK_STATE_PAUSED,
	PLAYBACK_STATE_PLAYING
} PlaybackState;

/*static int paused = 0;*/
static PlaybackState state = PLAYBACK_STATE_STOPPED;

/**
 * Starts/resumes playback.
 *
 * @param player
 * @param e
 * @param o
 * @param event
 */
EDJE_CB(play) {
	debug(DEBUG_LEVEL_INFO, "Play callback entered\n");

	switch (state) {
		case PLAYBACK_STATE_STOPPED:
		case PLAYBACK_STATE_PAUSED: /* continue playback */
			eplayer_playback_start(player, 0);
			break;
		case PLAYBACK_STATE_PLAYING: /* restart from beginning */
			eplayer_playback_stop(player);
			eplayer_playback_start(player, 1);
			break;
		default:
			assert(0);
			break;
	}
	
	state = PLAYBACK_STATE_PLAYING;
}

/**
 * Stops playback.
 *
 * @param player
 * @param e
 * @param o
 * @param event
 */
EDJE_CB(stop) {
	debug(DEBUG_LEVEL_INFO, "Stop callback entered\n");

	eplayer_playback_stop(player);
	track_rewind(player);
	state = PLAYBACK_STATE_STOPPED;
}

/**
 * Pauses/resumes playback.
 *
 * @param player
 * @param e
 * @param o
 * @param event
 */
EDJE_CB(pause) {
	debug(DEBUG_LEVEL_INFO, "Pause callback entered\n");

	switch (state) {
		case PLAYBACK_STATE_STOPPED:
			return;
			break;
		case PLAYBACK_STATE_PAUSED:
			eplayer_playback_start(player, 0);
			state = PLAYBACK_STATE_PLAYING;
			break;
		case PLAYBACK_STATE_PLAYING:
			eplayer_playback_stop(player);
			state = PLAYBACK_STATE_PAUSED;
			break;
		default:
			assert(0);
			break;
	}
}

/**
 * Moves to the next track and plays it, except when we're going
 * back to the beginning of the playlist.
 *
 * @param player
 * @param e
 * @param o
 * @param event
 */
EDJE_CB(track_next) {
	int play = 1;
	
	debug(DEBUG_LEVEL_INFO, "Next File Called\n");

	eplayer_playback_stop(player);

	/* check whether we moved to the beginning of the list */
	if (playlist_current_item_next(player->playlist))
		play = player->cfg.repeat;

	if (play) {
		eplayer_playback_start(player, 1);
		state = PLAYBACK_STATE_PLAYING;
	} else {
		/* refresh track info parts, but don't start playing yet */
		track_open(player);
		state = PLAYBACK_STATE_STOPPED;
	}
}

/**
 * Moves to the previous track and plays it, except when we're
 * at the first track already.
 *
 * @param player
 * @param e
 * @param o
 * @param event
 */
EDJE_CB(track_prev) {
	debug(DEBUG_LEVEL_INFO, "Previous File Called\n");

	/* first item on the list: do nothing */
	if (!playlist_current_item_has_prev(player->playlist))
		return;

	eplayer_playback_stop(player);
	
	/* Get the previous list item */
	playlist_current_item_prev(player->playlist);

	eplayer_playback_start(player, 1);
	state = PLAYBACK_STATE_PLAYING;
}

EDJE_CB(volume_raise) {
	int left = 0, right = 0;
	
	debug(DEBUG_LEVEL_INFO, "Raising volume\n");

	if (!player->output->volume_get(&left, &right))
		return;
	
	player->output->volume_set(left + 5, right + 5);
	ui_refresh_volume(player);
}

EDJE_CB(volume_lower) {
	int left = 0, right = 0;
	
	debug(DEBUG_LEVEL_INFO, "Lowering volume\n");
	
	if (!player->output->volume_get(&left, &right))
		return;
	
	player->output->volume_set(left - 5, right - 5);
	ui_refresh_volume(player);
}

EDJE_CB(time_display_toggle) {
	player->cfg.time_display = !player->cfg.time_display;
	track_update_time(player);
}

EDJE_CB(repeat_mode_toggle) {
	player->cfg.repeat = !player->cfg.repeat;
}

EDJE_CB(playlist_scroll_up) {
	/* it's * 3 because we're scrolling 3 elements at once */
	e_container_scroll(player->gui.playlist,
	                   player->gui.playlist_font_size * 3);
}

EDJE_CB(playlist_scroll_down) {
	/* it's * 3 because we're scrolling 3 elements at once */
	e_container_scroll(player->gui.playlist,
	                   player->gui.playlist_font_size * -3);
}

EDJE_CB(playlist_item_play) {
	PlayListItem *pli = evas_object_data_get(obj, "PlayListItem");

	eplayer_playback_stop(player);

	playlist_current_item_set(player->playlist, pli);
	eplayer_playback_start(player, 1);
	state = PLAYBACK_STATE_PLAYING;
}

EDJE_CB(playlist_item_remove) {
	int ok = 0;
	PlayListItem *pli = evas_object_data_get(obj, "PlayListItem");
	
	e_container_element_remove(player->gui.playlist, obj);
	evas_object_del(obj);
	
	if (pli == playlist_current_item_get(player->playlist))
	{
	    eplayer_playback_stop(player);
		
	    if (playlist_current_item_has_next(player->playlist))
			ok = playlist_current_item_next(player->playlist);
		
	    if (!ok && playlist_current_item_has_prev(player->playlist))
			ok = playlist_current_item_prev(player->playlist);
	    
		if (!ok)
			playlist_remove_all(player->playlist);
	}

	playlist_remove_item(player->playlist, pli);
}

EDJE_CB(playlist_item_selected) {
	Evas_List *items = e_container_elements_get(player->gui.playlist);
	Evas_List *l;

	for (l = items; l; l = l->next)
		if (l->data != obj)
			edje_object_signal_emit(l->data,
			                        "PLAYLIST_ITEM_UNSELECTED", "");
}

EDJE_CB(seek_forward) {
	PlayListItem *pli = playlist_current_item_get(player->playlist);

	debug(DEBUG_LEVEL_INFO, "Seeking forward\n");

	/* We don't care if you seek past the file, the play loop
	 * will catch EOF and play next file
	 */
	eplayer_playback_stop(player);
	pli->plugin->set_current_pos(pli->plugin->get_current_pos() + 5);
	eplayer_playback_start(player, 0);
	state = PLAYBACK_STATE_PLAYING;
}

EDJE_CB(seek_backward) {
	PlayListItem *pli = playlist_current_item_get(player->playlist);
	int cur_time  = pli->plugin->get_current_pos();
	
	debug(DEBUG_LEVEL_INFO, "Seeking backward - Current Pos: %i\n",
	      cur_time);

	eplayer_playback_stop(player);

	if (cur_time < 6) /* restart from the beginning */
		eplayer_playback_start(player, 1);
	else {
		pli->plugin->set_current_pos(cur_time - 5);
		eplayer_playback_start(player, 0);
	}
	
	state = PLAYBACK_STATE_PLAYING;
}

EDJE_CB(eplayer_quit) {
	ecore_main_loop_quit();
}

EDJE_CB(eplayer_raise) {
	ecore_evas_raise(player->gui.ee);
}

EDJE_CB(switch_group) {
	evas_object_del(player->gui.edje);
	ui_init_edje(player, src);
}
