/*
 * $Id$
 * vim:noexpandtab:sw=4:sts=4:ts=4
 */

#include <config.h>
#include <assert.h>
#include <Esmart/container.h>
#include <Esmart/dragable.h>
#include <Edje.h>
#include <Ewl.h>
#include "euphoria.h"
#include "callbacks.h"
#include "utils.h"
#include "interface.h"

typedef struct {
	char *name;
	char *src;
	EdjeCb func;
} EdjeCallback;

static void setup_playlist(Euphoria *e);
static void register_callbacks(Euphoria *e);

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
	Euphoria *e = data;

	ecore_evas_raise(e->gui.ee);
}

static bool ui_init_dragger(Euphoria *e) {
	Evas_Object *dragger;

	if (!(dragger = esmart_draggies_new(e->gui.ee)))
		return false;

	esmart_draggies_button_set(dragger, 1);

	evas_object_name_set(dragger, "dragger");
	evas_object_move(dragger, 0, 0);
	evas_object_layer_set(dragger, 0);
	evas_object_show(dragger);

	esmart_draggies_event_callback_add(dragger, EVAS_CALLBACK_MOUSE_UP,
	                                   cb_dragger_mouse_up, e);

	return true;
}

bool ui_init(Euphoria *e) {
	int zero = 0;
	char buf[PATH_MAX];

	debug(DEBUG_LEVEL_INFO, "Starting setup\n");

	ecore_evas_init();
	edje_init();
	ewl_init(&zero, NULL);

	ecore_event_handler_add(ECORE_EVENT_SIGNAL_EXIT, app_signal_exit,
	                        NULL);

#ifdef HAVE_ECORE_EVAS_GL
	if (!strcasecmp(e->cfg.evas_engine, "gl")) {
		debug(DEBUG_LEVEL_INFO, "Starting EVAS GL X11\n");
		e->gui.ee = ecore_evas_gl_x11_new(NULL, 0, 0, 0, 0, 0);
	} else
#endif

#ifdef HAVE_ECORE_EVAS_FB
	if (!strcasecmp(e->cfg.evas_engine, "fb")) {
		debug(DEBUG_LEVEL_INFO, "Starting EVAS FB\n");
		e->gui.ee = ecore_evas_fb_new(NULL, 0, 0, 0);
	} else
#endif

	{
		debug(DEBUG_LEVEL_INFO, "Starting EVAS X11\n");
		e->gui.ee = ecore_evas_software_x11_new(NULL, 0, 0, 0, 0, 0);
	}

	if (!e->gui.ee) {
		debug(DEBUG_LEVEL_CRITICAL,
		      "Cannot create Ecore Evas (using %s engine)\n",
		      e->cfg.evas_engine);

		return false;
	}

	ecore_evas_title_set(e->gui.ee, "Euphoria");
	ecore_evas_name_class_set(e->gui.ee, "ecore_test",
	                          "test_evas");
	ecore_evas_borderless_set(e->gui.ee, 1);
	ecore_evas_shaped_set(e->gui.ee, 1);

	ecore_evas_callback_pre_render_set(e->gui.ee,
	                                   cb_ee_pre_render);
	ecore_evas_callback_post_render_set(e->gui.ee,
	                                    cb_ee_post_render);
	ecore_evas_callback_resize_set(e->gui.ee, cb_ee_resize);

	ecore_evas_show(e->gui.ee);

	e->gui.evas = ecore_evas_get(e->gui.ee);

	snprintf(buf, sizeof(buf), "%s/.e/apps/%s/fonts",
	         getenv("HOME"), PACKAGE);

	evas_font_path_append(e->gui.evas, buf);
	evas_font_path_append(e->gui.evas, DATA_DIR "/fonts");
	evas_font_path_append(e->gui.evas, "/usr/X11R6/lib/X11/fonts");
	evas_font_path_append(e->gui.evas, "/usr/share/fonts");

	if (!ui_init_dragger(e))
		return false;

	if (!ui_init_edje(e, PACKAGE))
		return false;

	e->playlist = playlist_new(e->gui.evas, e->cfg.theme, e->xmms);
	assert(e->playlist);

	return true;
}

void ui_shutdown_edje(Euphoria *e) {
	if (e->gui.playlist) {
		edje_object_part_unswallow(e->gui.edje,
		                           e->gui.playlist);
		evas_object_del(e->gui.playlist);
		e->gui.playlist = NULL;
	}

	if (e->gui.edje) {
		evas_object_del(e->gui.edje);
		e->gui.edje = NULL;
	}
}

void ui_shutdown(Euphoria *e) {
	assert(e);

	ui_shutdown_edje(e);

	ewl_shutdown();
	edje_shutdown();
	ecore_evas_shutdown();
}

void ui_fill_track_info(Euphoria *e, PlayListItem *pli) {
	char buf[32];

	edje_object_part_text_set(e->gui.edje, "song_name",
	                          playlist_item_title_get(pli));

	edje_object_part_text_set(e->gui.edje, "artist_name",
	                          playlist_item_artist_get(pli));

	edje_object_part_text_set(e->gui.edje, "album_name",
	                          playlist_item_album_get(pli));

	/* sample rate */
	snprintf(buf, sizeof(buf), "%.1f",
	         (float) playlist_item_samplerate_get(pli) / 1000);
	edje_object_part_text_set(e->gui.edje, "track_samplerate", buf);

	/* bitrate */
	snprintf(buf, sizeof(buf), "%i", playlist_item_bitrate_get(pli));
	edje_object_part_text_set(e->gui.edje, "track_bitrate", buf);
}

bool ui_init_edje(Euphoria *e, const char *name) {
	double edje_w = 0, edje_h = 0;

	debug(DEBUG_LEVEL_INFO, "EDJE: Defining Edje \n");

	if (!(e->gui.edje = edje_object_add(e->gui.evas))) {
		debug(DEBUG_LEVEL_CRITICAL, "Cannot create Edje!\n");
		return false;
	}

	evas_object_name_set(e->gui.edje, "main_edje");

	if (!edje_object_file_set(e->gui.edje,
	                          find_theme(e->cfg.theme),
	                          name)) {
		debug(DEBUG_LEVEL_CRITICAL, "Cannot load theme '%s'!\n",
		      e->cfg.theme);
		return false;
	}

	evas_object_move(e->gui.edje, 0, 0);
	evas_object_pass_events_set (e->gui.edje, 1);
	evas_object_show(e->gui.edje);

	/* set max size */
	edje_object_size_max_get(e->gui.edje, &edje_w, &edje_h);
	ecore_evas_size_max_set(e->gui.ee, edje_w, edje_h);

	/* set min size */
	edje_object_size_min_get(e->gui.edje, &edje_w, &edje_h);
	ecore_evas_size_min_set(e->gui.ee, edje_w, edje_h);

	/* resize to the min size */
	ecore_evas_resize(e->gui.ee, (int) edje_w, (int) edje_h);

	setup_playlist(e);
	ui_refresh_volume(e);
	if(e->playlist && e->playlist->current_item)
	    ui_fill_track_info(e, e->playlist->current_item);

	register_callbacks(e);

	return true;
}

static void register_callbacks(Euphoria *e) {
	Evas_Object *o = e->gui.edje;
	int i;
	EdjeCallback cb[] = {
		{"QUIT", "*",
		 (EdjeCb) on_edje_euphoria_quit},
		{"PLAY_PREVIOUS", "*",
		 (EdjeCb) on_edje_track_prev},
		{"PLAY_NEXT", "*",
		 (EdjeCb) on_edje_track_next},
		{"SEEK_BACK", "*",
		 (EdjeCb) on_edje_seek_backward},
		{"SEEK_FORWARD", "*",
		 (EdjeCb) on_edje_seek_forward},
		{"SEEK_BACK_START", "*",
		 (EdjeCb) on_edje_seek_backward_start},
		{"SEEK_BACK_STOP", "*",
		 (EdjeCb) on_edje_seek_backward_stop},
		{"SEEK_FORWARD_START", "*",
		 (EdjeCb) on_edje_seek_forward_start},
		{"SEEK_FORWARD_STOP", "*",
		 (EdjeCb) on_edje_seek_forward_stop},
		{"PLAY", "*",
		 (EdjeCb) on_edje_play},
		{"PAUSE", "*",
		 (EdjeCb) on_edje_pause},
		{"STOP", "*",
		 (EdjeCb) on_edje_stop},
		{"VOL_INCR", "*",
		 (EdjeCb) on_edje_volume_raise},
		{"VOL_DECR", "*",
		 (EdjeCb) on_edje_volume_lower},
		{"TOGGLE_TIME_DISPLAY_MODE", "*",
		 (EdjeCb) on_edje_time_display_toggle},
		{"TOGGLE_REPEAT_MODE", "*",
		 (EdjeCb) on_edje_repeat_mode_toggle},
		{"SWITCH_GROUP", "*",
		 (EdjeCb) on_edje_switch_group},
		{"SEEKER*", "*",
		 (EdjeCb) on_edje_update_seeker},
		{"PLAYLIST_ADD", "*",
		 (EdjeCb) on_edje_playlist_add},
		{"PLAYLIST_DEL", "*",
		 (EdjeCb) on_edje_playlist_del}};

	for (i = 0; i < sizeof (cb) / sizeof (EdjeCallback); i++)
		edje_object_signal_callback_add(o, cb[i].name, cb[i].src,
		                                cb[i].func, e);

	/* Setup Key Binds */
	evas_object_focus_set(o, true);
	evas_object_event_callback_add(o, EVAS_CALLBACK_KEY_DOWN, cb_key_press, e);
	/* This is for a later time and place, see notes with cb_key_release() */
	//evas_object_event_callback_add(e, EVAS_CALLBACK_KEY_UP, cb_key_release, e);
}

/**
 * Add the playlist container.
 *
 * @param e
 */
static void setup_playlist(Euphoria *e) {
	if (!edje_object_part_exists(e->gui.edje, "playlist"))
		return;

	e->gui.playlist = e_container_new(e->gui.evas);
	assert(e->gui.playlist);

	evas_object_name_set(e->gui.playlist, "PlayList");
	evas_object_data_set(e->gui.playlist, "Euphoria", e);

	e_container_direction_set(e->gui.playlist, 1);
	e_container_spacing_set(e->gui.playlist, 0);
	e_container_fill_policy_set(e->gui.playlist,
	                            CONTAINER_FILL_POLICY_FILL_X);

	edje_object_part_swallow(e->gui.edje, "playlist",
	                         e->gui.playlist);
}

int ui_refresh_volume(void *udata) {
#if 0
	Euphoria *e = udata;
	char buf[8];
	int left = 0, right = 0;

	if (!e->output->volume_get(&left, &right))
		return 1;

	snprintf(buf, sizeof(buf), "%i", (left + right) / 2);
	edje_object_part_text_set(e->gui.edje, "vol_display_text", buf);

#endif

	return 1;
}

bool ui_refresh_time(Euphoria *e, int time) {
	char buf[9], *fmt[2];

	fmt[TIME_DISPLAY_ELAPSED] = "%i:%02i";
	fmt[TIME_DISPLAY_LEFT] = "-%i:%02i";

	snprintf(buf, sizeof(buf), fmt[e->cfg.time_display],
	         (time / 60), (time % 60));

	edje_object_part_text_set(e->gui.edje, "time_text", buf);
	evas_render(e->gui.evas);

	return true;
}

bool ui_refresh_seeker(Euphoria *e, double song_pos) {
	edje_object_part_drag_value_set(e->gui.edje, "seeker",
	                                song_pos, song_pos);
	edje_object_thaw(e->gui.edje);
	evas_render(e->gui.evas);

	return true;
}

