/* Eplayer OggVorbis Player - Phase 3 - Started 5/6/03 */
/* Edje Overhaul startnig phase 4 - Started 7/30/03 */

#include <Ecore_Evas.h>
#include "eplayer.h"
#include "interface.h"
#include "vorbis.h"

int main(int argc, const char **argv) {
	ePlayer *player;
	int args;

	player = malloc(sizeof(ePlayer)); 
	memset(player, 0, sizeof(ePlayer)); 

	if (argc == 1) {
		printf("eVorbisPlayer v0.7  - Usage: %s playlist.m3u [file.ogg] [some/dir] ...\n\n", argv[0]);
		return 1;
	}
	
	player->playlist = playlist_new();

	/* Parse Args */
	for (args = 1; args < argc; args++) {
#ifdef DEBUG
		printf("Adding file to playlist: %s\n", argv[args]);
#endif
		
		playlist_load_any(player->playlist, argv[args], args > 1);
	}
	
	setup_ecore(player);
	show_playlist(player);

	/*ecore_timer_add (1.5, mixer_refresh, st_session);*/

	open_track (player);

#ifdef DEBUG
	printf("DEBUG: Starting main loop\n");
#endif

	ecore_main_loop_begin();

	playlist_free(player->playlist);
	free(player);

	ecore_evas_shutdown();
	ecore_shutdown();
	
	return 0;
}

