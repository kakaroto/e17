#include <Edje.h>
#include "eplayer.h"
#include "mixer.h"
#include "vorbis.h"

static int paused = 1;

void unpause_playback(ePlayer *data, Evas *e, Evas_Object *obj, void *event_info) {
	/* This ensures we don't call this callback multiple times */
	if (!paused)
		return;

#ifdef DEBUG
	printf("Unpause callback entered\n");
#endif
	
	paused = 0;

	data->play_idler = ecore_idler_add(play_loop, data); /* Start the play idler */
}

void pause_playback(ePlayer *player, Evas *e, Evas_Object *obj,
                    void *event_info) {
	if (paused)
		return;
	
#ifdef DEBUG
	printf("Pause callback entered\n");
#endif
	
	paused = 1;

	/* Stop the current playing stream */
	if (player->play_idler) {
		ecore_idler_del(player->play_idler);
		player->play_idler = NULL;
	}
}

void next_file(ePlayer *player, Evas *e, Evas_Object *obj, void *event_info) {
#ifdef DEBUG
	printf("DEBUG: Next File Called\n");
#endif
	
	/* Stop the current playing stream */
	if (player->play_idler) {
		ecore_idler_del(player->play_idler);
		player->play_idler = NULL;
	}

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
	open_track(player);
    player->play_idler = ecore_idler_add(play_loop, player);
}

void prev_file(ePlayer *player, Evas *e, Evas_Object *obj,
               void *event_info) {
#ifdef DEBUG
	printf("DEBUG: Previous File Called\n");
#endif

	if (player->play_idler) {
		/* Stop the current playing stream */
		ecore_idler_del(player->play_idler);
		player->play_idler = NULL;
	}

	/* Get the previous list item */
	if (!player->playlist->cur_item->prev)
		return;

	player->playlist->cur_item = player->playlist->cur_item->prev;

	/* Start the play loop */
	open_track (player);
	player->play_idler = ecore_idler_add(play_loop, player);
}

void raise_vol(ePlayer *player, Evas_Object *obj, const char *emission,
               const char *src) {
	int vol;

#ifdef DEBUG
	printf("DEBUG: Raising volume\n");
#endif

	vol = read_mixer(player);
	set_mixer(vol + 1);
	read_mixer(player);
}

void lower_vol(ePlayer *player, Evas_Object *obj, const char *emission,
               const char *src) {
	int vol;

#ifdef DEBUG
	printf("DEBUG: Lowering volume\n");
#endif
	
	vol = read_mixer(player);
	set_mixer(vol - 1);
	read_mixer(player);
}

void switch_time_display(ePlayer *player, Evas_Object *obj,
                         const char *emission, const char *src) {
	player->time_display = !player->time_display;
	update_time(player);
}

