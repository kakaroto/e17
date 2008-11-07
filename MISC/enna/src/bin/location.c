/*
 * enna_location.c
 * Copyright (C) Nicolas Aguirre 2006,2007,2008 <aguirre.nicolas@gmail.com>
 *
 * enna_location.c is free software copyrighted by Nicolas Aguirre.
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
 * enna_location.c IS PROVIDED BY Nicolas Aguirre ``AS IS'' AND ANY EXPRESS
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
#include "location.h"

#define SMART_NAME "enna_location"

typedef struct _Smart_Data Smart_Data;
struct _Smart_Data
{
    Evas_Coord x, y, w, h;
    Evas_Object *smart_obj;
    Evas_Object *o_box;
    Evas_Object *o_scroll;
    Eina_List *items;
    unsigned char on_hold : 1;
};

typedef struct _Enna_Location_Item Enna_Location_Item;

struct _Enna_Location_Item
{
    Smart_Data *sd;
    Evas_Object *o_base;
    Evas_Object *o_icon;
    unsigned char selected : 1;
    void (*func)(void *data, void *data2);
    void *data;
    void *data2;
};

/* local subsystem functions */
static void _enna_location_smart_reconfigure(Smart_Data * sd);
static void _enna_location_smart_init(void);
static void _smart_add(Evas_Object * obj);
static void _smart_del(Evas_Object * obj);
static void _smart_move(Evas_Object * obj, Evas_Coord x, Evas_Coord y);
static void _smart_resize(Evas_Object * obj, Evas_Coord w, Evas_Coord h);
static void _smart_show(Evas_Object * obj);
static void _smart_hide(Evas_Object * obj);
static void _smart_color_set(Evas_Object * obj, int r, int g, int b, int a);
static void _smart_clip_set(Evas_Object * obj, Evas_Object * clip);
static void _smart_clip_unset(Evas_Object * obj);
static void _smart_event_mouse_up(void *data, Evas *evas, Evas_Object *obj,
        void *event_info);
static void _smart_event_mouse_down(void *data, Evas *evas, Evas_Object *obj,
        void *event_info);
/* local subsystem globals */
static Evas_Smart *_e_smart = NULL;

/* externally accessible functions */
Evas_Object *
enna_location_add(Evas * evas)
{
    _enna_location_smart_init();
    return evas_object_smart_add(evas, _e_smart);
}

void enna_location_append(Evas_Object *obj, const char *label,
        Evas_Object *icon, void (*func) (void *data, void *data2), void *data, void *data2)
{
    Enna_Location_Item *si;
    Evas_Coord mw = 0, mh = 0;
    Evas_Coord x, y, w, h;

    API_ENTRY
    return;
    si = ENNA_NEW(Enna_Location_Item, 1);
    si->sd = sd;
    si->o_base = edje_object_add(evas_object_evas_get(sd->o_scroll));

    if (eina_list_count(sd->items) & 0x1)
        edje_object_file_set(si->o_base, enna_config_theme_get(),
                "enna/location/item");
    else
        edje_object_file_set(si->o_base, enna_config_theme_get(),
                "enna/location/item");
    if (label)
        edje_object_part_text_set(si->o_base, "enna.text.label", label);

    si->o_icon = icon;
    if (si->o_icon)
    {
        edje_extern_object_min_size_set(si->o_icon, 32, 32);
        edje_object_part_swallow(si->o_base, "enna.swallow.icon", si->o_icon);
        evas_object_show(si->o_icon);
        edje_object_signal_emit(si->o_base, "icon,show", "enna");
    }

    si->func = func;
    si->data = data;
    si->data2 = data2;
    sd->items = eina_list_append(sd->items, si);

    edje_object_size_min_calc(si->o_base, &mw, &mh);

    enna_box_pack_end(sd->o_box, si->o_base);
    evas_object_size_hint_min_set(si->o_base, mw, sd->h);
    evas_object_size_hint_align_set(si->o_base, 0, 0.5);
    evas_object_size_hint_weight_set(si->o_base, 1.0, 1.0);

    mh = sd->h ? sd->h : 64;

    evas_object_size_hint_min_get(sd->o_box, &mw, NULL);
    evas_object_resize(sd->o_box, mw, sd->h);

    evas_object_lower(si->o_base);
    edje_object_signal_emit(si->o_base, "location,show", "enna");
    evas_object_event_callback_add(si->o_base, EVAS_CALLBACK_MOUSE_DOWN,
            _smart_event_mouse_down, si);
    evas_object_event_callback_add(si->o_base, EVAS_CALLBACK_MOUSE_UP,
            _smart_event_mouse_up, si);

    evas_object_geometry_get(si->o_base, &x, &y, &w, &h);
    enna_scrollframe_child_region_show(sd->o_scroll, x, y, w, h);

    evas_object_show(si->o_base);
}

static void _location_hide_end(void *data, Evas_Object *o, const char *sig,
        const char *src)
{
    Smart_Data *sd;
    Enna_Location_Item *si;
    Evas_Coord x, y, w, h;
    si = data;

    if (!si) return;

    sd = si->sd;
    sd->items = eina_list_remove(sd->items, si);
    enna_box_unpack(si->o_base);
    evas_object_geometry_get(si->o_base, &x, &y, &w, &h);
     enna_scrollframe_child_region_show(sd->o_scroll, x, y, w, h);
     edje_object_signal_callback_del(si->o_base, "location,hide,end", "edje",
       _location_hide_end);
     evas_object_del(si->o_icon);
     evas_object_del(si->o_base);
     free(si);
     _enna_location_smart_reconfigure(sd);
}

void enna_location_remove_nth(Evas_Object *obj, int n)
{
    Enna_Location_Item *si = NULL;

    API_ENTRY return;
    if (!sd->items)
        return;
    if (!(si = eina_list_nth(sd->items, n)))
        return;

    edje_object_signal_emit(si->o_base, "location,hide", "enna");
    edje_object_signal_callback_add(si->o_base, "location,hide,end", "edje",
            _location_hide_end, si);
}

const char * enna_location_label_get_nth(Evas_Object *obj, int n)
{
    Enna_Location_Item *si = NULL;

    API_ENTRY
    return NULL;
    if (!sd->items)
        return NULL;
    if (!(si = eina_list_nth(sd->items, n)))
        return NULL;
    return edje_object_part_text_get(si->o_base, "enna.text.label");
}

int enna_location_count(Evas_Object *obj)
{
    API_ENTRY
    return 0;
    return eina_list_count(sd->items);
}

/* local subsystem globals */
static void _enna_location_smart_reconfigure(Smart_Data * sd)
{
    Evas_Coord mw, w = 0;
    Evas_List *l;


    for (l = sd->items; l; l = l->next)
    {
	Enna_Location_Item *si = l->data;
	evas_object_size_hint_min_get(si->o_base, &mw, NULL);
	w+= mw;
    }

    evas_object_resize(sd->o_box, w, sd->h);

    evas_object_move(sd->o_scroll, sd->x, sd->y);
    evas_object_resize(sd->o_scroll, sd->w, sd->h);
}

static void _enna_location_smart_init(void)
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
    Smart_Data *sd;

    sd = calloc(1, sizeof(Smart_Data));
    if (!sd)
        return;

    sd->o_box = enna_box_add(evas_object_evas_get(obj));
    enna_box_homogenous_set(sd->o_box, 0);
    enna_box_orientation_set(sd->o_box, 1);
    evas_object_size_hint_align_set(sd->o_box, 0, 0.5);
    evas_object_size_hint_weight_set(sd->o_box, 1.0, 0.0);

    sd->x = 0;
    sd->y = 0;
    sd->w = 0;
    sd->h = 0;
    sd->items = NULL;
    sd->o_scroll = enna_scrollframe_add(evas_object_evas_get(obj));
    enna_scrollframe_policy_set(sd->o_scroll, ENNA_SCROLLFRAME_POLICY_OFF,
            ENNA_SCROLLFRAME_POLICY_OFF);
    enna_scrollframe_child_set(sd->o_scroll, sd->o_box);
    sd->smart_obj = obj;
    evas_object_smart_member_add(sd->o_scroll, obj);
    evas_object_smart_data_set(obj, sd);
}

static void _smart_del(Evas_Object * obj)
{
    Smart_Data *sd;

    sd = evas_object_smart_data_get(obj);
    if (!sd)
        return;

    evas_object_del(sd->o_scroll);
    evas_object_del(sd->o_box);

    while (sd->items)
    {
	Enna_Location_Item *si = sd->items->data;
	sd->items = eina_list_remove_list(sd->items, sd->items);
	evas_object_del(si->o_base);
	evas_object_del(si->o_icon);
	free(si);
    }
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
    _enna_location_smart_reconfigure(sd);
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
    _enna_location_smart_reconfigure(sd);
}

static void _smart_show(Evas_Object * obj)
{
    Smart_Data *sd;

    sd = evas_object_smart_data_get(obj);
    if (!sd)
        return;
    evas_object_show(sd->o_scroll);
}

static void _smart_hide(Evas_Object * obj)
{
    Smart_Data *sd;

    sd = evas_object_smart_data_get(obj);
    if (!sd)
        return;
    evas_object_hide(sd->o_scroll);
}

static void _smart_color_set(Evas_Object * obj, int r, int g, int b, int a)
{
    Smart_Data *sd;

    sd = evas_object_smart_data_get(obj);
    if (!sd)
        return;
    evas_object_color_set(sd->o_scroll, r, g, b, a);
}

static void _smart_clip_set(Evas_Object * obj, Evas_Object * clip)
{
    Smart_Data *sd;

    sd = evas_object_smart_data_get(obj);
    if (!sd)
        return;
    evas_object_clip_set(sd->o_scroll, clip);
}

static void _smart_clip_unset(Evas_Object * obj)
{
    Smart_Data *sd;

    sd = evas_object_smart_data_get(obj);
    if (!sd)
        return;
    evas_object_clip_unset(sd->o_scroll);
}

static void _smart_event_mouse_down(void *data, Evas *evas, Evas_Object *obj,
        void *event_info)
{
    Smart_Data *sd;
    Evas_Event_Mouse_Down *ev;
    Enna_Location_Item *si;

    ev = event_info;
    si = data;
    sd = si->sd;

    if (ev->event_flags & EVAS_EVENT_FLAG_ON_HOLD)
        sd->on_hold = 1;
    else
        sd->on_hold = 0;

    if (!sd->items)
        return;

}

static void _smart_event_mouse_up(void *data, Evas *evas, Evas_Object *obj,
        void *event_info)
{
    Smart_Data *sd;
    Evas_Event_Mouse_Up *ev;
    Enna_Location_Item *si;

    ev = event_info;
    si = data;
    sd = si->sd;

    if (ev->event_flags & EVAS_EVENT_FLAG_ON_HOLD)
        sd->on_hold = 1;
    else
        sd->on_hold = 0;

    if (!sd->items)
        return;
    //if (!sd->selector) return;
    //if (!(si = eina_list_nth(sd->items, sd->selected))) return;
    if (sd->on_hold)
    {
        sd->on_hold = 0;
        return;
    }

    if (si->func)
        si->func(si->data, si->data2);

}
