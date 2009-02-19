/*
 * smart_mediaplayer.c
 * Copyright (C) Nicolas Aguirre 2006,2007,2008 <aguirre.nicolas@gmail.com>
 *
 * smart_mediaplayer.c is free software copyrighted by Nicolas Aguirre.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name ``Nicolas Aguirre'' nor the name of any other
 *    contributor may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * smart_mediaplayer.c IS PROVIDED BY Nicolas Aguirre ``AS IS'' AND ANY EXPRESS
 * OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL Nicolas Aguirre OR ANY OTHER CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
 * BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/* derived from e_icon */

#include "smart_player.h"
#include "mediacontrol.h"

#define SMART_NAME "smart_mediaplayer"

typedef struct _Smart_Data Smart_Data;

struct _Smart_Data
{
    Evas_Coord x, y, w, h;
    Evas_Object *o_edje;
    Evas_Object *o_cover;
    Evas_Object *o_mediacontrol;
};

/* local subsystem functions */
static void _enna_mediaplayer_smart_reconfigure(Smart_Data * sd);
static void _enna_mediaplayer_smart_init(void);
static void _smart_add(Evas_Object * obj);
static void _smart_del(Evas_Object * obj);
static void _smart_move(Evas_Object * obj, Evas_Coord x, Evas_Coord y);
static void _smart_resize(Evas_Object * obj, Evas_Coord w, Evas_Coord h);
static void _smart_show(Evas_Object * obj);
static void _smart_hide(Evas_Object * obj);
static void _smart_color_set(Evas_Object * obj, int r, int g, int b, int a);
static void _smart_clip_set(Evas_Object * obj, Evas_Object * clip);
static void _smart_clip_unset(Evas_Object * obj);

/* local subsystem globals */
static Evas_Smart *_smart = NULL;
static Enna_Playlist *_enna_playlist;

static void _drag_bar_seek_cb(void *data, Evas_Object *obj,
        const char *emission, const char *source)
{
    double value;
    edje_object_part_drag_value_get(obj, "enna.dragable.pos", &value, NULL);
    enna_mediaplayer_seek(value);
}

/* externally accessible functions */
Evas_Object *
enna_smart_player_add(Evas * evas, Enna_Playlist *enna_playlist)
{
    _enna_playlist = enna_playlist;
    _enna_mediaplayer_smart_init();
    return evas_object_smart_add(evas, _smart);
}

void enna_smart_player_position_set(Evas_Object *obj, double pos,
        double len)
{
    long ph, pm, ps, lh, lm, ls;
    double fraction;
    char buf[256];
    char buf2[256];
    API_ENTRY
    ;

    lh = len / 3600000;
    lm = len / 60 - (lh * 60);
    ls = len - (lm * 60);
    ph = pos / 3600;
    pm = pos / 60 - (ph * 60);
    ps = pos - (pm * 60);
    snprintf(buf, sizeof(buf), "%02li:%02li", pm, ps);
    snprintf(buf2, sizeof(buf2), "%02li:%02li", lm, ls);
    if (len)
        fraction = pos / len;
    else
        fraction = 0.0;

    edje_object_part_text_set(sd->o_edje, "enna.text.length", buf2);
    edje_object_part_text_set(sd->o_edje, "enna.text.position", buf);
    edje_object_part_drag_value_set(sd->o_edje, "enna.dragable.pos", fraction,
            0.0);
}

void enna_smart_player_metadata_set(Evas_Object *obj,
        Enna_Metadata *metadata)
{
    Enna_Metadata *meta;

    API_ENTRY;

    if (!metadata)
        return;
    edje_object_part_text_set(sd->o_edje, "enna.text.title",
                              metadata->title ? metadata->title : "");
    edje_object_part_text_set(sd->o_edje, "enna.text.album",
                              metadata->music->album ? metadata->music->album : "");
    edje_object_part_text_set(sd->o_edje, "enna.text.artist",
                              metadata->music->artist ? metadata->music->artist : "");

    meta = enna_metadata_new (metadata->uri);
    enna_metadata_add_keywords (meta, metadata->music->artist);
    enna_metadata_add_keywords (meta, metadata->music->album);
    enna_metadata_grab (meta, ENNA_GRABBER_CAP_AUDIO | ENNA_GRABBER_CAP_COVER);

    ENNA_OBJECT_DEL(sd->o_cover);
    if (meta && meta->cover)
    {
        /* FIXME : add edje cb at end of cover transition to switch properly covers*/

//	sd->o_cover = enna_reflection_add(evas_object_evas_get(sd->o_edje));
        sd->o_cover = enna_image_add(evas_object_evas_get(sd->o_edje));
	//enna_reflection_fill_inside_set(sd->o_cover, 0);
	enna_image_fill_inside_set(sd->o_cover, 0);
        //enna_reflection_file_set(sd->o_cover, meta->cover);
	enna_image_file_set(sd->o_cover, meta->cover);
        edje_object_part_swallow(sd->o_edje, "enna.swallow.cover", sd->o_cover);
        edje_object_signal_emit(sd->o_edje, "cover,show", "enna");

    }
    else
    {
	sd->o_cover = edje_object_add(evas_object_evas_get(sd->o_edje));
	edje_object_file_set(sd->o_cover, enna_config_theme_get(), "icon/unknown_cover");
        edje_object_part_swallow(sd->o_edje, "enna.swallow.cover", sd->o_cover);
        edje_object_signal_emit(sd->o_edje, "cover,show", "enna");
    }
    enna_metadata_free(metadata);
    enna_metadata_free(meta);

}

/* local subsystem globals */
static void _enna_mediaplayer_smart_reconfigure(Smart_Data * sd)
{
    Evas_Coord x, y, w, h;

    x = sd->x;
    y = sd->y;
    w = sd->w;
    h = sd->h;

    evas_object_move(sd->o_edje, x, y);
    evas_object_resize(sd->o_edje, w, h);

}

static void _enna_mediaplayer_smart_init(void)
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
	NULL
    };
    _smart = evas_smart_class_new(&sc);
}

static void _smart_add(Evas_Object * obj)
{
    Smart_Data *sd;

    sd = calloc(1, sizeof(Smart_Data));
    if (!sd)
        return;
    sd->o_edje = edje_object_add(evas_object_evas_get(obj));
    edje_object_file_set(sd->o_edje, enna_config_theme_get(), "mediaplayer");
    sd->x = 0;
    sd->y = 0;
    sd->w = 0;
    sd->h = 0;
    sd->o_mediacontrol=enna_mediacontrol_add(evas_object_evas_get(sd->o_edje),_enna_playlist);
    edje_object_part_swallow(sd->o_edje, "enna.swallow.mediacontrol", sd->o_mediacontrol);

    evas_object_smart_member_add(sd->o_mediacontrol, obj);
    evas_object_smart_member_add(sd->o_edje, obj);
    evas_object_smart_data_set(obj, sd);
    edje_object_signal_callback_add(sd->o_edje, "drag", "enna.dragable.pos",
            _drag_bar_seek_cb, NULL);
}

static void _smart_del(Evas_Object * obj)
{
    Smart_Data *sd;

    sd = evas_object_smart_data_get(obj);
    if (!sd)
        return;
    ENNA_OBJECT_DEL(sd->o_mediacontrol);
    ENNA_OBJECT_DEL(sd->o_cover);
    ENNA_OBJECT_DEL(sd->o_edje);
    free(sd);
}

static void _smart_move(Evas_Object * obj, Evas_Coord x, Evas_Coord y)
{
    Smart_Data *sd;

    sd = evas_object_smart_data_get(obj);
    if (!sd)
        return;
    if ((sd->x == x) && (sd->y == y))
        return;
    sd->x = x;
    sd->y = y;
    _enna_mediaplayer_smart_reconfigure(sd);
}

static void _smart_resize(Evas_Object * obj, Evas_Coord w, Evas_Coord h)
{
    Smart_Data *sd;

    sd = evas_object_smart_data_get(obj);
    if (!sd)
        return;
    if ((sd->w == w) && (sd->h == h))
        return;
    sd->w = w;
    sd->h = h;
    _enna_mediaplayer_smart_reconfigure(sd);
}

static void _smart_show(Evas_Object * obj)
{
    Smart_Data *sd;

    sd = evas_object_smart_data_get(obj);
    if (!sd)
        return;
    evas_object_show(sd->o_edje);
}

static void _smart_hide(Evas_Object * obj)
{
    Smart_Data *sd;

    sd = evas_object_smart_data_get(obj);
    if (!sd)
        return;
    evas_object_hide(sd->o_edje);
}

static void _smart_color_set(Evas_Object * obj, int r, int g, int b, int a)
{
    Smart_Data *sd;

    sd = evas_object_smart_data_get(obj);
    if (!sd)
        return;
    evas_object_color_set(sd->o_edje, r, g, b, a);
}

static void _smart_clip_set(Evas_Object * obj, Evas_Object * clip)
{
    Smart_Data *sd;

    sd = evas_object_smart_data_get(obj);
    if (!sd)
        return;
    evas_object_clip_set(sd->o_edje, clip);
}

static void _smart_clip_unset(Evas_Object * obj)
{
    Smart_Data *sd;

    sd = evas_object_smart_data_get(obj);
    if (!sd)
        return;
    evas_object_clip_unset(sd->o_edje);
}
