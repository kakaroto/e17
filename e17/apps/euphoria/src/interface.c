/*
 * $Id$
 * vim:noexpandtab:sw=4:sts=4:ts=4
 */

#include <config.h>
#include <assert.h>
#include <Esmart/Esmart_Container.h>
#include <Esmart/Esmart_Draggies.h>
#include <Esmart/Esmart_File_Dialog.h>
#include <Esmart/Esmart_Trans.h>
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
static bool ui_init_esmart_file_dialog(Euphoria *e);

static int app_signal_exit(void *data, int type, void *event) {
    Evas_Object *o = NULL;
    if((o = (Evas_Object*)data))
    {
	edje_object_signal_emit(o, "QUIT", "");
    }
    return 1;
}

static void cb_ee_pre_render(Ecore_Evas *ee) {
	edje_thaw();
}

static void cb_ee_post_render(Ecore_Evas *ee) {
	edje_freeze();
}

/**
 * @param ee
 */
static void cb_ee_del(Ecore_Evas *ee) {
    ecore_evas_hide(ee);
}
/**
 * Resizes the Edje to the size of our Ecore Evas
 *
 * @param ee
 */
static void cb_ee_resize(Ecore_Evas *ee) {
	Evas *evas = ecore_evas_get(ee);
	Evas_Object *o = NULL;
	int x = 0, y = 0, w = 0, h = 0;

	ecore_evas_geometry_get(ee, &x, &y, &w, &h);
	if((o = evas_object_name_find(evas, "main_edje")))
	    evas_object_resize(o, (Evas_Coord) w, (Evas_Coord) h);
	if((o = evas_object_name_find(evas, "dragger")))
	    evas_object_resize(o, (Evas_Coord) w, (Evas_Coord) h);
	if((o = evas_object_name_find(evas, "trans")))
	{
	    evas_object_resize(o, (Evas_Coord) w, (Evas_Coord) h);
	    esmart_trans_x11_freshen(o, (Evas_Coord)x, (Evas_Coord)y,
					(Evas_Coord)w, (Evas_Coord)h);
	}
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
	ecore_evas_name_class_set(e->gui.ee, "euphoria", "Euphoria");
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

	if(ui_init_esmart_file_dialog(e))
	{
	    fprintf(stderr, "Success\n");
	}
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
	Evas_Coord edje_w = 0, edje_h = 0;
	const char *invert;

	debug(DEBUG_LEVEL_INFO, "EDJE: Defining Edje \n");

	if (!(e->gui.edje = edje_object_add(e->gui.evas))) {
		debug(DEBUG_LEVEL_CRITICAL, "Cannot create Edje!\n");
		return false;
	}

	ecore_event_handler_add(ECORE_EVENT_SIGNAL_EXIT, app_signal_exit,
	                        e->gui.edje);
	evas_object_name_set(e->gui.edje, "main_edje");

	if (!edje_object_file_set(e->gui.edje,
	                          find_theme(e->cfg.theme),
	                          name)) {
		debug(DEBUG_LEVEL_CRITICAL, "Cannot load theme '%s'!\n",
		      e->cfg.theme);
		return false;
	}

	invert = edje_object_data_get(e->gui.edje, "seeker_invert_dir");

	e->seekerflags.is_vertical = (edje_object_part_drag_dir_get(e->gui.edje, "seeker")
	                              == EDJE_DRAG_DIR_Y);
	/* inverted mode is on by default for vertical seekers */
	e->seekerflags.invert_dir = e->seekerflags.is_vertical;

	if (invert)
		e->seekerflags.invert_dir = atoi(invert);

	evas_object_move(e->gui.edje, 0, 0);
	evas_object_pass_events_set (e->gui.edje, 1);
	evas_object_show(e->gui.edje);

	/* set max size */
	edje_object_size_max_get(e->gui.edje, &edje_w, &edje_h);
	ecore_evas_size_max_set(e->gui.ee, (int) edje_w, (int) edje_h);

	/* set min size */
	edje_object_size_min_get(e->gui.edje, &edje_w, &edje_h);
	ecore_evas_size_min_set(e->gui.ee, (int) edje_w, (int) edje_h);

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
		{"CLOSE", "*",
		 (EdjeCb) on_edje_euphoria_close},
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
		 (EdjeCb) on_edje_playlist_del},
		{"PLAYLIST_CLEAR", "*",
		 (EdjeCb) on_edje_playlist_clear},
		{"PLAYLIST_SHUFFLE", "*", 
		 (EdjeCb)on_edje_playlist_shuffle}};

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

	e->gui.playlist = esmart_container_new(e->gui.evas);
	assert(e->gui.playlist);

	evas_object_name_set(e->gui.playlist, "PlayList");
	evas_object_data_set(e->gui.playlist, "Euphoria", e);

	esmart_container_direction_set(e->gui.playlist, 1);
	esmart_container_spacing_set(e->gui.playlist, 0);
	esmart_container_fill_policy_set(e->gui.playlist,
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

void ui_zero_track_info(Euphoria *e) {
	edje_object_part_text_set(e->gui.edje, "song_name", "");
	edje_object_part_text_set(e->gui.edje, "artist_name", "");
	edje_object_part_text_set(e->gui.edje, "album_name", "");
	edje_object_part_text_set(e->gui.edje, "track_samplerate", "---");
	edje_object_part_text_set(e->gui.edje, "track_bitrate", "---");
	edje_object_part_text_set(e->gui.edje, "time_text", "0:00");
}
static void file_dialog_cb(void *data, Evas_Object *efd, int type)
{
    char buf[PATH_MAX];
    Euphoria *e = NULL;
    Evas_List *l = NULL;
    Ecore_Evas *ee = NULL;
    const char *directory = NULL;
    
    if((e = (Euphoria*)data))
    {
	switch(type)
	{
	    case FILE_CANCEL:
		ecore_evas_hide(e->gui.file_dialog.ee);
		break;
	    case DIR_CHANGED:
		ecore_evas_title_set(e->gui.file_dialog.ee,
			    esmart_file_dialog_current_directory_get(efd));
		break;
	    case FILE_OK:
		if((directory = esmart_file_dialog_current_directory_get(efd)))
		{
		    l = esmart_file_dialog_selections_get(efd); 
		    for( ; l; l = l->next)
		    {
			snprintf(buf, PATH_MAX, "file://%s/%s", directory, (char*)l->data);
		    xmmsc_result_unref(xmmsc_playlist_add(e->xmms, buf));
		    }
		}
		break;
	    default:
		break;
	}
    }
}
bool ui_init_esmart_file_dialog(Euphoria *e)
{
    int zero = 0;
    char buf[PATH_MAX];
    Ecore_Evas *ee = NULL;
    Evas_Object *o = NULL;
    Evas_Object *efd = NULL;
    Evas_Object *edje = NULL;
    Evas_Object *trans = NULL;
    Evas_Object *dragger = NULL;
    Evas_Coord w = 320, h = 240;
    const char *window_type = NULL;

	debug(DEBUG_LEVEL_INFO, "Starting File Dialog Setup\n");

#ifdef HAVE_ECORE_EVAS_GL
	if (!strcasecmp(e->cfg.evas_engine, "gl")) {
		debug(DEBUG_LEVEL_INFO, "Starting EVAS GL X11\n");
		ee = ecore_evas_gl_x11_new(NULL, 0, 0, 0, 0, 0);
	} else
#endif

#ifdef HAVE_ECORE_EVAS_FB
	if (!strcasecmp(e->cfg.evas_engine, "fb")) {
		debug(DEBUG_LEVEL_INFO, "Starting EVAS FB\n");
		ee = ecore_evas_fb_new(NULL, 0, 0, 0);
	} else
#endif

	{
		debug(DEBUG_LEVEL_INFO, "Starting EVAS X11\n");
		ee = ecore_evas_software_x11_new(NULL, 0, 0, 0, 0, 0);
	}

	if (!ee) {
		debug(DEBUG_LEVEL_CRITICAL,
		      "Cannot create Ecore Evas (using %s engine)\n",
		      e->cfg.evas_engine);

		return false;
	}

	ecore_evas_title_set(ee, "Select a file");
	ecore_evas_name_class_set(ee, "euphoria", "Euphoria Files");
	ecore_evas_resize(ee, 320, 240);
#if 0
	ecore_evas_borderless_set(ee, 1);
	ecore_evas_shaped_set(ee, 1);
#endif

	ecore_evas_callback_pre_render_set(ee, cb_ee_pre_render);
	ecore_evas_callback_post_render_set(ee, cb_ee_post_render);
	ecore_evas_callback_resize_set(ee, cb_ee_resize);
	ecore_evas_callback_delete_request_set(ee, cb_ee_del);
	e->gui.file_dialog.ee = ee;

	debug(DEBUG_LEVEL_INFO, "EDJE: Defining Edje \n");
        fprintf(stderr, "Trying %s\n", find_theme(e->cfg.theme));
	if((efd = esmart_file_dialog_new(ecore_evas_get(ee), find_theme(e->cfg.theme))) == NULL)
	{
	    fprintf(stderr, "Trying %s\n", DATA_DIR"/themes/fd.eet");
	    if((efd = esmart_file_dialog_new(ecore_evas_get(ee), DATA_DIR"/themes/fd.eet")) == NULL)
	    {
		debug(DEBUG_LEVEL_CRITICAL, "Cannot load fd theme '%s'!\n",
		      e->cfg.theme);
		ecore_evas_free(ee);
		e->gui.file_dialog.ee = NULL;
		return false;

	    }
	}
	evas_object_name_set(efd, "main_edje");
	evas_object_move(efd, 0, 0);
	evas_object_layer_set(efd, 2);
	evas_object_resize(efd, 320, 240);
	esmart_file_dialog_callback_add(efd, file_dialog_cb, e);
	evas_object_show(efd);

	fprintf(stderr, "Getting the edje\n");
	o = esmart_file_dialog_edje_get(efd);
	if((window_type = edje_object_data_get(o, "e,fd,window,type")))
	{
	    if(!strcmp(window_type, "shaped"))
	    {
		ecore_evas_shaped_set(ee, 1);
		ecore_evas_borderless_set(ee, 1);
	    }
	    else if(!strcmp(window_type, "borderless"))
	    {
		ecore_evas_borderless_set(ee, 1);
	    }
	    else if(!strcmp(window_type, "trans"))
	    {
		ecore_evas_borderless_set(ee, 1);
		trans = esmart_trans_x11_new(ecore_evas_get(ee));
		esmart_trans_x11_window_set(trans, 
				ecore_evas_software_x11_window_get(ee));
		evas_object_name_set(trans, "trans");
		evas_object_move(trans, 0, 0);
		evas_object_layer_set(trans, 0);
		evas_object_show(trans);
	    }
	}
	dragger = esmart_draggies_new(ee);
	esmart_draggies_button_set(dragger, 1);
	evas_object_name_set(dragger, "dragger");
	evas_object_move(dragger, 0, 0);
	evas_object_layer_set(dragger, -5);
	evas_object_show(dragger);
	
	/* set max size */
	edje_object_size_max_get(o, &w, &h);
	fprintf(stderr, "%d %d is the size\n", w, h);
	if((w > 0) && (h > 0))
	{
	    if (w > INT_MAX)
		w = INT_MAX;
	    if(h > INT_MAX)
		h = INT_MAX;
	    ecore_evas_size_max_set(ee, (int) w, (int) h);
	}

	/* set min size */
	edje_object_size_min_get(o, &w, &h);
	ecore_evas_size_min_set(ee, (int) w, (int) h);
	evas_object_resize(efd, w, h);
	ecore_evas_resize(ee, (int)w, (int)h);
	return true;
}
