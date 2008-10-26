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

#define ENNA_MODULE_NAME "video"

#define SMART_NAME "smart_player"

#define API_ENTRY \
   E_Smart_Data *sd; \
   sd = evas_object_smart_data_get(obj); \
   if ((!obj) || (!sd) || \
     (evas_object_type_get(obj) && \
     strcmp(evas_object_type_get(obj), SMART_NAME)))

#define INTERNAL_ENTRY \
   E_Smart_Data *sd; \
   sd = evas_object_smart_data_get(obj); \
   if (!sd) \
      return;

typedef struct _E_Smart_Data E_Smart_Data;

struct _E_Smart_Data
{
    Evas_Coord x, y, w, h;
    Evas_Object *o_edje;
    Evas_Object *o_snapshot;
    Evas_Object *o_snapshot_old;
    Evas_Object *o_cover;
    Evas_Object *o_cover_old;
    Evas_Object *o_fs;
};

/* local subsystem functions */
static void _enna_mediaplayer_smart_reconfigure(E_Smart_Data * sd);
static void _enna_mediaplayer_smart_init(void);
static void _e_smart_add(Evas_Object * obj);
static void _e_smart_del(Evas_Object * obj);
static void _e_smart_move(Evas_Object * obj, Evas_Coord x, Evas_Coord y);
static void _e_smart_resize(Evas_Object * obj, Evas_Coord w, Evas_Coord h);
static void _e_smart_show(Evas_Object * obj);
static void _e_smart_hide(Evas_Object * obj);
static void _e_smart_color_set(Evas_Object * obj, int r, int g, int b, int a);
static void _e_smart_clip_set(Evas_Object * obj, Evas_Object * clip);
static void _e_smart_clip_unset(Evas_Object * obj);

/* local subsystem globals */
static Evas_Smart *_e_smart = NULL;

/* externally accessible functions */
EAPI Evas_Object *
enna_smart_player_add(Evas * evas)
{
    _enna_mediaplayer_smart_init();
    return evas_object_smart_add(evas, _e_smart);
}

EAPI void enna_smart_player_show_video(Evas_Object *obj)
{
    Evas_Object *o_video;

    API_ENTRY
    ;

    o_video = enna_mediaplayer_video_obj_get();
    if (o_video)
        edje_object_part_swallow(enna->o_edje, "enna.swallow.fullscreen",
                o_video);

}

EAPI void enna_smart_player_hide_video(Evas_Object *obj)
{
    Evas_Object *o_video;

    API_ENTRY
    ;

    o_video = enna_mediaplayer_video_obj_get();
    if (o_video)
    {
        edje_object_part_unswallow(enna->o_edje, o_video);
        evas_object_hide(o_video);
    }
}

EAPI void enna_smart_player_snapshot_set(Evas_Object *obj,
                                         Enna_Metadata *metadata)
{
    char *snap_file = NULL;

    API_ENTRY;

    snap_file = enna_snapshot_get(metadata->uri);
    if (snap_file)
    {
        Evas_Coord ow,oh;
        enna_log(ENNA_MSG_INFO, ENNA_MODULE_NAME, "snapshot filename : %s", snap_file);

        /* FIXME : add edje cb at end of snapshot transition to switch properly snapshots*/
        sd->o_snapshot_old = sd->o_snapshot;
        sd->o_snapshot = enna_image_add(evas_object_evas_get(sd->o_edje));
        evas_object_show(sd->o_snapshot);

        /* Stretch image to fit the parent container */
        enna_image_fill_inside_set(sd->o_snapshot, 0);

        enna_image_file_set(sd->o_snapshot, snap_file);
        /* Full definition for image loaded */
        enna_image_size_get(sd->o_snapshot, &ow, &oh);
        enna_image_load_size_set(sd->o_snapshot, ow, oh);

        edje_object_part_swallow(sd->o_edje,
                                 "enna.swallow.snapshot", sd->o_snapshot);
        edje_object_signal_emit(sd->o_edje, "snapshot,show", "enna");
        evas_object_del(sd->o_snapshot_old);
    }
    else
    {
        edje_object_signal_emit(sd->o_edje, "snapshot,hide", "enna");
        evas_object_del(sd->o_cover);
    }
}

EAPI void enna_smart_player_cover_set(Evas_Object *obj,
                                      Enna_Metadata *metadata)
{
    char *cover_file = NULL;

    API_ENTRY;

    cover_file = enna_cover_video_get(metadata->uri);
    if (cover_file)
    {
        Evas_Coord ow,oh;

        enna_log(ENNA_MSG_INFO, ENNA_MODULE_NAME, "cover filename : %s",
                cover_file);
        /* FIXME : add edje cb at end of cover transition to switch properly covers*/
        sd->o_cover_old = sd->o_cover;

        sd->o_cover = enna_image_add(evas_object_evas_get(sd->o_edje));
        enna_image_file_set(sd->o_cover, cover_file);
        enna_image_size_get(sd->o_cover, &ow, &oh);
        enna_image_load_size_set(sd->o_cover, ow, oh);
        enna_image_fill_inside_set(sd->o_cover, 0);
        edje_object_part_swallow(sd->o_edje, "enna.swallow.cover", sd->o_cover);
        edje_object_signal_emit(sd->o_edje, "cover,show", "enna");
        evas_object_del(sd->o_cover_old);
    }
    else
    {
        edje_object_signal_emit(sd->o_edje, "cover,hide", "enna");
        evas_object_del(sd->o_cover);
    }
}

EAPI void enna_smart_player_metadata_set(Evas_Object *obj,
        Enna_Metadata *metadata)
{
    char buf[4096];
    int h, mn, sec;
    float len;

    API_ENTRY
    ;

    enna_log(ENNA_MSG_INFO, ENNA_MODULE_NAME, "metadata set");

    if (!metadata)
        return;
    if (metadata->title)
        edje_object_part_text_set(sd->o_edje, "enna.text.title",
                ecore_file_file_get(metadata->title));
    else if (metadata->uri)
        edje_object_part_text_set(sd->o_edje, "enna.text.title",
                ecore_file_file_get(metadata->uri));

    snprintf(buf, sizeof(buf), "Size : %.2f MB", metadata->size / 1024.0
            / 1024.0);
    edje_object_part_text_set(sd->o_edje, "enna.text.size", buf);

    len = metadata->length / 1000.0;
    h = (int) (len / 3600);
    mn = (int) ((len - (h * 3600)) / 60);
    sec = (int) (len - (h * 3600) - (mn * 60));
    snprintf(buf, sizeof(buf), "Length: %.2d h %.2d mn %.2d sec", h, mn, sec);
    edje_object_part_text_set(sd->o_edje, "enna.text.length", buf);

    snprintf(buf, sizeof(buf), "Codec : %s", metadata->video->codec);
    edje_object_part_text_set(sd->o_edje, "enna.text.videocodec", buf);

    snprintf(buf, sizeof(buf), "Size : %dx%d", metadata->video->width,
            metadata->video->height);
    edje_object_part_text_set(sd->o_edje, "enna.text.videosize", buf);

    snprintf(buf, sizeof(buf), "Framerate : %.2f fps",
             metadata->video->framerate);
    edje_object_part_text_set(sd->o_edje, "enna.text.framerate", buf);

    snprintf(buf, sizeof(buf), "Codec : %s", metadata->music->codec);
    edje_object_part_text_set(sd->o_edje, "enna.text.audiocodec", buf);

    snprintf(buf, sizeof(buf), "Bitrate : %i kbps", metadata->music->bitrate
            / 1000);
    edje_object_part_text_set(sd->o_edje, "enna.text.bitrate", buf);

    snprintf(buf, sizeof(buf), "Samplerate : %i Hz",
            metadata->music->samplerate);
    edje_object_part_text_set(sd->o_edje, "enna.text.samplerate", buf);
}

/* local subsystem globals */
static void _enna_mediaplayer_smart_reconfigure(E_Smart_Data * sd)
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
    if (_e_smart)
        return;
    static const Evas_Smart_Class sc =
    { SMART_NAME, EVAS_SMART_CLASS_VERSION, _e_smart_add, _e_smart_del,
            _e_smart_move, _e_smart_resize, _e_smart_show, _e_smart_hide,
            _e_smart_color_set, _e_smart_clip_set, _e_smart_clip_unset, NULL };
    _e_smart = evas_smart_class_new(&sc);
}

static void _e_smart_add(Evas_Object * obj)
{
    E_Smart_Data *sd;

    sd = calloc(1, sizeof(E_Smart_Data));
    if (!sd)
        return;
    sd->o_edje = edje_object_add(evas_object_evas_get(obj));
    edje_object_file_set(sd->o_edje, enna_config_theme_get(), "video_info");
    sd->x = 0;
    sd->y = 0;
    sd->w = 0;

    sd->h = 0;
    evas_object_smart_member_add(sd->o_edje, obj);
    evas_object_smart_data_set(obj, sd);

}

static void _e_smart_del(Evas_Object * obj)
{
    E_Smart_Data *sd;
    sd = evas_object_smart_data_get(obj);
    if (!sd)
        return;
    ENNA_OBJECT_DEL(sd->o_snapshot);
    ENNA_OBJECT_DEL(sd->o_snapshot_old);
    ENNA_OBJECT_DEL(sd->o_cover);
    ENNA_OBJECT_DEL(sd->o_cover_old);
    ENNA_OBJECT_DEL(sd->o_edje);
    free(sd);
}

static void _e_smart_move(Evas_Object * obj, Evas_Coord x, Evas_Coord y)
{
    E_Smart_Data *sd;

    sd = evas_object_smart_data_get(obj);
    if (!sd)
        return;
    if ((sd->x == x) && (sd->y == y))
        return;
    sd->x = x;
    sd->y = y;
    _enna_mediaplayer_smart_reconfigure(sd);
}

static void _e_smart_resize(Evas_Object * obj, Evas_Coord w, Evas_Coord h)
{
    E_Smart_Data *sd;

    sd = evas_object_smart_data_get(obj);
    if (!sd)
        return;
    if ((sd->w == w) && (sd->h == h))
        return;
    sd->w = w;
    sd->h = h;
    _enna_mediaplayer_smart_reconfigure(sd);
    enna_mediaplayer_video_resize(0, 0, 0, 0);
}

static void _e_smart_show(Evas_Object * obj)
{
    E_Smart_Data *sd;

    sd = evas_object_smart_data_get(obj);
    if (!sd)
        return;
    evas_object_show(sd->o_edje);
}

static void _e_smart_hide(Evas_Object * obj)
{
    E_Smart_Data *sd;

    sd = evas_object_smart_data_get(obj);
    if (!sd)
        return;
    evas_object_hide(sd->o_edje);
}

static void _e_smart_color_set(Evas_Object * obj, int r, int g, int b, int a)
{
    E_Smart_Data *sd;

    sd = evas_object_smart_data_get(obj);
    if (!sd)
        return;
    evas_object_color_set(sd->o_edje, r, g, b, a);
}

static void _e_smart_clip_set(Evas_Object * obj, Evas_Object * clip)
{
    E_Smart_Data *sd;

    sd = evas_object_smart_data_get(obj);
    if (!sd)
        return;
    evas_object_clip_set(sd->o_edje, clip);
}

static void _e_smart_clip_unset(Evas_Object * obj)
{
    E_Smart_Data *sd;

    sd = evas_object_smart_data_get(obj);
    if (!sd)
        return;
    evas_object_clip_unset(sd->o_edje);
}
