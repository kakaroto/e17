#include <config.h>
#include <Edje.h>
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

void cb_track_next(ePlayer *player, Evas *e, Evas_Object *obj,
                   void *event_info) {
#ifdef DEBUG
	printf("DEBUG: Next File Called\n");
#endif

	eplayer_playback_stop(player, 0);

	/* Get the next list item */
	player->playlist->cur_item = player->playlist->cur_item->next;
	
	if (!player->playlist->cur_item) {
#ifdef DEBUG
		printf("\n\nDEBUG: Youve hit the end of the list!!! \n\n");
#endif

		edje_object_part_text_set(player->gui.edje, "artist_name", "*****************************");
		edje_object_part_text_set(player->gui.edje, "album_name", " END OF THE ROAD ");
		edje_object_part_text_set(player->gui.edje, "song_name", "*****************************");
		edje_object_part_text_set(player->gui.edje, "time_text", "DAS:EN:DE");
		
		/* Since we hit the end, start from the beginning. */
		player->playlist->cur_item = player->playlist->items;

		return;
	} 
	
	/* Start the play loop */
	eplayer_playback_start(player, 1);
}

void cb_track_prev(ePlayer *player, Evas *e, Evas_Object *obj,
                   void *event_info) {
#ifdef DEBUG
	printf("DEBUG: Previous File Called\n");
#endif

	eplayer_playback_stop(player, 0);

	/* Get the previous list item */
	if (!player->playlist->cur_item->prev)
		return;

	player->playlist->cur_item = player->playlist->cur_item->prev;

	/* Start the play loop */
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

