#include <config.h>
#include <Edje.h>
#include <Esmart/container.h>
#include <Ecore_X.h>
#include <assert.h>
#include <ewl/Ewl.h>
#include "eplayer.h"
#include "track.h"
#include "interface.h"
#include "utils.h"
#include "callbacks.h"

int _eplayer_seek_timer(void *data);

typedef enum {
	PLAYBACK_STATE_STOPPED,
	PLAYBACK_STATE_PAUSED,
	PLAYBACK_STATE_PLAYING
} PlaybackState;


/*static int paused = 0;*/
static PlaybackState state = PLAYBACK_STATE_STOPPED;

/**
 * Starts/resumes playback.
 *
 * @param player
 * @param e
 * @param o
 * @param event
 */
EDJE_CB(play) {
	debug(DEBUG_LEVEL_INFO, "Play callback entered\n");

	switch (state) {
		case PLAYBACK_STATE_STOPPED:
		case PLAYBACK_STATE_PAUSED: /* continue playback */
			eplayer_playback_start(player, 0);
			break;
		case PLAYBACK_STATE_PLAYING: /* restart from beginning */
			eplayer_playback_stop(player);
			eplayer_playback_start(player, 1);
			break;
		default:
			assert(0);
			break;
	}
	
	state = PLAYBACK_STATE_PLAYING;
}

/**
 * Stops playback.
 *
 * @param player
 * @param e
 * @param o
 * @param event
 */
EDJE_CB(stop) {
	debug(DEBUG_LEVEL_INFO, "Stop callback entered\n");

	eplayer_playback_stop(player);
	track_rewind(player);
	state = PLAYBACK_STATE_STOPPED;
}

/**
 * Pauses/resumes playback.
 *
 * @param player
 * @param e
 * @param o
 * @param event
 */
EDJE_CB(pause) {
	debug(DEBUG_LEVEL_INFO, "Pause callback entered\n");

	switch (state) {
		case PLAYBACK_STATE_STOPPED:
			return;
			break;
		case PLAYBACK_STATE_PAUSED:
			eplayer_playback_start(player, 0);
			state = PLAYBACK_STATE_PLAYING;
			break;
		case PLAYBACK_STATE_PLAYING:
			eplayer_playback_stop(player);
			state = PLAYBACK_STATE_PAUSED;
			break;
		default:
			assert(0);
			break;
	}
}

/**
 * Moves to the next track and plays it, except when we're going
 * back to the beginning of the playlist.
 *
 * @param player
 * @param e
 * @param o
 * @param event
 */
EDJE_CB(track_next) {
	int play = 1;
	
	debug(DEBUG_LEVEL_INFO, "Next File Called\n");

	eplayer_playback_stop(player);

	/* check whether we moved to the beginning of the list */
	if (playlist_current_item_next(player->playlist))
		play = player->cfg.repeat;

	if (play) {
		eplayer_playback_start(player, 1);
		state = PLAYBACK_STATE_PLAYING;
	} else {
		/* refresh track info parts, but don't start playing yet */
		track_open(player);
		state = PLAYBACK_STATE_STOPPED;
	}
}

/**
 * Moves to the previous track and plays it, except when we're
 * at the first track already.
 *
 * @param player
 * @param e
 * @param o
 * @param event
 */
EDJE_CB(track_prev) {
	debug(DEBUG_LEVEL_INFO, "Previous File Called\n");

	/* first item on the list: do nothing */
	if (!playlist_current_item_has_prev(player->playlist))
		return;

	eplayer_playback_stop(player);
	
	/* Get the previous list item */
	playlist_current_item_prev(player->playlist);

	eplayer_playback_start(player, 1);
	state = PLAYBACK_STATE_PLAYING;
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
	/* it's * 3 because we're scrolling 3 elements at once */
	e_container_scroll(player->gui.playlist,
	                   player->gui.playlist_font_size * 3);
}

EDJE_CB(playlist_scroll_down) {
	/* it's * 3 because we're scrolling 3 elements at once */
	e_container_scroll(player->gui.playlist,
	                   player->gui.playlist_font_size * -3);
}

EDJE_CB(playlist_item_play) {
	PlayListItem *pli = evas_object_data_get(obj, "PlayListItem");

	eplayer_playback_stop(player);

	playlist_current_item_set(player->playlist, pli);
	eplayer_playback_start(player, 1);
	state = PLAYBACK_STATE_PLAYING;
}

EDJE_CB(playlist_item_remove) {
	int ok = 0;
	PlayListItem *pli = evas_object_data_get(obj, "PlayListItem");
	
	e_container_element_remove(player->gui.playlist, obj);
	evas_object_del(obj);
	
	if (pli == playlist_current_item_get(player->playlist))
	{
	    eplayer_playback_stop(player);
		
	    if (playlist_current_item_has_next(player->playlist))
			ok = playlist_current_item_next(player->playlist);
		
	    if (!ok && playlist_current_item_has_prev(player->playlist))
			ok = playlist_current_item_prev(player->playlist);
	    
		if (!ok)
			playlist_remove_all(player->playlist);
	}

	playlist_remove_item(player->playlist, pli);
}

EDJE_CB(playlist_item_selected) {
	Evas_List *items = e_container_elements_get(player->gui.playlist);
	Evas_List *l;

	for (l = items; l; l = l->next)
		if (l->data != obj)
			edje_object_signal_emit(l->data,
			                        "PLAYLIST_ITEM_UNSELECTED", "");
}

EDJE_CB(seek_forward) {
	PlayListItem *pli = playlist_current_item_get(player->playlist);

	debug(DEBUG_LEVEL_INFO, "Seeking forward\n");

	/* We don't care if you seek past the file, the play loop
	 * will catch EOF and play next file
	 */
	eplayer_playback_stop(player);
	pli->plugin->set_current_pos(pli->plugin->get_current_pos() + 5);
	eplayer_playback_start(player, 0);
	state = PLAYBACK_STATE_PLAYING;
}

EDJE_CB(seek_backward) {
	PlayListItem *pli = playlist_current_item_get(player->playlist);
	int cur_time  = pli->plugin->get_current_pos();
	
	debug(DEBUG_LEVEL_INFO, "Seeking backward - Current Pos: %i\n",
	      cur_time);

	eplayer_playback_stop(player);

	if (cur_time < 6) /* restart from the beginning */
		eplayer_playback_start(player, 1);
	else {
		pli->plugin->set_current_pos(cur_time - 5);
		eplayer_playback_start(player, 0);
	}
	
	state = PLAYBACK_STATE_PLAYING;
}

EDJE_CB(seek_forward_start) {
  debug(DEBUG_LEVEL_INFO, "Start Seeking Forward");
  player->flags.seeking = 1; 
  player->flags.seek_dir = 1; 
  ecore_timer_add(.02, _eplayer_seek_timer, player);
}

EDJE_CB(seek_forward_stop) {
  debug(DEBUG_LEVEL_INFO, "Stop Seeking Forward");
  player->flags.seeking = 0; 
}

EDJE_CB(seek_backward_start) {
  debug(DEBUG_LEVEL_INFO, "Start Seeking Backward");
  player->flags.seeking = 1; 
  player->flags.seek_dir = -1; 
  ecore_timer_add(.02, _eplayer_seek_timer, player);
}

EDJE_CB(seek_backward_stop) {
  debug(DEBUG_LEVEL_INFO, "Stop Seeking Backward");
  player->flags.seeking = 0; 
}

EDJE_CB(eplayer_quit) {
	ecore_main_loop_quit();
}

EDJE_CB(eplayer_raise) {
	ecore_evas_raise(player->gui.ee);
}

EDJE_CB(switch_group) {
	evas_object_del(player->gui.edje);
	ui_init_edje(player, src);
}

EDJE_CB(update_seeker) {
	PlayListItem *pli = playlist_current_item_get(player->playlist);
	Evas_Coord x, y, w, h;
	int type, ex = 0, ey = 0;
	double pos;

	if (!strcmp(emission, "SEEKER_START"))
		player->flags.seeker_seeking = 1;
	else if (!strcmp(emission, "SEEKER_STOP"))
		player->flags.seeker_seeking = 0;

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
		assert(0);

	edje_object_part_geometry_get(player->gui.edje, "seeker_grabber",
	                              &x, &y, &w, &h);
      
	pos = ((double)(ex - x)) / ((double)w);
	
	if (pos < 0) pos = 0;
	if (pos > 1) pos = 1;

	track_position_set(player, (int)(pli->duration * pos));
}

int _eplayer_seek_timer(void *data)
{
	ePlayer *player = data;
	PlayListItem *pli = playlist_current_item_get(player->playlist);
	int new_pos;

	new_pos = pli->current_pos + player->flags.seek_dir;

	if (new_pos <= 0) new_pos = 0;
	if (new_pos > pli->duration) new_pos = pli->duration;

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

	return;
	ev_data = NULL;
	user_data = NULL;
}

/* File Dialog to add files, thanx to EWL */
EDJE_CB(playlist_add) {

	Ewl_Widget *fd_win;
	Ewl_Widget *fd;
	Ewl_Widget *vbox;

	fd_win = ewl_window_new();
	ewl_window_set_title(EWL_WINDOW(fd_win), "Eplayer Add File...");
  ewl_window_set_name(EWL_WINDOW(fd_win), "Eplayer Add File...");
  ewl_object_request_size(EWL_OBJECT(fd_win), 500, 400);
  ewl_object_set_fill_policy(EWL_OBJECT(fd_win), EWL_FLAG_FILL_FILL |
			EWL_FLAG_FILL_SHRINK);
	
  ewl_callback_append(fd_win, EWL_CALLBACK_DELETE_WINDOW,
			destroy_ewl_filedialog, NULL);
  ewl_widget_show(fd_win);
	
  vbox = ewl_vbox_new ();
  ewl_object_set_fill_policy(EWL_OBJECT(vbox), EWL_FLAG_FILL_FILL |
			EWL_FLAG_FILL_SHRINK);
  ewl_container_append_child(EWL_CONTAINER(fd_win), vbox);
  ewl_widget_show (vbox);

	fd = ewl_filedialog_new(fd_win, EWL_FILEDIALOG_TYPE_OPEN,
			report);

  ewl_container_append_child(EWL_CONTAINER(vbox), fd);
  ewl_widget_show(fd);
}

EDJE_CB(playlist_del) {

/* playlist_item_remove should be the callback to use.. not sure why that one doesn't work */

}

void report(Ewl_Widget *row, void *ev_data, void *user_data) {
	Ewl_Fileselector *fs = user_data;
	char *file;
	
	file = ewl_fileselector_get_filename (EWL_FILESELECTOR (fs));
	
	printf("eplayer file open : %s\n", file);
	
	/*
	playlist_load_file(player->playlist, file, 1);
	*/
}
