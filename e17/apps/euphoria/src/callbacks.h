#ifndef __CALLBACKS_H
#define __CALLBACKS_H

/*
 * $Id$
 * vim:noexpandtab:sw=4:sts=4:ts=4
 */

#include <Evas.h>
#include <Ewl.h>

typedef void (*EdjeCb)(void *udata, Evas_Object *o,
                       const char *emission, const char *src);
typedef void (*XmmsCb)(void *udata, void *arg);

struct _Euphoria;

#define EDJE_CB(name) \
	void on_edje_##name(struct _Euphoria *e, Evas_Object *obj, \
	                    const char *emission, const char *src)

#define XMMS_CB(name) \
	void on_xmms_##name(struct _Euphoria *e, void *arg)

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

EDJE_CB(euphoria_quit);
EDJE_CB(euphoria_close);
EDJE_CB(switch_group);

EDJE_CB(update_seeker);

EDJE_CB(playlist_add);
EDJE_CB(playlist_del);
EDJE_CB(playlist_shuffle);
EDJE_CB(playlist_clear);

void cb_key_press(void *data, Evas *e, Evas_Object *obj, void *event_info);
void cb_key_release(void *data, Evas *e, Evas_Object *obj, void *event_info);

XMMS_CB(playback_status);
XMMS_CB(playback_playtime);
XMMS_CB(playback_currentid);
XMMS_CB(playlist_mediainfo);
XMMS_CB(playlist_mediainfo_id);
XMMS_CB(playlist_list);
XMMS_CB(playlist_add);
XMMS_CB(playlist_remove);
XMMS_CB(playlist_clear);
XMMS_CB(playlist_shuffle);
XMMS_CB(visdata);

#endif

