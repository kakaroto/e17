#include "eplayer.h"

	int     	paused = 0;





void unpause_playback(player_session *data, Evas *e, Evas_Object *obj, void *event_info){

        /* This ensures we don't call this callback multiple times */
        if (paused == 0)
                return;

        printf("Unpause callback entered\n");
        paused = 0;

        data->play_idler = ecore_idler_add(play_loop, data);	/* Start the play idler */

}



void pause_playback(player_session *data, Evas *e, Evas_Object *obj, void *event_info){

        if (paused == 1)
                return;

        printf("Pause callback entered\n");
        paused = 1;

        ecore_idler_del(data->play_idler);	/* Stop the play idler */

}


void next_file(player_session *data, Evas *e, Evas_Object *obj, void *event_info){
	char		current_file[1000];

	printf("DEBUG: Next File Called\n");        /* Report what we're doing for debugging purposes */
	
	ecore_idler_del(data->play_idler);	/* Stop the current playing stream */

        data->play_list = evas_list_next(data->play_list);      /* Get the next list item */
        printf("DEBUG: Next file to play is: %s\n", evas_list_data(data->play_list));
	printf("DEBUG: Pointer addr is: %d\n", (int)data->play_list);

	if(data->play_list == NULL){
		printf("\n\nDEBUG: Youve hit the end of the list!!! \n\n");

		edje_object_part_text_set(data->edje, "artist_name", "*****************************");
                edje_object_part_text_set(data->edje, "album_name", " END OF THE ROAD ");
                edje_object_part_text_set(data->edje, "song_name", "*****************************");
                edje_object_part_text_set(data->edje, "time_text", "DAS:EN:DE");
		
		/* Since we hit the end, start from the beginning. */
		printf("DEBUG: Reseting playlist.  Currently NULL playlist pointer %d, reset pointer %d\n", 
			(int)data->play_list, (int)data->full_list);
		data->play_list = data->full_list;

		return 0;
	} 

	printf("DEBUG: In next-file, proccessing new file\n");
        if(file_is_ogg(evas_list_data(data->play_list))){	/* Make sure that the new item is really ogg */
                setup_ao();					/* If so, seutp the audio out path */
                get_vorbis(evas_list_data(data->play_list), data);	/* Setup the intrface with comments, etc */
                ao_open();					/* Open the outbound audio path */
                data->play_idler = ecore_idler_add(play_loop, data);	 /* Start the play loop */
        } else {
                printf("File %s is not an OggVorbis file\n", evas_list_data(data->play_list));	/* Or, report an error */
		/* Freek out the display */
		edje_object_part_text_set(data->edje, "artist_name", "*****************************");
		edje_object_part_text_set(data->edje, "album_name", " ERROR: NOT OGG VORBIS ");
		edje_object_part_text_set(data->edje, "song_name", "*****************************");
		edje_object_part_text_set(data->edje, "time_text", "FU:CK");
		//edje_object_signal_emit(data->edje, "PLAY_NEXT", "next_button"); /* Try next file */
	}

}


void prev_file(player_session *data, Evas *e, Evas_Object *obj, void *event_info){

	printf("DEBUG: Previous File Called\n");	/* Report what we're doing for debugging purposes */

        ecore_idler_del(data->play_idler);	/* Stop the current playing stream */

        data->play_list = evas_list_prev(data->play_list);	/* Get the previous list item */

        if(file_is_ogg(evas_list_data(data->play_list))){	/* Make sure that the new item is really ogg */
                setup_ao();					/* If so, seutp the audio out path */
                get_vorbis(evas_list_data(data->play_list), data);	/* Setup the intrface with comments, etc */
                ao_open();					/* Open the outbound audio path */
                data->play_idler = ecore_idler_add(play_loop, data);	/* Start the play loop */
        } else {
                printf("File %s is not an OggVorbis file\n", evas_list_data(data->play_list)); /* Or, report an error */
        }


}

void raise_vol(player_session *data, Evas *e, Evas_Object *obj, void *event_info){
	int vol;

	vol = read_mixer(data);
	set_mixer(vol + 1);
	read_mixer(data);

}


void lower_vol(player_session *data, Evas *e, Evas_Object *obj, void *event_info){
        int vol;

        vol = read_mixer(data);
        set_mixer(vol - 1);
        read_mixer(data);

}

