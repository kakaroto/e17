#include <config.h>
#include "eplayer.h"
#include <Ecore_Evas.h>
#include <Esmart/container.h>
#include <Edje.h>
#include "callbacks.h"
#include "track.h"
#include "utils.h"

static int setup_edje(ePlayer *player, Ecore_Evas *ee);
static void setup_playlist(ePlayer *player);

static int app_signal_exit(void *data, int type, void *event) {
	debug(DEBUG_LEVEL_INFO, "Exit called, shutting down\n");
	
	ecore_main_loop_quit();
	return 1;
}

int setup_gui(ePlayer *player) {
	Ecore_Evas *ee;

	debug(DEBUG_LEVEL_INFO, "Starting setup\n");

	ecore_init();
	ecore_evas_init();
	ecore_event_handler_add(ECORE_EVENT_SIGNAL_EXIT, app_signal_exit, NULL);

	if (!strcasecmp(player->cfg.evas_engine, "gl"))
		ee = ecore_evas_gl_x11_new(NULL, 0,  0, 0, 500, 500);
	else
		ee = ecore_evas_software_x11_new(NULL, 0,  0, 0, 500, 500);

	if (!ee) {
		debug(DEBUG_LEVEL_CRITICAL,
		      "Cannot create Ecore Evas (using %s engine)\n",
		      player->cfg.evas_engine);

		return 0;
	}
	
	ecore_evas_title_set(ee, "eVorbisPlayer");
	ecore_evas_name_class_set(ee, "ecore_test", "test_evas");
	ecore_evas_borderless_set(ee, 1);
	ecore_evas_shaped_set(ee, 1);
	ecore_evas_show(ee);

	player->gui.evas = ecore_evas_get(ee);
	evas_font_path_append(player->gui.evas, DATA_DIR "/themes/fonts");

	if (!setup_edje(player, ee))
		return 0;

	setup_playlist(player);

	return 1;
}

static int setup_edje(ePlayer *player, Ecore_Evas *ee) {
	double edje_w = 0, edje_h = 0;

	debug(DEBUG_LEVEL_INFO, "EDJE: Defining Edje \n");

	player->gui.edje = edje_object_add(player->gui.evas);
	
	if (!(edje_object_file_set(player->gui.edje,
	                     DATA_DIR "/themes/eplayer.eet", "eplayer"))) {
		debug(DEBUG_LEVEL_CRITICAL, "Cannot load theme!\n");
		return 0;
	}
	
	evas_object_move(player->gui.edje, 0, 0);
	edje_object_size_min_get(player->gui.edje, &edje_w, &edje_h);
	evas_object_resize(player->gui.edje, edje_w, edje_h);
	evas_object_show(player->gui.edje);

	ecore_evas_resize(ee, (int) edje_w, (int) edje_h);
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
	                                cb_track_prev, player);
	edje_object_signal_callback_add(player->gui.edje,
	                                "PLAY_NEXT", "next_button",
	                                cb_track_next, player);
	edje_object_signal_callback_add(player->gui.edje,
	                                "SEEK_BACK", "seekback_button",
	                                cb_seek_backward, player);
	edje_object_signal_callback_add(player->gui.edje,
	                                "SEEK_FORWARD", "seekforward_button",
	                                cb_seek_forward, player);
	edje_object_signal_callback_add(player->gui.edje,
	                                "PLAY", "play_button",
	                                cb_play, player);
	edje_object_signal_callback_add(player->gui.edje,
	                                "PAUSE", "pause_button",
	                                cb_pause, player);
	
	edje_object_signal_callback_add(player->gui.edje,
	                                "VOL_INCR", "vol_incr_button",
	                                cb_volume_raise, player);
	edje_object_signal_callback_add(player->gui.edje,
	                                "VOL_DECR", "vol_decr_button",
	                                cb_volume_lower, player);

	edje_object_signal_callback_add(player->gui.edje,
	                                "SWITCH_TIME_DISPLAY", "time_text",
	                                cb_time_display_toggle, player);

	return 1;
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

static void setup_playlist(ePlayer *player) {
	Evas_Object *o;
	double w, h;
	int i;
	
	for (i = 0; i < 2; i++) {
		/* instantiate the edje object first, to get the width/height */
		o = edje_object_add(player->gui.evas);

		edje_object_file_set(o, DATA_DIR "/themes/eplayer.eet",
		                     "playlist_item_title");
		
		w = h = 0;
		edje_object_size_min_get(o, &w, &h);
		evas_object_del(o);

		/* now add the columns */
		evas_object_geometry_get(player->gui.playlist,
		                         NULL, NULL, NULL, &h);

	
		player->gui.playlist_col[i] = 
			playlist_column_add(player, w, h,
			                    i ? CONTAINER_ALIGN_RIGHT
			                    : CONTAINER_ALIGN_LEFT);

		e_container_element_append(player->gui.playlist,
		                           player->gui.playlist_col[i]);
	}
}

void show_playlist_item(PlayListItem *pli, void *data) {
	ePlayer *player = data;
	Evas_Object *o;
	char *title, len[32];
	char *name[] = {"playlist_item_title", "playlist_item_length"};
	double w, h;
	int i, duration;

	/* get the information we want to display */
	title = pli->comment[COMMENT_ID_TITLE];
	duration = pli->duration;

	snprintf(len, sizeof(len), "%i:%02i", duration / 60,
	         duration % 60);
		
	/* add the title/length items to the container */
	for (i = 0; i < 2; i++) {
		o = edje_object_add(player->gui.evas);

		edje_object_file_set(o, DATA_DIR "/themes/eplayer.eet",
		                     name[i]);
			
		edje_object_part_text_set(o, "text", i ? len : title);

		w = h = 0;
		edje_object_size_min_get(o, &w, &h);
		evas_object_resize(o, w, h);

		e_container_element_append(player->gui.playlist_col[i], o);
		
		/* add playlist callbacks */
		edje_object_signal_callback_add(o,
		                                "PLAYLIST_SCROLL_UP", "text",
		                                cb_playlist_scroll_up,
		                                player);
		edje_object_signal_callback_add(o,
		                                "PLAYLIST_SCROLL_DOWN", "text",
		                                cb_playlist_scroll_down,
		                                player);

		/* FIXME: we're assuming that the objects minimal height
		 * equals the text size
		 */
		player->gui.playlist_font_size[i] = h;
	}
	
	edje_object_signal_callback_add(player->gui.edje,
	                                "PLAYLIST_SCROLL_DOWN", "playlist",
	                                cb_playlist_scroll_down, player);
	edje_object_signal_callback_add(player->gui.edje,
	                                "PLAYLIST_SCROLL_UP", "playlist",
	                                cb_playlist_scroll_up, player);
}

int refresh_volume(void *udata) {
	ePlayer *player = udata;
	char buf[8];
	int left = 0, right = 0;

	if (!player->output->volume_get(&left, &right))
		return 1;
	
	snprintf(buf, sizeof(buf), "%i", (left + right) / 2);
	edje_object_part_text_set(player->gui.edje, "vol_display_text", buf);
	
	return 1;
}

int refresh_time(ePlayer *player, int time) {
	char buf[9], *fmt[2];
	
	fmt[TIME_DISPLAY_ELAPSED] = "%i:%02i";
	fmt[TIME_DISPLAY_LEFT] = "-%i:%02i";

	snprintf(buf, sizeof(buf), fmt[player->cfg.time_display],
	         (time / 60), (time % 60));
	
	edje_object_part_text_set(player->gui.edje, "time_text", buf);
	evas_render(player->gui.evas);

	return 1;
}
