/* Eplayer OggVorbis Player - Phase 3 - Started 5/6/03 */
/* Edje Overhaul startnig phase 4 - Started 7/30/03 */

#include "eplayer.h"


int main(int argc, const char **argv ) {
	int args;
	int arg_count;

	player_session * st_session;
 	st_session = malloc(sizeof(player_session)); 
	memset(st_session, 0, sizeof(player_session)); 

  if( argc == 1){
        printf("eVorbisPlayer v0.7  - Usage: %s [file1.ogg] [file2.ogg] ...\n\n", argv[0]);
	exit(0);
  }


	/* Parse Args */
	for(args=1; args < argc ; args++){
		printf("Adding file to playlist: %s\n", argv[args]);
		
		st_session->play_list = evas_list_append(st_session->play_list, argv[args]);

	}

	arg_count = evas_list_count(st_session->play_list);
	st_session->full_list = st_session->play_list;
	printf("DEBUG: Int val of playlist pointers is: %d, and fulllist pointer is: %d\n\n", 
		(int)st_session->play_list, (int)st_session->full_list);
	printf("DEBUG: The list is ready with %d elements in it\n", arg_count);

	{	/* HACKISH HACKISH!!! */
		Evas_List       * temp_list;
		temp_list = evas_list_last(st_session->play_list);
		sprintf(st_session->last_file, "%s", evas_list_data(temp_list));
		printf("DEBUG: Last file in playlist is: %s\n", st_session->last_file);
		evas_list_free(temp_list);
	}

        printf("Going to play %s\n", evas_list_data(st_session->play_list));
	//printf("PCM Volume Level is: %d\n", read_mixer(NULL));
	/* Done with args */


	setup_ecore(st_session);

	printf("Going to play %s\n", evas_list_data(st_session->play_list));

	if(file_is_ogg(evas_list_data(st_session->play_list))){
	        setup_ao();
	        get_vorbis(evas_list_data(st_session->play_list), st_session);
       		ao_open();
		st_session->play_idler = ecore_idler_add(play_loop, st_session);
	} else {
		printf("File %s is not an OggVorbis file\n", evas_list_data(st_session->play_list));
	}


	printf("DEBUG: Starting main loop\n");
	ecore_main_loop_begin();

        ecore_evas_shutdown();
        ecore_shutdown();
	return 0;
}

