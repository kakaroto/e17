#include <config.h>
#include <Edje.h>
#include <Esmart/container.h>
#include "eplayer.h"
#include "track.h"
#include "interface.h"
#include "utils.h"

static int paused = 0;

/**
 * Starts/resumes playback.
 *
 * @param player
 * @param e
 * @param o
 * @param event
 */
void cb_play(ePlayer *player, Evas *e, Evas_Object *o, void *event) {
	debug(DEBUG_LEVEL_INFO, "Play callback entered\n");

	if (!paused) { /* restart from beginning */
		eplayer_playback_stop(player);
		eplayer_playback_start(player, 1);
	} else { /* continue playback */
		eplayer_playback_start(player, 0);
		paused = 0;
	}
}

/**
 * Pauses/resumes playback.
 *
 * @param player
 * @param e
 * @param o
 * @param event
 */
void cb_pause(ePlayer *player, Evas *e, Evas_Object *o, void *event) {
	debug(DEBUG_LEVEL_INFO, "Pause callback entered\n");
	
	if (paused)
		eplayer_playback_start(player, 0);
	else
		eplayer_playback_stop(player);

	paused = !paused;
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
void cb_track_next(ePlayer *player, Evas *e, Evas_Object *o,
                   void *event) {
	debug(DEBUG_LEVEL_INFO, "Next File Called\n");

	eplayer_playback_stop(player);

	if (player->playlist->cur_item->next) {
		player->playlist->cur_item = player->playlist->cur_item->next;
		eplayer_playback_start(player, 1);
		paused = 0;
	} else {
		/* there's no next item, so move to the beginning again
		 * but don't start playing yet.
		 */
		player->playlist->cur_item = player->playlist->items;
		track_open(player); /* refresh track info parts */
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
void cb_track_prev(ePlayer *player, Evas *e, Evas_Object *o,
                   void *event) {
	debug(DEBUG_LEVEL_INFO, "Previous File Called\n");

	/* first item on the list: do nothing */
	if (!player->playlist->cur_item->prev)
		return;

	eplayer_playback_stop(player);
	
	/* Get the previous list item */
	player->playlist->cur_item = player->playlist->cur_item->prev;

	eplayer_playback_start(player, 1);
	paused = 0;
}

void cb_volume_raise(ePlayer *player, Evas_Object *obj,
                     const char *emission, const char *src) {
	int left = 0, right = 0;
	
	debug(DEBUG_LEVEL_INFO, "Raising volume\n");

	if (!player->output->volume_get(&left, &right))
		return;
	
	player->output->volume_set(left + 5, right + 5);
	refresh_volume(player);
}

void cb_volume_lower(ePlayer *player, Evas_Object *obj,
                     const char *emission, const char *src) {
	int left = 0, right = 0;
	
	debug(DEBUG_LEVEL_INFO, "Lowering volume\n");
	
	if (!player->output->volume_get(&left, &right))
		return;
	
	player->output->volume_set(left - 5, right - 5);
	refresh_volume(player);
}

void cb_time_display_toggle(ePlayer *player, Evas_Object *obj,
                            const char *emission, const char *src) {
	player->cfg.time_display = !player->cfg.time_display;
	track_update_time(player);
}

/**
 * Scrolls the playlist containers.
 *
 * @param player
 * @param direction 1 (up) or -1 (down).
 */
static void playlist_scroll(ePlayer *player, int direction) {
	int i, val;

	for (i = 0; i < 2; i++) {
		/* it's * 3 because we're scrolling 3 elements at once */
		val = player->gui.playlist_font_size[i] * 3 * direction;
		e_container_scroll(player->gui.playlist_col[i], val);
	}
}

void cb_playlist_scroll_up(void *udata, Evas_Object *obj,
                           const char *emission, const char *src) {
	playlist_scroll(udata, 1);	
}

void cb_playlist_scroll_down(void *udata, Evas_Object *obj,
                             const char *emission, const char *src) {
	playlist_scroll(udata, -1);
}

void cb_seek_forward(void *udata, Evas_Object *obj,
                     const char *emission, const char *src) {
	ePlayer *player = udata;
	PlayListItem *pli = player->playlist->cur_item->data;

	debug(DEBUG_LEVEL_INFO, "Seeking forward\n");

	/* We don't care if you seek past the file, the play loop
	 * will catch EOF and play next file
	 */
	eplayer_playback_stop(player);
	pli->plugin->set_current_pos(pli->plugin->get_current_pos() + 5);
	eplayer_playback_start(player, 0);
	paused = 0;
}

void cb_seek_backward(void *udata, Evas_Object *obj,
                      const char *emission, const char *src) {
	ePlayer *player = udata;
	PlayListItem *pli = player->playlist->cur_item->data;
	int cur_time = pli->plugin->get_current_pos();
	
	debug(DEBUG_LEVEL_INFO, "Seeking backward - Current Pos: %i\n",
	      cur_time);

	eplayer_playback_stop(player);

	if (cur_time < 6) /* restart from the beginning */
		eplayer_playback_start(player, 1);
	else {
		pli->plugin->set_current_pos(cur_time - 5);
		eplayer_playback_start(player, 0);
	}
	
	paused = 0;
}
