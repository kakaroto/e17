/*
 * button.c
 * Copyright (C) Nicolas Aguirre 2008 <aguirre.nicolas@gmail.com>
 *
 * button.c is free software copyrighted by Nicolas Aguirre.
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
 * button.c IS PROVIDED BY Nicolas Aguirre ``AS IS'' AND ANY EXPRESS
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

#include "enna.h"


#define SMART_NAME "enna_button"

#define API_ENTRY \
  Smart_Data *sd; \
  sd = evas_object_smart_data_get(obj); \
  if ((!obj) || (!sd) || (evas_object_type_get(obj) \
    && strcmp(evas_object_type_get(obj), SMART_NAME)))

typedef struct _Smart_Data Smart_Data;

struct _Smart_Data
{
    Evas_Coord x, y, w, h;
    Evas_Object *o_edje;
    Evas_Object *o_icon;
    char *label;
    Evas_Hash *funcs;       /* Callback functions hash */
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

/* local subsystem globals */
static Evas_Smart *_smart = NULL;

/* externally accessible functions */
EAPI Evas_Object *
enna_button_add(Evas * evas)
{
    
    _smart_init();
    return evas_object_smart_add(evas, _smart);
}

/* Add Callback function */
EAPI void
enna_button_cb_add(Evas_Object *obj, void (*func_cb) (void *data), const char *event, void *data)
{
    //Button_Cb_Func *f;
    API_ENTRY;
    if (!func_cb || !event) return;
    
    /*f = malloc(sizeof(Button_Cb_Func));
    f->func = func;
    f->data = data;
    evas_hash_append(sd->funcs, event, f);*/       
}

EAPI void
enna_button_label_set(Evas_Object *obj, const char *label)
{
    API_ENTRY;
    
    if (!label) return; 
    
    ENNA_FREE(sd->label);
    sd->label = strdup(label);
    edje_object_part_text_set(sd->o_edje, "enna.text.label", label);
}

EAPI void
enna_button_icon_set(Evas_Object *obj, const char *icon)
{
    API_ENTRY;
    if (!icon) return;
    /* Check if icon is an image */
    if (icon[0] == '/' && ecore_file_exists(icon))
    {
        sd->o_icon = enna_image_add(evas_object_evas_get(sd->o_edje));
        enna_image_file_set(sd->o_icon, icon);
    }
    /* Not a filename, get edje object */
    else
    {
        sd->o_icon = edje_object_add(evas_object_evas_get(sd->o_edje));
        edje_object_file_set(sd->o_icon, enna_config_theme_get(), icon);
    }
    edje_object_part_swallow(sd->o_edje, "enna.swallow.icon", sd->o_icon);
}

/* local subsystem globals */
static void _smart_reconfigure(Smart_Data * sd)
{
    Evas_Coord x, y, w, h;

    x = sd->x;
    y = sd->y;
    w = sd->w;
    h = sd->h;

    evas_object_move(sd->o_edje, x, y);
    evas_object_resize(sd->o_edje, w, h);

}

static void _smart_init(void)
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

static void _smart_add(Evas_Object * obj)
{
    Smart_Data *sd;
    
    sd = calloc(1, sizeof(Smart_Data));
    if (!sd)
        return;
    sd->o_edje = edje_object_add(evas_object_evas_get(obj));
    edje_object_file_set(sd->o_edje, enna_config_theme_get(), "enna/button");
    evas_object_show(sd->o_edje);
    evas_object_smart_member_add(sd->o_edje, obj);
    evas_object_smart_data_set(obj, sd);
}

static void _smart_del(Evas_Object * obj)
{
    Smart_Data *sd;

    sd = evas_object_smart_data_get(obj);
    if (!sd)
        return;
    ENNA_OBJECT_DEL(sd->o_icon);
    ENNA_FREE(sd->label);
    evas_object_del(sd->o_edje);
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
    _smart_reconfigure(sd);
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
    _smart_reconfigure(sd);
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
