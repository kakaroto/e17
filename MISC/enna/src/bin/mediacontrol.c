/*
 * MEDIACONTROL.c
 * Copyright (C) Ludovic Restif 2008 <ludovic.restif@gmail.com>
 *
 * MEDIACONTROL.c is free software copyrighted by Ludovic Restif.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name ``Ludovic Restif'' nor the name of any other
 *    contributor may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * MEDIACONTROL.c IS PROVIDED BY Ludovic Restif ``AS IS'' AND ANY EXPRESS
 * OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL Ludovic Restif OR ANY OTHER CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
 * BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/* derived from e_icon */

#include "enna.h"

#define SMART_NAME "enna_MEDIACONTROL"

typedef struct _Smart_Data Smart_Data;

struct _Smart_Data
{
    Evas_Coord x, y, w, h;
    Evas_Object *o_edje;
    Evas_Object *o_play;
    Evas_Object *o_pause;
    Evas_Object *o_prev;
    Evas_Object *o_rewind;
    Evas_Object *o_forward;
    Evas_Object *o_next;
    Evas_Object *o_stop;
    Ecore_Event_Handler *start_event_handler;
    Ecore_Event_Handler *stop_event_handler;
    Ecore_Event_Handler *next_event_handler;
    Ecore_Event_Handler *prev_event_handler;
    Ecore_Event_Handler *pause_event_handler;
    Ecore_Event_Handler *unpause_event_handler;
    Ecore_Event_Handler *seek_event_handler;
    float seek_step; /*percent value*/
};

/* local subsystem functions */
static void _smart_reconfigure(Smart_Data * sd);
static void _smart_init(void);
static void _smart_add(Evas_Object * obj);
static void _smart_del(Evas_Object * obj);
static void _smart_move(Evas_Object * obj, Evas_Coord x, Evas_Coord y);
static void _smart_resize(Evas_Object * obj, Evas_Coord w, Evas_Coord h);
static void _smart_show(Evas_Object * obj);
static void _smart_hide(Evas_Object * obj);
static void _smart_color_set(Evas_Object * obj, int r, int g, int b, int a);
static void _smart_clip_set(Evas_Object * obj, Evas_Object * clip);
static void _smart_clip_unset(Evas_Object * obj);

/*event from Media player*/
static int _start_cb(void *data, int type, void *event);
static int _pause_cb(void *data, int type, void *event);
static int _next_cb(void *data, int type, void *event);
static int _prev_cb(void *data, int type, void *event);
static int _pause_cb(void *data, int type, void *event);
static int _unpause_cb(void *data, int type, void *event);
static int _seek_cb(void *data, int type, void *event);

static void show_play_button(Smart_Data * sd);
static void show_pause_button(Smart_Data * sd);

/* local subsystem globals */
static Evas_Smart *_smart = NULL;
static Enna_Playlist *_enna_playlist;

/* externally accessible functions */
Evas_Object *
enna_mediacontrol_add(Evas * evas,Enna_Playlist *enna_playlist)
{
    _enna_playlist=enna_playlist;
    _smart_init();
    return evas_object_smart_add(evas, _smart);
}

/* Event from mediaplayer*/
static int
_start_cb(void *data, int type, void *event)
{
    show_pause_button(data);
    enna_log(ENNA_MSG_EVENT, NULL, "Media control Event PLAY ");
    return 1;
}

static int
_stop_cb(void *data, int type, void *event)
{
    show_play_button(data);
    enna_log(ENNA_MSG_EVENT, NULL, "Media control Event STOP ");
    return 1;
}

static int
_prev_cb(void *data, int type, void *event)
{
    enna_log(ENNA_MSG_EVENT, NULL, "Media control Event PREV");
    return 1;
}

static int
_next_cb(void *data, int type, void *event)
{
    enna_log(ENNA_MSG_EVENT, NULL, "Media control Event NEXT");
    return 1;
}

static int
_unpause_cb(void *data, int type, void *event)
{
    show_pause_button(data);
    enna_log(ENNA_MSG_EVENT, NULL, "Media control Event UN_PAUSE");
    return 1;
}

static int
_pause_cb(void *data, int type, void *event)
{
    show_play_button(data);
    enna_log(ENNA_MSG_EVENT, NULL, "Media control Event PAUSE ");
    return 1;
}

static int
_seek_cb(void *data, int type, void *event)
{
    Enna_Event_Mediaplayer_Seek_Data *ev;
    ev=event;
    enna_log(ENNA_MSG_EVENT, NULL, "Media control Event SEEK %d%%",(int) (100 * ev->seek_value));
    return 1;
}

/* events from buttons*/
static void
_button_clicked_play_cb(void *data, Evas_Object *obj, void *event_info)
{
    enna_mediaplayer_play(_enna_playlist);
}

static void
_button_clicked_pause_cb(void *data, Evas_Object *obj, void *event_info)
{
    enna_mediaplayer_pause();
}

static void
_button_clicked_prev_cb(void *data, Evas_Object *obj, void *event_info)
{
    enna_mediaplayer_prev(_enna_playlist);
}

static void
_button_clicked_rewind_cb(void *data, Evas_Object *obj, void *event_info)
{
    Smart_Data * sd;
    sd = (Smart_Data *) data;
    double pos;
    double length;
    length = enna_mediaplayer_length_get();
    pos = enna_mediaplayer_position_get();
    enna_mediaplayer_seek((pos/length)-(sd->seek_step/100));
}

static void
_button_clicked_forward_cb(void *data, Evas_Object *obj, void *event_info)
{
    Smart_Data * sd;
    sd = (Smart_Data *) data;
    double pos;
    double length;
    length = enna_mediaplayer_length_get();
    pos = enna_mediaplayer_position_get();
    enna_mediaplayer_seek((pos/length)+(sd->seek_step/100));
}

static void
_button_clicked_next_cb(void *data, Evas_Object *obj, void *event_info)
{
    enna_mediaplayer_next(_enna_playlist);
}

static void
_button_clicked_stop_cb(void *data, Evas_Object *obj, void *event_info)
{
    enna_mediaplayer_stop();
}

static void
show_play_button(Smart_Data * sd)
{
    edje_object_signal_emit(sd->o_edje, "play,show", "enna");
}

static void
show_pause_button(Smart_Data * sd)
{
    edje_object_signal_emit(sd->o_edje, "play,hide", "enna");
}


/* local subsystem globals */
static void
_smart_reconfigure(Smart_Data * sd)
{
    Evas_Coord x, y, w, h;

    x = sd->x;
    y = sd->y;
    w = sd->w;
    h = sd->h;

    evas_object_move(sd->o_edje, x, y);
    evas_object_resize(sd->o_edje, w, h);

}

static void
_smart_init(void)
{
    if (_smart)
        return;
    static const Evas_Smart_Class sc =
    {
            SMART_NAME,
            EVAS_SMART_CLASS_VERSION,
            _smart_add,
            _smart_del,
            _smart_move,
            _smart_resize,
            _smart_show,
            _smart_hide,
            _smart_color_set,
            _smart_clip_set,
            _smart_clip_unset,
            NULL,
            NULL };
    _smart = evas_smart_class_new(&sc);
}

static void
_smart_add(Evas_Object * obj)
{
    Evas_Object *o;
    Smart_Data *sd;
    Evas *evas;
    sd = calloc(1, sizeof(Smart_Data));
    if (!sd)
        return;
    sd->x = 0;
    sd->y = 0;
    sd->w = 0;
    sd->h = 0;
    sd->seek_step = 2;
    evas=evas_object_evas_get(obj);

    sd->o_edje = edje_object_add(evas);
    edje_object_file_set(sd->o_edje, enna_config_theme_get(), "mediacontrol");
    evas_object_show(sd->o_edje);

    o= enna_button_add(evas);
    enna_button_icon_set(o, "icon/mp_play");
    edje_object_part_swallow(sd->o_edje,"mediacontrol.swallow.play",o);
    evas_object_smart_callback_add(o, "clicked", _button_clicked_play_cb, sd);
    sd->o_play=o;

    o = enna_button_add(evas);
    enna_button_icon_set(o, "icon/mp_pause");
    edje_object_part_swallow(sd->o_edje,"mediacontrol.swallow.pause",o);
    evas_object_smart_callback_add(o, "clicked", _button_clicked_pause_cb, sd);
    sd->o_pause=o;

    o = enna_button_add(evas);
    enna_button_icon_set(o, "icon/mp_prev");
    edje_object_part_swallow(sd->o_edje,"mediacontrol.swallow.prev",o);
    evas_object_smart_callback_add(o, "clicked", _button_clicked_prev_cb, sd);
    sd->o_prev=o;

    o = enna_button_add(evas);
    enna_button_icon_set(o, "icon/mp_rewind");
    edje_object_part_swallow(sd->o_edje,"mediacontrol.swallow.rewind",o);
    evas_object_smart_callback_add(o, "clicked", _button_clicked_rewind_cb, sd);
    sd->o_rewind=o;

    o = enna_button_add(evas);
    enna_button_icon_set(o, "icon/mp_forward");
    edje_object_part_swallow(sd->o_edje,"mediacontrol.swallow.forward",o);
    evas_object_smart_callback_add(o, "clicked", _button_clicked_forward_cb, sd);
    sd->o_forward=o;

    o = enna_button_add(evas);
    enna_button_icon_set(o, "icon/mp_next");
    edje_object_part_swallow(sd->o_edje,"mediacontrol.swallow.next",o);
    evas_object_smart_callback_add(o, "clicked", _button_clicked_next_cb, sd);
    sd->o_next=o;

    o = enna_button_add(evas);
    enna_button_icon_set(o, "icon/mp_stop");
    edje_object_part_swallow(sd->o_edje,"mediacontrol.swallow.stop",o);
    evas_object_smart_callback_add(o, "clicked", _button_clicked_stop_cb, sd);
    sd->o_stop=o;

    sd->start_event_handler = ecore_event_handler_add(
             ENNA_EVENT_MEDIAPLAYER_START, _start_cb, sd);
    sd->stop_event_handler = ecore_event_handler_add(
             ENNA_EVENT_MEDIAPLAYER_STOP, _stop_cb, sd);
    sd->prev_event_handler = ecore_event_handler_add(
             ENNA_EVENT_MEDIAPLAYER_PREV, _prev_cb, sd);
    sd->next_event_handler = ecore_event_handler_add(
            ENNA_EVENT_MEDIAPLAYER_NEXT, _next_cb, sd);
    sd->pause_event_handler = ecore_event_handler_add(
            ENNA_EVENT_MEDIAPLAYER_PAUSE, _pause_cb, sd);
    sd->unpause_event_handler = ecore_event_handler_add(
            ENNA_EVENT_MEDIAPLAYER_UNPAUSE, _unpause_cb, sd);
    sd->seek_event_handler = ecore_event_handler_add(
            ENNA_EVENT_MEDIAPLAYER_SEEK, _seek_cb, sd);
    if (enna_mediaplayer_state_get() == PLAYING)
        show_pause_button(sd);
    else
        show_play_button(sd);

    evas_object_smart_member_add(sd->o_edje, obj);
    evas_object_smart_data_set(obj, sd);
}

static void
_smart_del(Evas_Object * obj)
{
    Smart_Data *sd;

    sd = evas_object_smart_data_get(obj);
    if (!sd)
        return;

    ecore_event_handler_del(sd->start_event_handler);
    ecore_event_handler_del(sd->stop_event_handler);
    ecore_event_handler_del(sd->next_event_handler);
    ecore_event_handler_del(sd->prev_event_handler);
    ecore_event_handler_del(sd->pause_event_handler);
    ecore_event_handler_del(sd->unpause_event_handler);
    ecore_event_handler_del(sd->seek_event_handler);
    evas_object_del(sd->o_play);
    evas_object_del(sd->o_pause);
    evas_object_del(sd->o_prev);
    evas_object_del(sd->o_rewind);
    evas_object_del(sd->o_forward);
    evas_object_del(sd->o_next);
    evas_object_del(sd->o_stop);
    evas_object_del(sd->o_edje);
    free(sd);
}

static void
_smart_move(Evas_Object * obj, Evas_Coord x, Evas_Coord y)
{
    Smart_Data *sd;

    sd = evas_object_smart_data_get(obj);
    if (!sd)
        return;
    if ((sd->x == x) && (sd->y == y))
        return;
    sd->x = x;
    sd->y = y;
    _smart_reconfigure(sd);
}

static void
_smart_resize(Evas_Object * obj, Evas_Coord w, Evas_Coord h)
{
    Smart_Data *sd;

    sd = evas_object_smart_data_get(obj);
    if (!sd)
        return;
    if ((sd->w == w) && (sd->h == h))
        return;
    sd->w = w;
    sd->h = h;
    _smart_reconfigure(sd);
}

static void
_smart_show(Evas_Object * obj)
{
    Smart_Data *sd;

    sd = evas_object_smart_data_get(obj);
    if (!sd)
        return;
    evas_object_show(sd->o_edje);
}

static void
_smart_hide(Evas_Object * obj)
{
    Smart_Data *sd;

    sd = evas_object_smart_data_get(obj);
    if (!sd)
        return;
    evas_object_hide(sd->o_edje);

}

static void
_smart_color_set(Evas_Object * obj, int r, int g, int b, int a)
{
    Smart_Data *sd;

    sd = evas_object_smart_data_get(obj);
    if (!sd)
        return;
    evas_object_color_set(sd->o_edje, r, g, b, a);
}

static void
_smart_clip_set(Evas_Object * obj, Evas_Object * clip)
{
    Smart_Data *sd;

    sd = evas_object_smart_data_get(obj);
    if (!sd)
        return;
    evas_object_clip_set(sd->o_edje, clip);
}

static void
_smart_clip_unset(Evas_Object * obj)
{
    Smart_Data *sd;

    sd = evas_object_smart_data_get(obj);
    if (!sd)
        return;
    evas_object_clip_unset(sd->o_edje);
}
