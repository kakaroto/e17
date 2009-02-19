/*
 * wall.c
 * Copyright (C) Nicolas Aguirre 2006,2007,2008 <aguirre.nicolas@gmail.com>
 *
 * wall.c is free software copyrighted by Nicolas Aguirre.
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
 * wall.c IS PROVIDED BY Nicolas Aguirre ``AS IS'' AND ANY EXPRESS
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

#define SMART_NAME "enna_carousel"

typedef struct _Smart_Data Smart_Data;
typedef struct _Carousel_Item Carousel_Item;

struct _Carousel_Item
{
    Evas_Object *o_edje;
    int row;
    Evas_Object *o_child;
    const char *label;
    Smart_Data *sd;
    unsigned char selected : 1;
};

struct _Smart_Data
{
    Evas_Coord x, y, w, h;
    Evas_Object *obj;
    Evas_Object *o_scroll;
    Evas_Object *o_table;
    Eina_List *pictures;
    Evas_Object *o_cont;
    Evas_Object *o_box;
    Eina_List *items;
    int nb;
};

struct _Preload_Data
{
    Smart_Data *sd;
    Evas_Object *item;
};

/* local subsystem functions */
static void _carousel_left_select(Evas_Object *obj);
static void _carousel_right_select(Evas_Object *obj);
static Carousel_Item *_smart_selected_item_get(Smart_Data *sd, int *col);
static void _smart_item_unselect(Smart_Data *sd, Carousel_Item *pi);
static void _smart_item_select(Smart_Data *sd, Carousel_Item *pi);
static void _smart_event_mouse_down(void *data, Evas *evas, Evas_Object *obj,
        void *event_info);
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
Evas_Object * enna_carousel_add(Evas * evas)
{
    _smart_init();
    return evas_object_smart_add(evas, _smart);
}

void enna_carousel_object_append(Evas_Object *obj, Evas_Object *child, const char *label)
{

    Evas_Coord ow, oh;
    Evas_Object *o_edje;
    Carousel_Item *pi;
    Evas_Aspect_Control apref;
    Evas_Coord aw, ah;
    float ratio;

    API_ENTRY return;

    pi = calloc(1, sizeof(Carousel_Item));

    sd->nb++;

    o_edje = edje_object_add(evas_object_evas_get(sd->o_scroll));
    edje_object_file_set(o_edje, enna_config_theme_get(), "enna/mainmenu/item");
    edje_object_part_text_set(o_edje, "enna.text.label", label);
    pi->label = eina_stringshare_add(label);

    evas_object_show(o_edje);

    pi->o_child = child;
    pi->o_edje = o_edje;
    pi->sd = sd;
    pi->selected = 0;

    sd->items = eina_list_append(sd->items, pi);
    evas_object_size_hint_align_set(o_edje, 0.5, 0.5);
    evas_object_size_hint_aspect_get(child, &apref, &aw, &ah);

    if (ah) ratio = (float)aw /(float)ah;
    else ratio = 1.0;

    oh = sd->h;
    ow = oh * ratio;
    edje_extern_object_min_size_set(o_edje, ow, oh);
    edje_object_part_swallow(o_edje, "enna.swallow.content", child);
    evas_object_event_callback_add(o_edje, EVAS_CALLBACK_MOUSE_DOWN,
            _smart_event_mouse_down, pi);
    enna_box_pack_end(sd->o_box, o_edje);
    evas_object_size_hint_min_set(o_edje, ow, oh);
}

void enna_carousel_event_feed(Evas_Object *obj, void *event_info)
{
    Evas_Event_Key_Down *ev = event_info;
    enna_key_t key = enna_get_key(ev);

    API_ENTRY return;

     enna_log(ENNA_MSG_EVENT, SMART_NAME, "Key pressed : %s\n", ev->key);
    switch (key)
    {
    case ENNA_KEY_LEFT:
	_carousel_left_select(obj);
	break;
    case ENNA_KEY_RIGHT:
	_carousel_right_select(obj);
	break;
    default:
	break;
    }


}

void enna_carousel_select_nth(Evas_Object *obj, int n)
{
    Carousel_Item *pi;

    API_ENTRY;

    pi = eina_list_nth(sd->items, n);
    if (!pi) return;

    _smart_item_unselect(sd, _smart_selected_item_get(sd, NULL));
    _smart_item_select(sd, pi);


}

void enna_carousel_activate_nth(Evas_Object *obj, int n)
{
    Carousel_Item *pi;

    API_ENTRY;

    pi = eina_list_nth(sd->items, n);
    if (!pi) return;

    evas_object_smart_callback_call (pi->sd->obj, "activate", (void*)pi->label);

}


void enna_carousel_selected_geometry_get(Evas_Object *obj, int *x, int *y, int *w, int *h)
{
    Eina_List *l;
    API_ENTRY;

    for (l = sd->items; l; l = l->next)
    {
        Carousel_Item *pi = l->data;
	if (pi->selected)
	{
	    evas_object_geometry_get(pi->o_child, x, y, w, h);
	    return;
	}
    }
}

Evas_Object * enna_carousel_selected_object_get(Evas_Object *obj)
{
    API_ENTRY;
    Eina_List *l;
    for (l = sd->items; l; l = l->next)
    {
        Carousel_Item *pi = l->data;
	if (pi->selected)
	{
	    return pi->o_child;
	}
    }
    return NULL;
}

int enna_carousel_selected_get(Evas_Object *obj)
{
    API_ENTRY;
    Eina_List *l;
    int i = 0;
    for (l = sd->items, i = 0; l; l = l->next, i++)
    {
        Carousel_Item *pi = l->data;
	if (pi->selected)
	{
	    return i;
	}
    }
    return -1;
}

/* local subsystem globals */


///////////////////// LEFT /////////////////////
static void _carousel_left_select(Evas_Object *obj)
{
    Carousel_Item *pi, *ppi;
    int col;

    API_ENTRY return;

    ppi = _smart_selected_item_get(sd, &col);

    if (!ppi)
    {
        col = 0;
    }
    else
    {
        col--;

    }
    pi = eina_list_nth(sd->items, col);
    if (pi)
    {
	Evas_Coord x, y;
	Evas_Coord xedje, yedje, wedje, xbox;

	enna_scrollframe_child_pos_get(sd->o_scroll, NULL, &y);
	evas_object_geometry_get(pi->o_edje, &xedje, &yedje, &wedje, NULL);
	evas_object_geometry_get(sd->o_box, &xbox, NULL, NULL, NULL);
	x = (xedje + wedje / 2 - xbox - sd->w / 2 );
	enna_scrollframe_child_pos_set(sd->o_scroll, x , y);
	_smart_item_select(sd, pi);
	if (ppi) _smart_item_unselect(sd, ppi);
    }

}


///////////////////// RIGHT /////////////////////
static void _carousel_right_select(Evas_Object *obj)
{
    Carousel_Item *pi, *ppi;
    int col;

    API_ENTRY return;

    ppi = _smart_selected_item_get(sd, &col);
    if (!ppi)
    {
        col = 0;
    }
    else
    {
        col++;
    }
    pi = eina_list_nth(sd->items, col);
    if (pi)
    {
	Evas_Coord x, y;
	Evas_Coord xedje, yedje, wedje, xbox;

	enna_scrollframe_child_pos_get(sd->o_scroll, NULL, &y);
	evas_object_geometry_get(pi->o_edje, &xedje, &yedje, &wedje, NULL);
	evas_object_geometry_get(sd->o_box, &xbox, NULL, NULL, NULL);
	x = (xedje + wedje / 2 - xbox - sd->w / 2 );
	enna_scrollframe_child_pos_set(sd->o_scroll, x , y);
	_smart_item_select(sd, pi);
	if (ppi) _smart_item_unselect(sd, ppi);
    }

}

static Carousel_Item *_smart_selected_item_get(Smart_Data *sd, int *col)
{
    Eina_List *l;
    int j;


    for (l = sd->items, j = 0; l; l = l->next, j++)
    {
	Carousel_Item *pi = l->data;
	if (pi->selected)
	{
	    if (col) *col = j;
	    return pi;
	}
    }

    return NULL;
}

static void _smart_item_unselect(Smart_Data *sd, Carousel_Item *pi)
{
    if (!pi || !sd)
        return;
    pi->selected = 0;
    edje_object_signal_emit(pi->o_edje, "unselect", "enna");

}

static void _smart_item_select(Smart_Data *sd, Carousel_Item *pi)
{
    if (!pi || !sd)
        return;

    pi->selected = 1;
    evas_object_raise(pi->o_edje);
    edje_object_signal_emit(pi->o_edje, "select", "enna");
}

static void _smart_event_mouse_down(void *data, Evas *evas, Evas_Object *obj,
        void *event_info)
{
    Carousel_Item *pi, *ppi;

    pi = data;

    ppi = _smart_selected_item_get(pi->sd, NULL);
    if (ppi && ppi != pi)
    {
        ppi->selected = 0;
        edje_object_signal_emit(ppi->o_edje, "unselect", "enna");
    }

    evas_object_raise(pi->o_edje);
    pi->selected = 1;
    edje_object_signal_emit(pi->o_edje, "select", "enna");
    evas_object_smart_callback_call (pi->sd->obj, "activate", (void*)pi->label);
}

static void _smart_reconfigure(Smart_Data * sd)
{
    Evas_Coord w, h;
    Evas_Coord ow, oh;
    Carousel_Item *it;
    Eina_List *l;
    Evas_Aspect_Control apref;
    Evas_Coord aw, ah;
    float ratio;

    EINA_LIST_FOREACH(sd->items, l, it)
    {
	evas_object_size_hint_aspect_get(it->o_child, &apref, &aw, &ah);

	if (ah) ratio = (float)aw /(float)ah;
	else ratio = 1.0;
	oh = sd->h;
	ow = oh * ratio;
	edje_extern_object_min_size_set(it->o_edje, ow, oh);
	evas_object_size_hint_min_set(it->o_edje, ow, oh);
    }



    evas_object_size_hint_min_get(sd->o_box, &w, &h);

    if (sd->w >= w) ow = sd->w;
    else ow = w;

    if (sd->h >= h) oh = sd->h;
    else oh = h;

    evas_object_resize(sd->o_box, ow, oh);
    evas_object_size_hint_min_set(sd->o_box, ow, oh);

    evas_object_move(sd->o_scroll, sd->x, sd->y);
    evas_object_resize(sd->o_scroll, sd->w, sd->h);
}

static void _smart_init(void)
{
    if (_smart)
        return;
    static const Evas_Smart_Class sc =
    { SMART_NAME, EVAS_SMART_CLASS_VERSION, _smart_add, _smart_del,
            _smart_move, _smart_resize, _smart_show, _smart_hide,
                    _smart_color_set, _smart_clip_set, _smart_clip_unset, NULL,
                    NULL };
    _smart = evas_smart_class_new(&sc);
}

static void _smart_add(Evas_Object * obj)
{
    Smart_Data *sd;

    sd = calloc(1, sizeof(Smart_Data));
    if (!sd)
        return;
    sd->obj = obj;
    sd->nb = -1;
    sd->o_scroll = enna_scrollframe_add(evas_object_evas_get(obj));
    evas_object_move(sd->o_scroll, 0, 0);
    evas_object_show(sd->o_scroll);
    enna_scrollframe_policy_set(sd->o_scroll, ENNA_SCROLLFRAME_POLICY_OFF,
            ENNA_SCROLLFRAME_POLICY_OFF);

    sd->o_box = enna_box_add(evas_object_evas_get(obj));
    enna_box_homogenous_set(sd->o_box, 1);
    enna_box_orientation_set(sd->o_box, 1);
    enna_box_pack_end(sd->o_cont, sd->o_box);
    evas_object_show(sd->o_box);
    enna_scrollframe_child_set(sd->o_scroll, sd->o_box);

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
    evas_object_del(sd->o_cont);

    evas_object_del(sd->o_box);
    while (sd->items)
    {
	Carousel_Item *pi = sd->items->data;
	sd->items = eina_list_remove_list(sd->items, sd->items);
	evas_object_del(pi->o_edje);
	evas_object_del(pi->o_child);
	eina_stringshare_del(pi->label);
	free(pi);
    }

    evas_object_del(sd->o_scroll);
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

