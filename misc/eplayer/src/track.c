#include <config.h>
#include <Edje.h>
#include <assert.h>
#include "eplayer.h"
#include "interface.h"
#include "utils.h"

/**
 * Plays a chunk of the current track.
 *
 * @param udata Pointer to an ePlayer struct.
 */
void track_play_chunk(void *udata) {
	ePlayer *player = udata;
	PlayListItem *pli = playlist_current_item_get(player->playlist);
	int read;
	unsigned char *buf = NULL;

	while (!player->playback_stop &&
	       (read = pli->plugin->read(&buf)) != EOF) {
		player->output->play(buf, read);

		pthread_mutex_lock(&pli->pos_mutex);
		pli->current_pos = pli->plugin->get_current_pos();
		pthread_mutex_unlock(&pli->pos_mutex);
	}
	
	if (!player->playback_stop) {
		/* EOF -> move to the next track */
		pthread_mutex_lock(&player->playback_next_mutex);
		player->playback_next = 1;
		pthread_mutex_unlock(&player->playback_next_mutex);
	}
}

int track_update_time(void *udata) {
	ePlayer *player = udata;
	PlayListItem *current_item = playlist_current_item_get(player->playlist);
	static int old_time = -1;
	int cur_time;

	cur_time = current_item->current_pos;
	
	if (player->cfg.time_display == TIME_DISPLAY_LEFT)
		cur_time = current_item->duration - cur_time;

	if (cur_time == old_time) /* value didn't change, so don't update */
		return 1;

	old_time = cur_time;
	ui_refresh_time(player, cur_time);

	return 1;
}

/**
 * Closes the current track.
 *
 * @param player
 */
void track_close(ePlayer *player) {
	PlayListItem *pli;
	
	if ((pli = playlist_current_item_get(player->playlist)))
		pli->plugin->close();
}

/**
 * Opens the current track and configured the output plugin for playback.
 *
 * @param player
 */
void track_open(ePlayer *player) {
	PlayListItem *pli = playlist_current_item_get(player->playlist);

	assert(pli);

	pli->current_pos = 0;
	pli->plugin->open(pli->file);

	ui_fill_track_info(player);

	if (!player->output->configure(pli->channels, pli->sample_rate, 16)) {
		debug(DEBUG_LEVEL_CRITICAL, "Cannot configure output plugin\n");
	
		/* move to the next track */
		edje_object_signal_emit(player->gui.edje,
		                        "PLAY_NEXT", "next_button");
	}
}

/**
 * Rewinds the current track.
 *
 * @param player
 */
void track_rewind(ePlayer *player) {
	track_close(player);
	track_open(player);
}
