#include "eplayer.h"
#include <Ecore_Evas.h>
#include <Esmart/container.h>
#include <Edje.h>
#include "callbacks.h"
#include "mixer.h"
#include "vorbis.h"

/* ECORE/EVAS */
static Ecore_Evas *ee = NULL;

static int app_signal_exit(void *data, int type, void *event) {
#ifdef DEBUG
	printf("DEBUG: Exit called, shutting down\n");
#endif
	
	ecore_main_loop_quit();
	return 1;
}

static void window_move(Ecore_Evas *ee) {
#ifdef DEBUG
	printf("DEBUG: Window Move callback entered.\n");
#endif
}

void setup_ecore(ePlayer *player) {
	double edje_w = 0, edje_h = 0;

#ifdef DEBUG
	printf("DEBUG: Starting setup\n");
#endif

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

	player->gui.evas = ecore_evas_get(ee);
	evas_font_path_append(player->gui.evas, DATA_DIR"/themes/fonts");

	/* EDJE */
#ifdef DEBUG
	printf("DEBUG - EDJE: Defining Edje \n");
#endif

	player->gui.edje = edje_object_add(player->gui.evas);
	edje_object_file_set(player->gui.edje,
	                     DATA_DIR"/themes/eplayer.eet", "eplayer");
	evas_object_move(player->gui.edje, 0, 0);
	edje_object_size_min_get(player->gui.edje, &edje_w, &edje_h);
	evas_object_resize(player->gui.edje, edje_w, edje_h);
	evas_object_show(player->gui.edje);

	ecore_evas_resize(ee, (int)edje_w, (int)edje_h);
	ecore_evas_show(ee);

	/* add the playlist container */
	player->gui.playlist = e_container_new(player->gui.evas);
	e_container_direction_set(player->gui.playlist, 0);
	e_container_spacing_set(player->gui.playlist, 0);
	e_container_fill_policy_set(player->gui.playlist,
	                            CONTAINER_FILL_POLICY_FILL_Y);
	
	edje_object_part_swallow(player->gui.edje, "playlist",
	                         player->gui.playlist);

	/*** Edje Callbacks ***************************/
	edje_object_signal_callback_add(player->gui.edje,
	                                "PLAY_PREVIOUS", "previous_button",
	                                prev_file, player);
	edje_object_signal_callback_add(player->gui.edje,
	                                "PLAY_NEXT", "next_button",
	                                next_file, player);
	edje_object_signal_callback_add(player->gui.edje,
	                                "SEEK_BACK", "seekback_button",
	                                seek_backward, player);
	edje_object_signal_callback_add(player->gui.edje,
	                                "SEEK_FORWARD", "seekforward_button",
	                                seek_forward, player);
	edje_object_signal_callback_add(player->gui.edje,
	                                "PLAY", "play_button",
	                                unpause_playback, player);
	edje_object_signal_callback_add(player->gui.edje,
	                                "PAUSE", "pause_button",
	                                pause_playback, player);
	edje_object_signal_callback_add(player->gui.edje,
	                                "VOL_INCR", "vol_incr_button",
	                                raise_vol, player);
	edje_object_signal_callback_add(player->gui.edje,
	                                "VOL_DECR", "vol_decr_button",
	                                lower_vol, player);
	edje_object_signal_callback_add(player->gui.edje,
	                                "SWITCH_TIME_DISPLAY", "time_text",
	                                switch_time_display, player);

	/* Update interface with current PCM Volume Setting */
	read_mixer(player);	
}

static Evas_Object *playlist_column_add(ePlayer *player,
                                        double width, double height,
                                        Container_Alignment align) {
	Evas_Object *o = e_container_new(player->gui.evas);
	e_container_direction_set(o, 1);
	e_container_spacing_set(o, 0);
	e_container_alignment_set(o, align);
	e_container_fill_policy_set(o, CONTAINER_FILL_POLICY_FILL_X);

	evas_object_resize(o, width, height);

	return o;
}

void show_playlist(ePlayer *player) {
	Evas_Object *o, *col[2];
	Evas_List *l;
	char *title, len[32];
	char *name[] = {"playlist_item_title", "playlist_item_length"};
	double w = 0, h = 0;
	int i, added_cols = 0, duration;

	for (l = player->playlist->items; l; l = l->next) {
		/* get the information we want to display */
		title = ((PlayListItem *) l->data)->title;
		duration = ((PlayListItem *) l->data)->duration;
		
		snprintf(len, sizeof (len), "%i:%02i", (duration / 60),
		         duration % 60);
		
		/* add the title/length items to the container */
		for (i = 0; i < 2; i++) {
			o = edje_object_add(player->gui.evas);
			edje_object_file_set(o, DATA_DIR"/themes/eplayer.eet", name[i]);
			edje_object_part_text_set(o, "text", i ? len : title);
			edje_object_size_min_get(o, &w, &h);
			evas_object_resize(o, w, h);

			/* add the columns if we haven't yet
			 * we do this at this point, because we need to instantiate
			 * the edje object first, to get the width/height
			 */
			if (!added_cols) {
				evas_object_geometry_get(player->gui.playlist, NULL, NULL, NULL, &h);
				col[i] = playlist_column_add(player, w, h,
				                             i ? CONTAINER_ALIGN_RIGHT
				                             : CONTAINER_ALIGN_LEFT);
				e_container_element_append(player->gui.playlist, col[i]);
			}

			e_container_element_append(col[i], o);
		}

		added_cols = 1;
	}
}

