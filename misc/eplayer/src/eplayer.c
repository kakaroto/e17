/* Eplayer OggVorbis Player - Phase 3 - Started 5/6/03 */
/* Edje Overhaul startnig phase 4 - Started 7/30/03 */

#include "eplayer.h"

int main(int argc, const char **argv) {
	player_session *st_session;
	PlayList *pl = NULL;
	int arg_count;

	st_session = malloc(sizeof(player_session)); 
	memset(st_session, 0, sizeof(player_session)); 

	if( argc == 1){
		printf("eVorbisPlayer v0.7  - Usage: %s playlist.m3u ...\n\n", argv[0]);
		exit(0);
	}

	/* FIXME: Parse Args, free playlist when shutting ecore down etc */
	/*for (args = 1; args < argc; args++) {
		printf("Adding file to playlist: %s\n", argv[args]);
		
		st_session->play_list = evas_list_append(st_session->play_list, argv[args]);
	}*/
	
	pl = playlist_new();

	if (!playlist_load_m3u(pl, argv[1], 0)) {
		printf ("no files loaded!\n");
		exit (0);
	}
	
	st_session->play_list = pl->items;
	
	arg_count = pl->num;
	st_session->full_list = st_session->play_list;
	printf("DEBUG: Int val of playlist pointers is: %d, and fulllist pointer is: %d\n\n", 
		(int)st_session->play_list, (int)st_session->full_list);
	printf("DEBUG: The list is ready with %d elements in it\n", arg_count);

	{	/* HACKISH HACKISH!!! */
		Evas_List       * temp_list;
		temp_list = evas_list_last(st_session->play_list);
		sprintf(st_session->last_file, "%s", ((PlayListItem *) temp_list->data)->file);
		printf("DEBUG: Last file in playlist is: %s\n", st_session->last_file);
	}

	printf("Going to play %s\n", ((PlayListItem *) st_session->play_list->data)->file);
	//printf("PCM Volume Level is: %d\n", read_mixer(NULL));
	
	/* Done with args */
	
	setup_ecore(st_session);
	show_playlist (st_session);

	/*ecore_timer_add (1.5, mixer_refresh, st_session);*/

	setup_ao();
	get_vorbis (st_session, (PlayListItem *) st_session->play_list->data);
	ao_open();
	st_session->play_idler = ecore_idler_add(play_loop, st_session);

	printf("DEBUG: Starting main loop\n");
	ecore_main_loop_begin();

	ecore_evas_shutdown();
	ecore_shutdown();
	
	return 0;
}

