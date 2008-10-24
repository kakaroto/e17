/*
 * $Id$
 */

#include <config.h>
#include <Edje.h>
#include <Esmart/Esmart_Container.h>
#include <Ecore_X.h>
#include <assert.h>
#include <ewl/Ewl.h>
#include "eplayer.h"
#include "track.h"
#include "interface.h"
#include "utils.h"
#include "callbacks.h"

typedef enum {
	PLAYBACK_STATE_STOPPED,
	PLAYBACK_STATE_PAUSED,
	PLAYBACK_STATE_PLAYING,
	PLAYBACK_STATE_NUM
} PlaybackState;

static void hilight_current_track(ePlayer *player);
static int _eplayer_seek_timer(void *data);

static PlaybackState playback_state = PLAYBACK_STATE_STOPPED;
static Ewl_Widget *_fd_win = NULL;

static void signal_playback_state(ePlayer *player) {
	char *sig[PLAYBACK_STATE_NUM] = {"PLAYBACK_STATE_STOPPED",
	                                 "PLAYBACK_STATE_PAUSED",
	                                 "PLAYBACK_STATE_PLAYING"};

	assert(player);

	edje_object_signal_emit(player->gui.edje, sig[playback_state],
	                        "ePlayer");
}

static void playback_state_set(ePlayer *player, PlaybackState new_state) {
	assert(player);

	if (new_state != playback_state) {
		playback_state = new_state;
		signal_playback_state(player);
	}
}

/**
 * Starts/resumes playback.
 */
EDJE_CB(play) {
	int res;

	debug(DEBUG_LEVEL_INFO, "Play callback entered\n");

	switch (playback_state) {
		case PLAYBACK_STATE_STOPPED:
		case PLAYBACK_STATE_PAUSED: /* continue playback */
			res = eplayer_playback_start(player, false);
			break;
		case PLAYBACK_STATE_PLAYING: /* restart from beginning */
			eplayer_playback_stop(player);
			res = eplayer_playback_start(player, true);
			break;
		default:
			assert(false);
			break;
	}

	if (res)
		playback_state_set(player, PLAYBACK_STATE_PLAYING);

	hilight_current_track(player);
}

/**
 * Stops playback.
 */
EDJE_CB(stop) {
	debug(DEBUG_LEVEL_INFO, "Stop callback entered\n");

	eplayer_playback_stop(player);
	track_rewind(player);

	playback_state_set(player, PLAYBACK_STATE_STOPPED);
}

/**
 * Pauses/resumes playback.
 */
EDJE_CB(pause) {
	PlaybackState state = playback_state;

	debug(DEBUG_LEVEL_INFO, "Pause callback entered\n");

	switch (playback_state) {
		case PLAYBACK_STATE_STOPPED:
			break;
		case PLAYBACK_STATE_PAUSED:
			if (eplayer_playback_start(player, false))
				state = PLAYBACK_STATE_PLAYING;
			break;
		case PLAYBACK_STATE_PLAYING:
			eplayer_playback_stop(player);
			state = PLAYBACK_STATE_PAUSED;
			break;
		default:
			assert(false);
			break;
	}

	playback_state_set(player, state);
}

/**
 * Hilight the current playlist item.
 * This automatically un-highlights all other items
 * (see EDJE_CB(playlist_item_selected).
 */
static void hilight_current_track(ePlayer *player) {
	PlayListItem *pli;

	if (!(pli = playlist_current_item_get(player->playlist)))
		return;

	edje_object_signal_emit(pli->edje, "PLAYLIST_ITEM_SELECTED", "");
}

/**
 * Moves to the next track and plays it, except when we're going
 * back to the beginning of the playlist.
 */
EDJE_CB(track_next) {
	PlaybackState state = playback_state;
	bool play = true;

	debug(DEBUG_LEVEL_INFO, "Next File Called\n");

	eplayer_playback_stop(player);

	/* check whether we moved to the beginning of the list */
	if (playlist_current_item_next(player->playlist))
		play = player->cfg.repeat;

	if (play) {
		if (eplayer_playback_start(player, true))
			state = PLAYBACK_STATE_PLAYING;
	} else {
		/* refresh track info parts, but don't start playing yet */
		track_open(player);
		state = PLAYBACK_STATE_STOPPED;
	}

	hilight_current_track(player);
	playback_state_set(player, state);
}

/**
 * Moves to the previous track and plays it, except when we're
 * at the first track already.
 */
EDJE_CB(track_prev) {
	PlaybackState state = playback_state;

	debug(DEBUG_LEVEL_INFO, "Previous File Called\n");

	/* first item on the list: do nothing */
	if (!playlist_current_item_has_prev(player->playlist))
		return;

	eplayer_playback_stop(player);

	/* Get the previous list item */
	playlist_current_item_prev(player->playlist);

	if (eplayer_playback_start(player, true))
		state = PLAYBACK_STATE_PLAYING;

	hilight_current_track(player);
	playback_state_set(player, state);
}

EDJE_CB(volume_raise) {
	int left = 0, right = 0;

	debug(DEBUG_LEVEL_INFO, "Raising volume\n");

	if (!player->output->volume_get(&left, &right))
		return;

	player->output->volume_set(left + 5, right + 5);
	ui_refresh_volume(player);
}

EDJE_CB(volume_lower) {
	int left = 0, right = 0;

	debug(DEBUG_LEVEL_INFO, "Lowering volume\n");

	if (!player->output->volume_get(&left, &right))
		return;

	player->output->volume_set(left - 5, right - 5);
	ui_refresh_volume(player);
}

EDJE_CB(time_display_toggle) {
	player->cfg.time_display = !player->cfg.time_display;
	track_update_time(player);
}

EDJE_CB(repeat_mode_toggle) {
	player->cfg.repeat = !player->cfg.repeat;
}

EDJE_CB(playlist_scroll_up) {
	int size = (int) evas_object_data_get(player->gui.playlist,
	                                      "PlaylistFontSize");

	/* it's * 3 because we're scrolling 3 elements at once */
	esmart_container_scroll(player->gui.playlist, size * 3);
}

EDJE_CB(playlist_scroll_down) {
	int size = (int) evas_object_data_get(player->gui.playlist,
	                                      "PlaylistFontSize");

	/* it's * 3 because we're scrolling 3 elements at once */
	esmart_container_scroll(player->gui.playlist, size * -3);
}

EDJE_CB(playlist_item_play) {
	PlaybackState state = playback_state;
	PlayListItem *pli = evas_object_data_get(obj, "PlayListItem");

	eplayer_playback_stop(player);

	playlist_current_item_set(player->playlist, pli);

	if (eplayer_playback_start(player, true))
		state = PLAYBACK_STATE_PLAYING;

	playback_state_set(player, state);
}

static void remove_playlist_item(ePlayer *player, PlayListItem *pli) {
	bool ok = false;

	assert(pli);

	if (pli != playlist_current_item_get(player->playlist)) {
		playlist_remove_item(player->playlist, pli);
		return;
	}

	eplayer_playback_stop(player);

	if (playlist_current_item_has_next(player->playlist))
		ok = playlist_current_item_next(player->playlist);

	if (!ok && playlist_current_item_has_prev(player->playlist))
		ok = playlist_current_item_prev(player->playlist);

	if (!ok)
		playlist_remove_all(player->playlist);
	else
		playlist_remove_item(player->playlist, pli);
}

EDJE_CB(playlist_item_remove) {
	PlayListItem *pli = evas_object_data_get(obj, "PlayListItem");

	if (pli)
		remove_playlist_item(player, pli);
}

EDJE_CB(playlist_item_selected) {
	Eina_List *items = esmart_container_elements_get(player->gui.playlist);
	Eina_List *l;

	for (l = items; l; l = l->next)
		if (l->data != obj)
			edje_object_signal_emit(l->data,
			                        "PLAYLIST_ITEM_UNSELECTED", "");
}

EDJE_CB(seek_forward) {
	PlaybackState state = playback_state;
	PlayListItem *pli = playlist_current_item_get(player->playlist);

	debug(DEBUG_LEVEL_INFO, "Seeking forward\n");

	/* We don't care if you seek past the file, the play loop
	 * will catch EOF and play next file
	 */
	eplayer_playback_stop(player);
	pli->plugin->set_current_pos(pli->plugin->get_current_pos() + 5);

	if (eplayer_playback_start(player, false))
		state = PLAYBACK_STATE_PLAYING;

	playback_state_set(player, state);
}

EDJE_CB(seek_backward) {
	PlaybackState state = playback_state;
	PlayListItem *pli = playlist_current_item_get(player->playlist);
	int cur_time = pli->plugin->get_current_pos();

	debug(DEBUG_LEVEL_INFO, "Seeking backward\n",
	      cur_time);

	eplayer_playback_stop(player);

	if (cur_time >= 6)
		pli->plugin->set_current_pos(cur_time - 5);

	if (eplayer_playback_start(player, cur_time < 6))
		state = PLAYBACK_STATE_PLAYING;

	playback_state_set(player, state);
}

EDJE_CB(seek_forward_start) {
	debug(DEBUG_LEVEL_INFO, "Start seeking forward\n");
	player->flags.seeking = true;
	player->flags.seek_dir = 1;
	ecore_timer_add(.02, _eplayer_seek_timer, player);
}

EDJE_CB(seek_forward_stop) {
	debug(DEBUG_LEVEL_INFO, "Stop seeking forward\n");
	player->flags.seeking = false;
}

EDJE_CB(seek_backward_start) {
	debug(DEBUG_LEVEL_INFO, "Start seeking backward\n");
	player->flags.seeking = true;
	player->flags.seek_dir = -1;
	ecore_timer_add(.02, _eplayer_seek_timer, player);
}

EDJE_CB(seek_backward_stop) {
	debug(DEBUG_LEVEL_INFO, "Stop seeking backward\n");
	player->flags.seeking = false;
}

EDJE_CB(eplayer_quit) {
	ecore_main_loop_quit();
}

EDJE_CB(eplayer_raise) {
	ecore_evas_raise(player->gui.ee);
}

EDJE_CB(switch_group) {
	ui_shutdown_edje(player);
	ui_init_edje(player, src);

	ui_refresh_volume(player);
	ui_fill_track_info(player);

	playlist_container_set(player->playlist, player->gui.playlist);
	hilight_current_track(player);

	signal_playback_state(player);
}

EDJE_CB(update_seeker) {
	PlayListItem *pli = playlist_current_item_get(player->playlist);
	Evas_Coord x, y, w, h;
	int type, ex = 0, ey = 0;
	double pos;

	if (!strcmp(emission, "SEEKER_START"))
		player->flags.seeker_seeking = true;
	else if (!strcmp(emission, "SEEKER_STOP"))
		player->flags.seeker_seeking = false;

	if (!player->flags.seeker_seeking)
		return;

	type = ecore_event_current_type_get();

	if (type == ECORE_X_EVENT_MOUSE_MOVE) {
		Ecore_X_Event_Mouse_Move *event;

		event = ecore_event_current_event_get();
		ex = event->x;
		ey = event->y;
	} else if (type == ECORE_X_EVENT_MOUSE_BUTTON_DOWN) {
		Ecore_X_Event_Mouse_Button_Down *event;

		event = ecore_event_current_event_get();
		ex = event->x;
		ey = event->y;
	} else
		assert(false);

	edje_object_part_geometry_get(player->gui.edje, "seeker_grabber",
	                              &x, &y, &w, &h);

	pos = ((double)(ex - x)) / ((double)w);

	if (pos < 0) pos = 0;
	if (pos > 1) pos = 1;

	track_position_set(player, (int)(pli->duration * pos));
}

static int _eplayer_seek_timer(void *data)
{
	ePlayer *player = data;
	PlayListItem *pli;
	int new_pos;

	if (!(pli = playlist_current_item_get(player->playlist)))
		return 0;

	new_pos = pli->current_pos + player->flags.seek_dir;

	if (new_pos <= 0)
		new_pos = 0;

	if (new_pos > pli->duration)
		new_pos = pli->duration;

	track_position_set(player, new_pos);

	return !!player->flags.seeking;
}

/* Handle Key Bindings via EVAS Event Callback */
void
cb_key_press(void *data, Evas *e, Evas_Object *obj, void *event_info) {
	ePlayer *player = data;

        Evas_Event_Key_Down *ev;
        ev = (Evas_Event_Key_Down *)event_info;

        //printf("DEBUG: You hit key: %s\n", ev->keyname);

	if      (!strcmp(ev->keyname, "Return"))
	   edje_object_signal_emit(player->gui.edje, "PLAY_NEXT", "*");
        if      (!strcmp(ev->keyname, "space"))
           edje_object_signal_emit(player->gui.edje, "PAUSE", "*");
	else if (!strcmp(ev->keyname, "Escape"))
	   edje_object_signal_emit(player->gui.edje, "QUIT", "*");
        else if (!strcmp(ev->keyname, "q"))
           edje_object_signal_emit(player->gui.edje, "QUIT", "*");
        else if (!strcmp(ev->keyname, "Down"))
           edje_object_signal_emit(player->gui.edje, "PLAY_NEXT", "*");
        else if (!strcmp(ev->keyname, "Up"))
           edje_object_signal_emit(player->gui.edje, "PLAY_PREVIOUS", "*");
        else if (!strcmp(ev->keyname, "Right"))
           edje_object_signal_emit(player->gui.edje, "SEEK_FORWARD", "*");
        else if (!strcmp(ev->keyname, "Left"))
           edje_object_signal_emit(player->gui.edje, "SEEK_BACK", "*");
        else if (!strcmp(ev->keyname, "KP_Add"))
           edje_object_signal_emit(player->gui.edje, "VOL_INCR", "*");
        else if (!strcmp(ev->keyname, "equal"))
           edje_object_signal_emit(player->gui.edje, "VOL_INCR", "*");
        else if (!strcmp(ev->keyname, "KP_Subtract"))
           edje_object_signal_emit(player->gui.edje, "VOL_DECR", "*");
        else if (!strcmp(ev->keyname, "minus"))
           edje_object_signal_emit(player->gui.edje, "VOL_DECR", "*");


	/********* See notes on cb_key_release().
        else if (!strcmp(ev->keyname, "Right"))
           edje_object_signal_emit(player->gui.edje, "SEEK_FORWARD_START", "*");
        else if (!strcmp(ev->keyname, "Left"))
           edje_object_signal_emit(player->gui.edje, "SEEK_BACK_START", "*");
	************/

}

/* Handle Key Bindings via EVAS Event Callback */
/* - This only works IF you turn off X key repeat, which raster */
/*   pointed out is global... so for most of us, this won't work. */
/* This function is, thus, currently unused, but left in for testing. */
void
cb_key_release(void *data, Evas *e, Evas_Object *obj, void *event_info) {
        ePlayer *player = data;

        Evas_Event_Key_Down *ev;
        ev = (Evas_Event_Key_Down *)event_info;

        //printf("DEBUG: You released key: %s\n", ev->keyname);

        if      (!strcmp(ev->keyname, "Right"))
           edje_object_signal_emit(player->gui.edje, "SEEK_FORWARD_STOP", "*");
        else if (!strcmp(ev->keyname, "Left"))
           edje_object_signal_emit(player->gui.edje, "SEEK_BACK_STOP", "*");

}

/* Callback to to close the filedialog window */
void destroy_ewl_filedialog(Ewl_Widget * w, void *ev_data,
                            void *user_data)
{
	ewl_widget_destroy(w);
	_fd_win = NULL;
}

static void cb_file_dialog_value_changed(Ewl_Widget *w, void *ev_data,
                                         void *udata) {
	ePlayer *player = udata;

	if (ev_data)
		playlist_load_any(player->playlist, ev_data, true);

	ewl_widget_hide(_fd_win);
}

/* File Dialog to add files, thanx to EWL */
EDJE_CB(playlist_add) {
    Ewl_Widget *fd_win = NULL;
    Ewl_Widget *fd = NULL;
    Ewl_Widget *vbox = NULL;

    if (_fd_win) {
		ewl_widget_show(_fd_win);
		return;
	}

	fd_win = ewl_window_new();
	ewl_window_title_set(EWL_WINDOW(fd_win), "Eplayer Add File...");
	ewl_window_name_set(EWL_WINDOW(fd_win), "Eplayer Add File...");
	ewl_object_size_request(EWL_OBJECT(fd_win), 500, 400);
	ewl_object_fill_policy_set(EWL_OBJECT(fd_win), EWL_FLAG_FILL_FILL |
	                           EWL_FLAG_FILL_SHRINK);

	ewl_callback_append(fd_win, EWL_CALLBACK_DELETE_WINDOW,
	                    destroy_ewl_filedialog, NULL);

	vbox = ewl_vbox_new();
	ewl_object_fill_policy_set(EWL_OBJECT(vbox), EWL_FLAG_FILL_FILL |
	                           EWL_FLAG_FILL_SHRINK);
	ewl_container_child_append(EWL_CONTAINER(fd_win), vbox);
	ewl_widget_show(vbox);

/* 	fd = ewl_filedialog_new(EWL_FILEDIALOG_TYPE_OPEN); */

	ewl_callback_append(fd, EWL_CALLBACK_VALUE_CHANGED,
	                    cb_file_dialog_value_changed, player);

	ewl_container_child_append(EWL_CONTAINER(vbox), fd);
	ewl_widget_show(fd);

	_fd_win = fd_win;
	ewl_widget_data_set(_fd_win, "player", player);
    ewl_widget_show(_fd_win);
}

EDJE_CB(playlist_del) {
	/* FIXME find the currently selected track and call
	 * remove_playlist_item()
	 */
}
