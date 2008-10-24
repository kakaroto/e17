/*
 * slideshow.c
 * Copyright (C) Nicolas Aguirre 2006,2007,2008 <aguirre.nicolas@gmail.com>
 *
 * slideshow.c is free software copyrighted by Nicolas Aguirre.
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
 * slideshow.c IS PROVIDED BY Nicolas Aguirre ``AS IS'' AND ANY EXPRESS
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

#include "slideshow.h"

#define SMART_NAME "slideshow"

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

#define NB_TRANSITIONS_MAX 3.0

#define STOP 0
#define PLAY 1
#define PAUSE 2

typedef struct _E_Smart_Data E_Smart_Data;

struct _E_Smart_Data
{
    Evas_Coord x, y, w, h;
    Evas_Object *o_edje;
    Evas_Object *o_transition;
    Evas_Object *obj;
    Eina_List *playlist;
    unsigned int playlist_id;
    Ecore_Timer *timer;
    Evas_Object *old_slide;
    Evas_Object *slide;
    unsigned char state;
};

/* local subsystem functions */
static void _enna_slideshow_smart_reconfigure(E_Smart_Data * sd);
static void _enna_slideshow_smart_init(void);
static void _random_transition(E_Smart_Data *sd);
static void _edje_cb(void *data, Evas_Object *obj, const char *emission,
        const char *source);
static void _switch_images(E_Smart_Data * sd, Evas_Object * new_slide);
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
enna_slideshow_add(Evas * evas)
{
    _enna_slideshow_smart_init();
    return evas_object_smart_add(evas, _e_smart);
}

EAPI void enna_slideshow_image_append(Evas_Object *obj, const char *filename)
{
    Evas_Object *o;
    Evas_Coord w, h;

    API_ENTRY
    return;

    if (!filename)
        return;

    o = enna_image_add(evas_object_evas_get(obj));
    enna_log(ENNA_MSG_EVENT, NULL, "append : %s", filename);
    enna_image_file_set(o, filename+7);
    enna_image_size_get(o, &w, &h);
    enna_image_load_size_set(o, w, h);

    sd->playlist = eina_list_append(sd->playlist, o);

}

EAPI int enna_slideshow_next(void *data)
{
    Evas_Object *o;
    Evas_Object * obj = (Evas_Object *) data;

    API_ENTRY
    return 0;

    if (sd->old_slide)
        return 1;

    sd->playlist_id++;
    o = eina_list_nth(sd->playlist, sd->playlist_id);

    if (o)
    {
        _switch_images(sd, o);
        return 1;
    }
    else
    {
        sd->playlist_id--;
        return 0;
    }
    return 0;
}

EAPI int enna_slideshow_prev(void *data)
{
    Evas_Object *o;
    Evas_Object * obj = (Evas_Object *) data;

    API_ENTRY
    return 0;

    if (sd->old_slide)
        return 1;

    sd->playlist_id--;
    o = eina_list_nth(sd->playlist, sd->playlist_id);

    if (o)
    {
        if (sd->state == PLAY)
        {
            sd->state = PAUSE;
            ecore_timer_del(sd->timer);
            sd->timer = NULL;
        }
        _switch_images(sd, o);
        return 1;
    }
    else
    {
        sd->playlist_id++;
        return 0;
    }
    return 0;
}

EAPI void enna_slideshow_play(void *data)
{
    Evas_Object *o;
    Evas_Object * obj = (Evas_Object *) data;

    API_ENTRY
    return;

    if (!sd->timer)
    {
        /* Play */
        sd->state = PLAY;
        o = eina_list_nth(sd->playlist, sd->playlist_id);
        _switch_images(sd, o);
        sd->timer = ecore_timer_add(4, enna_slideshow_next, sd->obj);
    }
    else
    {

        /* Pause */
        sd->state = PAUSE;
        ecore_timer_del(sd->timer);
        sd->timer = NULL;
    }

}

/* local subsystem globals */

static void _random_transition(E_Smart_Data *sd)
{
    unsigned int n;

    if (!sd)
        return;

    n = 1 + (int) ( 3.0 * rand() / (RAND_MAX + 1.0 ));
    if (sd->o_transition)
        evas_object_del(sd->o_transition);
    sd->o_transition = edje_object_add(evas_object_evas_get(sd->obj));
    enna_log(ENNA_MSG_EVENT, NULL, "Transition n°%d", n);
    switch (n)
    {
        case 1:
            edje_object_file_set(sd->o_transition, enna_config_theme_get(),
                    "transitions/crossfade");
            break;
        case 2:
            edje_object_file_set(sd->o_transition, enna_config_theme_get(),
                    "transitions/vswipe");
            break;
        case 3:
            edje_object_file_set(sd->o_transition, enna_config_theme_get(),
                    "transitions/hslide");
            break;
        default:
            break;
    }
    edje_object_part_swallow(sd->o_edje, "enna.swallow.transition",
            sd->o_transition);
    edje_object_signal_callback_add(sd->o_transition, "*", "*", _edje_cb, sd);
}

static void _edje_cb(void *data, Evas_Object *obj, const char *emission,
        const char *source)
{

    E_Smart_Data *sd = (E_Smart_Data*)data;

    if (!strcmp(emission, "done"))
    {
        edje_object_part_unswallow(sd->o_transition, sd->old_slide);
        evas_object_hide(sd->old_slide);
        sd->old_slide = NULL;
    }
}

static void _switch_images(E_Smart_Data * sd, Evas_Object * new_slide)
{

    if (!sd || !new_slide || !sd->o_transition)
        return;

    _random_transition(sd);

    edje_object_part_unswallow(sd->o_transition, sd->slide);
    edje_object_part_unswallow(sd->o_transition, sd->old_slide);
    sd->old_slide = sd->slide;
    sd->slide = new_slide;
    edje_object_signal_emit(sd->o_transition, "reset", "enna");
    edje_object_part_swallow(sd->o_transition, "slide.1", sd->old_slide);
    edje_object_part_swallow(sd->o_transition, "slide.2", sd->slide);
    edje_object_signal_emit(sd->o_transition, "show,2", "enna");
}

static void _enna_slideshow_smart_reconfigure(E_Smart_Data * sd)
{
    Evas_Coord x, y, w, h;

    x = sd->x;
    y = sd->y;
    w = sd->w;
    h = sd->h;

    evas_object_move(sd->o_edje, x, y);
    evas_object_resize(sd->o_edje, w, h);

}

static void _enna_slideshow_smart_init(void)
{
    if (_e_smart)
        return;
    static const Evas_Smart_Class sc =
    { SMART_NAME, EVAS_SMART_CLASS_VERSION, _e_smart_add, _e_smart_del,
            _e_smart_move, _e_smart_resize, _e_smart_show, _e_smart_hide,
            _e_smart_color_set, _e_smart_clip_set, _e_smart_clip_unset, NULL,
            NULL };
    _e_smart = evas_smart_class_new(&sc);
}

static void _e_smart_add(Evas_Object * obj)
{
    E_Smart_Data *sd;

    sd = calloc(1, sizeof(E_Smart_Data));
    if (!sd)
        return;

    srand(time(NULL));

    sd->o_edje = edje_object_add(evas_object_evas_get(obj));
    edje_object_file_set(sd->o_edje, enna_config_theme_get(), "transitions");
    sd->x = 0;
    sd->y = 0;
    sd->w = 0;

    sd->obj = obj;

    sd->h = 0;
    evas_object_smart_member_add(sd->o_edje, obj);
    evas_object_smart_data_set(obj, sd);

    sd->playlist = NULL;
    sd->playlist_id = 0;
    sd->timer = NULL;
    sd->old_slide = NULL;
    sd->slide = NULL;
    sd->state = STOP;
    sd->o_transition = NULL;
    _random_transition(sd);
}

static void _e_smart_del(Evas_Object * obj)
{
    E_Smart_Data *sd;
    Eina_List *l;

    sd = evas_object_smart_data_get(obj);
    if (!sd)
        return;

    for (l = sd->playlist; l; l = l->next)
        evas_object_del(l->data);

    evas_object_del(sd->o_edje);
    ecore_timer_del(sd->timer);
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
    _enna_slideshow_smart_reconfigure(sd);
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
    _enna_slideshow_smart_reconfigure(sd);
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
