/*
 * enna_background.c
 * Copyright (C) Nicolas Aguirre 2006,2007,2008 <aguirre.nicolas@gmail.com>
 *
 * enna_background.c is free software copyrighted by Nicolas Aguirre.
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
 * enna_background.c IS PROVIDED BY Nicolas Aguirre ``AS IS'' AND ANY EXPRESS
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

#include "background.h"
#include "enna.h"

#define SMART_NAME "enna_background"

typedef struct _smart_Data E_Smart_Data;

struct _smart_Data
{
    Evas_Coord x, y, w, h;
    Evas_Object *obj;
    Evas_Object *o_bg;
    Evas_Object *o_edje;
};

/* local subsystem functions */
static void _enna_background_smart_reconfigure(E_Smart_Data * sd);
static void _enna_background_smart_init(void);
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
static Evas_Smart *_e_smart = NULL;

/* externally accessible functions */
Evas_Object *
enna_background_add(Evas * evas)
{
    _enna_background_smart_init();
    return evas_object_smart_add(evas, _e_smart);
}

/* local subsystem globals */
static void _enna_background_smart_reconfigure(E_Smart_Data * sd)
{
    evas_object_move(sd->o_bg, sd->x, sd->y);
    evas_object_resize(sd->o_bg, sd->w, sd->h);
    evas_object_move(sd->o_edje, sd->x, sd->y);
    evas_object_resize(sd->o_edje, sd->w, sd->h);

}

static void _enna_background_smart_init(void)
{
    if (_e_smart)
        return;
    static const Evas_Smart_Class sc =
    { SMART_NAME, EVAS_SMART_CLASS_VERSION, _smart_add, _smart_del,
            _smart_move, _smart_resize, _smart_show, _smart_hide,
            _smart_color_set, _smart_clip_set, _smart_clip_unset, NULL,
            NULL };
    _e_smart = evas_smart_class_new(&sc);
}

static void _smart_add(Evas_Object * obj)
{
    E_Smart_Data *sd;
    Evas *e;

    sd = calloc(1, sizeof(E_Smart_Data));
    if (!sd)
        return;

    e = evas_object_evas_get(obj);

    sd->x = 0;
    sd->y = 0;
    sd->w = 0;
    sd->h = 0;

    sd->o_bg = evas_object_rectangle_add(e);
    evas_object_color_set(sd->o_bg, 0, 0, 0, 255);
    evas_object_layer_set(sd->o_bg, 0);
    evas_object_show(sd->o_bg);

    sd->o_edje = edje_object_add(e);
    edje_object_file_set(sd->o_edje, enna_config_theme_get(), "enna/background");

    evas_object_smart_member_add(sd->o_bg, obj);
    evas_object_smart_member_add(sd->o_edje, obj);
    evas_object_smart_data_set(obj, sd);
}

static void _smart_del(Evas_Object * obj)
{
    E_Smart_Data *sd;

    sd = evas_object_smart_data_get(obj);
    if (!sd)
        return;
    evas_object_del(sd->o_bg);
    evas_object_del(sd->o_edje);
    free(sd);
}

static void _smart_move(Evas_Object * obj, Evas_Coord x, Evas_Coord y)
{
    E_Smart_Data *sd;

    sd = evas_object_smart_data_get(obj);
    if (!sd)
        return;
    if ((sd->x == x) && (sd->y == y))
        return;
    sd->x = x;
    sd->y = y;
    _enna_background_smart_reconfigure(sd);
}

static void _smart_resize(Evas_Object * obj, Evas_Coord w, Evas_Coord h)
{
    E_Smart_Data *sd;

    sd = evas_object_smart_data_get(obj);
    if (!sd)
        return;
    if ((sd->w == w) && (sd->h == h))
        return;
    sd->w = w;
    sd->h = h;
    _enna_background_smart_reconfigure(sd);
}

static void _smart_show(Evas_Object * obj)
{
    E_Smart_Data *sd;

    sd = evas_object_smart_data_get(obj);
    if (!sd)
        return;
    evas_object_show(sd->o_bg);
    evas_object_show(sd->o_edje);
}

static void _smart_hide(Evas_Object * obj)
{
    E_Smart_Data *sd;

    sd = evas_object_smart_data_get(obj);
    if (!sd)
        return;
    evas_object_hide(sd->o_bg);
    evas_object_hide(sd->o_edje);

}

static void _smart_color_set(Evas_Object * obj, int r, int g, int b, int a)
{
    E_Smart_Data *sd;

    sd = evas_object_smart_data_get(obj);
    if (!sd)
        return;
}

static void _smart_clip_set(Evas_Object * obj, Evas_Object * clip)
{
    E_Smart_Data *sd;

    sd = evas_object_smart_data_get(obj);
    if (!sd)
        return;
    evas_object_clip_set(sd->o_bg, clip);
    evas_object_clip_set(sd->o_edje, clip);
}

static void _smart_clip_unset(Evas_Object * obj)
{
    E_Smart_Data *sd;

    sd = evas_object_smart_data_get(obj);
    if (!sd)
        return;
    evas_object_clip_unset(sd->o_bg);
    evas_object_clip_unset(sd->o_edje);
}
