#ifndef __CALLBACKS_H
#define __CALLBACKS_H

#include <Evas.h>

#define EDJE_CB(name) \
	void cb_##name(ePlayer *player, Evas_Object *obj, \
	               const char *emission, const char *src)

EDJE_CB(play);
EDJE_CB(pause);
EDJE_CB(stop);

EDJE_CB(seek_forward);
EDJE_CB(seek_backward);

EDJE_CB(seek_forward_start);
EDJE_CB(seek_forward_stop);
EDJE_CB(seek_backward_start);
EDJE_CB(seek_backward_stop);

EDJE_CB(track_next);
EDJE_CB(track_prev);

EDJE_CB(time_display_toggle);
EDJE_CB(repeat_mode_toggle);

EDJE_CB(volume_raise);
EDJE_CB(volume_lower);

EDJE_CB(playlist_scroll_up);
EDJE_CB(playlist_scroll_down);

EDJE_CB(playlist_item_play);
EDJE_CB(playlist_item_remove);
EDJE_CB(playlist_item_selected);

EDJE_CB(eplayer_quit);
EDJE_CB(eplayer_raise);
EDJE_CB(switch_group);

EDJE_CB(update_seeker);

#endif

