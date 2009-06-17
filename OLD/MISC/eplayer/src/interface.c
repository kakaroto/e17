/*
 * $Id$
 */

#include <config.h>
#include <assert.h>
#include "eplayer.h"
#include <Esmart/Esmart_Container.h>
#include <Esmart/Esmart_Draggies.h>
#include <Edje.h>
#include <Ewl.h>
#include "callbacks.h"
#include "track.h"
#include "utils.h"
#include "interface.h"

typedef struct {
	char *name;
	char *src;
	EdjeCb func;
} EdjeCallback;

static void setup_playlist(ePlayer *player);
static void register_callbacks(ePlayer *player);

static int app_signal_exit(void *data, int type, void *event) {	
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
	Evas_Object *dragger = evas_object_name_find(evas, "dragger");
	int w = 0, h = 0;

	ecore_evas_geometry_get(ee, NULL, NULL, &w, &h);
	evas_object_resize(edje, (Evas_Coord) w, (Evas_Coord) h);
	evas_object_resize(dragger, (Evas_Coord) w, (Evas_Coord) h);
}

static void cb_dragger_mouse_up(void *data, Evas *evas, Evas_Object *o,
                                void *ev) {
	ePlayer *player = data;

	ecore_evas_raise(player->gui.ee);
}

static bool ui_init_dragger(ePlayer *player) {
	Evas_Object *dragger;

	if (!(dragger = esmart_draggies_new(player->gui.ee)))
		return false;

	esmart_draggies_button_set(dragger, 1);

	evas_object_name_set(dragger, "dragger");
	evas_object_move(dragger, 0, 0);
	evas_object_layer_set(dragger, 0);
	evas_object_show(dragger);

	esmart_draggies_event_callback_add(dragger, EVAS_CALLBACK_MOUSE_UP,
	                                   cb_dragger_mouse_up, player);

	return true;
}

bool ui_init(ePlayer *player) {
	int zero = 0;
	char buf[PATH_MAX];
	
	debug(DEBUG_LEVEL_INFO, "Starting setup\n");
	
	ecore_init();
	ecore_evas_init();
	edje_init();
	ewl_init(&zero, NULL);

	ecore_event_handler_add(ECORE_EVENT_SIGNAL_EXIT, app_signal_exit,
	                        NULL);

#ifdef HAVE_ECORE_EVAS_GL
	if (!strcasecmp(player->cfg.evas_engine, "gl")) {
		debug(DEBUG_LEVEL_INFO, "Starting EVAS GL X11\n");
		player->gui.ee = ecore_evas_gl_x11_new(NULL, 0, 0, 0, 0, 0);
	} else
#endif

#ifdef HAVE_ECORE_EVAS_FB
	if (!strcasecmp(player->cfg.evas_engine, "fb")) {
		debug(DEBUG_LEVEL_INFO, "Starting EVAS FB\n");
		player->gui.ee = ecore_evas_fb_new(NULL, 0, 0, 0);
	} else
#endif
	
	{
		debug(DEBUG_LEVEL_INFO, "Starting EVAS X11\n");
		player->gui.ee = ecore_evas_software_x11_new(NULL, 0, 0, 0, 0, 0);
	}
			
	if (!player->gui.ee) {
		debug(DEBUG_LEVEL_CRITICAL,
		      "Cannot create Ecore Evas (using %s engine)\n",
		      player->cfg.evas_engine);

		return false;
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

	snprintf(buf, sizeof(buf), "%s/.e/apps/%s/fonts",
	         getenv("HOME"), PACKAGE);

	evas_font_path_append(player->gui.evas, buf);
	evas_font_path_append(player->gui.evas, DATA_DIR "/fonts");
	evas_font_path_append(player->gui.evas, "/usr/X11R6/lib/X11/fonts");
	evas_font_path_append(player->gui.evas, "/usr/share/fonts");

	if (!ui_init_dragger(player))
		return false;

	return ui_init_edje(player, "eplayer");
}

void ui_shutdown_edje(ePlayer *player) {
	if (player->gui.playlist) {
		edje_object_part_unswallow(player->gui.edje,
		                           player->gui.playlist);
		evas_object_del(player->gui.playlist);
		player->gui.playlist = NULL;
	}

	if (player->gui.edje) {
		evas_object_del(player->gui.edje);
		player->gui.edje = NULL;
	}
}

void ui_shutdown(ePlayer *player) {
	assert(player);

	ui_shutdown_edje(player);

	ewl_shutdown();
	edje_shutdown();
	ecore_evas_shutdown();
	ecore_shutdown();
}

void ui_fill_track_info(ePlayer *player) {
	PlayListItem *pli;
	char buf[128];

	if (!(pli = playlist_current_item_get(player->playlist)))
		return;

	track_update_time(player);

	/* If there is no Title Name, use the file name */
	/* The other fields can stay blank. */
	if(strcmp(pli->comment[COMMENT_ID_TITLE], "")) {
          edje_object_part_text_set(player->gui.edje, "song_name",
                                  pli->comment[COMMENT_ID_TITLE]);
	} else {
          edje_object_part_text_set(player->gui.edje, "song_name",
                                  pli->file);
	}

	edje_object_part_text_set(player->gui.edje, "artist_name",
	                          pli->comment[COMMENT_ID_ARTIST]);
	edje_object_part_text_set(player->gui.edje, "album_name",
	                          pli->comment[COMMENT_ID_ALBUM]);

	/* sample rate */
	snprintf(buf, sizeof(buf), "%.1f",
	         (float) pli->sample_rate / 1000);
	edje_object_part_text_set(player->gui.edje, "track_sample_rate",
	                          buf);

	/* bitrate */
	snprintf(buf, sizeof(buf), "%i", pli->bitrate / 1000);
	edje_object_part_text_set(player->gui.edje, "track_bitrate",
	                          buf);
}

bool ui_init_edje(ePlayer *player, const char *name) {
	double edje_w = 0, edje_h = 0;

	debug(DEBUG_LEVEL_INFO, "EDJE: Defining Edje \n");

	if (!(player->gui.edje = edje_object_add(player->gui.evas))) {
		debug(DEBUG_LEVEL_CRITICAL, "Cannot create Edje!\n");
		return false;
	}

	evas_object_name_set(player->gui.edje, "main_edje");

	if (!edje_object_file_set(player->gui.edje,
	                          find_theme(player->cfg.theme),
	                          name)) {
		debug(DEBUG_LEVEL_CRITICAL, "Cannot load theme '%s'!\n",
		      player->cfg.theme);
		return false;
	}
	
	evas_object_move(player->gui.edje, 0, 0);
	evas_object_pass_events_set (player->gui.edje, 1);
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
	ui_refresh_volume(player);

	register_callbacks(player);

	return true;
}

static void register_callbacks(ePlayer *player) {
	Evas_Object *e = player->gui.edje;
	int i;
	EdjeCallback cb[] = {
		{"QUIT", "*",
		 (EdjeCb) cb_eplayer_quit},
		{"PLAY_PREVIOUS", "*",
		 (EdjeCb) cb_track_prev},
		{"PLAY_NEXT", "*",
		 (EdjeCb) cb_track_next},
		{"SEEK_BACK", "*",
		 (EdjeCb) cb_seek_backward},
		{"SEEK_FORWARD", "*",
		 (EdjeCb) cb_seek_forward},
		{"SEEK_BACK_START", "*",
		 (EdjeCb) cb_seek_backward_start},
		{"SEEK_BACK_STOP", "*",
		 (EdjeCb) cb_seek_backward_stop},
		{"SEEK_FORWARD_START", "*",
		 (EdjeCb) cb_seek_forward_start},
		{"SEEK_FORWARD_STOP", "*",
		 (EdjeCb) cb_seek_forward_stop},
		{"PLAY", "*",
		 (EdjeCb) cb_play},
		{"PAUSE", "*",
		 (EdjeCb) cb_pause},
		{"STOP", "*",
		 (EdjeCb) cb_stop},
		{"VOL_INCR", "*",
		 (EdjeCb) cb_volume_raise},
		{"VOL_DECR", "*",
		 (EdjeCb) cb_volume_lower},
		{"TOGGLE_TIME_DISPLAY_MODE", "*",
		 (EdjeCb) cb_time_display_toggle},
		{"TOGGLE_REPEAT_MODE", "*",
		 (EdjeCb) cb_repeat_mode_toggle},
		{"SWITCH_GROUP", "*",
		 (EdjeCb) cb_switch_group},
		{"SEEKER*", "*",
		 (EdjeCb) cb_update_seeker},
		{"PLAYLIST_ADD", "*",
		 (EdjeCb) cb_playlist_add},
		{"PLAYLIST_DEL", "*",
		 (EdjeCb) cb_playlist_del}};

	for (i = 0; i < sizeof (cb) / sizeof (EdjeCallback); i++)
		edje_object_signal_callback_add(e, cb[i].name, cb[i].src,
		                                cb[i].func, player);

	/* Setup Key Binds */
	evas_object_focus_set(e, 1);
	evas_object_event_callback_add(e, EVAS_CALLBACK_KEY_DOWN, cb_key_press, player);	
	/* This is for a later time and place, see notes with cb_key_release() */
	//evas_object_event_callback_add(e, EVAS_CALLBACK_KEY_UP, cb_key_release, player);	
	
}

/**
 * Add the playlist container.
 *
 * @param player
 */
static void setup_playlist(ePlayer *player) {
	if (!edje_object_part_exists(player->gui.edje, "playlist"))
		return;

	player->gui.playlist = esmart_container_new(player->gui.evas);
	assert(player->gui.playlist);

	evas_object_name_set(player->gui.playlist, "PlayList");
	evas_object_data_set(player->gui.playlist, "ePlayer", player);

	esmart_container_direction_set(player->gui.playlist, 1);
	esmart_container_spacing_set(player->gui.playlist, 0);
	esmart_container_fill_policy_set(player->gui.playlist,
	                            CONTAINER_FILL_POLICY_FILL_X);
	
	edje_object_part_swallow(player->gui.edje, "playlist",
	                         player->gui.playlist);
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

bool ui_refresh_time(ePlayer *player, int time) {
	char buf[9], *fmt[2];
	
	fmt[TIME_DISPLAY_ELAPSED] = "%i:%02i";
	fmt[TIME_DISPLAY_LEFT] = "-%i:%02i";

	snprintf(buf, sizeof(buf), fmt[player->cfg.time_display],
	         (time / 60), (time % 60));
	
	edje_object_part_text_set(player->gui.edje, "time_text", buf);
	evas_render(player->gui.evas);

	return true;
}

bool ui_refresh_seeker(ePlayer *player, double song_pos) {
	edje_object_part_drag_value_set(player->gui.edje, "seeker",
	                                song_pos, song_pos);
	edje_object_thaw(player->gui.edje);					
	evas_render(player->gui.evas);

	return true;
}

