#include <config.h>
#include <Edje.h>
#include <Esmart/container.h>
#include "eplayer.h"
#include "mixer.h"
#include "vorbis.h"
#include "interface.h"

static int paused = 0;

void cb_play(ePlayer *player, Evas *e, Evas_Object *obj,
             void *event_info) {
#ifdef DEBUG
	printf("Play callback entered\n");
#endif

	if (!paused) /* restart from beginning */
		eplayer_playback_start(player, 1);
	else { /* continue playback */
		eplayer_playback_start(player, 0);
		paused = 0;
	}
}

void cb_pause(ePlayer *player, Evas *e, Evas_Object *obj,
              void *event_info) {
#ifdef DEBUG
	printf("Pause callback entered\n");
#endif
	
	if (paused)
		eplayer_playback_start(player, 0);
	else
		eplayer_playback_stop(player, 0);

	paused = !paused;
}

/**
 * Moves to the next track and plays it, except when we're going
 * back to the beginning of the playlist.
 *
 * @param player
 * @param e
 * @param obj
 * @param event_info
 */
void cb_track_next(ePlayer *player, Evas *e, Evas_Object *obj,
                   void *event_info) {
#ifdef DEBUG
	printf("DEBUG: Next File Called\n");
#endif

	eplayer_playback_stop(player, 0);

	if (player->playlist->cur_item->next) {
		player->playlist->cur_item = player->playlist->cur_item->next;
		eplayer_playback_start(player, 1);
	} else {
		/* there's no next item, so move to the beginning again
		 * but don't start playing yet.
		 */
		player->playlist->cur_item = player->playlist->items;
		track_open(player); /* refresh track info parts */
	}
}

void cb_track_prev(ePlayer *player, Evas *e, Evas_Object *obj,
                   void *event_info) {
#ifdef DEBUG
	printf("DEBUG: Previous File Called\n");
#endif

	/* first item on the list: do nothing */
	if (!player->playlist->cur_item->prev)
		return;

	eplayer_playback_stop(player, 0);
	
	/* Get the previous list item */
	player->playlist->cur_item = player->playlist->cur_item->prev;

	eplayer_playback_start(player, 1);
}

void cb_volume_raise(ePlayer *player, Evas_Object *obj,
                     const char *emission, const char *src) {
#ifdef DEBUG
	printf("DEBUG: Raising volume\n");
#endif

	mixer_change(player->mixer, 5);
	refresh_volume(player, 0);
}

void cb_volume_lower(ePlayer *player, Evas_Object *obj,
                     const char *emission, const char *src) {
#ifdef DEBUG
	printf("DEBUG: Lowering volume\n");
#endif
	
	mixer_change(player->mixer, -5);
	refresh_volume(player, 0);
}

void cb_time_display_toggle(ePlayer *player, Evas_Object *obj,
                            const char *emission, const char *src) {
	player->time_display = !player->time_display;
	update_time(player);
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
