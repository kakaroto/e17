#include "eplayer.h"

        /* ECORE/EVAS */
        Ecore_Evas  *   ee;

int app_signal_exit(void *data, int type, void *event){

        printf("DEBUG: Exit called, shutting down\n");
	ecore_main_loop_quit();
   	return 1;
}

void window_move(Ecore_Evas *ee){
	printf("DEBUG: Window Move callback entered.\n");
}


int setup_ecore(player_session *st_session){

	double   edje_w, edje_h;

        printf("DEBUG: Starting setup\n");

        ecore_init();
	ecore_evas_init();
        ecore_event_handler_add(ECORE_EVENT_SIGNAL_EXIT, app_signal_exit, NULL);

   ee = ecore_evas_software_x11_new(NULL, 0,  0, 0, WIDTH, HEIGHT);
        ecore_evas_title_set(ee, "eVorbisPlayer");
        ecore_evas_name_class_set(ee, "ecore_test", "test_evas");
        ecore_evas_borderless_set(ee, 1);
	ecore_evas_shaped_set(ee, 1);
        ecore_evas_show(ee);

	ecore_evas_callback_move_set(ee, window_move);

   st_session->evas = ecore_evas_get(ee);
        evas_font_path_append(st_session->evas, "../data/");
        evas_font_path_append(st_session->evas, "../data/fonts/");


	/* EDJE *****/
	printf("DEBUG - EDJE: Definining Edje \n");

   st_session->edje = edje_object_add(st_session->evas);
        edje_object_file_set(st_session->edje, "../data/eplayer.eet", "eplayer");
        evas_object_move(st_session->edje, 0, 0);
	edje_object_size_min_get(st_session->edje, &edje_w, &edje_h);
        evas_object_resize(st_session->edje, edje_w, edje_h);
        evas_object_show(st_session->edje);
	
	ecore_evas_resize(ee, (int)edje_w, (int)edje_h);
	ecore_evas_show(ee);

	/*** Edje Callbacks ***************************/
	edje_object_signal_callback_add(st_session->edje, "PLAY_PREVIOUS", "previous_button", prev_file, st_session);
        edje_object_signal_callback_add(st_session->edje, "PLAY_NEXT", "next_button", next_file, st_session);

	edje_object_signal_callback_add(st_session->edje, "SEEK_BACK", "seekback_button", seek_backward, st_session);
	edje_object_signal_callback_add(st_session->edje, "SEEK_FORWARD", "seekforward_button", seek_forward, st_session);

	edje_object_signal_callback_add(st_session->edje, "PLAY", "play_button", unpause_playback, st_session);
	edje_object_signal_callback_add(st_session->edje, "PAUSE", "pause_button", pause_playback, st_session);

        edje_object_signal_callback_add(st_session->edje, "VOL_INCR", "vol_incr_button", raise_vol, st_session);
        edje_object_signal_callback_add(st_session->edje, "VOL_DECR", "vol_decr_button", lower_vol, st_session);

        printf("DEBUG: Done with Ecore Setup\n");

	/* Update interface with current PCM Volume Setting */
	read_mixer(st_session);	

	/* Update interface with current play list */

/**********  Under construction
	{
		Evas_List * tmplist;
		int i;
		
		tmplist = st_session->full_list;
		
		for(i = 0; i < 7; i++){
			char    song[1000];
				song = evas_list_data(tmplist);
				//get_track_name();			
				//get_track_time();
		}
				
				
	}	
**************/

}

