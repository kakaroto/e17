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
	PlayListItem *pli = player->playlist->cur_item->data;
	int read;
	unsigned char *buf = NULL;

	while (!player->playback_stop &&
	       (read = pli->plugin->read(&buf))) {
		player->output->play(buf, read);

		pthread_mutex_lock(&pli->pos_mutex);
		pli->current_pos = pli->plugin->get_current_pos();
		pthread_mutex_unlock(&pli->pos_mutex);
	}
	
	if (!player->playback_stop) /* EOF -> move to the next track */
		edje_object_signal_emit(player->gui.edje,
		                        "PLAY_NEXT", "next_button");
}

int track_update_time(void *udata) {
	ePlayer *player = udata;
	PlayListItem *current_item = player->playlist->cur_item->data;
	static int old_time = -1;
	int cur_time;

	cur_time = current_item->current_pos;
	
	if (player->cfg.time_display == TIME_DISPLAY_LEFT)
		cur_time = current_item->duration - cur_time;

	if (cur_time == old_time) /* value didn't change, so don't update */
		return 1;

	old_time = cur_time;
	refresh_time(player, cur_time);

	return 1;
}

/**
 * Closes the current track.
 *
 * @param player
 */
void track_close(ePlayer *player) {
	PlayListItem *pli;
	
	if (player->playlist && player->playlist->cur_item) {
		pli = player->playlist->cur_item->data;
		pli->plugin->close();
	}
}

/**
 * Opens the current track and configured the output plugin for playback.
 *
 * @param player
 */
void track_open(ePlayer *player) {
	PlayListItem *pli;

	assert(player->playlist->cur_item);
	pli = player->playlist->cur_item->data;
	pli->current_pos = 0;

	pli->plugin->open(pli->file);

	edje_object_part_text_set(player->gui.edje, "song_name",
	                          pli->comment[COMMENT_ID_TITLE]);
	edje_object_part_text_set(player->gui.edje, "artist_name",
	                          pli->comment[COMMENT_ID_ARTIST]);
	edje_object_part_text_set(player->gui.edje, "album_name",
	                          pli->comment[COMMENT_ID_ALBUM]);
	edje_object_part_text_set(player->gui.edje, "time_text", "0:00");

	if (!player->output->configure(pli->channels, pli->sample_rate, 16)) {
		debug(DEBUG_LEVEL_CRITICAL, "Cannot configure output plugin\n");
	
		/* move to the next track */
		edje_object_signal_emit(player->gui.edje,
		                        "PLAY_NEXT", "next_button");
	}
}

