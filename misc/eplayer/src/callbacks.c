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
	printf("DEBUG: Next File Called\n");        /* Report what we're doing for debugging purposes */
	
	ecore_idler_del(data->play_idler);	/* Stop the current playing stream */

        data->play_list = data->play_list->next;      /* Get the next list item */
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

		return;
	} 

    printf("DEBUG: Next file to play is: %s\n", (char *) data->play_list->data);
	printf("DEBUG: In next-file, proccessing new file\n");
	setup_ao();					/* If so, seutp the audio out path */
	get_vorbis (data, (PlayListItem *) data->play_list->data); /* Setup the intrface with comments, etc */
    ao_open();					/* Open the outbound audio path */
    data->play_idler = ecore_idler_add(play_loop, data);	 /* Start the play loop */
}


void prev_file(void *udata, Evas *e, Evas_Object *obj, void *event_info) {
	player_session *data = udata;
	printf("DEBUG: Previous File Called\n");	/* Report what we're doing for debugging purposes */

	ecore_idler_del(data->play_idler);	/* Stop the current playing stream */

	data->play_list = evas_list_prev(data->play_list);	/* Get the previous list item */

	setup_ao(); /* If so, seutp the audio out path */
	get_vorbis (data, (PlayListItem *) data->play_list->data); /* Setup the intrface with comments, etc */
	ao_open(); /* Open the outbound audio path */
	data->play_idler = ecore_idler_add(play_loop, data);	/* Start the play loop */
}

void raise_vol(void *udata, Evas_Object *obj, const char *emission, const char *src) {
	player_session *data = udata;
	int vol;

	vol = read_mixer(data);
	set_mixer(vol + 1);
	read_mixer(data);
}

void lower_vol(void *udata, Evas_Object *obj, const char *emission, const char *src) {
	player_session *data = udata;
	int vol;

	vol = read_mixer(data);
	set_mixer(vol - 1);
	read_mixer(data);
}

void switch_time_display (void *udata, Evas_Object *obj, const char *emission, const char *src) {
	player_session *session = udata;

	session->time_display = !session->time_display;
	update_time (session);
}
