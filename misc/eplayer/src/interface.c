#include <config.h>
#include <assert.h>
#include "eplayer.h"
#include <Esmart/container.h>
#include <Edje.h>
#include "callbacks.h"
#include "track.h"
#include "utils.h"
#include "interface.h"

static void setup_playlist(ePlayer *player);

static int app_signal_exit(void *data, int type, void *event) {
	debug(DEBUG_LEVEL_INFO, "Exit called, shutting down\n");
	
	ecore_main_loop_quit();
	return 1;
}

static void cb_ee_pre_render(Ecore_Evas *ee) {
	edje_thaw();
}

static void cb_ee_post_render(Ecore_Evas *ee) {
	edje_freeze();
}

/**
 * Resizes the Edje to the size of our Ecore Evas
 *
 * @param ee
 */
static void cb_ee_resize(Ecore_Evas *ee) {
	Evas *evas = ecore_evas_get(ee);
	Evas_Object *edje = evas_object_name_find(evas, "main_edje");
	int w = 0, h = 0;

	ecore_evas_geometry_get(ee, NULL, NULL, &w, &h);
	evas_object_resize(edje, (Evas_Coord) w, (Evas_Coord) h);
}

int ui_init(ePlayer *player) {
	debug(DEBUG_LEVEL_INFO, "Starting setup\n");

	ecore_init();
	ecore_evas_init();
	ecore_event_handler_add(ECORE_EVENT_SIGNAL_EXIT, app_signal_exit,
	                        NULL);

	if (!strcasecmp(player->cfg.evas_engine, "gl"))
		player->gui.ee = ecore_evas_gl_x11_new(NULL, 0,  0, 0,
		                                       500, 500);
	else
		player->gui.ee = ecore_evas_software_x11_new(NULL, 0,  0, 0,
		                                             500, 500);

	if (!player->gui.ee) {
		debug(DEBUG_LEVEL_CRITICAL,
		      "Cannot create Ecore Evas (using %s engine)\n",
		      player->cfg.evas_engine);

		return 0;
	}
	
	ecore_evas_title_set(player->gui.ee, "ePlayer");
	ecore_evas_name_class_set(player->gui.ee, "ecore_test",
	                          "test_evas");
	ecore_evas_borderless_set(player->gui.ee, 1);
	ecore_evas_shaped_set(player->gui.ee, 1);

	ecore_evas_callback_pre_render_set(player->gui.ee,
	                                   cb_ee_pre_render);
	ecore_evas_callback_post_render_set(player->gui.ee,
	                                    cb_ee_post_render);
	ecore_evas_callback_resize_set(player->gui.ee, cb_ee_resize);

	ecore_evas_show(player->gui.ee);

	player->gui.evas = ecore_evas_get(player->gui.ee);
	evas_font_path_append(player->gui.evas, DATA_DIR "/fonts");

	if (!ui_init_edje(player, "eplayer"))
		return 0;

	return 1;
}

void ui_fill_track_info(ePlayer *player) {
	PlayListItem *pli;

	if (!(pli = playlist_current_item_get(player->playlist)))
		return;

	track_update_time(player);

	edje_object_part_text_set(player->gui.edje, "song_name",
	                          pli->comment[COMMENT_ID_TITLE]);
	edje_object_part_text_set(player->gui.edje, "artist_name",
	                          pli->comment[COMMENT_ID_ARTIST]);
	edje_object_part_text_set(player->gui.edje, "album_name",
	                          pli->comment[COMMENT_ID_ALBUM]);
}

int ui_init_edje(ePlayer *player, const char *name) {
	char eet[PATH_MAX + 1];
	double edje_w = 0, edje_h = 0;

	debug(DEBUG_LEVEL_INFO, "EDJE: Defining Edje \n");

	player->gui.edje = edje_object_add(player->gui.evas);
	evas_object_name_set(player->gui.edje, "main_edje");
	
	snprintf(eet, sizeof(eet), DATA_DIR "/themes/%s.eet",
	         player->cfg.theme);
	
	if (!edje_object_file_set(player->gui.edje, eet, name)) {
		debug(DEBUG_LEVEL_CRITICAL, "Cannot load theme '%s'!\n",
		      player->cfg.theme);
		return 0;
	}
	
	evas_object_move(player->gui.edje, 0, 0);
	evas_object_show(player->gui.edje);

	/* set max size */
	edje_object_size_max_get(player->gui.edje, &edje_w, &edje_h);
	ecore_evas_size_max_set(player->gui.ee, edje_w, edje_h);

	/* set min size */
	edje_object_size_min_get(player->gui.edje, &edje_w, &edje_h);
	ecore_evas_size_min_set(player->gui.ee, edje_w, edje_h);
	
	/* resize to the min size */
	ecore_evas_resize(player->gui.ee, (int) edje_w, (int) edje_h);
	
	setup_playlist(player);
	ui_fill_playlist(player);
	ui_fill_track_info(player);
	ui_refresh_volume(player);
	
	/*** Edje Callbacks ***************************/
	edje_object_signal_callback_add(player->gui.edje,
	                                "QUIT", "quit",
	                                cb_eplayer_quit, player);
	edje_object_signal_callback_add(player->gui.edje,
	                                "RAISE", "*",
	                                cb_eplayer_raise, player);
	
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
	                                "STOP", "stop_button",
	                                cb_stop, player);
	
	edje_object_signal_callback_add(player->gui.edje,
	                                "VOL_INCR", "vol_incr_button",
	                                cb_volume_raise, player);
	edje_object_signal_callback_add(player->gui.edje,
	                                "VOL_DECR", "vol_decr_button",
	                                cb_volume_lower, player);

	edje_object_signal_callback_add(player->gui.edje,
	                                "TOGGLE_TIME_DISPLAY_MODE", "time_text",
	                                cb_time_display_toggle, player);

	edje_object_signal_callback_add(player->gui.edje,
	                                "TOGGLE_REPEAT_MODE", "repeat_mode",
	                                cb_repeat_mode_toggle, player);

	edje_object_signal_callback_add(player->gui.edje,
	                                "SWITCH_GROUP", "*",
	                                cb_switch_group, player);

	return 1;
}

static void setup_playlist(ePlayer *player) {
	/* add the playlist container */
	player->gui.playlist = e_container_new(player->gui.evas);
	e_container_direction_set(player->gui.playlist, 1);
	e_container_spacing_set(player->gui.playlist, 0);
	e_container_fill_policy_set(player->gui.playlist,
	                            CONTAINER_FILL_POLICY_FILL_X);
	
	edje_object_part_swallow(player->gui.edje, "playlist",
	                         player->gui.playlist);
}

static void show_playlist_item(ePlayer *player, PlayListItem *pli) {
	Evas_Object *o;
	char len[32], eet[PATH_MAX + 1];
	double w = 0, h = 0;

	/* add the item to the container */
	o = edje_object_add(player->gui.evas);

	snprintf(eet, sizeof(eet), DATA_DIR "/themes/%s.eet",
	         player->cfg.theme);

	edje_object_file_set(o, eet, "playlist_item");

	/* set parts text */
	snprintf(len, sizeof(len), "%i:%02i", pli->duration / 60,
	         pli->duration % 60);
	edje_object_part_text_set(o, "length", len);
	edje_object_part_text_set(o, "title",
	                          pli->comment[COMMENT_ID_TITLE]);

	/* set parts dimensions */
	edje_object_size_min_get(o, &w, &h);
	evas_object_resize(o, w, h);
	
	evas_object_data_set(o, "PlayListItem", pli);

	/* store font size, we need it later for scrolling
	 * FIXME: we're assuming that the objects minimal height
	 * equals the text size
	 */
	player->gui.playlist_font_size = h;

	e_container_element_append(player->gui.playlist, o);
	
	/* add playlist item callbacks */
	edje_object_signal_callback_add(o, "PLAYLIST_SCROLL_UP", "",
			                        cb_playlist_scroll_up, player);
	edje_object_signal_callback_add(o, "PLAYLIST_SCROLL_DOWN", "",
			                        cb_playlist_scroll_down, player);
	edje_object_signal_callback_add(o, "PLAYLIST_ITEM_PLAY", "",
			                        cb_playlist_item_play, player);
	edje_object_signal_callback_add(o, "PLAYLIST_ITEM_SELECTED", "",
	                                cb_playlist_item_selected, player);
}

void ui_fill_playlist(ePlayer *player) {
	Evas_List *l;

	assert (player);
	assert (player->playlist);

	for (l = player->playlist->items; l; l = l->next)
		show_playlist_item(player, l->data);
}

int ui_refresh_volume(void *udata) {
	ePlayer *player = udata;
	char buf[8];
	int left = 0, right = 0;

	if (!player->output->volume_get(&left, &right))
		return 1;
	
	snprintf(buf, sizeof(buf), "%i", (left + right) / 2);
	edje_object_part_text_set(player->gui.edje, "vol_display_text", buf);
	
	return 1;
}

int ui_refresh_time(ePlayer *player, int time) {
	char buf[9], *fmt[2];
	
	fmt[TIME_DISPLAY_ELAPSED] = "%i:%02i";
	fmt[TIME_DISPLAY_LEFT] = "-%i:%02i";

	snprintf(buf, sizeof(buf), fmt[player->cfg.time_display],
	         (time / 60), (time % 60));
	
	edje_object_part_text_set(player->gui.edje, "time_text", buf);
	evas_render(player->gui.evas);

	return 1;
}
