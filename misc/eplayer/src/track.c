#include <config.h>
#include <Edje.h>
#include <assert.h>
#include "eplayer.h"
#include "interface.h"

/**
 * Plays a chunk of the current track.
 *
 * @param udata Pointer to an ePlayer struct.
 */
int track_play_chunk(void *udata) {
	ePlayer *player = udata;
	PlayListItem *pli = player->playlist->cur_item->data;
	InputPlugin *plugin = pli->plugin;
	int bytes_read, big_endian = 0;
	static unsigned char pcmout[8192];

#ifdef WORDS_BIGENDIAN
	big_endian = 1;
#endif
	
	/* read the data ... */
	bytes_read = plugin->read(pcmout, sizeof(pcmout), big_endian);
	
	if (bytes_read) /* ...and play it */
		player->output->play(pcmout, bytes_read);
	else /* EOF -> move to the next track */
		edje_object_signal_emit(player->gui.edje,
		                        "PLAY_NEXT", "next_button");

	/* the edje callback will re-add the idler, so we can remove it here,
	 * in case ov_read() failed
	 */
	return !!bytes_read;
}

int track_update_time(void *udata) {
	ePlayer *player = udata;
	PlayListItem *current_item = player->playlist->cur_item->data;
	static int old_time = -1;
	int cur_time;

	cur_time = current_item->plugin->get_current_pos();
	
	if (player->cfg.time_display == TIME_DISPLAY_LEFT)
		cur_time = current_item->duration - cur_time;

	if (cur_time == old_time) /* value didn't change, so don't update */
		return 1;

	old_time = cur_time;
	refresh_time(player, cur_time);

	return 1;
}

static int prepare_output(ePlayer *player) {
	PlayListItem *current = player->playlist->cur_item->data;
	int bigendian = 0;

#ifdef WORDS_BIGENDIAN
	bigendian = 1;
#endif

	return player->output->configure(current->channels,
	                                 current->sample_rate,
	                                 16, bigendian);
}

/**
 * Closes the current track.
 *
 * @param player
 */
void track_close(ePlayer *player) {
	PlayListItem *pli = player->playlist->cur_item->data;
	
	pli->plugin->close();
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

	pli->plugin->open(pli->file);

	edje_object_part_text_set(player->gui.edje, "song_name",
	                          pli->comment[COMMENT_ID_TITLE]);
	edje_object_part_text_set(player->gui.edje, "artist_name",
	                          pli->comment[COMMENT_ID_ARTIST]);
	edje_object_part_text_set(player->gui.edje, "album_name",
	                          pli->comment[COMMENT_ID_ALBUM]);
	edje_object_part_text_set(player->gui.edje, "time_text", "0:00");

	if (!prepare_output(player)) {
		fprintf(stderr, "Cannot configure output plugin\n");
	
		/* move to the next track */
		edje_object_signal_emit(player->gui.edje,
		                        "PLAY_NEXT", "next_button");
	}
}

