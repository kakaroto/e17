/* Eplayer OggVorbis Player - Phase 3 - Started 5/6/03 */
/* Edje Overhaul startnig phase 4 - Started 7/30/03 */

#include <config.h>
#include <Ecore_Evas.h>
#include "eplayer.h"
#include "interface.h"
#include "vorbis.h"

static void eplayer_free(ePlayer *player) {
	if (!player)
		return;

	playlist_free(player->playlist);
	output_plugin_free(player->output);
	
	free(player);
}

static ePlayer *eplayer_new() {
	ePlayer *player;
	const char *plugin = PLUGIN_DIR"/output/libOSS.so";

	if (!(player = malloc(sizeof(ePlayer))))
		return NULL;

	memset(player, 0, sizeof(ePlayer));

	player->playlist = playlist_new();

	/* load the output plugin */
	player->output = output_plugin_new(plugin);

	if (!player->output) {
		fprintf(stderr, "Cannot load output plugin %s!\n", plugin);
		eplayer_free(player);
		return NULL;
	}

	return player;
}

/**
 * Stops playback.
 *
 * @param player
 * @param rewind_track
 */
void eplayer_playback_stop(ePlayer *player, int rewind_track) {
	if (!player)
		return;

	/* stop the playloop */
	if (player->play_idler) {
		ecore_idler_del(player->play_idler);
		player->play_idler = NULL;
	}

	/* stop the timer that updates the time part */
	if (player->time_timer) {
		ecore_timer_del(player->time_timer);
		player->time_timer = NULL;
	}

	if (rewind_track) {
		vorbis_close(player);
		vorbis_open(player);
	}
}

/**
 * Starts playback.
 *
 * @param player
 * @param rewind_track
 */
void eplayer_playback_start(ePlayer *player, int rewind_track) {
	if (rewind_track) {
		vorbis_close(player);
		vorbis_open(player);
	}

	/* start the playloop */
	player->play_idler = ecore_idler_add(vorbis_play_chunk, player);
	player->time_timer = ecore_timer_add(0.5, vorbis_update_time,
	                                     player);
}

int main(int argc, const char **argv) {
	ePlayer *player;
	int args;

	if (!(player = eplayer_new())) {
		fprintf(stderr, "Out of memory!\n");
		return 1;
	}

	if (argc == 1) {
		printf("%s v%s  - Usage: %s playlist.m3u [file.ogg] [some/dir] ...\n\n",
		       PACKAGE, VERSION, argv[0]);
		return 1;
	}
	
	/* Parse Args */
	for (args = 1; args < argc; args++) {
#ifdef DEBUG
		printf("Adding file to playlist: %s\n", argv[args]);
#endif
		
		playlist_load_any(player->playlist, argv[args], args > 1);
	}
	
	if (!setup_gui(player)) {
		eplayer_free(player);
		return 1;
	}
		
	show_playlist(player);

	refresh_volume(player);
	ecore_timer_add(1.5, refresh_volume, player);

	vorbis_open(player);
	refresh_time(player, 0);

#ifdef DEBUG
	printf("DEBUG: Starting main loop\n");
#endif

	ecore_main_loop_begin();

	eplayer_free(player);

	ecore_evas_shutdown();
	ecore_shutdown();
	
	return 0;
}

