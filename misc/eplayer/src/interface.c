#include "eplayer.h"
#include <Esmart/container.h>

/* ECORE/EVAS */
Ecore_Evas *ee;

static int app_signal_exit(void *data, int type, void *event) {
	printf("DEBUG: Exit called, shutting down\n");
	ecore_main_loop_quit();
	return 1;
}

static void window_move(Ecore_Evas *ee) {
	printf("DEBUG: Window Move callback entered.\n");
}

void setup_ecore(player_session *st_session) {
	double edje_w = 0, edje_h = 0;

	printf("DEBUG: Starting setup\n");

	ecore_init();
	ecore_evas_init();
	ecore_event_handler_add(ECORE_EVENT_SIGNAL_EXIT, app_signal_exit, NULL);

	ee = ecore_evas_software_x11_new(NULL, 0,  0, 0, WIDTH, HEIGHT);
	ecore_evas_title_set(ee, "eVorbisPlayer");
	ecore_evas_name_class_set(ee, "ecore_test", "test_evas");
	ecore_evas_borderless_set(ee, 0);
	ecore_evas_shaped_set(ee, 1);
	ecore_evas_show(ee);

	ecore_evas_callback_move_set(ee, window_move);

	st_session->evas = ecore_evas_get(ee);
	evas_font_path_append(st_session->evas, "../data/");
	evas_font_path_append(st_session->evas, "../data/fonts/");

	/* EDJE */
	printf("DEBUG - EDJE: Defining Edje \n");

	st_session->edje = edje_object_add(st_session->evas);
	edje_object_file_set(st_session->edje, "../data/eplayer.eet", "eplayer");
	evas_object_move(st_session->edje, 0, 0);
	edje_object_size_min_get(st_session->edje, &edje_w, &edje_h);
	evas_object_resize(st_session->edje, edje_w, edje_h);
	evas_object_show(st_session->edje);

	ecore_evas_resize(ee, (int)edje_w, (int)edje_h);
	ecore_evas_show(ee);

	/* add the playlist container */
	st_session->playlist_obj = e_container_new(st_session->evas);
	e_container_direction_set(st_session->playlist_obj, 0);
	e_container_spacing_set(st_session->playlist_obj, 0);
	e_container_fill_policy_set(st_session->playlist_obj,
	                            CONTAINER_FILL_POLICY_FILL_Y);
	
	edje_object_part_swallow(st_session->edje, "playlist", st_session->playlist_obj);

	/*** Edje Callbacks ***************************/
	edje_object_signal_callback_add(st_session->edje, "PLAY_PREVIOUS",
	                                "previous_button", prev_file,
	                                st_session);
	edje_object_signal_callback_add(st_session->edje, "PLAY_NEXT", "next_button", next_file, st_session);

	edje_object_signal_callback_add(st_session->edje, "SEEK_BACK", "seekback_button", seek_backward, st_session);
	edje_object_signal_callback_add(st_session->edje, "SEEK_FORWARD", "seekforward_button", seek_forward, st_session);

	edje_object_signal_callback_add(st_session->edje, "PLAY", "play_button", unpause_playback, st_session);
	edje_object_signal_callback_add(st_session->edje, "PAUSE", "pause_button", pause_playback, st_session);

	edje_object_signal_callback_add(st_session->edje, "VOL_INCR", "vol_incr_button", raise_vol, st_session);
	edje_object_signal_callback_add(st_session->edje, "VOL_DECR", "vol_decr_button", lower_vol, st_session);

	edje_object_signal_callback_add (st_session->edje, "SWITCH_TIME_DISPLAY", "time_text", switch_time_display, st_session);

	printf("DEBUG: Done with Ecore Setup\n");

	/* Update interface with current PCM Volume Setting */
	read_mixer(st_session);	
}

Evas_Object *playlist_column_add(player_session *session,
                                 double width, double height,
                                 Container_Alignment align) {
	Evas_Object *o = e_container_new(session->evas);
	e_container_direction_set(o, 1);
	e_container_spacing_set(o, 0);
	e_container_alignment_set(o, align);
	e_container_fill_policy_set(o, CONTAINER_FILL_POLICY_FILL_X);

	evas_object_resize(o, width, height);

	return o;
}

void show_playlist(player_session *session) {
	Evas_Object *o, *col[2];
	Evas_List *l;
	char *title, len[32];
	char *name[] = {"playlist_item_title", "playlist_item_length"};
	double w = 0, h = 0;
	int i, added_cols = 0, duration;

	for (l = session->full_list; l; l = l->next) {
		/* get the information we want to display */
		title = ((PlayListItem *) l->data)->title;
		duration = ((PlayListItem *) l->data)->duration;
		
		snprintf(len, sizeof (len), "%i:%02i", (duration / 60),
		         duration % 60);
		
		/* add the title/length items to the container */
		for (i = 0; i < 2; i++) {
			o = edje_object_add(session->evas);
			edje_object_file_set(o, "../data/eplayer.eet", name[i]);
			edje_object_part_text_set(o, "text", i ? len : title);
			edje_object_size_min_get(o, &w, &h);
			evas_object_resize(o, w, h);

			/* add the columns if we haven't yet
			 * we do this at this point, because we need to instantiate
			 * the edje object first, to get the width/height
			 */
			if (!added_cols) {
				evas_object_geometry_get(session->playlist_obj, NULL, NULL, NULL, &h);
				col[i] = playlist_column_add(session, w, h,
				                             i ? CONTAINER_ALIGN_RIGHT
				                             : CONTAINER_ALIGN_LEFT);
				e_container_element_append(session->playlist_obj, col[i]);
			}

			e_container_element_append(col[i], o);
		}

		added_cols = 1;
	}
}

